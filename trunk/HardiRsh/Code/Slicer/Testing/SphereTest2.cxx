/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: DiffuseSpheres.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This examples demonstrates the effect of diffuse lighting.
//
#include "vtkSphereSource.h"
#include "vtkSbiaSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"

int main()
{
  // The following lines create a sphere represented by polygons.
  //

  vtkSphereSource *sphere1 = vtkSphereSource::New();
  sphere1->SetThetaResolution(20);
  sphere1->SetPhiResolution(10);

  vtkSbiaSphereSource *sphere2 = vtkSbiaSphereSource::New();
  sphere2->SetThetaResolution(20);
  sphere2->SetPhiResolution(10);


  // The mapper is responsible for pushing the geometry into the graphics
  // library. It may also do color mapping, if scalars or other attributes
  // are defined.
  //
  vtkPolyDataMapper *sphereMapper1 = vtkPolyDataMapper::New();
  vtkPolyDataMapper *sphereMapper2 = vtkPolyDataMapper::New();
  sphereMapper1->SetInputConnection(sphere1->GetOutputPort());
  sphereMapper2->SetInputConnection(sphere2->GetOutputPort());

  // The actor is a grouping mechanism: besides the geometry (mapper), it
  // also has a property, transformation matrix, and/or texture map.
  // In this example we create eight different spheres (two rows of four
  // spheres) and set the specular lighting coefficients. A little ambient
  // is turned on so the sphere is not completely black on the back side.
  //
  vtkActor *sphereA1 = vtkActor::New();
  sphereA1->SetMapper(sphereMapper1);
  sphereA1->GetProperty()->SetColor(1,0,0);
  sphereA1->GetProperty()->SetAmbient(0.45);
  sphereA1->GetProperty()->SetDiffuse(0.125);
  sphereA1->GetProperty()->SetSpecular(0.125);
  sphereA1->GetProperty()->SetSpecularPower(5.0);

  vtkActor *sphereA2 = vtkActor::New();
  sphereA2->SetMapper(sphereMapper2);
  sphereA2->GetProperty()->SetColor(1,0,0);
  sphereA2->GetProperty()->SetAmbient(0.45);
  sphereA2->GetProperty()->SetDiffuse(0.25);
  sphereA2->GetProperty()->SetSpecular(0.25);
  sphereA2->GetProperty()->SetSpecularPower(10.0);
  sphereA2->AddPosition(0,1.25,0);

  // Create the graphics structure. The renderer renders into the
  // render window. The render window interactor captures mouse events
  // and will perform appropriate camera or actor manipulation
  // depending on the nature of the events.
  //
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren1);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size.
  //
  ren1->AddActor(sphereA1);
  ren1->SetBackground(0.1, 0.2, 0.4);
  renWin->SetSize(400, 200);

  ren1->AddActor(sphereA2);
  ren1->SetBackground(0.1, 0.2, 0.4);
  renWin->SetSize(400, 200);

  // Set up the lighting.
  //
  vtkLight *light = vtkLight::New();
  light->SetFocalPoint(1.875,0.6125,0);
  light->SetPosition(0.875,1.6125,1);
  ren1->AddLight(light);

  // We want to eliminate perspective effects on the apparent lighting.
  // Parallel camera projection will be used. To zoom in parallel projection
  // mode, the ParallelScale is set.
  //
  ren1->GetActiveCamera()->SetFocalPoint(0,0,0);
  ren1->GetActiveCamera()->SetPosition(0.5,0,0);
  ren1->GetActiveCamera()->SetViewUp(0,0,1);
//  ren1->GetActiveCamera()->ParallelProjectionOn();
  ren1->ResetCamera();
//  ren1->GetActiveCamera()->SetParallelScale(1.5);

  // This starts the event loop and invokes an initial render.
  //
  iren->Initialize();
  iren->Start();

  // Exiting from here, we have to delete all the instances that
  // have been created.
  //
  
  sphere1->Delete();
  sphere2->Delete();
  sphereA1->Delete();
  sphereA2->Delete();
  sphereMapper1->Delete();
  sphereMapper2->Delete();


  ren1->Delete();
  renWin->Delete();
  iren->Delete();
  light->Delete();

  return 0;
}




