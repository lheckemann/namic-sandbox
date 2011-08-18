/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCylinderWidget.cxx
  Language:  C++
  RCS:   $Id: vtkCylinderWidget.cxx 1.2 2005/01/18 12:45:04 Goodwin Exp Goodwin $

  Copyright (c) 2003 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.

=========================================================================*/
#include "vtkCylinderWidget.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCylinder.h"
#include "vtkPlane.h"
#include "vtkPolyDataNormals.h"
#include "vtkLineSource.h"
#include "vtkConeSource.h"

vtkCxxRevisionMacro(vtkCylinderWidget, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkCylinderWidget);

vtkCylinderWidget::vtkCylinderWidget()
{
  this->State = vtkCylinderWidget::Start;
  this->EventCallbackCommand->SetCallback(vtkCylinderWidget::ProcessEvents);
  
  // Enable/disable the translation, rotation, and scaling of the widget
  this->TranslationEnabled = 1;
  this->RotationEnabled = 1;
  this->ScalingEnabled = 1;

  //Build the representation of the widget
  //int i;

  // Control orientation of normals
  this->InsideOut = 0;

  // Construct the poly data representing the cylinder
  this->Cylinder = vtkCylinderSource::New();
  this->Cylinder->SetResolution(20);
  this->Cylinder->CappingOff();
  
  this->TranslationTransform = vtkTransform::New();
  this->TranslationTransform->Identity();
  this->TranslationTransform->PostMultiply();
  
  this->RotationTransform = vtkTransform::New();
  this->RotationTransform->Identity();
  this->RotationTransform->PostMultiply();
  
  this->Transform = vtkTransform::New();
  this->Transform->Identity();
  this->Transform->PostMultiply();
  this->Transform->Concatenate(RotationTransform);
  this->Transform->Concatenate(TranslationTransform);
  
  this->TransformFilter = vtkTransformPolyDataFilter::New();
  this->TransformFilter->SetInputConnection(this->Cylinder->GetOutputPort());
  this->TransformFilter->SetTransform(this->Transform); 
  
  this->CylNormals = vtkPolyDataNormals::New();
  this->CylNormals->SetInputConnection(TransformFilter->GetOutputPort());
  this->CylNormals->Update();

  this->CylPolyData = CylNormals->GetOutput();
  this->CylMapper = vtkPolyDataMapper::New();
  this->CylMapper->SetInput(CylPolyData);
  this->CylActor = vtkActor::New();
  this->CylActor->SetMapper(this->CylMapper);
  
  // Set up the initial properties
  this->CreateDefaultProperties();
  
  // Create the + plane normal
  this->LineSource = vtkLineSource::New();
  this->LineSource->SetResolution(1);
  this->LineMapper = vtkPolyDataMapper::New();
  this->LineMapper->SetInput(this->LineSource->GetOutput());
  this->LineActor = vtkActor::New();
  this->LineActor->SetMapper(this->LineMapper);

  this->ConeSource = vtkConeSource::New();
  this->ConeSource->SetResolution(20);
  this->ConeSource->SetAngle(25.0);
  this->ConeMapper = vtkPolyDataMapper::New();
  this->ConeMapper->SetInput(this->ConeSource->GetOutput());
  this->ConeActor = vtkActor::New();
  this->ConeActor->SetMapper(this->ConeMapper);

  // Create the - plane normal
  this->LineSource2 = vtkLineSource::New();
  this->LineSource2->SetResolution(1);
  this->LineMapper2 = vtkPolyDataMapper::New();
  this->LineMapper2->SetInput(this->LineSource2->GetOutput());
  this->LineActor2 = vtkActor::New();
  this->LineActor2->SetMapper(this->LineMapper2);

  this->ConeSource2 = vtkConeSource::New();
  this->ConeSource2->SetResolution(20);
  this->ConeSource2->SetAngle(25.0);
  this->ConeMapper2 = vtkPolyDataMapper::New();
  this->ConeMapper2->SetInput(this->ConeSource2->GetOutput());
  this->ConeActor2 = vtkActor::New();
  this->ConeActor2->SetMapper(this->ConeMapper2);


  // Create the handle
  this->Handle = vtkActor::New();
  this->HandleMapper = vtkPolyDataMapper::New();
  this->HandleGeometry = vtkSphereSource::New();
  this->HandleGeometry->SetThetaResolution(16);
  this->HandleGeometry->SetPhiResolution(8);
  this->HandleMapper->SetInput(this->HandleGeometry->GetOutput());
  this->Handle->SetMapper(this->HandleMapper);

      
  // Define the point coordinates
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;
  
  this->Direction[0] =0.0;
  this->Direction[1] =1.0;
  this->Direction[2] =0.0;
  this->Height =1.0;
  this->Radius = 1.0;
  
  //Place widget
  this->PlaceWidget(bounds);
  
  //Manage the picking stuff
  this->HandlePicker = vtkCellPicker::New();
  this->HandlePicker->SetTolerance(0.001);
  this->HandlePicker->AddPickList(this->Handle);
  this->HandlePicker->AddPickList(this->ConeActor);
  this->HandlePicker->AddPickList(this->ConeActor2);
  this->HandlePicker->PickFromListOn();

  this->CylPicker = vtkCellPicker::New();
  this->CylPicker->SetTolerance(0.001);
  this->CylPicker->AddPickList(CylActor);
  this->CylPicker->PickFromListOn();
  
  this->CurrentHandle = NULL;
  
}

