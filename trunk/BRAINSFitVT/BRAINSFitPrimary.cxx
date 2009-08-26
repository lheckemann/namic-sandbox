/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date: 2007-08-31 11:20:20 -0500 (Fri, 31 Aug 2007) $
  Version:   $Revision: 10358 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

//  This program was modified from
// Insight/Examples/Registration/ImageRegistration8.cxx
//  and is an improved replacement for the old (and defective)
//  brains2/src/iplProg/MutualRegistration program.

//  Image registration using the VersorRigid3DTransform and the
//  MattesMutualInformationImageToImageMetric is the logical
//  thing to insist on (with ITK) when seeking rigid 3D registrations.

//TODO:  This needs to be moved to the top, and header files moved to this header where needed.
#include "BRAINSFitBSpline.h"

#include "CenterOfBrain.h"
#include "itkImageMaskSpatialObject.h"
#include "itkMedianImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"

#include "itkTransformFactory.h"
#include "itkScaleVersor3DTransform.h"
#include "itkScaleSkewVersor3DTransform.h"
#include "itkAffineTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkIO.h"
#include "ReadMask.h"
#include "BRAINSFitPrimaryCLP.h"

#include "itkCenteredVersorTransformInitializer.h"
#include "itkCenteredTransformInitializer.h"

#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkVersorTransformOptimizer.h"

#include <fstream>
#include <string>

#include "itkVector.h"
#include "itkMultiThreader.h"

#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkExtractImageFilter.h"

#include "FindLargestForgroundFilledMask.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkAffineTransform.h"
#include <stdio.h>
#include "itkMultiModal3DMutualRegistrationHelper.h"
#include "ConvertToRigidAffine.h"

#include "itkTimeProbesCollectorBase.h"

#ifdef USE_DEBUG_IMAGE_VIEWER
#include "DebugImageViewerClient.h"
#endif


// Check that ITK was compiled with correct flags set:
#ifndef ITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE
#error \
  "Results will not be correct if ITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE is turned off"
#endif
#ifndef ITK_USE_ORIENTED_IMAGE_DIRECTION
#error \
  "Results will not be correct if ITK_USE_ORIENTED_IMAGE_DIRECTION is turned off"
#endif
#ifndef ITK_USE_TRANSFORM_IO_FACTORIES
#error \
  "BRAINSFit Requires ITK_USE_TRANSFORM_IO_FACTORIES to be on, please rebuild ITK."
#endif

#if (ITK_VERSION_MAJOR < 3  )
#if (ITK_VERSION_MINOR < 15 )
#if (ITK_VERSION_PATCH < 0 )
  #warning \
  "A version of ITK greater than 3.15.0 is needed in order to have the cross platform transform patches included."
  #warning \
  "Use at your own risk with versions less than 3.15.0."
  #warning \
  "Version 3.15.0 has fixed several issues related to inconsistent rounding on different platforms, and inconsistent definition of the center of voxels."
#endif
#endif
#endif

typedef float PixelType;
// Dimension and MaxInputDimension comes from an enum at the start of itkMultiModal3DMutualRegistrationHelper.h

typedef itk::Image<PixelType,
  Dimension>                            fixedVolumeType;
typedef itk::Image<PixelType,
  Dimension>                            movingVolumeType;

typedef itk::Image<PixelType,
  MaxInputDimension>                    InputImageType;

typedef itk::SpatialObject<Dimension>  ImageMaskType;
typedef ImageMaskType::Pointer ImageMaskPointer;

typedef itk::ImageFileReader<InputImageType>
fixedVolumeReaderType;
typedef itk::ImageFileReader<InputImageType>
movingVolumeReaderType;

typedef itk::ResampleImageFilter<movingVolumeType,
  fixedVolumeType> ResampleFilterType;

typedef itk::AffineTransform<double,
  Dimension>                     AffineTransformType;
typedef AffineTransformType::Pointer
AffineTransformPointer;

typedef itk::Vector<double, Dimension> VectorType;

// This function deciphers the BackgroundFillValueString and returns a double
// precision number based on the requested value
double GetBackgroundFillValueFromString(
  const std::string BackgroundFillValueString )
{
  const std::string BIGNEGText("BIGNEG");
  const std::string NaNText("NaN");
  double            BackgroundFillValue = 0.0;

  if ( BackgroundFillValueString == BIGNEGText )
    {
    union {
      unsigned int i_val[2];
      double d_val;
      } FourByteHolder;
    FourByteHolder.i_val[0] = 0xF000F000;
    FourByteHolder.i_val[1] = 0xF000F000;
    BackgroundFillValue = FourByteHolder.d_val;
    }
  else if ( BackgroundFillValueString == NaNText )
    {
    union {
      unsigned int i_val[2];
      double d_val;
      } FourByteHolder;
    FourByteHolder.i_val[0] = 0xFFFFFFFF;
    FourByteHolder.i_val[1] = 0xFFFFFFFF;
    BackgroundFillValue = FourByteHolder.d_val;
    }
  else
    {
    BackgroundFillValue
      = static_cast<double>( atof( BackgroundFillValueString.c_str() ) );
    }
  return BackgroundFillValue;
}

template <class TransformType>
void WriteTransform(typename TransformType::Pointer MyTransform,
  const std::string TransformFilename)
{
  /*
   *  Convert the transform to the appropriate assumptions and write it out as requested.
   */
    {
    typedef itk::TransformFileWriter TransformWriterType;
    TransformWriterType::Pointer transformWriter =  TransformWriterType::New();
    transformWriter->SetFileName( TransformFilename.c_str() );
    transformWriter->SetInput( MyTransform );
    transformWriter->Update();
    std::cout << "Wrote ITK transform to text file: "
              << TransformFilename.c_str() << std::endl;
    }
}

