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

#include "vtkPointRegistrationGUI.h"
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
#include "vtkCornerAnnotation.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWPushButton.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWMultiColumnList.h"
#include "vtkPoints.h"
#include "vtkMRMLLinearTransformNode.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkPointRegistrationGUI );
vtkCxxRevisionMacro ( vtkPointRegistrationGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkPointRegistrationGUI::vtkPointRegistrationGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPointRegistrationGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->PatTransformNode = NULL;
  this->TrackerNode = NULL;
  this->PatCoordinatesEntry = NULL;
  this->ImagCoordinatesEntry = NULL;
  this->GetPatCoordinatesButton = NULL;
  this->AddPointPairButton = NULL;
  this->PointPairMultiColumnList = NULL;
  this->DeletePointPairButton = NULL;
  this->DeleteAllPointPairButton = NULL;    
  this->RegisterButton = NULL;
  this->ResetButton = NULL;
  this->RegistrationStatus = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkPointRegistrationGUI::~vtkPointRegistrationGUI ( )
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

  if (this->PatTransformNode)
    {
    this->PatTransformNode->SetParent(NULL);
    this->PatTransformNode->Delete();
    }

  if (this->TrackerNode)
    {
    this->TrackerNode->SetParent(NULL);
    this->TrackerNode->Delete();
    }

  if (this->PatCoordinatesEntry)
    {
    this->PatCoordinatesEntry->SetParent(NULL);
    this->PatCoordinatesEntry->Delete();
    }

  if (this->ImagCoordinatesEntry)
    {
    this->ImagCoordinatesEntry->SetParent(NULL);
    this->ImagCoordinatesEntry->Delete();
    }

  if (this->GetPatCoordinatesButton)
    {
    this->GetPatCoordinatesButton->SetParent(NULL);
    this->GetPatCoordinatesButton->Delete();
    }
    
  if (this->AddPointPairButton)
    {
    this->AddPointPairButton ->SetParent(NULL);
    this->AddPointPairButton ->Delete();
    }

  if (this->PointPairMultiColumnList)
    {
    this->PointPairMultiColumnList->SetParent(NULL);
    this->PointPairMultiColumnList->Delete();
    }

  if (this->DeletePointPairButton)
    {
    this->DeletePointPairButton->SetParent(NULL);
    this->DeletePointPairButton->Delete();
    }

  if (this->DeleteAllPointPairButton)
    {
    this->DeleteAllPointPairButton->SetParent(NULL);
    this->DeleteAllPointPairButton->Delete();
    }

  if (this->RegisterButton)
    {
    this->RegisterButton->SetParent(NULL);
    this->RegisterButton->Delete();
    }

  if (this->ResetButton)
    {
    this->ResetButton->SetParent(NULL);
    this->ResetButton->Delete();
    }

  if (this->RegistrationStatus)
    {
    this->RegistrationStatus->SetParent(NULL);
    this->RegistrationStatus->Delete();
    }
  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}

//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::Init()
{
}

