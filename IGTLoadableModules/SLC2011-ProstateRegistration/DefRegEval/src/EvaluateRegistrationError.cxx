#include "DefRegEval.h"

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkSubtractImageFilter.h"
#include "itkGradientToMagnitudeImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleFunctionErodeImageFilter.h"
#include "itkThresholdImageFilter.h"


int main( int argc, char * argv[] )
{

  std::string deformationReferenceFilename = "deformationRef.mha";
  std::string deformationComputedFilename = "deformationComputed.mha";
  std::string maskFilename;
  std::string deformationDiffVectorFilename;
  std::string deformationDiffMagFilename;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--deformationReference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deformationReferenceFilename, "Ground truth deformation field image");
  args.AddArgument("--deformationComputed", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deformationComputedFilename, "Computed deformation field image");

  args.AddArgument("--mask", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maskFilename, "Binary mask that will be applied on the output (all voxels where the mask is 0 will be 0)");  
  args.AddArgument("--deformationDifferenceVector", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deformationDiffVectorFilename, "Deformation field error (computed-ref)");
  args.AddArgument("--deformationDifferenceMagnitude", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deformationDiffMagFilename, "Magnitude of deformation field error (computed-ref)");


  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  static const unsigned int  ImageDimension = 3;
  typedef float  InternalPixelType;
  typedef unsigned char  ExternalPixelType;
  typedef itk::Vector<InternalPixelType, ImageDimension>    VectorPixelType;
  typedef itk::Image<VectorPixelType,  ImageDimension> DeformationFieldType;
  typedef itk::Image <InternalPixelType, ImageDimension> OutputImageType;

  typedef itk::ImageFileReader< DeformationFieldType  > DefImageReaderType;

  DefImageReaderType::Pointer defRefReader = DefImageReaderType::New();
  defRefReader->SetFileName( deformationReferenceFilename );
  try
  {
    defRefReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  DefImageReaderType::Pointer defCompReader = DefImageReaderType::New();
  defCompReader->SetFileName( deformationComputedFilename );
  try
  {
    defCompReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  // Compute the difference between the reference and computed deformation field
  typedef itk::SubtractImageFilter<DeformationFieldType, DeformationFieldType, DeformationFieldType> SubtractFilterType;
  SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();
  subtractFilter->SetInput1(defCompReader->GetOutput());
  subtractFilter->SetInput2(defRefReader->GetOutput());
  subtractFilter->Update();
  DeformationFieldType::Pointer diffImage=subtractFilter->GetOutput();

  if (!maskFilename.empty())
  {
    typedef itk::ImageFileReader< OutputImageType  > MaskImageReaderType;
    MaskImageReaderType::Pointer maskReader = MaskImageReaderType::New();
    maskReader->SetFileName( maskFilename );
    try
    {
      maskReader->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }

    // Remove border pixels of the mask using morphological eroding+thresholding (to make the mask somehat smaller)
    // It is necessary because there could be a outlier voxels at the border (e.g., deformation info is available
    // from FEM, but not from registration)

    typedef itk::BinaryBallStructuringElement<InternalPixelType, ImageDimension> KernelType;
    typedef itk::GrayscaleFunctionErodeImageFilter<OutputImageType, OutputImageType, KernelType> ErodeFilterType;

    // Erode
    ErodeFilterType::Pointer erodeFilter = ErodeFilterType::New();
    KernelType ball;
    KernelType::SizeType ballSize;
    OutputImageType::Pointer maskImage= maskReader->GetOutput();    
    ballSize[0] = maskImage->GetSpacing()[0]/2.0;
    ballSize[1] = maskImage->GetSpacing()[1]/2.0;
    ballSize[2] = maskImage->GetSpacing()[2]/2.0;
    ball.SetRadius(ballSize);
    ball.CreateStructuringElement();
    erodeFilter->SetInput( maskReader->GetOutput() );
    erodeFilter->SetKernel( ball );
    erodeFilter->Update();

    // Threshold
    typedef itk::ThresholdImageFilter<OutputImageType> ThresholdFilterType;
    ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();    
    thresholdFilter->SetInput(erodeFilter->GetOutput());

    erodeFilter->SetInput( maskReader->GetOutput() );

    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->ThresholdBelow(0);

    // Mask
    typedef itk::MaskImageFilter< DeformationFieldType, OutputImageType > MaskFilterType;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();    
    maskFilter->SetInput1(subtractFilter->GetOutput());
    maskFilter->SetInput2(thresholdFilter->GetOutput());
    maskFilter->Update();
    diffImage=maskFilter->GetOutput();
  }

  if (!deformationDiffVectorFilename.empty())
  {
    typedef itk::ImageFileWriter< DeformationFieldType  > DefImageWriterType;
    DefImageWriterType::Pointer defDiffWriter = DefImageWriterType::New();
    defDiffWriter->SetFileName( deformationDiffVectorFilename );
    defDiffWriter->SetInput(diffImage);
    try
    {
      defDiffWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (!deformationDiffMagFilename.empty())
  {
    typedef itk::GradientToMagnitudeImageFilter< DeformationFieldType, OutputImageType > MagnitudeFilterType;
    MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();
    magnitudeFilter->SetInput(diffImage);

    typedef itk::ImageFileWriter< OutputImageType  > DefImageMagWriterType;
    DefImageMagWriterType::Pointer defDiffMagWriter = DefImageMagWriterType::New();
    defDiffMagWriter->SetFileName( deformationDiffMagFilename );
    defDiffMagWriter->SetInput(magnitudeFilter->GetOutput());
    try
    {
      defDiffMagWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
