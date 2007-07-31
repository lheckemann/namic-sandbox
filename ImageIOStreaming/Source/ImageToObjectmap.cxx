#ifndef __ImageToObjectMap_cxx
#define __ImageToObjectMap_cxx

#include "ImageToObjectmap.h"



namespace itk
{

/**
 *
 */
template<class TConvertImage>
ImageToObjectMap<TConvertImage>
::ImageToObjectMap()
{
  std::cout<<"I am actually in here"<<std::endl;
}

template<class TConvertImage>
ImageToObjectMap<TConvertImage>
::~ImageToObjectMap()
{
  
}


template<class TConvertImage>
itk::AnalyzeObjectMap *
ImageToObjectMap<TConvertImage>
::TransformImage(InputImageType *image)
{
  this->ObjectMap = itk::AnalyzeObjectMap::New();
  this->ObjectMap->SetRegions(image->GetLargestPossibleRegion());
  this->ObjectMap->Allocate();
  itk::ImageRegionIterator<TConvertImage> ImageIndexIT(image, image->GetLargestPossibleRegion());
  itk::ImageRegionIterator<TConvertImage> ObjectmapIndexIT(this->ObjectMap, image->GetLargestPossibleRegion());
  for(ImageIndexIT.GoToBegin(); !ImageIndexIT.IsAtEnd(); ++ImageIndexIT, ++ObjectmapIndexIT)
  {
    this->ObjectMap->SetPixel(ImageIndexIT.GetIndex(), ImageIndexIT.Get());
    //ObjectmapIndexIT.Set(ImageIndexIT.Get());
  }
  itk::AnalyzeObjectEntryArrayType *my_reference = this->ObjectMap->GetAnalyzeObjectEntryArrayPointer();
  itk::ExposeMetaData<itk::AnalyzeObjectEntryArrayType>(image->GetMetaDataDictionary(),ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY, *my_reference);

  return this->ObjectMap;
}


}

#endif
