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

#include "vtkNeuroNavGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"

#include "vtkKWRenderWidget.h"
#include "vtkKWWidget.h"
#include "vtkKWMenuButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMenu.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWEvent.h"
#include "vtkKWOptions.h"

#include "vtkKWTkUtilities.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCornerAnnotation.h"

// for Realtime Image
#include "vtkImageChangeInformation.h"
#include "vtkSlicerColorLogic.h"

#include "vtkMRMLLinearTransformNode.h"

#include <vector>
#include <sstream>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkNeuroNavGUI );
vtkCxxRevisionMacro ( vtkNeuroNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkNeuroNavGUI::vtkNeuroNavGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  
  this->Logic = NULL;
  this->DataManager = vtkIGTDataManager::New();
  this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();
  
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkNeuroNavGUI::DataCallback);
  
  this->SliceNode0 = NULL; 
  this->SliceNode1 = NULL; 
  this->SliceNode2 = NULL; 
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  //----------------------------------------------------------------
  // Connector List Frame

  this->ConnectorList = NULL;
  this->AddConnectorButton = NULL;
  this->DeleteConnectorButton = NULL;
  this->ConnectorNameEntry = NULL;
  this->ConnectorTypeButtonSet = NULL;
  this->ConnectorStatusCheckButton = NULL;
  this->ConnectorAddressEntry = NULL;
  this->ConnectorPortEntry = NULL;

  //----------------------------------------------------------------
  // Visualization Control Frame
  
  this->FreezeImageCheckButton = NULL;
  this->ObliqueCheckButton     = NULL;
  this->SetLocatorModeButton   = NULL;
  this->SetUserModeButton      = NULL;
  this->RedSliceMenu           = NULL;
  this->YellowSliceMenu        = NULL;
  this->GreenSliceMenu         = NULL;

  this->StartScanButton        = NULL;
  this->StopScanButton         = NULL;

  this->FreezeImageCheckButton = NULL;
  this->LocatorCheckButton     = NULL;

  this->ImagingMenu            = NULL;


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->CloseScene             = false;

  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkNeuroNavGUI::~vtkNeuroNavGUI ( )
{

  if (this->DataManager)
    {
    // If we don't set the scence to NULL for DataManager,
    // Slicer will report a lot leak when it is closed.
    this->DataManager->SetMRMLScene(NULL);
    this->DataManager->Delete();
    }
  if (this->Pat2ImgReg)
    {
    this->Pat2ImgReg->Delete();
    }
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  this->RemoveGUIObservers();


  //----------------------------------------------------------------
  // Workphase Frame

  this->SetModuleLogic ( NULL );


  //----------------------------------------------------------------
  // Visualization Control Frame

  if (this->FreezeImageCheckButton)
    {
    this->FreezeImageCheckButton->SetParent(NULL );
    this->FreezeImageCheckButton->Delete ( );
    }
  if (this->ObliqueCheckButton)
    {
    this->ObliqueCheckButton->SetParent(NULL );
    this->ObliqueCheckButton->Delete ( );
    }


  if (this->SetLocatorModeButton)
    {
    this->SetLocatorModeButton->SetParent(NULL);
    this->SetLocatorModeButton->Delete();
    }
  
  if (this->SetUserModeButton)
    {
    this->SetUserModeButton->SetParent(NULL);
    this->SetUserModeButton->Delete();
    }

  if (this->RedSliceMenu)
    {
    this->RedSliceMenu->SetParent(NULL );
    this->RedSliceMenu->Delete ( );
    }

  if (this->YellowSliceMenu)
    {
    this->YellowSliceMenu->SetParent(NULL );
    this->YellowSliceMenu->Delete ( );
    }

  if (this->GreenSliceMenu)
    {
    this->GreenSliceMenu->SetParent(NULL );
    this->GreenSliceMenu->Delete ( );
    }

  /*
  if (this->StartScanButton)
    {
    this->StartScanButton->SetParent(NULL);
    this->StartScanButton->Delete();
    }

  if (this->StopScanButton)
    {
    this->StopScanButton->SetParent(NULL);
    this->StopScanButton->Delete();
    }

  if ( this->ImagingMenu )
    {
    this->ImagingMenu->SetParent(NULL);
    this->ImagingMenu->Delete();
    }
  */

  this->IsSliceOrientationAdded = false;


  //----------------------------------------------------------------
  // Etc Frame


  if (this->LocatorCheckButton)
    {
    this->LocatorCheckButton->SetParent(NULL );
    this->LocatorCheckButton->Delete ( );
    }

}



