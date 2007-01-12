// THE FAST SWEEPING TRACTOGRAPHY APPROACH
// by John Melonakos

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkFinslerTractographyImageFilterTest.h"

int main( int argc, char *argv[] )
{
  std::cerr << "##########################################" << std::endl;
  std::cerr << "## DWI TRACTOGRAPHY USING FAST SWEEPING ##" << std::endl;
  std::cerr << "##########################################" << std::endl;
  std::cerr << std::endl;

  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0]
              << " NrrdFileName(.nhdr) threshold(on B0)" << std::endl;
    std::cerr << "\tExample args: xt_dwi.nhdr 80" << std::endl;
    return EXIT_FAILURE;
    }

  
  /* Data Input */
  DWIImageType::Pointer dwiInputImage;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    dwiInputImage = reader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }


  /* Parse the NRRD Header */
  itk::MetaDataDictionary imgMetaDictionary = dwiInputImage->GetMetaDataDictionary();    
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  TensorReconstructionImageFilterType::GradientDirectionType vect3d;
  TensorReconstructionImageFilterType::GradientDirectionContainerType::Pointer 
    GradientDiffusionVectors = 
    TensorReconstructionImageFilterType::GradientDirectionContainerType::New();
 
  for (; itKey != imgMetaKeys.end(); itKey ++)
    {
    double x,y,z;

    itk::ExposeMetaData<std::string> (imgMetaDictionary, *itKey, metaString);
    if (itKey->find("DWMRI_gradient") != std::string::npos)
      { 
      std::cout << *itKey << " ---> " << metaString << std::endl;      
      sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
      vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;
      
      GradientDiffusionVectors->InsertElement( numberOfImages, vect3d );
      ++numberOfImages;
      }
    else if (itKey->find("DWMRI_b-value") != std::string::npos)
      {
      std::cout << *itKey << " ---> " << metaString << std::endl;      
      readb0 = true;
      b0 = atof(metaString.c_str());
      }
    }
  std::cout << "Number Of Gradient images: " << numberOfImages << std::endl;
  if(!readb0)
    {
    std::cerr << "BValue not specified in header file" << std::endl;
    return EXIT_FAILURE;
    }

  /* Separate the Reference Image from the Gradient Images */
  DWIIteratorType dwiIt( dwiInputImage, dwiInputImage->GetBufferedRegion() ); 
  
  GradientImageType::Pointer referenceImage = GradientImageType::New();
  referenceImage->CopyInformation( dwiInputImage );
  referenceImage->SetBufferedRegion( dwiInputImage->GetBufferedRegion() );
  referenceImage->SetRequestedRegion( dwiInputImage->GetRequestedRegion() );
  referenceImage->Allocate();
    
  GradientIteratorType gradIt( referenceImage, referenceImage->GetBufferedRegion() );
  dwiIt.GoToBegin();
  gradIt.GoToBegin();
  while (!gradIt.IsAtEnd())
    {
    gradIt.Set(dwiIt.Get()[0]);
    ++gradIt;
    ++dwiIt;
    }
  if( writeReferenceImageToFile )
    {
    GradientWriterType::Pointer gradientWriter = GradientWriterType::New();
    gradientWriter->SetInput( referenceImage );
    gradientWriter->SetFileName( "ReferenceImage.nhdr" );
    gradientWriter->Update();
    }
  
  /* Reconstruct the Tensors */
  TensorReconstructionImageFilterType::Pointer tensorReconstructionFilter = 
    TensorReconstructionImageFilterType::New();
  tensorReconstructionFilter->SetGradientImage(
    GradientDiffusionVectors, reader->GetOutput() );
  tensorReconstructionFilter->SetNumberOfThreads( 1 );  // This is necessary until we fix netlib/dsvdc.c
  tensorReconstructionFilter->SetBValue(b0);  
  tensorReconstructionFilter->SetThreshold(
    static_cast< TensorReconstructionImageFilterType::ReferencePixelType >( 
      atof(argv[2])));
  tensorReconstructionFilter->Update();
  if( writeTensorsImageToFile )
    {
    TensorWriterType::Pointer tensorWriter = TensorWriterType::New();
    tensorWriter->SetFileName( "TensorsImage.nhdr" );
    tensorWriter->SetInput( tensorReconstructionFilter->GetOutput() );
    tensorWriter->Update();
    }

  /* Compute the FA and RA */
  FAFilterType::Pointer fractionalAnisotropyFilter = FAFilterType::New();
  fractionalAnisotropyFilter->SetInput( tensorReconstructionFilter->GetOutput() );
  if( writeFAImageToFile )
    {
    FAWriterType::Pointer faWriter = FAWriterType::New();
    faWriter->SetInput( fractionalAnisotropyFilter->GetOutput() );
    faWriter->SetFileName( "FAImage.nhdr" );
    faWriter->Update();
    }
  
  RAFilterType::Pointer relativeAnisotropyFilter = RAFilterType::New();
  relativeAnisotropyFilter->SetInput( tensorReconstructionFilter->GetOutput() );
  if( writeRAImageToFile )
    {
    RAWriterType::Pointer raWriter = RAWriterType::New();
    raWriter->SetInput( relativeAnisotropyFilter->GetOutput() );
    raWriter->SetFileName( "RAImage.nhdr" );
    raWriter->Update();
    }

  /* Execute Finsler Tractography */
  FinslerTractographyFilterType::Pointer finslerTractographyFilter =
    FinslerTractographyFilterType::New();
  finslerTractographyFilter->SetInput( reader->GetOutput() );
  finslerTractographyFilter->SetGradientDiffusionVectors( GradientDiffusionVectors );
  finslerTractographyFilter->Update();
  
  return EXIT_SUCCESS;
}
