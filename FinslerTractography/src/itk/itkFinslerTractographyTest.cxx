// THE FAST SWEEPING TRACTOGRAPHY APPROACH
// by John Melonakos

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkFinslerTractographyTest.h"

int main( int argc, char *argv[] )
{
  std::cerr << "##########################################" << std::endl;
  std::cerr << "## DWI TRACTOGRAPHY USING FAST SWEEPING ##" << std::endl;
  std::cerr << "##########################################" << std::endl;
  std::cerr << std::endl;

  if(argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << " NrrdFileName(.nhdr) threshold(on B0)"
              << " FAImageFileName RelativeAnisotropyFileName " << 
      "[ExtractGradientAndReferenceImage from the NRRD file and "
              << "write them as images]" << std::endl;
    std::cerr << "\tExample args: xt_dwi.nhdr 80 FA.mhd 1" << std::endl;
    return EXIT_FAILURE;
    }
  
  /* Data Input */
  ImageType::Pointer img;
  ReaderType::Pointer reader = ReaderType::New();
  
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    img = reader->GetOutput();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  /* Parse the NRRD Header */
  itk::MetaDataDictionary imgMetaDictionary = img->GetMetaDataDictionary();    
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  TensorReconstructionImageFilterType::GradientDirectionType vect3d;
  TensorReconstructionImageFilterType::GradientDirectionContainerType::Pointer 
    DiffusionVectors = 
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
      
      DiffusionVectors->InsertElement( numberOfImages, vect3d );
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
  DWIIteratorType dwiit( img, img->GetBufferedRegion() ); 
  
  GradientImageType::Pointer referenceImage = GradientImageType::New();
  referenceImage->CopyInformation( img );
  referenceImage->SetBufferedRegion( img->GetBufferedRegion() );
  referenceImage->SetRequestedRegion( img->GetRequestedRegion() );
  referenceImage->Allocate();
    
  IteratorType it( referenceImage, referenceImage->GetBufferedRegion() );
  dwiit.GoToBegin();
  it.GoToBegin();
  std::cerr << "DEBUG: 1" << std::endl;
  while (!it.IsAtEnd())
    {
    it.Set(dwiit.Get()[0]);
    ++it;
    ++dwiit;
    }
  std::cerr << "DEBUG: 2" << std::endl;
  if( writeReferenceImageToFile )
    {
    GradientWriterType::Pointer gradientWriter = GradientWriterType::New();
    gradientWriter->SetInput( referenceImage );
    gradientWriter->SetFileName( "ReferenceImage.nhdr" );
  std::cerr << "DEBUG: 3" << std::endl;
    gradientWriter->Update();
  std::cerr << "DEBUG: 4" << std::endl;
    }
  
  /* Reconstruct the Tensors */
  TensorReconstructionImageFilterType::Pointer tensorReconstructionFilter = 
    TensorReconstructionImageFilterType::New();
  tensorReconstructionFilter->SetGradientImage(
    DiffusionVectors, reader->GetOutput() );
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

  std::cerr << "DEBUG: 4" << std::endl;
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

  /* Generate the Local Cost Function */


  /* Run Fast Sweeping */

  ImageType::ConstPointer image = reader->GetOutput();
  
  ImageType::RegionType region = image->GetBufferedRegion();
  ImageType::IndexType start = region.GetIndex();
  ImageType::SizeType size = region.GetSize();


  ImageType::RegionType region2;
  ImageType::IndexType start2;
  ImageType::SizeType size2;

  const unsigned int radius = 1;

  size2[0] = size[0] - 2 * radius;
  size2[1] = size[1] - 2 * radius;
  size2[2] = size[2] - 2 * radius;

  start2[0] = start[0] + radius;
  start2[1] = start[1] + radius;
  start2[2] = start[2] + radius;

  region2.SetSize( size2 );
  region2.SetIndex( start2 );

  IteratorType itr2( image, region2 );

  NormalIteratorType itr1( image, region2 );

  std::cout << "Region = " << region2 << std::endl;

  std::cout << "Original image values = " << std::endl;
  itr1.GoToBegin();
  while( !itr1.IsAtEnd() )
    {
    std::cout <<  itr1.GetIndex() << " = " << itr1.Get() << std::endl;
    ++itr1;
    }


  std::cout << std::endl;
  std::cout << "Now visiting with ImageDirectionalConstIterator" << std::endl;
  std::cout << std::endl;

  itr2.SetRadius( radius );

  typedef IteratorType::Iterator  NeighborPixel;

  itr2.GoToBegin();

  while( !itr2.IsAtEnd() )
    {
    while( !itr2.IsAtEndOfDirection() )
      {

      PixelType neighborValue = itr2.Get();
      std::cout << itr2.GetIndex() << " = " << neighborValue << std::endl;

      // Access the neighbor pixels
      NeighborPixel neighbor = itr2.GetFirstNeighborIterator();
      for(unsigned int k=0; k < 27; k++)
        {
        PixelType neighborValue = itr2.GetNeighborPixel(k);
        std::cout << neighborValue << std::endl;
        }

      ++itr2;
      }
    itr2.NextDirection();
    }

  /* Generate the Tracts */

  /* Visualize the Results */

  return EXIT_SUCCESS;
}
