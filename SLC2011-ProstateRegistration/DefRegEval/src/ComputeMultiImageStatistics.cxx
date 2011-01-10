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
#include "itkImageDuplicator.h"
#include "itkMaximumImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkAddImageFilter.h"

void readTextLineToListOfString(const char* textFileName, std::vector< std::string > &listOfStrings)
{
  /* The file MUST end with an empty line, then each line will be
  stored as an element in the returned vector object. */

  std::ifstream f(textFileName);
  std::string thisLine;

  if (f.good())
  {
    while( std::getline(f, thisLine) )
    {
      listOfStrings.push_back(thisLine);
    }
  }
  else
  {
    std::cerr<<"Error: can not open file:"<<textFileName<<std::endl;
    exit(EXIT_FAILURE);
  }

  f.close();

} 

int main( int argc, char * argv[] )
{

  std::string fileListFilename;
  std::string meanImageFilename;
  std::string maxImageFilename;
  bool vectorImageInput=false;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--fileList", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fileListFilename, "Text file with the list of files to be analysed");
  args.AddArgument("--meanImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &meanImageFilename, "Mean image output");
  args.AddArgument("--maxImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxImageFilename, "Max image output");
  args.AddBooleanArgument("--vectorInput", &vectorImageInput, "Set it to true for processing vector images");
  
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector< std::string > listOfStrings;
  readTextLineToListOfString(fileListFilename.c_str(), listOfStrings);

  const unsigned int  ImageDimension = 3;

  typedef float  ScalarPixelType;
  typedef itk::Image< ScalarPixelType, ImageDimension > ScalarImageType;
  typedef itk::ImageFileReader< ScalarImageType > ScalarImageReaderType;

  typedef itk::Vector< ScalarPixelType, ImageDimension > VectorPixelType;
  typedef itk::Image< VectorPixelType,  ImageDimension > VectorImageType;
  typedef itk::ImageFileReader< VectorImageType > VectorImageReaderType;


  ScalarImageType::Pointer meanAccumulatorImage;
  ScalarImageType::Pointer maxAccumulatorImage;
  
  ScalarImageType::Pointer readImage;

  ScalarImageReaderType::Pointer scalarImageReader = ScalarImageReaderType::New();
  VectorImageReaderType::Pointer vectorImageReader = VectorImageReaderType::New();

  typedef itk::GradientToMagnitudeImageFilter< VectorImageType, ScalarImageType > MagnitudeFilterType;
  MagnitudeFilterType::Pointer magnitudeFilter = MagnitudeFilterType::New();

  int numberOfImages=0;
  for (std::vector< std::string >::iterator it=listOfStrings.begin(); it!=listOfStrings.end(); ++it)
  {
    numberOfImages++;
    if (!vectorImageInput)
    {
      scalarImageReader->SetFileName( *it );
      try
      {
        scalarImageReader->Update();
      }
      catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
      }
      scalarImageReader->Update(); 
      readImage=scalarImageReader->GetOutput();
    }
    else
    {
      vectorImageReader->SetFileName( *it );
      try
      {
        vectorImageReader->Update();
      }
      catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
      }
      magnitudeFilter->SetInput(vectorImageReader->GetOutput());
      magnitudeFilter->Update(); 
      readImage=magnitudeFilter->GetOutput();
    }

    if (meanAccumulatorImage.IsNull())
    {
      typedef itk::ImageDuplicator< ScalarImageType >   DuplicatorType;
      DuplicatorType::Pointer duplicator  = DuplicatorType::New();
      duplicator->SetInputImage( readImage );      
      duplicator->Update();
      meanAccumulatorImage = duplicator->GetOutput();
    }
    else
    {
      typedef itk::ImageDuplicator< ScalarImageType >   DuplicatorType;
      DuplicatorType::Pointer duplicator  = DuplicatorType::New();
      duplicator->SetInputImage( meanAccumulatorImage );
      duplicator->Update();
      typedef itk::AddImageFilter<ScalarImageType,ScalarImageType,ScalarImageType> AddFilterType;
      AddFilterType::Pointer addFilter = AddFilterType::New();
      addFilter->SetInput1(duplicator->GetOutput());
      addFilter->SetInput2(readImage);
      addFilter->Update();
      meanAccumulatorImage = addFilter->GetOutput();
    }

    if (maxAccumulatorImage.IsNull())
    {
      typedef itk::ImageDuplicator< ScalarImageType >   DuplicatorType;
      DuplicatorType::Pointer duplicator  = DuplicatorType::New();
      duplicator->SetInputImage( readImage );
      duplicator->Update();
      maxAccumulatorImage = duplicator->GetOutput();
    }
    else
    {
      typedef itk::ImageDuplicator< ScalarImageType >   DuplicatorType;
      DuplicatorType::Pointer duplicator  = DuplicatorType::New();
      duplicator->SetInputImage( maxAccumulatorImage );
      duplicator->Update();
      typedef itk::MaximumImageFilter< ScalarImageType, ScalarImageType, ScalarImageType  > MaxFilterType;
      MaxFilterType::Pointer maxFilter = MaxFilterType::New();
      maxFilter->SetInput1(duplicator->GetOutput());
      maxFilter->SetInput2(readImage);
      maxFilter->Update();
      maxAccumulatorImage = maxFilter->GetOutput();
    }

  }

  typedef itk::ShiftScaleImageFilter<ScalarImageType, ScalarImageType> DivideFilterType;
  DivideFilterType::Pointer divider = DivideFilterType::New();
  divider->SetInput(meanAccumulatorImage);
  double scaleFactor=1.0/numberOfImages;
  divider->SetScale(scaleFactor);
  divider->SetShift(0);
  divider->Update();

  if (!meanImageFilename.empty())
  {

    typedef itk::ImageFileWriter< ScalarImageType  > meanWriterType;
    meanWriterType::Pointer meanWriter = meanWriterType::New();
    meanWriter->SetFileName( meanImageFilename );
    meanWriter->SetInput(divider->GetOutput());
    try
    {
      meanWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (!maxImageFilename.empty())
  {

    typedef itk::ImageFileWriter< ScalarImageType  > maxWriterType;
    maxWriterType::Pointer maxWriter = maxWriterType::New();
    maxWriter->SetFileName( maxImageFilename );
    maxWriter->SetInput(maxAccumulatorImage);
    try
    {
      maxWriter->Update();
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