vtkCylinderWidget::~vtkCylinderWidget()
{
  this->Cylinder->Delete();
  this->CylNormals->Delete();
  this->CylMapper->Delete();
  this->CylActor->Delete();
  

  this->HandleGeometry->Delete();
  this->HandleMapper->Delete();
  this->Handle->Delete();

  this->ConeActor->Delete();
  this->ConeMapper->Delete();
  this->ConeSource->Delete();

  this->LineActor->Delete();
  this->LineMapper->Delete();
  this->LineSource->Delete();

  this->ConeActor2->Delete();
  this->ConeMapper2->Delete();
  this->ConeSource2->Delete();

  this->LineActor2->Delete();
  this->LineMapper2->Delete();
  this->LineSource2->Delete();
  
  this->HandlePicker->Delete();
  this->CylPicker->Delete();

  this->Transform->Delete();
  this->RotationTransform->Delete();
  this->TranslationTransform->Delete();
  this->TransformFilter->Delete();

  this->HandleProperty->Delete();
  this->SelectedHandleProperty->Delete();
  this->FaceProperty->Delete();
  this->SelectedFaceProperty->Delete();
  this->OutlineProperty->Delete();
  this->SelectedOutlineProperty->Delete();

  
}

void vtkCylinderWidget::SetEnabled(int enabling)
{
  if ( ! this->Interactor )
    {
    vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
    return;
    }

  if ( enabling ) //------------------------------------------------------------
    {
    vtkDebugMacro(<<"Enabling widget");

    if ( this->Enabled ) //already enabled, just return
      {
      return;
      }

    if ( ! this->CurrentRenderer )
      {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]));
      if (this->CurrentRenderer == NULL)
        {
        return;
        }
      }

    this->Enabled = 1;

    // listen to the following events
    vtkRenderWindowInteractor *i = this->Interactor;
    i->AddObserver(vtkCommand::MouseMoveEvent, this->EventCallbackCommand, 
                   this->Priority);
    i->AddObserver(vtkCommand::LeftButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::LeftButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::MiddleButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::MiddleButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonPressEvent, 
                   this->EventCallbackCommand, this->Priority);
    i->AddObserver(vtkCommand::RightButtonReleaseEvent, 
                   this->EventCallbackCommand, this->Priority);

    // Add the various actors
    this->CurrentRenderer->AddActor(this->CylActor);
    this->CylActor->SetProperty(this->OutlineProperty);

    // turn on the handles
    this->CurrentRenderer->AddActor(this->Handle);
    this->Handle->SetProperty(this->HandleProperty);
    // add the normal vector
    this->CurrentRenderer->AddActor(this->LineActor);
    this->LineActor->SetProperty(this->OutlineProperty);
    this->CurrentRenderer->AddActor(this->ConeActor);
    this->ConeActor->SetProperty(this->HandleProperty);
    this->CurrentRenderer->AddActor(this->LineActor2);
    this->LineActor2->SetProperty(this->OutlineProperty);
    this->CurrentRenderer->AddActor(this->ConeActor2);
    this->ConeActor2->SetProperty(this->HandleProperty);

    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    
    //this->SizeHandles();
    }

  else //disabling-------------------------------------------------------------
    {
    vtkDebugMacro(<<"Disabling widget");

    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }
    
    this->Enabled = 0;

    // don't listen for events any more
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // turn off the cylinder 
    this->CurrentRenderer->RemoveActor(this->CylActor);

    // turn off the handles

    this->CurrentRenderer->RemoveActor(this->Handle);
    this->CurrentRenderer->RemoveActor(this->LineActor);
    this->CurrentRenderer->RemoveActor(this->ConeActor);
    this->CurrentRenderer->RemoveActor(this->LineActor2);
    this->CurrentRenderer->RemoveActor(this->ConeActor2);

    this->CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL);
    }

  this->Interactor->Render();
}

