#ifndef __ImageToObjectMap_cxx
#define __ImageToObjectMap_cxx

#include "ImageToObjectmap.h"

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkImageIOFactory.h"
#include "itkCommand.h"
#include "vnl/vnl_vector.h"
#include "itkVectorImage.h"

namespace itk
{

/**
 *
 */
template<class ConvertImage>
ImageToObjectMap<ConvertImage>
::ImageToObjectMap()
{
  
}

template<class ConvertImage>
ImageToObjectMap<ConvertImage>
::~ImageToObjectMap()
{
  
}


template<class ConvertImage>
void
ImageToObjectMap<ConvertImage>
::TransformImage(InputImageType *image)
{
  itk::MetaDataDictionary *data = image->GetMetaDataDictionary();
  typedef itk::MetaDataDictionary::Iterator dataIterator = data->Begin();
  itk::AnalyzeObjectMap *ObjectMap = itk::AnalyzeObjectMap::New();

  for(int i=0;!dataIterator->IsAtEnd();++dataIterator, i++)
  {
    ObjectMap->getObjectEntry(i) = dataIterator; 
  }
  itk::ImageRegionIterator<ConvertImage > indexIt(image,image->GetLargetRegion());
  itk::ImageRegionIterator<itk::Image<unsigned char,image->GetDimensions() > > indexObjectMap(ObjectMap,image->GetLargetRegion());

  for(indexIt->GoToBegin();!indexIt->IsAtEnd(); ++indexIt, ++indexObjectMap)
  {
    indexObjectMap->Set(indexIt->Get());
  }


}


}

#endif
