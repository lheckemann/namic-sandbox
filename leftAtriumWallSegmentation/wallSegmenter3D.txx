#ifndef wallSegmenter3D_txx_
#define wallSegmenter3D_txx_

#include "wallSegmenter3D.h"

#include <algorithm>

#include "itkImageDuplicator.h"
#include "itkDanielssonDistanceMapImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkSigmoidImageFilter.h"


#include "vnl/vnl_math.h"


/* ============================================================
   basicInit    */
template< typename TPixel >
void
CWallSegmenter3D< TPixel >
::basicInit()
{
  m_sigmoidAlpha = 1.0;
  m_sigmoidBeta = 0.1;

  m_meanWallThickness = 3.02; // mm


  SuperClassType::basicInit();
}  



template< typename TPixel >
void
CWallSegmenter3D< TPixel >
::setMask(typename MaskImageType::Pointer mask)
{
  SuperClassType::setMask(mask);
  
  /**
   * Duplicate the mask
   *
   */
  typedef itk::ImageDuplicator< MaskImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(this->mp_mask);
  duplicator->Update();
  mp_maskOfInterior = duplicator->GetOutput();

  /**
   * Construct distance map from mask.
   */
  typedef itk::DanielssonDistanceMapImageFilter< MaskImageType, floatImage_t >  FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( mp_maskOfInterior );
  filter->InputIsBinaryOn();
  filter->UseImageSpacingOn();

  filter->Update();

  mp_distanceMapOfInterior = filter->GetOutput();


  computeMetricFromDistanceMap();



  return;
}

template< typename TPixel >
typename itk::Image<unsigned char, 3>::Pointer
CWallSegmenter3D< TPixel >
::getWallMask()
{
  if (!mp_maskOfWall)
    {
      mp_maskOfWall = MaskImageType::New();
      mp_maskOfWall->SetRegions(this->mp_mask->GetLargestPossibleRegion() );
      mp_maskOfWall->CopyInformation(this->mp_img);
      mp_maskOfWall->Allocate();
      mp_maskOfWall->FillBuffer(0);

  
      typedef itk::ImageRegionConstIterator< floatImage_t > itkImageRegionConstIterator_t;
      itkImageRegionConstIterator_t itConstPhi(this->mp_phi, this->mp_phi->GetLargestPossibleRegion());
      itConstPhi.GoToBegin();

      typedef itk::ImageRegionConstIterator< MaskImageType > itkMaskImageRegionConstIterator_t;
      itkMaskImageRegionConstIterator_t itConstInteriorMask(this->mp_maskOfInterior, \
                                                            this->mp_maskOfInterior->GetLargestPossibleRegion());
      itConstInteriorMask.GoToBegin();


      typedef itk::ImageRegionIterator< MaskImageType > itkImageRegionIterator_t;
      itkImageRegionIterator_t iter(mp_maskOfWall, mp_maskOfWall->GetLargestPossibleRegion() );
      iter.GoToBegin();

      for (; !itConstPhi.IsAtEnd(); ++itConstPhi, ++itConstInteriorMask, ++iter)
        {
          if (itConstPhi.Get() <= 0 && itConstInteriorMask.Get() == 0)
            {
              iter.Set(1);
            }
        }
    }

  return mp_maskOfWall;
}


template< typename TPixel >
void
CWallSegmenter3D< TPixel >
::computeMetricFromDistanceMap()
{
  /**
   * From distance map to compute the conformal metric
   */

  mp_metricFromDistanceMapOfInterior = floatImage_t::New();
  mp_metricFromDistanceMapOfInterior->SetRegions(mp_distanceMapOfInterior->GetLargestPossibleRegion());
  mp_metricFromDistanceMapOfInterior->Allocate();
  mp_metricFromDistanceMapOfInterior->CopyInformation(mp_distanceMapOfInterior);
  mp_metricFromDistanceMapOfInterior->FillBuffer(0.0);

  float maxMetricValue = -1e6;
  float minMetricValue = 1e6;

  typedef itk::ImageRegionConstIterator< floatImage_t > itkImageRegionConstIterator_t;
  itkImageRegionConstIterator_t itConst(mp_distanceMapOfInterior, mp_distanceMapOfInterior->GetLargestPossibleRegion());
  itConst.GoToBegin();

  typedef itk::ImageRegionIterator< floatImage_t > itkImageRegionIterator_t;
  itkImageRegionIterator_t iter(mp_metricFromDistanceMapOfInterior,   \
                                mp_metricFromDistanceMapOfInterior->GetLargestPossibleRegion() );

  float meanThickness2 = m_meanWallThickness*m_meanWallThickness;
  float newValue = 0.0;
  float d = 0.0;
  for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter, ++itConst)
    {
      d = itConst.Get(); 
      if (d <= vnl_math::eps)
        {
          continue;
          //iter.Set(100.0);
        }

      newValue = 0.1*(meanThickness2/(vnl_math::eps + d) + d - 2*m_meanWallThickness + 1.0);
      // + 1.0 coz can't be zero, but the smallest value should be
      // smaller than 1.0 otherwise its always larger than
      // Euclidian, so multiply the whole by 0.1

      iter.Set(newValue);

      maxMetricValue = maxMetricValue>=newValue?maxMetricValue:newValue;
      minMetricValue = minMetricValue<=newValue?minMetricValue:newValue;
    }

  /** Set the interior to max value above*/
  for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
    {
      float d = iter.Get(); 
      if (d <= vnl_math::eps)
        {
          iter.Set(maxMetricValue);
        }
    }



  /**
   * Pass the metric to the sigmoid filter to make it sharper. This is
   * done in the GAC example in the itkSoftware guide. So I assume it
   * is an effective step for GAC.
   */
  typedef itk::SigmoidImageFilter< floatImage_t, floatImage_t >  SigmoidFilterType;
  typename SigmoidFilterType::Pointer sigmoidFilter = SigmoidFilterType::New();

  sigmoidFilter->SetOutputMinimum(   0.02  );
  sigmoidFilter->SetOutputMaximum(   1.0  );


  m_sigmoidAlpha = 0.1;
  sigmoidFilter->SetAlpha(  m_sigmoidAlpha  );
  m_sigmoidBeta = minMetricValue + 8.0*m_sigmoidAlpha;
  sigmoidFilter->SetBeta( m_sigmoidBeta );

  sigmoidFilter->SetInput( mp_metricFromDistanceMapOfInterior );
  sigmoidFilter->Update();

  mp_metricFromDistanceMapOfInterior = sigmoidFilter->GetOutput();

  return;
}



#endif //wallSegmenter3D_txx_
