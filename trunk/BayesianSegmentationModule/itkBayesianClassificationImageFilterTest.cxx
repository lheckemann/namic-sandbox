#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBayesianClassificationImageFilter.h"


int main(int argc, char *argv[])
{
  const unsigned int Dimension = 2;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage ClassifiedImage numberOfClasses numberOfSmoothingIterations" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Image< unsigned char, Dimension > InputImageType;
  typedef itk::Image< unsigned char, Dimension > LabelImageType;

  typedef itk::BayesianClassificationImageFilter<
      InputImageType, LabelImageType > ClassifierType;

  ClassifierType::Pointer filter = ClassifierType::New();
  filter->SetNumberOfClasses( atoi(argv[3]) );
  filter->SetNumberOfSmoothingIterations(atoi(argv[4]));
    
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  filter->SetInput(reader->GetOutput());
  
  typedef itk::ImageFileWriter< 
    ClassifierType::OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName( argv[2] );
  
  try
    {
    filter->Update();
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
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

