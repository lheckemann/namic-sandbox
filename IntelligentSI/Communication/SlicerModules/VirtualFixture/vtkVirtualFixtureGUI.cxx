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
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

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
  this->DeleteSphereButton = NULL;

  /*
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;
  */
  
  //----------------------------------------------------------------
  // Sphere data
  this->SphereList.clear();
  this->CurrentSphere = -1;

  //----------------------------------------------------------------
  // 
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

  if (this->SphereMenu)
    {
    this->SphereMenu->SetParent(NULL);
    this->SphereMenu->Delete();
    }

  if (this->SphereNameEntry)
    {
    this->SphereNameEntry->SetParent(NULL);
    this->SphereNameEntry->Delete();
    }
  
  if (this->CenterXEntry)
    {
    this->CenterXEntry->SetParent(NULL);
    this->CenterXEntry->Delete();
    }

  if (this->CenterYEntry)
    {
    this->CenterYEntry->SetParent(NULL);
    this->CenterYEntry->Delete();
    }

  if (this->CenterZEntry)
    {
    this->CenterZEntry->SetParent(NULL);
    this->CenterZEntry->Delete();
    }

  if (this->RadiusEntry)
    {
    this->RadiusEntry->SetParent(NULL);
    this->RadiusEntry->Delete();
    }

  if (this->UpdateSphereButton)
    {
    this->UpdateSphereButton->SetParent(NULL);
    this->UpdateSphereButton->Delete();
    }

  if (this->DeleteSphereButton)
    {
    this->DeleteSphereButton->SetParent(NULL);
    this->DeleteSphereButton->Delete();
    }

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


  if (this->SphereMenu)
    {
    this->SphereMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SphereNameEntry)
    {
    this->SphereNameEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->CenterXEntry)
    {
    this->CenterXEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CenterYEntry)
    {
    this->CenterYEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CenterZEntry)
    {
    this->CenterZEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RadiusEntry)
    {
    this->RadiusEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->UpdateSphereButton)
    {
    this->UpdateSphereButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->DeleteSphereButton)
    {
    this->DeleteSphereButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

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

  this->SphereMenu->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->SphereNameEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->CenterXEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->CenterYEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->CenterZEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->RadiusEntry
    ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->UpdateSphereButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DeleteSphereButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    
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

  if (this->SphereMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller) 
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int nitem = this->SphereMenu->GetMenu()->GetNumberOfItems();
    // Note: GetIndexOfSelectedItem is not available in Slicer 3.2
    //int sitem = this->SphereMenu->GetMenu()->GetIndexOfSelectedItem();
    int sitem = -1;
    for (int i = 0; i < nitem; i ++)
      {
      if (this->SphereMenu->GetMenu()->GetItemSelectedState(i))
        {
        sitem = i;
        }
      }

    std::cerr << "sitem = " << sitem << ", nitem = " << nitem << std::endl;
    if (sitem == nitem - 1) // "New Sphere" is selected
      {
      char name[128];
      sprintf(name, "VF Sphere %d", nitem - 1);
      AddNewSphere(name);
      }
    else
      {
      SelectSphere(sitem);
      }
    }
  else if (this->SphereNameEntry == vtkKWEntry::SafeDownCast(caller) 
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->CenterXEntry == vtkKWEntry::SafeDownCast(caller) 
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->CenterYEntry == vtkKWEntry::SafeDownCast(caller) 
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->CenterZEntry == vtkKWEntry::SafeDownCast(caller) 
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->RadiusEntry == vtkKWEntry::SafeDownCast(caller) 
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->UpdateSphereButton == vtkKWPushButton::SafeDownCast(caller) 
           && event == vtkKWPushButton::InvokedEvent)
    {
    UpdateSphere();
    }
  else if (this->DeleteSphereButton == vtkKWPushButton::SafeDownCast(caller) 
           && event == vtkKWPushButton::InvokedEvent)
    {
    }

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
  sphereMenuLabel->SetText("VFixture: ");

  this->SphereMenu = vtkKWMenuButton::New();
  this->SphereMenu->SetParent(sphereMenuFrame);
  this->SphereMenu->Create();
  this->SphereMenu->SetWidth(10);
  this->SphereMenu->GetMenu()->AddRadioButton ("New Sphere");
  this->SphereMenu->SetValue ("");

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
  this->SphereNameEntry->SetValue ("");

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
  this->CenterXEntry->SetValueAsDouble (0.0);

  this->CenterYEntry = vtkKWEntry::New();
  this->CenterYEntry->SetParent(centerCoordinateFrame);
  this->CenterYEntry->Create();
  this->CenterYEntry->SetWidth(8);
  this->CenterYEntry->SetRestrictValueToDouble();
  this->CenterYEntry->SetValueAsDouble (0.0);

  this->CenterZEntry = vtkKWEntry::New();
  this->CenterZEntry->SetParent(centerCoordinateFrame);
  this->CenterZEntry->Create();
  this->CenterZEntry->SetWidth(8);
  this->CenterZEntry->SetRestrictValueToDouble();
  this->CenterZEntry->SetValueAsDouble (0.0);

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
  radiusLabel->SetText("Radius: ");

  this->RadiusEntry = vtkKWEntry::New();
  this->RadiusEntry->SetParent(radiusFrame);
  this->RadiusEntry->Create();
  this->RadiusEntry->SetWidth(10);
  this->RadiusEntry->SetRestrictValueToDouble();
  this->RadiusEntry->SetValueAsDouble (0.0);

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               radiusLabel->GetWidgetName() , this->RadiusEntry->GetWidgetName());

  // -----------------------------------------
  // BUttons
  vtkKWFrame *buttonsFrame = vtkKWFrame::New();
  buttonsFrame->SetParent(conBrowsFrame->GetFrame());
  buttonsFrame->Create();
  this->Script ("pack %s -fill both -expand true",  
                buttonsFrame->GetWidgetName() );

  this->UpdateSphereButton = vtkKWPushButton::New();
  this->UpdateSphereButton->SetParent(buttonsFrame);
  this->UpdateSphereButton->Create();
  this->UpdateSphereButton->SetWidth(10);
  this->UpdateSphereButton->SetText("Update");

  this->DeleteSphereButton = vtkKWPushButton::New();
  this->DeleteSphereButton->SetParent(buttonsFrame);
  this->DeleteSphereButton->Create();
  this->DeleteSphereButton->SetWidth(10);
  this->DeleteSphereButton->SetText("Delete");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->UpdateSphereButton->GetWidgetName(),
               this->DeleteSphereButton->GetWidgetName());

  sphereMenuFrame->Delete();
  sphereMenuLabel->Delete();
  sphereNameFrame->Delete();
  sphereNameLabel->Delete();
  radiusFrame->Delete();
  radiusLabel->Delete();
  buttonsFrame->Delete();
  centerCoordinateFrame->Delete();
  centerCoordinateLabel->Delete();

  conBrowsFrame->Delete();

  
}


