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

  this->linkerCheck = NULL;
  this->DefaultViewButton = NULL;

  //----------------------------------------------------------------
  // Green Frame

  this->GreenViewerMenu = NULL;

  //----------------------------------------------------------------
  // Yellow Frame

  this->YellowViewerMenu = NULL;

  //----------------------------------------------------------------
  // Red Frame

  this->RedViewerMenu = NULL;

  //----------------------------------------------------------------
  // 3D Frame

  this->Viewer3DMenu = NULL;

  //----------------------------------------------------------------
  // Slice

  this->SliceNodeRed = NULL;
  this->SliceNodeYellow = NULL;
  this->SliceNodeRed = NULL;

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

  //----------------------------------------------------------------
  // Green Frame
  
  if(this->GreenViewerMenu)
    {
      this->GreenViewerMenu->SetParent(NULL);
      this->GreenViewerMenu->Delete();
    }

  //----------------------------------------------------------------
  // Yellow Frame

  if(this->YellowViewerMenu)
    {
      this->YellowViewerMenu->SetParent(NULL);
      this->YellowViewerMenu->Delete();
    }

  //----------------------------------------------------------------
  // Red Frame

  if(this->RedViewerMenu)
    {
      this->RedViewerMenu->SetParent(NULL);
      this->RedViewerMenu->Delete();
    }

  //----------------------------------------------------------------
  // 3D Frame

  if(this->Viewer3DMenu)
    {
      this->Viewer3DMenu->SetParent(NULL);
      this->Viewer3DMenu->Delete();
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

  if(this->linkerCheck)
    {
      this->linkerCheck->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->DefaultViewButton)
    {
      this->DefaultViewButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Green Frame
  
  if(this->GreenViewerMenu)
    {
      this->GreenViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Yellow Frame

  if(this->YellowViewerMenu)
    {
      this->YellowViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Red Frame

  if(this->RedViewerMenu)
    {
      this->RedViewerMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // 3D Frame

  if(this->Viewer3DMenu)
    {
      this->Viewer3DMenu->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
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

  this->linkerCheck->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->DefaultViewButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Green Frame

  this->GreenViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Yellow Frame

  this->YellowViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Red Frame

  this->RedViewerMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // 3D Frame

  this->Viewer3DMenu->GetWidget()->GetMenu()->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);


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

  else if(this->linkerCheck == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
      // Link 2D viewers
      if(this->linkerCheck->GetSelectedState())
  {
    this->GetLogic()->SetLink2DViewers(true);
    // Update slice if one is "locator"
    if(this->RedViewerMenu && this->YellowViewerMenu && this->RedViewerMenu)
      {
        const char* rSelection = this->RedViewerMenu->GetWidget()->GetValue();
        const char* ySelection = this->YellowViewerMenu->GetWidget()->GetValue();
        const char* gSelection = this->GreenViewerMenu->GetWidget()->GetValue();

        if(!strcmp(rSelection, "Locator") || !strcmp(ySelection, "Locator") || !strcmp(gSelection, "Locator"))
    {
      // TODO: All Locator
      std::cerr << "INSIDE: All Locators (Linker)" << std::endl;
    }
      }
  }
      else
  {
    this->GetLogic()->SetLink2DViewers(false);
  }
    }
  else if(this->DefaultViewButton == vtkKWPushButton::SafeDownCast(caller)
    && event == vtkKWPushButton::InvokedEvent)
    {
      // Set values to default
      if(this->SliceNodeRed && this->SliceNodeYellow && this->SliceNodeGreen)
  {
    if(this->RedViewerMenu->GetWidget())
      {
        this->RedViewerMenu->GetWidget()->SetValue("Axial");
        this->SliceNodeRed->SetOrientationToAxial();
      }
    
    if(this->YellowViewerMenu->GetWidget())
      {
        this->YellowViewerMenu->GetWidget()->SetValue("Sagittal");
        this->SliceNodeYellow->SetOrientationToSagittal();
      }
    
    if(this->GreenViewerMenu->GetWidget())
      {
        this->GreenViewerMenu->GetWidget()->SetValue("Coronal");
        this->SliceNodeGreen->SetOrientationToCoronal();
      }
  }

      if(this->Viewer3DMenu->GetWidget())
  {
    this->Viewer3DMenu->GetWidget()->SetValue("Not Available yet");
  }


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
        this->SliceNodeRed->SetOrientationToAxial();
      }
    else if(!strcmp(redSelection, "Sagittal"))
      {
        this->SliceNodeRed->SetOrientationToSagittal();
      }
    else if(!strcmp(redSelection, "Coronal"))
      {
        this->SliceNodeRed->SetOrientationToCoronal();
      }
    else if(!strcmp(redSelection, "Locator"))
      {
        if(this->GetLogic()->GetLink2DViewers())
    {
      // Set all viewers to locator
      std::cerr << "INSIDE: All Locators" << std::endl;
    }
        else
    {
      // Just Red slice to locator
      std::cerr << "INSIDE: Red Locator" << std::endl;
    }
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
        this->SliceNodeYellow->SetOrientationToAxial();
      }
    else if(!strcmp(yellowSelection, "Sagittal"))
      {
        this->SliceNodeYellow->SetOrientationToSagittal();
      }
    else if(!strcmp(yellowSelection, "Coronal"))
      {
        this->SliceNodeYellow->SetOrientationToCoronal();
      }
    else if(!strcmp(yellowSelection, "Locator"))
      {
        if(this->GetLogic()->GetLink2DViewers())
    {
      // Set all viewers to locator
      std::cerr << "INSIDE: All Locators" << std::endl;
    }
        else
    {
      // Just Yellow slice to locator
      std::cerr << "INSIDE: Yellow Locator" << std::endl;
    }
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
        this->SliceNodeGreen->SetOrientationToAxial();
      }
    else if(!strcmp(greenSelection, "Sagittal"))
      {
        this->SliceNodeGreen->SetOrientationToSagittal();
      }
    else if(!strcmp(greenSelection, "Coronal"))
      {
        this->SliceNodeGreen->SetOrientationToCoronal();
      }
    else if(!strcmp(greenSelection, "Locator"))
      {
        if(this->GetLogic()->GetLink2DViewers())
    {
      // Set all viewers to locator
      std::cerr << "INSIDE: All Locators" << std::endl;
    }
        else
    {
      // Just Green slice to locator
      std::cerr << "INSIDE: Green Locator" << std::endl;
    }
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

  vtkSlicerModuleCollapsibleFrame *optionsFrame = vtkSlicerModuleCollapsibleFrame::New();
  optionsFrame->SetParent(page);
  optionsFrame->Create();
  optionsFrame->SetLabelText("Options");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               optionsFrame->GetWidgetName(), page->GetWidgetName());

  this->linkerCheck = vtkKWCheckButton::New();
  this->linkerCheck->SetParent(optionsFrame->GetFrame());
  this->linkerCheck->Create();
  this->linkerCheck->SetText("Link 2D Viewers");
  this->linkerCheck->SelectedStateOff();

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2",
         this->linkerCheck->GetWidgetName());

  this->DefaultViewButton = vtkKWPushButton::New();
  this->DefaultViewButton->SetParent(optionsFrame->GetFrame());
  this->DefaultViewButton->Create();
  this->DefaultViewButton->SetText("Default All Views");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->DefaultViewButton->GetWidgetName()); 

  vtkSlicerModuleCollapsibleFrame *driversFrame = vtkSlicerModuleCollapsibleFrame::New();
  driversFrame->SetParent(page);
  driversFrame->Create();
  driversFrame->SetLabelText("Visualization");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               driversFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Viewer Frame
 
  vtkKWFrameWithLabel *frameRed = vtkKWFrameWithLabel::New();
  frameRed->SetParent(driversFrame->GetFrame());
  frameRed->Create();
  frameRed->SetLabelText ("Red");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frameRed->GetWidgetName() );

  this->RedViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->RedViewerMenu->SetParent(frameRed->GetFrame());
  this->RedViewerMenu->Create();
  this->RedViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIRed);
  this->RedViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->RedViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Locator");
  this->RedViewerMenu->GetWidget()->SetValue("Axial");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->RedViewerMenu->GetWidgetName());


  vtkKWFrameWithLabel *frameYellow = vtkKWFrameWithLabel::New();
  frameYellow->SetParent(driversFrame->GetFrame());
  frameYellow->Create();
  frameYellow->SetLabelText ("Yellow");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frameYellow->GetWidgetName() );

  this->YellowViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->YellowViewerMenu->SetParent(frameYellow->GetFrame());
  this->YellowViewerMenu->Create();
  this->YellowViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->YellowViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Locator");
  this->YellowViewerMenu->GetWidget()->SetValue("Sagittal");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->YellowViewerMenu->GetWidgetName());
 

  vtkKWFrameWithLabel *frameGreen = vtkKWFrameWithLabel::New();
  frameGreen->SetParent(driversFrame->GetFrame());
  frameGreen->Create();
  frameGreen->SetLabelText ("Green");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frameGreen->GetWidgetName() );

  this->GreenViewerMenu = vtkKWMenuButtonWithLabel::New();
  this->GreenViewerMenu->SetParent(frameGreen->GetFrame());
  this->GreenViewerMenu->Create();
  this->GreenViewerMenu->GetWidget()->SetBackgroundColor(color->SliceGUIGreen);
  this->GreenViewerMenu->GetWidget()->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Axial");
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Sagittal");
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Coronal");
  this->GreenViewerMenu->GetWidget()->GetMenu()->AddRadioButton("Locator");
  this->GreenViewerMenu->GetWidget()->SetValue("Coronal");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->GreenViewerMenu->GetWidgetName());
 
 
  vtkKWFrameWithLabel *frame3D = vtkKWFrameWithLabel::New();
  frame3D->SetParent(driversFrame->GetFrame());
  frame3D->Create();
  frame3D->SetLabelText ("3D");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame3D->GetWidgetName() );

  this->Viewer3DMenu = vtkKWMenuButtonWithLabel::New();
  this->Viewer3DMenu->SetParent(frame3D->GetFrame());
  this->Viewer3DMenu->Create();
  this->Viewer3DMenu->GetWidget()->SetBackgroundColor(color->White);
  this->Viewer3DMenu->GetWidget()->SetActiveBackgroundColor(color->White);
  this->Viewer3DMenu->GetWidget()->SetValue("Not Available yet");

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
         this->Viewer3DMenu->GetWidgetName());


  optionsFrame->Delete();
  driversFrame->Delete();
  frame3D->Delete();
  frameGreen->Delete();
  frameYellow->Delete();
  frameRed->Delete();
  
}


//----------------------------------------------------------------------------
void vtkIGTViewGUI::UpdateAll()
{
}

