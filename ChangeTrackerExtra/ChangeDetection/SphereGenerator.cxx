#include "itkImageFileWriter.h"
#include "itkLabelObject.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelObjectAccessors.h"
#include "itkLabelMap.h"
#include "itkOrientedImage.h"
#include "itkImageRegionIteratorWithIndex.h"

#define PI      3.1415

#define RADIUS  10
#define BG       0
#define FG     100
#define SX       1
#define SY       1
#define SZ       1

int main(int argc, char * argv[])
{
  typedef char  PixelType;
  typedef itk::OrientedImage< PixelType,3> ImageType; 

  // Declare the type of the index,size and region to initialize images
  typedef itk::Index<3>                     IndexType;
  typedef itk::Size<3>                      SizeType;
  typedef ImageType::PointType                     PointType;
  typedef itk::ImageRegion<3>               RegionType;
  typedef ImageType::PixelType                      PixelType;
  typedef ImageType::SpacingType                    SpacingType;
  typedef itk::ShapeLabelObject< PixelType, 3 >   ShapeLabelObjectType;
//  typedef itk::LabelMap< ShapeLabelObjectType >     LabelMapType;
//  typedef itk::LabelImageToShapeLabelMapFilter< ImageType, LabelMapType> I2LType;
  typedef itk::ImageFileWriter<ImageType>           WriterType;

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;
//  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;

  SizeType size;
  size.Fill(128);

  SpacingType spacing;
  spacing[0] = SX;
  spacing[1] = SY;
  spacing[2] = SZ;

  IndexType start;
  start.Fill(0);

  RegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(BG);

  IteratorType it(image,region);
  it.GoToBegin();

  PointType point, center;
  PointType::VectorType radial;
  IndexType centralIndex;
  centralIndex[0] = size[0]/2;
  centralIndex[1] = size[1]/2;
  centralIndex[2] = size[2]/2;

  float dVol = 0, voxelVol = spacing[0]*spacing[1]*spacing[2];

  image->TransformIndexToPhysicalPoint(centralIndex, center);
  while(!it.IsAtEnd()){
    image->TransformIndexToPhysicalPoint(it.GetIndex(),point);
    radial = point-center;
    if(radial.GetNorm()<RADIUS){
      it.Set(FG);
      dVol += voxelVol;
    }
    ++it;
  }

  std::cout << "True volume: " << 4./3.*PI*RADIUS*RADIUS*RADIUS << std::endl;
  std::cout << "Digitized volume: " << dVol << std::endl;

  if(argc>1){
    WriterType::Pointer w = WriterType::New();
    w->SetFileName(argv[1]);
    w->SetInput(image);
    w->Update();
  }

  return 0;
}
