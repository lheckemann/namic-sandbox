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

#include "vtkVirtualFixtureGUI.h"
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

#include "vtkKWMenuButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntry.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkVirtualFixtureGUI );
vtkCxxRevisionMacro ( vtkVirtualFixtureGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkVirtualFixtureGUI::vtkVirtualFixtureGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkVirtualFixtureGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->SphereMenu         = NULL;
  this->SphereNameEntry    = NULL;
  this->CenterXEntry       = NULL;
  this->CenterYEntry       = NULL;
  this->CenterZEntry       = NULL;
  this->RadiusEntry        = NULL;
  this->UpdateSphereButton = NULL;

  /*
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;
  */
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkVirtualFixtureGUI::~vtkVirtualFixtureGUI ( )
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
  if (this->TestButton11)
    {
    this->TestButton11->SetParent(NULL);
    this->TestButton11->Delete();
    }

  if (this->TestButton12)
    {
    this->TestButton12->SetParent(NULL);
    this->TestButton12->Delete();
    }

  if (this->TestButton21)
    {
    this->TestButton21->SetParent(NULL);
    this->TestButton21->Delete();
    }

  if (this->TestButton22)
    {
    this->TestButton22->SetParent(NULL);
    this->TestButton22->Delete();
    }
  */

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::Enter()
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
void vtkVirtualFixtureGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "VirtualFixtureGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  /*
  if (this->TestButton11)
    {
    this->TestButton11
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton12)
    {
    this->TestButton12
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton21)
    {
    this->TestButton21
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton22)
    {
    this->TestButton22
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  */


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  /*
  this->SphereMenu
  this->SphereNameEntry
  this->CneterXEntry
  this->CenterYEntry
  this->CenterZEntry
  this->RadiusEntry
  this->UpdateSphereButton
  */

  /*
  this->TestButton11
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton12
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton21
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  */

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkVirtualFixtureLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  /*
  if (this->TestButton11 == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton11 is pressed." << std::endl;
    }
  else if (this->TestButton12 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton12 is pressed." << std::endl;
    }
  else if (this->TestButton21 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton21 is pressed." << std::endl;
    }
  else if (this->TestButton22 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton22 is pressed." << std::endl;
    }
  */

} 


void vtkVirtualFixtureGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkVirtualFixtureGUI *self = reinterpret_cast<vtkVirtualFixtureGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkVirtualFixtureGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkVirtualFixtureLogic::SafeDownCast(caller))
    {
    if (event == vtkVirtualFixtureLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::ProcessTimerEvents()
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
void vtkVirtualFixtureGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "VirtualFixture", "VirtualFixture", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForSphereControl();
  //BuildGUIForTestFrame1();
  //BuildGUIForTestFrame2();

}


void vtkVirtualFixtureGUI::BuildGUIForHelpFrame ()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "VirtualFixture" );

  // Define your help text here.
  const char *help = 
    " Write your help text here.";

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------
  vtkSlicerModuleCollapsibleFrame *VirtualFixtureHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  VirtualFixtureHelpFrame->SetParent ( page );
  VirtualFixtureHelpFrame->Create ( );
  VirtualFixtureHelpFrame->CollapseFrame ( );
  VirtualFixtureHelpFrame->SetLabelText ("Help");
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                VirtualFixtureHelpFrame->GetWidgetName(), page->GetWidgetName());
    
  // configure the parent classes help text widget
  this->HelpText->SetParent ( VirtualFixtureHelpFrame->GetFrame() );
  this->HelpText->Create ( );
  this->HelpText->SetHorizontalScrollbarVisibility ( 0 );
  this->HelpText->SetVerticalScrollbarVisibility ( 1 );
  this->HelpText->GetWidget()->SetText ( help );
  this->HelpText->GetWidget()->SetReliefToFlat ( );
  this->HelpText->GetWidget()->SetWrapToWord ( );
  this->HelpText->GetWidget()->ReadOnlyOn ( );
  this->HelpText->GetWidget()->QuickFormattingOn ( );
  this->HelpText->GetWidget()->SetBalloonHelpString ( "" );
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
                this->HelpText->GetWidgetName ( ) );

  VirtualFixtureHelpFrame->Delete();

}

