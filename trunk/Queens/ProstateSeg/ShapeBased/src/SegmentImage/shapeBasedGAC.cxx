#include "shapeBasedGAC.h"

// itk
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"

#include "itkRescaleIntensityImageFilter.h"

// itk vnl
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"

#include <vnl/vnl_random.h>

//douher
#include "lib/cArrayOp.h"
#include "lib/imageProcessing.h"


/* basicInit    */
void CShapeBasedGAC::basicInit()
{
  SuperClassType::basicInit();
    
  m_curvatureWeight = 0.5;
  m_inflation = 1.0; // neg: shrink, pos: dilation


  //  m_numEigenShapes = 0;

  m_volume = 0;
  m_volumeOld = 0;

  m_centroid.set_size(3);
  m_centroid.fill(0);


  m_startingCenter.set_size(3);
  m_startingCenter[0] = 128.2;
  m_startingCenter[1] = 129.6;
  m_startingCenter[2] = 10.82;


  // The distance between the left and right point in the middle slice of the mean shape
  m_LRDistanceInMiddleSliceOfMeanShape = 83;
}


/* ============================================================
   computeFeatureImage    */
void CShapeBasedGAC::computeFeatureImage()
{
  if (!mp_img)
    {
      std::cerr<<"set image first.\n";
      raise(SIGABRT);
    }

  typedef itk::Image< double, 3 > ItkImageType;

  ItkImageType::Pointer imageItk = douher::cArray3ToItkImage< double >(mp_img);
  
  /* smooth image */
  typedef itk::CurvatureAnisotropicDiffusionImageFilter< ItkImageType, ItkImageType >  SmoothingFilterType;
  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput( imageItk );
  smoothing->SetTimeStep( 0.05 );
  smoothing->SetNumberOfIterations(  10 );
  smoothing->SetConductanceParameter( 9.0 );

  /* get gradient magnitude map */
  typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< ItkImageType, ItkImageType >  GradientFilterType;
  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  gradientMagnitude->SetSigma( 1.0 );

  /* sigmoid transform */
  typedef   itk::SigmoidImageFilter< ItkImageType, ItkImageType >  SigmoidFilterType;
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );

  double alpha =  -1;
  double beta  =  20;

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );

  sigmoid->Update();

  
  m_featureImage = douher::itkImageToArray3< double >( sigmoid->GetOutput() );

//   //debug//
//   douher::saveAsImage3< double >(m_featureImage, "feature.nrrd");
//   //DEBUG//

  return;
}

/* ============================================================
   setFeatureImage    */
void CShapeBasedGAC::setFeatureImage(douher::cArray3D< double >::Pointer featureImg)
{
  if (!this->mp_img)
    {
      std::cerr<<"set image first.\n";
      raise(SIGABRT);
    }

  if (featureImg->getSizeX() != m_nx || featureImg->getSizeY() != m_ny || featureImg->getSizeZ() != m_nz )
    {
      std::cerr<<"sizes don't match.\n";
      raise(SIGABRT);
    }

  m_featureImage = featureImg;
}


/* ============================================================
   computeForce    */
void CShapeBasedGAC::computeForce()
{
  this->m_force.clear();

  updateVolume();
  updateCentroid();
  //  computeCentroid();
  
  //debug//
  //  std::cout<<m_centroid<<std::endl;
  //DEBUG//

  double fmax = -1e10;

  long n = this->m_lz->size();
  double* theForce = new double[ n ];

  {
    long i = 0;
    for (douher::CSFLSLayer::iterator itz = this->m_lz->begin(); itz != this->m_lz->end(); ++itz, ++i)
      {
  long ix = (*itz)[0];
  long iy = (*itz)[1];
  long iz = (*itz)[2];

  double f = computeForceAt(ix, iy, iz);
        
  fmax = fabs(f)>fmax?fabs(f):fmax;

  theForce[i] = f;
      }
  }


  for (long i = 0; i < n; ++i)
    {
      this->m_force.push_back(theForce[i]/(fmax + 1e-10));
    }
    
  delete[] theForce;
}