//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::Enter()
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
void vtkPointRegistrationGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "PointRegistrationGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::RemoveGUIObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (appGUI)
    {
    appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->PatTransformNode)
    {
    this->PatTransformNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TrackerNode)
    {
    this->TrackerNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PatCoordinatesEntry)
    {
    this->PatCoordinatesEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ImagCoordinatesEntry)
    {
    this->ImagCoordinatesEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->GetPatCoordinatesButton)
    {
    this->GetPatCoordinatesButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AddPointPairButton)
    {
    this->AddPointPairButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PointPairMultiColumnList)
    {
    this->PointPairMultiColumnList
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->DeletePointPairButton)
    {
    this->DeletePointPairButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->DeleteAllPointPairButton)
    {
    this->DeleteAllPointPairButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->RegisterButton)
    {
    this->RegisterButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    
  if (this->ResetButton)
    {
    this->ResetButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

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
  // GUI Observers
  
  //Image Slice GUI
    appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  
  //Widgets
  this->GetPatCoordinatesButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->AddPointPairButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DeletePointPairButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DeleteAllPointPairButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->RegisterButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ResetButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->PatTransformNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkPointRegistrationLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  vtkSlicerInteractorStyle *istyle0 = 
    vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()
                                           ->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkSlicerInteractorStyle *istyle1 = 
    vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()
                                           ->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkSlicerInteractorStyle *istyle2 = 
    vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()
                                           ->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkCornerAnnotation *anno = NULL;
  if (style == istyle0)
    {
    anno = appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle1)
    {
    anno = appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle2)
    {
    anno = appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }

  if (anno)
    {
    const char *rasText = anno->GetText(1);
    if ( rasText != NULL )
      {
      std::string ras = std::string(rasText);

      // remove "R:," "A:," and "S:" from the string
      size_t loc = ras.find("R:", 0);
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
      size_t found = ras.find("\n", 0);
      while ( found != std::string::npos )
        {
        ras = ras.replace(found, 1, " ");
        found = ras.find("\n", 0);
        }

      this->ImagCoordinatesEntry->GetWidget()->SetValue(ras.c_str());
      }
    }
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //Get Patient Coordinates button
  else if(this->GetPatCoordinatesButton == vtkKWPushButton::SafeDownCast(caller) 
        && event == vtkKWPushButton::InvokedEvent)
    {
    double px, py, pz;
    char value[50];
    this->GetLogic()->GetCurrentPosition(this->PatTransformNode->GetSelected() , &px, &py, &pz);
    sprintf(value, "%6.2f  %6.2f  %6.2f", px, py, pz);
    this->PatCoordinatesEntry->GetWidget()->SetValue(value);
    }
  
  //Add point pair to list button  
  else if (this->AddPointPairButton == vtkKWPushButton::SafeDownCast(caller) 
               && event == vtkKWPushButton::InvokedEvent)
    {
    int scSize = 0;
    int pcSize = 0;
    const char *pc = this->PatCoordinatesEntry->GetWidget()->GetValue();
    const char *sc = this->ImagCoordinatesEntry->GetWidget()->GetValue();
    if (pc) 
      {
      const vtksys_stl::string pcCor(pc);
      pcSize = pcCor.size();
      }
    if (sc) 
      {
      const vtksys_stl::string scCor(sc);
      scSize = scCor.size();
      }
    if (pcSize < 5 || scSize < 5)
      {
      vtkSlicerApplication::GetInstance()->ErrorMessage("Patient or Image coordinates are invalid."); 
      }
    else 
      {
      int row = this->PointPairMultiColumnList->GetWidget()->GetNumberOfRows();
      this->PointPairMultiColumnList->GetWidget()->AddRow();
      this->PointPairMultiColumnList->GetWidget()->SetCellText(row, 0, pc);
      this->PointPairMultiColumnList->GetWidget()->SetCellText(row, 1, sc);
      this->PatCoordinatesEntry->GetWidget()->SetValue("");
      this->ImagCoordinatesEntry->GetWidget()->SetValue("");
      }
    } 
  
  //delete a point pair from the list button  
  else if (this->DeletePointPairButton == vtkKWPushButton::SafeDownCast(caller) 
           && event == vtkKWPushButton::InvokedEvent)
    {
    int numOfRows = this->PointPairMultiColumnList->GetWidget()->GetNumberOfSelectedRows();
    if (numOfRows == 1)
      {
      int index[2];
      this->PointPairMultiColumnList->GetWidget()->GetSelectedRows(index);
      this->PointPairMultiColumnList->GetWidget()->DeleteRow(index[0]);
      }
    }
  
  //delete all points from the list button  
  else if (this->DeleteAllPointPairButton == vtkKWPushButton::SafeDownCast(caller) 
          && event == vtkKWPushButton::InvokedEvent)
    {
    this->PointPairMultiColumnList->GetWidget()->DeleteAllRows();
    } 
  
  //Register button
  else if (this->RegisterButton == vtkKWPushButton::SafeDownCast(caller) 
             && event == vtkKWPushButton::InvokedEvent)
    {
    int row = this->PointPairMultiColumnList->GetWidget()->GetNumberOfRows();
    if (row < 2)
      {
      vtkSlicerApplication::GetInstance()->ErrorMessage("At least 2 pairs of landmarks are needed for patient to image registration.");
      }
    else
      {
      std::cerr << "About to extract the coordinates from the list" << std::endl;
      float* coord = new float[3];
      vtkPoints* patCoords = vtkPoints::New();
      vtkPoints* imCoords = vtkPoints::New();
      for (int r = 0; r < row; r++)
        {
        for (int c = 0; c < 2; c++)
          {
          const char *val = this->PointPairMultiColumnList->GetWidget()->GetCellText(r, c);
          if (c == 0)
            {
            sscanf(val, "%f %f %f", &coord[0], &coord[1], &coord[2]);
            patCoords->InsertNextPoint(coord);
            }
          else
            {
            sscanf(val, "%f %f %f", &coord[0], &coord[1], &coord[2]);
            imCoords->InsertNextPoint(coord);
            }
          }
        }
      std::cerr << "Extracted coordinates from the list" << std::endl;
      //Calculate registration Matrix
      if (this->regTrans == NULL)
        {
        this->regTrans = vtkMatrix4x4::New();
        }
      int error = this->GetLogic()->PerformPatientToImageRegistration(patCoords, imCoords, this->regTrans);
      if (error == 1)
        {
        vtkSlicerApplication::GetInstance()->ErrorMessage("Error in registration between patient and image land marks.");
        return;
        }
      std::cerr << "Registration Matrix calculated" << std::endl;
      //Create Registration Node
      vtkMRMLLinearTransformNode* registrationMatrix = vtkMRMLLinearTransformNode::New();
      registrationMatrix->SetAndObserveMatrixTransformToParent(this->regTrans);
      registrationMatrix->SetName("RegistrationMatrix");
      this->GetMRMLScene()->AddNode(vtkMRMLNode::SafeDownCast(registrationMatrix));
      this->GetMRMLScene()->Modified();
      std::cerr << "Node added" << std::endl;
      
      //update status
      this->RegistrationStatus->SetText("Registration Matrix calculated successfully");

      //clean up
      patCoords->Delete();
      imCoords->Delete();
      }
    }
  
  //Reset Button
  else if (this->ResetButton == vtkKWPushButton::SafeDownCast(caller) 
          && event == vtkKWPushButton::InvokedEvent)
    {
    //Delete all values from text boxes
    this->PatCoordinatesEntry->GetWidget()->SetValue("");
    this->ImagCoordinatesEntry->GetWidget()->SetValue("");
    this->PointPairMultiColumnList->GetWidget()->DeleteAllRows();
    
    //Check if registration matrix has been defined
    if (!this->regTrans)
      this->regTrans = vtkMatrix4x4::New();
    else
      this->regTrans->Identity();

    //update Status
    this->RegistrationStatus->SetText("No registration matrix calculated");
    }  
    
}


//---------------------------------------------------------------------------------------
void vtkPointRegistrationGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkPointRegistrationGUI *self = reinterpret_cast<vtkPointRegistrationGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPointRegistrationGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkPointRegistrationLogic::SafeDownCast(caller))
    {
    if (event == vtkPointRegistrationLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

}


//---------------------------------------------------------------------------
void vtkPointRegistrationGUI::ProcessTimerEvents()
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
void vtkPointRegistrationGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "PointRegistration", "PointRegistration", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForRegistrationFrame();

}

