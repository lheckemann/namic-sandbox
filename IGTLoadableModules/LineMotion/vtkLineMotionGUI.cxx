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

  this->lineLength = 0;

  this->vector_normalized[0] = 0;
  this->vector_normalized[1] = 0;
  this->vector_normalized[2] = 0;

  this->dpoint1[0] = 0;
  this->dpoint1[1] = 0;
  this->dpoint1[2] = 0;

  this->dpoint2[0] = 0;
  this->dpoint2[1] = 0;
  this->dpoint2[2] = 0;

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
         
      lineBetweenFiducials->SetPoint1(dpoint1);
      lineBetweenFiducials->SetPoint2(dpoint2);
      lineBetweenFiducials->Update();
     
      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
      mapper->SetInput(lineBetweenFiducials->GetOutput());
     
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
     
      // Create normalized vector representing direction of the line
      double vector[3] = {dpoint1[0]-dpoint2[0], dpoint1[1]-dpoint2[1], dpoint1[2]-dpoint2[2]};
      double normalization = vector[0]+vector[1]+vector[2];
      this->vector_normalized[0] = vector[0]/normalization;
      this->vector_normalized[1] = vector[1]/normalization;
      this->vector_normalized[2] = vector[2]/normalization;

      // Enable Controllers 
      this->lineRange->SetEnabled(1);
      this->translation->SetEnabled(1);

      // Set vtkKWRange widget
      vtkMath* DistanceBetweenPoint = vtkMath::New();
      
        // Square root is missing on Distance2BetweenPoints
      this->lineLength = sqrt( DistanceBetweenPoint->Distance2BetweenPoints(dpoint1,dpoint2) );
      this->lineRange->SetWholeRange((-this->lineLength/2)-200,(this->lineLength/2)+200);
      this->lineRange->SetRange(-this->lineLength/2,this->lineLength/2);
      this->lineRange->Modified();

      DistanceBetweenPoint->Delete();

      // Set Default Whole Range value
      this->WholeRangeWidget->GetWidget()->SetValueAsDouble(this->lineLength/2+200);

      }
   }

  // lineRange Changed
  if(this->lineRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
      // Minimum size
      if(this->lineRange->GetEntry1()->GetValueAsDouble() > (-this->lineLength/2))
     {
       this->lineRange->GetEntry1()->SetValueAsDouble(-this->lineLength/2);
     }
      if(this->lineRange->GetEntry2()->GetValueAsDouble() < (this->lineLength/2))
     {
       this->lineRange->GetEntry2()->SetValueAsDouble(this->lineLength/2);
     }
     
      // Update Line
         // Orientation of the normalized vector
      int xsign = 1;
      int ysign = 1;
      int zsign = 1;

      if(dpoint2[0] > dpoint1[0]) { xsign = -1; } 
      if(dpoint2[1] > dpoint1[1]) { ysign = -1; }
      if(dpoint2[2] > dpoint1[2]) { zsign = -1; } 
     
      double point1[3] = { dpoint1[0] + xsign*((std::abs(this->lineRange->GetEntry1()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[0])),
                  dpoint1[1] + ysign*((std::abs(this->lineRange->GetEntry1()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[1])),
                  dpoint1[2] + zsign*((std::abs(this->lineRange->GetEntry1()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[2]))};
      this->lineBetweenFiducials->SetPoint1(point1);

      double point2[3] = { dpoint2[0] - xsign*((std::abs(this->lineRange->GetEntry2()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[0])),
                           dpoint2[1] - ysign*((std::abs(this->lineRange->GetEntry2()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[1])),
                           dpoint2[2] - zsign*((std::abs(this->lineRange->GetEntry2()->GetValueAsDouble())-(this->lineLength/2))*std::abs(this->vector_normalized[2]))};
      this->lineBetweenFiducials->SetPoint2(point2);

      this->lineBetweenFiducials->Update();
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

    }

  // Update Range Button Pressed
  if(this->UpdateWholeRangeButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
      if(this->WholeRangeWidget)
     {
          double WholeRangeValue = std::abs(this->WholeRangeWidget->GetWidget()->GetValueAsDouble());
       if(WholeRangeValue > this->lineLength/2)
         {
           this->lineRange->SetWholeRange(-WholeRangeValue,WholeRangeValue);
              this->lineRange->Modified();
         }
       else
         {
           this->lineRange->SetWholeRange(-this->lineLength/2,this->lineLength/2);
              this->lineRange->Modified();
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
  this->translation->Create();
  this->translation->SetEnabled(0);
  this->translation->SetLabelText("Translation");
  this->translation->SetRange(-100,100);

  this->Script("pack %s %s %s %s -side top -fill x -padx 2 -pady 2",
               this->WholeRangeWidget->GetWidgetName(),
               this->UpdateWholeRangeButton->GetWidgetName(), 
               this->lineRange->GetWidgetName(),
               this->translation->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();


}


//----------------------------------------------------------------------------
void vtkLineMotionGUI::UpdateAll()
{
}
