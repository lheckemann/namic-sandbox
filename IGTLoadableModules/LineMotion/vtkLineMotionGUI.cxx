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

#include "vtkLineMotionGUI.h"
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

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkLineMotionGUI );
vtkCxxRevisionMacro ( vtkLineMotionGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------
 

//---------------------------------------------------------------------------
vtkLineMotionGUI::vtkLineMotionGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkLineMotionGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  
  this->fiducialListWidget = NULL;
  this->fiducialListNode = NULL;
  this->drawline = NULL;
  this->keepLine = NULL;
  this->removeLine = NULL;

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

  this->AxisPlane = vtkPlaneSource::New();
  this->PlaneRotation = NULL;

  this->togglePlaneVisibility = NULL;
  this->planeActor = vtkActor::New();

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


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkLineMotionGUI::~vtkLineMotionGUI ( )
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

  if (this->keepLine)
    {
    this->keepLine->SetParent(NULL);
    this->keepLine->Delete();
    }

  if (this->removeLine)
    {
    this->removeLine->SetParent(NULL);
    this->removeLine->Delete();
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

  if (this->AxisPlane)
    {
    this->AxisPlane->Delete();
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

 if (this->transformMatrix)
    {
    this->transformMatrix->Delete();
    }


  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::Enter()
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
void vtkLineMotionGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "LineMotionGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";

}
//---------------------------------------------------------------------------
void vtkLineMotionGUI::RemoveGUIObservers ( )
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

  if (this->keepLine)
    {
    this->keepLine
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->removeLine)
    {
    this->removeLine
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

  if(this->GetMRMLScene())
    {
      this->GetMRMLScene()
        ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::AddGUIObservers ( )
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

this->keepLine
   ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand); 

this->removeLine
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

 this->GetMRMLScene()
   ->AddObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkLineMotionGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkLineMotionLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkLineMotionGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::ProcessGUIEvents(vtkObject *caller,
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
        //this->fiducialListNode->SetName("LineMotionFiducialList");
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
      if(this->AxisPlane && this->lineCenter)
     {
      double Point1XYZ[3] = {50*this->normalVector1[0],
                               50*this->normalVector1[1],
                               50*this->normalVector1[2]};

      double Point2XYZ[3] = {50*this->normalVector2[0],
                               50*this->normalVector2[1],
                               50*this->normalVector2[2]};


        this->AxisPlane->SetOrigin(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);  
        this->AxisPlane->SetPoint1(this->lineCenter[0]+Point1XYZ[0],this->lineCenter[1]+Point1XYZ[1],this->lineCenter[2]+Point1XYZ[2]);
        this->AxisPlane->SetPoint2(this->lineCenter[0]+Point2XYZ[0],this->lineCenter[1]+Point2XYZ[1],this->lineCenter[2]+Point2XYZ[2]);
     this->AxisPlane->SetCenter(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);
        this->AxisPlane->Update();

     }
     
      vtkPolyDataMapper* mapperPlane = vtkPolyDataMapper::New();
      mapperPlane->SetInput(this->AxisPlane->GetOutput());
     
      this->planeActor->SetMapper(mapperPlane);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(this->planeActor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperPlane->Delete();
 
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
        this->lineRange->SetRange(-this->PVectorLength, this->PVectorLength);
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
          double sphereCenter[3] = {this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]};

          if(this->dpoint1[0] > this->dpoint2[0])
         {
           sphereCenter[0] = this->lineCenter[0] - this->translation->GetValue()*this->P1VectorNormalized[0];
           sphereCenter[1] = this->lineCenter[1] - this->translation->GetValue()*this->P1VectorNormalized[1];
              sphereCenter[2] = this->lineCenter[2] - this->translation->GetValue()*this->P1VectorNormalized[2];
            
         }
       else
         {
           sphereCenter[0] = this->lineCenter[0] - this->translation->GetValue()*this->P2VectorNormalized[0];
              sphereCenter[1] = this->lineCenter[1] - this->translation->GetValue()*this->P2VectorNormalized[1];
              sphereCenter[2] = this->lineCenter[2] - this->translation->GetValue()*this->P2VectorNormalized[2];
         }

          // Update Plane
          this->AxisPlane->SetCenter(sphereCenter[0],sphereCenter[1],sphereCenter[2]);
          this->AxisPlane->Update();

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

       // Update vtkMRMLLinearTransformNode
          if(this->transformMatrix && this->transformNode)
         {
           vtkMatrix4x4* tempMatrix = this->transformNode->GetMatrixTransformToParent();
              this->transformMatrix->Identity();
              this->transformMatrix->SetElement(0,3,sphereCenter[0]-this->lineCenter[0]);
              this->transformMatrix->SetElement(1,3,sphereCenter[1]-this->lineCenter[1]);
              this->transformMatrix->SetElement(2,3,sphereCenter[2]-this->lineCenter[2]);

              tempMatrix->DeepCopy(this->transformMatrix);
              this->transformNode->Modified();
         } 
     }

    }



  if(this->PlaneRotation == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
      if(this->AxisPlane && this->P1Vector)
     {
       /*
       vtkMath* NormalVectorMath = vtkMath::New();
          double normalVector[3] = {0,0,0};
   
          NormalVectorMath->Perpendiculars(this->P1Vector, normalVector, NULL, NormalVectorMath->RadiansFromDegrees(this->PlaneRotation->GetValue()));
          this->AxisPlane->SetNormal(this->P1Vector);
          this->AxisPlane->Update();

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

          NormalVectorMath->Delete();
       */
     }

    }

  if(this->togglePlaneVisibility->GetWidget() == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      if(this->AxisPlane)
     {
        this->planeActor->SetVisibility(this->togglePlaneVisibility->GetWidget()->GetSelectedState());
        this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
     }
    }


  if(this->keepLine == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
      bool already_inside = false;
      if(this->fiducialLists && this->fiducialListNode)
     {
       for(int i=0;i<this->fiducialLists->GetNumberOfItems();i++)
         {
           vtkMRMLFiducialListNode* tempFiducialListNode = reinterpret_cast<vtkMRMLFiducialListNode*>(this->fiducialLists->GetItemAsObject(i));
              if(tempFiducialListNode->GetID() == this->fiducialListNode->GetID())
          {
            already_inside = true;
          }
         }
           
          if(!already_inside)
         {
              this->fiducialLists->AddItem(this->fiducialListNode);
              this->GetLogic()->RefreshLines(this->fiducialListNode,this->lineActors, this->colorActors , this->GetApplicationGUI());
              this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
         }

     }

    }


  if(this->removeLine == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
      bool already_inside = false;
      if(this->fiducialLists && this->fiducialListNode)
     {
       for(int i=0;i<this->fiducialLists->GetNumberOfItems();i++)
         {
           vtkMRMLFiducialListNode* tempFiducialListNode = reinterpret_cast<vtkMRMLFiducialListNode*>(this->fiducialLists->GetItemAsObject(i));
              if(tempFiducialListNode->GetID() == this->fiducialListNode->GetID())
          {
            already_inside = true;
          }
         }
           
          if(already_inside)
         {
           int item_present = this->fiducialLists->IsItemPresent(this->fiducialListNode);
 
           if(item_present > 0)
          {
            int item_number = item_present - 1;
                  this->fiducialLists->RemoveItem(this->fiducialListNode);
                  vtkActor* ItemToRemove = reinterpret_cast<vtkActor*>(this->lineActors->GetItemAsObject(item_number)); 
                  this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->RemoveActor(ItemToRemove);
                  this->lineActors->RemoveItem(ItemToRemove);
                  this->colorActors->RemoveItem(ItemToRemove);

                 this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
          }
         }

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


void vtkLineMotionGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkLineMotionGUI *self = reinterpret_cast<vtkLineMotionGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkLineMotionGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkLineMotionLogic::SafeDownCast(caller))
    {
    if (event == vtkLineMotionLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::ProcessTimerEvents()
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
void vtkLineMotionGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "LineMotion", "LineMotion", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLineMotion();

}


void vtkLineMotionGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:LineMotion</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "LineMotion" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkLineMotionGUI::BuildGUIForLineMotion()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("LineMotion");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("LineMotion");
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
  
  this->keepLine = vtkKWPushButton::New();
  this->keepLine->SetParent ( frame3 );
  this->keepLine->Create();
  this->keepLine->SetText("Keep Line");
 
  this->removeLine = vtkKWPushButton::New();
  this->removeLine->SetParent ( frame3 );
  this->removeLine->Create();
  this->removeLine->SetText("Remove Line");

  app->Script("pack %s %s -side left -fill x -expand y -padx 2 -pady 2", 
               this->keepLine->GetWidgetName(),
               this->removeLine->GetWidgetName());
 

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

  this->Script("pack %s %s %s %s %s %s -side top -fill x -padx 2 -pady 2",
               this->WholeRangeWidget->GetWidgetName(),
               this->UpdateWholeRangeButton->GetWidgetName(), 
               this->lineRange->GetWidgetName(),
               this->translation->GetWidgetName(),
               this->PlaneRotation->GetWidgetName(),
               this->togglePlaneVisibility->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame4->Delete();
}


//----------------------------------------------------------------------------
void vtkLineMotionGUI::UpdateAll()
{
}
