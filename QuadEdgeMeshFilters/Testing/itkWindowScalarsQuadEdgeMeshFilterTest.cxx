/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWindowScalarsQuadEdgeMeshFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2010-09-17 19:46:31 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadEdgeMesh.h"
#include "itkWindowScalarsQuadEdgeMeshFilter.h"

#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

int main( int argc, char * argv [] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputPolyData outputPolyData";
    std::cerr << "window_min window_max";
    std::cerr << "output_min output_max";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef float PixelType;
  const unsigned int Dimension = 3;

  // Declaration of the type of Mesh
  typedef itk::QuadEdgeMesh< PixelType, Dimension >  MeshType;

  // Here read a mesh from a file
  typedef itk::QuadEdgeMeshVTKPolyDataReader< MeshType >  ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::WindowScalarsQuadEdgeMeshFilter< MeshType, MeshType> MeshFilterType;
  MeshFilterType::Pointer filter = MeshFilterType::New();
  filter -> SetInput(reader->GetOutput());
  filter -> SetWindowMinimum(atof(argv[3]));
  filter -> SetWindowMaximum(atof(argv[4]));
  filter -> SetOutputMinimum(atof(argv[5]));
  filter -> SetOutputMaximum(atof(argv[6]));
  filter -> Update();

  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName(argv[2]);
  writer->Update(); 

  return EXIT_SUCCESS;
}
