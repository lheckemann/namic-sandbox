/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshBoundarySmoothFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-10 19:46:31 $
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

#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "itkQuadEdgeMeshBoundarySmoothFilter.h"

int main( int argc, char** argv )
{
  if( argc != 6 )
    {
    std::cout <<"It requires 5 arguments" <<std::endl;
    std::cout <<"1-Input FileName 2-Input FileName" <<std::endl;
    std::cout <<"1-Output FileName 2-Output FileName" <<std::endl;
    std::cout <<"NumberOfIterations" <<std::endl;
    return EXIT_FAILURE;
    }

  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >               MeshType;
  typedef MeshType::Pointer                           MeshPointer;
  typedef itk::VTKPolyDataReader< MeshType >          ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >          WriterType;
  
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  reader1->SetFileName( argv[1] );
  reader2->SetFileName( argv[2] );
  try
    {
    reader1->Update( );
    reader2->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::QuadEdgeMeshBoundarySmoothFilter< MeshType, MeshType > SmoothFilterType;

  SmoothFilterType::Pointer smoothFilter = SmoothFilterType::New();

  smoothFilter->SetInputMesh1 (reader1->GetOutput());
  smoothFilter->SetInputMesh2 (reader2->GetOutput());

  smoothFilter->SetIterations (atoi(argv[5]));

  smoothFilter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( smoothFilter->GetOutputMesh1() );
  writer->SetFileName( argv[3] );
  writer->Update();
  
  writer->SetInput( smoothFilter->GetOutputMesh2() );
  writer->SetFileName( argv[4] );
  writer->Update();

  return EXIT_SUCCESS;
}
