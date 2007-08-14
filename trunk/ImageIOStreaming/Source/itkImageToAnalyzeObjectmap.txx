#ifndef __itkImageToObjectMap_txx
#define __itkImageToObjectMap_txx

#include "itkImageToAnalyzeObjectmap.h"



namespace itk
{

/**
 *
 */
template<class TConvertImage>
ImageToObjectMap<TConvertImage>
::ImageToObjectMap()
{
}

template<class TConvertImage>
ImageToObjectMap<TConvertImage>
::~ImageToObjectMap()
{
  
}

//TODO: Check the meta data to see if it contains the anaylze object map entries.
//Right now I am assuming what is passed in has the analyze object map entries in the meta data.
template<class TConvertImage>
itk::AnalyzeObjectMap *
ImageToObjectMap<TConvertImage>
::TransformImage(InputImageType *image)
{
  this->ObjectMap = itk::AnalyzeObjectMap::New();
  this->ObjectMap->SetRegions(image->GetLargestPossibleRegion());
  this->ObjectMap->Allocate();
  this->ObjectMap->SetPixelContainer(image->GetPixelContainer());
  //itk::ImageRegionIterator<TConvertImage> ImageIndexIT(image, image->GetLargestPossibleRegion());
  //itk::ImageRegionIterator<TConvertImage> ObjectmapIndexIT(this->ObjectMap, image->GetLargestPossibleRegion());
  //for(ImageIndexIT.GoToBegin(); !ImageIndexIT.IsAtEnd(); ++ImageIndexIT, ++ObjectmapIndexIT)
  //{
  //  this->ObjectMap->SetPixel(ImageIndexIT.GetIndex(), ImageIndexIT.Get());
  //  //ObjectmapIndexIT.Set(ImageIndexIT.Get());
  //}
  itk::AnalyzeObjectEntryArrayType *my_reference = this->ObjectMap->GetAnalyzeObjectEntryArrayPointer();
  itk::ExposeMetaData<itk::AnalyzeObjectEntryArrayType>(image->GetMetaDataDictionary(),ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY, *my_reference);
  this->ObjectMap->SetNumberOfObjects(this->ObjectMap->GetAnalyzeObjectEntryArrayPointer()->size());

  return this->ObjectMap;
}

template<class TConvertImage>
void
ImageToObjectMap<TConvertImage>
::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

}

#endif