void vtkCylinderWidget::AddObservers(void)
{
    // listen for the following events
    vtkRenderWindowInteractor *i = this->Interactor;
    if (i)
    {
        i->AddObserver(vtkCommand::MouseMoveEvent, this->EventCallbackCommand,
                       this->Priority);
        i->AddObserver(vtkCommand::LeftButtonPressEvent,
                       this->EventCallbackCommand, this->Priority);
        i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
                       this->EventCallbackCommand, this->Priority);
        i->AddObserver(vtkCommand::MiddleButtonPressEvent,
                       this->EventCallbackCommand, this->Priority);
        i->AddObserver(vtkCommand::MiddleButtonReleaseEvent,
                       this->EventCallbackCommand, this->Priority);
        i->AddObserver(vtkCommand::RightButtonPressEvent,
                       this->EventCallbackCommand, this->Priority);
        i->AddObserver(vtkCommand::RightButtonReleaseEvent,
                       this->EventCallbackCommand, this->Priority);
    }
}

void vtkCylinderWidget::SetInteraction(int interact)
{
  if (this->Interactor && this->Enabled)
    {
    if (this->Interaction == interact)
      {
      return;
      }
    if (interact == 0)
      {
      this->Interactor->RemoveObserver(this->EventCallbackCommand);
      }
    else
      {
        this->AddObservers();
      }
    this->Interaction = interact;
    }
  else
    {
    vtkGenericWarningMacro(<<"set interactor and Enabled before changing interaction...");
    }
}

void vtkCylinderWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                 unsigned long event,
                                 void* clientdata, 
                                 void* vtkNotUsed(calldata))
{
  vtkCylinderWidget* self = reinterpret_cast<vtkCylinderWidget *>( clientdata );

  //okay, let's do the right thing
  switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
      self->OnLeftButtonDown();
      break;
    case vtkCommand::LeftButtonReleaseEvent:
      self->OnLeftButtonUp();
      break;
    case vtkCommand::MiddleButtonPressEvent:
      self->OnMiddleButtonDown();
      break;
    case vtkCommand::MiddleButtonReleaseEvent:
      self->OnMiddleButtonUp();
      break;
    case vtkCommand::RightButtonPressEvent:
      self->OnRightButtonDown();
      break;
    case vtkCommand::RightButtonReleaseEvent:
      self->OnRightButtonUp();
      break;
    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}

void vtkCylinderWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->HandleProperty )
    {
    os << indent << "Handle Property: " << this->HandleProperty << "\n";
    }
  else
    {
    os << indent << "Handle Property: (none)\n";
    }
  if ( this->SelectedHandleProperty )
    {
    os << indent << "Selected Handle Property: " 
       << this->SelectedHandleProperty << "\n";
    }
  else
    {
    os << indent << "SelectedHandle Property: (none)\n";
    }


  os << indent << "Inside Out: " << (this->InsideOut ? "On\n" : "Off\n");
  os << indent << "Translation Enabled: " << (this->TranslationEnabled ? "On\n" : "Off\n");
  os << indent << "Scaling Enabled: " << (this->ScalingEnabled ? "On\n" : "Off\n");
  os << indent << "Rotation Enabled: " << (this->RotationEnabled ? "On\n" : "Off\n");
  
}

void vtkCylinderWidget::GetCylinder(vtkCylinder *cylinder)
{
  vtkTransform *trans;
  
  cylinder->SetRadius(this->Cylinder->GetRadius());
  
  if ((trans = (vtkTransform *)cylinder->GetTransform()) == NULL)
    {
    trans = vtkTransform::New();
    cylinder->SetTransform(trans);
    trans->Delete();
    }
  
  vtkMatrix4x4 *matrix;
  
  matrix = this->Transform->GetLinearInverse()->GetMatrix();

  trans->SetMatrix(matrix);

}