//--------------------------------------------------------------------------
void vtkPointRegistrationGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:PointRegistration</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PointRegistration" );
  this->BuildHelpAndAboutFrame (page, help, about);
}

//--------------------------------------------------------------------------
void vtkPointRegistrationGUI::BuildGUIForRegistrationFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PointRegistration" );

  // ----------------------------------------------------------------
  // REGISTRATION FRAME            

  vtkSlicerModuleCollapsibleFrame *regFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  regFrame->SetParent ( page );
  regFrame->Create ( );
  regFrame->SetLabelText ("Registration");
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                regFrame->GetWidgetName(), page->GetWidgetName());


  // Add a Point Pair 
  vtkKWFrameWithLabel *pairFrame = vtkKWFrameWithLabel::New();
  pairFrame->SetParent ( regFrame->GetFrame() );
  pairFrame->Create ( );
  pairFrame->SetLabelText ("Add a Point Pair");
  this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                pairFrame->GetWidgetName());
               
  //---------------------------------------------
  //Patient Coordinates Frame
  //---------------------------------------------
  vtkKWFrameWithLabel *patCoordFrame = vtkKWFrameWithLabel::New();
  patCoordFrame->SetParent ( pairFrame->GetFrame() );
  patCoordFrame->Create ( );
  patCoordFrame->SetLabelText ("Patient Coordinates");
  this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                patCoordFrame->GetWidgetName());

  //Transform Row
  vtkKWFrame *patFrame = vtkKWFrame::New();
  patFrame->SetParent ( patCoordFrame->GetFrame() );
  patFrame->Create ( );
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                patFrame->GetWidgetName());

  vtkKWLabel *pnodeLabel = vtkKWLabel::New();
  pnodeLabel->SetParent(patFrame);
  pnodeLabel->Create();
  pnodeLabel->SetWidth(15);
  pnodeLabel->SetText("Transform Node: ");

  this->PatTransformNode = vtkSlicerNodeSelectorWidget::New();
  this->PatTransformNode->SetParent(patFrame);
  this->PatTransformNode->Create();
  this->PatTransformNode->SetWidth(20);
  this->PatTransformNode->SetNewNodeEnabled(0);
  this->PatTransformNode->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->PatTransformNode->NoneEnabledOn();
  this->PatTransformNode->SetShowHidden(1);
  this->PatTransformNode->Create();
  this->PatTransformNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->PatTransformNode->UpdateMenu();
  this->PatTransformNode->SetBorderWidth(0);
  this->PatTransformNode->SetBalloonHelpString("Select a transform from the Scene");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              pnodeLabel->GetWidgetName() , this->PatTransformNode->GetWidgetName());

  //Patient Entry text box row
  vtkKWFrame *patEntryFrame = vtkKWFrame::New();
  patEntryFrame->SetParent ( patCoordFrame->GetFrame() );
  patEntryFrame->Create ( );
  app->Script( "pack %s -side top -anchor w -expand n -padx 2 -pady 2",
                patEntryFrame->GetWidgetName());

  this->PatCoordinatesEntry = vtkKWEntryWithLabel::New();
  this->PatCoordinatesEntry->SetParent(patEntryFrame);
  this->PatCoordinatesEntry->Create();
  this->PatCoordinatesEntry->SetWidth(30);
  this->PatCoordinatesEntry->SetLabelWidth(16);
  this->PatCoordinatesEntry->SetLabelText("Patient Coordinates:");
  this->PatCoordinatesEntry->GetWidget()->SetValue ( "" );
  app->Script( "pack %s -side top -anchor w -expand n -padx 2 -pady 2",
                PatCoordinatesEntry->GetWidgetName());


  //Patient Get Coordinate Button Row
  vtkKWFrame *patGetFrame = vtkKWFrame::New();
  patGetFrame->SetParent ( patCoordFrame->GetFrame() );
  patGetFrame->Create ( );
  this->Script( "pack %s -padx 2 -pady 2",
                patGetFrame->GetWidgetName());

  this->GetPatCoordinatesButton = vtkKWPushButton::New();
  this->GetPatCoordinatesButton->SetParent(patGetFrame);
  this->GetPatCoordinatesButton->Create();
  this->GetPatCoordinatesButton->SetText("Get Coordinates");
  this->GetPatCoordinatesButton->SetWidth ( 15 );

  this->Script( "pack %s -side left -anchor w -fill x -padx 2 -pady 2",
                GetPatCoordinatesButton->GetWidgetName());

  pnodeLabel->Delete();
  patFrame->Delete();
  patEntryFrame->Delete();
  patGetFrame->Delete();
  patCoordFrame->Delete();

  //---------------------------------------------
  //Image Coordinates Frame
  //---------------------------------------------
  vtkKWFrameWithLabel *imCoordFrame = vtkKWFrameWithLabel::New();
  imCoordFrame->SetParent ( pairFrame->GetFrame() );
  imCoordFrame->Create ( );
  imCoordFrame->SetLabelText ("Image Coordinates");
  this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                imCoordFrame->GetWidgetName());

  //Label Instructions Row:
  vtkKWFrame *imFrame = vtkKWFrame::New();
  imFrame->SetParent ( imCoordFrame->GetFrame() );
  imFrame->Create ( );
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                imFrame->GetWidgetName());

  vtkKWLabel* imLabel = vtkKWLabel::New();
  imLabel->SetParent(imFrame);
  imLabel->Create();
  imLabel->SetWidth(30);
  imLabel->SetText("Click on image slices to select point ");

  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                imLabel->GetWidgetName());

  //Image Entry text box row
  vtkKWFrame *imEntryFrame = vtkKWFrame::New();
  imEntryFrame->SetParent ( imCoordFrame->GetFrame() );
  imEntryFrame->Create ( );
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                imEntryFrame->GetWidgetName());
                
  this->ImagCoordinatesEntry = vtkKWEntryWithLabel::New();
  this->ImagCoordinatesEntry->SetParent(imEntryFrame);
  this->ImagCoordinatesEntry->Create();
  this->ImagCoordinatesEntry->SetWidth(30);
  this->ImagCoordinatesEntry->SetLabelWidth(16);
  this->ImagCoordinatesEntry->SetLabelText("Image Coordinates:");
  this->ImagCoordinatesEntry->GetWidget()->SetValue ( "" );
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->ImagCoordinatesEntry->GetWidgetName());

  //Add Point Pair to List Row
  vtkKWFrame *addFrame = vtkKWFrame::New();
  addFrame->SetParent ( pairFrame->GetFrame() );
  addFrame->Create ( );
  this->Script( "pack %s -padx 2 -pady 2",
                addFrame->GetWidgetName());
  
  this->AddPointPairButton = vtkKWPushButton::New();
  this->AddPointPairButton->SetParent(addFrame);
  this->AddPointPairButton->Create();
  this->AddPointPairButton->SetText( "Add Point Pair" );
  this->AddPointPairButton->SetWidth ( 15 );
  this->Script( "pack %s -side left -anchor w -fill x -padx 2 -pady 2",
                this->AddPointPairButton->GetWidgetName());

  imLabel->Delete();
  imFrame->Delete();
  imEntryFrame->Delete();
  addFrame->Delete();
  imCoordFrame->Delete();

  //-----------------------------------------------
  // Point Pair List
  //-----------------------------------------------
  vtkKWFrameWithLabel *listFrame = vtkKWFrameWithLabel::New();
  listFrame->SetParent ( regFrame->GetFrame() );
  listFrame->Create ( );
  listFrame->SetLabelText ("Defined Point Pairs");
  this->Script( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                listFrame->GetWidgetName());

  // add the multicolumn list to show the points
  this->PointPairMultiColumnList = vtkKWMultiColumnListWithScrollbars::New ( );
  this->PointPairMultiColumnList->SetParent ( listFrame->GetFrame() );
  this->PointPairMultiColumnList->Create ( );
  this->PointPairMultiColumnList->SetHeight(1);
  this->PointPairMultiColumnList->GetWidget()->SetSelectionTypeToRow();
  this->PointPairMultiColumnList->GetWidget()->MovableRowsOff();
  this->PointPairMultiColumnList->GetWidget()->MovableColumnsOff();
  // set up the columns of data for each point
  // refer to the header file for order
  this->PointPairMultiColumnList->GetWidget()->AddColumn("Patient Coordinates");
  this->PointPairMultiColumnList->GetWidget()->AddColumn("Image Coordinates");

  // now set the attributes that are equal across the columns
  for (int col = 0; col < 2; col++)
    {
    this->PointPairMultiColumnList->GetWidget()->SetColumnWidth(col, 24);
    this->PointPairMultiColumnList->GetWidget()->SetColumnAlignmentToLeft(col);
    this->PointPairMultiColumnList->GetWidget()->ColumnEditableOff(col);
    }

  app->Script ( "pack %s -fill both -expand true",
                this->PointPairMultiColumnList->GetWidgetName());

  // Button frame
  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent ( listFrame->GetFrame() );
  buttonFrame->Create ( );
  app->Script ("pack %s -padx 2 -pady 2",
               buttonFrame->GetWidgetName());

  // add a delete button 
  this->DeletePointPairButton = vtkKWPushButton::New ( );
  this->DeletePointPairButton->SetParent ( buttonFrame );
  this->DeletePointPairButton->Create ( );
  this->DeletePointPairButton->SetText ("Delete Point");
  this->DeletePointPairButton->SetWidth (12);
  this->DeletePointPairButton->SetBalloonHelpString("Delete the selected point pair.");

  // add a delete button 
  this->DeleteAllPointPairButton = vtkKWPushButton::New ( );
  this->DeleteAllPointPairButton->SetParent ( buttonFrame );
  this->DeleteAllPointPairButton->Create ( );
  this->DeleteAllPointPairButton->SetText ("Delete All Points");
  this->DeleteAllPointPairButton->SetWidth (17);
  this->DeleteAllPointPairButton->SetBalloonHelpString("Delete all point pairs.");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->DeletePointPairButton->GetWidgetName(),
              this->DeleteAllPointPairButton->GetWidgetName());

  buttonFrame->Delete();

  //-----------------------------------------------
  // Register Buttons
  //-----------------------------------------------
  // do registration
  vtkKWFrame *registerFrame = vtkKWFrame::New();
  registerFrame->SetParent ( regFrame->GetFrame() );
  registerFrame->Create ( );
  this->Script( "pack %s -padx 2 -pady 2",
                registerFrame->GetWidgetName());

  // add a register button 
  this->RegisterButton = vtkKWPushButton::New ( );
  this->RegisterButton->SetParent ( registerFrame );
  this->RegisterButton->Create ( );
  this->RegisterButton->SetText ("Register");
  this->RegisterButton->SetWidth (12);
  this->RegisterButton->SetBalloonHelpString("Perform patient to image registration.");
  
  // add a reset button 
  this->ResetButton = vtkKWPushButton::New ( );
  this->ResetButton->SetParent ( registerFrame );
  this->ResetButton->Create ( );
  this->ResetButton->SetText ("Reset");
  this->ResetButton->SetWidth (12);
  this->ResetButton->SetBalloonHelpString("Reset all values.");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->RegisterButton->GetWidgetName(),
              this->ResetButton->GetWidgetName());

  // registration status
  vtkKWFrame *regStatusFrame = vtkKWFrame::New();
  regStatusFrame->SetParent ( regFrame->GetFrame() );
  regStatusFrame->Create ( );
  this->Script( "pack %s -padx 2 -pady 2",
               regStatusFrame->GetWidgetName());

  vtkKWLabel* statusLabel = vtkKWLabel::New();
  statusLabel->SetParent(regStatusFrame);
  statusLabel->Create();
  statusLabel->SetWidth(15);
  statusLabel->SetText("Status:");

  this->RegistrationStatus = vtkKWLabel::New();
  this->RegistrationStatus->SetParent(regStatusFrame);
  this->RegistrationStatus->Create();
  this->RegistrationStatus->SetWidth(60);
  this->RegistrationStatus->SetText("No registration matrix calculated");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              statusLabel->GetWidgetName(),
              this->RegistrationStatus->GetWidgetName());

  //clean up
  registerFrame->Delete ();
  listFrame->Delete();
  pairFrame->Delete();
  regFrame->Delete();
  regStatusFrame->Delete();
  statusLabel->Delete();

}

//----------------------------------------------------------------------------
void vtkPointRegistrationGUI::UpdateAll()
{
}

