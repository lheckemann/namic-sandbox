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
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkFibonacciSphereSource.h"
#include "vtkPolyDataWriter.h"
#include "vtkDebugLeaks.h"

int main( int argc, char *argv[] )
{

  if (argc < 2 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " outputFilename.vtk" << std::endl;
    return 1;
    }

  vtkRenderer *renderer = vtkRenderer::New();

  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

  vtkFibonacciSphereSource *sphere = vtkFibonacciSphereSource::New();
    sphere->SetTotalNumberOfPoints(1000); 
    sphere->SetRadius(1.0);
    sphere->Update();

  vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
    writer->SetInput( sphere->GetOutput() );
    writer->SetFileName( argv[1] );
    writer->Write();

  vtkPolyDataMapper *sphereMapper = vtkPolyDataMapper::New();
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkActor *sphereActor = vtkActor::New();
    sphereActor->SetMapper(sphereMapper);
    
 
  renderer->AddActor(sphereActor);
  renderer->SetBackground(1,1,1);
  renWin->SetSize(300,300);

  // interact with data
  renWin->Render();
  iren->Start();

  // Clean up
  writer->Delete();
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  sphere->Delete();
  sphereMapper->Delete();
  sphereActor->Delete();

  return 0;
}