void vtkCylinderWidget::GetPlane1(vtkPlane *plane1)
{
  double p1[3], center[3], n[3];
  
  ConeSource->GetCenter(p1);
  HandleGeometry->GetCenter(center);
  
  // the normal points outwards
  for (int i=0; i<3; i++)
    {
    n[i] =  p1[i] - center[i];
    }
  
  vtkMath::Normalize(n);  
    
  plane1->SetOrigin(p1);
  plane1->SetNormal(n);
  
}


void vtkCylinderWidget::GetPlane2(vtkPlane *plane2)
{
  double p2[3], center[3], n[3];
  
  ConeSource2->GetCenter(p2);
  HandleGeometry->GetCenter(center);
  
  // the normal points outwards
  for (int i=0; i<3; i++)
    {
    n[i] =  p2[i] - center[i];
    }
  
  vtkMath::Normalize(n);  
    
  plane2->SetOrigin(p2);
  plane2->SetNormal(n);
  
}


void vtkCylinderWidget::PositionHandles()
{
  double height, center[3], p1[3], p2[3];
  
  height = this->Cylinder->GetHeight();
  this->Height = height;
  this->Cylinder->GetCenter(center);
  
  p1[0] = p2[0] = center[0];
  p1[2] = p2[2] = center[2];
  
  p1[1] = center[1] - height/2;
  p2[1] = center[1] + height/2;
  
  this->Transform->TransformPoint(p1,p1);
  this->Transform->TransformPoint(p2,p2);
  this->Transform->TransformPoint(center,center);
  
  for (int i=0; i<3; i++)
    {
    this->Direction[i] = p2[i]-p1[i];
    }
  
  ConeSource->SetCenter(p1);
  ConeSource->SetDirection(this->Direction);
  LineSource->SetPoint1(p1);
  LineSource->SetPoint2(center);
  HandleGeometry->SetCenter(center);
  ConeSource2->SetCenter(p2);
  ConeSource2->SetDirection(this->Direction);
  LineSource2->SetPoint1(center);
  LineSource2->SetPoint2(p2);

}

void vtkCylinderWidget::SetPoint1(double x, double y, double z)
{
  double p1[3], p2[3], center[3], height;
  
  p1[0] = x; p1[1] = y; p1[2] = z;
  
  ConeSource->SetCenter(x,y,z);  
  ConeSource2->GetCenter(p2);
  for (int i=0; i<3; i++)
    {
    this->Direction[i] = p2[i] - p1[i]; 
    center[i] = (p2[i] + p1[i])/2.0;
    }

  ConeSource->SetDirection(this->Direction);
  ConeSource2->SetDirection(this->Direction);
  LineSource->SetPoint1(x,y,z);
  HandleGeometry->SetCenter(center);
  
  this->Cylinder->SetCenter(center);
  height = vtkMath::Distance2BetweenPoints(p1,p2);
  height = sqrt(height);
  this->Cylinder->SetHeight(height);
  
}

void vtkCylinderWidget::SetPoint2(double x, double y, double z)
{
  double p1[3], p2[3], center[3], height;
  
  p2[0] = x; p2[1] = y; p2[2] = z;
  
  ConeSource2->SetCenter(x,y,z);  
  ConeSource->GetCenter(p1);
  for (int i=0; i<3; i++)
    {
    this->Direction[i] = p2[i] - p1[i]; 
    center[i] = (p2[i] + p1[i])/2.0;
    }

  ConeSource->SetDirection(this->Direction);
  ConeSource2->SetDirection(this->Direction);
  LineSource->SetPoint2(x,y,z);
  HandleGeometry->SetCenter(center);
  
  this->Cylinder->SetCenter(center);
  height = vtkMath::Distance2BetweenPoints(p1,p2);
  height = sqrt(height);
  this->Cylinder->SetHeight(height);
  
}


void vtkCylinderWidget::HandlesOn()
{
  this->Handle->VisibilityOn();
  this->ConeActor->VisibilityOn();
  this->ConeActor2->VisibilityOn();
  this->LineActor->VisibilityOn();
  this->LineActor2->VisibilityOn();
}

void vtkCylinderWidget::HandlesOff()
{

  this->Handle->VisibilityOff();
  this->ConeActor->VisibilityOff();
  this->ConeActor2->VisibilityOff();
  this->LineActor->VisibilityOff();
  this->LineActor2->VisibilityOff();

}

void vtkCylinderWidget::CylinderOn()
{
  this->CylActor->VisibilityOn();  
}