//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::BuildGUIForSphereControl()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("VirtualFixture");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Sphere Control");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Sphere menu

  vtkKWFrame *sphereMenuFrame = vtkKWFrame::New();
  sphereMenuFrame->SetParent(conBrowsFrame->GetFrame());
  sphereMenuFrame->Create();
  this->Script ("pack %s -fill both -expand true",  
                 sphereMenuFrame->GetWidgetName() );

  vtkKWLabel *sphereMenuLabel = vtkKWLabel::New();
  sphereMenuLabel->SetParent(sphereMenuFrame);
  sphereMenuLabel->Create();
  sphereMenuLabel->SetWidth(14);
  sphereMenuLabel->SetText("SphereName: ");

  this->SphereMenu = vtkKWMenuButton::New();
  this->SphereMenu->SetParent(sphereMenuFrame);
  this->SphereMenu->Create();
  this->SphereMenu->SetWidth(10);
  this->SphereMenu->GetMenu()->AddRadioButton ("Sphere 1");
  this->SphereMenu->GetMenu()->AddRadioButton ("Sphere 2");
  this->SphereMenu->GetMenu()->AddRadioButton ("New Sphere");
  this->SphereMenu->SetValue ("Sphere 1");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              sphereMenuLabel->GetWidgetName() , this->SphereMenu->GetWidgetName());

  // -----------------------------------------
  // Sphere name
  vtkKWFrame *sphereNameFrame = vtkKWFrame::New();
  sphereNameFrame->SetParent(conBrowsFrame->GetFrame());
  sphereNameFrame->Create();
  this->Script ("pack %s -fill both -expand true",  
                sphereNameFrame->GetWidgetName() );

  vtkKWLabel *sphereNameLabel = vtkKWLabel::New();
  sphereNameLabel->SetParent(sphereNameFrame);
  sphereNameLabel->Create();
  sphereNameLabel->SetWidth(14);
  sphereNameLabel->SetText("SphereName: ");

  this->SphereNameEntry = vtkKWEntry::New();
  this->SphereNameEntry->SetParent(sphereNameFrame);
  this->SphereNameEntry->Create();
  this->SphereNameEntry->SetWidth(10);
  this->SphereNameEntry->SetValue ("Sphere 1");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               sphereNameLabel->GetWidgetName() , this->SphereNameEntry->GetWidgetName());

  // -----------------------------------------
  // Center coordinate

  vtkKWFrame *centerCoordinateFrame = vtkKWFrame::New();
  centerCoordinateFrame->SetParent(conBrowsFrame->GetFrame());
  centerCoordinateFrame->Create();
  this->Script ("pack %s -fill both -expand true",  
                centerCoordinateFrame->GetWidgetName() );

  vtkKWLabel *centerCoordinateLabel = vtkKWLabel::New();
  centerCoordinateLabel->SetParent(centerCoordinateFrame);
  centerCoordinateLabel->Create();
  centerCoordinateLabel->SetWidth(14);
  centerCoordinateLabel->SetText("Center: ");

  this->CenterXEntry = vtkKWEntry::New();
  this->CenterXEntry->SetParent(centerCoordinateFrame);
  this->CenterXEntry->Create();
  this->CenterXEntry->SetWidth(8);
  this->CenterXEntry->SetRestrictValueToDouble();
  this->CenterXEntry->SetValue ("0.0");

  this->CenterYEntry = vtkKWEntry::New();
  this->CenterYEntry->SetParent(centerCoordinateFrame);
  this->CenterYEntry->Create();
  this->CenterYEntry->SetWidth(8);
  this->CenterYEntry->SetRestrictValueToDouble();
  this->CenterYEntry->SetValue ("0.0");

  this->CenterZEntry = vtkKWEntry::New();
  this->CenterZEntry->SetParent(centerCoordinateFrame);
  this->CenterZEntry->Create();
  this->CenterZEntry->SetWidth(8);
  this->CenterZEntry->SetRestrictValueToDouble();
  this->CenterZEntry->SetValue ("0.0");

  this->Script("pack %s %s %s %s -side left -anchor w -fill x -padx 4 -pady 2", 
               centerCoordinateLabel->GetWidgetName(),
               this->CenterXEntry->GetWidgetName(),
               this->CenterYEntry->GetWidgetName(),
               this->CenterZEntry->GetWidgetName());

  // -----------------------------------------
  // Radius
  vtkKWFrame *radiusFrame = vtkKWFrame::New();
  radiusFrame->SetParent(conBrowsFrame->GetFrame());
  radiusFrame->Create();
  this->Script ("pack %s -fill both -expand true",  
                radiusFrame->GetWidgetName() );

  vtkKWLabel *radiusLabel = vtkKWLabel::New();
  radiusLabel->SetParent(radiusFrame);
  radiusLabel->Create();
  radiusLabel->SetWidth(14);
  radiusLabel->SetText("SphereName: ");

  this->RadiusEntry = vtkKWEntry::New();
  this->RadiusEntry->SetParent(radiusFrame);
  this->RadiusEntry->Create();
  this->RadiusEntry->SetWidth(10);
  this->RadiusEntry->SetRestrictValueToDouble();
  this->RadiusEntry->SetValue ("Sphere 1");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               radiusLabel->GetWidgetName() , this->RadiusEntry->GetWidgetName());

  sphereMenuFrame->Delete();
  sphereMenuLabel->Delete();
  sphereNameFrame->Delete();
  sphereNameLabel->Delete();
  radiusFrame->Delete();
  radiusLabel->Delete();
  centerCoordinateFrame->Delete();
  centerCoordinateLabel->Delete();

  conBrowsFrame->Delete();

  // -----------------------------------------
  // Sphere name entry
  
  /*
  this->CneterXEntry       = NULL;
  this->CenterYEntry       = NULL;
  this->CenterZEntry       = NULL;
  this->RadiusEntry        = NULL;
  this->UpdateSphereButton = NULL;
  */

  
}


//----------------------------------------------------------------------------
void vtkVirtualFixtureGUI::UpdateAll()
{
}

