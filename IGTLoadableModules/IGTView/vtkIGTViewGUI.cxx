/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkIGTViewGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"
#include "vtkCornerAnnotation.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWMenuButtonWithLabel.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLCrosshairNode.h"
#include "vtkSlicerSlicesControlGUI.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkIGTViewGUI );
vtkCxxRevisionMacro ( vtkIGTViewGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkIGTViewGUI::vtkIGTViewGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkIGTViewGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets

  /*
  this->linkerCheck = NULL;
  this->DefaultViewButton = NULL;
  */

  this->GreenViewerMenu = NULL;
  this->YellowViewerMenu = NULL;
  this->RedViewerMenu = NULL;
  this->Viewer3DMenu = NULL;

  //----------------------------------------------------------------
  // Slice

  this->SliceNodeRed = NULL;
  this->SliceNodeYellow = NULL;
  this->SliceNodeRed = NULL;

  //----------------------------------------------------------------
  // Overlay

  this->crosshairButton = NULL;
  this->transformNodeSelector = NULL;
  //  this->Crosshair = NULL;
  //this->ShowCrosshair = false;
  
  this->RedObliqueReslicing = false;
  this->YellowObliqueReslicing = false;
  this->GreenObliqueReslicing = false;

  this->RedReslicingType = "";
  this->YellowReslicingType = "";
  this->GreenReslicingType = "";

  this->trajectoryButton = NULL;
  this->trajectoryNodeSelector = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkIGTViewGUI::~vtkIGTViewGUI ( )
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
  /*
  if(this->linkerCheck)
    {
      this->linkerCheck->SetParent(NULL);
      this->linkerCheck->Delete();
    }

  if(this->DefaultViewButton)
    {
      this->DefaultViewButton->SetParent(NULL);
      this->DefaultViewButton->Delete();
    }
  */
  
  //----------------------------------------------------------------
  // Viewers

  if(this->RedViewerMenu)
    {
      this->RedViewerMenu->SetParent(NULL);
      this->RedViewerMenu->Delete();
    }

  if(this->YellowViewerMenu)
    {
      this->YellowViewerMenu->SetParent(NULL);
      this->YellowViewerMenu->Delete();
    }

  if(this->GreenViewerMenu)
    {
      this->GreenViewerMenu->SetParent(NULL);
      this->GreenViewerMenu->Delete();
    }

  if(this->Viewer3DMenu)
    {
      this->Viewer3DMenu->SetParent(NULL);
      this->Viewer3DMenu->Delete();
    }


  //----------------------------------------------------------------
  // Overlay

  if(this->crosshairButton)
    {
      this->crosshairButton->SetParent(NULL);
      this->crosshairButton->Delete();
    }

  if(this->transformNodeSelector)
    {
      this->transformNodeSelector->SetParent(NULL);
      this->transformNodeSelector->Delete();
    }


  if(this->trajectoryButton)
    {
      this->trajectoryButton->SetParent(NULL);
      this->trajectoryButton->Delete();
    }

  if(this->trajectoryNodeSelector)
    {
      this->trajectoryNodeSelector->SetParent(NULL);
      this->trajectoryNodeSelector->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();

  this->SliceNodeRed = appGUI->GetMainSliceGUI("Red")->GetLogic()->GetSliceNode();
  this->SliceNodeYellow = appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetSliceNode();
  this->SliceNodeGreen = appGUI->GetMainSliceGUI("Green")->GetLogic()->GetSliceNode();

}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "IGTViewGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::RemoveGUIObservers ( )
{
  /*
  if(this->linkerCheck)
    {
      this->linkerCheck->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->DefaultViewButton)
    {
      this->DefaultViewButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  */
  //----------------------------------------------------------------
  // Viewers
  
  
  if(this->RedViewerMenu)
    {
      this->RedViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->YellowViewerMenu)
    {
      this->YellowViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->GreenViewerMenu)
    {
      this->GreenViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->Viewer3DMenu)
    {
      this->Viewer3DMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Overlay

  if(this->crosshairButton)
    {
      this->crosshairButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->transformNodeSelector)
    {
      this->transformNodeSelector->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }



  if(this->trajectoryButton)
    {
      this->trajectoryButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->trajectoryNodeSelector)
    {
      this->trajectoryNodeSelector->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

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
  /*
  this->linkerCheck->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->DefaultViewButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  */
  //----------------------------------------------------------------
  // Viewers

  this->RedViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Viewer3DMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Overlay

  this->crosshairButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->transformNodeSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);

  this->trajectoryButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->trajectoryNodeSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }

}




//---------------------------------------------------------------------------
void vtkIGTViewGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkIGTViewLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkIGTViewGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //----------------------------------------------------------------
  // Red Frame

  else if (this->RedViewerMenu->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
      // Change red view
      if(this->SliceNodeRed)
  {    
    const char* redSelection = this->RedViewerMenu->GetWidget()->GetValue();
    if(!strcmp(redSelection, "Axial"))
      {
        this->RedObliqueReslicing = false;
        this->SliceNodeRed->SetOrientationToAxial();
      }
    else if(!strcmp(redSelection, "Sagittal"))
      {
        this->RedObliqueReslicing = false;
        this->SliceNodeRed->SetOrientationToSagittal();
      }
    else if(!strcmp(redSelection, "Coronal"))
      {
        this->RedObliqueReslicing = false;
        this->SliceNodeRed->SetOrientationToCoronal();
      }
    else if(!strcmp(redSelection, "InPlane0"))
      {
        this->RedObliqueReslicing = true;
        this->RedReslicingType = "Inplane0";
      }
    else if(!strcmp(redSelection, "InPlane90"))
      {
        this->RedObliqueReslicing = true;
        this->RedReslicingType = "Inplane90";
      }
    else if(!strcmp(redSelection, "Probe's Eye"))
      {
        this->RedObliqueReslicing = true;
        this->RedReslicingType = "Probe's Eye";
      }
  }
    }

  //----------------------------------------------------------------
  // Yellow Frame


  else if (this->YellowViewerMenu->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
      // Change yellow view
      if(this->SliceNodeYellow)
  {    
    const char* yellowSelection = this->YellowViewerMenu->GetWidget()->GetValue();
    if(!strcmp(yellowSelection, "Axial"))
      {
        this->YellowObliqueReslicing = false;
        this->SliceNodeYellow->SetOrientationToAxial();
      }
    else if(!strcmp(yellowSelection, "Sagittal"))
      {
        this->YellowObliqueReslicing = false;
        this->SliceNodeYellow->SetOrientationToSagittal();
      }
    else if(!strcmp(yellowSelection, "Coronal"))
      {
        this->YellowObliqueReslicing = false;
        this->SliceNodeYellow->SetOrientationToCoronal();
      }
    else if(!strcmp(yellowSelection, "InPlane0"))
      {
        this->YellowObliqueReslicing = true;
        this->YellowReslicingType = "Inplane0";
      } 
    else if(!strcmp(yellowSelection, "InPlane90"))
      {
        this->YellowObliqueReslicing = true;
        this->YellowReslicingType = "Inplane90";
      } 
    else if(!strcmp(yellowSelection, "Probe's Eye"))
      {
        this->YellowObliqueReslicing = true;
        this->YellowReslicingType = "Probe's Eye";
      } 

  }
    }

  //----------------------------------------------------------------
  // Green Frame

  else if (this->GreenViewerMenu->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
      // Change green view
      if(this->SliceNodeGreen)
  {    
    const char* greenSelection = this->GreenViewerMenu->GetWidget()->GetValue();
    if(!strcmp(greenSelection, "Axial"))
      {
        this->GreenObliqueReslicing = false;
        this->SliceNodeGreen->SetOrientationToAxial();
      }
    else if(!strcmp(greenSelection, "Sagittal"))
      {
        this->GreenObliqueReslicing = false;
        this->SliceNodeGreen->SetOrientationToSagittal();
      }
    else if(!strcmp(greenSelection, "Coronal"))
      {
        this->GreenObliqueReslicing = false;
        this->SliceNodeGreen->SetOrientationToCoronal();
      }
    else if(!strcmp(greenSelection, "InPlane0"))
      {
        this->GreenObliqueReslicing = true;
        this->GreenReslicingType = "Inplane0";
      } 
    else if(!strcmp(greenSelection, "InPlane90"))
      {
        this->GreenObliqueReslicing = true;
        this->GreenReslicingType = "Inplane90";
      } 
    else if(!strcmp(greenSelection, "Probe's Eye"))
      {
        this->GreenObliqueReslicing = true;
        this->GreenReslicingType = "Probe's Eye";
      } 

  }
    }

  //----------------------------------------------------------------
  // 3D Frame

  else if (this->Viewer3DMenu->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
      // Change 3D view

    }  

  //----------------------------------------------------------------
  // Overlay

  else if (this->crosshairButton == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      vtkMRMLCrosshairNode *Crosshair = this->GetApplicationGUI()->GetSlicesControlGUI()->GetCrosshairNode();

      if(this->crosshairButton->GetSelectedState())
  {
    if(Crosshair)
      {
        this->GetLogic()->SetCrosshairEnabled(true);
        this->GetLogic()->SetCrosshair(Crosshair);
        this->GetLogic()->UpdateCrosshair();
      }

    if(this->transformNodeSelector)
      {
        this->transformNodeSelector->SetEnabled(1);
        
        // Trick to update slices menu, even if transform node is already selected in the list before activating crosshair 
        this->transformNodeSelector->InvokeEvent(vtkSlicerNodeSelectorWidget::NodeSelectedEvent);
      }
  }
      else
  {
    if(Crosshair)
      {
        this->GetLogic()->SetCrosshairEnabled(false);
        this->GetLogic()->GetCrosshair()->SetCrosshairMode(vtkMRMLCrosshairNode::NoCrosshair);
      }

    if(this->transformNodeSelector)
      {
        this->transformNodeSelector->SetEnabled(0);
      }

    RemoveInplaneMenu();
  }
    } 

  else if (this->transformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
      if(this->GetLogic()->GetCrosshair())
  {
    if(this->GetLogic()->GetCrosshair()->GetCrosshairMode() == vtkMRMLCrosshairNode::ShowAll)
      {
        vtkMRMLLinearTransformNode* SelectedNode = vtkMRMLLinearTransformNode::SafeDownCast(this->transformNodeSelector->GetSelected());
        if(SelectedNode)
    {
      this->GetLogic()->SetTransformNodeSelected(true);
      this->GetLogic()->SetlocatorTransformNode(SelectedNode);

      // Update Menu
      AddInplaneMenu();  
    }
      }
  }
    }



  else if (this->trajectoryButton == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      if(this->trajectoryButton->GetSelectedState())
  {
    if(this->trajectoryNodeSelector)
      {
        this->trajectoryNodeSelector->SetEnabled(1);
      }
  }
      else
  {
    if(this->trajectoryNodeSelector)
      {
        this->trajectoryNodeSelector->SetEnabled(0);
      }
  }

    } 

  else if (this->trajectoryNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {

    }
} 


void vtkIGTViewGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkIGTViewGUI *self = reinterpret_cast<vtkIGTViewGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkIGTViewGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkIGTViewLogic::SafeDownCast(caller))
    {
    if (event == vtkIGTViewLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }

}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::ProcessTimerEvents()
{
 
  if(this->GetLogic()->GetCrosshairEnabled())
    {
      if(this->SliceNodeRed && this->SliceNodeYellow && this->SliceNodeGreen)
  {
    this->GetLogic()->Reslice(this->SliceNodeRed, this->SliceNodeYellow, this->SliceNodeGreen);
  }
    }

  if(this->RedObliqueReslicing)
    {
      this->GetLogic()->ObliqueOrientation(this->SliceNodeRed, this->RedReslicingType);
    }

  if(this->YellowObliqueReslicing)
    {
      this->GetLogic()->ObliqueOrientation(this->SliceNodeYellow, this->YellowReslicingType);
    }

  if(this->GreenObliqueReslicing)
    {
      this->GetLogic()->ObliqueOrientation(this->SliceNodeGreen, this->GreenReslicingType);
    }

 if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "IGTView", "IGTView", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForViewers();
  BuildGUIForOverlay();

}


void vtkIGTViewGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:IGTView</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "IGTView" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkIGTViewGUI::BuildGUIForViewers()
{

  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("IGTView");
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
 
  vtkSlicerModuleCollapsibleFrame *driversFrame = vtkSlicerModuleCollapsibleFrame::New();
  driversFrame->SetParent(page);
  driversFrame->Create();
  driversFrame->SetLabelText("Visualization");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               driversFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Viewer Frame
 
  this->Viewer3DMenu = vtkKWMenuButtonWithLabel::New();
  this->Viewer3DMenu->SetParent(driversFrame->GetFrame());
  this->Viewer3DMenu->Create();
  this->Viewer3DMenu->GetWidget()->SetBackgroundColor(color->White);
  this->Viewer3DMenu->GetWidget()->SetActiveBackgroundColor(color->White);
  this->Viewer3DMenu->GetWidget()->SetValue("3D View");
 
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->Viewer3DMenu->GetWidgetName());

  this->RedViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->RedViewerMenu->SetParent(driversFrame->GetFrame());
  this->RedViewerMenu->Create();
  this->RedViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIRed);
  this->RedViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->RedViewerMenu->GetWidget()->SetValue("Axial");

  this->YellowViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->YellowViewerMenu->SetParent(driversFrame->GetFrame());
  this->YellowViewerMenu->Create();
  this->YellowViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->YellowViewerMenu->GetWidget()->SetValue("Sagittal");

  this->GreenViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->GreenViewerMenu->SetParent(driversFrame->GetFrame());
  this->GreenViewerMenu->Create();
  this->GreenViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIGreen);
  this->GreenViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->GreenViewerMenu->GetWidget()->SetValue("Coronal");
 
 this->Script("pack %s %s %s -side left -anchor nw -fill x -expand y -padx 2 -pady 2",
         this->RedViewerMenu->GetWidgetName(),
         this->YellowViewerMenu->GetWidgetName(),         
         this->GreenViewerMenu->GetWidgetName());
 
 //  optionsFrame->Delete();
  driversFrame->Delete();
}