//---------------------------------------------------------------------------
void vtkNeuroNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "NeuroNavGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
   
  // print widgets?
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::RemoveGUIObservers ( )
{
  /*
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Connector Browser Frame

  if (this->ConnectorList->GetWidget())
    {
    this->ConnectorList->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->AddConnectorButton)
    {
    this->AddConnectorButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->DeleteConnectorButton)
    {
    this->DeleteConnectorButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  if (this->ConnectorNameEntry)
    {
    this->ConnectorNameEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
      
  if (this->ConnectorTypeButtonSet)
    {
    this->ConnectorTypeButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ConnectorTypeButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->ConnectorStatusCheckButton)
    {
    this->ConnectorStatusCheckButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }

  if (this->ConnectorAddressEntry)
    {
    this->ConnectorAddressEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ConnectorPortEntry)
    {
    this->ConnectorPortEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  
  //----------------------------------------------------------------
  // Visualization Control Frame

  if (this->FreezeImageCheckButton)
    {
    this->FreezeImageCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }
  if (this->ObliqueCheckButton)
    {
    this->ObliqueCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }


  if (this->SetLocatorModeButton)
    {
    this->SetLocatorModeButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }

  if (this->SetUserModeButton)
    {
    this->SetUserModeButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }

  if (this->LocatorCheckButton)
    {
    this->LocatorCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand );
    }

  if (this->RedSliceMenu)
    {
    this->RedSliceMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->YellowSliceMenu)
    {
    this->YellowSliceMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->GreenSliceMenu)
    {
    this->GreenSliceMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImagingMenu)
    {
    this->ImagingMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  this->RemoveLogicObservers();
  */
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::RemoveLogicObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::AddGUIObservers ( )
{
  /*
  this->RemoveGUIObservers();

  // make a user interactor style to process our events
  // look at the InteractorStyle to get our events

  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

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
  // Main Slice GUI

  appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  

  //----------------------------------------------------------------
  // Connector Browser Frame

  //vtkKWMultiColumnListWithScrollbars* ConnectorList;
  
  this->ConnectorList->GetWidget()
    ->AddObserver(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->AddConnectorButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DeleteConnectorButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ConnectorNameEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ConnectorTypeButtonSet->GetWidget(0)
    ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ConnectorTypeButtonSet->GetWidget(1)
    ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ConnectorStatusCheckButton
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ConnectorAddressEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ConnectorPortEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Visualization Control Frame


  this->LocatorCheckButton
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);


  this->RedSliceMenu->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->YellowSliceMenu->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->GreenSliceMenu->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->FreezeImageCheckButton
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ObliqueCheckButton
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);

  this->SetLocatorModeButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->SetUserModeButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->StartScanButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StopScanButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ImagingMenu->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);



  //----------------------------------------------------------------
  // Etc Frame

  // observer load volume button

  this->AddLogicObservers();
 */ 
  
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    /*
      this->GetLogic()->AddObserver(vtkNeuroNavLogic::LocatorUpdateEvent,
      (vtkCommand *)this->LogicCallbackCommand);
    */
    /*
      this->GetLogic()->AddObserver(vtkNeuroNavLogic::SliceUpdateEvent,
      (vtkCommand *)this->LogicCallbackCommand);
    */
    this->GetLogic()->AddObserver(vtkNeuroNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkNeuroNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{

  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  vtkSlicerInteractorStyle *istyle0 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI0()->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle1 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI1()->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI2()->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  vtkCornerAnnotation *anno = NULL;
  if (style == istyle0)
    {
    anno = appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle1)
    {
    anno = appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle2)
    {
    anno = appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  if (anno)
    {
    const char *rasText = anno->GetText(1);
    if ( rasText != NULL )
      {
      std::string ras = std::string(rasText);
        
      // remove "R:," "A:," and "S:" from the string
      int loc = ras.find("R:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      loc = ras.find("A:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      loc = ras.find("S:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      
      // remove "\n" from the string
      int found = ras.find("\n", 0);
      while ( found != std::string::npos )
        {
        ras = ras.replace(found, 1, " ");
        found = ras.find("\n", 0);
        }
      
      }
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{
      

} 


void vtkNeuroNavGUI::Init()
{
  this->DataManager->SetMRMLScene(this->GetMRMLScene());
}



void vtkNeuroNavGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkNeuroNavGUI *self = reinterpret_cast<vtkNeuroNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkNeuroNavGUI DataCallback");

  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkNeuroNavLogic::SafeDownCast(caller))
    {
    if (event == vtkNeuroNavLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    if (this->LocatorCheckButton != NULL && this->LocatorCheckButton->GetSelectedState())
      {
      this->CloseScene = true;
      this->LocatorCheckButton->SelectedStateOff();
      }
    }
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::ProcessTimerEvents()
{

}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::Enter()
{
  // Fill in
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  this->SliceNode0 = appGUI->GetMainSliceGUI0()->GetLogic()->GetSliceNode();
  this->SliceNode1 = appGUI->GetMainSliceGUI1()->GetLogic()->GetSliceNode();
  this->SliceNode2 = appGUI->GetMainSliceGUI2()->GetLogic()->GetSliceNode();
  
  //this->GetLogic()->AddRealtimeVolumeNode("Realtime");
  //this->Logic0->GetForegroundLayer()->SetUseReslice(0);
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }


  /*
  //----------------------------------------------------------------
  // Hack SlicerSlicesControlGUI -- Add "InPlane", "InPlane90", "Perp"

  if (this->IsSliceOrientationAdded == false && this->GetApplication() )
    {
    vtkSlicerSliceGUI* sgui;
    vtkSlicerSlicesGUI* ssgui;
    vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast (this->GetApplication());
    ssgui = vtkSlicerSlicesGUI::SafeDownCast(app->GetModuleGUIByName ("Slices") );

    if (ssgui != NULL)
      {
      ssgui->GetSliceGUICollection()->InitTraversal();
      sgui = vtkSlicerSliceGUI::SafeDownCast(ssgui->GetSliceGUICollection()->GetNextItemAsObject());
      while ( sgui != NULL )
        {
        vtkSlicerSliceControllerWidget* sscw = sgui->GetSliceController();
        vtkKWMenuButtonWithSpinButtonsWithLabel* oriSel = sscw->GetOrientationSelector();
        vtkKWMenuButton *mb = oriSel->GetWidget()->GetWidget();
        mb->GetMenu()->AddRadioButton("InPlane");
        mb->GetMenu()->AddRadioButton("InPlane90");
        mb->GetMenu()->AddRadioButton("Perp");
        sgui = vtkSlicerSliceGUI::SafeDownCast ( ssgui->GetSliceGUICollection()->GetNextItemAsObject() );
        }
      }
    this->IsSliceOrientationAdded = true;
    }
  */

}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "NeuroNav", "NeuroNav", NULL );

  // BuildGUIForHelpFrame();
  // BuildGUIForConnectorBrowserFrame();
  // BuildGUIForVisualizationControlFrame();

  // UpdateConnectorPropertyFrame(-1);
}


void vtkNeuroNavGUI::BuildGUIForHelpFrame ()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "NeuroNav" );

  // Define your help text here.
  const char *help = 
    "The **OpenIGTLink Module** helps you to manage OpenIGTLink connections:"
    " OpenIGTLink is an open network protocol for communication among software / hardware "
    " for image-guided therapy, e.g. robot-navigation and imager-viewer connections."
    " The information of the OpenIGTLink protocol can be found at http://wiki.na-mic.org/Wiki/index.php/OpenIGTLink ."
    " The module is designed and implemented by Junichi Tokuda for Brigham and Women's Hospital."
    " This work is supported by NCIGT, NA-MIC and BRP Prostate robot project.";

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------
  vtkSlicerModuleCollapsibleFrame *OpenIGTLinkHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  OpenIGTLinkHelpFrame->SetParent ( page );
  OpenIGTLinkHelpFrame->Create ( );
  OpenIGTLinkHelpFrame->CollapseFrame ( );
  OpenIGTLinkHelpFrame->SetLabelText ("Help");
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                OpenIGTLinkHelpFrame->GetWidgetName(), page->GetWidgetName());
    
  // configure the parent classes help text widget
  this->HelpText->SetParent ( OpenIGTLinkHelpFrame->GetFrame() );
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

  OpenIGTLinkHelpFrame->Delete();

}

//---------------------------------------------------------------------------
void vtkNeuroNavGUI::BuildGUIForConnectorBrowserFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("OpenIGTLink");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Connector Browser");
  conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  vtkKWFrameWithLabel *listFrame = vtkKWFrameWithLabel::New();
  listFrame->SetParent(conBrowsFrame->GetFrame());
  listFrame->Create();
  listFrame->SetLabelText ("Connectors");
  app->Script ( "pack %s -fill both -expand true",  
                listFrame->GetWidgetName());

  // Connector List Frame
  
  this->ConnectorList = vtkKWMultiColumnListWithScrollbars::New();
  this->ConnectorList->SetParent(listFrame->GetFrame());
  this->ConnectorList->Create();
  this->ConnectorList->SetHeight(1);
  this->ConnectorList->GetWidget()->SetSelectionTypeToRow();
  this->ConnectorList->GetWidget()->SetSelectionModeToSingle();
  this->ConnectorList->GetWidget()->MovableRowsOff();
  this->ConnectorList->GetWidget()->MovableColumnsOff();

  const char* labels[] =
    { "Name", "Type", "Status", "Destination"};
  const int widths[] = 
    { 10, 5, 7, 20 };

  for (int col = 0; col < 4; col ++)
    {
    this->ConnectorList->GetWidget()->AddColumn(labels[col]);
    this->ConnectorList->GetWidget()->SetColumnWidth(col, widths[col]);
    this->ConnectorList->GetWidget()->SetColumnAlignmentToLeft(col);
    this->ConnectorList->GetWidget()->ColumnEditableOff(col);
    //this->ConnectorList->GetWidget()->ColumnEditableOn(col);
    this->ConnectorList->GetWidget()->SetColumnEditWindowToSpinBox(col);
    }
  this->ConnectorList->GetWidget()->SetColumnEditWindowToCheckButton(0);
  //this->ConnectorList->GetWidget()->SetCellUpdatedCommand(this, "OnConnectorListUpdate");
  //this->ConnectorList->GetWidget()->SetSelectionChangedCommand(this, "OnConnectorListSelectionChanged");
  
  vtkKWFrame *listButtonsFrame = vtkKWFrame::New();
  listButtonsFrame->SetParent(listFrame->GetFrame());
  listButtonsFrame->Create();

  app->Script ("pack %s %s -fill both -expand true",  
               //app->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->ConnectorList->GetWidgetName(), listButtonsFrame->GetWidgetName());

  this->AddConnectorButton = vtkKWPushButton::New();
  this->AddConnectorButton->SetParent(listButtonsFrame);
  this->AddConnectorButton->Create();
  this->AddConnectorButton->SetText( "Add" );
  this->AddConnectorButton->SetWidth (6);

  this->DeleteConnectorButton = vtkKWPushButton::New();
  this->DeleteConnectorButton->SetParent(listButtonsFrame);
  this->DeleteConnectorButton->Create();
  this->DeleteConnectorButton->SetText( "Delete" );
  this->DeleteConnectorButton->SetWidth (6);

  app->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->AddConnectorButton->GetWidgetName(), this->DeleteConnectorButton->GetWidgetName());

  // -----------------------------------------
  // Connector Property frame

  vtkKWFrameWithLabel *controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(conBrowsFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText ("Connector Property");
  app->Script ( "pack %s -fill both -expand true",  
                controlFrame->GetWidgetName());


  // Connector Property -- Connector name
  vtkKWFrame *nameFrame = vtkKWFrame::New();
  nameFrame->SetParent(controlFrame->GetFrame());
  nameFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                nameFrame->GetWidgetName());

  vtkKWLabel *nameLabel = vtkKWLabel::New();
  nameLabel->SetParent(nameFrame);
  nameLabel->Create();
  nameLabel->SetWidth(8);
  nameLabel->SetText("Name: ");

  this->ConnectorNameEntry = vtkKWEntry::New();
  this->ConnectorNameEntry->SetParent(nameFrame);
  this->ConnectorNameEntry->Create();
  this->ConnectorNameEntry->SetWidth(18);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              nameLabel->GetWidgetName() , this->ConnectorNameEntry->GetWidgetName());
  

  // Connector Property -- Connector type (server or client)
  vtkKWFrame *typeFrame = vtkKWFrame::New();
  typeFrame->SetParent(controlFrame->GetFrame());
  typeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                typeFrame->GetWidgetName());

  vtkKWLabel *typeLabel = vtkKWLabel::New();
  typeLabel->SetParent(typeFrame);
  typeLabel->Create();
  typeLabel->SetWidth(8);
  typeLabel->SetText("Type: ");

  this->ConnectorTypeButtonSet = vtkKWRadioButtonSet::New();
  this->ConnectorTypeButtonSet->SetParent(typeFrame);
  this->ConnectorTypeButtonSet->Create();
  this->ConnectorTypeButtonSet->PackHorizontallyOn();
  this->ConnectorTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ConnectorTypeButtonSet->UniformColumnsOn();
  this->ConnectorTypeButtonSet->UniformRowsOn();

  this->ConnectorTypeButtonSet->AddWidget(0);
  this->ConnectorTypeButtonSet->GetWidget(0)->SetText("Server");
  this->ConnectorTypeButtonSet->AddWidget(1);
  this->ConnectorTypeButtonSet->GetWidget(1)->SetText("Client");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              typeLabel->GetWidgetName() , this->ConnectorTypeButtonSet->GetWidgetName());

  // Connector Property -- Connector type (server or client)
  vtkKWFrame *statusFrame = vtkKWFrame::New();
  statusFrame->SetParent(controlFrame->GetFrame());
  statusFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                statusFrame->GetWidgetName());
  
  vtkKWLabel *statusLabel = vtkKWLabel::New();
  statusLabel->SetParent(statusFrame);
  statusLabel->Create();
  statusLabel->SetWidth(8);
  statusLabel->SetText("Status: ");

  this->ConnectorStatusCheckButton = vtkKWCheckButton::New();
  this->ConnectorStatusCheckButton->SetParent(statusFrame);
  this->ConnectorStatusCheckButton->Create();
  this->ConnectorStatusCheckButton->SelectedStateOff();
  this->ConnectorStatusCheckButton->SetText("Active");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              statusLabel->GetWidgetName() , this->ConnectorStatusCheckButton->GetWidgetName());

  vtkKWFrame *addressFrame = vtkKWFrame::New();
  addressFrame->SetParent(controlFrame->GetFrame());
  addressFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                addressFrame->GetWidgetName());

  vtkKWLabel *addressLabel = vtkKWLabel::New();
  addressLabel->SetParent(addressFrame);
  addressLabel->Create();
  addressLabel->SetWidth(8);
  addressLabel->SetText("Addr: ");

  this->ConnectorAddressEntry = vtkKWEntry::New();
  this->ConnectorAddressEntry->SetParent(addressFrame);
  this->ConnectorAddressEntry->Create();
  this->ConnectorAddressEntry->SetWidth(18);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              addressLabel->GetWidgetName() , this->ConnectorAddressEntry->GetWidgetName());
  

  vtkKWFrame *portFrame = vtkKWFrame::New();
  portFrame->SetParent(controlFrame->GetFrame());
  portFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                portFrame->GetWidgetName());

  vtkKWLabel *portLabel = vtkKWLabel::New();
  portLabel->SetParent(portFrame);
  portLabel->Create();
  portLabel->SetWidth(8);
  portLabel->SetText("Port: ");

  this->ConnectorPortEntry = vtkKWEntry::New();
  this->ConnectorPortEntry->SetParent(portFrame);
  this->ConnectorPortEntry->SetRestrictValueToInteger();
  this->ConnectorPortEntry->Create();
  this->ConnectorPortEntry->SetWidth(8);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              portLabel->GetWidgetName() , this->ConnectorPortEntry->GetWidgetName());
  

}



