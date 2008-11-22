#include "itkImageNetworkReader.h"


int main ( int argc, const char* argv[] )
{

  std::cout << "Starting ImageNetworkReaderTest" << std::endl;

  typedef itk::Image<short, 3> ImageType;
  typedef itk::ImageNetworkReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetURI ( argv[1] );
  // reader->SetDataExtention ( "raw" );
  reader->GenerateOutputInformation();

  reader->Update();
  std::cout << "Information: \n";
  reader->GetOutput()->Print ( std::cout );

  std::cout << "Ending ImageNetworkReaderTest" << std::endl;
  return ( EXIT_SUCCESS );

}