//---------------------------------------------------------------------------
void vtkIGTViewGUI::BuildGUIForOverlay()
{

  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("IGTView");

  vtkSlicerModuleCollapsibleFrame *overlayFrame = vtkSlicerModuleCollapsibleFrame::New();
  overlayFrame->SetParent(page);
  overlayFrame->Create();
  overlayFrame->SetLabelText("Overlay");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               overlayFrame->GetWidgetName(), page->GetWidgetName());

  // Crosshair

  this->crosshairButton = vtkKWCheckButton::New();
  this->crosshairButton->SetParent(overlayFrame->GetFrame());
  this->crosshairButton->Create();
  this->crosshairButton->SetText("Crosshair");
  this->crosshairButton->SelectedStateOff();

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2",
         this->crosshairButton->GetWidgetName());

  vtkKWFrame* locatorSourceFrame = vtkKWFrame::New();
  locatorSourceFrame->SetParent(overlayFrame->GetFrame());
  locatorSourceFrame->Create();

  this->Script("pack %s -side top -anchor nw -fill x -padx 25 -pady 2",
         locatorSourceFrame->GetWidgetName());

  vtkKWLabel* locatorSource = vtkKWLabel::New();
  locatorSource->SetParent(locatorSourceFrame);
  locatorSource->Create();
  locatorSource->SetText("Source: ");

  this->Script("pack %s -side left -anchor nw -padx 0 -pady 2",
         locatorSource->GetWidgetName());
  
  this->transformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->transformNodeSelector->SetParent(locatorSourceFrame);
  this->transformNodeSelector->Create();
  this->transformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode",NULL,NULL,NULL);
  this->transformNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->transformNodeSelector->SetNewNodeEnabled(1);
  this->transformNodeSelector->SetEnabled(0);

  this->Script("pack %s -side left -anchor nw -fill x -expand y -padx 0 -pady 2",
         this->transformNodeSelector->GetWidgetName());


  // Trajectory

  this->trajectoryButton = vtkKWCheckButton::New();
  this->trajectoryButton->SetParent(overlayFrame->GetFrame());
  this->trajectoryButton->Create();
  this->trajectoryButton->SetText("Trajectory");
  this->trajectoryButton->SelectedStateOff();

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2",
         this->trajectoryButton->GetWidgetName());


  vtkKWFrame* trajectorySourceFrame = vtkKWFrame::New();
  trajectorySourceFrame->SetParent(overlayFrame->GetFrame());
  trajectorySourceFrame->Create();

  this->Script("pack %s -side top -anchor nw -fill x -padx 25 -pady 2",
         trajectorySourceFrame->GetWidgetName());

  vtkKWLabel* trajectorySource = vtkKWLabel::New();
  trajectorySource->SetParent(trajectorySourceFrame);
  trajectorySource->Create();
  trajectorySource->SetText("Source: ");

  this->Script("pack %s -side left -anchor nw -padx 0 -pady 2",
         trajectorySource->GetWidgetName());
  
  // !!                                      !!                          
  // !   TODO: Create vtkMRMLTrajectoryNode   !
  // !!                                      !!

  this->trajectoryNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->trajectoryNodeSelector->SetParent(trajectorySourceFrame);
  this->trajectoryNodeSelector->Create();
  this->trajectoryNodeSelector->SetNodeClass("vtkMRMLTrajectoryNode",NULL,NULL,NULL);
  this->trajectoryNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->trajectoryNodeSelector->SetNewNodeEnabled(1);
  this->trajectoryNodeSelector->SetEnabled(0);

  this->Script("pack %s -side left -anchor nw -fill x -expand y -padx 0 -pady 2",
         this->trajectoryNodeSelector->GetWidgetName());


  // Delete

  locatorSourceFrame->Delete();
  locatorSource->Delete();
  trajectorySourceFrame->Delete();
  trajectorySource->Delete();
  overlayFrame->Delete();
}

