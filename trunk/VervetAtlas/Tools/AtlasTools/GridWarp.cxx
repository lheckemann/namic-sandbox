/*==================================================================

  This tool is derived from BRAINSFitDemonsWarp. It's purpose is to 
  construct a VTK unstructured grid file that would visualize a bspline 
  deformation.

==================================================================*/

#include <iostream>
#include "itkVector.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkTransformFileReader.h"
#include <itkBSplineDeformableTransform.h>

#include "itkBinaryThresholdImageFilter.h"


#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"

#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"

namespace GenericTransformImageNS {
static const unsigned int SpaceDimension = 3;
static const unsigned int SplineOrder = 3;
}


typedef double CoordinateRepType;
typedef itk::BSplineDeformableTransform<
  CoordinateRepType,
  GenericTransformImageNS::SpaceDimension,
  GenericTransformImageNS::SplineOrder > BSplineTransformType;


int main(int argc, char *argv[])
{
  if(argc!=3){
    std::cout << "Usage: " << argv[0] << " bspline_tfm output_vtk" << std::endl;
    return -1;
  }

  itk::TransformFileReader::Pointer tfmReader = itk::TransformFileReader::New();
  tfmReader->SetFileName(argv[1]);
  tfmReader->Update();
  
  typedef itk::TransformFileReader::TransformListType *TransformListType;
  TransformListType transforms = tfmReader->GetTransformList();
  std::cout << "Number of transforms = " << transforms->size() << std::endl;
  
  itk::TransformFileReader::TransformListType::const_iterator it
    = transforms->begin();

  BSplineTransformType::Pointer itkBSplineTransform;
  itkBSplineTransform = static_cast<BSplineTransformType *>( ( *it ).GetPointer() );

  if (transforms->size() != 1){
    std::cerr << "Expect single bspline transform on input!" << std::endl;
    return -1;
  }

  

  BSplineTransformType::ParametersType fixedParams;
  BSplineTransformType::ParametersType params;
  fixedParams = itkBSplineTransform->GetFixedParameters();
  params = itkBSplineTransform->GetParameters();

  itk::Indent indent;
  std::cout << itkBSplineTransform << std::endl;

  int gridRegionSize[3];
  double gridOrigin[3];
  double gridSpacing[3];
  double gridDirection[9];

  gridRegionSize[0] = fixedParams[0];
  gridRegionSize[1] = fixedParams[1];
  gridRegionSize[2] = fixedParams[2];
  gridOrigin[0] = fixedParams[3];
  gridOrigin[1] = fixedParams[4];
  gridOrigin[2] = fixedParams[5];
  gridSpacing[0] = fixedParams[6];
  gridSpacing[1] = fixedParams[7];
  gridSpacing[2] = fixedParams[8];
  gridDirection[0] = fixedParams[9];
  gridDirection[1] = fixedParams[10];
  gridDirection[2] = fixedParams[11];
  gridDirection[3] = fixedParams[12];
  gridDirection[4] = fixedParams[13];
  gridDirection[5] = fixedParams[14];
  gridDirection[6] = fixedParams[15];
  gridDirection[7] = fixedParams[16];
  gridDirection[8] = fixedParams[17];

  std::cout << "Grid size: " << gridRegionSize[0] << ", " << gridRegionSize[1] << ", " << gridRegionSize[2] << std::endl;
  std::cout << "Grid spacing: " << gridSpacing[0] << ", " << gridSpacing[1] << ", " << gridSpacing[2] << std::endl;
  
  vtkUnstructuredGrid *ug = vtkUnstructuredGrid::New();
  vtkPoints *pts = vtkPoints::New();
  vtkCellArray *cells = vtkCellArray::New();
  vtkFloatArray *def = vtkFloatArray::New();
  
  int totalGridPts = gridRegionSize[0]*gridRegionSize[1]*gridRegionSize[2];
  pts->SetNumberOfPoints(totalGridPts);
  cells->EstimateSize(totalGridPts, 1);
  def->SetNumberOfComponents(3);
  def->SetName("Displacements");
  ug->GetPointData()->AddArray(def);
  def->Delete();

  int i,j,k,numPts = 0;
  int ctypes[totalGridPts];
  vtkIdType id = 0;
  for(k=0;k<gridRegionSize[2];k++){
    for(j=0;j<gridRegionSize[1];j++){
      for(i=0;i<gridRegionSize[0];i++){
        pts->SetPoint(numPts, gridOrigin[0]+gridSpacing[0]*i, 
          gridOrigin[1]+gridSpacing[1]*j, gridOrigin[2]+gridSpacing[2]*k);
        cells->InsertNextCell(1, &id);
        def->InsertTuple3(numPts, params[numPts], params[numPts+totalGridPts], 
          params[numPts+totalGridPts*2]);
        ctypes[numPts] = VTK_VERTEX;
        numPts++;id++;
      }
    }
  }
  ug->SetPoints(pts);
  ug->SetCells(ctypes, cells);

  vtkUnstructuredGridWriter *gw = vtkUnstructuredGridWriter::New();
  gw->SetFileName(argv[2]);
  gw->SetInput(ug);
  gw->Update();

  return EXIT_SUCCESS;
}
