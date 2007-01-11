// A test of the FastSweepingImageFilter
// by John Melonakos

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkFastSweepingImageFilterTest.h"
#include "itkFastSweepingImageFilter.h"

int main( int argc, char *argv[] )
{
  std::cerr << "#####################################" << std::endl;
  std::cerr << "## FAST SWEEPING IMAGE FILTER TEST ##" << std::endl;
  std::cerr << "#####################################" << std::endl;
  std::cerr << std::endl;

  if(argc < 1)
    {
    std::cerr << "Usage: " << argv[0] << " NrrdFileName(.nhdr)" << std::endl;
    return EXIT_FAILURE;
    }
  
  /* Data Input */
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  /* Run Fast Sweeping */
  typedef itk::FastSweepingImageFilter< InputImageType, ArrivalTimesImageType >
    FastSweepingFilterType;
  FastSweepingFilterType::Pointer fastSweepingFilter = FastSweepingFilterType::New();
  fastSweepingFilter->SetInput( reader->GetOutput() );
  fastSweepingFilter->Update();
  
  /* Write Results to File */
  ArrivalTimesWriterType::Pointer atWriter = ArrivalTimesWriterType::New();
  atWriter->SetInput( fastSweepingFilter->GetArrivalTimes() );
  atWriter->SetFileName( "arrivalTimes.nhdr" );
  try
    {
    atWriter->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ArrivalVectorsWriterType::Pointer avWriter = ArrivalVectorsWriterType::New();
  avWriter->SetInput( fastSweepingFilter->GetArrivalVectors() );
  avWriter->SetFileName( "arrivalVectors.nhdr" );
  try
    {
    avWriter->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
