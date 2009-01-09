#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkBilateralImageFilter.h>
#include <itkDifferenceImageFilter.h>
#include <itkTimeProbesCollectorBase.h>
#include "itkBilateralZThreadImageFilter.h"
#include <itkRandomImageSource.h>


int main ( int argc, const char* argv[] ) 
{
  itk::OStringStream msg;

  typedef itk::Image<short, 3> Volume;
  typedef itk::RandomImageSource<Volume> RIS;

  RIS::Pointer ris = RIS::New();
  unsigned long int size[3] = {128, 128, 128};
  ris->SetSize ( size );

  itk::TimeProbesCollectorBase probe;

  typedef itk::BilateralImageFilter<Volume,Volume> BilateralType;
  BilateralType::Pointer bilateral = BilateralType::New();
  bilateral->SetDomainSigma ( 0.5 );
  bilateral->SetRangeSigma ( 50.0 );
  bilateral->SetInput ( ris->GetOutput() );

  probe.Start ( "Original Bilateral" );
  bilateral->Update();
  probe.Stop ( "Original Bilateral" );

  // Process here
  typedef itk::BilateralZThreadImageFilter<Volume,Volume> BilateralZThreadType;
  BilateralZThreadType::Pointer bilateralZ = BilateralZThreadType::New();
  bilateralZ->SetDomainSigma ( 0.5 );
  bilateralZ->SetRangeSigma ( 50.0 );
  bilateralZ->SetInput ( ris->GetOutput() );
  probe.Start ( "MultiZThreader" );
  bilateralZ->Update();
  probe.Stop ( "MultiZThreader" );

  probe.Report();

  // Now compare the two images
  typedef itk::DifferenceImageFilter<Volume,Volume> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(bilateral->GetOutput());
  diff->SetTestInput(bilateralZ->GetOutput());
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

  std::cout << "\nTesting with more jobs" << std::endl;
  for ( int jobs = 1; jobs <= 10; jobs++ ) 
    {
    msg.str(""); msg << "Jobs == " << jobs;
    probe.Start ( msg.str().c_str() );
    bilateralZ->GetMultiZThreader()->SetNumberOfJobs ( jobs );
    bilateralZ->Modified();
    try
      {
      bilateralZ->Update();
      }
    catch ( itk::ExceptionObject &e )
      {
      std::cout << "Caught ITK exception " << e.what() << std::endl;
      return EXIT_FAILURE;
      }
    catch (...)
      {
      std::cout << "Something or the other went wrong" << std::endl;
      return EXIT_FAILURE;
      }
    probe.Stop ( msg.str().c_str() );

    diff->UpdateLargestPossibleRegion();
    std::cout << "\t" << jobs << " jobs, difference: " << diff->GetTotalDifference() << std::endl;
    if ( diff->GetTotalDifference() > 0.1 )
      {
      std::cerr << "Difference is too great, returning error" << std::endl;
      return EXIT_FAILURE;
      }
    }

  probe.Report();

  return EXIT_SUCCESS;
}