//----------------------------------------------------------------------------
void vtkIGTViewGUI::UpdateAll()
{
}

void vtkIGTViewGUI::AddInplaneMenu()
{
  if(this->RedViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0") == -1)
    {
      this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane0");
      this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane90");
      this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Probe's Eye");
    }    
  
  if(this->YellowViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0") == -1)
    {
      this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane0");
      this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane90");
      this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Probe's Eye");
    }
  
  if(this->GreenViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0") == -1)
    {
      this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane0");
      this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("InPlane90");
      this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Probe's Eye");
    }

}

void vtkIGTViewGUI::RemoveInplaneMenu()
{
  int RedInplane0 = this->RedViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0");
  int RedInplane90 = this->RedViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane90");
  int RedPE = this->RedViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("Probe's Eye");

 if(RedInplane0 != -1)
    {
      this->RedViewerMenu->GetWidget()->GetMenu()->DeleteItem(RedInplane0);
      this->RedViewerMenu->GetWidget()->GetMenu()->DeleteItem(RedInplane90);
      this->RedViewerMenu->GetWidget()->GetMenu()->DeleteItem(RedPE);
    }    

  int YellowInplane0 = this->YellowViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0");
  int YellowInplane90 = this->YellowViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane90");
  int YellowPE = this->YellowViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("Probe's Eye");
  
  if(YellowInplane0 != -1)
    {
      this->YellowViewerMenu->GetWidget()->GetMenu()->DeleteItem(YellowInplane0);
      this->YellowViewerMenu->GetWidget()->GetMenu()->DeleteItem(YellowInplane90);
      this->YellowViewerMenu->GetWidget()->GetMenu()->DeleteItem(YellowPE);
    }

  int GreenInplane0 = this->GreenViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane0");
  int GreenInplane90 = this->GreenViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("InPlane90");
  int GreenPE = this->GreenViewerMenu->GetWidget()->GetMenu()->GetIndexOfItemWithSelectedValue("Probe's Eye");
  
  if(GreenInplane0 != -1)
    {
      this->GreenViewerMenu->GetWidget()->GetMenu()->DeleteItem(GreenInplane0);
      this->GreenViewerMenu->GetWidget()->GetMenu()->DeleteItem(GreenInplane90);
      this->GreenViewerMenu->GetWidget()->GetMenu()->DeleteItem(GreenPE);
    }
}
