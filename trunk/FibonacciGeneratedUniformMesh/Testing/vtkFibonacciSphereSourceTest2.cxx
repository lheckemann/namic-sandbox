/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: CellLocator.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPolyData.h"
#include "vtkFibonacciSphereSource.h"
#include "vtkPolyDataWriter.h"
#include "vtkDebugLeaks.h"

int main( int argc, char *argv[] )
{

  if (argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " outputFilename.vtk";
    std::cerr << " [numberOfPoints] " << std::endl;
    return 1;
    }

  int numberOfPoints = 1000;

  if (argc > 2)
    {
    numberOfPoints = atoi( argv[2] );
    }

  vtkFibonacciSphereSource *sphere = vtkFibonacciSphereSource::New();
    sphere->SetTotalNumberOfPoints(numberOfPoints); 
    sphere->SetRadius(1.0);
    sphere->Update();

  sphere->Print( std::cout );

  vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
    writer->SetInput( sphere->GetOutput() );
    writer->SetFileName( argv[1] );
    writer->Write();

  // Clean up
  writer->Delete();
  sphere->Delete();

  return 0;
}
