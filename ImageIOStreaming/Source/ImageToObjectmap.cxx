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
void
ImageToObjectMap<TConvertImage>
::TransformImage(InputImageType *image)
{
//  itk::MetaDataObjectBase::Pointer lib = itk::MetaDataObjectBase::New();

  //itk::MetaDataDictionary data = image->GetMetaDataDictionary();
  //itk::AnalyzeObjectMap::Pointer objectMap = itk::AnalyzeObjectMap::New();
  //std::vector<std::string> stringData = data.GetKeys();
  //itk::AnalyzeObjectEntryArrayType my_reference=data.GetKeys()[1][2];//objectMap->GetAnalyzeObjectEntryArrayPointer();
  //my_reference = data.GetKeys();
//  data::Iterator iteratorForData;
//  itk::ImageIterator<TConvertImage> bob = data->Begin();
  //itk::ImageIterator<TConvertImage> ImageIteratorType;
  //int i = data.Begin();
  //data.Find("ANALYZE_OBJECT_LABEL_MAP_ENTRY_ARRAY");
  //typedef itk::MetaDataDictionary::Iterator dataIterator = data.Begin();
  //dataIterator = data.Begin();
  //dataIterator
  //data::Iterator piff;
  //dataIterator::Pointer = dataIterator::New();
  //= data->Begin();
  //itk::AnalyzeObjectMap *ObjectMap = itk::AnalyzeObjectMap::New();

  //for(int i=0, data.Begin();!data.End();++data, i++)
  //{
  //  ObjectMap->getObjectEntry(i) = data; 
  //}
  //itk::ImageRegionIterator<ConvertImage > indexIt(image,image->GetLargetRegion());
  //itk::ImageRegionIterator<itk::Image<unsigned char,image->GetDimensions() > > indexObjectMap(ObjectMap,image->GetLargetRegion());

  //for(indexIt->GoToBegin();!indexIt->IsAtEnd(); ++indexIt, ++indexObjectMap)
  //{
  //  indexObjectMap->Set(indexIt->Get());
  //}


}


}

#endif
