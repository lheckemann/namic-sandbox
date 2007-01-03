#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include "itkImageDirectionalConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

int main( int argc, char *argv[] )
{

  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0] << " inputFileName.nhdr" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;

  typedef unsigned short PixelScalarType;
  typedef itk::VectorImage< PixelScalarType, Dimension >  ImageType;

  typedef ImageType::PixelType PixelType;

  typedef itk::ImageDirectionalConstIteratorWithIndex< 
            ImageType > IteratorType;

  typedef itk::ImageRegionConstIteratorWithIndex< 
            ImageType > NormalIteratorType;

  typedef itk::ImageFileReader< ImageType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

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

      // Access the neighbor pixels
      NeighborPixel neighbor = itr2.GetFirstNeighborIterator();
      PixelType neighborValue = itr2.GetNeighborPixel(13);
      std::cout << itr2.GetIndex() << " = " << neighborValue << std::endl;

      ++itr2;
      }
    itr2.NextDirection();
    }

}