/* ============================================================
   doShapeBasedGACSegmenation    */
void CShapeBasedGAC::doShapeBasedGACSegmenation()
{
  checkIfReadyToGo();

  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */
  this->initializeSFLS();
  computeVolume();
  computeCentroid();


  for (unsigned int it = 0; it < this->m_numIter; ++it)
    {
      computeForce();

      this->normalizeForce();

      this->oneStepLevelSetEvolution();
    }
}

/*============================================================

computeUpwindGradAt 

Compute upwind gradient of the level set fn at a point */
double CShapeBasedGAC::computeForceAt(long ix, long iy, long iz)
{
  /*----------------------------------------------------------------------
    According to eq(8) in Yezzi GAC TMI paper. */

  double f = 0;
    
  double dPhiDxMinus = 0.0;
  double dPhiDxPlus = 0.0;
  double dPhiDyMinus = 0.0;
  double dPhiDyPlus = 0.0;
  double dPhiDzMinus = 0.0;
  double dPhiDzPlus = 0.0;

  double dFeatureDx = 0.0;
  double dFeatureDy = 0.0;
  double dFeatureDz = 0.0;

  double featureVal = m_featureImage->get(ix, iy, iz);


  if( ix+1 < this->m_nx && ix-1 >=0 )
    {
      dPhiDxMinus = this->mp_phi->get(ix, iy, iz) - this->mp_phi->get(ix-1, iy, iz);
      dPhiDxPlus = this->mp_phi->get(ix+1, iy, iz) - this->mp_phi->get(ix, iy, iz);

      dFeatureDx = (this->m_featureImage->get(ix+1, iy, iz) - this->m_featureImage->get(ix-1, iy, iz))/2.0;
    }

  if( iy+1 < this->m_ny && iy-1 >=0 ) 
    {
      dPhiDyMinus = this->mp_phi->get(ix, iy, iz) - this->mp_phi->get(ix, iy-1, iz);
      dPhiDyPlus = this->mp_phi->get(ix, iy+1, iz) - this->mp_phi->get(ix, iy, iz);

      dFeatureDy = (this->m_featureImage->get(ix, iy+1, iz) - this->m_featureImage->get(ix, iy-1, iz))/2.0;
    }

  if( iz+1 < this->m_nz && iz-1 >=0 )
    {
      dPhiDzMinus = this->mp_phi->get(ix, iy, iz) - this->mp_phi->get(ix, iy, iz-1);
      dPhiDzPlus = this->mp_phi->get(ix, iy, iz+1) - this->mp_phi->get(ix, iy, iz);

      dFeatureDz = (this->m_featureImage->get(ix, iy, iz+1) - this->m_featureImage->get(ix, iy, iz-1))/2.0;
    }


  // \gradient(feature) dotProd \upwind_gradient(\phi)
  double dGdxdPhidx = dFeatureDx>0?(dFeatureDx*dPhiDxPlus):(dFeatureDx*dPhiDxMinus);
  double dGdydPhidy = dFeatureDy>0?(dFeatureDy*dPhiDyPlus):(dFeatureDy*dPhiDyMinus);
  double dGdzdPhidz = dFeatureDz>0?(dFeatureDz*dPhiDzPlus):(dFeatureDz*dPhiDzMinus);

  double gradFeatureDotGradPhi = dGdxdPhidx*dGdydPhidy*dGdzdPhidz;

  f += gradFeatureDotGradPhi;

  double k = this->computeKappa(ix, iy, iz);


  f += featureVal*(m_curvatureWeight*k - m_inflation);

  return f;
}



/*================================================================================*/
void
CShapeBasedGAC::computeVolume()
{
  m_volume = computeVolume(mp_phi);
  m_volumeOld = m_volume;

  return;
}
  
