/*
 * Input: tetrahedral mesh with the displacements at each vertex, greyscale
 * image, binary mask.
 * Output: masked part of the greyscale image deformed according to the mesh
 * displacements.
 *
 */

#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "itkTetrahedralMeshReader.h"
#include "vtkTetra.h"
#include <iostream>
#include "vtkMeshQuality.h"
#include "vtkPoints.h"

#include "vtkDebugLeaks.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkFieldData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkTensor.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMaskImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkChangeInformationImageFilter.h"
#include "itkVector.h"
#include "itkShiftScaleInPlaceImageFilter.h"

typedef float PixelType;
typedef itk::Vector<double,3> DFPixelType;
typedef itk::Image<PixelType,3> ImageType;
typedef itk::Image<DFPixelType,3> DFImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageFileWriter<ImageType> ImageWriterType;
typedef itk::ImageFileWriter<DFImageType> DFImageWriterType;
typedef itk::MaskImageFilter<ImageType,ImageType,ImageType> MaskFilterType;
typedef itk::WarpImageFilter<ImageType,ImageType,DFImageType> WarpFilterType;
typedef itk::ShiftScaleInPlaceImageFilter<DFImageType> ScaleFilterType;
typedef itk::ChangeInformationImageFilter<ImageType> ChangeInfoType;

typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ImageIterator;
typedef itk::ImageRegionIterator<DFImageType> DFImageIterator;

