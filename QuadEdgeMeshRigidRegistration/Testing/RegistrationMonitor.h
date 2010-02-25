/*=========================================================================

  Program:   Surface Registration Program
  Module:    $RCSfile: RegistrationMonitor.h,v $

  Copyright (c) Kitware Inc. 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __RegistrationMonitor_h
#define __RegistrationMonitor_h

#include "itkMatrixOffsetTransformBase.h"
#include "itkCommand.h"
#include "itkOptimizer.h"

#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"


/** \class RegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a registration process. 
 */
class RegistrationMonitor
{
public:
  
  typedef RegistrationMonitor  Self;

  typedef itk::Optimizer       OptimizerType;

  typedef itk::MatrixOffsetTransformBase< double, 3, 3 > TransformType;

  RegistrationMonitor();
  ~RegistrationMonitor();

  void SetFixedSurface( vtkPolyData* surface );
  void SetMovingSurface( vtkPolyData* surface );

  void SetNumberOfIterationsPerUpdate( unsigned int number );
    
  void Observe( OptimizerType * optimizer, TransformType * transform );

  void SetVerbose( bool );

private:
  
  vtkSmartPointer< vtkMatrix4x4 >        Matrix;

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

  OptimizerType::Pointer          ObservedOptimizer;

  TransformType::Pointer          ObservedTransform;

  unsigned int                    CurrentIterationNumber;
  unsigned int                    NumberOfIterationsPerUpdate;
 
  bool                            Verbose;

  // These methods will only be called by the Observer
  void Update();
  void StartVisualization();

};

#endif