void vtkCylinderWidget::CylinderOff()
{
  this->CylActor->VisibilityOff();  
}

void vtkCylinderWidget::SizeHandles()
{
  double radius = this->vtk3DWidget::SizeHandles(1.5);

    this->ConeSource->SetRadius(radius);
    this->ConeSource->SetHeight(radius);
    this->ConeSource2->SetRadius(radius);
    this->ConeSource2->SetHeight(radius);
    this->HandleGeometry->SetRadius(radius);

}

void vtkCylinderWidget::HighlightOutline(int highlight)
{
  if ( highlight )
    {
    this->CylActor->SetProperty(this->SelectedOutlineProperty);
    }
  else
    {
    this->CylActor->SetProperty(this->OutlineProperty);
    }
}

int vtkCylinderWidget::HighlightHandle(vtkProp *prop)
{

  if ( this->CurrentHandle && this->CurrentHandle != CylActor)
    {
    this->CurrentHandle->SetProperty(this->HandleProperty);
    }

  this->CurrentHandle = (vtkActor *)prop;

  if ( this->CurrentHandle && this->CurrentHandle != CylActor)
    {
    this->CurrentHandle->SetProperty(this->SelectedHandleProperty);
    for (int i=0; i<3; i++) //find attached face
      {
      if ( this->CurrentHandle == this->Handle )
        {
        return 1;
        }
       else if (this->CurrentHandle == this->ConeActor)
        {
         return 0;
        }
      else
        {
        return 2;
        }     
      }
    }
  
  return -1;
}


void vtkCylinderWidget::OnLeftButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then pick the bounding box.
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
    {
    this->State = vtkCylinderWidget::Outside;
    return;
    }
  
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkCylinderWidget::Moving;
    this->HighlightHandle(path->GetFirstNode()->GetViewProp());
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    this->ValidPick = 1;
    }
  else
    {
    this->CylPicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->CylPicker->GetPath();
    if ( path != NULL )
      {
      this->State = vtkCylinderWidget::Moving;
      this->CylPicker->GetPickPosition(this->LastPickPosition);
      this->ValidPick = 1;
      if ( !this->Interactor->GetShiftKey() )
        {
        this->HighlightHandle(NULL);
        this->CurrentHandle = this->CylActor;
        }
      else
        {
        this->CurrentHandle = this->Handle;
        //this->HighlightOutline(1);
        }
      }
    else
      {
      this->HighlightHandle(NULL);
      this->State = vtkCylinderWidget::Outside;
      return;
      }
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
  this->Interactor->Render();

}

void vtkCylinderWidget::OnLeftButtonUp()
{
  if ( this->State == vtkCylinderWidget::Outside ||
       this->State == vtkCylinderWidget::Start )
    {
    return;
    }

  this->State = vtkCylinderWidget::Start;
  this->HighlightHandle(NULL);
  //this->SizeHandles();

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->Interactor->Render();
  
}

void vtkCylinderWidget::OnMiddleButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then pick the bounding box.
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
    {
    this->State = vtkCylinderWidget::Outside;
    return;
    }
  
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkCylinderWidget::Moving;
    this->CurrentHandle = this->Handle;
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    this->ValidPick = 1;
    }
  else
    {
    this->CylPicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->CylPicker->GetPath();
    if ( path != NULL )
      {
      this->State = vtkCylinderWidget::Moving;
      this->CurrentHandle = this->Handle;
      this->CylPicker->GetPickPosition(this->LastPickPosition);
      this->ValidPick = 1;
      }
    else
      {
      this->HighlightHandle(NULL);
      this->State = vtkCylinderWidget::Outside;
      return;
      }
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
  this->Interactor->Render();
}

void vtkCylinderWidget::OnMiddleButtonUp()
{
  if ( this->State == vtkCylinderWidget::Outside ||
       this->State == vtkCylinderWidget::Start )
    {
    return;
    }

  this->State = vtkCylinderWidget::Start;
  this->HighlightHandle(NULL);
  //this->SizeHandles();

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->Interactor->Render();
  
}

