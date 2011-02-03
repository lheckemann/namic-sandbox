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
#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"

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

  this->translation = NULL;
  this->transformation = NULL;
  this->transformNode = NULL;
  this->lineBetweenFiducials = NULL;
  this->lineRange = NULL;

  this->WholeRangeWidget = NULL;
  this->UpdateWholeRangeButton = NULL;

  this->sphereCenterPlane = NULL;

  this->AxisPlane = NULL;
  this->PlaneRotation = NULL;

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

  this->normalVector[0] = 0;
  this->normalVector[1] = 0;
  this->normalVector[2] = 0;

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

  if (this->translation)
    {
    this->translation->SetParent(NULL);
    this->translation->Delete();
    }

  if (this->transformation)
    {
    this->transformation->Delete();
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

  if (this->sphereCenterPlane)
    {
    this->sphereCenterPlane->Delete();
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

  if (this->fiducialListNode)
    {
    this->fiducialListNode
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
        this->fiducialListNode->SetName("LineMotionFiducialList");
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
     
      // Create a line if not already existing
      if(!this->lineBetweenFiducials)
        {
        this->lineBetweenFiducials = vtkLineSource::New();
        }
         
      this->lineBetweenFiducials->SetPoint1(dpoint1);
      this->lineBetweenFiducials->SetPoint2(dpoint2);
      this->lineBetweenFiducials->Update();
     
      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
      mapper->SetInput(this->lineBetweenFiducials->GetOutput());
     
      vtkActor* lineactor = vtkActor::New();
      lineactor->SetMapper(mapper);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(lineactor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapper->Delete();
      lineactor->Delete();
      
      // Create a transform node if not already existing
      if(!this->transformation && !this->transformNode)
        {
        this->transformation = vtkTransform::New();
        this->transformNode = vtkMRMLLinearTransformNode::New();
        this->transformNode->SetScene(this->GetMRMLScene());
        this->GetMRMLScene()->AddNode(this->transformNode);
        }
     
      // Enable Controllers 
      this->lineRange->SetEnabled(1);
      this->translation->SetEnabled(1);
      this->PlaneRotation->SetEnabled(1);
      
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

      // Add Sphere in the center
      if(!this->sphereCenterPlane)
        {
        this->sphereCenterPlane = vtkSphereSource::New();
        }
 
      if(this->sphereCenterPlane && this->lineCenter)
     {        
        this->sphereCenterPlane->SetRadius(5);
        this->sphereCenterPlane->SetCenter(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);
        this->sphereCenterPlane->Update();
     }
     
      vtkPolyDataMapper* mapperSphere = vtkPolyDataMapper::New();
      mapperSphere->SetInput(this->sphereCenterPlane->GetOutput());
     
      vtkActor* sphereactor = vtkActor::New();
      sphereactor->SetMapper(mapperSphere);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(sphereactor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperSphere->Delete();
      sphereactor->Delete();

      // Add Plane in the center
      // -- Calculate normal vector
      vtkMath* findNormalVector = vtkMath::New();
      findNormalVector->Perpendiculars(this->P1Vector, this->normalVector, NULL, 0);

      findNormalVector->Delete();
      
      // -- Build vtkPlaneSource
      if(!this->AxisPlane)
        {
        this->AxisPlane = vtkPlaneSource::New();
        }
 
      // FIXME: Orientation of the plane (How to place point 1 and point 2 to have a plane in direction of the vector ?)
      if(this->AxisPlane && this->lineCenter)
     {        
        this->AxisPlane->SetPoint1(this->lineCenter[0]-50,this->lineCenter[1]-50,this->lineCenter[2]-50);
        this->AxisPlane->SetPoint2(this->lineCenter[0]+50,this->lineCenter[1]+50,this->lineCenter[2]+50);
        this->AxisPlane->SetCenter(this->lineCenter[0],this->lineCenter[1],this->lineCenter[2]);
        this->AxisPlane->SetNormal(this->normalVector);
        this->AxisPlane->Update();
     }
     
      vtkPolyDataMapper* mapperPlane = vtkPolyDataMapper::New();
      mapperPlane->SetInput(this->AxisPlane->GetOutput());
     
      vtkActor* planeactor = vtkActor::New();
      planeactor->SetMapper(mapperPlane);
     
      this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(planeactor);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      
      mapperPlane->Delete();
      planeactor->Delete();   
  
      // Reset Scale
      if(this->translation)
       {
      this->translation->SetValue(0);
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
                  this->lineTip1[0] = this->lineCenter[0] + Slider1*this->P1VectorNormalized[0];
                this->lineTip1[1] = this->lineCenter[1] + Slider1*this->P1VectorNormalized[1];
                this->lineTip1[2] = this->lineCenter[2] + Slider1*this->P1VectorNormalized[2];
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
                  this->lineTip2[0] = this->lineCenter[0] + Slider2*this->P2VectorNormalized[0];
                this->lineTip2[1] = this->lineCenter[1] + Slider2*this->P2VectorNormalized[1];
                this->lineTip2[2] = this->lineCenter[2] + Slider2*this->P2VectorNormalized[2];
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
                  this->lineTip1[0] = this->lineCenter[0] - Slider1*this->P2VectorNormalized[0];
                this->lineTip1[1] = this->lineCenter[1] - Slider1*this->P2VectorNormalized[1];
                this->lineTip1[2] = this->lineCenter[2] - Slider1*this->P2VectorNormalized[2];
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
                  this->lineTip2[0] = this->lineCenter[0] + Slider2*this->P1VectorNormalized[0];
                this->lineTip2[1] = this->lineCenter[1] + Slider2*this->P1VectorNormalized[1];
                this->lineTip2[2] = this->lineCenter[2] + Slider2*this->P1VectorNormalized[2];
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
      if(this->translation && this->sphereCenterPlane)
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
       this->sphereCenterPlane->SetCenter(sphereCenter[0],sphereCenter[1],sphereCenter[2]);
          this->sphereCenterPlane->Update();

          // Update Plane
          this->AxisPlane->SetCenter(sphereCenter[0],sphereCenter[1],sphereCenter[2]);
          this->AxisPlane->Update();

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();
     }

    }



  if(this->PlaneRotation == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
      if(this->AxisPlane && this->P1Vector)
     {
       vtkMath* NormalVectorMath = vtkMath::New();
          double normalVector[3] = {0,0,0};
   
          NormalVectorMath->Perpendiculars(this->P1Vector, normalVector, NULL, NormalVectorMath->RadiansFromDegrees(this->PlaneRotation->GetValue()));
          this->AxisPlane->SetNormal(normalVector);
          this->AxisPlane->Update();

          this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

          NormalVectorMath->Delete();
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
  
  // -----------------------------------------
  // Node Selector (vtkMRMLFiducialListNode)

  this->fiducialListWidget = vtkSlicerNodeSelectorWidget::New ( );
  this->fiducialListWidget->SetParent ( frame->GetFrame() );
  this->fiducialListWidget->Create ( );
  this->fiducialListWidget->SetNodeClass("vtkMRMLFiducialListNode",NULL,NULL,NULL);
  this->fiducialListWidget->SetWidth(30);
  this->fiducialListWidget->SetNewNodeEnabled(0);
  this->fiducialListWidget->SetMRMLScene(this->GetMRMLScene());
  this->fiducialListWidget->UpdateMenu();

  // -----------------------------------------
  // Draw line button

  this->drawline = vtkKWPushButton::New();
  this->drawline->SetParent ( frame->GetFrame() );
  this->drawline->Create();
  this->drawline->SetText("Draw Line");

  this->Script("pack %s %s -side top -fill x -padx 2 -pady 2", 
               this->fiducialListWidget->GetWidgetName(),
               this->drawline->GetWidgetName());


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

  this->Script("pack %s %s %s %s %s -side top -fill x -padx 2 -pady 2",
               this->WholeRangeWidget->GetWidgetName(),
               this->UpdateWholeRangeButton->GetWidgetName(), 
               this->lineRange->GetWidgetName(),
               this->translation->GetWidgetName(),
               this->PlaneRotation->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();


}


//----------------------------------------------------------------------------
void vtkLineMotionGUI::UpdateAll()
{
}
