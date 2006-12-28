#include "itkImage.h"
#include "itkImageDirectionalConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

main()
{
  const unsigned int Dimension = 3;

  typedef itk::Index< Dimension > PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;

  typedef itk::ImageDirectionalConstIteratorWithIndex< 
            ImageType > IteratorType;

  ImageType::Pointer image = ImageType::New();
  
  ImageType::RegionType region;
  ImageType::IndexType start;
  ImageType::SizeType size;

  size.Fill( 5 );
  start.Fill( 0 );

  region.SetSize( size );
  region.SetIndex( start );

  image->SetRegions( region );
  image->Allocate();

  typedef itk::ImageRegionIteratorWithIndex< 
    ImageType > NormalIteratorType;

  NormalIteratorType itr1( image, region );
  itr1.GoToBegin();
  while( !itr1.IsAtEnd() )
    {
    itr1.Set( itr1.GetIndex() );
    ++itr1;
    }

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

  IteratorType itr2( image, region );

  itr2.SetRadius( radius );

  typedef IteratorType::Iterator  NeighborPixel;

  itr2.GoToBegin();

  while( !itr2.IsAtEnd() )
    {
    while( !itr2.IsAtEndOfDirection() )
      {
      if( itr2.Get() != itr2.GetIndex() )
        {
        std::cerr << "Error: ";
        std::cerr << " Get()      = " << itr2.Get();
        std::cerr << " GetIndex() = " << itr2.GetIndex();
        std::cerr << std::endl;
        }  
      std::cout << "Begin walking neighbors..." << std::endl; 
      NeighborPixel neighbor = itr2.GetFirstNeighborIterator();
      NeighborPixel last     = itr2.GetTerminalNeighborIterator();
      while( neighbor != last )
        {
//        PixelType neighborValue = *(*neighbor);
//        std::cout << neighborValue << std::endl;
        std::cout << *neighbor << std::endl;
        ++neighbor;
        }
      std::cout << "End walking neighbors..." << std::endl; 
      ++itr2;
      }
    itr2.NextDirection();
    }


}


