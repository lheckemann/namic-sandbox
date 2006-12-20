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
  itr2.GoToBegin();
  while( !itr2.IsAtEnd() )
    {
    while( !itr2.IsAtEndOfDirection() )
      {
      std::cout << itr2.Get();
      std::cout << " : " << itr2.GetIndex();
      std::cout << std::endl;
      ++itr2;
      }
    std::cout << "-----------------" << std::endl;
    itr2.NextDirection();
    }


}