//----------------------------------------------------------------------------
void vtkVirtualFixtureGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
int vtkVirtualFixtureGUI::AddNewSphere(const char* name)
{
  // Create data
  SphereData data;
  data.name = std::string(name);
  data.center[0] = 0.0;
  data.center[1] = 0.0;
  data.center[2] = 0.0;
  data.radius = 10.0;

  double color[3];
  color[0] = 1.0; // red
  color[1] = 0; // green
  color[2] = 0; // blue

  // Create Sphere Model
  AddSphereModel(&data, data.center, data.radius, color);

  // push into the list
  this->SphereList.push_back(data);
  this->SphereMenu->GetMenu()->DeleteAllItems();
  SphereListType::iterator iter;
  for (iter = this->SphereList.begin(); iter != this->SphereList.end(); iter ++)
    {
    this->SphereMenu->GetMenu()->AddRadioButton (iter->name.c_str());
    }
  this->SphereMenu->GetMenu()->AddRadioButton ("New Sphere");

  SelectSphere(this->SphereList.size()-1);

  return this->SphereList.size()-1;
}

//----------------------------------------------------------------------------
int vtkVirtualFixtureGUI::SelectSphere(int n)
{
  if (n >= 0 && n < this->SphereList.size())
    {
    this->CurrentSphere = n;
    this->SphereMenu->GetMenu()->SelectItem(n);

    SphereData data = this->SphereList[n];
    this->SphereNameEntry->SetValue(data.name.c_str());
    this->CenterXEntry->SetValueAsDouble(data.center[0]);
    this->CenterYEntry->SetValueAsDouble(data.center[1]);
    this->CenterZEntry->SetValueAsDouble(data.center[2]);
    this->RadiusEntry->SetValueAsDouble(data.radius);

    // highlight sphere model in the 3D scene
    SphereListType::iterator iter;
    for (iter = this->SphereList.begin(); iter != this->SphereList.end(); iter ++)
      {
      HighlightSphereModel(&(*iter), false);
      }
    HighlightSphereModel(&(this->SphereList[n]), true);

    return n;
    }
  else
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
int vtkVirtualFixtureGUI::UpdateSphere()
{

  if (this->CurrentSphere < 0)
    return -1;

  int n = this->CurrentSphere;

  this->SphereList[n].name = std::string(this->SphereNameEntry->GetValue());
  this->SphereList[n].center[0] = this->CenterXEntry->GetValueAsDouble();
  this->SphereList[n].center[1] = this->CenterYEntry->GetValueAsDouble();
  this->SphereList[n].center[2] = this->CenterZEntry->GetValueAsDouble();
  this->SphereList[n].radius    = this->RadiusEntry->GetValueAsDouble();
  this->SphereMenu->GetMenu()->SetItemLabel(n, this->SphereNameEntry->GetValue());
  this->SphereMenu->GetMenu()->SelectItem(this->SphereMenu->GetMenu()->GetNumberOfItems()-1);
  this->SphereMenu->GetMenu()->SelectItem(n);

  UpdateSphereModel(&(this->SphereList[n]));

  return n;
}

//---------------------------------------------------------------------------
void vtkVirtualFixtureGUI::AddSphereModel(SphereData* data, double center[3], double radius,
                                                       double color[3])
{
  vtkMRMLModelNode           *model;
  vtkMRMLModelDisplayNode    *disp;

  model = vtkMRMLModelNode::New();
  disp  = vtkMRMLModelDisplayNode::New();
  
  this->GetMRMLScene()->SaveStateForUndo();
  this->GetMRMLScene()->AddNode(disp);
  this->GetMRMLScene()->AddNode(model);  
  
  disp->SetScene(this->GetMRMLScene());
  
  model->SetName(data->name.c_str());
  model->SetScene(this->GetMRMLScene());
  model->SetAndObserveDisplayNodeID(disp->GetID());
  model->SetHideFromEditors(1);
  
  // Sphere represents the locator tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(radius);
  sphere->SetCenter(center);
  sphere->Update();

  /*
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  apd->Update();
  */
  
  //model->SetAndObservePolyData(apd->GetOutput());
  model->SetAndObservePolyData(sphere->GetOutput());
  
  disp->SetPolyData(model->GetPolyData());
  sphere->Delete();
  //apd->Delete();

  disp->SetColor(color);
  disp->SetVisibility(1);
  disp->SetOpacity(0.3);
  model->Modified();
  this->GetApplicationLogic()->GetMRMLScene()->Modified();

  data->sphere = sphere;
  data->model  = model;

  model->Delete();
  disp->Delete();
}


void vtkVirtualFixtureGUI::UpdateSphereModel(SphereData* data)
{
  data->sphere->SetRadius(data->radius);
  data->sphere->SetCenter(data->center);
  data->sphere->Update();
  
  //vtkMRMLModelDisplayNode* disp = data->model->GetDisplayNode();
  //disp->SetOpacity(0.5);
  //disp->SetColor(color);

  data->model->Modified();
  this->GetApplicationLogic()->GetMRMLScene()->Modified();
}


void vtkVirtualFixtureGUI::HighlightSphereModel(SphereData* data, bool highlight)
{
  double color[3];
  if (highlight)
    {
    color[0] = 1.0;
    color[1] = 0.2;
    color[2] = 0.2;
    }
  else
    {
    color[0] = 0.2;
    color[1] = 0.2;
    color[2] = 1.0;
    }

  vtkMRMLDisplayNode* disp = data->model->GetDisplayNode();
  disp->SetColor(color);

  data->model->Modified();
  this->GetApplicationLogic()->GetMRMLScene()->Modified();
}


