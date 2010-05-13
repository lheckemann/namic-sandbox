/* read binary image and deformation field; recover surface of the label;
 * apply deformation to the surface nodes; rasterize the resulting mesh on the
 * template image; calculate change image
 */

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkIterativeInverseDeformationFieldImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleCell.h"

#include "itkBinaryMask3DMeshSource.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkQuadEdgeMeshQuadricDecimation.h"
#include "itkQuadEdgeMeshSquaredEdgeLengthDecimation.h"
#include "itkQuadEdgeMeshNormalFilter.h"
#include "itkQuadEdgeMeshExtendedTraits.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkConstNeighborhoodIterator.h"

#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

#include "vtkMassProperties.h"
#include "vtkWarpVector.h"

#include "itkBinaryMask3DMeshSource.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkWarpMeshFilter.h"
#include "itkIterativeInverseDeformationFieldImageFilter.h"

#include "itkTriangleMeshToBinaryImageFilter.h"

#include "itkImageDuplicator.h"
#include "itkImageRegionIterator.h"

typedef itk::OrientedImage< float, 3> ImageType;
typedef itk::OrientedImage< itk::Vector<double,3>,3 > DeformationFieldType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileWriter< ImageType > WriterType;
typedef itk::ImageFileReader< DeformationFieldType > DFReaderType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;
typedef itk::IterativeInverseDeformationFieldImageFilter<DeformationFieldType,DeformationFieldType>
  DFInvertorType;
typedef itk::ImageDuplicator<ImageType> DupType;
typedef itk::ImageRegionIterator<ImageType> IteratorType;

typedef float CoordType;
typedef itk::Vector<CoordType,3> VectorType;
typedef itk::QuadEdgeMeshExtendedTraits <
    VectorType,
    3,
    2,
    CoordType,
    CoordType,
    VectorType,
    bool,
    bool > Traits;
//  typedef itk::QuadEdgeMesh<CoordType, 3>              MeshType;
//  typedef itk::QuadEdgeMesh<CoordType, 3>              OutputMeshType;
typedef itk::QuadEdgeMesh < VectorType, 3, Traits > MeshType;
typedef itk::QuadEdgeMesh < VectorType, 3, Traits > OutputMeshType;
typedef MeshType::PointType                       PointType;
//  typedef itk::VTKPolyDataWriter<MeshType> MeshWriterType;

typedef itk::BinaryMask3DMeshSource< ImageType, MeshType >   MeshSourceType;
typedef itk::WarpMeshFilter<MeshType,MeshType,DeformationFieldType> MeshWarperType;
typedef OutputMeshType::PointsContainer::Iterator PointsIterator;
typedef OutputMeshType::CellsContainer::Iterator CellsIterator;
typedef OutputMeshType::PointDataContainerIterator PointDataIterator;
typedef itk::ImageRegionIterator<DeformationFieldType> DFIterType;

typedef itk::TriangleCell<MeshType::CellType> TriangleType;
  
typedef itk::TriangleMeshToBinaryImageFilter<MeshType,ImageType> Mesh2ImageType;

vtkPolyData* ITKMesh2PolyData(MeshType::Pointer mesh);
void WriteMesh(MeshType::Pointer mesh, const char* fname);
float SurfaceVolume(vtkPolyData* surf);
ImageType::Pointer MarkChanges(ImageType::Pointer tp0, ImageType::Pointer tp1);
void WriteImage(ImageType::Pointer image, const char* fname);

