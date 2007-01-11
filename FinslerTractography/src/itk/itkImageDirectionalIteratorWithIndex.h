#ifndef __itkImageDirectionalIteratorWithIndex_h
#define __itkImageDirectionalIteratorWithIndex_h

#include "itkImageDirectionalConstIteratorWithIndex.h"
#include "itkImageIteratorWithIndex.h"

namespace itk
{
template<typename TImage>
class ITK_EXPORT ImageDirectionalIteratorWithIndex : public ImageDirectionalConstIteratorWithIndex<TImage>
{
public:
  /** Standard class typedefs. */
  typedef ImageDirectionalIteratorWithIndex Self;
  typedef ImageDirectionalConstIteratorWithIndex<TImage>  Superclass;
  
  /** Types inherited from the Superclass */
  typedef typename Superclass::IndexType              IndexType;
  typedef typename Superclass::IndexValueType         IndexValueType;
  typedef typename Superclass::SizeType               SizeType;
  typedef typename Superclass::SizeValueType          SizeValueType;
  typedef typename Superclass::OffsetType             OffsetType;
  typedef typename Superclass::OffsetValueType        OffsetValueType;
  typedef typename Superclass::RegionType             RegionType;
  typedef typename Superclass::ImageType              ImageType;
  typedef typename Superclass::PixelContainer         PixelContainer;
  typedef typename Superclass::PixelContainerPointer  PixelContainerPointer;
  typedef typename Superclass::InternalPixelType      InternalPixelType;
  typedef typename Superclass::PixelType              PixelType;
  typedef typename Superclass::AccessorType           AccessorType;

  ImageDirectionalIteratorWithIndex();
  
  ImageDirectionalIteratorWithIndex(TImage *ptr, const RegionType& region);

  ImageDirectionalIteratorWithIndex( const ImageIteratorWithIndex<TImage> &it);

  void Set( const PixelType & value) const  
    { this->m_PixelAccessorFunctor.Set(*(const_cast<InternalPixelType *>(this->m_Position)),value); }

  PixelType & Value(void) 
    { return *(const_cast<InternalPixelType *>(this->m_Position)); }
 
protected:
  ImageDirectionalIteratorWithIndex( const ImageDirectionalConstIteratorWithIndex<TImage> &it);
  Self &operator=(const ImageDirectionalConstIteratorWithIndex<TImage> & it);
  
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageDirectionalIteratorWithIndex.txx"
#endif

#endif 