//---------------------------------------------------------------------------
void vtkNeuroNavGUI::BuildGUIForVisualizationControlFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("NeuroNav");
  
  vtkSlicerModuleCollapsibleFrame *visCtrlFrame = vtkSlicerModuleCollapsibleFrame::New();
  visCtrlFrame->SetParent(page);
  visCtrlFrame->Create();
  visCtrlFrame->SetLabelText("Visualization / Slice Control");
  visCtrlFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               visCtrlFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Locator Display frame: Options to locator display 

  vtkKWFrameWithLabel *displayFrame = vtkKWFrameWithLabel::New ( );
  displayFrame->SetParent(visCtrlFrame->GetFrame());
  displayFrame->Create();
  displayFrame->SetLabelText("Locator Display");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               displayFrame->GetWidgetName());
  
  this->LocatorCheckButton = vtkKWCheckButton::New();
  this->LocatorCheckButton->SetParent(displayFrame->GetFrame());
  this->LocatorCheckButton->Create();
  this->LocatorCheckButton->SelectedStateOff();
  this->LocatorCheckButton->SetText("Show Locator");
  
  this->Script("pack %s -side left -anchor w -padx 2 -pady 2", 
               this->LocatorCheckButton->GetWidgetName());
  

  // -----------------------------------------
  // Driver frame: Locator can drive slices 


  vtkKWFrameWithLabel *driverFrame = vtkKWFrameWithLabel::New();
  driverFrame->SetParent(visCtrlFrame->GetFrame());
  driverFrame->Create();
  driverFrame->SetLabelText ("Driver");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               driverFrame->GetWidgetName());
  
  // slice frame
  vtkKWFrame *sliceFrame = vtkKWFrame::New();
  sliceFrame->SetParent(driverFrame->GetFrame());
  sliceFrame->Create();
  app->Script("pack %s -side top -anchor nw -fill x -pady 1 -in %s",
              sliceFrame->GetWidgetName(),
              driverFrame->GetFrame()->GetWidgetName());
  

  // Contents in slice frame 
  vtkSlicerColor *color = app->GetSlicerTheme()->GetSlicerColors ( );
  
  this->RedSliceMenu = vtkKWMenuButton::New();
  this->RedSliceMenu->SetParent(sliceFrame);
  this->RedSliceMenu->Create();
  this->RedSliceMenu->SetWidth(10);
  this->RedSliceMenu->SetBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenu->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenu->GetMenu()->AddRadioButton ("User");
  this->RedSliceMenu->GetMenu()->AddRadioButton ("Locator");
  this->RedSliceMenu->GetMenu()->AddRadioButton ("RT Image");
  this->RedSliceMenu->SetValue ("User");
  
  this->YellowSliceMenu = vtkKWMenuButton::New();
  this->YellowSliceMenu->SetParent(sliceFrame);
  this->YellowSliceMenu->Create();
  this->YellowSliceMenu->SetWidth(10);
  this->YellowSliceMenu->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenu->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenu->GetMenu()->AddRadioButton ("User");
  this->YellowSliceMenu->GetMenu()->AddRadioButton ("Locator");
  this->YellowSliceMenu->GetMenu()->AddRadioButton ("RT Image");
  this->YellowSliceMenu->SetValue ("User");
  
  this->GreenSliceMenu = vtkKWMenuButton::New();
  this->GreenSliceMenu->SetParent(sliceFrame);
  this->GreenSliceMenu->Create();
  this->GreenSliceMenu->SetWidth(10);
  this->GreenSliceMenu->SetBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenu->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenu->GetMenu()->AddRadioButton ("User");
  this->GreenSliceMenu->GetMenu()->AddRadioButton ("Locator");
  this->GreenSliceMenu->GetMenu()->AddRadioButton ("RT Image");
  this->GreenSliceMenu->SetValue ("User");
  
  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->RedSliceMenu->GetWidgetName(),
               this->YellowSliceMenu->GetWidgetName(),
               this->GreenSliceMenu->GetWidgetName());
  
  // Mode frame
  vtkKWFrame *modeFrame = vtkKWFrame::New();
  modeFrame->SetParent ( driverFrame->GetFrame() );
  modeFrame->Create ( );
  app->Script ("pack %s -side top -anchor nw -fill x -pady 1 -in %s",
               modeFrame->GetWidgetName(),
               driverFrame->GetFrame()->GetWidgetName());
  
  // "Locator All" button
  this->SetLocatorModeButton = vtkKWPushButton::New ( );
  this->SetLocatorModeButton->SetParent ( modeFrame );
  this->SetLocatorModeButton->Create ( );
  this->SetLocatorModeButton->SetText ("Locator All");
  this->SetLocatorModeButton->SetWidth (12);
  
  // "User All" button
  this->SetUserModeButton = vtkKWPushButton::New ( );
  this->SetUserModeButton->SetParent ( modeFrame );
  this->SetUserModeButton->Create ( );
  this->SetUserModeButton->SetText ("User All");
  this->SetUserModeButton->SetWidth (12);
  
  
  // "Freeze" check button
  this->FreezeImageCheckButton = vtkKWCheckButton::New();
  this->FreezeImageCheckButton->SetParent(modeFrame);
  this->FreezeImageCheckButton->Create();
  this->FreezeImageCheckButton->SelectedStateOff();
  this->FreezeImageCheckButton->SetText("Freeze");
  
  this->ObliqueCheckButton = vtkKWCheckButton::New();
  this->ObliqueCheckButton->SetParent(modeFrame);
  this->ObliqueCheckButton->Create();
  this->ObliqueCheckButton->SelectedStateOff();
  this->ObliqueCheckButton->SetText("Orient");

  this->Script("pack %s %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->SetLocatorModeButton->GetWidgetName(),
               this->SetUserModeButton->GetWidgetName(),
               this->FreezeImageCheckButton->GetWidgetName(),
               this->ObliqueCheckButton->GetWidgetName());


  // -----------------------------------------
  // Real-time imaging: Scanner controled

  /*
  vtkKWFrameWithLabel *rtImageFrame = vtkKWFrameWithLabel::New ( );
  rtImageFrame->SetParent(visCtrlFrame->GetFrame());
  rtImageFrame->Create();
  rtImageFrame->SetLabelText("Real-time Imaging");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               rtImageFrame->GetWidgetName());

  // Scan start/stop frame
  vtkKWFrame *scanFrame = vtkKWFrame::New();
  scanFrame->SetParent (rtImageFrame->GetFrame());
  scanFrame->Create();
  app->Script("pack %s -side top -anchor nw -fill x -pady 1 -in %s",
              scanFrame->GetWidgetName(),
              rtImageFrame->GetFrame()->GetWidgetName());
  
  this->StartScanButton = vtkKWPushButton::New();
  this->StartScanButton->SetParent(scanFrame);
  this->StartScanButton->Create();
  this->StartScanButton->SetText("Start Scan");
  this->StartScanButton->SetWidth(12);
  
  this->StopScanButton = vtkKWPushButton::New();
  this->StopScanButton->SetParent(scanFrame);
  this->StopScanButton->Create();
  this->StopScanButton->SetText("Stop Scan");
  this->StopScanButton->SetWidth(12);

  this->ImagingMenu = vtkKWMenuButton::New();
  this->ImagingMenu->SetParent(scanFrame);
  this->ImagingMenu->Create();
  this->ImagingMenu->SetWidth(10);
  this->ImagingMenu->GetMenu()->AddRadioButton ("None");
  this->ImagingMenu->GetMenu()->AddRadioButton ("Perpendicular");
  this->ImagingMenu->GetMenu()->AddRadioButton ("In-plane 90");
  this->ImagingMenu->GetMenu()->AddRadioButton ("In-plane");
  this->ImagingMenu->SetValue("None");

  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               StartScanButton->GetWidgetName(),
               StopScanButton->GetWidgetName(),
               ImagingMenu->GetWidgetName());

  */
  displayFrame->Delete();
  driverFrame->Delete();
  modeFrame->Delete();
  sliceFrame->Delete();
  visCtrlFrame->Delete();

}


