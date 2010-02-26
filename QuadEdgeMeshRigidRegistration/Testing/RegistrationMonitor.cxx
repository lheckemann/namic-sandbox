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

#include "RegistrationMonitor.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"

/** Constructor */
RegistrationMonitor::RegistrationMonitor()
{
  this->FixedSurface  = vtkSmartPointer< vtkPolyData >::New();
  this->MovingSurface = vtkSmartPointer< vtkPolyData >::New();

  // Own member objects
  this->FixedActor     = vtkSmartPointer<vtkActor>::New();
  this->FixedProperty  = vtkSmartPointer<vtkProperty>::New();
  this->FixedMapper    = vtkSmartPointer<vtkPolyDataMapper>::New();

  this->MovingActor    = vtkSmartPointer<vtkActor>::New();
  this->MovingProperty = vtkSmartPointer<vtkProperty>::New();
  this->MovingMapper   = vtkSmartPointer<vtkPolyDataMapper>::New();

  this->FixedRenderer            = vtkSmartPointer<vtkRenderer>::New();
  this->MovingRenderer           = vtkSmartPointer<vtkRenderer>::New();
  this->RenderWindow             = vtkSmartPointer<vtkRenderWindow>::New();
  this->RenderWindowInteractor   = vtkSmartPointer<vtkRenderWindowInteractor>::New();

  this->WindowToImageFilter = vtkSmartPointer< vtkWindowToImageFilter >::New();

  this->Writer = vtkSmartPointer< vtkPNGWriter >::New();

  this->WindowToImageFilter->SetInput( this->RenderWindow );

  this->Writer->SetInput( this->WindowToImageFilter->GetOutput() );
  
  this->StartObserver       = ObserverType::New();
  this->IterationObserver   = ObserverType::New();

  this->StartObserver->SetCallbackFunction( this, & Self::StartVisualization );

  this->IterationObserver->SetCallbackFunction( this, & Self::Update );

  this->CurrentIterationNumber = 0;
  this->NumberOfIterationsPerUpdate = 1;

  this->Verbose = true;
}


/** Destructor */
RegistrationMonitor::~RegistrationMonitor()
{
}


/** Set the Fixed Surface */
void RegistrationMonitor::SetFixedSurface(vtkPolyData* surface)
{
  this->FixedSurface->DeepCopy( surface );
}


/** Set the Moving Surface */
void RegistrationMonitor::SetMovingSurface(vtkPolyData* surface)
{
  this->MovingSurface->DeepCopy( surface );
}


