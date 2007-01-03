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

  typedef itk::ImageFileReader< ImageType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );
  reader->Update();

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

  itr2.SetRadius( radius );

  typedef IteratorType::Iterator  NeighborPixel;

  itr2.GoToBegin();

  while( !itr2.IsAtEnd() )
    {
    while( !itr2.IsAtEndOfDirection() )
      {
      NeighborPixel neighbor = itr2.GetFirstNeighborIterator();
      NeighborPixel last     = itr2.GetTerminalNeighborIterator();
      while( neighbor != last )
        {
        PixelType neighborValue = *(neighbor[13]);
        std::cout <<  neighborValue << std::endl;
        ++neighbor;
        }
      ++itr2;
      }
    itr2.NextDirection();
    }

}


