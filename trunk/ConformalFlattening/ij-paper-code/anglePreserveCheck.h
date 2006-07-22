// In the Conformal Flattening map, the transform function is the
// solution of a partial differential equation(PDE). The PDE is solved
// by Finite Element Method which turns the original problem into a
// system of linear equation Dx=b. In this section we are to generate
// the matrix D (b will be computed in another program) which will
// feed into the linear equation solver part.

// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

//
// ITK Headers
// 
#include "itkMesh.h"
#include "itkLineCell.h"
#include "itkTriangleCell.h"

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


typedef MeshType::PointsContainer::ConstIterator PointIterator;


typedef MeshType::CellType CellType;
typedef MeshType::CellsContainer::ConstIterator CellIterator;

typedef CellType::PointIdIterator PointIdIterator;



//functions definition:
MeshType::Pointer vtkPolyDataToITKMesh(vtkPolyData* polyData);
vtkPolyData* readDataToPolyData(char* fName);
vtkPolyData* ITKMeshToVtkPolyData(MeshType::Pointer mesh);
void Display(vtkPolyData* polyData);

void anglePreserveCheck( MeshType::Pointer mesh1, MeshType::Pointer mesh2 );
