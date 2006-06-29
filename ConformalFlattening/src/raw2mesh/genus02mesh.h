// This is the header file for the Conformal Flattenning mapping

// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkConformalFlatteningFilter.h"

//
//
//  This program illustrates how to convert 
//  a vtkPolyData structure into an itk::Mesh.
//  
//  The example is tailored for the case in which the vtkPolyData
//  only containes triangle strips and triangles.
//
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

//
// ITK Headers
// 
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMesh.h"
#include "itkLineCell.h"
#include "itkTriangleCell.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"

//
// VTK headers
//
#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkMarchingCubes.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkQuadricDecimation.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCurvatures.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageData.h"
#include "vtkImageShrink3D.h"

// This define is needed to deal with double/float changes in VTK
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

const unsigned int pointDimension   = 3;
const unsigned int maxCellDimension = 3;

typedef itk::Image< unsigned char, pointDimension > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::DefaultStaticMeshTraits< 
  vtkFloatingPointType, 
  pointDimension,
  maxCellDimension, 
  vtkFloatingPointType, 
  vtkFloatingPointType  >       MeshTraits;
typedef itk::Mesh<
  vtkFloatingPointType, 
  pointDimension, 
  MeshTraits              >     MeshType;
typedef MeshType::PointsContainer::ConstIterator PointIterator;
typedef MeshType::CellType CellType;
typedef CellType::PointIdIterator PointIdIterator;
typedef MeshType::CellsContainer::ConstIterator CellIterator;
typedef itk::Point< vtkFloatingPointType, pointDimension > ItkPoint;

// Function definitions
MeshType::Pointer vtkPolyDataToITKMesh( vtkPolyData* polyData );
vtkPolyData* ITKMeshToVtkPolyData( MeshType::Pointer mesh );
void Display( vtkPolyData* polyData );

// THE FILTER
typedef itk::ConformalFlatteningFilter< MeshType, MeshType>  ConformalFlatteningFilterType;
typedef itk::VTKImageExport< InputImageType > VTKImageExportType;
typedef itk::VTKImageImport< InputImageType > VTKImageImportType;
void ConnectITKToVTK( VTKImageExportType* in, vtkImageImport* out );
