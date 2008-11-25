#include "itkImageNetworkReader.h"


int main ( int argc, const char* argv[] )
{

  std::cout << "Starting ImageNetworkReaderTest" << std::endl;

  typedef itk::Image<short, 3> ImageType;
  typedef itk::ImageNetworkReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetURI ( argv[1] );
  reader->GenerateOutputInformation();
  reader->GetOutput()->Print ( std::cout );

  std::cout << "Updating" << std::endl;

  reader->Update();
  std::cout << "Information: \n";
  reader->GetOutput()->Print ( std::cout );

  std::cout << "Ending ImageNetworkReaderTest" << std::endl;
  return ( EXIT_SUCCESS );

}
