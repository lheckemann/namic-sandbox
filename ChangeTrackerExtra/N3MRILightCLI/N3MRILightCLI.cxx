#include "N3MRILightCLICLP.h"

#include "itkBSplineControlPointImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkN3MRIBiasFieldCorrectionImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkShrinkImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"



int main( int argc, char *argv[] )
{
  PARSE_ARGS;

  // TODO: handle properly various image types
  typedef float RealType;

  typedef itk::OrientedImage<RealType, 3> ImageType;
  typedef itk::OrientedImage<unsigned char, 3> MaskImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImage );
  reader->Update();

  typedef itk::ShrinkImageFilter<ImageType, ImageType> ShrinkerType;
  ShrinkerType::Pointer shrinker = ShrinkerType::New();
  shrinker->SetInput( reader->GetOutput() );
  shrinker->SetShrinkFactors( shrinkFactor );

  MaskImageType::Pointer maskImage = NULL;

  // print out arguments
  std::cout << " === IO ===" << std::endl 
            << "Input image name: " << inputImage << std::endl 
            << "Mask image name: " << maskImageName << std::endl 
            << "Output image name: " << outputImage << std::endl 
            << "Output bias field: " << outputBiasField << std::endl 
            << " === Parameters ===" << std::endl 
            << "Shrink factor: " << shrinkFactor << std::endl 
            << "Number of fitting levels: " << nFittingLevels << std::endl 
            << "Number of iterations: " << nIterations << std::endl 
            << "Number of histogram bins: " << nHistogramBins << std::endl 
            << "Weiner filter noise: " << weinerFilterNoise << std::endl 
            << "Bias field full width at half maximum: " << biasFieldFullWidthAtHalfMaximum << std::endl 
            << "Convergence threshold: " << convergenceThreshold << std::endl 
            << "Spline order: " << splineOrder << std::endl 
            << "Number of control points: " << nControlPoints[0] << "," << nControlPoints[1] 
            << "," << nControlPoints[2] << std::endl << std::endl;

  if( maskImageName != "")
    {
    typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
    typedef itk::BinaryThresholdImageFilter<MaskImageType,MaskImageType> ThresholdType;
    MaskReaderType::Pointer maskreader = MaskReaderType::New();
    maskreader->SetFileName( maskImageName );

    try
      {
      maskreader->Update();
      maskImage = maskreader->GetOutput();
      }
    catch(itk::ExceptionObject &e)
      {
      std::cout << "Mask file not read.  Generating mask file using otsu"
        << " thresholding. Exception: " << e << std::endl;
      }

    ThresholdType::Pointer thresh = ThresholdType::New();
    thresh->SetInput(maskreader->GetOutput());
    thresh->SetInsideValue(1);
    thresh->SetOutsideValue(0);
    thresh->SetLowerThreshold(1);
    thresh->SetUpperThreshold(255);
    thresh->Update();

    maskImage = thresh->GetOutput();
    }
  if( maskImageName == "")
    {
    typedef itk::OtsuThresholdImageFilter<ImageType, MaskImageType>
      ThresholderType;
    ThresholderType::Pointer otsu = ThresholderType::New();
    otsu->SetInput( reader->GetOutput() );
    otsu->SetNumberOfHistogramBins( nHistogramBins );
    otsu->SetInsideValue( 0 );
    otsu->SetOutsideValue( 1 );
    otsu->Update();

    maskImage = otsu->GetOutput();
    }
  typedef itk::ShrinkImageFilter<MaskImageType, MaskImageType> MaskShrinkerType;
  MaskShrinkerType::Pointer maskshrinker = MaskShrinkerType::New();
  maskshrinker->SetInput( maskImage );
  maskshrinker->SetShrinkFactors( shrinkFactor );

  shrinker->Update();
  maskshrinker->Update();

  typedef itk::N3MRIBiasFieldCorrectionImageFilter<ImageType, MaskImageType,
    ImageType> CorrecterType;
  CorrecterType::Pointer correcter = CorrecterType::New();
  correcter->SetInput( shrinker->GetOutput() );
  correcter->SetMaskImage( maskshrinker->GetOutput() );

  correcter->SetWeinerFilterNoise(weinerFilterNoise);
  correcter->SetBiasFieldFullWidthAtHalfMaximum(biasFieldFullWidthAtHalfMaximum);
  correcter->SetConvergenceThreshold(convergenceThreshold);
  correcter->SetSplineOrder(splineOrder);

  CorrecterType::ArrayType nCP;
  nCP[0] = nControlPoints[0];
  nCP[1] = nControlPoints[1];
  nCP[2] = nControlPoints[2];
  correcter->SetNumberOfControlPoints(nCP);

  correcter->SetMaximumNumberOfIterations( nIterations );
  correcter->SetNumberOfFittingLevels( nFittingLevels );

  try
    {
    correcter->Update();
    }
  catch(itk::ExceptionObject e)
    {
    std::cerr << "Exception caught while performing bias correction: " << e << std::endl;
    return EXIT_FAILURE;
    }

//  correcter->Print( std::cout, 3 );

  /**
   * Reconstruct the bias field at full image resolution.  Divide
   * the original input image by the bias field to get the final
   * corrected image.
   */
  typedef itk::BSplineControlPointImageFilter<
    CorrecterType::BiasFieldControlPointLatticeType,
    CorrecterType::ScalarImageType> BSplinerType;
  BSplinerType::Pointer bspliner = BSplinerType::New();
  bspliner->SetInput( correcter->GetLogBiasFieldControlPointLattice() );
  bspliner->SetSplineOrder( correcter->GetSplineOrder() );
  bspliner->SetSize(
    reader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  bspliner->SetOrigin( reader->GetOutput()->GetOrigin() );
  bspliner->SetDirection( reader->GetOutput()->GetDirection() );
  bspliner->SetSpacing( reader->GetOutput()->GetSpacing() );
  bspliner->Update();

  ImageType::Pointer logField = ImageType::New();
  logField->SetOrigin( bspliner->GetOutput()->GetOrigin() );
  logField->SetSpacing( bspliner->GetOutput()->GetSpacing() );
  logField->SetRegions(
    bspliner->GetOutput()->GetLargestPossibleRegion().GetSize() );
  logField->SetDirection( bspliner->GetOutput()->GetDirection() );
  logField->Allocate();

  itk::ImageRegionIterator<CorrecterType::ScalarImageType> ItB(
    bspliner->GetOutput(),
    bspliner->GetOutput()->GetLargestPossibleRegion() );
  itk::ImageRegionIterator<ImageType> ItF( logField,
    logField->GetLargestPossibleRegion() );
  for( ItB.GoToBegin(), ItF.GoToBegin(); !ItB.IsAtEnd(); ++ItB, ++ItF )
    {
    ItF.Set( ItB.Get()[0] );
    }

  typedef itk::ExpImageFilter<ImageType, ImageType> ExpFilterType;
  ExpFilterType::Pointer expFilter = ExpFilterType::New();
  expFilter->SetInput( logField );
  expFilter->Update();

  typedef itk::DivideImageFilter<ImageType, ImageType, ImageType> DividerType;
  DividerType::Pointer divider = DividerType::New();
  divider->SetInput1( reader->GetOutput() );
  divider->SetInput2( expFilter->GetOutput() );
  divider->Update();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputImage );
  writer->SetInput( divider->GetOutput() );
  writer->Update();

  if( outputBiasField != "")
    {
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputBiasField );
    writer->SetInput( expFilter->GetOutput() );
    writer->Update();
    }

  return EXIT_SUCCESS;
}

