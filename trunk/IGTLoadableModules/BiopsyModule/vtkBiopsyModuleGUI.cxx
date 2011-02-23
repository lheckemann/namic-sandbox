/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include <cmath>

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkBiopsyModuleGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkKWScale.h"
#include "vtkTransform.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"
#include "vtkKWRange.h"
#include "vtkMath.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkPlaneSource.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkCollection.h"
#include "vtkProperty.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkBiopsyModuleGUI );
vtkCxxRevisionMacro ( vtkBiopsyModuleGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------
 

//---------------------------------------------------------------------------
vtkBiopsyModuleGUI::vtkBiopsyModuleGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBiopsyModuleGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  
  this->fiducialListWidget = NULL;
  this->fiducialListNode = NULL;
  this->drawline = NULL;

  this->fiducialLists = vtkCollection::New();
  this->lineActors = vtkActorCollection::New();
  this->colorActors = vtkCollection::New();

  this->translation = NULL;
  this->transformNode = vtkMRMLLinearTransformNode::New();
  this->transformMatrix = vtkMatrix4x4::New();
  this->lineBetweenFiducials = vtkLineSource::New();
  this->lineRange = NULL;

  this->WholeRangeWidget = NULL;
  this->UpdateWholeRangeButton = NULL;

  this->PerpendicularPlane = vtkPlaneSource::New();
  this->Plane0 = vtkPlaneSource::New();
  this->Plane90 = vtkPlaneSource::New();
  this->PlaneRotation = NULL;

  this->togglePlaneVisibility = NULL;
  this->toggleReslicing = NULL;
  this->planeActor = vtkActor::New();
  this->plane0Actor = vtkActor::New();
  this->plane90Actor = vtkActor::New();

  this->lineCenter[0] = 0;
  this->lineCenter[1] = 0;
  this->lineCenter[2] = 0;  

  this->P1Vector[0] = 0;
  this->P1Vector[1] = 0;
  this->P1Vector[2] = 0;  

  this->P2Vector[0] = 0;
  this->P2Vector[1] = 0;
  this->P2Vector[2] = 0;  

  this->P1VectorNormalized[0] = 0;
  this->P1VectorNormalized[1] = 0;
  this->P1VectorNormalized[2] = 0;  

  this->P2VectorNormalized[0] = 0;
  this->P2VectorNormalized[1] = 0;
  this->P2VectorNormalized[2] = 0;  

  this->PVectorLength = 0;
 
  this->lineTip1[0] = 0;
  this->lineTip1[1] = 0;
  this->lineTip1[2] = 0;

  this->lineTip2[0] = 0;
  this->lineTip2[1] = 0;
  this->lineTip2[2] = 0;

  this->dpoint1[0] = 0;
  this->dpoint1[1] = 0;
  this->dpoint1[2] = 0;

  this->dpoint2[0] = 0;
  this->dpoint2[1] = 0;
  this->dpoint2[2] = 0;

  this->normalVector1[0] = 0;
  this->normalVector1[1] = 0;
  this->normalVector1[2] = 0;

  this->normalVector2[0] = 0;
  this->normalVector2[1] = 0;
  this->normalVector2[2] = 0;

  this->sphereCenter[0] = 0;
  this->sphereCenter[1] = 0;
  this->sphereCenter[2] = 0;


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkBiopsyModuleGUI::~vtkBiopsyModuleGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if (this->fiducialListWidget)
    {
    this->fiducialListWidget->SetParent(NULL);
    this->fiducialListWidget->Delete();
    }

  if (this->drawline)
    {
    this->drawline->SetParent(NULL);
    this->drawline->Delete();
    }

  if (this->fiducialLists)
    {
    this->fiducialLists->Delete();
    }

  if (this->lineActors)
    {
    this->lineActors->Delete();
    }

  if (this->colorActors)
    {
    this->colorActors->Delete();
    }

  if (this->translation)
    {
    this->translation->SetParent(NULL);
    this->translation->Delete();
    }

  if (this->lineBetweenFiducials)
    {
    this->lineBetweenFiducials->Delete();
    }

  if (this->lineRange)
    {
    this->lineRange->SetParent(NULL);
    this->lineRange->Delete();
    }

  if (this->WholeRangeWidget)
    {
    this->WholeRangeWidget->SetParent(NULL);
    this->WholeRangeWidget->Delete();
    }

  if (this->UpdateWholeRangeButton)
    {
    this->UpdateWholeRangeButton->SetParent(NULL);
    this->UpdateWholeRangeButton->Delete();
    }

  if (this->togglePlaneVisibility)
    {
    this->togglePlaneVisibility->SetParent(NULL);
    this->togglePlaneVisibility->Delete();
    }

  if (this->toggleReslicing)
    {
    this->toggleReslicing->SetParent(NULL);
    this->toggleReslicing->Delete();
    }

  if (this->PerpendicularPlane)
    {
    this->PerpendicularPlane->Delete();
    }

  if (this->Plane0)
    {
    this->Plane0->Delete();
    }

  if (this->Plane90)
    {
    this->Plane90->Delete();
    }

  if (this->PlaneRotation)
    {
    this->PlaneRotation->Delete();
    }

  if (this->transformNode)
    {
    this->transformNode->Delete();
    }

 if (this->planeActor)
    {
    this->planeActor->Delete();
    }

 if (this->plane0Actor)
    {
    this->plane0Actor->Delete();
    }

 if (this->plane90Actor)
    {
    this->plane90Actor->Delete();
    }

 if (this->transformMatrix)
    {
    this->transformMatrix->Delete();
    }


  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "BiopsyModuleGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";

}
//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->fiducialListWidget)
    {
    this->fiducialListWidget
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
 
  if (this->drawline)
    {
    this->drawline
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->translation)
    {
    this->translation
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->lineRange)
    {
    this->lineRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->UpdateWholeRangeButton)
    {
    this->UpdateWholeRangeButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PlaneRotation)
    {
    this->PlaneRotation
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->togglePlaneVisibility)
    {
    this->togglePlaneVisibility
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->toggleReslicing)
    {
    this->toggleReslicing
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->GetMRMLScene())
    {
      this->GetMRMLScene()
        ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

this->fiducialListWidget
   ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

this->fiducialListWidget
   ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent, (vtkCommand *)this->GUICallbackCommand);

this->drawline
   ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand); 

