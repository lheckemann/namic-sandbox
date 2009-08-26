#ifndef LoadMask_h
#define LoadMask_h

#include "itkBrains2MaskImageIO.h"
#include "itkBrains2MaskImageIOFactory.h"
#include "itkIO.h"
#include "itkImageMaskSpatialObject.h"

inline
void RegisterBrains2MaskFactory()
{
  itk::ObjectFactoryBase::RegisterFactory( itk::Brains2MaskImageIOFactory::New() );
}

template <class MaskType, unsigned Dimension>
typename MaskType::Pointer
ReadImageMask(const std::string & filename,
  typename itk::Image<unsigned char,
    Dimension>::DirectionType desiredImageDirection,
  const itk::Point<double,
    Dimension> & ImageOriginPoint)
{
  typedef unsigned char                                 MaskPixelType;
  typedef typename itk::Image<MaskPixelType, Dimension> MaskImageType;

  // read in the mask
  typename MaskImageType::Pointer OrientedMaskImage
    = itkUtil::ReadImageAndOrient<MaskImageType>(filename,
                                                 desiredImageDirection);
  OrientedMaskImage->SetOrigin(ImageOriginPoint);

  // convert mask image to mask
  typedef typename itk::ImageMaskSpatialObject<Dimension>
  ImageMaskSpatialObjectType;
  typename ImageMaskSpatialObjectType::Pointer mask
    = ImageMaskSpatialObjectType::New();
  mask->SetImage(OrientedMaskImage);
  //
  mask->ComputeObjectToWorldTransform();

  // return pointer to mask
  typename MaskType::Pointer p = dynamic_cast<MaskType *>( mask.GetPointer() );
  if ( p.IsNull() )
    {
    std::cout << "ERROR::" << __FILE__ << " " << __LINE__ << std::endl;
    exit(-1);
    }
  return p;
}

#endif // LoadMask_h