/*================================================================================*/
void
CShapeBasedGAC::updateVolume()
{
  m_volume -= this->m_lIn2out.size();
  m_volume += this->m_lOut2in.size();

  return;
}


/* ============================================================
   computeFeatureImage    */
void CShapeBasedGAC::checkIfReadyToGo()
{
  if (!mp_img)
    {
      std::cerr<<"mp_img not ready.\n";
      raise(SIGABRT);
    }

  if (!m_featureImage)
    {
      computeFeatureImage();
    }


  /*  Mandatary build the mask from the mean shape, not matter whether
      it's been set from outside. (The base class CSFLSSegmentor3D has the
      public setMask fn, I can not prevent outside to call it, but I do
      rebuild the mask.) */
  getMaskFromMeanShape();


  return;
}



/*================================================================================*/
void
CShapeBasedGAC::updateCentroid()
{
  // this should be run right after updateVolume()
  
  vnl_vector<double> sumCentroid(3, 0.0);

  sumCentroid = m_centroid*m_volumeOld;

  for (douher::CSFLSLayer::const_iterator it = this->m_lIn2out.begin(); it != this->m_lIn2out.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];

      vnl_vector<double> a(3, 0);
      a[0] = ix;
      a[1] = iy;
      a[2] = iz;

      sumCentroid -= a;
    }

  for (douher::CSFLSLayer::const_iterator it = this->m_lOut2in.begin(); it != this->m_lOut2in.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];
      
      vnl_vector<double> a(3, 0);
      a[0] = ix;
      a[1] = iy;
      a[2] = iz;

      sumCentroid += a;
    }


  m_volumeOld = m_volume;
  m_centroid = sumCentroid/m_volume;
}



/*================================================================================*/
void
CShapeBasedGAC::setMeanShape(douher::cArray3D< double >::Pointer meanShape)
{
  if (!mp_img)
    {
      std::cerr<<"set image first.\n";
      raise(SIGABRT);
    }
  else if (meanShape->getSizeX() != m_nx || meanShape->getSizeY() != m_ny || meanShape->getSizeZ() != m_nz)
    {
      // mean shape size is not input image size. resize mean shape size
      double fillValue = 3.0;
      douher::cArray3D< double >::Pointer meanShapeSameSize = correctSize(meanShape, fillValue);
      m_meanShape = meanShapeSameSize;
    }
  else
    {
      m_meanShape = meanShape;
    }

  // update the mean shape volume
  m_volumneOfMeanShape = computeVolume(m_meanShape);


  return;
}



// /*================================================================================*/
// void
// CShapeBasedGAC::setEigenShapes(std::vector< douher::cArray3D< double >::Pointer > eigenShapes )
// {
//   if (!m_meanShape)
//     {
//       std::cerr<<"set mean shape first.\n";
//       raise(SIGABRT);
//     }

//   m_eigenShapes.clear();

//   for (long i = 0; i < (long)eigenShapes.size(); ++i)
//     {
//       if (eigenShapes[i]->getSizeX() != m_nx || eigenShapes[i]->getSizeY() != m_ny || eigenShapes[i]->getSizeZ() != m_nz)
//   {
//     // eigen shape size is not input image size. resize eigen shape size
//     double fillValue = 0.0;
//     douher::cArray3D< double >::Pointer eigenShapeSameSize = correctSize(eigenShapes[i], fillValue);
//     m_eigenShapes.push_back(eigenShapeSameSize);
//   }
//       else
//   {
//     m_eigenShapes.push_back(eigenShapes[i]);

//   }
//     }

//   m_numEigenShapes = m_eigenShapes.size();


//   return;
// }


