#include "shapeBasedSeg.h"
#include "shapeBasedSegCost.h"

#include <csignal>

//vnl
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_lbfgsb.h>


//douher
#include "lib/imageProcessing.h"
#include "lib/shuxue.h"

//============================================================
CShapeBasedSeg::CShapeBasedSeg()
{
  basicInitialization();
}

//============================================================
void CShapeBasedSeg::basicInitialization()
{ 
  m_numOfParameters = 0;

  m_segDone = false;

  m_maxFnEval = 10;

  m_maxIteration = 10;

  m_inputImageSet = false;
  m_meanShapeSet = false;

  m_shapeSize[0] = 0;
  m_shapeSize[1] = 0;
  m_shapeSize[2] = 0;

  m_spacing[0] = 1.0;
  m_spacing[1] = 1.0;
  m_spacing[2] = 1.0;

  m_epsilon = 0.1;

  m_sigma = 0.01;

  m_affineMatrixCoef = 1000.0;

  return;
}

//============================================================
void CShapeBasedSeg::generateInitialScalings()
{
  assert(m_numOfParameters >= 12);

  m_scalings.set_size(m_numOfParameters);
  m_scalings.fill(1.0);

  return;
}

//============================================================
void CShapeBasedSeg::generateInitialParameters()
{
  assert(m_numOfParameters >= 12);

  m_parameters.set_size(m_numOfParameters);
  m_parameters.fill(0.0);

  // affine mtx is be the identity mtx
  m_parameters[0] = 1.0;
  m_parameters[4] = 1.0;
  m_parameters[8] = 1.0;

  return;
}

//============================================================
void CShapeBasedSeg::setInputImage(DoubleImageType::Pointer image) 
{
  m_inputImage = image;
  m_inputImageSet = true;  

  DoubleImageType::SpacingType sp = m_inputImage->GetSpacing();

  m_spacing[0] = sp[0];
  m_spacing[1] = sp[1];
  m_spacing[2] = sp[2];


  return;
}

//============================================================
void CShapeBasedSeg::setMeanShape(DoubleImageType::Pointer meanShape)
{
  DoubleImageType::SizeType sz = meanShape->GetLargestPossibleRegion().GetSize();
  if (0 == sz[0]*sz[1]*sz[2])
    {
      std::cerr<<"mean shape has 0 in size. abort.\n";
      raise(SIGABRT);
    }

  if ( 0 == m_shapeSize[0] )
    {
      m_shapeSize[0] = sz[0];
      m_shapeSize[1] = sz[1];
      m_shapeSize[2] = sz[2];
    }
  else if( (long)sz[0] != m_shapeSize[0] || (long)sz[1] != m_shapeSize[1] || (long)sz[2] != m_shapeSize[2] )
    {
      std::cerr<<"meanShape and m_shapeSize doesnt match. abort\n";
      raise(SIGABRT);
    }


  m_meanShape = meanShape;
  m_meanShapeSet = true;

  return;
}

//============================================================
void CShapeBasedSeg::addEigenShape(DoubleImageType::Pointer eigenShape)
{
  DoubleImageType::SizeType sz = eigenShape->GetLargestPossibleRegion().GetSize();
  if (0 == sz[0]*sz[1]*sz[2])
    {
      std::cerr<<"eigen shape has 0 in size. abort.\n";
      raise(SIGABRT);
    }

  if ( 0 == m_shapeSize[0] )
    {
      m_shapeSize[0] = sz[0];
      m_shapeSize[1] = sz[1];
      m_shapeSize[2] = sz[2];
    }
  else if( (long)sz[0] != m_shapeSize[0] || (long)sz[1] != m_shapeSize[1] || (long)sz[2] != m_shapeSize[2] )
    {
      std::cerr<<"eigenShape and m_shapeSize doesnt match. abort\n";
      raise(SIGABRT);
    }


  m_listOfEigenShapes.push_back(eigenShape);

  return;
}

//============================================================
itk::Image< double, 3 >::Pointer CShapeBasedSeg::transformImageUsingParameter(DoubleImageType::Pointer img) const
{
  return transformImageUsingParameter(img, m_parameters);
}

