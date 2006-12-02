#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <stdlib.h>
#include <iostream>

//
// ITK Headers
// 
#include "itkMesh.h"
#include "itkSWaveletSource.h"
#include "itkDefaultStaticMeshTraits.h"
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



// This define is needed to deal with double/float changes in VTK
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

const unsigned int pointDimension   = 3;
const unsigned int maxCellDimension = 2;

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


//typedef itk::Mesh<float, 3>   MeshType;

typedef itk::SWaveletSource< MeshType >  SphereMeshSourceType;
//typedef itk::SphereMeshSource< MeshType >  SphereMeshSourceType;

typedef SphereMeshSourceType::PointType   PointType;
typedef SphereMeshSourceType::VectorType  VectorType;


#include "displayMesh.txx"