/** Callback for the StartEvent */
void RegistrationMonitor::StartVisualization()
{
  // flag for preventing the pipeline from being created
  // multiple times:
  static bool visualizationPipelineInitialized = false;

  if( visualizationPipelineInitialized )
    {
    return;
    }
  
  // Setup the RenderWindow and its basic infrastructure
  this->RenderWindow->SetSize( 1200, 600 );

  this->FixedRenderer->SetViewport(0, 0, 0.5, 1.0);
  this->MovingRenderer->SetViewport(0.5, 0, 1.0, 1.0);

  this->RenderWindow->AddRenderer( this->FixedRenderer );
  this->RenderWindow->AddRenderer( this->MovingRenderer );

  this->RenderWindowInteractor->SetRenderWindow( this->RenderWindow );

  // Set the background to something grayish
  this->FixedRenderer->SetBackground(0.4392, 0.5020, 0.5647);
  this->MovingRenderer->SetBackground(0.4392, 0.5020, 0.5647);

  // Setup the Fixed Surface infrastructure
  this->FixedActor->SetMapper( this->FixedMapper );
  this->FixedMapper->SetInput( this->FixedSurface );
  this->FixedMapper->ScalarVisibilityOn();

  this->FixedProperty->SetAmbient(0.1);
  this->FixedProperty->SetDiffuse(0.1);
  this->FixedProperty->SetSpecular(0.1);
  // this->FixedProperty->SetColor(1.0, 1.0, 1.0);
  this->FixedProperty->SetLineWidth(2.0);
  //this->FixedProperty->SetRepresentationToPoints();

  this->FixedActor->SetProperty( this->FixedProperty );


  // Setup the Moving Surface infrastructure
  this->MovingActor->SetMapper( this->MovingMapper );
  this->MovingMapper->SetInput( this->MovingSurface );
  this->MovingMapper->ScalarVisibilityOn();

  this->MovingProperty->SetAmbient(0.1);
  this->MovingProperty->SetDiffuse(0.1);
  this->MovingProperty->SetSpecular(0.1);
  // this->MovingProperty->SetColor(1.0,1.0,1.0);
  this->MovingProperty->SetLineWidth(2.0);
  //this->MovingProperty->SetRepresentationToPoints();

  this->MovingActor->SetProperty( this->MovingProperty );


  // Connecting the Fixed and Moving surfaces to the 
  // visualization pipeline
  this->FixedRenderer->AddActor( this->FixedActor );
  this->MovingRenderer->AddActor( this->MovingActor );


  // Bring up the render window and begin interaction.
  this->FixedRenderer->ResetCamera();
  this->MovingRenderer->ResetCamera();

  vtkCamera * fixedCamera = this->FixedRenderer->GetActiveCamera();
  vtkCamera * movingCamera = this->MovingRenderer->GetActiveCamera();

  const double zoomFactor = 1.5;

  fixedCamera->Zoom( zoomFactor );
  movingCamera->Zoom( zoomFactor );

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


/** Return the points of the Fixed Surface. */
vtkPoints * 
RegistrationMonitor
:: GetFixedSurfacePoints()
{
  return this->FixedSurface->GetPoints();
}

/** Set the transform in the actor of the moving surface */
void RegistrationMonitor
::SetMovingActorMatrix( vtkMatrix4x4 * matrix )
{
  if( matrix )
    {
    this->MovingActor->SetUserMatrix( matrix );
    }
}


/** Refresh the rendering of the scene. */
void RegistrationMonitor
::RefreshRendering()
{
  if( this->ScreenShotsBaseFileName.empty() )
    {
    this->RenderWindowInteractor->Render();
    }
  else
    {
    this->RenderAndSaveScreenShot();
    }
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

  this->UpdateDataBeforeRendering();

  this->RefreshRendering();
}


void 
RegistrationMonitor
::SetScreenShotsBaseFileName( const char * screenShotFileName )
{
  this->ScreenShotsBaseFileName = screenShotFileName;
}


void RegistrationMonitor
::RemovePreviousObservers()
{
  if( this->ObservedObject.IsNotNull() )
    {
    ObserverTagsArrayType::const_iterator observerTagItr = this->ObserverTags.begin();
    ObserverTagsArrayType::const_iterator observerTagEnd = this->ObserverTags.end();

    while( observerTagItr != observerTagEnd )
      {
      this->ObservedObject->RemoveObserver( *observerTagItr );
      ++observerTagItr;
      }
    }
}


/** Set the objects to be observed: optimizer and transform */
void RegistrationMonitor
::Observe( itk::Object * processObject )
{
  if( this->ObservedObject != processObject )
    {
    this->RemovePreviousObservers();

    this->ObservedObject = processObject;

    unsigned long observerTag =
      this->ObservedObject->AddObserver( 
        itk::StartEvent(), this->StartObserver     );
    
    this->ObserverTags.push_back( observerTag );

    observerTag = this->ObservedObject->AddObserver( 
      itk::ProgressEvent(), this->IterationObserver );

    this->ObserverTags.push_back( observerTag );

    observerTag = this->ObservedObject->AddObserver( 
      itk::IterationEvent(), this->IterationObserver );

    this->ObserverTags.push_back( observerTag );
    }
}


void 
RegistrationMonitor
::RenderAndSaveScreenShot()
{
  ::itk::OStringStream message;
  message << this->ScreenShotsBaseFileName;
  message.fill('0');
  message.width(5);
  message << this->CurrentIterationNumber;
  message << ".png";

  this->WindowToImageFilter = vtkSmartPointer< vtkWindowToImageFilter >::New();

  this->Writer = vtkSmartPointer< vtkPNGWriter >::New();

  this->WindowToImageFilter->SetInput( this->RenderWindow );

  this->Writer->SetInput( this->WindowToImageFilter->GetOutput() );
 
  this->Writer->SetFileName( message.str().c_str() );
  
  this->WindowToImageFilter->Update();
  this->RenderWindow->Render();
  
  this->Writer->Write();
}