//----------------------------------------------------------------------------
void vtkNeuroNavGUI::UpdateAll()
{

  /*
  if (this->LocatorCheckButton->GetSelectedState())
    {
    }
  */

}


void vtkNeuroNavGUI::ChangeSlicePlaneDriver(int slice, const char* driver)
{

  if (slice == vtkNeuroNavGUI::SLICE_PLANE_RED)
    {
    this->RedSliceMenu->SetValue(driver);
    if (strcmp(driver, "User") == 0)
      {
      this->SliceNode0->SetOrientationToAxial();
      //this->SliceDriver0 = vtkNeuroNavGUI::SLICE_DRIVER_USER;
      this->GetLogic()->SetSliceDriver0(vtkNeuroNavLogic::SLICE_DRIVER_USER);
      }
    else if (strcmp(driver, "Locator") == 0)
      {
      //this->SliceDriver0 = vtkNeuroNavGUI::SLICE_DRIVER_LOCATOR;
      this->GetLogic()->SetSliceDriver0(vtkNeuroNavLogic::SLICE_DRIVER_LOCATOR);
      }
    else if (strcmp(driver, "RT Image") == 0)
      {
      //this->SliceDriver0 = vtkNeuroNavGUI::SLICE_DRIVER_RTIMAGE;
      this->GetLogic()->SetSliceDriver0(vtkNeuroNavLogic::SLICE_DRIVER_RTIMAGE);
      }
    }
  else if (slice == vtkNeuroNavGUI::SLICE_PLANE_YELLOW)
    {
    this->YellowSliceMenu->SetValue(driver);
    if (strcmp(driver, "User") == 0)
      {
      this->SliceNode1->SetOrientationToSagittal();
      //this->SliceDriver1 = vtkNeuroNavGUI::SLICE_DRIVER_USER;
      this->GetLogic()->SetSliceDriver1(vtkNeuroNavLogic::SLICE_DRIVER_USER);
      }
    else if (strcmp(driver, "Locator") == 0)
      {
      //this->SliceDriver1 = vtkNeuroNavGUI::SLICE_DRIVER_LOCATOR;
      this->GetLogic()->SetSliceDriver1(vtkNeuroNavLogic::SLICE_DRIVER_LOCATOR);
      }
    else if (strcmp(driver, "RT Image") == 0)
      {
      //this->SliceDriver1 = vtkNeuroNavGUI::SLICE_DRIVER_RTIMAGE;
      this->GetLogic()->SetSliceDriver1(vtkNeuroNavLogic::SLICE_DRIVER_RTIMAGE);
      }
    }
  else //if ( slice == vtkNeuroNavGUI::SLICE_PLANE_GREEN )
    {
    this->GreenSliceMenu->SetValue(driver);
    if (strcmp(driver, "User") == 0)
      {
      this->SliceNode2->SetOrientationToCoronal();
      //this->SliceDriver2 = vtkNeuroNavGUI::SLICE_DRIVER_USER;
      this->GetLogic()->SetSliceDriver2(vtkNeuroNavLogic::SLICE_DRIVER_USER);
      }
    else if (strcmp(driver, "Locator") == 0)
      {
      //this->SliceDriver2 = vtkNeuroNavGUI::SLICE_DRIVER_LOCATOR;
      this->GetLogic()->SetSliceDriver2(vtkNeuroNavLogic::SLICE_DRIVER_LOCATOR);
      }
    else if (strcmp(driver, "RT Image") == 0)
      {
      //this->SliceDriver2 = vtkNeuroNavGUI::SLICE_DRIVER_RTIMAGE;
      this->GetLogic()->SetSliceDriver2(vtkNeuroNavLogic::SLICE_DRIVER_RTIMAGE);
      }
    }

}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::UpdateConnectorList(int updateLevel)
{

}


//---------------------------------------------------------------------------
void vtkNeuroNavGUI::UpdateConnectorPropertyFrame(int i)
{


}

