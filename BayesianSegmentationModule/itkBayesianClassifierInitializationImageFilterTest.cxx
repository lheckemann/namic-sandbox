#include "itkImage.h"
#include "itkBayesianClassifierInitializationImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

int main(int argc, char *argv[])
{

  const unsigned int Dimension = 2;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage MembershipImage numberOfClasses [componentToExtract ExtractedImage]" << std::endl;
    std::cerr << "  The MembershipImage image written is a VectorImage, ( an image with multiple components ) ";
    std::cerr << "Given that most viewers can't see vector images, we will optionally extract a component and ";
    std::cerr << "write it out as a scalar image as well." << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< unsigned char, Dimension > ImageType;
  typedef itk::BayesianClassifierInitializationImageFilter< ImageType > 
                                                BayesianInitializerType;
  BayesianInitializerType::Pointer bayesianInitializer 
                                          = BayesianInitializerType::New();

  typedef itk::ImageFileReader< ImageType > ReaderType;
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

  bayesianInitializer->SetInput( reader->GetOutput() );
  bayesianInitializer->SetNumberOfClasses( atoi( argv[3] ) );

  // TODO add test where we specify membership functions
  
  typedef itk::ImageFileWriter< 
    BayesianInitializerType::OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( bayesianInitializer->GetOutput() );
  writer->SetFileName( argv[2] );
  
  try
    {
    bayesianInitializer->Update();
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

  if( argv[4] && argv[5] )
    {
    typedef BayesianInitializerType::OutputImageType MembershipImageType;
    typedef itk::Image< MembershipImageType::InternalPixelType, 
                        Dimension > ExtractedComponentImageType;
    ExtractedComponentImageType::Pointer extractedComponentImage = 
                                    ExtractedComponentImageType::New();
    extractedComponentImage->CopyInformation( 
                          bayesianInitializer->GetOutput() );
    extractedComponentImage->SetBufferedRegion( bayesianInitializer->GetOutput()->GetBufferedRegion() );
    extractedComponentImage->SetRequestedRegion( bayesianInitializer->GetOutput()->GetRequestedRegion() );
    extractedComponentImage->Allocate();
    typedef itk::ImageRegionConstIterator< MembershipImageType > ConstIteratorType;
    typedef itk::ImageRegionIterator< ExtractedComponentImageType > IteratorType;
    ConstIteratorType cit( bayesianInitializer->GetOutput(), 
                     bayesianInitializer->GetOutput()->GetBufferedRegion() );
    IteratorType it( extractedComponentImage, 
                     extractedComponentImage->GetLargestPossibleRegion() );
    
    const unsigned int componentToExtract = atoi( argv[4] );
    cit.Begin();
    it.Begin();
    while( !cit.IsAtEnd() )
      {
      it.Set(cit.Get()[componentToExtract]);
      ++it;
      ++cit;
      }

    // Write out the rescaled extracted component
    typedef itk::ImageFileWriter<  
                       ExtractedComponentImageType >  ExtractedComponentWriterType;
    ExtractedComponentWriterType::Pointer 
               rescaledImageWriter = ExtractedComponentWriterType::New();
    rescaledImageWriter->SetInput( extractedComponentImage );
    rescaledImageWriter->SetFileName( argv[5] );
    rescaledImageWriter->Update();
    }

  return EXIT_SUCCESS;
}

