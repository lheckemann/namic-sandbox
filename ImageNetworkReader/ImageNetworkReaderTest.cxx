#include "itkImageNetworkReader.h"
#ifdef TEST_CURL_SSL
#include <curl/curl.h>
#endif


int main ( int argc, const char* argv[] )
{

#ifdef TEST_CURL_SSL
  curl_global_init ( CURL_GLOBAL_ALL );
  CURL* handle = curl_easy_init();
  curl_easy_setopt ( handle, CURLOPT_URL, "https://ssl.fujitsu-general.com/" );
  // Don't verify the certificate, not recmmonded...
  curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
  CURLcode retval = curl_easy_perform ( handle );
  if ( retval != CURLE_OK ) 
    {
    std::cout << "Error in curl: " << curl_easy_strerror ( retval ) << std::endl;
    }
  curl_easy_cleanup ( handle );
#endif


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
