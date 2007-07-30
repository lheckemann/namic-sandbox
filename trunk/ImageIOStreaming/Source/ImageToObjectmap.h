#include "itkImageSource.h"
#include <itkMetaDataObject.h>
#include "objectmap.h"
#include "itkImage.h"
#include "itkExceptionObject.h"

namespace itk
{

template <class ConvertImage>

class ITK_EXPORT ImageToObjectMap : public ProcessObject
{

  public:
  /** Standard class typedefs */
  typedef ImageToObjectMap               Self;
  typedef ProcessObject  Superclass;
  typedef SmartPointer<Self>  Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  typedef ConvertImage InputImageType;
  //typedef SmartPointer<const Self>  ConstPointer;

  

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToObjectMap, ImageSource);

  void TransformImage(InputImageType *image);

  

protected:
  ImageToObjectMap();
~ImageToObjectMap();

private:
  ImageToObjectMap(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};
}
