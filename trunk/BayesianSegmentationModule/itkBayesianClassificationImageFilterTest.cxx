#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBayesianClassificationImageFilter.h"


int main(int argc, char *argv[])
{
  const unsigned int Dimension = 2;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage ClassifiedImage numberOfClasses numberOfSmoothingIterations [Optional: MaskImage MaskValue]" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Image< unsigned char, Dimension > InputImageType;
  typedef itk::Image< unsigned char, Dimension > MaskImageType;
  typedef itk::Image< unsigned char, Dimension > LabelImageType;

  typedef itk::BayesianClassificationImageFilter<
      InputImageType, MaskImageType, LabelImageType > ClassifierType;

  ClassifierType::Pointer filter = ClassifierType::New();
  filter->SetNumberOfClasses( atoi(argv[3]) );
  filter->SetNumberOfSmoothingIterations(atoi(argv[4]));
    
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    std::cerr << "Successfully read the input image!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  filter->SetInput(reader->GetOutput());

  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  if( argc == 7 )
    {
    maskReader->SetFileName( argv[5] );

    try
      {
      maskReader->Update();
      std::cerr << "Successfully read the mask image!" << std::endl;
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }
  
    filter->SetMaskImage( maskReader->GetOutput() );
    filter->SetMaskValue( atoi(argv[6]) );
    }

  typedef itk::ImageFileWriter< 
    ClassifierType::OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName( argv[2] );
  
  try
    {
    filter->Update();
    std::cerr << "Successfully executed the filter!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  try
    {
    writer->Update();
    std::cerr << "Successfully wrote the output image!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

