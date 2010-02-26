/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTestingMacros.h,v $
  Language:  C++
  Date:      $Date: 2009-05-09 17:40:20 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __RegistrationMonitor_h
#define __RegistrationMonitor_h

#include "itkCommand.h"

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"


/** \class RegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a registration process. 
 */
class RegistrationMonitor
{
public:
  
  typedef RegistrationMonitor  Self;

  RegistrationMonitor();
  virtual ~RegistrationMonitor();

  void SetFixedSurface( vtkPolyData* surface );
  void SetMovingSurface( vtkPolyData* surface );

  void SetNumberOfIterationsPerUpdate( unsigned int number );
    
  void SetVerbose( bool );

  void SetScreenShotsBaseFileName( const char * screenShotFileName );

protected:

  // These methods will only be called by the Observer
  virtual void Update();
  virtual void StartVisualization();

  virtual void SetMovingActorMatrix( vtkMatrix4x4 * matrix );

  virtual void RefreshRendering();
  virtual void UpdateDataBeforeRendering() = 0;

  vtkPoints * GetFixedSurfacePoints();

  virtual void SaveScreenShot();

private:
  
  vtkSmartPointer< vtkPolyData >         FixedSurface;
  vtkSmartPointer< vtkActor >            FixedActor;
  vtkSmartPointer< vtkProperty >         FixedProperty;
  vtkSmartPointer< vtkPolyDataMapper >   FixedMapper;

  vtkSmartPointer< vtkPolyData >         MovingSurface;
  vtkSmartPointer< vtkActor >            MovingActor;
  vtkSmartPointer< vtkProperty >         MovingProperty;
  vtkSmartPointer< vtkPolyDataMapper >   MovingMapper;

  // Visualization pipeline
  vtkSmartPointer< vtkRenderer >         FixedRenderer;
  vtkSmartPointer< vtkRenderer >         MovingRenderer;
  vtkSmartPointer< vtkRenderWindow >     RenderWindow;
  vtkSmartPointer< vtkRenderWindowInteractor >   RenderWindowInteractor;

  typedef itk::SimpleMemberCommand< Self >  ObserverType;

  ObserverType::Pointer           IterationObserver;
  ObserverType::Pointer           StartObserver;

  unsigned int                    CurrentIterationNumber;
  unsigned int                    NumberOfIterationsPerUpdate;
 
  bool                            Verbose;

  std::string                     ScreenShotsBaseFileName;
};

#endif
