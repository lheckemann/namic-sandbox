#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkGDCMImageIO.h>
#include <itkImageSeriesReader.h>
#include <itkLogger.h>
#include <itkLoggerBase.h>
#include <itkStdStreamLogOutput.h>
#include <itkSimpleFilterWatcher.h>
#include <itkBilateralImageFilter.h>
#include <itkTimeProbesCollectorBase.h>
#include "itkBilateralZThreadImageFilter.h"
#include "itkMedianZThreadImageFilter.h"
#include <itkMedianImageFilter.h>
#include <itkRandomImageSource.h>


int main ( int argc, const char* argv[] ) 
{
  itk::OStringStream msg;
  itk::StdStreamLogOutput::Pointer coutput = itk::StdStreamLogOutput::New();
  coutput->SetStream(std::cout);
  itk::Logger::Pointer logger = itk::Logger::New();
  logger->SetName ( "ThreadIT" );
  logger->AddLogOutput ( coutput );

  typedef itk::Image<short, 3> Volume;
  typedef itk::RandomImageSource<Volume> RIS;

  RIS::Pointer ris = RIS::New();
  unsigned long int size[3] = {64, 64, 64};
  ris->SetSize ( size );

  itk::TimeProbesCollectorBase probe;

  // Process here
  typedef itk::BilateralZThreadImageFilter<Volume,Volume> BilateralZThreadType;
  BilateralZThreadType::Pointer bilateralZ = BilateralZThreadType::New();
  bilateralZ->SetDomainSigma ( 0.1 );
  bilateralZ->SetRangeSigma ( 50.0 );
  bilateralZ->SetInput ( ris->GetOutput() );

  std::cout << "Starting bilateralZ" << std::endl;
  bilateralZ->Update();
  typedef itk::MedianZThreadImageFilter<Volume,Volume> MedianZThreadType;

  MedianZThreadType::InputSizeType sz;
  sz[0] = 3;
  sz[1] = 3;
  sz[2] = 3;

  ZThread::PoolExecutor executor((size_t)bilateralZ->GetMultiThreader()->GetNumberOfThreads());
  char buffer[128];
  for ( int i = 1; i < bilateralZ->GetMultiThreader()->GetNumberOfThreads() * 2; i++ )
    {
    MedianZThreadType::Pointer medianZ = MedianZThreadType::New();
    medianZ->executor = &executor;
    medianZ->SetRadius ( sz );
    medianZ->SetInput ( bilateralZ->GetOutput() );
    medianZ->SetNumberOfPieces ( i );

    sprintf ( buffer, "MedianZ %02d Chunk", i );
    probe.Start ( buffer );
    // itk::SimpleFilterWatcher watcher ( medianZ, "MedianZ" );
    medianZ->Update();
    probe.Stop ( buffer );
    std::cout << "Finished: " << buffer << std::endl;
    
    typedef itk::MedianImageFilter<Volume,Volume> MedianType;
    MedianType::Pointer median = MedianType::New();
    median->SetRadius ( sz );
    median->SetInput ( bilateralZ->GetOutput() );
    median->GetMultiThreader()->SetNumberOfThreads ( i );
    
    sprintf ( buffer, "Median %02d Threads", i );
    probe.Start ( buffer );
    // itk::SimpleFilterWatcher watcher2 ( median, "Median" );
    median->Update();
    probe.Stop ( buffer );
    }

  probe.Report();

  return EXIT_SUCCESS;

}
