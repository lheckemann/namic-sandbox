// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

//
// ITK headers
//
#include <iostream>
#include <fstream>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMesh.h"
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


// This define is needed to deal with double/float changes in VTK
#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif


// IMAGE STUFF
typedef itk::Image< unsigned char, 3 > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::ConnectedComponentImageFilter< InputImageType, InputImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< InputImageType, InputImageType > RelabelType;
typedef itk::LabelStatisticsImageFilter<InputImageType, InputImageType> LabelStatisticsImageFilterType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdFilterType;


// MESH STUFF
const unsigned int pointDimension   = 3;
const unsigned int maxCellDimension = 3;

typedef itk::DefaultStaticMeshTraits< 
  vtkFloatingPointType, 
  pointDimension,
  maxCellDimension, 
  vtkFloatingPointType, 
  vtkFloatingPointType  >       MeshTraits;
  
typedef itk::Mesh<
  vtkFloatingPointType,
  pointDimension,
  MeshTraits >                  MeshType;

typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType > MeshSourceType;

// FUNCTIONS
vtkPolyData* ITKMeshToVtkPolyData(MeshType::Pointer mesh);
void Display(vtkPolyData* polyData);