double getNorm(double* vec){
  return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

int main(int argc, char **argv){

  if(argc!=6){
    std::cerr << "Usage: " << "input_image input_mask input_mesh output_image output_image_df" << std::endl;
    return -1;
  }

  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(argv[1]);
  imageReader->Update();

//  ImageReaderType::Pointer maskReader = ImageReaderType::New();
//  maskReader->SetFileName(argv[2]);
//  maskReader->Update();

  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName(argv[3]);
  meshReader->Update();

  vtkUnstructuredGrid *mesh = meshReader->GetOutput();
  vtkDataArray *meshDeformation = mesh->GetPointData()->GetArray("ResultDisplacement");
  ImageType::Pointer image = imageReader->GetOutput();
//  ImageType::Pointer maskImage = maskReader->GetOutput();

  DFImageType::Pointer dfImage = DFImageType::New();
  DFImageType::RegionType region;
  region.SetSize(image->GetBufferedRegion().GetSize());
  region.SetIndex(image->GetBufferedRegion().GetIndex());
  dfImage->SetRegions(region);
  dfImage->Allocate();
  dfImage->SetSpacing(image->GetSpacing());

  ImageType::Pointer maskImage = ImageType::New();
  maskImage->SetRegions(region);
  maskImage->Allocate();
  maskImage->FillBuffer(0);
  maskImage->SetSpacing(image->GetSpacing());
  maskImage->SetDirection(image->GetDirection());
  maskImage->SetOrigin(image->GetOrigin());

  vtkPoints *meshPoints = mesh->GetPoints();
  vtkCellArray *cells = mesh->GetCells();
  vtkIdType npts, *pts;
  int i, cellId=0;
  
  std::cout << "Before the main loop" << std::endl;

  for(cells->InitTraversal();cells->GetNextCell(npts,pts);cellId++){
    assert(npts==4);
    vtkTetra* tetra = vtkTetra::New();
    vtkCell* thisCell = mesh->GetCell(cellId);
    tetra->Initialize(npts, pts, meshPoints);
//    std::cout << tetra->GetPointId(0) << " " << tetra->GetPointId(1) <<
//      " " << tetra->GetPointId(2) << " " << tetra->GetPointId(3) << std::endl;
    vtkPoints *tetraPoints = tetra->GetPoints();
    ImageType::IndexType bbMin, bbMax;
    bbMin[0] = image->GetBufferedRegion().GetSize()[0];
    bbMin[1] = image->GetBufferedRegion().GetSize()[1];
    bbMin[2] = image->GetBufferedRegion().GetSize()[2];
    bbMax = image->GetBufferedRegion().GetIndex();

    for(i=0;i<4;i++){
      ImageType::PointType point;
      ImageType::IndexType index;
      point[0] = tetraPoints->GetPoint(i)[0];
      point[1] = tetraPoints->GetPoint(i)[1];
      point[2] = tetraPoints->GetPoint(i)[2];
//      std::cout << point[0] << " " << point[1] << " " << point[2] << std::endl;
      image->TransformPhysicalPointToIndex(point, index);
      if(index[0]<bbMin[0])
        bbMin[0] = index[0];
      if(index[1]<bbMin[1])
        bbMin[1] = index[1];
      if(index[2]<bbMin[2])
        bbMin[2] = index[2];
      
      if(index[0]>bbMax[0])
        bbMax[0] = index[0];
      if(index[1]>bbMax[1])
        bbMax[1] = index[1];
      if(index[2]>bbMax[2])
        bbMax[2] = index[2];
    }

    bbMax[0]++;
    bbMax[1]++;
    bbMax[2]++;
    bbMin[0]--;
    bbMin[1]--;
    bbMin[2]--;

    ImageType::RegionType tetraRegion;
    ImageType::SizeType tetraRegionSize;
    tetraRegion.SetIndex(bbMin);
    tetraRegionSize[0] = bbMax[0]-bbMin[0];
    tetraRegionSize[1] = bbMax[1]-bbMin[1];
    tetraRegionSize[2] = bbMax[2]-bbMin[2];
    tetraRegion.SetSize(tetraRegionSize);
//    std::cout << "bounding box: " << tetraRegion << std::endl;

    double dp0[3], dp1[3], dp2[3], dp3[3];
      
    memcpy(&dp0[0], meshDeformation->GetTuple3(thisCell->GetPointId(0)), 
        sizeof(double)*3);
    memcpy(&dp1[0], meshDeformation->GetTuple3(thisCell->GetPointId(1)), 
        sizeof(double)*3);
    memcpy(&dp2[0], meshDeformation->GetTuple3(thisCell->GetPointId(2)), 
        sizeof(double)*3);
    memcpy(&dp3[0], meshDeformation->GetTuple3(thisCell->GetPointId(3)), 
        sizeof(double)*3);

//    std::cout << dp0[0] << " " << dp0[1] << " " << dp0[2] << std::endl;
//    std::cout << dp1[0] << " " << dp1[1] << " " << dp1[2] << std::endl;
//    std::cout << dp2[0] << " " << dp2[1] << " " << dp2[2] << std::endl;
//    std::cout << dp3[0] << " " << dp3[1] << " " << dp3[2] << std::endl;

//    std::cout << thisCell->GetPointId(0) << std::endl;
//    std::cout << thisCell->GetPointId(1) << std::endl;
//    std::cout << thisCell->GetPointId(2) << std::endl;
//    std::cout << thisCell->GetPointId(3) << std::endl;

    ImageIterator imageI(image, tetraRegion);
    DFImageIterator dfImageI(dfImage, tetraRegion);
    imageI.GoToBegin(); dfImageI.GoToBegin();
    for(;!imageI.IsAtEnd();++imageI,++dfImageI){
      ImageType::PointType point;
      image->TransformIndexToPhysicalPoint(imageI.GetIndex(), point);
      double coord[3], closestPoint[3], pcoords[3], bc[4], dist;
      DFImageType::PixelType dfPixel;
      int subId, isInside;
      coord[0] = point[0];
      coord[1] = point[1];
      coord[2] = point[2];

      isInside = tetra->EvaluatePosition(&coord[0], &closestPoint[0],
        subId, pcoords, dist, &bc[0]);

      if(!isInside)
        continue;

      dfPixel[0] = bc[0]*dp0[0]+bc[1]*dp1[0]+bc[2]*dp2[0]+bc[3]*dp3[0];
      dfPixel[1] = bc[0]*dp0[1]+bc[1]*dp1[1]+bc[2]*dp2[1]+bc[3]*dp3[1];
      dfPixel[2] = bc[0]*dp0[2]+bc[1]*dp1[2]+bc[2]*dp2[2]+bc[3]*dp3[2];

      dfPixel[0] *= -1.;
      dfPixel[1] *= -1.;
      dfPixel[2] *= -1.;
      
      dfImageI.Set(dfPixel);
      maskImage->SetPixel(imageI.GetIndex(), 1);
    }
  }

  std::cout << "After the main loop" << std::endl;

  MaskFilterType::Pointer maskFilter = MaskFilterType::New();
  maskFilter->SetInput1(image);
  maskFilter->SetInput2(maskImage);
  
  WarpFilterType::Pointer warpFilter = WarpFilterType::New();
  warpFilter->SetInput(maskFilter->GetOutput());
  warpFilter->SetDeformationField(dfImage);
  warpFilter->SetOutputSpacing(image->GetSpacing());

  ImageWriterType::Pointer imageWriter = ImageWriterType::New();
  imageWriter->SetInput(maskImage);
  imageWriter->SetFileName(argv[4]);
  imageWriter->Update();

  DFImageIterator dfImageI(dfImage, dfImage->GetBufferedRegion());
  for(dfImageI.GoToBegin();!dfImageI.IsAtEnd();++dfImageI){
    dfImageI.Set(dfImageI.Get()*-1.);
  }

  DFImageWriterType::Pointer dfImageWriter = DFImageWriterType::New();
  dfImageWriter->SetInput(dfImage);
  dfImageWriter->SetFileName(argv[5]);
  dfImageWriter->Update();

  return 0;
}
