#include "itkImageFileWriter.h"
#include "itkLabelObject.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelObjectAccessors.h"
#include "itkLabelMap.h"
#include "itkOrientedImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"

#include <fstream>
#include <string>

#define PI      3.1415

//#define RADIUS  10
#define BG1     50
#define BG2     20
#define FG     100
#define SX       .9375
#define SY       .9375
#define SZ       1.4

int main(int argc, char * argv[])
{
  typedef char  PixelType;
  typedef itk::OrientedImage< PixelType,3> ImageType; 

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " radius output_prefix" << std::endl;
    return -1;
  }

  float RADIUS = atof(argv[1]);

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
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;

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
  image->SetSpacing(spacing);
  image->Allocate();
  image->FillBuffer(0);

  IteratorType it(image,region);
  it.GoToBegin();

  PointType point, center;
  PointType::VectorType radial;
  IndexType centralIndex;
  centralIndex[0] = size[0]/2;
  centralIndex[1] = size[1]/2;
  centralIndex[2] = size[2]/2;

  float dVol = 0, voxelVol = spacing[0]*spacing[1]*spacing[2];
  float tVol = (4./3.)*PI*RADIUS*RADIUS*RADIUS;
  float tSurf = 4.*PI*RADIUS*RADIUS;

  image->TransformIndexToPhysicalPoint(centralIndex, center);
  while(!it.IsAtEnd()){
    image->TransformIndexToPhysicalPoint(it.GetIndex(),point);
    radial = point-center;
    if(radial.GetNorm()<RADIUS){
      it.Set(FG);
      dVol += voxelVol;
    } else {
      if(((unsigned)it.GetIndex()[0])<size[0]/2)
        it.Set(BG1);
      else
        it.Set(BG2);
    }
    ++it;
  }

  std::cout << "Surf area: " << tSurf << std::endl;
  std::cout << "True volume: " << tVol << std::endl;
  std::cout << "Digitized volume: " << dVol << std::endl;
  std::cout << "Error: " << tVol-dVol << " (" << (tVol-dVol)/tVol*100. << "%)" << std::endl;

  std::string gfName = std::string(argv[2])+".nrrd";
  std::string bfName = std::string(argv[2])+"_label.nrrd";
  std::string lfName = std::string(argv[2])+".info";

  std::ofstream info(lfName.c_str());
  info << "Surf area: " << tSurf << std::endl;
  info << "True volume: " << tVol << std::endl;
  info << "Digitized volume: " << dVol << std::endl;
  info << "Error: " << tVol-dVol << " (" << (tVol-dVol)/tVol*100. << "%)" << std::endl;

  WriterType::Pointer w = WriterType::New();
  w->SetFileName(gfName.c_str());
  w->SetInput(image);
  w->SetUseCompression(1);
  w->Update();

  ThreshType::Pointer thresh = ThreshType::New();
  thresh->SetInput(image);
  thresh->SetUpperThreshold(FG);
  thresh->SetLowerThreshold(FG);
  thresh->SetInsideValue(255);
  thresh->SetOutsideValue(0);

  WriterType::Pointer ws = WriterType::New();
  ws->SetFileName(bfName.c_str());
  ws->SetInput(thresh->GetOutput());
  ws->SetUseCompression(1);
  ws->Update();

  return 0;
}
