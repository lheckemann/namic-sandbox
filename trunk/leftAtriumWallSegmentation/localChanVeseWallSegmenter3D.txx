#ifndef localChanVeseWallSegmenter3D_txx_
#define localChanVeseWallSegmenter3D_txx_

#include "localChanVeseWallSegmenter3D.h"

//itk
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 


#include <algorithm>


/* ============================================================
   basicInit    */
template< typename TPixel >
void
CLocalChanVeseWallSegmenter3D< TPixel >
::basicInit()
{
  SuperClassType::basicInit();
    
  // neighbor size
  m_nbx = 20;
  m_nby = 20;
  m_nbz = 3;

  m_globalInflation = 0.0; // pos: inflation; neg: contraction
  m_GACWeight = 0.9;
}  


/**
 * computeInnerProductBetweenGradientOfLevelSetAndGradientOfMetric    
 */
template< typename TPixel >
double
CLocalChanVeseWallSegmenter3D< TPixel >
::computeTwoTermsInGAC(long ix, long iy, long iz)
{
  /**
   * Compute the f \|\nabla \phi\|\kappa + \nabla f \cdot \nabla \Phi
   * where f is the conformal metric
   */

  if (ix < 1 || ix > this->m_nx-2 || iy < 1 || iy > this->m_ny-2 || iz < 1 || iz > this->m_nz-2)
    {
      return 0.0;
    }


  typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};
  typename itk::Image<TPixel, 3>::IndexType idxXMinus = {{ix-1, iy, iz}};
  typename itk::Image<TPixel, 3>::IndexType idxXPlus = {{ix+1, iy, iz}};
  typename itk::Image<TPixel, 3>::IndexType idxYMinus = {{ix, iy-1, iz}};
  typename itk::Image<TPixel, 3>::IndexType idxYPlus = {{ix, iy+1, iz}};
  typename itk::Image<TPixel, 3>::IndexType idxZMinus = {{ix, iy, iz-1}};
  typename itk::Image<TPixel, 3>::IndexType idxZPlus = {{ix, iy, iz+1}};

  TPixel phiValue = (this->mp_phi)->GetPixel(idx);
  TPixel phiValueXMinux = (this->mp_phi)->GetPixel(idxXMinus);
  TPixel phiValueXPlus = (this->mp_phi)->GetPixel(idxXPlus);
  TPixel phiValueYMinux = (this->mp_phi)->GetPixel(idxYMinus);
  TPixel phiValueYPlus = (this->mp_phi)->GetPixel(idxYPlus);
  TPixel phiValueZMinux = (this->mp_phi)->GetPixel(idxZMinus);
  TPixel phiValueZPlus = (this->mp_phi)->GetPixel(idxZPlus);

  double dPhiDxMinus = (phiValue - phiValueXMinux)/(this->m_dx);
  double dPhiDxPlus = (phiValueXPlus - phiValue)/(this->m_dx);
  double dPhiDyMinus = (phiValue - phiValueYMinux)/(this->m_dy);
  double dPhiDyPlus = (phiValueYPlus - phiValue)/(this->m_dy);
  double dPhiDzMinus = (phiValue - phiValueZMinux)/(this->m_dz);
  double dPhiDzPlus = (phiValueZPlus - phiValue)/(this->m_dz);
              
  double magnitudeOfNablaPhi_upwind = sqrt( ((dPhiDxPlus<=0)?dPhiDxPlus:0)*((dPhiDxPlus<=0)?dPhiDxPlus:0) \
                                                 + ((dPhiDxMinus>=0)?dPhiDxMinus:0)*((dPhiDxMinus>=0)?dPhiDxMinus:0) \
                                                 + ((dPhiDyPlus<=0)?dPhiDyPlus:0)*((dPhiDyPlus<=0)?dPhiDyPlus:0) \
                                                 + ((dPhiDyMinus>=0)?dPhiDyMinus:0)*((dPhiDyMinus>=0)?dPhiDyMinus:0)\
                                                 + ((dPhiDzPlus<=0)?dPhiDzPlus:0)*((dPhiDzPlus<=0)?dPhiDzPlus:0) \
                                                 + ((dPhiDzMinus>=0)?dPhiDzMinus:0)*((dPhiDzMinus>=0)?dPhiDzMinus:0));

  double f = this->mp_metricFromDistanceMapOfInterior->GetPixel(idx);

  double gradientOhPhi_upwind[3];
  gradientOhPhi_upwind[0] = sqrt(((dPhiDxPlus<=0)?dPhiDxPlus:0)*((dPhiDxPlus<=0)?dPhiDxPlus:0) \
                                 + ((dPhiDxMinus>=0)?dPhiDxMinus:0)*((dPhiDxMinus>=0)?dPhiDxMinus:0));

  gradientOhPhi_upwind[1] = sqrt(((dPhiDyPlus<=0)?dPhiDyPlus:0)*((dPhiDyPlus<=0)?dPhiDyPlus:0) \
                                 + ((dPhiDyMinus>=0)?dPhiDyMinus:0)*((dPhiDyMinus>=0)?dPhiDyMinus:0));

  gradientOhPhi_upwind[2] = sqrt(((dPhiDzPlus<=0)?dPhiDzPlus:0)*((dPhiDzPlus<=0)?dPhiDzPlus:0) \
                                 + ((dPhiDzMinus>=0)?dPhiDzMinus:0)*((dPhiDzMinus>=0)?dPhiDzMinus:0));

  double gradientMetric[3];
  gradientMetric[0] =                                                   \
    (this->mp_metricFromDistanceMapOfInterior->GetPixel(idxXPlus)             \
     - this->mp_metricFromDistanceMapOfInterior->GetPixel(idxXMinus))/(2.0*this->m_dx);
  gradientMetric[1] =                                                   \
    (this->mp_metricFromDistanceMapOfInterior->GetPixel(idxYPlus)             \
     - this->mp_metricFromDistanceMapOfInterior->GetPixel(idxYMinus))/(2.0*this->m_dy);
  gradientMetric[2] =                                                   \
    (this->mp_metricFromDistanceMapOfInterior->GetPixel(idxZPlus)             \
     - this->mp_metricFromDistanceMapOfInterior->GetPixel(idxZMinus))/(2.0*this->m_dz);

  double nablaMetricInerProductWithGradientOhPhi = gradientOhPhi_upwind[0]*gradientMetric[0] \
    + gradientOhPhi_upwind[1]*gradientMetric[1]                         \
    + gradientOhPhi_upwind[2]*gradientMetric[2];


  double kappaTimesMagnitudeOfNablaPhi = this->computeKappa(ix, iy, iz);

  double dGACValue = nablaMetricInerProductWithGradientOhPhi    \
    + f*magnitudeOfNablaPhi_upwind                 \
    + f*kappaTimesMagnitudeOfNablaPhi;


  return dGACValue;
}