void vtkCylinderWidget::OnRightButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Okay, we can process this. Try to pick handles first;
  // if no handles picked, then pick the bounding box.
  if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
    {
    this->State = vtkCylinderWidget::Outside;
    return;
    }
  
  vtkAssemblyPath *path;
  this->HandlePicker->Pick(X,Y,0.0,this->CurrentRenderer);
  path = this->HandlePicker->GetPath();
  if ( path != NULL )
    {
    this->State = vtkCylinderWidget::Scaling;
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    this->ValidPick = 1;
    }
  else
    {
    this->CylPicker->Pick(X,Y,0.0,this->CurrentRenderer);
    path = this->CylPicker->GetPath();
    if ( path != NULL )
      {
      this->State = vtkCylinderWidget::Scaling;
      this->CylPicker->GetPickPosition(this->LastPickPosition);
      this->ValidPick = 1;
      }
    else
      {
      this->State = vtkCylinderWidget::Outside;
      return;
      }
    }
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
  this->Interactor->Render();
}

void vtkCylinderWidget::OnRightButtonUp()
{
  if ( this->State == vtkCylinderWidget::Outside )
    {
    return;
    }

  this->State = vtkCylinderWidget::Start;
  //this->SizeHandles();
  
  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->Interactor->Render();
}

void vtkCylinderWidget::OnMouseMove()
{
  // See whether we're active
  if ( this->State == vtkCylinderWidget::Outside || 
       this->State == vtkCylinderWidget::Start )
    {
    return;
    }

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z, vpn[3];

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  this->ComputeWorldToDisplay(this->LastPickPosition[0], this->LastPickPosition[1],
                              this->LastPickPosition[2], focalPoint);
  z = focalPoint[2];
  this->ComputeDisplayToWorld(double(this->Interactor->GetLastEventPosition()[0]),
                              double(this->Interactor->GetLastEventPosition()[1]),
                              z, prevPickPoint);
  this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);

  // Process the motion
  if ( this->State == vtkCylinderWidget::Moving )
    {
    // Okay to process
    if ( this->CurrentHandle )
      {
      if ( this->RotationEnabled && this->CurrentHandle == this->CylActor )
        {
        camera->GetViewPlaneNormal(vpn);
        this->Rotate(X, Y, prevPickPoint, pickPoint, vpn);
        }
      else if ( this->TranslationEnabled && this->CurrentHandle == this->Handle )
        {
        this->Translate(prevPickPoint, pickPoint);
        }
      else if ( this->ScalingEnabled ) 
        {
        if ( this->CurrentHandle == this->ConeActor || this->CurrentHandle == this->ConeActor2 )
          {
          this->Elongate(prevPickPoint, pickPoint);
          }
        }
      }
    }
  else if ( this->ScalingEnabled && this->State == vtkCylinderWidget::Scaling )
    {
    this->Scale(prevPickPoint, pickPoint, X, Y);
    }

  // Interact, if desired
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
  this->Interactor->Render();

}


// Elongate
void vtkCylinderWidget::Elongate(double *p1, double *p2)
{
  int i;
  double v[3], v2[3], dh;
  double h1[3], h2[3], height;
  //double center[3];

  ConeSource->GetCenter(h1);
  ConeSource2->GetCenter(h2);
  
  for (i=0; i<3; i++)
    {
    v[i] = p2[i] - p1[i];
    v2[i] = h2[i] - h1[i];
    }

  vtkMath::Normalize(v2);
  double f = vtkMath::Dot(v,v2);

  for (i=0; i<3; i++)
    {
    v[i] = f*v2[i];
    }
    
  dh = vtkMath::Norm(v);
  
  if ((this->CurrentHandle == ConeActor && f>0.0) || 
      (this->CurrentHandle == ConeActor2 && f<0.0))
    {
    dh = -dh;
    }
    
  height = this->Cylinder->GetHeight();
  this->Cylinder->SetHeight(height+dh);
  this->Height = height;
  
  this->PositionHandles();
}

// Loop through all points and translate them
void vtkCylinderWidget::Translate(double *p1, double *p2)
{
  double v[3];
  
  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];
  
  // Move the points
  this->TranslationTransform->Translate(v);

  this->PositionHandles();
}

void vtkCylinderWidget::Scale(double* vtkNotUsed(p1), double* vtkNotUsed(p2), 
                         int vtkNotUsed(X), int Y)
{

  double sf;
  
  if ( Y > this->Interactor->GetLastEventPosition()[1] )
    {
    sf = 1.05;
    }
  else
    {
    sf = 0.95;
    }  
    
  // Move the points
  double radius = this->Cylinder->GetRadius();
  this->Cylinder->SetRadius(sf*radius);
  this->Radius = sf*radius;
  
  this->PositionHandles();
}