//============================================================
itk::Image< double, 3 >::Pointer \
CShapeBasedSeg::transformImageUsingParameter(DoubleImageType::Pointer img, const VnlDoubleVectorType& param) const
{
  VnlDoubleVector3DType translation;
  VnlDoubleMatrix3x3Type A;
  parameterVectorToAffineMatrixAndTranslationVector(param, A, translation);


  DoubleImageType::SizeType inputImageSize = m_inputImage->GetLargestPossibleRegion().GetSize();
  double fillValue = 3.0; // for sparse field level set, outside value is 3.0
  unsigned char interpType = 1; // lnr

  DoubleImageType::Pointer \
    shapeOfInputImageSize = douher::affineTransform3<double>(img, A, translation, fillValue, interpType, \
                                                             inputImageSize[0], inputImageSize[1], inputImageSize[2]);

  // now the shapeOfInputImageSize is of the same size of input image

  return shapeOfInputImageSize;
}


//============================================================
itk::Image< double, 3 >::Pointer CShapeBasedSeg::generateShapeFromParameter() const
{
  return generateShapeFromParameter(m_parameters);
}


//============================================================
itk::Image< double, 3 >::Pointer CShapeBasedSeg::generateShapeFromParameter(const VnlDoubleVectorType& param) const
{
  /*
    extract the affine, translation and pca parameters
   */
  VnlDoubleVector3DType translation;
  VnlDoubleMatrix3x3Type A;
  parameterVectorToAffineMatrixAndTranslationVector(param, A, translation);

  VnlDoubleVectorType eigenParam;
  parameterVectorToEigenShapeParameters(param, eigenParam);


  // duplicate the mean shape
  typedef itk::ImageDuplicator< DoubleImageType >  DuplicatorType;
  DuplicatorType::Pointer duplicator  = DuplicatorType::New();
  duplicator->SetInputImage( m_meanShape );
  duplicator->Update();

  DoubleImageType::Pointer shape = duplicator->GetOutput();
  
  long n = eigenParam.size();

  DoubleImageType::SizeType sz = shape->GetLargestPossibleRegion().GetSize();
  for ( long ix = 0; ix < (long)sz[0]; ++ix)
    {
      for (long iy = 0; iy < (long)sz[1]; ++iy)
        {
          for (long iz = 0; iz < (long)sz[2]; ++iz)
            {
              DoubleImageType::IndexType idx;
              idx[0] = ix;
              idx[1] = iy;
              idx[2] = iz;

              for (long p = 0; p < n; ++p)
                {
                  double v = shape->GetPixel(idx);
              
                  double pThEigenImgValueAtHere = (m_listOfEigenShapes[p])->GetPixel(idx);

                  shape->SetPixel(idx, v + eigenParam[p]*pThEigenImgValueAtHere);
                }
            }
        }
    }


  DoubleImageType::SizeType inputImageSize = m_inputImage->GetLargestPossibleRegion().GetSize();
  double fillValue = 3.0; // for sparse field level set, outside value is 3.0
  unsigned char interpType = 1; // lnr
  DoubleImageType::Pointer \
    shapeOfInputImageSize = douher::affineTransform3<double>(shape, A, translation, fillValue, interpType, \
                                                             inputImageSize[0], inputImageSize[1], inputImageSize[2]);

  // now the shapeOfInputImageSize is of the same size of input image

  return shapeOfInputImageSize;
}




/*============================================================
 * Yezzi eng */
double CShapeBasedSeg::getCost() const
{
  return getCost(m_parameters);
}

/*============================================================
 * Yezzi eng */