template <class TransformType>
void WriteTransformWithBulk(typename TransformType::Pointer MyTransform,
  const std::string TransformFilename)
{
  /*
   *  Convert the transform to the appropriate assumptions and write it out as requested.
   */
    {
    typedef itk::TransformFileWriter TransformWriterType;
    TransformWriterType::Pointer transformWriter =  TransformWriterType::New();
    transformWriter->SetFileName( TransformFilename.c_str() );
    transformWriter->AddTransform( MyTransform->GetBulkTransform() );
    transformWriter->AddTransform( MyTransform );
    transformWriter->Update();
    std::cout << "Appended ITK transform to text file: "
              << TransformFilename.c_str() << std::endl;
    }
}

template <class ImageType>
typename ImageType::Pointer ExtractImage(
  typename InputImageType::Pointer & inputImage,
  unsigned int InputImageTimeIndex)
{
  typedef typename itk::ExtractImageFilter<InputImageType,
    ImageType> ExtractImageFilterType;
  typename ExtractImageFilterType::Pointer extractImageFilter
    = ExtractImageFilterType::New();

  // fixedVolumeReader->GetOutput();
  InputImageType::RegionType inputRegion = inputImage->GetLargestPossibleRegion();
  InputImageType::SizeType   inputSize = inputRegion.GetSize();
  inputSize[3] = 0;
  inputRegion.SetSize(inputSize);

  InputImageType::IndexType inputIndex = inputRegion.GetIndex();
  inputIndex[0] = 0;
  inputIndex[1] = 0;
  inputIndex[2] = 0;
  inputIndex[3] = InputImageTimeIndex;
  inputRegion.SetIndex(inputIndex);
  extractImageFilter->SetExtractionRegion( inputRegion );
  extractImageFilter->SetInput(inputImage);

  try
    {
    extractImageFilter->Update();
    }
  catch (... )
    {
    std::cout << "Error while extracting a time indexed fixed image."
              << std::endl;
    throw;
    }
  typename ImageType::Pointer extractImage = extractImageFilter->GetOutput();
  //  std::cerr << "Extract fixed image origin" << extractImage->GetOrigin() <<
  // std::endl;

  return extractImage;
}

template <class ImageType>
typename ImageType::Pointer DoMedian(typename ImageType::Pointer & input,
  typename ImageType::SizeType indexRadius)
{
  typedef typename itk::MedianImageFilter<ImageType,
    ImageType> MedianFilterType;
  typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

  medianFilter->SetRadius( indexRadius );
  medianFilter->SetInput( input );
  medianFilter->Update();
  typename ImageType::Pointer result = medianFilter->GetOutput();
  return result;
}

template <class fixedVolumeType, class movingVolumeType, class TransformType,
  class SpecificInitializerType>
typename TransformType::Pointer DoCenteredInitialization(
  typename fixedVolumeType::Pointer & orientedFixedVolume,
  typename movingVolumeType::Pointer & orientedMovingVolume,
  std::string & initializeTransformMode)
{
  typename TransformType::Pointer initialITKTransform = TransformType::New();
  initialITKTransform->SetIdentity();

  if ( initializeTransformMode == "GeometryOn" )
    {
    // GeometryOn assumes objects are center in field of view, with different
    // physical extents tot he fields of view
    typedef itk::CenteredTransformInitializer<TransformType, fixedVolumeType,
      movingVolumeType> OrdinaryInitializerType;
    typename OrdinaryInitializerType::Pointer CenteredInitializer
      = OrdinaryInitializerType::New();

    CenteredInitializer->SetFixedImage(orientedFixedVolume);
    CenteredInitializer->SetMovingImage(orientedMovingVolume);
    CenteredInitializer->SetTransform(initialITKTransform);
    CenteredInitializer->GeometryOn(); // Use the image spce center:

    //  DILEMMA: this was the comment on Doxygen for ITK's implementation of GeometryOn and MomentsOn:
    //  The geometrical center of the [emphasis here:] moving image is passed as initial center
    //  of rotation to the transform and the vector from the center of the
    //  fixed image to the center of the moving image is passed as the initial
    //  translation. This mode basically assumes that the anatomical objects
    //  to be registered are centered in their respective images. Hence the best
    //  initial guess for the registration is the one that superimposes those two centers.
    CenteredInitializer->InitializeTransform();
    }
  else if ( initializeTransformMode == "CenterOfHead" )
    {
    typename movingVolumeType::PointType movingCenter = GetCenterOfBrain<movingVolumeType>(orientedMovingVolume);
    typename fixedVolumeType::PointType fixedCenter = GetCenterOfBrain<fixedVolumeType>(orientedFixedVolume);

    typename TransformType::InputPointType rotationCenter;
    typename TransformType::OutputVectorType translationVector;

    //  DILEMMA: this was the code inside Code/Common/itkCenteredTransformInitializer.txx:
    //  Note that it doesn't agree with the comment;  what gives?  Are we supposed to swap the parameters?
    for( unsigned int i=0; i<Dimension; i++ )
      {
      rotationCenter[i]    = fixedCenter[i];
      translationVector[i] = movingCenter[i] - fixedCenter[i];
      }

    initialITKTransform->SetCenter( rotationCenter );
    initialITKTransform->SetTranslation( translationVector );
    }
  else if ( initializeTransformMode == "MomentsOn" )
    {
    // MomentsOn assumes that the structures being registered have same amount
    // of mass approximately uniformly distributed.
    typename SpecificInitializerType::Pointer CenteredInitializer
      = SpecificInitializerType::New();

    CenteredInitializer->SetFixedImage(orientedFixedVolume);
    CenteredInitializer->SetMovingImage(orientedMovingVolume);
    CenteredInitializer->SetTransform(initialITKTransform);
    CenteredInitializer->MomentsOn(); // Use intensity center of mass

    CenteredInitializer->InitializeTransform();
    }
  else  // can't happen unless an unimplemented CLP option was added:
    {
    std::cout << "FAILURE:  Improper mode for initializeTransformMode: "
              << initializeTransformMode << std::endl;
    exit(-1);
    }
  std::cout << "Initializing transform with "  << initializeTransformMode
            << " to " << std::endl;
  std::cout << initialITKTransform << std::endl;
  std::cout << "===============================================" << std::endl;
  return initialITKTransform;
}