/*================================================================================*/
douher::cArray3D< double >::Pointer
CShapeBasedGAC::correctSize(douher::cArray3D< double >::Pointer img, double fillValue)
{
  if (!mp_img)
    {
      std::cerr<<"set image first.\n";
      raise(SIGABRT);
    }

  vnl_matrix_fixed< double, 3, 3 > A;
  A.set_identity();
  vnl_vector_fixed< double, 3 > translation(0.0);

  itk::Image< double, 3 >::Pointer imgItk = douher::cArray3ToItkImage< double >(img);

  unsigned char interpType = 1; // lnr interp

  itk::Image< double, 3 >::Pointer newImgItk = douher::affineTransform3< double >(imgItk, A, translation, fillValue, interpType, \
                      m_nx, m_ny, m_nz); // mp_img's size

  return douher::itkImageToArray3< double >(newImgItk);
}


/*================================================================================*/
double 
CShapeBasedGAC::computeVolume(douher::cArray3D< double >::Pointer phi)
{
  double v = 0.0;

  long nx = phi->getSizeX();
  long ny = phi->getSizeY();
  long nz = phi->getSizeZ();

  for (long ix = 0; ix < nx; ++ix )
    {
      for (long iy = 0; iy < ny; ++iy )
  {
    for (long iz = 0; iz < nz; ++iz )
      {
        double val = phi->get(ix, iy, iz);

        val = val<=0?1:0;
        
        v += val;
      }
  }
    }

  return v;
}




/*================================================================================*/
void 
CShapeBasedGAC::setStartingCenter(VectorType startingCenter) 
{
  m_startingCenter = startingCenter;
}


/*================================================================================*/
void 
CShapeBasedGAC::setStartingCenter(double x, double y, double z) 
{
  VectorType v(3); 
  v[0] = x; 
  v[1] = y; 
  v[2] = z; 

  setStartingCenter(v);
}


/*================================================================================*/
douher::cArray3D< double >::Pointer 
CShapeBasedGAC::moveCentroidToThePoint(douher::cArray3D< double >::Pointer phi, vnl_vector<double> c, double fillValue)
{
  vnl_vector<double> currentCentroid = computeCentroid(phi);

  vnl_vector<double> t = currentCentroid - c;

  vnl_matrix< double > A(3, 3);
  A.set_identity();


  itk::Image< double, 3 >::Pointer phiItk = douher::cArray3ToItkImage< double >(phi);

  unsigned char interpType = 1; // lnr interp

  itk::Image< double, 3 >::Pointer newImgItk = douher::affineTransform3< double >(phiItk, A, t, fillValue, interpType);

  return douher::itkImageToArray3< double >(newImgItk);
}


/*================================================================================*/
void CShapeBasedGAC::getMaskFromMeanShape()
{
  /* Generate a mask from the mean shape according to the given centroid and scale. 
     
  1. Scale the mean shape according to m_startingScale. Note,
  m_startingScale = 2 means the object has 2-times "middle l-r
  distance" than the mean shape.

  2. Move the centroid of the scaled mean shape to m_startingCenter

  3. Threshold to get mask
  */

  if (!m_meanShape)
    {
      std::cerr<<"set mean shape first.\n";
      raise(SIGABRT);
    }

  // 1.
  double fillValue = 3.0;
  douher::cArray3D< double >::Pointer scaleMean = isoScaleImage(m_meanShape, m_startingScale, fillValue);


//   //debug//
//   std::cout<<"m_startingScale = "<<m_startingScale<<std::endl;
//   std::cout<<"m_startingCenter = "<<m_startingCenter<<std::endl;
//   douher::saveAsImage3< double >(scaleMean, "scaleMean.nrrd");
//   exit(0);
//   //DEBUG//


  // 2.
  fillValue = 3.0;
  douher::cArray3D< double >::Pointer dm = moveCentroidToThePoint(scaleMean, m_startingCenter, fillValue);

  // 3.
  mp_mask.reset(new douher::cArray3D< unsigned char >(m_nx, m_ny, m_nz, 0));

  for (long ix = 0; ix < m_nx; ++ix)
    {
      for (long iy = 0; iy < m_ny; ++iy)
  {
    for (long iz = 0; iz < m_nz; ++iz)
      {
              if (dm->get(ix, iy, iz) <= 0)
    {
      mp_mask->set(ix, iy, iz, 1);
    }
      }
  }
    }
  
  //debug//
  douher::saveAsImage3< unsigned char >(mp_mask, "initMask.nrrd");
  //  exit(0);
  //DEBUG//


  return;
}