double CShapeBasedSeg::getCost( const VnlDoubleVectorType& param) const
{
  assert(param.size() >= 12);

  /* 
   * refer to work log #6, 20090625 for detail
   */
  DoubleImageType::Pointer shape = generateShapeFromParameter(param);  // same size as input image

  double S_u = 0; // image first origin moment inside contour
  double S_v = 0; // image first origin moment outside contour

  double A_u = 0; // area inside contour
  double A_v = 0; // area outside contour

  typedef itk::ImageRegionIteratorWithIndex< DoubleImageType > ImageRegionIteratorWithIndex;
  ImageRegionIteratorWithIndex imIter(m_inputImage, m_inputImage->GetLargestPossibleRegion());
  ImageRegionIteratorWithIndex shapeIter(shape, shape->GetLargestPossibleRegion());

  for ( imIter.GoToBegin(), shapeIter.GoToBegin(); !imIter.IsAtEnd(); ++imIter, ++shapeIter )
    {
      DoubleImageType::PixelType imVal = imIter.Get();
      DoubleImageType::PixelType shapeVal = shapeIter.Get();

//       double h = bandHeaviside(-shapeVal, m_epsilon);
//       double h_minus = bandHeaviside(shapeVal, m_epsilon);
      double h = heaviside(-shapeVal, m_epsilon);
      double h_minus = heaviside(shapeVal, m_epsilon);

      A_u += h;
      A_v += h_minus;

      S_u += imVal*( h );
      S_v += imVal*( h_minus );
    }


  double u = S_u/( A_u + vnl_math::eps );
  double v = S_v/( A_v + vnl_math::eps );

  //debug//
  std::cout<<"A_u = "<<A_u<<", \t A_v = "<<A_v<<std::endl;
  std::cout<<"S_u = "<<S_u<<", \t S_v = "<<S_v<<std::endl;
  std::cout<<"u = "<<u<<", \t v = "<<v<<std::endl;
  //DEBUG//

//   std::cout<<"----------"<<std::endl<<std::flush;

//   std::cout<<"current param = "<<pcaParam<<",   "<<transfmParam.extract(3)<<", "<<transfmParam.extract(3, 3)<<", "<<transfmParam[transfmParam.size() - 1]<<std::endl<<std::flush;

//   std::cout<<"eng = "<< -(u-v)*(u-v)/2.0<<std::endl<<std::flush;


  double theCost = -(u-v)*(u-v)/2.0;


  return theCost;
}



/*============================================================
 * Yezzi eng */
