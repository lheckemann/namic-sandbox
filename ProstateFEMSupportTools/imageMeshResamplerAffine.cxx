/*
 *  Resample image based on displacements defined at mesh vertices
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

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMaskImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkChangeInformationImageFilter.h"
#include "itkVector.h"
#include "itkShiftScaleInPlaceImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "itkTransformFileReader.h"
#include "itkAffineTransform.h"

#include "itkImageDuplicator.h"

typedef float PixelType;
typedef itk::Vector<double,3> DFPixelType;
typedef itk::OrientedImage<PixelType,3> ImageType;
typedef itk::OrientedImage<DFPixelType,3> DFImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageFileWriter<ImageType> ImageWriterType;
typedef itk::ImageFileWriter<DFImageType> DFImageWriterType;
typedef itk::MaskImageFilter<ImageType,ImageType,ImageType> MaskFilterType;
typedef itk::WarpImageFilter<ImageType,ImageType,DFImageType> WarpFilterType;
typedef itk::ShiftScaleInPlaceImageFilter<DFImageType> ScaleFilterType;
typedef itk::ChangeInformationImageFilter<ImageType> ChangeInfoType;
typedef itk::ImageDuplicator<ImageType> DupType;
typedef itk::TransformFileReader TransformReaderType;
typedef itk::AffineTransform<double> TransformType;

typedef itk::NearestNeighborInterpolateImageFunction<ImageType,double> NNType;

typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ImageIterator;
typedef itk::ImageRegionIterator<DFImageType> DFImageIterator;

double getNorm(double* vec){
  return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

int main(int argc, char **argv){

  // TODO: add option to choose interpolator
  if(argc<7){
    std::cerr << "Usage: " << "input_image input_mesh affine_transform reference_image output_image output_image_df [0|1]" << std::endl;
    return -1;
  }

  char* inputImageName = argv[1];
  char* inputMeshName = argv[2];
  char* affineTfmName = argv[3];
  char* refImageName = argv[4];
  char* outImageName = argv[5];
  char* outDFName = argv[6];

  bool useNearestNeighbor = false; // linear by default
  
  if(argc>7)
    useNearestNeighbor = (bool) atoi(argv[7]);

  TransformReaderType::Pointer tfmReader = TransformReaderType::New();
  tfmReader->SetFileName(affineTfmName);
  tfmReader->Update();
  
  TransformType::Pointer affineTfm = TransformType::New();

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


  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageName);
  imageReader->Update();

  ImageReaderType::Pointer refImageReader = ImageReaderType::New();
  refImageReader->SetFileName(refImageName);
  refImageReader->Update();

  ImageType::Pointer refImage = refImageReader->GetOutput();

  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName(inputMeshName);
  meshReader->Update();

  vtkUnstructuredGrid *mesh = meshReader->GetOutput();
  vtkDataArray *meshDeformation = mesh->GetPointData()->GetArray("Displacement");
  ImageType::Pointer inputImage = imageReader->GetOutput();

  DFImageType::Pointer dfImage = DFImageType::New();
  DFImageType::RegionType region;
  region.SetSize(refImage->GetBufferedRegion().GetSize());
  region.SetIndex(refImage->GetBufferedRegion().GetIndex());
  dfImage->SetRegions(region);
  dfImage->Allocate();
  dfImage->SetSpacing(refImage->GetSpacing());
  dfImage->SetDirection(refImage->GetDirection());
  dfImage->SetOrigin(refImage->GetOrigin());

  DupType::Pointer dup1 = DupType::New();
  dup1->SetInputImage(refImage);
  dup1->Update();
  ImageType::Pointer outputImage = dup1->GetOutput();
  outputImage->FillBuffer(0);

  DupType::Pointer dup2 = DupType::New();
  dup2->SetInputImage(refImage);
  dup2->Update();
  ImageType::Pointer outputImageMask = dup2->GetOutput();
  outputImageMask->FillBuffer(0);

  vtkPoints *meshPoints = mesh->GetPoints();
  vtkCellArray *cells = mesh->GetCells();
  vtkIdType npts, *pts;
  int i, cellId=0;

  for(i=0;i<mesh->GetNumberOfPoints();i++){
    double thisPt[3], dp[3];

    memcpy(&thisPt[0], meshPoints->GetPoint(i), sizeof(double)*3);
    memcpy(&dp[0], meshDeformation->GetTuple3(i), sizeof(double)*3);

    thisPt[0] += dp[0];
    thisPt[1] += dp[1];
    thisPt[2] += dp[2];

    thisPt[0] *= -1;
    thisPt[1] *= -1;

    meshPoints->SetPoint(i,thisPt[0],thisPt[1],thisPt[2]);
  }
  
//
//
//

  for(cells->InitTraversal();cells->GetNextCell(npts,pts);cellId++){
    assert(npts==4);
    vtkTetra* tetra = vtkTetra::New();
    vtkCell* thisCell = mesh->GetCell(cellId);
    tetra->Initialize(npts, pts, meshPoints);
//    std::cout << tetra->GetPointId(0) << " " << tetra->GetPointId(1) <<
//      " " << tetra->GetPointId(2) << " " << tetra->GetPointId(3) << std::endl;
    vtkPoints *tetraPoints = tetra->GetPoints();
    ImageType::IndexType bbMin, bbMax;
    bbMin[0] = refImage->GetBufferedRegion().GetSize()[0];
    bbMin[1] = refImage->GetBufferedRegion().GetSize()[1];
    bbMin[2] = refImage->GetBufferedRegion().GetSize()[2];
    bbMax = refImage->GetBufferedRegion().GetIndex();

    for(i=0;i<4;i++){
      ImageType::PointType point;
      ImageType::IndexType index;
      point[0] = tetraPoints->GetPoint(i)[0];
      point[1] = tetraPoints->GetPoint(i)[1];
      point[2] = tetraPoints->GetPoint(i)[2];
//      std::cout << point[0] << " " << point[1] << " " << point[2] << std::endl;
      if(!refImage->TransformPhysicalPointToIndex(point, index)){
        std::cerr << "Point is outside the image" << std::endl;
        return -1;
      }
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

    ImageIterator imageI(refImage, tetraRegion);
    DFImageIterator dfImageI(dfImage, tetraRegion);
    imageI.GoToBegin(); dfImageI.GoToBegin();
    for(;!imageI.IsAtEnd();++imageI,++dfImageI){
      ImageType::PointType pointRef, pointIn;
      ImageType::IndexType idxOut;
      refImage->TransformIndexToPhysicalPoint(imageI.GetIndex(), pointRef);
      double coord[3], closestPoint[3], pcoords[3], bc[4], dist;
      DFImageType::PixelType dfPixel;
      int subId, isInside;
      coord[0] = pointRef[0];
      coord[1] = pointRef[1];
      coord[2] = pointRef[2];

      isInside = tetra->EvaluatePosition(&coord[0], &closestPoint[0],
        subId, pcoords, dist, &bc[0]);

      if(!isInside)
        continue;

      dfPixel[0] = (bc[0]*dp0[0]+bc[1]*dp1[0]+bc[2]*dp2[0]+bc[3]*dp3[0]);
      dfPixel[1] = (bc[0]*dp0[1]+bc[1]*dp1[1]+bc[2]*dp2[1]+bc[3]*dp3[1]);
      dfPixel[2] = -(bc[0]*dp0[2]+bc[1]*dp1[2]+bc[2]*dp2[2]+bc[3]*dp3[2]);

      dfImageI.Set(dfPixel);

      pointIn[0] = pointRef[0]+dfPixel[0];
      pointIn[1] = pointRef[1]+dfPixel[1];
      pointIn[2] = pointRef[2]+dfPixel[2];

      ImageType::PointType affTfmPt;
      affTfmPt = affineTfm->TransformPoint(pointIn);
      dfPixel[0] = affTfmPt[0]-pointRef[0];
      dfPixel[1] = affTfmPt[1]-pointRef[1];
      dfPixel[2] = affTfmPt[2]-pointRef[2];
      dfImageI.Set(dfPixel);

      if(!inputImage->TransformPhysicalPointToIndex(affTfmPt, idxOut)){
        std::cerr << "Point is outside the input image" << std::endl;
        return -2;
      }
      outputImage->SetPixel(imageI.GetIndex(), inputImage->GetPixel(idxOut));
      outputImageMask->SetPixel(imageI.GetIndex(), 1);
    }
  }

//
//
//

  WarpFilterType::Pointer warpFilter = WarpFilterType::New();
  if(useNearestNeighbor){
    NNType::Pointer nnInterp = NNType::New();
    warpFilter->SetInterpolator(nnInterp);
    std::cout << "Using nearest neighbor interpolator" << std::endl;
  }

  warpFilter->SetInput(inputImage);
  warpFilter->SetDeformationField(dfImage);
  warpFilter->SetOutputSpacing(refImage->GetSpacing());
  warpFilter->SetOutputDirection(refImage->GetDirection());
  warpFilter->SetOutputOrigin(refImage->GetOrigin());

  MaskFilterType::Pointer maskFilter = MaskFilterType::New();
  maskFilter->SetInput1(warpFilter->GetOutput());
  maskFilter->SetInput2(outputImageMask);
  maskFilter->Update();
  
  /*
  ImageWriterType::Pointer wimageWriter = ImageWriterType::New();
  wimageWriter->SetInput(maskFilter->GetOutput());
  wimageWriter->SetFileName("warped_image.nrrd");
  wimageWriter->Update();
  */

  ImageWriterType::Pointer imageWriter = ImageWriterType::New();
  imageWriter->SetInput(maskFilter->GetOutput());
  imageWriter->SetFileName(outImageName);
  imageWriter->Update();

  DFImageWriterType::Pointer dfImageWriter = DFImageWriterType::New();
  dfImageWriter->SetInput(dfImage);
  dfImageWriter->SetFileName(outDFName);
  dfImageWriter->Update();

  return 0;
}
