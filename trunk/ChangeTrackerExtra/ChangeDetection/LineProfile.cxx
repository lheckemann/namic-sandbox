/*=========================================================================


=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAndImageFilter.h"
#include "itkOrientedImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkLineIterator.h"

int main(int argc, char * argv[])
{
  const unsigned int dim = 3;
  typedef float                           PixelType;
  typedef itk::OrientedImage< PixelType, dim >            ImageType;

  ImageType::IndexType idx0, idx1;
  if( argc < 8 )
    {
    idx0[0] = -1;
    } 
  else 
    {
    idx0[0] = atoi(argv[2]);
    idx0[1] = atoi(argv[3]);
    idx0[2] = atoi(argv[4]);

    idx1[0] = atoi(argv[5]);
    idx1[1] = atoi(argv[6]);
    idx1[2] = atoi(argv[7]);
    }

 
  typedef itk::ImageFileReader< ImageType >       ReaderType;
  typedef itk::ImageFileWriter< ImageType >       WriterType;
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;
  typedef itk::LineIterator<ImageType> LineIteratorType;

  ReaderType::Pointer r1 = ReaderType::New();
  r1->SetFileName(argv[1]);
  r1->Update();

  ImageType::Pointer image = r1->GetOutput();

  if(idx0[0] == -1){
    ImageType::SizeType imageSize;
    imageSize = image->GetBufferedRegion().GetSize();
    idx0[0] = 0;
    idx0[1] = 0;
    idx0[2] = imageSize[2]/2;
    idx1[0] = imageSize[0]-1;
    idx1[1] = imageSize[1]-1;
    idx1[2] = imageSize[2]/2;
    std::cerr << "Scanning line from " << idx0 << " to " << idx1 << std::endl;
  }

  int i = 0;
  LineIteratorType lit(image, idx0, idx1);
  for(lit.GoToBegin();!lit.IsAtEnd();++lit)
    std::cout << i++ << " " << lit.Get() << std::endl;

  return 0;
}
