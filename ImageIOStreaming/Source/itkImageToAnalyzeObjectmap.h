#ifndef __ImageToObjectMap_h
#define __ImageToObjectMap_h

#include "itkImageSource.h"
#include "itkMetaDataObject.h"
#include "itkAnalyzeObjectMap.h"
#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"


namespace itk
{

template <class TConvertImage>

/**
   * \class ImageToAnalyzeObjectMap
   * \brief This class changes any Image with a meta data dictionary of Analyze Object Entries and changes it into
   *an Analyze Object Map.
   */
class ITK_EXPORT ImageToAnalyzeObjectMap : public ImageSource<TConvertImage>
{
  public:
  /** Standard class typedefs */
  typedef ImageToAnalyzeObjectMap    Self;
  typedef ImageSource<TConvertImage>       Superclass;
  typedef SmartPointer<Self>  Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToAnalyzeObjectMap, ImageSource);

  typedef TConvertImage InputImageType;

  itk::AnalyzeObjectMap * TransformImage(InputImageType *image);

protected:
  ImageToAnalyzeObjectMap();
~ImageToAnalyzeObjectMap();
void PrintSelf(std::ostream& os, Indent indent) const;

private:
  //ImageToAnalyzeObjectMap(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  itk::AnalyzeObjectMap::Pointer ObjectMap;
  
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToAnalyzeObjectmap.txx"
#endif
#endif
