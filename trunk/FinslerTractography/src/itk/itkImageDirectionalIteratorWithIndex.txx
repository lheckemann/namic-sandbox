#ifndef _itkImageDirectionalIteratorWithIndex_txx
#define _itkImageDirectionalIteratorWithIndex_txx

#include "itkImageDirectionalIteratorWithIndex.h"

namespace itk
{


template< typename TImage >
ImageDirectionalIteratorWithIndex<TImage>
::ImageDirectionalIteratorWithIndex()
  : ImageDirectionalConstIteratorWithIndex<TImage>() 
{


}



template< typename TImage >
ImageDirectionalIteratorWithIndex<TImage>
::ImageDirectionalIteratorWithIndex(TImage *ptr, const RegionType& region)
  : ImageDirectionalConstIteratorWithIndex<TImage>(ptr, region) 
{


}


 
template< typename TImage >
ImageDirectionalIteratorWithIndex<TImage>
::ImageDirectionalIteratorWithIndex( const ImageIteratorWithIndex<TImage> &it):
  ImageDirectionalConstIteratorWithIndex<TImage>(it)
{ 
}

 
template< typename TImage >
ImageDirectionalIteratorWithIndex<TImage>
::ImageDirectionalIteratorWithIndex( const ImageDirectionalConstIteratorWithIndex<TImage> &it):
  ImageDirectionalConstIteratorWithIndex<TImage>(it)
{ 
}

 
template< typename TImage >
ImageDirectionalIteratorWithIndex<TImage> &
ImageDirectionalIteratorWithIndex<TImage>
::operator=( const ImageDirectionalConstIteratorWithIndex<TImage> &it)
{ 
  this->ImageDirectionalConstIteratorWithIndex<TImage>::operator=(it);
  return *this;
}



} // end namespace itk

#endif