/* ============================================================
   computeForce    */
template< typename TPixel >
void
CLocalChanVeseWallSegmenter3D< TPixel >
::computeForce()
{
  this->m_force.clear();

  //double fmax = -1e10;
  double GACMax = -1e10;
  double CVMax = -1e10;
  //double kappamax = -1e10;

  long n = this->m_lz.size();
  //  double* kappaOnZeroLS = new double[ n ];
  //double* externalForce = new double[ n ];
  double* GACForce = new double[ n ];
  double* CVForce = new double[ n ];

//   //tst
// #ifndef NDEBUG
//   extern std::ofstream leftAtriumWallSegmentGlobalOutputFile;
// #endif
//   //tst//

  {
    long i = 0;
    for (typename CSFLSLayer::iterator itz = this->m_lz.begin(); itz != this->m_lz.end(); ++itz)
      {
        long ix = (*itz)[0];
        long iy = (*itz)[1];
        long iz = (*itz)[2];

        //        kappaOnZeroLS[i] = this->computeKappa(ix, iy, iz);
        //        kappamax = kappamax>fabs(kappaOnZeroLS[i])?kappamax:fabs(kappaOnZeroLS[i]);
        
        computeMeansAt(ix, iy, iz);

        typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};

        double I = this->mp_img->GetPixel(idx);

        double distanceGACTerm = computeTwoTermsInGAC(ix, iy, iz);
        GACMax = (fabs(distanceGACTerm)>GACMax)?fabs(distanceGACTerm):GACMax;
        GACForce[i] = distanceGACTerm;

        
        double cvTerm = (I - m_meanIn)*(I - m_meanIn) - (I - m_meanOut)*(I - m_meanOut);
        CVMax = fabs(cvTerm)>CVMax?fabs(cvTerm):CVMax;
        CVForce[i] = cvTerm;

        //double externalValue = (1.0 - m_GACWeight)*cvTerm + m_GACWeight*distanceGACTerm;

//         //tst
// #ifndef NDEBUG
//         leftAtriumWallSegmentGlobalOutputFile<<cvTerm<<'\t'<<distanceGACTerm<<'\n';
// #endif
//         //tst//

        //fmax = fabs(externalValue)>fmax?fabs(externalValue):fmax;
        
        //externalForce[i] = externalValue;

        ++i;
      }
  }

  this->m_force.resize(n);

  for (long i = 0; i < n; ++i)
    {
      double fff = (1.0 - m_GACWeight)*CVForce[i]/(CVMax + vnl_math::eps) + m_GACWeight*GACForce[i]/(GACMax + vnl_math::eps);

      this->m_force[i] = fff;
    }

  delete[] GACForce;
  delete[] CVForce;
    
//delete[] kappaOnZeroLS;
//  delete[] externalForce;
}


/* ============================================================
   initializeSFLSFromMask    */