void CShapeBasedSeg::getCostAndGradient(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const
{
  /*
    compute function value (*f) and its gradient vector (*g), gvn
    current state x
   */

  assert(currentParam.size() >= 12);

  (*cost) = 0.0;

  gradient->set_size(m_numOfParameters);
  gradient->fill(0.0);


  typedef itk::ImageRegionIteratorWithIndex< DoubleImageType > ImageRegionIteratorWithIndex;


  DoubleImageType::Pointer shape = generateShapeFromParameter(currentParam);  // same size as input image


  /*============================================================
   * compute the gradient wrt eigen parameters 
   */
  int nEig = m_listOfEigenShapes.size();
  std::vector< double > dSudEigenParam(nEig);
  std::vector< double > dAudEigenParam(nEig);
  std::vector< double > dSvdEigenParam(nEig);
  std::vector< double > dAvdEigenParam(nEig);


  if ((m_scalings.extract(nEig, 12)).two_norm() >= 1e-8 )
    {
      //      std::cout<<"I'm in\n";

      for (int iEig = 0; iEig < nEig; ++iEig)
        {
          DoubleImageType::Pointer iNthEigenImageTransformed = transformImageUsingParameter(m_listOfEigenShapes[iEig]);

          ImageRegionIteratorWithIndex iNthEigenIter(iNthEigenImageTransformed, \
                                                     iNthEigenImageTransformed->GetLargestPossibleRegion());

          ImageRegionIteratorWithIndex imIter(m_inputImage, m_inputImage->GetLargestPossibleRegion());
          ImageRegionIteratorWithIndex shapeIter(shape, shape->GetLargestPossibleRegion());

          for ( imIter.GoToBegin(), iNthEigenIter.GoToBegin(), shapeIter.GoToBegin(); \
                !imIter.IsAtEnd(); \
                ++iNthEigenIter, ++imIter, ++shapeIter )
            {
              DoubleImageType::PixelType iNthEigenVal = iNthEigenIter.Get();
              DoubleImageType::PixelType shapeVal  = shapeIter.Get();
              DoubleImageType::PixelType imVal = imIter.Get();

//               double dltShapeVal = bandDelta(-shapeVal, m_epsilon);
//               double dltMinusShapeVal = bandDelta(shapeVal, m_epsilon);

              double dltShapeVal = delta(-shapeVal, m_epsilon);
              double dltMinusShapeVal = delta(shapeVal, m_epsilon);


              dAudEigenParam[iEig] -= dltShapeVal*iNthEigenVal;
              dSudEigenParam[iEig] -= imVal*dltShapeVal*iNthEigenVal;

              dAvdEigenParam[iEig] += dltMinusShapeVal*iNthEigenVal;
              dSvdEigenParam[iEig] += imVal*dltMinusShapeVal*iNthEigenVal;
            }
        }
    }



  /*============================================================
   * grad wrt affine transformation
   */
  ImageRegionIteratorWithIndex imIter(m_inputImage, m_inputImage->GetLargestPossibleRegion());
  ImageRegionIteratorWithIndex shapeIter(shape, shape->GetLargestPossibleRegion());


  DoubleImageType::SizeType shapeSize = shape->GetLargestPossibleRegion().GetSize();
  double centerX = (double)shapeSize[0]/2.0;
  double centerY = (double)shapeSize[1]/2.0;
  double centerZ = (double)shapeSize[2]/2.0;


  double S_u = 0; // image first origin moment inside contour
  double S_v = 0; // image first origin moment outside contour

  double A_u = 0; // area inside contour
  double A_v = 0; // area outside contour

  // affine
  double dAuDA00 = 0;
  double dAuDA01 = 0;
  double dAuDA02 = 0;

  double dAuDA10 = 0;
  double dAuDA11 = 0;
  double dAuDA12 = 0;

  double dAuDA20 = 0;
  double dAuDA21 = 0;
  double dAuDA22 = 0;

  double dSuDA00 = 0;
  double dSuDA01 = 0;
  double dSuDA02 = 0;

  double dSuDA10 = 0;
  double dSuDA11 = 0;
  double dSuDA12 = 0;

  double dSuDA20 = 0;
  double dSuDA21 = 0;
  double dSuDA22 = 0;

  // translation
  double dAuDTx = 0;
  double dAuDTy = 0;
  double dAuDTz = 0;

  double dSuDTx = 0;
  double dSuDTy = 0;
  double dSuDTz = 0;

  for ( imIter.GoToBegin(), shapeIter.GoToBegin(); \
        !imIter.IsAtEnd(); \
        ++imIter, ++shapeIter )
    {
      DoubleImageType::IndexType shapeIdx = shapeIter.GetIndex();

      if (shapeIdx[0] == 0 || shapeIdx[1] == 0 || shapeIdx[2] == 0 \
          || shapeIdx[0] == (long)shapeSize[0]-1 || shapeIdx[1] == (long)shapeSize[1]-1 || shapeIdx[2] == (long)shapeSize[2]-1 )
  {
    // on boundary
    continue;
  }

      DoubleImageType::PixelType shapeVal = shapeIter.Get();
      DoubleImageType::PixelType imVal = imIter.Get();

//       double h = bandHeaviside(-shapeVal, m_epsilon);
//       double h_minus = bandHeaviside(shapeVal, m_epsilon);
      double h = heaviside(-shapeVal, m_epsilon);
      double h_minus = heaviside(shapeVal, m_epsilon);


      A_u += h;
      A_v += h_minus;

      S_u += imVal*( h );
      S_v += imVal*( h_minus );


      DoubleImageType::IndexType shapeIdx_xMinus = shapeIdx;
      shapeIdx_xMinus[0] --;
      DoubleImageType::IndexType shapeIdx_xPlus = shapeIdx; 
      shapeIdx_xPlus[0] ++;
      DoubleImageType::IndexType shapeIdx_yMinus = shapeIdx;
      shapeIdx_yMinus[1] --;
      DoubleImageType::IndexType shapeIdx_yPlus = shapeIdx;
      shapeIdx_yPlus[1] ++;
      DoubleImageType::IndexType shapeIdx_zMinus = shapeIdx;
      shapeIdx_zMinus[2] --;
      DoubleImageType::IndexType shapeIdx_zPlus = shapeIdx;
      shapeIdx_zPlus[2] ++;

      DoubleImageType::PixelType shapeVal_xMinus = shape->GetPixel(shapeIdx_xMinus);
      DoubleImageType::PixelType shapeVal_xPlus = shape->GetPixel(shapeIdx_xPlus);
      DoubleImageType::PixelType shapeVal_yMinus = shape->GetPixel(shapeIdx_yMinus);
      DoubleImageType::PixelType shapeVal_yPlus = shape->GetPixel(shapeIdx_yPlus);
      DoubleImageType::PixelType shapeVal_zMinus = shape->GetPixel(shapeIdx_zMinus);
      DoubleImageType::PixelType shapeVal_zPlus = shape->GetPixel(shapeIdx_zPlus);


      //       double dShapedx = (shapeVal_xPlus - shapeVal_xMinus)/(2.0*m_spacing[0]);
      //       double dShapedy = (shapeVal_yPlus - shapeVal_yMinus)/(2.0*m_spacing[1]);
      //       double dShapedz = (shapeVal_zPlus - shapeVal_zMinus)/(2.0*m_spacing[2]);
      double dShapedx = (shapeVal_xPlus - shapeVal_xMinus)/(2.0);
      double dShapedy = (shapeVal_yPlus - shapeVal_yMinus)/(2.0);
      double dShapedz = (shapeVal_zPlus - shapeVal_zMinus)/(2.0);


      //      double dltShape = douher::delta(shapeVal, m_epsilon);
      // double dltShape = bandDelta(shapeVal, m_epsilon);
      double dltShape = douher::delta(shapeVal, m_epsilon);

      // dltShape*dShapedx appears many times, use tmp variable to save time

      // dAv/d* = dAu/d*, so no need to compute dAv/d*
      // dSv/d* = dSu/d*, so no need to compute dSv/d*

      double dltShapedShapedx = dltShape*dShapedx;
      double dltShapedShapedy = dltShape*dShapedy;
      double dltShapedShapedz = dltShape*dShapedz;

      dAuDTx -= dltShapedShapedx;
      dAuDTy -= dltShapedShapedy;
      dAuDTz -= dltShapedShapedz;

      dSuDTx -= imVal*dltShapedShapedx;
      dSuDTy -= imVal*dltShapedShapedy;
      dSuDTz -= imVal*dltShapedShapedz;

      double theX = shapeIdx[0] - centerX;
      double theY = shapeIdx[1] - centerY;
      double theZ = shapeIdx[2] - centerZ;

      dAuDA00 -= dltShapedShapedx*theX;
      dAuDA01 -= dltShapedShapedx*theY;
      dAuDA02 -= dltShapedShapedx*theZ;

      dAuDA10 -= dltShapedShapedy*theX;
      dAuDA11 -= dltShapedShapedy*theY;
      dAuDA12 -= dltShapedShapedy*theZ;

      dAuDA20 -= dltShapedShapedz*theX;
      dAuDA21 -= dltShapedShapedz*theY;
      dAuDA22 -= dltShapedShapedz*theZ;


      dSuDA00 -= imVal*dltShapedShapedx*theX;
      dSuDA01 -= imVal*dltShapedShapedx*theY;
      dSuDA02 -= imVal*dltShapedShapedx*theZ;

      dSuDA10 -= imVal*dltShapedShapedy*theX;
      dSuDA11 -= imVal*dltShapedShapedy*theY;
      dSuDA12 -= imVal*dltShapedShapedy*theZ;

      dSuDA20 -= imVal*dltShapedShapedz*theX;
      dSuDA21 -= imVal*dltShapedShapedz*theY;
      dSuDA22 -= imVal*dltShapedShapedz*theZ;
    }


  double u = S_u/( A_u + vnl_math::eps );
  double v = S_v/( A_v + vnl_math::eps );

  // affine
  double dCostDA00 = (v - u)*( (dSuDA00 - u*dAuDA00)/(A_u + vnl_math::eps) \
                               + (dSuDA00 - v*dAuDA00)/(A_v + vnl_math::eps));
                                   
  double dCostDA01 = (v - u)*( (dSuDA01 - u*dAuDA01)/(A_u + vnl_math::eps) \
                               + (dSuDA01 - v*dAuDA01)/(A_v + vnl_math::eps));
  double dCostDA02 = (v - u)*( (dSuDA02 - u*dAuDA02)/(A_u + vnl_math::eps) \
                               + (dSuDA02 - v*dAuDA02)/(A_v + vnl_math::eps));

  double dCostDA10 = (v - u)*( (dSuDA10 - u*dAuDA10)/(A_u + vnl_math::eps) \
                               + (dSuDA10 - v*dAuDA10)/(A_v + vnl_math::eps));
  double dCostDA11 = (v - u)*( (dSuDA11 - u*dAuDA11)/(A_u + vnl_math::eps) \
                               + (dSuDA11 - v*dAuDA11)/(A_v + vnl_math::eps));
  double dCostDA12 = (v - u)*( (dSuDA12 - u*dAuDA12)/(A_u + vnl_math::eps) \
                               + (dSuDA12 - v*dAuDA12)/(A_v + vnl_math::eps));

  double dCostDA20 = (v - u)*( (dSuDA20 - u*dAuDA20)/(A_u + vnl_math::eps) \
                               + (dSuDA20 - v*dAuDA20)/(A_v + vnl_math::eps));
  double dCostDA21 = (v - u)*( (dSuDA21 - u*dAuDA21)/(A_u + vnl_math::eps) \
                               + (dSuDA21 - v*dAuDA21)/(A_v + vnl_math::eps));
  double dCostDA22 = (v - u)*( (dSuDA22 - u*dAuDA22)/(A_u + vnl_math::eps) \
                               + (dSuDA22 - v*dAuDA22)/(A_v + vnl_math::eps));

  // translation
  double dCostDTx = (v - u)*( (dSuDTx - u*dAuDTx)/(A_u + vnl_math::eps) \
                              + (dSuDTx - v*dAuDTx)/(A_v + vnl_math::eps));
  double dCostDTy = (v - u)*( (dSuDTy - u*dAuDTy)/(A_u + vnl_math::eps) \
                              + (dSuDTy - v*dAuDTy)/(A_v + vnl_math::eps));
  double dCostDTz = (v - u)*( (dSuDTz - u*dAuDTz)/(A_u + vnl_math::eps) \
                              + (dSuDTz - v*dAuDTz)/(A_v + vnl_math::eps));


  gradient->put(0, dCostDA00 ); 
  gradient->put(1, dCostDA01 ); 
  gradient->put(2, dCostDA02 ); 

  gradient->put(3, dCostDA10 ); 
  gradient->put(4, dCostDA11 ); 
  gradient->put(5, dCostDA12 ); 

  gradient->put(6, dCostDA20 ); 
  gradient->put(7, dCostDA21 ); 
  gradient->put(8, dCostDA22 ); 


  gradient->put(9, dCostDTx);
  gradient->put(10, dCostDTy);
  gradient->put(11, dCostDTz);




  if ((m_scalings.extract(nEig, 12)).two_norm() >= 1e-8 )
    {
      // eigen shapes
      for (int iEig = 12; iEig < m_numOfParameters; ++iEig)
        {
          (*gradient)[iEig] = (v - u)*( (dSudEigenParam[iEig-12] - u*dAudEigenParam[iEig-12])/(A_u + vnl_math::eps) \
                                        - (dSvdEigenParam[iEig-12] - v*dAvdEigenParam[iEig-12])/(A_v + vnl_math::eps));
        }
    }



  //  std::cout<<"before adjustment = "<<(*gradient)<<std::endl<<std::flush;
  this->adjustGradient(gradient);
  //  std::cout<<"after adjustment = "<<(*gradient)<<std::endl<<std::flush;

  (*cost) = -(u-v)*(u-v)/2.0;

  return;
}




//============================================================
void CShapeBasedSeg::adjustGradient(VnlDoubleVectorType* originalGradient) const
{
  // mask out by m_scalings
  for (long i = 0; i < m_numOfParameters; ++i)
    {
      (*originalGradient)[i] = m_scalings[i]*(*originalGradient)[i];
    }


  // get norm after masking 
  double lInf = fabs( (*originalGradient)[0] );
  for (int i = 1; i < m_numOfParameters; ++i)
    {
      double tmp = fabs( (*originalGradient)[i] );
      lInf = lInf>tmp?lInf:tmp;
    }

  //  std::cout<<"lInf = "<<lInf<<std::endl<<std::flush;


  // normalize
  if (lInf > 1)
    {
      for (int i = 0; i < m_numOfParameters; ++i)
        {
          (*originalGradient)[i] /= (lInf + vnl_math::eps );
        }
    }


  return;
}



//============================================================
void CShapeBasedSeg::parameterVectorToEigenShapeParameters(const VnlDoubleVectorType& currentParam, \
                                             VnlDoubleVectorType& eigenParam) const
{
  eigenParam = currentParam.extract(m_numOfParameters - 12, 12);

  return;
}


//============================================================
void CShapeBasedSeg::eigenShapeParametersToParameterVector(const VnlDoubleVectorType& eigenParam, \
                                                           VnlDoubleVectorType& currentParam) const
{
  currentParam.update(eigenParam, 12);

  return;
}



/*==================================================*/
void CShapeBasedSeg::parameterVectorToAffineMatrixAndTranslationVector(const VnlDoubleVectorType &currentParam, \
                                                                       VnlDoubleMatrix3x3Type& affine, \
                                                                       VnlDoubleVector3DType& translation) const
{
  affine(0, 0) = currentParam[0];
  affine(0, 1) = currentParam[1];
  affine(0, 2) = currentParam[2];

  affine(1, 0) = currentParam[3];
  affine(1, 1) = currentParam[4];
  affine(1, 2) = currentParam[5];

  affine(2, 0) = currentParam[6];
  affine(2, 1) = currentParam[7];
  affine(2, 2) = currentParam[8];

  translation[0] = currentParam[9];
  translation[1] = currentParam[10];
  translation[2] = currentParam[11];

  return;
}


/*==================================================*/
void CShapeBasedSeg::affineMatrixAndTranslationVectorToParameterVector(const VnlDoubleMatrix3x3Type& affine, \
                                                                       const VnlDoubleVector3DType& translation, \
                                                                       VnlDoubleVectorType& currentParam) const
{
  currentParam[0] = affine(0, 0);
  currentParam[1] = affine(0, 1);
  currentParam[2] = affine(0, 2);

  currentParam[3] = affine(1, 0);
  currentParam[4] = affine(1, 1);
  currentParam[5] = affine(1, 2);

  currentParam[6] = affine(2, 0);
  currentParam[7] = affine(2, 1);
  currentParam[8] = affine(2, 2);

  currentParam[9] = translation[0];
  currentParam[10]= translation[1];
  currentParam[11] = translation[2];

  return;
}

/*============================================================*/
void CShapeBasedSeg::optimizeEigenParameters()
{
  // 1. store the m_scalings
  VnlDoubleVectorType oldScalings = m_scalings;
  vnl_vector< long > optBoundSelection(m_numOfParameters, 0);
  VnlDoubleVectorType upperBound(m_numOfParameters, 3.0);
  VnlDoubleVectorType lowerBound(m_numOfParameters, -3.0);

  // 2. m_scalings only turn on those for translation
  m_scalings.fill(0);
  for (int i = 12; i < m_numOfParameters; ++i)
    {
      m_scalings[i] = 1;
      optBoundSelection[i] = 2; // both upper/lower bounds
    }


  // 3. opt
  CShapeBasedSegCost costFn(this);
  costFn.dim = m_numOfParameters;

  vnl_lbfgsb optimizer(costFn);
  optimizer.set_max_function_evals(m_maxFnEval);

  optimizer.set_bound_selection(optBoundSelection);
  optimizer.set_lower_bound(lowerBound);
  optimizer.set_upper_bound(upperBound);
    //   optimizer.set_verbose(false);
    //   optimizer.set_trace(false);
  optimizer.minimize(m_parameters);


  // 4. restore m_scalings
  m_scalings = oldScalings;  

  return;
}


/*============================================================*/
void CShapeBasedSeg::optimizeTranslationParameters()
{
  // 1. store the m_scalings
  VnlDoubleVectorType oldScalings = m_scalings;

//   vnl_vector< long > optBoundSelection(m_numOfParameters, 0);
//   VnlDoubleVectorType upperBound(m_numOfParameters, 0.0);
//   VnlDoubleVectorType lowerBound(m_numOfParameters, 0.0);

//   // set the bounds using image size
//   DoubleImageType::SizeType sz = m_inputImage->GetLargestPossibleRegion().GetSize();

  // 2. m_scalings only turn on those for translation
  m_scalings.fill(0);
  for (int i = 9; i < 12; ++i)
    {
      m_scalings[i] = 1;

      //      optBoundSelection[i] = 2; // both upper/lower bounds
    }

//   upperBound[9] = sz[0]/10;
//   lowerBound[9] = -(long)sz[0]/10;
//   upperBound[10] = sz[1]/10;
//   lowerBound[10] = -(long)sz[1]/10;
//   upperBound[11] = sz[2]/4;
//   lowerBound[11] = -(long)sz[2]/4;

//   std::cout<<"lower bounds = "<<lowerBound<<std::endl;
//   std::cout<<"upper bounds = "<<upperBound<<std::endl;


  // 3. opt
  CShapeBasedSegCost costFn(this);
  costFn.dim = m_numOfParameters;

  vnl_lbfgsb optimizer(costFn);
  optimizer.set_max_function_evals(m_maxFnEval);

  //  optimizer.set_bound_selection(optBoundSelection);
//   optimizer.set_lower_bound(lowerBound);
//   optimizer.set_upper_bound(upperBound);


//   optimizer.set_verbose(false);
//   optimizer.set_trace(false);
  optimizer.minimize(m_parameters);


  // 4. restore m_scalings
  m_scalings = oldScalings;

  return;
}

/*============================================================*/
void CShapeBasedSeg::optimizeAffineParameters()
{
  // 1. store the m_scalings
  VnlDoubleVectorType oldScalings = m_scalings;

  // 2. m_scalings only turn on those for translation
  m_scalings.fill(0);
  for (int i = 0; i < 9; ++i)
    {
      m_scalings[i] = 1;
    }

  //  3. opt
  CShapeBasedSegCost costFn(this);
  costFn.dim = m_numOfParameters;

  vnl_lbfgsb optimizer(costFn);
  optimizer.set_max_function_evals(m_maxFnEval);
//   optimizer.set_verbose(false);
//   optimizer.set_trace(false);
  optimizer.minimize(m_parameters);


  // 4. restore m_scalings
  m_scalings = oldScalings;

  return;
}

/*============================================================*/
void CShapeBasedSeg::optimizeParameters()
{
  VnlDoubleVectorType tmp;

  for (unsigned long i = 0; i < m_maxIteration; ++i)
    {
      tmp = m_parameters;

      optimizeTranslationParameters();
      std::cout<<"reg trans x = "<<m_parameters<<std::endl;

      optimizeAffineParameters();
      std::cout<<"ref affin x = "<<m_parameters<<std::endl;

      optimizeEigenParameters();
      std::cout<<"ref eigen x = "<<m_parameters<<std::endl;


      if ((tmp - m_parameters).inf_norm() <= 0.01)
        {
          std::cout<<" inf norm of chg in parameter <= 0.01. stop.\n";
          break;
        }
    }

  return;
}


/*==================================================*/
void CShapeBasedSeg::areYouReady()
{
  if (m_numOfParameters < 12)
    {
      std::cerr<<"m_numOfParameters < 12, abort\n";
      raise(SIGABRT);
    }

  if (!m_inputImageSet)
    {
      std::cerr<<"Input image not set. abort\n";
      raise(SIGABRT);
    }

  if (!m_meanShapeSet)
    {
      std::cerr<<"Mean shape not set. abort\n";
      raise(SIGABRT);
    }

  if ( m_listOfEigenShapes.empty() )
    {
      std::cerr<<"m_listOfEigenShapes is empty. abort\n";
      raise(SIGABRT);
    }

  return;
}

/*==================================================*/
void CShapeBasedSeg::gogogo(void)
{
  /*============================================================
   * set num of total parameters */
  m_numOfParameters = 12 + m_listOfEigenShapes.size();
  /* set num of total parameters
   *============================================================*/

  generateInitialParameters();
  generateInitialScalings();


  areYouReady();

  //dbug
  std::cout<<"m_numOfParameters = "<<m_numOfParameters<<std::endl;




  // optimize the parameters
  optimizeParameters();


  m_segDone = true;


  return;
}



/*==================================================*/
itk::Image< double, 3 >::Pointer CShapeBasedSeg::getSegmentation() const
{
  if (!m_segDone)
    {
      std::cerr<<"seg not done yet. abort.\n";
      raise(SIGABRT);
    }

  return generateShapeFromParameter();
}

// /*==================================================*/
// inline double CShapeBasedSeg::bandHeaviside(double t, double epsilon) const
// {
//   double b = 10;
//   return (atan(t/epsilon) - atan((t - b)/epsilon))/(vnl_math::pi);
// }

// /*==================================================*/
// inline double CShapeBasedSeg::bandDelta(double t, double epsilon) const
// {
//   double e2 = epsilon*epsilon;

//   double b = 10;
//   double y = (e2/(e2 + t*t) + e2/(e2 + (t - b)*(t - b)))/(vnl_math::pi);

//   return y;
// }


/*==================================================*/
inline double CShapeBasedSeg::heaviside(double t, double epsilon) const
{
  return 0.5 + atan(t/epsilon)/(vnl_math::pi);
}

/*==================================================*/
inline double CShapeBasedSeg::delta(double t, double epsilon) const
{
  double e2 = epsilon*epsilon;

  return e2/(vnl_math::pi)/(e2 + t*t);
}