this->translation
  ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);

this->lineRange
  ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);

this->UpdateWholeRangeButton
  ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

this->PlaneRotation
  ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);

this->togglePlaneVisibility->GetWidget()
  ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

this->toggleReslicing->GetWidget()
  ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

 this->GetMRMLScene()
   ->AddObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkBiopsyModuleLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->fiducialListWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->fiducialListWidget->GetSelected())
      {
     this->fiducialListNode = reinterpret_cast<vtkMRMLFiducialListNode*>(this->fiducialListWidget->GetSelected());
        //this->fiducialListNode->SetName("BiopsyModuleFiducialList");
        if(this->fiducialListNode)
       {
         this->ApplicationLogic->GetSelectionNode()->SetActiveFiducialListID(this->fiducialListNode->GetID());
       }
        this->fiducialListWidget->UpdateMenu();
      }
    
    }

  if (this->fiducialListWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->fiducialListWidget->GetSelected())
      {
      this->fiducialListNode = reinterpret_cast<vtkMRMLFiducialListNode*>(this->fiducialListWidget->GetSelected());
      this->fiducialListNode->Modified();
      this->fiducialListWidget->UpdateMenu();
      }
    }

  // DrawLine Button Pressed
  if(this->drawline == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
      
    if(this->fiducialListNode && (this->fiducialListNode->GetNumberOfFiducials() == 2))
      {
      // Get Fiducials from the list
      float* point1 = this->fiducialListNode->GetNthFiducialXYZ(0);
      float* point2 = this->fiducialListNode->GetNthFiducialXYZ(1);
      
      // Cast positions (float* -> double[3])
      this->dpoint1[0] = point1[0];
      this->dpoint1[1] = point1[1];
      this->dpoint1[2] = point1[2];

      this->dpoint2[0] = point2[0];
      this->dpoint2[1] = point2[1];
      this->dpoint2[2] = point2[2];
      
      // Create a line
      if(this->lineBetweenFiducials)
     {         
        this->lineBetweenFiducials->SetPoint1(dpoint1);
        this->lineBetweenFiducials->SetPoint2(dpoint2);
        this->lineBetweenFiducials->Update();
     }

      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
      mapper->SetInputConnection(this->lineBetweenFiducials->GetOutputPort());
     
      vtkActor* lineactor = vtkActor::New();
      lineactor->SetMapper(mapper);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(lineactor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapper->Delete();
      lineactor->Delete();
           
      // Enable Controllers 
      this->lineRange->SetEnabled(1);
      this->translation->SetEnabled(1);
      this->PlaneRotation->SetEnabled(1);
      this->togglePlaneVisibility->SetEnabled(1);
      this->toggleReslicing->SetEnabled(1);      

      // Calculate line center
      this->lineCenter[0] = (dpoint1[0] + dpoint2[0])/2;
      this->lineCenter[1] = (dpoint1[1] + dpoint2[1])/2;
      this->lineCenter[2] = (dpoint1[2] + dpoint2[2])/2;

      // Calculate vectors from center to P1 and P2
      this->P1Vector[0] = dpoint1[0] - this->lineCenter[0];
      this->P1Vector[1] = dpoint1[1] - this->lineCenter[1];
      this->P1Vector[2] = dpoint1[2] - this->lineCenter[2];

      this->P2Vector[0] = dpoint2[0] - this->lineCenter[0];
      this->P2Vector[1] = dpoint2[1] - this->lineCenter[1];
      this->P2Vector[2] = dpoint2[2] - this->lineCenter[2];
      
      // Normalize vectors
      // -- Calculate vector length (add sqrt because of missing in Distance2BetweenPoints in vtkMath)
      vtkMath* VectorLength = vtkMath::New();

      // Length is the same from P1 to center or P2 to center       
      this->PVectorLength = sqrt(VectorLength->Distance2BetweenPoints(dpoint1,this->lineCenter));

      VectorLength->Delete();
      
      // -- Normalize
      this->P1VectorNormalized[0] = this->P1Vector[0]/this->PVectorLength;
      this->P1VectorNormalized[1] = this->P1Vector[1]/this->PVectorLength;
      this->P1VectorNormalized[2] = this->P1Vector[2]/this->PVectorLength;
 
      this->P2VectorNormalized[0] = this->P2Vector[0]/this->PVectorLength;
      this->P2VectorNormalized[1] = this->P2Vector[1]/this->PVectorLength;
      this->P2VectorNormalized[2] = this->P2Vector[2]/this->PVectorLength;

      // Update Slider Range
      this->lineRange->SetWholeRange(-this->PVectorLength-200, this->PVectorLength+200);
      this->lineRange->SetRange(-this->PVectorLength, this->PVectorLength);
      this->Modified();

      // Create perpendiculars vector
      vtkMath* perpendicularVectors = vtkMath::New();
      perpendicularVectors->Perpendiculars(this->P1VectorNormalized,this->normalVector1,this->normalVector2,0);
      perpendicularVectors->Delete();
 
      // Create perpendicular plane
      if(this->PerpendicularPlane && this->Plane0 && this->Plane90 && this->lineCenter)
     {
        DrawPlanes(0, this->lineCenter);
     }

      // Perpendicular Plane     
      vtkPolyDataMapper* mapperPlaneP = vtkPolyDataMapper::New();
      mapperPlaneP->SetInput(this->PerpendicularPlane->GetOutput());
     
      this->planeActor->SetMapper(mapperPlaneP);
      this->planeActor->GetProperty()->SetColor(0,1,0);
      this->planeActor->GetProperty()->SetOpacity(0.5);

      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(this->planeActor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperPlaneP->Delete();

      // Plane 0 
      vtkPolyDataMapper* mapperPlane0 = vtkPolyDataMapper::New();
      mapperPlane0->SetInput(this->Plane0->GetOutput());
     
      this->plane0Actor->SetMapper(mapperPlane0);
      this->plane0Actor->GetProperty()->SetColor(1,0,0);     
      this->plane0Actor->GetProperty()->SetOpacity(0.5);

      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(this->plane0Actor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperPlane0->Delete();

      // Plane 90
      vtkPolyDataMapper* mapperPlane90 = vtkPolyDataMapper::New();
      mapperPlane90->SetInput(this->Plane90->GetOutput());
     
      this->plane90Actor->SetMapper(mapperPlane90);
      this->plane90Actor->GetProperty()->SetColor(1,1,0);
      this->plane90Actor->GetProperty()->SetOpacity(0.5);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(this->plane90Actor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperPlane90->Delete();

      // Enable planes
      this->togglePlaneVisibility->GetWidget()->SetSelectedState(1);
      
      // Reset Scale
      if(this->translation)
       {
      this->translation->SetValue(0);
       }
      
      // Create a transform node if not already existing  
      if(this->transformNode && this->transformMatrix)
        {
        this->GetMRMLScene()->AddNode(this->transformNode);
        this->GetMRMLScene()->Modified();

        vtkMatrix4x4* tempMatrix = this->transformNode->GetMatrixTransformToParent();        
        this->transformMatrix->Identity();

        tempMatrix->DeepCopy(this->transformMatrix);
        this->transformNode->Modified(); 
        }
      
      // Set Default Whole Range value
      this->WholeRangeWidget->GetWidget()->SetValueAsDouble(this->PVectorLength+200);
      }
   }

  // lineRange Changed
  if(this->lineRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
      // FIXME: Find a better way to round PVectorLength to second decimal
      if((std::abs(this->lineRange->GetEntry1()->GetValueAsDouble())) >= std::floor(this->PVectorLength*100)/100 && (std::abs(this->lineRange->GetEntry2()->GetValueAsDouble())) >= std::floor(this->PVectorLength*100)/100)
     {
        // Set Tips of the line
        if(this->dpoint1[0] > this->dpoint2[0])
            {
            // Tip 1
            double Slider1 = this->lineRange->GetEntry1()->GetValueAsDouble();
              if(std::abs(Slider1) > this->PVectorLength)
                {
           this->lineTip1[0] = this->lineCenter[0] + std::abs(Slider1)*this->P1VectorNormalized[0];
           this->lineTip1[1] = this->lineCenter[1] + std::abs(Slider1)*this->P1VectorNormalized[1];
           this->lineTip1[2] = this->lineCenter[2] + std::abs(Slider1)*this->P1VectorNormalized[2];
                } 
              else
                {
                this->lineTip1[0] = this->dpoint1[0];
                this->lineTip1[1] = this->dpoint1[1];
                this->lineTip1[2] = this->dpoint1[2];
                }
       
              // Tip 2
              double Slider2 = this->lineRange->GetEntry2()->GetValueAsDouble();
              if(std::abs(Slider2) > this->PVectorLength)
                {
                  this->lineTip2[0] = this->lineCenter[0] + std::abs(Slider2)*this->P2VectorNormalized[0];
           this->lineTip2[1] = this->lineCenter[1] + std::abs(Slider2)*this->P2VectorNormalized[1];
           this->lineTip2[2] = this->lineCenter[2] + std::abs(Slider2)*this->P2VectorNormalized[2];
                }
              else
                {
                  this->lineTip2[0] = this->dpoint2[0];
                this->lineTip2[1] = this->dpoint2[1];
                this->lineTip2[2] = this->dpoint2[2];
                }
       
            }
        else
            {
            // Tip 1
            double Slider1 = this->lineRange->GetEntry1()->GetValueAsDouble();
              if(std::abs(Slider1) > this->PVectorLength)
                {
                  this->lineTip1[0] = this->lineCenter[0] + std::abs(Slider1)*this->P2VectorNormalized[0];
           this->lineTip1[1] = this->lineCenter[1] + std::abs(Slider1)*this->P2VectorNormalized[1];
           this->lineTip1[2] = this->lineCenter[2] + std::abs(Slider1)*this->P2VectorNormalized[2];
                } 
              else
                {
                this->lineTip1[0] = this->dpoint2[0];
                this->lineTip1[1] = this->dpoint2[1];
                this->lineTip1[2] = this->dpoint2[2];
                }
       
              // Tip 2
              double Slider2 = this->lineRange->GetEntry2()->GetValueAsDouble();
              if(std::abs(Slider2) > this->PVectorLength)
                {
                  this->lineTip2[0] = this->lineCenter[0] + std::abs(Slider2)*this->P1VectorNormalized[0];
           this->lineTip2[1] = this->lineCenter[1] + std::abs(Slider2)*this->P1VectorNormalized[1];
           this->lineTip2[2] = this->lineCenter[2] + std::abs(Slider2)*this->P1VectorNormalized[2];
                }
              else
                {
                  this->lineTip2[0] = this->dpoint1[0];
                this->lineTip2[1] = this->dpoint1[1];
                this->lineTip2[2] = this->dpoint1[2];
                }
       
            }
       
        this->lineBetweenFiducials->SetPoint1(lineTip1);
        this->lineBetweenFiducials->SetPoint2(lineTip2);
        this->lineBetweenFiducials->Update();
       
        // Update Scale
        this->translation->SetRange(this->lineRange->GetEntry1()->GetValueAsDouble(),this->lineRange->GetEntry2()->GetValueAsDouble());
       
        this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
     }
      else
     {
     // FIXME: Fix bug when a slider reach the fiducial, both sliders came back to fiducial position
        // Need to know which slider is "touching" the fiducial
        this->lineRange->SetRange(-this->PVectorLength, this->PVectorLength );
     }
    }

  // Update Range Button Pressed
  if(this->UpdateWholeRangeButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
      if(this->WholeRangeWidget)
     {
          double WholeRangeValue = std::abs(this->WholeRangeWidget->GetWidget()->GetValueAsDouble());
       if(WholeRangeValue > this->PVectorLength)
         {
           this->lineRange->SetWholeRange(-WholeRangeValue,WholeRangeValue);
              this->lineRange->Modified();
         }
       else
         {
           this->lineRange->SetWholeRange(-this->PVectorLength,this->PVectorLength);
              this->lineRange->Modified();
         }
     }

    }


  if(this->translation == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
      if(this->translation)
     { 

          if(this->dpoint1[0] > this->dpoint2[0])
         {
           this->sphereCenter[0] = this->lineCenter[0] - this->translation->GetValue()*this->P1VectorNormalized[0];
           this->sphereCenter[1] = this->lineCenter[1] - this->translation->GetValue()*this->P1VectorNormalized[1];
              this->sphereCenter[2] = this->lineCenter[2] - this->translation->GetValue()*this->P1VectorNormalized[2];
            
         }
       else
         {
           this->sphereCenter[0] = this->lineCenter[0] - this->translation->GetValue()*this->P2VectorNormalized[0];
              this->sphereCenter[1] = this->lineCenter[1] - this->translation->GetValue()*this->P2VectorNormalized[1];
              this->sphereCenter[2] = this->lineCenter[2] - this->translation->GetValue()*this->P2VectorNormalized[2];
         }

          // Update Planes
          this->PerpendicularPlane->SetCenter(this->sphereCenter[0],this->sphereCenter[1],this->sphereCenter[2]);
          this->PerpendicularPlane->Update();

          this->Plane0->SetCenter(this->sphereCenter[0],this->sphereCenter[1],this->sphereCenter[2]);
          this->Plane0->Update();

          this->Plane90->SetCenter(this->sphereCenter[0],this->sphereCenter[1],this->sphereCenter[2]);
          this->Plane90->Update();

       // Update vtkMRMLLinearTransformNode
          if(this->transformMatrix && this->transformNode)
         {
           vtkMatrix4x4* tempMatrix = this->transformNode->GetMatrixTransformToParent();
              this->transformMatrix->Identity();
              this->transformMatrix->SetElement(0,3,this->sphereCenter[0]-this->lineCenter[0]);
              this->transformMatrix->SetElement(1,3,this->sphereCenter[1]-this->lineCenter[1]);
              this->transformMatrix->SetElement(2,3,this->sphereCenter[2]-this->lineCenter[2]);

              tempMatrix->DeepCopy(this->transformMatrix);
              this->transformNode->Modified();
         }

       // Reslicing
       // Update Render if slices not present
       if( !this->toggleReslicing->GetWidget()->GetSelectedState() )
        {
           this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      else
        {
           // FIXME: flip vectors ?
        this->GetLogic()->UpdateSliceNode(this->normalVector1, this->P1VectorNormalized,this->sphereCenter,0);
           this->GetLogic()->UpdateSliceNode(this->P1VectorNormalized,this->normalVector2,this->sphereCenter,1);
           this->GetLogic()->UpdateSliceNode(this->normalVector1,this->normalVector2,this->sphereCenter,2);
        }
     }

    }



  if(this->PlaneRotation == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
      if(this->PerpendicularPlane && this->P1Vector)
     {
          DrawPlanes(this->PlaneRotation->GetValue(), this->sphereCenter);

         // Update Render if slices are not present
       if( !this->toggleReslicing->GetWidget()->GetSelectedState() )
        {
           this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      else
        {
           this->GetLogic()->UpdateSliceNode(this->normalVector1, this->P1VectorNormalized,this->sphereCenter,0);
           this->GetLogic()->UpdateSliceNode(this->P1VectorNormalized,this->normalVector2,this->sphereCenter,1);
           this->GetLogic()->UpdateSliceNode(this->normalVector1,this->normalVector2,this->sphereCenter,2);
        }
     }

    }

  if(this->togglePlaneVisibility->GetWidget() == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    if(this->planeActor && this->plane0Actor && this->plane90Actor)
      {
      this->planeActor->SetVisibility(this->togglePlaneVisibility->GetWidget()->GetSelectedState());
      this->plane0Actor->SetVisibility(this->togglePlaneVisibility->GetWidget()->GetSelectedState());
      this->plane90Actor->SetVisibility(this->togglePlaneVisibility->GetWidget()->GetSelectedState());
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      }
    }

  if(this->toggleReslicing->GetWidget() == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    if(this->planeActor && this->plane0Actor && this->plane90Actor)
      {
      this->GetLogic()->UpdateSliceNode(this->normalVector1, this->P1VectorNormalized,this->sphereCenter,0);
      this->GetLogic()->UpdateSliceNode(this->P1VectorNormalized,this->normalVector2,this->sphereCenter,1);
      this->GetLogic()->UpdateSliceNode(this->normalVector1,this->normalVector2,this->sphereCenter,2);
      }
    }


  if(this->GetMRMLScene() == vtkMRMLScene::SafeDownCast(caller)
     && event == vtkMRMLScene::NodeRemovedEvent)
    {
      vtkMRMLFiducialListNode* removedNode = reinterpret_cast<vtkMRMLFiducialListNode*>(callData);
      if(this->fiducialLists && this->lineActors)
     {
          int item_present = this->fiducialLists->IsItemPresent(removedNode);
       if(item_present > 0)
         {
              // Remove actor in the renderer
              vtkActor* removedActor = reinterpret_cast<vtkActor*>(this->lineActors->GetItemAsObject(item_present-1));
              this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->RemoveActor(removedActor);

              // Remove node from the fiducial list
              this->fiducialLists->RemoveItem(removedNode);                            

              // Remove Actor from the list
           this->lineActors->RemoveItem(item_present-1);
         }
     }

    }

} 


void vtkBiopsyModuleGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkBiopsyModuleGUI *self = reinterpret_cast<vtkBiopsyModuleGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBiopsyModuleGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkBiopsyModuleLogic::SafeDownCast(caller))
    {
    if (event == vtkBiopsyModuleLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "BiopsyModule", "BiopsyModule", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForBiopsyModule();

}


void vtkBiopsyModuleGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:BiopsyModule</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "BiopsyModule" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleGUI::BuildGUIForBiopsyModule()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BiopsyModule");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("BiopsyModule");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Fiducial frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Select Fiducial List");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );


  vtkKWFrame *frame4 = vtkKWFrame::New();
  frame4->SetParent(frame->GetFrame());
  frame4->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame4->GetWidgetName() );
  
  
  // -----------------------------------------
  // Node Selector (vtkMRMLFiducialListNode)

  this->fiducialListWidget = vtkSlicerNodeSelectorWidget::New ( );
  this->fiducialListWidget->SetParent ( frame4 );
  this->fiducialListWidget->Create ( );
  this->fiducialListWidget->SetNodeClass("vtkMRMLFiducialListNode",NULL,NULL,NULL);
  //this->fiducialListWidget->SetWidth(30);
  this->fiducialListWidget->SetNewNodeEnabled(1);
  this->fiducialListWidget->SetMRMLScene(this->GetMRMLScene());
  this->fiducialListWidget->UpdateMenu();

  // -----------------------------------------
  // Draw line button

  this->drawline = vtkKWPushButton::New();
  this->drawline->SetParent ( frame4 );
  this->drawline->Create();
  this->drawline->SetText("Draw Line");


  app->Script("pack %s %s -fill x -side top -expand y -padx 2 -pady 2",
               this->fiducialListWidget->GetWidgetName(), 
               this->drawline->GetWidgetName());


  // -----------------------------------------
  // Keep and remove line buttons

  vtkKWFrame *frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();
  app->Script ( "pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame3->GetWidgetName() );
  

  // -----------------------------------------
  // Motion frame

  vtkKWFrameWithLabel *frame2 = vtkKWFrameWithLabel::New();
  frame2->SetParent(conBrowsFrame->GetFrame());
  frame2->Create();
  frame2->SetLabelText ("Motion Sliders");

  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame2->GetWidgetName() );
  
  this->WholeRangeWidget = vtkKWEntryWithLabel::New();
  this->WholeRangeWidget->SetParent(frame2->GetFrame());
  this->WholeRangeWidget->Create();
  this->WholeRangeWidget->SetLabelText("Set Range Value (range between -x and x)");
 
  this->UpdateWholeRangeButton = vtkKWPushButton::New();
  this->UpdateWholeRangeButton->SetParent(frame2->GetFrame());
  this->UpdateWholeRangeButton->Create();
  this->UpdateWholeRangeButton->SetText("Update range"); 

  this->lineRange = vtkKWRange::New();
  this->lineRange->SetParent(frame2->GetFrame());
  this->lineRange->Create();
  this->lineRange->SetLabelText("Line Range");
  this->lineRange->SetEnabled(0);


  this->translation = vtkKWScale::New();
  this->translation->SetParent(frame2->GetFrame());
  this->translation->SetResolution(0.1);
  this->translation->Create();
  this->translation->SetEnabled(0);
  this->translation->SetLabelText("Translation");
  this->translation->SetRange(-100,100);

  this->PlaneRotation = vtkKWScale::New();
  this->PlaneRotation->SetParent(frame2->GetFrame());
  this->PlaneRotation->Create();
  this->PlaneRotation->SetEnabled(0);
  this->PlaneRotation->SetRange(0,180);

  this->togglePlaneVisibility = vtkKWCheckButtonWithLabel::New();
  this->togglePlaneVisibility->SetParent(frame2->GetFrame());
  this->togglePlaneVisibility->Create();
  this->togglePlaneVisibility->SetLabelText("Show plane");
  this->togglePlaneVisibility->GetWidget()->SetSelectedState(0);
  this->togglePlaneVisibility->SetEnabled(0);

  this->toggleReslicing = vtkKWCheckButtonWithLabel::New();
  this->toggleReslicing->SetParent(frame2->GetFrame());
  this->toggleReslicing->Create();
  this->toggleReslicing->SetLabelText("Reslicing");
  this->toggleReslicing->GetWidget()->SetSelectedState(0);
  this->toggleReslicing->SetEnabled(0);


  this->Script("pack %s %s %s %s %s %s %s -side top -fill x -padx 2 -pady 2",
               this->WholeRangeWidget->GetWidgetName(),
               this->UpdateWholeRangeButton->GetWidgetName(), 
               this->lineRange->GetWidgetName(),
               this->translation->GetWidgetName(),
               this->PlaneRotation->GetWidgetName(),
               this->togglePlaneVisibility->GetWidgetName(),
               this->toggleReslicing->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame4->Delete();
}


//----------------------------------------------------------------------------
void vtkBiopsyModuleGUI::UpdateAll()
{
}


void vtkBiopsyModuleGUI::DrawPlanes(double angle_in_deg, double* center)
{

vtkMath* NormalVectorMath = vtkMath::New();

NormalVectorMath->Perpendiculars(this->P1VectorNormalized, this->normalVector1, this->normalVector2, NormalVectorMath->RadiansFromDegrees(angle_in_deg));
NormalVectorMath->Delete();

double Point1XYZ[3] = {50*this->normalVector1[0],
                       50*this->normalVector1[1],
                       50*this->normalVector1[2]};

double Point2XYZ[3] = {50*this->normalVector2[0],
                       50*this->normalVector2[1],
                       50*this->normalVector2[2]};

double Point3XYZ[3] = {50*this->P1VectorNormalized[0],
                       50*this->P1VectorNormalized[1],
                       50*this->P1VectorNormalized[2]};

double Point4XYZ[3] = {50*this->normalVector2[0],
                       50*this->normalVector2[1],
                       50*this->normalVector2[2]};

double Point5XYZ[3] = {50*this->normalVector1[0],
                       50*this->normalVector1[1],
                       50*this->normalVector1[2]};

double Point6XYZ[3] = {50*this->P1VectorNormalized[0],
                       50*this->P1VectorNormalized[1],
                       50*this->P1VectorNormalized[2]};


this->PerpendicularPlane->SetOrigin(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);  
this->PerpendicularPlane->SetPoint1(this->lineCenter[0]+Point1XYZ[0],this->lineCenter[1]+Point1XYZ[1],this->lineCenter[2]+Point1XYZ[2]);
this->PerpendicularPlane->SetPoint2(this->lineCenter[0]+Point2XYZ[0],this->lineCenter[1]+Point2XYZ[1],this->lineCenter[2]+Point2XYZ[2]);
this->PerpendicularPlane->SetCenter(center[0],center[1],center[2]);
this->PerpendicularPlane->Update();

this->Plane0->SetOrigin(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);  
this->Plane0->SetPoint1(this->lineCenter[0]+Point3XYZ[0],this->lineCenter[1]+Point3XYZ[1],this->lineCenter[2]+Point3XYZ[2]);
this->Plane0->SetPoint2(this->lineCenter[0]+Point4XYZ[0],this->lineCenter[1]+Point4XYZ[1],this->lineCenter[2]+Point4XYZ[2]);
this->Plane0->SetCenter(center[0],center[1],center[2]);
this->Plane0->Update();

this->Plane90->SetOrigin(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);  
this->Plane90->SetPoint1(this->lineCenter[0]+Point5XYZ[0],this->lineCenter[1]+Point5XYZ[1],this->lineCenter[2]+Point5XYZ[2]);
this->Plane90->SetPoint2(this->lineCenter[0]+Point6XYZ[0],this->lineCenter[1]+Point6XYZ[1],this->lineCenter[2]+Point6XYZ[2]);
this->Plane90->SetCenter(center[0],center[1],center[2]);
this->Plane90->Update();

}
