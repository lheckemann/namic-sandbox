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
#include <itkDifferenceImageFilter.h>
#include <itkTimeProbesCollectorBase.h>
#include "itkBilateralZThreadImageFilter.h"
#include "itkBilateralZThreadImageFilter2.h"
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
  unsigned long int size[3] = {128, 128, 128};
  ris->SetSize ( size );

  itk::TimeProbesCollectorBase probe;

  typedef itk::BilateralImageFilter<Volume,Volume> BilateralType;
  BilateralType::Pointer bilateral = BilateralType::New();
  bilateral->SetDomainSigma ( 0.1 );
  bilateral->SetRangeSigma ( 50.0 );
  bilateral->SetInput ( ris->GetOutput() );

  probe.Start ( "Original Bilateral" );
  bilateral->Update();
  probe.Stop ( "Original Bilateral" );

  // Process here
  typedef itk::BilateralZThreadImageFilter2<Volume,Volume> BilateralZThreadType2;
  BilateralZThreadType2::Pointer bilateralZ2 = BilateralZThreadType2::New();
  bilateralZ2->SetDomainSigma ( 0.1 );
  bilateralZ2->SetRangeSigma ( 50.0 );
  bilateralZ2->SetInput ( ris->GetOutput() );
  probe.Start ( "MultiZThreader" );
  bilateralZ2->Update();
  probe.Stop ( "MultiZThreader" );

  probe.Report();

  // Now compare the two images
  typedef itk::DifferenceImageFilter<Volume,Volume> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(bilateral->GetOutput());
  diff->SetTestInput(bilateralZ2->GetOutput());
  diff->SetDifferenceThreshold(2.0);
  diff->UpdateLargestPossibleRegion();

  double status = diff->GetTotalDifference();

  std::cout << "\nDifference between implementations" << std::endl;
  std::cout << "\tTotal difference:           " << diff->GetTotalDifference() << std::endl;
  std::cout << "\tMean difference:            " << diff->GetMeanDifference() << std::endl;
  std::cout << "\tNumber of different pixels: " << diff->GetNumberOfPixelsWithDifferences() << std::endl;

  if ( status > 0.1 )
    {
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
