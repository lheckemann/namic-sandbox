#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkPNGImageIO.h"

int main( int argc, char * argv [] )
{

  if( argc < 1 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: OutputFileName" << std::endl;
    return -1;
    }

  /* Reading an image series from file...*/
  std::cerr << "Reading image series from file..." << std::endl;
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageSeriesReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  typedef itk::NumericSeriesFileNames NameGeneratorType;
  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  nameGenerator->SetSeriesFormat( "raw%03d.png" );
  nameGenerator->SetStartIndex( 0 );
  nameGenerator->SetEndIndex( 231 );
  nameGenerator->SetIncrementIndex( 1 );
  reader->SetImageIO( itk::PNGImageIO::New() );
  reader->SetFileNames( nameGenerator->GetFileNames() );

  /* Writing an image to file */
  std::cerr << "Writing image to file..." << std::endl;
  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[1] );
  writer->SetInput( reader->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
