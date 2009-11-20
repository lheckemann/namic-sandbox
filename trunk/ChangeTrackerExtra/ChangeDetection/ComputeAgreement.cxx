/*=========================================================================


=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAndImageFilter.h"
#include "itkOrientedImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

int main(int argc, char * argv[])
{
  if( argc != 4 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " label1 label2 overlap ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 3;
 
  typedef unsigned char                           PixelType;
  typedef itk::OrientedImage< PixelType, dim >            ImageType;
  typedef itk::ImageFileReader< ImageType >       ReaderType;
  typedef itk::ImageFileWriter< ImageType >       WriterType;
  typedef itk::AndImageFilter< ImageType, ImageType> AndType;
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;
  typedef itk::ImageRegionIteratorWithIndex<ImageType> IterType;

  ReaderType::Pointer r1 = ReaderType::New();
  ReaderType::Pointer r2 = ReaderType::New();
  r1->SetFileName(argv[1]);
  r2->SetFileName(argv[2]);
  
  r1->Update();r2->Update();
  ImageType::Pointer i1 = r1->GetOutput(), i2 = r2->GetOutput();

  IterType it(i1,i1->GetLargestPossibleRegion());
  it.GoToBegin();
  for(;!it.IsAtEnd();++it){
    if(!it.Get())
      continue;
    ImageType::IndexType idx1 = it.GetIndex(), idx2;
    ImageType::PointType pt;
    i1->TransformIndexToPhysicalPoint(idx1,pt);
    i2->TransformPhysicalPointToIndex(pt,idx2);
    if(i2->GetPixel(idx2))
      it.Set(10);
    else
      it.Set(0);
  }
  
  WriterType::Pointer w = WriterType::New();
  w->SetFileName(argv[3]);
  w->SetInput(i1);
  w->Update();

  return 0;
  
}
