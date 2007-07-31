#ifndef __ImageToObjectMap_h
#define __ImageToObjectMap_h

#include "itkImageSource.h"
#include <itkMetaDataObject.h>
#include "objectmap.h"
#include "itkImage.h"
#include "itkExceptionObject.h"
#include <itkObjectfactory.h>
#include <itkImageRegionIterator.h>


namespace itk
{

template <class TConvertImage>

class ITK_EXPORT ImageToObjectMap : public ImageSource<TConvertImage>
{

  public:
  /** Standard class typedefs */
  typedef ImageToObjectMap    Self;
  typedef ImageSource<TConvertImage>       Superclass;
  typedef SmartPointer<Self>  Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToObjectMap, ImageSource);

  typedef TConvertImage InputImageType;
  //typedef SmartPointer<const Self>  ConstPointer;

  itk::AnalyzeObjectMap * TransformImage(InputImageType *image);

protected:
  ImageToObjectMap();
~ImageToObjectMap();

private:
  //ImageToObjectMap(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  itk::AnalyzeObjectMap::Pointer ObjectMap;
  
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "ImageToObjectmap.cxx"
#endif
#endif
