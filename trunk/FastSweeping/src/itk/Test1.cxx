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


  IteratorType itr2( image, region );

  itr2.SetRadius( 1 );

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
      NeighborPixel neigbor = itr2.GetFirstNeighborIterator();
      NeighborPixel last    = itr2.GetTerminalNeighborIterator();
      while( neigbor != last )
        {
        PixelType neighborValue = *neigbor;
        ++neigbor;
        }
      ++itr2;
      }
    itr2.NextDirection();
    }


}


