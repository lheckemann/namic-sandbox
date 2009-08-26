#ifndef __BRAINSFITUTILS_H__
#define __BRAINSFITUTILS_H__

//#include "FindLargestForgroundFilledMask.h"
#include "itkLargestForegroundFilledMaskImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImageRegionIteratorWithIndex.h"


/**
 * This file contains utility functions that are common to a few of the BRAINSFit Programs.
 */

typedef itk::SpatialObject<3>  ImageMaskType;
typedef ImageMaskType::Pointer ImageMaskPointer;

template <class VolumeType>
ImageMaskPointer DoROIAUTO(typename VolumeType::Pointer & extractedVolume,
  const double otsuPercentileThreshold, const int closingSize, const double thresholdCorrectionFactor = 1.0)
{
//   tempMaskImage = FindLargestForgroundFilledMask<VolumeType>(
//                           extractedVolume,
//                           otsuPercentileThreshold,
//                           closingSize);
  typedef itk::LargestForegroundFilledMaskImageFilter<VolumeType> LFFMaskFilterType;
  typename LFFMaskFilterType::Pointer LFF = LFFMaskFilterType::New();
  LFF->SetInput(extractedVolume);
  LFF->SetOtsuPercentileThreshold(otsuPercentileThreshold);
  LFF->SetClosingSize(closingSize);
  LFF->SetThresholdCorrectionFactor(thresholdCorrectionFactor);
  //  LFF->Update();
  
  typedef unsigned char                     NewPixelType;
  typedef itk::Image<NewPixelType, VolumeType::ImageDimension> NewImageType;
  typedef itk::CastImageFilter<VolumeType, NewImageType> CastImageFilter;
  typename CastImageFilter::Pointer castFilter = CastImageFilter::New();
  castFilter->SetInput( LFF->GetOutput() );
  castFilter->Update( );

  // save mask image to output variable
  // tempMaskImage = castFilter->GetOutput();
  // tempMaskImage->DisconnectPipeline();

  // convert mask image to mask
  typedef itk::ImageMaskSpatialObject<VolumeType::ImageDimension> ImageMaskSpatialObjectType;
  typename ImageMaskSpatialObjectType::Pointer mask = ImageMaskSpatialObjectType::New();
  mask->SetImage( castFilter->GetOutput() );
  mask->ComputeObjectToWorldTransform();

  ImageMaskPointer resultMaskPointer = dynamic_cast< ImageMaskType * >( mask.GetPointer() );
  return resultMaskPointer;
}



template <class TransformType, unsigned int ImageDimension>
void DoCenteredTransformMaskClipping(
        ImageMaskPointer &fixedMask, 
        ImageMaskPointer &movingMask,
        typename TransformType::Pointer transform, 
        double maskInferiorCutOffFromCenter)
{
  if ( fixedMask.IsNull()  ||  movingMask.IsNull() )
    {
    return;
    }
  if ( maskInferiorCutOffFromCenter >= 1000.0 )
    {
    return;
    }
std::cerr << "maskInferiorCutOffFromCenter is " << maskInferiorCutOffFromCenter << std::endl;

  typedef itk::ImageMaskSpatialObject<ImageDimension> ImageMaskSpatialObjectType;

  typedef unsigned char PixelType;
  typedef itk::Image<PixelType, ImageDimension> MaskImageType;
  
  typename TransformType::InputPointType rotationCenter = transform->GetCenter();
  typename TransformType::OutputVectorType translationVector = transform->GetTranslation();

  typename MaskImageType::PointType fixedCenter;
  typename MaskImageType::PointType movingCenter;

  for( unsigned int i=0; i<ImageDimension; i++ )
    {
    fixedCenter[i]  = rotationCenter[i];
    movingCenter[i] = translationVector[i] - rotationCenter[i];
    }

  typename ImageMaskSpatialObjectType::Pointer fixedImageMask( 
                                 dynamic_cast< ImageMaskSpatialObjectType * >( fixedMask.GetPointer() ));
  typename ImageMaskSpatialObjectType::Pointer movingImageMask( 
                                 dynamic_cast< ImageMaskSpatialObjectType * >( movingMask.GetPointer() ));

  typename MaskImageType::Pointer fixedMaskImage  = const_cast<MaskImageType *>( fixedImageMask->GetImage() );
  typename MaskImageType::Pointer movingMaskImage = const_cast<MaskImageType *>( movingImageMask->GetImage() );

  typename MaskImageType::PointType fixedInferior  = fixedCenter;
  typename MaskImageType::PointType movingInferior = movingCenter;
  
  fixedInferior[2]  -= maskInferiorCutOffFromCenter;  // negative because Superior is large in magnitude.
  movingInferior[2] -= maskInferiorCutOffFromCenter;  // ITK works in an LPS system.

  //  Here we will set the appropriate parts of the f/m MaskImages to zeros....
  typename MaskImageType::PixelType zero = 0;
  typename MaskImageType::PointType location;
  typedef itk::ImageRegionIteratorWithIndex<MaskImageType> MaskIteratorType;

  MaskIteratorType fixedIter(fixedMaskImage, fixedMaskImage->GetLargestPossibleRegion());
  fixedIter.Begin();
  while ( !fixedIter.IsAtEnd() )
    {
    fixedMaskImage->TransformIndexToPhysicalPoint( fixedIter.GetIndex(), location );
    if (location[2] < fixedInferior[2] )
      {
      fixedIter.Set(zero);
      }
    ++fixedIter;
    }

  MaskIteratorType movingIter(movingMaskImage, movingMaskImage->GetLargestPossibleRegion());
  movingIter.Begin();
  while ( !movingIter.IsAtEnd() )
    {
    movingMaskImage->TransformIndexToPhysicalPoint( movingIter.GetIndex(), location );
    if (location[2] < movingInferior[2] )
      {
      movingIter.Set(zero);
      }
    ++movingIter;
    }
  
  fixedImageMask->SetImage(  fixedMaskImage );
  movingImageMask->SetImage( movingMaskImage );
  
  fixedImageMask->ComputeObjectToWorldTransform();
  movingImageMask->ComputeObjectToWorldTransform();

  fixedMask  = dynamic_cast< ImageMaskType * >( fixedImageMask.GetPointer() );
  movingMask = dynamic_cast< ImageMaskType * >( movingImageMask.GetPointer() );
}

#endif // __BRAINSFITUTILS_H__