void vtkCylinderWidget::Rotate(int X, int Y, double *p1, double *p2, double *vpn)
{

  double v[3]; //vector of motion
  double axis[3]; //axis of rotation
  double theta; //rotation angle
  //int i;
  double center[3];

  this->HandleGeometry->GetCenter(center);

  v[0] = p2[0] - p1[0];
  v[1] = p2[1] - p1[1];
  v[2] = p2[2] - p1[2];

  // Create axis of rotation and angle of rotation
  vtkMath::Cross(vpn,v,axis);
  if ( vtkMath::Normalize(axis) == 0.0 )
    {
    return;
    }
  int *size = this->CurrentRenderer->GetSize();
  double l2 = (X-this->Interactor->GetLastEventPosition()[0])*(X-this->Interactor->GetLastEventPosition()[0]) 
  + (Y-this->Interactor->GetLastEventPosition()[1])*(Y-this->Interactor->GetLastEventPosition()[1]);
  theta = 360.0 * sqrt(l2/((double)size[0]*size[0]+size[1]*size[1]));

  //Manipulate the transform to reflect the rotation
  //We want the rotations to happen before the translations!

  this->RotationTransform->RotateWXYZ(theta,axis);


  this->PositionHandles();
}
  
void vtkCylinderWidget::CreateDefaultProperties()
{
  // Handle properties
  this->HandleProperty = vtkProperty::New();
  this->HandleProperty->SetColor(1,1,1);

  this->SelectedHandleProperty = vtkProperty::New();
  this->SelectedHandleProperty->SetColor(1,0,0);
  
  // Face properties
  this->FaceProperty = vtkProperty::New();
  this->FaceProperty->SetColor(1,1,1);
  this->FaceProperty->SetOpacity(0.0);

  this->SelectedFaceProperty = vtkProperty::New();
  this->SelectedFaceProperty->SetColor(1,1,0);
  this->SelectedFaceProperty->SetOpacity(0.25);
  
  // Outline properties
  this->OutlineProperty = vtkProperty::New();
  this->OutlineProperty->SetRepresentationToWireframe();
  this->OutlineProperty->SetDiffuse(0.0);
  this->OutlineProperty->SetAmbient(1.0);
  this->OutlineProperty->SetColor(1.0,1.0,1.0);
  this->OutlineProperty->SetOpacity(1.0);
  this->OutlineProperty->SetLineWidth(2.0);

  this->SelectedOutlineProperty = vtkProperty::New();
  this->SelectedOutlineProperty->SetRepresentationToWireframe();
  this->SelectedOutlineProperty->SetDiffuse(0.0);
  this->SelectedOutlineProperty->SetAmbient(1.0);
  this->SelectedOutlineProperty->SetColor(0.0,1.0,0.0);
  this->SelectedOutlineProperty->SetLineWidth(2.0);
  
}

void vtkCylinderWidget::PlaceWidget(double bds[6])
{
  int i;
  double bounds[6], center[3], p1[3], p2[3], h;
  

  
  this->AdjustBounds(bds,bounds,center);
  
  h = bounds[3]-bounds[2];
  p1[0] = p1[2] = p2[0] = p2[2] = 0.0;
  p1[1] = center[1] - h/2;
  p2[1] = center[1] + h/2;
 
  
  this->Cylinder->SetRadius((bounds[1]-bounds[0])/2.0);
  this->Cylinder->SetHeight(h);
  this->Height = h;
  this->Radius = (bounds[1]-bounds[0])/2.0;
  
  this->TranslationTransform->Translate(center);
  this->Cylinder->Update();
  
  for (i=0; i<6; i++)
    {
    this->InitialBounds[i] = bounds[i];
    }
  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  this->PositionHandles();
  //this->SizeHandles();
  
}

void vtkCylinderWidget::UpdatePlacement(void)
{
  this->CylNormals->Update();
  this->PositionHandles();
}

void vtkCylinderWidget::GetPolyData(vtkPolyData *pd)
{
  pd->SetPoints(this->CylPolyData->GetPoints());
  pd->SetPolys(this->CylPolyData->GetPolys());
}

//vtkPolyDataSource *vtkCylinderWidget::GetPolyDataSource()
//{
//  return this->CylNormals;
//}

vtkPolyDataAlgorithm *vtkCylinderWidget::GetPolyDataAlgorithm()
{
  return this->Cylinder;
}