template <class VolumeType>
ImageMaskPointer DoROIAUTO(typename VolumeType::Pointer & extractedVolume,
  const double otsuPercentileThreshold, const int closingSize, std::string& maskFileName)
{
  typename VolumeType::Pointer tempMaskImage
    = FindLargestForgroundFilledMask<VolumeType>(
                          extractedVolume,
                          otsuPercentileThreshold,
                          closingSize);

  typedef unsigned char                     NewPixelType;
  typedef itk::Image<NewPixelType, Dimension> NewImageType;
  typedef itk::CastImageFilter<VolumeType, NewImageType> CastImageFilter;
  typename CastImageFilter::Pointer castFilter = CastImageFilter::New();
  castFilter->SetInput( tempMaskImage );
  castFilter->Update( );

  // convert mask image to mask
  typedef itk::ImageMaskSpatialObject<Dimension> ImageMaskSpatialObjectType;
  typename ImageMaskSpatialObjectType::Pointer mask = ImageMaskSpatialObjectType::New();
  mask->SetImage( castFilter->GetOutput() );
  mask->ComputeObjectToWorldTransform();

  if (maskFileName != "")
    {
      typedef itk::ImageFileWriter<NewImageType> WriterType;
      WriterType::Pointer w = WriterType::New();
      w->SetFileName(maskFileName.c_str());
      w->SetInput(castFilter->GetOutput());
      w->Update();
    }

  ImageMaskPointer resultMaskPointer = dynamic_cast< ImageMaskType * >( mask.GetPointer() );
  return resultMaskPointer;
}



#ifdef USE_DEBUG_IMAGE_VIEWER
/*************************
 * Have a global variable to
 * add debugging information.
*/
DebugImageViewerClient DebugImageDisplaySender;
#endif