/*================================================================================*/
void
CShapeBasedGAC::computeCentroid()
{
  m_centroid = computeCentroid(mp_phi);

  return;    
}



/*================================================================================*/
vnl_vector<double> 
CShapeBasedGAC::computeCentroid(douher::cArray3D< double >::Pointer phi)
{
  vnl_vector<double> centroid(3, 0.0);
  double totalMass = 0.0;

  long nx = phi->getSizeX();
  long ny = phi->getSizeY();
  long nz = phi->getSizeZ();

  for (long ix = 0; ix < nx; ++ix )
    {
      for (long iy = 0; iy < ny; ++iy )
  {
    for (long iz = 0; iz < nz; ++iz )
      {
        double val = phi->get(ix, iy, iz);

        val = val<=0?1:0;
        
        totalMass += val;

        centroid[0] += val*ix;
        centroid[1] += val*iy;
        centroid[2] += val*iz;
      }
  }
    }


  centroid /= totalMass;

  return centroid;
}

/*================================================================================*/
void CShapeBasedGAC::setLeftAndRightPointsInTheMiddleSlice(long lx, long ly, long lz, long rx, long ry, long rz)
{
  VectorType l(3);
  l[0] = lx;
  l[1] = ly;
  l[2] = lz;

  VectorType r(3);
  r[0] = rx;
  r[1] = ry;
  r[2] = rz;

  setLeftAndRightPointsInTheMiddleSlice(l, r);

  return;
}

/*================================================================================*/
void CShapeBasedGAC::setLeftAndRightPointsInTheMiddleSlice(VectorType l, VectorType r)
{
  assert(l.size() == 3);
  assert(r.size() == 3);

  if (l[2] != r[2])
    {
      std::cerr<<"error, the two points should be in the same slice.\n";
      raise(SIGABRT);
    }

  VectorType c = (l+r)/2;

  setStartingCenter(c);

  double s = (l-r).two_norm();


  /* If the target pst has larger "middle l-r distance" than the mean
     shape, this scale is larger than 1, and the initial mask is set
     using this scale, in getMaskFromMeanShape()  */
  m_startingScale = s/m_LRDistanceInMiddleSliceOfMeanShape; 


//   //debug//
//   std::cout<<"m_LRDistanceInMiddleSliceOfMeanShape = "<<m_LRDistanceInMiddleSliceOfMeanShape<<std::endl;
//   std::cout<<"l = "<<l<<std::endl;
//   std::cout<<"r = "<<r<<std::endl;
//   std::cout<<"c = "<<c<<std::endl;
//   std::cout<<"m_startingScale = "<<m_startingScale<<std::endl;
//   exit(0);
//   //DEBUG//


}



/*================================================================================*/
douher::cArray3D< double >::Pointer 
CShapeBasedGAC::isoScaleImage(douher::cArray3D< double >::Pointer img, double r, double fillValue)
{
  vnl_matrix_fixed< double, 3, 3 > A(0.0);
  A(0, 0) = 1/(r + vnl_math::eps);
  A(1, 1) = 1/(r + vnl_math::eps);
  A(2, 2) = 1/(r + vnl_math::eps);
  vnl_vector_fixed< double, 3 > translation(0.0);

  itk::Image< double, 3 >::Pointer imgItk = douher::cArray3ToItkImage< double >(img);

  unsigned char interpType = 1; // lnr interp

  itk::Image< double, 3 >::Pointer newImgItk = douher::affineTransform3< double >(imgItk, A, translation, fillValue, interpType);

  return douher::itkImageToArray3< double >(newImgItk);
}
