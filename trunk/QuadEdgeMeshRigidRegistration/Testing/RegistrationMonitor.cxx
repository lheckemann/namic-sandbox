/*=========================================================================

  Program:   Surface Registration Program
  Module:    $RCSfile: RegistrationMonitor.cxx,v $

  Copyright (c) Kitware Inc. 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "RegistrationMonitor.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkMatrix4x4.h"

#define DeleteIfNotNullMacro(x) \
  if( this->x ) this->x->Delete();

/** Constructor */
RegistrationMonitor::RegistrationMonitor()
{
  this->FixedSurface  = NULL; // connected externally
  this->MovingSurface = NULL; // connected externally

  // Variable for the conversion of the ITK transform into VTK Matrix
  this->Matrix     = vtkMatrix4x4::New();

  // Own member objects
  this->FixedActor     = vtkActor::New();
  this->FixedProperty  = vtkProperty::New();
  this->FixedMapper    = vtkPolyDataMapper::New();

  this->MovingActor    = vtkActor::New();
  this->MovingProperty = vtkProperty::New();
  this->MovingMapper   = vtkPolyDataMapper::New();

  this->Renderer                 = vtkRenderer::New();
  this->RenderWindow             = vtkRenderWindow::New();
  this->RenderWindowInteractor   = vtkRenderWindowInteractor::New();

  // ITK Objects, does not require to call Delete()
  this->StartObserver       = ObserverType::New();
  this->IterationObserver   = ObserverType::New();

  this->StartObserver->SetCallbackFunction( 
    this, & Self::StartVisualization );

  this->IterationObserver->SetCallbackFunction( 
    this, & Self::Update );

  this->CurrentIterationNumber = 0;
  this->NumberOfIterationsPerUpdate = 1;

  this->Verbose = true;
}

/** Destructor */
RegistrationMonitor::~RegistrationMonitor()
{
  DeleteIfNotNullMacro( Matrix );

  DeleteIfNotNullMacro( Renderer );
  DeleteIfNotNullMacro( RenderWindow );
  DeleteIfNotNullMacro( RenderWindowInteractor );

  DeleteIfNotNullMacro( FixedActor );
  DeleteIfNotNullMacro( FixedProperty );
  DeleteIfNotNullMacro( FixedMapper );

  DeleteIfNotNullMacro( MovingActor );
  DeleteIfNotNullMacro( MovingProperty );
  DeleteIfNotNullMacro( MovingMapper );
}

/** Set the Fixed Surface */
void RegistrationMonitor::SetFixedSurface(vtkPolyData* surface)
{
  this->FixedSurface = surface;
}

/** Set the Moving Surface */
void RegistrationMonitor::SetMovingSurface(vtkPolyData* surface)
{
  this->MovingSurface = surface;
}

/** Set transform */
void RegistrationMonitor
::Observe( OptimizerType * optimizer, TransformType * transform )
{
  this->ObservedOptimizer = optimizer;
  this->ObservedTransform = transform;

  this->ObservedOptimizer->AddObserver( 
    itk::StartEvent(), this->StartObserver     );

  this->ObservedOptimizer->AddObserver( 
    itk::IterationEvent(), this->IterationObserver );
}

/** Callback for the StartEvent */
void RegistrationMonitor::StartVisualization()
{
  // flag for preventing the pipeline from being created
  // multiple times:
  static bool visualizationPipelineInitialized = false;

  this->CurrentIterationNumber = 0;

  if( visualizationPipelineInitialized )
    {
    return;
    }
  
  // Setup the RenderWindow and its basic infrastructure
  this->RenderWindow->SetSize( 600, 600 );
  this->RenderWindow->AddRenderer( this->Renderer );
  this->RenderWindowInteractor->SetRenderWindow( this->RenderWindow );

  // Set the background to something grayish
  this->Renderer->SetBackground(0.4392, 0.5020, 0.5647);

  // Setup the Fixed Surface infrastructure
  this->FixedActor->SetMapper( this->FixedMapper );
  this->FixedMapper->SetInput( this->FixedSurface );
  //this->FixedMapper->ScalarVisibilityOff();

  this->FixedProperty->SetAmbient(0.1);
  this->FixedProperty->SetDiffuse(0.1);
  this->FixedProperty->SetSpecular(0.5);
  this->FixedProperty->SetColor(0.785,0.0,0.0);
  this->FixedProperty->SetLineWidth(2.0);
  //this->FixedProperty->SetRepresentationToPoints();

  this->FixedActor->SetProperty( this->FixedProperty );


  // Setup the Moving Surface infrastructure
  this->MovingActor->SetMapper( this->MovingMapper );
  this->MovingMapper->SetInput( this->MovingSurface );
  //this->MovingMapper->ScalarVisibilityOff();

  this->MovingProperty->SetAmbient(0.1);
  this->MovingProperty->SetDiffuse(0.1);
  this->MovingProperty->SetSpecular(0.5);
  this->MovingProperty->SetColor(0.0,0.0,0.785);
  this->MovingProperty->SetLineWidth(2.0);
  //this->MovingProperty->SetRepresentationToPoints();

  this->MovingActor->SetProperty( this->MovingProperty );


  // Connecting the Fixed and Moving surfaces to the 
  // visualization pipeline
  //this->Renderer->AddActor( this->FixedActor );
  this->Renderer->AddActor( this->MovingActor );


  // Bring up the render window and begin interaction.
  this->Renderer->ResetCamera();
  this->RenderWindow->Render();
  this->RenderWindowInteractor->Initialize();

  visualizationPipelineInitialized = true;
}

/** Update the Visualization */
void RegistrationMonitor
::SetNumberOfIterationsPerUpdate( unsigned int number )
{
  this->NumberOfIterationsPerUpdate = number;
}


/** Define whether to print out messages or not. */
void RegistrationMonitor
::SetVerbose( bool verbose )
{
  this->Verbose = verbose;
}


/** Update the Visualization */
void RegistrationMonitor::Update()
{

  if( this->Verbose )
    {
    std::cout << "Iteration " << this->CurrentIterationNumber << std::endl;
    }
  
  if( this->CurrentIterationNumber % 
      this->NumberOfIterationsPerUpdate )
    {
    this->CurrentIterationNumber++;
    return;
    }
  else
    {
    this->CurrentIterationNumber++;
    }

  TransformType::MatrixType matrix = 
    this->ObservedTransform->GetMatrix();
 
  TransformType::OffsetType offset = 
    this->ObservedTransform->GetOffset();

  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      this->Matrix->SetElement(i,j,
        matrix.GetVnlMatrix().get(i,j));   
      }

    this->Matrix->SetElement( i, 3, offset[i]);
    }

  this->MovingActor->SetUserMatrix( this->Matrix );
  this->RenderWindowInteractor->Render();
}
