/*=========================================================================


=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAndImageFilter.h"
#include "itkOrientedImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageDuplicator.h"
#include "itkTransformFileReader.h"
#include "itkAffineTransform.h"

int main(int argc, char * argv[])
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " baseline followup result [transform]";
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
  typedef itk::ImageDuplicator<ImageType> DupType;
  typedef itk::TransformFileReader TransformReaderType;
  typedef itk::AffineTransform<double> TransformType;

  ReaderType::Pointer r1 = ReaderType::New();
  ReaderType::Pointer r2 = ReaderType::New();
  r1->SetFileName(argv[1]);
  r2->SetFileName(argv[2]);
  
  TransformType::Pointer affineTfm = NULL;
  if(argc>4){
    std::cout << "Will use the supplied transform!" << std::endl;
    // transform is available
    affineTfm = TransformType::New();

    TransformReaderType::Pointer tfmReader = TransformReaderType::New();
    tfmReader->SetFileName(argv[4]);
    tfmReader->Update();
  
    TransformReaderType::TransformType::Pointer rtfm = 
      *(tfmReader->GetTransformList()->begin());
    typedef itk::MatrixOffsetTransformBase<double,3,3> OffsetType;
    OffsetType::Pointer d =
      dynamic_cast<OffsetType*>(rtfm.GetPointer());

    if(!d){
      std::cerr << "Cast failed!" << std::endl;
    return -1;
    }

    vnl_svd<double> svd(d->GetMatrix().GetVnlMatrix());
    affineTfm->SetMatrix(svd.U()*vnl_transpose(svd.V()));
    affineTfm->SetOffset(d->GetOffset());
  }


  r1->Update();r2->Update();
  ImageType::Pointer i1 = r1->GetOutput(), i2 = r2->GetOutput();

  DupType::Pointer dup = DupType::New();
  dup->SetInputImage(i1);
  dup->Update();

  ImageType::Pointer r = dup->GetOutput();
  r->FillBuffer(0);

  IterType it1(i1,i1->GetLargestPossibleRegion());
  IterType it2(i2,i2->GetLargestPossibleRegion());
  IterType itR(r,r->GetLargestPossibleRegion());
  
  it1.GoToBegin();
  it2.GoToBegin();
  itR.GoToBegin();
  for(;!it1.IsAtEnd();++it1,++it2,++itR){    
    
    ImageType::IndexType idx1 = it1.GetIndex(), idx2;
    ImageType::PointType pt;
    i1->TransformIndexToPhysicalPoint(idx1,pt);

    if(affineTfm)
      pt = affineTfm->TransformPoint(pt);

    i2->TransformPhysicalPointToIndex(pt,idx2);    

    if(it1.Get() && i2->GetPixel(idx2))
      itR.Set(0);
    if(it1.Get() && i2->GetPixel(idx2)==0)
      itR.Set(12);
    if(it1.Get()==0 && i2->GetPixel(idx2))
      itR.Set(14);
  }
  
  WriterType::Pointer w = WriterType::New();
  w->SetFileName(argv[3]);
  w->SetInput(r);
  w->SetUseCompression(1);
  w->Update();

  return 0;
  
}