template< typename TPixel >
void
CLocalChanVeseWallSegmenter3D< TPixel >
::initializeSFLS()
{
  /**
   * 1. Dilate the mask to get the initial epi-cardium position. See
   * wiki to see why use distance map but not using binary dilation.
   */
  typedef itk::ImageRegionConstIterator< floatImage_t > itkImageRegionConstIterator_t;
  itkImageRegionConstIterator_t itConst(this->mp_distanceMapOfInterior, \
                                        this->mp_distanceMapOfInterior->GetLargestPossibleRegion());
  itConst.GoToBegin();

  typedef itk::ImageRegionIterator< MaskImageType > itkImageRegionIterator_t;
  itkImageRegionIterator_t iter(this->mp_mask, this->mp_mask->GetLargestPossibleRegion() );
  iter.GoToBegin();


  for (; !iter.IsAtEnd(); ++iter, ++itConst)
    {
      if (itConst.Get() <= 4)
        {
          iter.Set(1);
        }
      else
        {
          iter.Set(0);
        }
    }

//   typedef itk::BinaryBallStructuringElement< typename MaskImageType::PixelType, 3 >  StructuringElementType;
//   StructuringElementType  structuringElement;
//   structuringElement.SetRadius( this->m_meanWallThickness );  // structuring element
//   structuringElement.CreateStructuringElement();

//   typedef itk::BinaryDilateImageFilter< MaskImageType, MaskImageType, StructuringElementType >  DilateFilterType;
//   typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
//   binaryDilate->SetInput( this->mp_mask );
//   binaryDilate->SetKernel( structuringElement );
//   binaryDilate->SetDilateValue( 1 );
//   binaryDilate->Update();

//   this->mp_mask = binaryDilate->GetOutput();


  SuperClassType::initializeSFLS();
}


/* ============================================================
   doSegmenation    */
template< typename TPixel >
void
CLocalChanVeseWallSegmenter3D< TPixel >
::doSegmenation()
{
  this->resampleMaskImage();
  this->computeMetricFromDistanceMap();

  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */
  initializeSFLS();

  //douher::saveAsImage3< double >(mp_phi, "initPhi.nrrd");
  for (unsigned int it = 0; it < this->m_numIter; ++it)
    {
      //         //debug//
      //        std::cout<<"In "<<it<<"-th iteration, check nan:\n"<<std::flush;
      //         checkNan();
      //         //DEBUG//

      computeForce();

      this->normalizeForce();

      this->oneStepLevelSetEvolution();
    }
}


/* ============================================================
   computeMeansAt    */
template< typename TPixel >
void
CLocalChanVeseWallSegmenter3D< TPixel >
::computeMeansAt(long ix, long iy, long iz)
{
  /*----------------------------------------------------------------------
    Compute the local meanIn/Out areaIn/Out at this pixel. */

  m_areaIn = 0;
  m_areaOut = 0;

  m_meanIn = 0;
  m_meanOut = 0;

  for (long iix = ix-m_nbx; iix <= ix+m_nbx; ++iix)
    {
      for (long iiy = iy-m_nby; iiy <= iy+m_nby; ++iiy)
        {
          for (long iiz = iz-m_nbz; iiz <= iz+m_nbz; ++iiz)
            {
              if (iix >= 0 && iix < this->m_nx && iiy >= 0 && iiy < this->m_ny && iiz >= 0 && iiz < this->m_nz)
                {
                  typename itk::Image<TPixel, 3>::IndexType idx = {{iix, iiy, iiz}};

                  TPixel imgVal = (this->mp_img)->GetPixel(idx);
                  double phi = (this->mp_phi)->GetPixel(idx);
                  typename MaskImageType::PixelType isInside = this->mp_maskOfInterior->GetPixel(idx);
                  //float distToInterior = this->mp_distanceMapOfInterior->GetPixel(idx);

                  if (phi <= 0 && isInside == 0)
                    {
                      // inside the contour, outside the interior region
                      ++m_areaIn;
                      m_meanIn += imgVal;

                    }
                  else 
                    {
                      ++m_areaOut;
                      m_meanOut += imgVal;;
                    }
                }
            }
        }
    }

  m_meanIn /= (m_areaIn + vnl_math::eps);
  m_meanOut /= (m_areaOut + vnl_math::eps);

  return;
}


// //debug//
// template< typename TPixel >
// void
// CLocalChanVeseWallSegmenter3D< TPixel >
// ::checkNan()
// {
//   for (long ix = 0; ix < this->m_nx; ++ix)
//     {
//       for (long iy = 0; iy < this->m_ny; ++iy)
//         {
//           for (long iz = 0; iz < this->m_nz; ++iz)
//             {
//               typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};

//               if (isnan((this->mp_phi)->GetPixel(idx)))
//                 {
//                   std::cout<<"at ( "<<ix<<", "<<iy<<", "<<iz<<" ), has nan\n";
//                   raise(SIGABRT);
//                 }
//             }
//         }
//     }    
// }
//DEBUG//


#endif //localChanVeseWallSegmenter3D
