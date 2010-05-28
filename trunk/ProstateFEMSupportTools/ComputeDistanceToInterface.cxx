/*
 *  Add a float array to vtk mesh defining the approximate distance to the
 *  interface between labels 4 and 5 in the input label image
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
#include "vtkPoints.h"
#include "vtkIntArray.h"

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkTetrahedralMeshWriter.h"
#include "vtkUnstructuredGridWriter.h"
#include "MeshUtil.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

typedef float PixelType;
typedef itk::OrientedImage<PixelType,3> ImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageFileWriter<ImageType> ImageWriterType;
typedef itk::ImageRegionIteratorWithIndex<ImageType> ImageIterator;
typedef itk::SignedMaurerDistanceMapImageFilter<ImageType,ImageType> DistanceFilterType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThresholdType;

int main(int argc, char **argv){

  if(argc<3){
    std::cerr << "Usage: " << "input_image input_mesh output_mesh_vtk" << std::endl;
    return -1;
  }


  char* inputImageName = argv[1];
  char* inputMeshName = argv[2];
  char* outVTKmesh = argv[3];
  
  std::cout << "Input image: " << inputImageName << std::endl;
  std::cout << "Input mesh: " << inputMeshName << std::endl;
  std::cout << "Output VTK mesh: " << outVTKmesh << std::endl;

  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageName);
  imageReader->Update();
  ImageType::Pointer image = imageReader->GetOutput();

  vtkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SetFileName(inputMeshName);
  meshReader->Update();
  vtkUnstructuredGrid *mesh = meshReader->GetOutput();
  std::cout << "Mesh read" << std::endl;

  vtkPoints *meshPoints = mesh->GetPoints();
  vtkCellArray *cells = mesh->GetCells();
  vtkFloatArray *distance = vtkFloatArray::New();

  // allocate an array to keep tissue type for each cell
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(mesh->GetNumberOfPoints());
  distance->SetName("DistanceToInterface");
  mesh->GetPointData()->AddArray(distance);
  distance->Delete();

  ImageIterator iter(image, image->GetBufferedRegion());
  ImageType::SizeType size = image->GetBufferedRegion().GetSize();
  iter.GoToBegin();
  for(;!iter.IsAtEnd();++iter){
    if(!iter.Get())
      continue;

    ImageType::IndexType idx = iter.GetIndex();

    // reset pixels near the boundary
    if(!idx[0] || !idx[1] || !idx[2] ||
        idx[0] == size[0]-1 ||
        idx[1] == size[1]-1 ||
        idx[2] == size[2]-1){
      iter.Set(0);
      continue;
    }

    ImageType::IndexType idxN[6]; // look at 6 neighbors that share a voxel face

    idxN[0] = idx;
    idxN[1] = idx;
    idxN[2] = idx;
    idxN[3] = idx;
    idxN[4] = idx;
    idxN[5] = idx;

    idxN[0][0]--;
    idxN[1][0]++;
    idxN[2][1]--;
    idxN[3][1]++;
    idxN[4][2]--;
    idxN[5][2]++;

    ImageType::PixelType p = iter.Get();
    ImageType::PixelType pN[6];
    pN[0] = image->GetPixel(idxN[0]);
    pN[1] = image->GetPixel(idxN[1]);
    pN[2] = image->GetPixel(idxN[2]);
    pN[3] = image->GetPixel(idxN[3]);
    pN[4] = image->GetPixel(idxN[4]);
    pN[5] = image->GetPixel(idxN[5]);

    if( (pN[0] && pN[0] != p) ||
        (pN[1] && pN[1] != p) ||
        (pN[2] && pN[2] != p) ||
        (pN[3] && pN[3] != p) ||
        (pN[4] && pN[4] != p) ||
        (pN[5] && pN[5] != p)){
      continue;
    } else
      iter.Set(0);
  }

  ThresholdType::Pointer t = ThresholdType::New();
  t->SetInput(image);
  t->SetInsideValue(1);
  t->SetOutsideValue(0);
  t->SetLowerThreshold(1);
  t->SetUpperThreshold(5);
  t->Update();

  DistanceFilterType::Pointer dt = DistanceFilterType::New();
  dt->SetInput(image);
  dt->SetSquaredDistance(0);
  dt->SetUseImageSpacing(1);
  dt->Update();


  ImageWriterType::Pointer w = ImageWriterType::New();
  w->SetFileName("test.nrrd");
  w->SetInput(dt->GetOutput());
  w->Update();

  for(int i=0;i<mesh->GetNumberOfPoints();i++){
    ImageType::PointType pt;
    ImageType::IndexType idx;
    ImageType::PixelType p;

    // account for the differences in coordinate space
    pt[0] = -mesh->GetPoint(i)[0];
    pt[1] = -mesh->GetPoint(i)[1];
    pt[2] = mesh->GetPoint(i)[2];
    
    if(!image->TransformPhysicalPointToIndex(pt, idx)){
      std::cerr << "ERROR: mesh vertex is outside input image! Check the inputs" << std::endl;
      return -1;
    }
    
    p = dt->GetOutput()->GetPixel(idx);
    if(p<0)
      p = 0;
    distance->InsertTuple(i, &p);
  }

  vtkUnstructuredGridWriter *meshWriter = vtkUnstructuredGridWriter::New();
  meshWriter->SetInput(mesh);
  meshWriter->SetFileName(outVTKmesh);
  meshWriter->Update();

  return 0;
}