int main(int argc, char** argv){
  if(argc<5){
    std::cerr << "Usage: " << argv[0] << " binary_image deformation_field target output" << std::endl;
    return -1;
  }

  ReaderType::Pointer imageReader = ReaderType::New();
  DFReaderType::Pointer defReader = DFReaderType::New();
  imageReader->SetFileName(argv[1]);
  defReader->SetFileName(argv[2]);
  
  ImageType::Pointer movingImage = imageReader->GetOutput();
  DeformationFieldType::Pointer defField = defReader->GetOutput();
  imageReader->Update();
  defReader->Update();

  ThreshType::Pointer thresh = ThreshType::New();
  thresh->SetInput(imageReader->GetOutput());
  thresh->SetLowerThreshold(1);
  thresh->SetUpperThreshold(255);
  thresh->SetInsideValue(255);  

  ImageType::Pointer mask = thresh->GetOutput();
  MeshSourceType::Pointer meshSource = MeshSourceType::New();
  meshSource->SetInput(mask);
  meshSource->SetObjectValue(255);
  meshSource->Update();

  if(argc>5){
//    DFInvertorType::Pointer invert = DFInvertorType::New();
//    invert->SetInput(defField);
//    invert->SetNumberOfIterations(100);
//    invert->SetStopValue(0.5);
//    invert->Update();
//    std::cout << "Inverted DF" << std::endl;
//    defField = invert->GetOutput();
    
    DFIterType it(defField, defField->GetBufferedRegion());
    for(it.GoToBegin();!it.IsAtEnd();++it){
      DeformationFieldType::PixelType p;
      p = it.Get();
      p[0] *= -1.;
      p[1] *= -1.;
      p[2] *= -1.;
      it.Set(p);
    } 
    
  }

  MeshWarperType::Pointer mwarper = MeshWarperType::New();
  mwarper->SetInput(meshSource->GetOutput());
  mwarper->SetDeformationField(defField);
  mwarper->Update();
  
  MeshType::Pointer mesh = mwarper->GetOutput();

  ReaderType::Pointer templateReader = ReaderType::New();
  templateReader->SetFileName(argv[3]);
  templateReader->Update();

  Mesh2ImageType::Pointer m2i1 = Mesh2ImageType::New();
  Mesh2ImageType::Pointer m2i2 = Mesh2ImageType::New();
  ImageType::Pointer tp0image, tp1image;

  m2i1->SetInput(meshSource->GetOutput());
  m2i1->SetInfoImage(templateReader->GetOutput());
  m2i1->SetInsideValue(255);
  m2i1->Update();
  tp0image = m2i1->GetOutput();

  m2i2->SetInput(mwarper->GetOutput());
  m2i2->SetInfoImage(templateReader->GetOutput());
  m2i2->SetInsideValue(255);
  m2i2->Update();
  tp1image = m2i2->GetOutput();

  ImageType::Pointer changes = MarkChanges(tp0image, tp1image);
  WriteImage(changes, argv[4]);

  WriteMesh(meshSource->GetOutput(), "mc_mesh.vtk");
  WriteMesh(mwarper->GetOutput(), "warped_mesh.vtk");

  return 0;
}

void WriteImage(ImageType::Pointer image, const char* fname){
  WriterType::Pointer imageWriter1 = WriterType::New();
  imageWriter1->SetFileName(fname);
  imageWriter1->SetInput(image);
  imageWriter1->SetUseCompression(1);
  imageWriter1->Update();
}

vtkPolyData* ITKMesh2PolyData(MeshType::Pointer mesh){
  vtkPolyData *surface = vtkPolyData::New();
  vtkPoints *surfacePoints = vtkPoints::New();
//  vtkCellArray *surfaceCells = vtkCellArray::New();

  surfacePoints->SetNumberOfPoints(mesh->GetPoints()->Size());
//  surfaceCells->EstimateSize(mesh->GetCells()->Size(), 3);
  
  PointsIterator pIt = mesh->GetPoints()->Begin(), pItEnd = mesh->GetPoints()->End();
  CellsIterator cIt = mesh->GetCells()->Begin(), cItEnd = mesh->GetCells()->End();

  while(pIt!=pItEnd){
    OutputMeshType::PointType pt = pIt->Value();
    surfacePoints->SetPoint(pIt->Index(), pt[0], pt[1], pt[2]);
    
    ++pIt;
  }

  surface->SetPoints(surfacePoints);
  surface->Allocate();

  while(cIt!=cItEnd){
    OutputMeshType::CellType *cell = cIt->Value();
    OutputMeshType::CellType::PointIdIterator pidIt = cell->PointIdsBegin(); 
    vtkIdType cIds[3];
    cIds[0] = *pidIt;
    cIds[1] = *(pidIt+1);
    cIds[2] = *(pidIt+2);
    surface->InsertNextCell(VTK_TRIANGLE, 3, cIds);

    ++cIt;
  }
  return surface;
}

void WriteMesh(MeshType::Pointer mesh, const char* fname){
  vtkPolyData *vtksurf = ITKMesh2PolyData(mesh);
  vtkPolyDataWriter *pdw = vtkPolyDataWriter::New();
  pdw->SetFileName(fname);
  pdw->SetInput(vtksurf);
  pdw->Update();
}

float SurfaceVolume(vtkPolyData* surf){
  vtkMassProperties *mp = vtkMassProperties::New();  
  mp->SetInput(surf);
  mp->Update();
  return mp->GetVolume();
}
ImageType::Pointer MarkChanges(ImageType::Pointer tp0, ImageType::Pointer tp1){
  DupType::Pointer dup = DupType::New();
  dup->SetInputImage(tp0);
  dup->Update();

  ImageType::Pointer output = dup->GetOutput();

  IteratorType it0(tp0, tp0->GetBufferedRegion());
  IteratorType it1(tp1, tp1->GetBufferedRegion());
  IteratorType it(output, output->GetBufferedRegion());
  it0.GoToBegin(); it1.GoToBegin(); it.GoToBegin();
  for(;!it0.IsAtEnd();++it0,++it1,++it){
    if(it0.Get()==0 && it1.Get()!=0)
      // growth
      it.Set(14);
    else if(it0.Get()!=0 && it1.Get()==0)
      // shrinkage
      it.Set(12);    
    else
      it.Set(0);
  }
  return output;
}