int BRAINSFitPrimary( int argc, char *argv[] )
{
  PARSE_ARGS;

  itk::TimeProbesCollectorBase tp0;


  RegisterBrains2MaskFactory();

#ifdef USE_DEBUG_IMAGE_VIEWER
  DebugImageDisplaySender.SetEnabled(UseDebugImageViewer);
#endif

  if ( debugNumberOfThreads > 0 ) // NOTE: Default is -1, which then uses the
                                  // ITK default.
    {
    itk::MultiThreader::SetGlobalMaximumNumberOfThreads(debugNumberOfThreads);
    }
  if ( ( initializeTransformMode != "Off" ) && ( initialTransform.size() > 0 ) )
    {
    std::cout
   << "FATAL ERROR:  initializeTransformMode and initialTransform can not be used at the same time!"
   << std::endl;
    exit(-1);
    }
  std::vector<int> zeroOrigin(3, 0);

  //Verify that the spline grid sizes are greater than 3
    {
    bool validGridSize=true;
    for( unsigned int sgs=0; sgs< splineGridSize.size(); sgs++)
      {
      if(splineGridSize[sgs] < 3)
        {
        validGridSize=false;
        std::cout << "splineGridSize["<<sgs<<"]= "<< splineGridSize[sgs] 
          << " is invalid.  There must be at lest 3 divisions in each dimension of the image." << std::endl;
        }
      }
    if(validGridSize == false)
      {
      exit(-1);
      }
    }

  //  const bool explicitOriginsFlag(fixedVolumeOriginArg.isSet() &&
  //                                 movingVolumeOriginArg.isSet());

  if ( outputTransform.size() == 0
       && strippedOutputTransform.size() == 0
       && outputVolume.size() == 0 )
    {
    std::cout << "Error:  user requested neither outputTransform,"
              << " nor strippedOutputTransform,"
              << " nor outputVolume." << std::endl;
    return 2;
    }

  if(numberOfIterations.size() != transformType.size())
    {
    if(numberOfIterations.size() != 1)
      {
      std::cerr << "The numberOfIterations array must match the transformType length" <<std::endl;
      exit(-1);
      }
      else
      {
      // replicate throughout
      const int numberOf = numberOfIterations[0];
      for (unsigned int i=1; i<transformType.size(); i++)
        {
        numberOfIterations.push_back(numberOf);
        }
      }
    }
  if(minimumStepSize.size() != transformType.size())
    {
    if(minimumStepSize.size() != 1)
      {
      std::cerr << "The minimumStepSize array must match the transformType length" <<std::endl;
      exit(-1);
      }
      else
      {
      // replicate throughout
      const double stepSize = minimumStepSize[0];
      for (unsigned int i=1; i<transformType.size(); i++)
        {
        minimumStepSize.push_back(stepSize);
        }
      }
    }
    {
    // Need to review transform types in the transform type vector to ensure that they are ordered appropriately
    // for processing.  I.e. that they go from low dimensional to high dimensional, and throw an error if 
    // B-Spline is before Rigid, or any other non-sensical ordering of the transform types.
    //Rigid=1, ScaleVersor3D=2, Affine=4, ScaleSkewVersor3D=3, and BSpline=5
    unsigned int CurrentTransformRank=0;
    for(unsigned int l=0;l<transformType.size();l++)
      {
      if(transformType[l] == "Rigid")
        {
        if(CurrentTransformRank<=1)
          {
          CurrentTransformRank=1;
          }
        else
          {
          std::cerr << "Ordering of transforms does not proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
          exit (-1);
          }
        }
      else if(transformType[l] == "ScaleVersor3D")
        {
        if(CurrentTransformRank<=2)
          {
          CurrentTransformRank=2;
          }
        else
          {
          std::cerr << "Ordering of transforms does not proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
          exit (-1);
          }
        }
      else if(transformType[l] == "ScaleSkewVersor3D")
        {
        if(CurrentTransformRank<=3)
          {
          CurrentTransformRank=3;
          }
        else
          {
          std::cerr << "Ordering of transforms does not proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
          exit (-1);
          }
        }
      else if(transformType[l] == "Affine")
        {
        if(CurrentTransformRank<=4)
          {
          CurrentTransformRank=4;
          }
        else
          {
          std::cerr << "Ordering of transforms does not proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
          exit (-1);
          }
        }
      else if(transformType[l] == "BSpline")
        {
        if(CurrentTransformRank<=5)
          {
          CurrentTransformRank=5;
          }
        else
          {
          std::cerr << "Ordering of transforms does not proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
          exit (-1);
          }
        }
      else
        {
        std::cerr << " ERROR:  Invalid transform type specified for element " << l << " of --transformType: " << transformType[l] <<std::endl;
        std::cerr << "Ordering of transforms must proceed from\n"
            << "smallest to largest.  Please review settings for transformType.\n"
            << "Rigid < ScaleVersor3D < ScaleSkewVersor3D < Affine < BSpline" << std::endl;
        exit(-1);
        }
      }
    }

  fixedVolumeType::Pointer  orientedFixedVolume;
  movingVolumeType::Pointer orientedMovingVolume;
  // Extracting a timeIndex cube from the fixed image goes here....
  // Also MedianFilter
  fixedVolumeType::Pointer  extractfixedVolume;
  movingVolumeType::Pointer extractmovingVolume;
  InputImageType::Pointer
  OriginalfixedVolume ( itkUtil::ReadImage<InputImageType>(fixedVolume) );

  std::cerr << "Original Fixed image origin"
            << OriginalfixedVolume->GetOrigin() << std::endl;
  // fixedVolumeTimeIndex lets lets us treat 3D as 4D.
  /***********************
   * Acquire Fixed Image Index
   **********************/
  extractfixedVolume = ExtractImage<fixedVolumeType>(OriginalfixedVolume,
                                                     fixedVolumeTimeIndex);
  // Extracting a timeIndex cube from the moving image goes here....

  InputImageType::Pointer OriginalmovingVolume (
    itkUtil::ReadImage<InputImageType>(movingVolume) );
  // This default lets us treat 3D as 4D.
  // const unsigned int movingVolumeTimeIndex;

  /***********************
   * Acquire Moving Image Index
   **********************/
  extractmovingVolume = ExtractImage<movingVolumeType>(OriginalmovingVolume,
                                                       movingVolumeTimeIndex);

#ifdef USE_DEBUG_IMAGE_VIEWER
  if(DebugImageDisplaySender.Enabled())
    {
    DebugImageDisplaySender.SendImage<itk::Image<float, 3> >(extractfixedVolume,
                                                             0);
    }
  //  DebugImageDisplaySender.SendImage<itk::Image<float,3>
  // >(extractmovingVolume,1);
#endif

  // get median filter radius.
  // const unsigned int MedianFilterRadius =
  // command.GetValueAsInt(MedianFilterRadiusText, IntegerText);
  // Median Filter images if requested.
  if ( medianFilterSize[0] > 0 || medianFilterSize[1] > 0
       || medianFilterSize[2] > 0 )
    {
    fixedVolumeType::SizeType indexRadius;
    indexRadius[0] = static_cast<long int>( medianFilterSize[0] ); // radius along x
    indexRadius[1] = static_cast<long int>( medianFilterSize[1] ); // radius along y
    indexRadius[2] = static_cast<long int>( medianFilterSize[2] ); // radius along z
    // DEBUG
    std::cout << "Median radius  " << indexRadius << std::endl;
    extractfixedVolume = DoMedian<fixedVolumeType>(extractfixedVolume,
                                                   indexRadius);
    extractmovingVolume = DoMedian<movingVolumeType>(extractmovingVolume,
                                                     indexRadius);
    }

  orientedFixedVolume = extractfixedVolume;
  orientedMovingVolume = extractmovingVolume;


  if ( histogramMatch )
    {
    typedef itk::HistogramMatchingImageFilter<itk::Image<float, 3>,
            itk::Image<float, 3> > HistogramMatchingFilterType;
    HistogramMatchingFilterType::Pointer histogramfilter
      = HistogramMatchingFilterType::New();

    histogramfilter->SetInput( orientedMovingVolume  );
    histogramfilter->SetReferenceImage( orientedFixedVolume );

    histogramfilter->SetNumberOfHistogramLevels( numberOfHistogramBins );
    histogramfilter->SetNumberOfMatchPoints( numberOfMatchPoints );
    histogramfilter->ThresholdAtMeanIntensityOn();
    histogramfilter->Update();
    orientedMovingVolume=histogramfilter->GetOutput() ;
    }

#ifdef USE_DEBUG_IMAGE_VIEWER
  // if(DebugImageViewer_On)
  //   {
  //   DebugImageDisplaySender.SendImage<itk::Image<float,3>
  //   }
  // >(extractmovingVolume,2);
#endif

  // DEBUG
  std::cout << "Original moving volume Direction matrix:  " << std::endl
            << OriginalmovingVolume->GetDirection() << std::endl;
  std::cout << "extracted moving volume Direction matrix:  " << std::endl
            << extractmovingVolume->GetDirection() << std::endl;
  std::cout << "Original fixed volume Direction matrix:  " << std::endl
            << OriginalfixedVolume->GetDirection() << std::endl;
  std::cout << "extracted fixed volume Direction matrix:  " << std::endl
            << extractfixedVolume->GetDirection() << std::endl;
  std::cout << "Original moving volume Origin:  "
            << OriginalmovingVolume->GetOrigin() << std::endl;
  std::cout << "extracted moving volume Origin:  "
            << extractmovingVolume->GetOrigin() << std::endl;
  std::cout << "Original fixed volume Origin:  "
            << OriginalfixedVolume->GetOrigin() << std::endl;
  std::cout << "extracted fixed volume Origin:  "
            << extractfixedVolume->GetOrigin() << std::endl;

  //
  // If masks are associated with the images, then read them into the correct
  // orientation.
  // if they've been defined assign the masks...
  //
  ImageMaskPointer fixedMask = NULL;
  ImageMaskPointer movingMask = NULL;

if (maskProcessingMode == "NOMASK")
  {
  if ( ( fixedBinaryVolume != "" )
      || ( movingBinaryVolume != "" ) )
    {
    std::cout
   << "ERROR:  Can not specify mask file names when the default of NOMASK is used for the maskProcessingMode"
   << std::endl;
    exit(-1);
    }
  }
else if ( maskProcessingMode == "ROIAUTO" )
  {
  if ( ( fixedBinaryVolume != "" )
      || ( movingBinaryVolume != "" ) )
    {
    std::cout
   << "ERROR:  Can not specify mask file names when ROIAUTO is used for the maskProcessingMode"
   << std::endl;
    exit(-1);
    }
  const double otsuPercentileThreshold = 0.01;
  const int    closingSize = 7;
  tp0.Start("ROIAUTO -- fixed image");
  fixedMask = DoROIAUTO<fixedVolumeType>(extractfixedVolume,
    otsuPercentileThreshold,
    closingSize,fixedVolumeROI);
  std::cout << "Time before second ROIAUTO: " << std::endl;
  tp0.Stop("ROIAUTO -- fixed image");
  tp0.Start("ROIAUTO -- moving image");
  movingMask = DoROIAUTO<movingVolumeType>(extractmovingVolume,
    otsuPercentileThreshold,
    closingSize,movingVolumeROI);
  tp0.Stop("ROIAUTO -- moving image");
  
  }
else if ( maskProcessingMode == "ROI" )
  {
  if ( ( fixedBinaryVolume == "" )
      || ( movingBinaryVolume == "" ) )
    {
    std::cout
   <<
    "ERROR:  Must specify mask file names when ROI is used for the maskProcessingMode"
   << std::endl;
    exit(-1);
    }
  fixedMask = ReadImageMask<ImageMaskType, Dimension>(
    fixedBinaryVolume,
    orientedFixedVolume->GetDirection(),
    orientedFixedVolume->GetOrigin() );
  movingMask = ReadImageMask<ImageMaskType, Dimension>(
    movingBinaryVolume,
    orientedMovingVolume->GetDirection(),
    orientedMovingVolume->GetOrigin() );
  }


  // This default fills the background with zeros
  //  const double BackgroundFillValue =
  //  GetBackgroundFillValueFromString(command.GetValueAsString(BackgroundFillValueText,
  //  FloatCodeText));

  //  const std::string patientID(command.GetValueAsString(patientIDText,
  // IdStringText));
  //  const std::string studyID(command.GetValueAsString(patientIDText,
  // IdStringText));

  /*
   *  Everything prior to this point is preprocessing
   *  Start Processing
   *
   */
  static const unsigned int SpaceDimension = 3;
  static const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;
  typedef itk::BSplineDeformableTransform<
    CoordinateRepType,
    SpaceDimension,
    SplineOrder > BSplineTransformType;

  typedef itk::VersorRigid3DTransform<double> VersorRigid3DTransformType;
  typedef itk::ScaleVersor3DTransform<double> ScaleVersor3DTransformType;
  typedef itk::ScaleSkewVersor3DTransform<double> ScaleSkewVersor3DTransformType;

  // Apparently when you register one transform, you need to register all your
  // transforms.
  //
  itk::TransformFactory<VersorRigid3DTransformType>::RegisterTransform();
  itk::TransformFactory<ScaleVersor3DTransformType>::RegisterTransform();
  itk::TransformFactory<ScaleSkewVersor3DTransformType>::RegisterTransform();
  itk::TransformFactory<AffineTransformType>::RegisterTransform();
  itk::TransformFactory<BSplineTransformType>::RegisterTransform();

  //
  // read in the initial ITKTransform
  //
  //
  typedef itk::TransformFileReader                    TransformReaderType;
  typedef itk::TransformFileReader::TransformListType TransformListType;
  TransformReaderType::Pointer affineReader =  TransformReaderType::New();
  TransformListType transformsList;
  itk::TransformFileReader::TransformListType::const_iterator transformsListIterator;
  if ( initialTransform.size() > 0 )
    {
    std::cout
                 << "Read ITK affine transform from text file: "
                 << initialTransform << std::endl;

    affineReader->SetFileName( initialTransform.c_str() );
    try
      {
      affineReader->Update();
      }
    catch ( itk::ExceptionObject & err )
      {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return 7;
      }
    transformsList = *( affineReader->GetTransformList() );
    if ( transformsList.size() == 0 )
      {
      std::cout << "Number of transformsList = " << transformsList.size() << std::endl;
      std::cout << "FATAL ERROR: Failed to read transform"
                << initialTransform << std::endl;
      exit(-1);
      }
    }

  fixedVolumeType::Pointer resampledImage;
  int actualIterations = 0;
  int allLevelsIterations=0;
  for(unsigned int l=0; l< transformType.size(); l++)
    {
    allLevelsIterations+=numberOfIterations[l];
    const std::string currentTransformType(transformType[l]);
    std::cout << "\n\n\n=============================== Starting Transform Estimations for " 
      << currentTransformType
      << "==============================="
      << std::endl;
    //
    // Break into cases on TransformType:
    //
    if ( currentTransformType == "Rigid" )
      {
      //     if (explicitOrigins) { assert(0==1); }

      //  Choose TransformType for the itk registration class template:
      typedef itk::VersorRigid3DTransform<double>  TransformType;
      typedef itk::VersorRigid3DTransformOptimizer OptimizerType;
      const int NumberOfEstimatedParameter = 6;

      //
      // Process the initialITKTransform as VersorRigid3DTransform:
      //
      TransformType::Pointer initialITKTransform = TransformType::New();
      initialITKTransform->SetIdentity();

      if ( initializeTransformMode != "Off" ) // Use
        // CenteredVersorTranformInitializer
        {
        typedef itk::CenteredVersorTransformInitializer<fixedVolumeType,
                movingVolumeType> InitializerType;
        tp0.Start("CenterInitialization");
        initialITKTransform
          = DoCenteredInitialization<fixedVolumeType, movingVolumeType,
          TransformType, InitializerType>(
            orientedFixedVolume,
            orientedMovingVolume,
            initializeTransformMode);
        tp0.Stop("CenterInitialization");
        }
      else if ( initialTransform.size() > 0 )
        {
        try
          {
          //DELETE transformsList = affineReader->GetTransformList();
          transformsListIterator = transformsList.begin();
          const std::string
            transformFileType = ( *transformsListIterator )->GetNameOfClass();
          if ( transformFileType == "VersorRigid3DTransform" )
            {
            VersorRigid3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<VersorRigid3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else  // || transformFileType == "ScaleVersor3DTransform" ||
            // transformFileType == "ScaleSkewVersor3DTransform" ||
            // transformFileType == "AffineTransform"
            {
            std::cout
              << "Unsupported initial transform file -- TransformBase first transform typestring, "
              << transformFileType
              << " not equal to required type VersorRigid3DTransform"
              << std::endl;
            return 8;
            }
          }
        catch ( itk::ExceptionObject & excp )
          {
          std::cout << "[FAILED]" << std::endl;
          std::cerr << "Error while reading the transformsList" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }
        }

  tp0.Start("Rigid registration");
#include "FitCommonCode.tmpl"
  tp0.Stop("Rigid registration");
      /*
       *  At this point, we are prepared to save results.
       */
      }
    else if ( currentTransformType == "ScaleVersor3D" )
      {
      //  Choose TransformType for the itk registration class template:
      typedef itk::ScaleVersor3DTransform<double> TransformType;
      typedef itk::VersorTransformOptimizer       OptimizerType;
      const int NumberOfEstimatedParameter = 9;

      //
      // Process the initialITKTransform as ScaleVersor3DTransform:
      //
      TransformType::Pointer initialITKTransform = TransformType::New();
      initialITKTransform->SetIdentity();

      if ( initializeTransformMode != "Off" ) // Use
        // CenteredVersorTranformInitializer
        {
        typedef itk::CenteredVersorTransformInitializer<fixedVolumeType,
                movingVolumeType> InitializerType;
        typedef itk::VersorRigid3DTransform<double>
          VersorTransformType;
        VersorTransformType::Pointer tempVersorITKTransform
          = DoCenteredInitialization<fixedVolumeType, movingVolumeType,
          VersorTransformType, InitializerType>(
            orientedFixedVolume,
            orientedMovingVolume,
            initializeTransformMode);
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempVersorITKTransform);
        }
      else if ( initialTransform.size() > 0 )
        {
        try
          {
          //DELETE transformsList = affineReader->GetTransformList();
          transformsListIterator = transformsList.begin();
          const std::string
            transformFileType = ( *transformsListIterator )->GetNameOfClass();
          if ( transformFileType == "VersorRigid3DTransform" )
            {
            VersorRigid3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<VersorRigid3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleVersor3DTransform" )
            {
            ScaleVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else  // || transformFileType == "ScaleSkewVersor3DTransform" ||
            // transformFileType == "AffineTransform"
            {
            std::cout
              << "Unsupported initial transform file -- TransformBase first transform typestring, "
              << transformFileType
              << " not equal to required type VersorRigid3DTransform OR ScaleVersor3DTransform"
              << std::endl;
            return 9;
            }
          }
        catch ( itk::ExceptionObject & excp )
          {
          std::cout << "[FAILED]" << std::endl;
          std::cerr << "Error while reading the transformsList" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }
        }

  tp0.Start("ScaleVersor registration");
#include "FitCommonCode.tmpl"
  tp0.Stop("ScaleVersor registration");
      }
    else if ( currentTransformType == "ScaleSkewVersor3D" )
      {
      //  Choose TransformType for the itk registration class template:
      typedef itk::ScaleSkewVersor3DTransform<double> TransformType;
      typedef itk::VersorTransformOptimizer           OptimizerType;
      const int NumberOfEstimatedParameter = 15;

      //
      // Process the initialITKTransform as ScaleSkewVersor3D:
      //
      TransformType::Pointer initialITKTransform = TransformType::New();
      initialITKTransform->SetIdentity();

      if ( initializeTransformMode != "Off" ) // Use
        // CenteredVersorTranformInitializer
        {
        typedef itk::CenteredVersorTransformInitializer<fixedVolumeType,
                movingVolumeType> InitializerType;
        typedef itk::VersorRigid3DTransform<double>
          VersorTransformType;
        VersorTransformType::Pointer tempVersorITKTransform
          = DoCenteredInitialization<fixedVolumeType, movingVolumeType,
          VersorTransformType, InitializerType>(
            orientedFixedVolume,
            orientedMovingVolume,
            initializeTransformMode);
        AssignRigid::AssignConvertedTransform(initialITKTransform,
          tempVersorITKTransform);
        }
      else if ( initialTransform.size() > 0 )
        {
        try
          {
          //DELETE transformsList = affineReader->GetTransformList();
          transformsListIterator = transformsList.begin();
          const std::string
            transformFileType = ( *transformsListIterator )->GetNameOfClass();
          if ( transformFileType == "VersorRigid3DTransform" )
            {
            VersorRigid3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<VersorRigid3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleVersor3DTransform" )
            {
            ScaleVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleSkewVersor3DTransform" )
            {
            ScaleSkewVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleSkewVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else  // || transformFileType == "AffineTransform" || transformFileType
            // == "ScaleVersor3DTransform"
            {
            std::cout
              << "Unsupported initial transform file -- TransformBase first transform typestring, "
              << transformFileType
              << " not equal to required type VersorRigid3DTransform OR ScaleVersor3DTransform OR ScaleSkewVersor3DTransform"
              << std::endl;
            return 10;
            }
          }
        catch ( itk::ExceptionObject & excp )
          {
          std::cout << "[FAILED]" << std::endl;
          std::cerr << "Error while reading the transformsList" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }
        }
  tp0.Start("ScaleSkewVersor registration");
#include "FitCommonCode.tmpl"
  tp0.Stop("ScaleSkewVersor registration");
      }
    else if ( currentTransformType == "Affine" )
      {
      //  Choose TransformType for the itk registration class template:
      typedef itk::AffineTransform<double, Dimension>  TransformType;
      typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
      const int NumberOfEstimatedParameter = 12;

      //
      // Process the initialITKTransform
      //
      TransformType::Pointer initialITKTransform = TransformType::New();
      initialITKTransform->SetIdentity();

      if ( initializeTransformMode != "Off" ) // Use CenteredTranformInitializer
        {
        typedef itk::CenteredTransformInitializer<TransformType, fixedVolumeType,
                movingVolumeType> InitializerType;
        initialITKTransform
          = DoCenteredInitialization<fixedVolumeType, movingVolumeType,
          TransformType, InitializerType>(
            orientedFixedVolume,
            orientedMovingVolume,
            initializeTransformMode);
        }
      else if ( initialTransform.size() > 0 )
        {
        try
          {
          //DELETE transformsList = affineReader->GetTransformList();
          transformsListIterator = transformsList.begin();
          if ( transformsList.size() == 0 )
            {
            std::cout << "Number of transformsList = " << transformsList.size()
              << std::endl;
            std::cout << "FATAL ERROR: Failed to read transform"
              << initialTransform << std::endl;
            exit(-1);
            }

          const std::string
            transformFileType = ( *transformsListIterator )->GetNameOfClass();
          if ( transformFileType == "VersorRigid3DTransform" )
            {
            VersorRigid3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<VersorRigid3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleVersor3DTransform" )
            {
            ScaleVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleSkewVersor3DTransform" )
            {
            ScaleSkewVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleSkewVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "AffineTransform" )
            {
            AffineTransformType::Pointer tempInitializerITKTransform
              = static_cast<AffineTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else  //  NO SUCH CASE!!
            {
            std::cout
              << "Unsupported initial transform file -- TransformBase first transform typestring, "
              << transformFileType
              << " not equal to any recognized type VersorRigid3DTransform OR ScaleVersor3DTransform OR ScaleSkewVersor3DTransform OR AffineTransform"
              << std::endl;
            return 11;
            }
          }
        catch ( itk::ExceptionObject & excp )
          {
          std::cout << "[FAILED]" << std::endl;
          std::cerr << "Error while reading the transformsList" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }
        }

  tp0.Start("Affine registration");
#include "FitCommonCode.tmpl"
  tp0.Start("Affine registration");
      }
    else if ( currentTransformType == "BSpline" )
      {
      //
      // Process the initialITKTransform for BSpline's BULK
      //
      AffineTransformType::Pointer initialITKTransform = AffineTransformType::New();
      initialITKTransform->SetIdentity();

      if ( initializeTransformMode != "Off" ) // Use CenteredTranformInitializer
        {
        typedef itk::CenteredTransformInitializer<AffineTransformType, fixedVolumeType,
                movingVolumeType> InitializerType;
        initialITKTransform
          = DoCenteredInitialization<fixedVolumeType, movingVolumeType,
          AffineTransformType, InitializerType>(
            orientedFixedVolume,
            orientedMovingVolume,
            initializeTransformMode);
        }
      else if ( initialTransform.size() > 0 )
        {
        try
          {
          //DELETE transformsList = affineReader->GetTransformList();
          transformsListIterator = transformsList.begin();
          if ( transformsList.size() == 0 )
            {
            std::cout << "Number of transformsList = " << transformsList.size()
              << std::endl;
            std::cout << "FATAL ERROR: Failed to read transform"
              << initialTransform << std::endl;
            exit(-1);
            }

          const std::string
            transformFileType = ( *transformsListIterator )->GetNameOfClass();
          if ( transformFileType == "VersorRigid3DTransform" )
            {
            VersorRigid3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<VersorRigid3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleVersor3DTransform" )
            {
            ScaleVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "ScaleSkewVersor3DTransform" )
            {
            ScaleSkewVersor3DTransformType::Pointer tempInitializerITKTransform
              = static_cast<ScaleSkewVersor3DTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else if ( transformFileType == "AffineTransform" )
            {
            AffineTransformType::Pointer tempInitializerITKTransform
              = static_cast<AffineTransformType *>( ( *transformsListIterator ).GetPointer() );
            AssignRigid::AssignConvertedTransform(initialITKTransform,
              tempInitializerITKTransform);
            }
          else  //  Did you initialize the BSpline with a non-BULK transform?
            {
            std::cout
              << "Unsupported BULK transform file -- TransformBase first transform typestring, "
              << transformFileType
              << " not equal to any recognized type VersorRigid3DTransform OR ScaleVersor3DTransform OR ScaleSkewVersor3DTransform OR AffineTransform"
              << std::endl;
            return 11;
            }
          }
        catch ( itk::ExceptionObject & excp )
          {
          std::cout << "[FAILED]" << std::endl;
          std::cerr << "Error while reading the transformsList" << std::endl;
          std::cerr << excp << std::endl;
          return EXIT_FAILURE;
          }
        }
  AffineTransformType::Pointer      bulkAffineTransform = AffineTransformType::New();
  AssignRigid::AssignConvertedTransform(bulkAffineTransform, initialITKTransform);

  typedef itk::Image<float,3>               RegisterImageType;

  BSplineTransformType::Pointer outputBSplineTransform = BSplineTransformType::New();
  outputBSplineTransform->SetIdentity();
    {
      BSplineTransformType::Pointer initializeBSplineTransform = BSplineTransformType::New();
      initializeBSplineTransform->SetIdentity();
      {
      typedef BSplineTransformType::RegionType                TransformRegionType;
      typedef TransformRegionType::SizeType                   TransformSizeType;
      typedef itk::BSplineDeformableTransformInitializer<
        BSplineTransformType,
        RegisterImageType>      InitializerType;
      InitializerType::Pointer transformInitializer = InitializerType::New();
      transformInitializer->SetTransform( initializeBSplineTransform );
      transformInitializer->SetImage( orientedFixedVolume );
      TransformSizeType m_GridSize;
      m_GridSize[0] = splineGridSize[0];
      m_GridSize[1] = splineGridSize[1];
      m_GridSize[2] = splineGridSize[2];
      transformInitializer->SetGridSizeInsideTheImage( m_GridSize );
      transformInitializer->InitializeTransform();
      initializeBSplineTransform->SetBulkTransform(   bulkAffineTransform   );
      }
    const bool UseCachingOfBSplineWeights= ( useCachingOfBSplineWeightsMode == "ON" )?true:false;
    // As recommended in documentation in itkMattesMutualInformationImageToImageMetric.h
    // "UseExplicitPDFDerivatives = False ... This method is well suited
    // for Transforms with a large number of parameters, such as, 
    // BSplineDeformableTransforms."
    const bool UseExplicitPDFDerivatives=  ( useExplicitPDFDerivativesMode == "ON" )?true:false;

    tp0.Start("BSpline registration");
    outputBSplineTransform=DoBSpline<RegisterImageType,ImageMaskType,BSplineTransformType>(
      initializeBSplineTransform,
      orientedFixedVolume,orientedMovingVolume,
      fixedMask, movingMask,
      numberOfSamples,
      UseCachingOfBSplineWeights,UseExplicitPDFDerivatives);
    }
    tp0.Stop("BSpline registration");

      std::cout << outputBSplineTransform << std::endl;

      if (outputTransform.size() > 0)
        {
        WriteTransformWithBulk<BSplineTransformType>(outputBSplineTransform,outputTransform);
        }

      if ( outputVolume.size() > 0 )
        {
        ResampleFilterType::Pointer resampler = ResampleFilterType::New();

        resampler->SetTransform( outputBSplineTransform );
        resampler->SetInput( orientedMovingVolume );         // Remember:  the Data is
        // Moving's, the shape is
        // Fixed's.
        resampler->SetOutputParametersFromImage(orientedFixedVolume);
        resampler->SetDefaultPixelValue( backgroundFillValue );
        if ( useWindowedSinc==true )
          {
          typedef itk::ConstantBoundaryCondition<RegisterImageType>
            BoundaryConditionType;
          static const unsigned int WindowedSincHammingWindowRadius = 5;
          typedef itk::Function::HammingWindowFunction<
            WindowedSincHammingWindowRadius, double, double>
            WindowFunctionType;
          typedef itk::WindowedSincInterpolateImageFunction<
            RegisterImageType,
            WindowedSincHammingWindowRadius,
            WindowFunctionType,
            BoundaryConditionType,
            double>
              WindowedSincInterpolatorType;

          WindowedSincInterpolatorType::Pointer interpolator
            = WindowedSincInterpolatorType::New();
          resampler->SetInterpolator( interpolator );
          }
        else     // Default to LINEAR_INTERP
          {
          // NOTE: Linear is the default.  resampler->SetInterpolator( interpolator );
          }
        resampler->Update();       //  Explicit Update() required here.
        resampledImage = resampler->GetOutput();
        }
      }
    else
      {
      std::cerr
        << "Error choosing what kind of transform to fit "
        << currentTransformType << std::endl;
      return 13;
      }
    }

  tp0.Report();

  /*
   *  At this point we can save the resampled image.
   */

  if ( outputVolume.size() > 0 )
    {
    //      std::cout << "=========== resampledImage :\n" <<
    // resampledImage->GetDirection() << std::endl;
    // Set in PARSEARGS const bool scaleOutputValues=false;//TODO: Make this a
    // command line parameter
    if ( outputVolumePixelType == "float" )
      {
      // itkUtil::WriteCastImage<itk::Image<float, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<float,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
    else if ( outputVolumePixelType == "short" )
      {
      // itkUtil::WriteCastImage<itk::Image<signed short, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<signed short,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
    else if ( outputVolumePixelType == "ushort" )
      {
      // itkUtil::WriteCastImage<itk::Image<unsigned short, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<unsigned short,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
    else if ( outputVolumePixelType == "int" )
      {
      // itkUtil::WriteCastImage<itk::Image<signed int, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<signed int,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
    else if ( outputVolumePixelType == "uint" )
      {
      // itkUtil::WriteCastImage<itk::Image<unsigned int, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<unsigned int,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
#if 0
    else if ( outputVolumePixelType == "char" )
      {
      // itkUtil::WriteCastImage<itk::Image<signed char, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<char,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
#endif
    else if ( outputVolumePixelType == "uchar" )
      {
      // itkUtil::WriteCastImage<itk::Image<unsigned char, fixedVolumeType::ImageDimension>, fixedVolumeType>(resampledImage,outputVolume);
      typedef itk::Image<unsigned char,
        fixedVolumeType::ImageDimension> WriteOutImageType;
      WriteOutImageType::Pointer CastImage
        = ( scaleOutputValues == true ) ?
          ( itkUtil::PreserveCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) ) :
          ( itkUtil::TypeCast<fixedVolumeType,
             WriteOutImageType>(resampledImage) );
      itkUtil::WriteImage<WriteOutImageType>(CastImage, outputVolume);
      }
    }

  if ( actualIterations + 1 >= allLevelsIterations )
    {
    return failureExitCode;
    }
  return 0;
}
