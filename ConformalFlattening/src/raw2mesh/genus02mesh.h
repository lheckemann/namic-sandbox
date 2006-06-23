// This is the header file for the Conformal Flattenning mapping

// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

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
// #include "itkMeshSpatialObject.h"
// #include "itkSpatialObjectWriter.h"
// #include "itkDefaultDynamicMeshTraits.h"

//
// VTK headers
//
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

//
// vnl headers
//
#include <vcl_iostream.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/algo/vnl_conjugate_gradient.h>

// This define is needed to deal with double/float changes in VTK
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

const unsigned int pointDimension   = 3;
const unsigned int maxCellDimension = 3;

typedef itk::Image< unsigned char, pointDimension > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::Point<vtkFloatingPointType, pointDimension> ItkPoint;
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
typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType > MeshSourceType;
typedef MeshType::PointsContainer::ConstIterator PointIterator;
typedef MeshType::CellType CellType;
typedef MeshType::CellsContainer::ConstIterator CellIterator;
typedef CellType::PointIdIterator PointIdIterator;

//functions definition:
MeshType::Pointer vtkPolyDataToITKMesh(vtkPolyData* polyData);
vtkPolyData* readDataToPolyData(char* fName);
vtkPolyData* ITKMeshToVtkPolyData(MeshType::Pointer mesh);
void Display(vtkPolyData* polyData);
MeshType::Pointer mapping(MeshType::Pointer mesh);
void getDb(MeshType::Pointer mesh, 
           vnl_sparse_matrix<vtkFloatingPointType> &D,
           vnl_vector<vtkFloatingPointType> &bR,
           vnl_vector<vtkFloatingPointType> &bI);
