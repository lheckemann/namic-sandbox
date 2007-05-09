#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileWriter.h"

int main(int argc, char *argv[])
{
  typedef short VoxelType;
  const unsigned int Dimension = 3;
  if( argc < 2 )
    {
    std::cerr << "Usage arguments: InputImage OutputImage " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Image< VoxelType, Dimension > InputImageType;
  typedef itk::Image< unsigned char, Dimension > MaskImageType;

  typedef itk::BinaryThresholdImageFilter<
      InputImageType, MaskImageType > FilterType;

  FilterType::Pointer filter = FilterType::New();
    
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
  filter->SetOutsideValue( 0 );
  filter->SetInsideValue( 1 );
  filter->SetLowerThreshold( 40 );

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
  
  //
  // Setup writer.. Rescale the label map to the dynamic range of the 
  // datatype and write it 
  //
  typedef itk::ImageFileWriter< MaskImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  
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

