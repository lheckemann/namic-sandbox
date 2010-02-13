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

#include "vtkBetaProbeNavGUI.h"
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
#include "vtkKWEntry.h"

#include "vtkKWPushButton.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkCornerAnnotation.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkBetaProbeNavGUI );
vtkCxxRevisionMacro ( vtkBetaProbeNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkBetaProbeNavGUI::vtkBetaProbeNavGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeNavGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->StartButton = NULL;
  this->StopButton = NULL;
  this->TransformNode = NULL;
  this->CountNode = NULL;
  this->ImageNode = NULL;
  this->SphereTypeButtonSet = NULL;
  this->RangeEntry = NULL;
  this->UpdateEntry = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
  this->TimerInterval = 1000; //ms
  
  //----------------------------------------------------------------
  // MRML
  //mnode = vtkMRMLModelNode::New();

}

//---------------------------------------------------------------------------
vtkBetaProbeNavGUI::~vtkBetaProbeNavGUI ( )
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

  if (this->StartButton)
    {
    this->StartButton->SetParent(NULL);
    this->StartButton->Delete();
    }

  if (this->StopButton)
    {
    this->StopButton->SetParent(NULL);
    this->StopButton->Delete();
    }

  if (this->TransformNode)
    {
    this->TransformNode->SetParent(NULL);
    this->TransformNode->Delete();
    }

  if (this->CountNode)
    {
    this->CountNode->SetParent(NULL);
    this->CountNode->Delete();
    }
    
  if (this->ImageNode)
    {
    this->ImageNode->SetParent(NULL);
    this->ImageNode->Delete();
    }
    
  if (this->SphereTypeButtonSet)
    {
    this->SphereTypeButtonSet->SetParent(NULL);
    this->SphereTypeButtonSet->Delete();
    }

  if (this->RangeEntry)
    {
    this->RangeEntry->SetParent(NULL);
    this->RangeEntry->Delete();
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry->SetParent(NULL);
    this->UpdateEntry->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Enter()
{
  // Fill in

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "BetaProbeNavGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->StartButton)
    {
    this->StartButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopButton)
    {
    this->StopButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TransformNode)
    {
    this->TransformNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CountNode)
    {
    this->CountNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ImageNode)
    {
    this->ImageNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RangeEntry)
    {
    this->RangeEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::AddGUIObservers ( )
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

  this->StartButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StopButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TransformNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->CountNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ImageNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->RangeEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->UpdateEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  
  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkBetaProbeNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //---------------------------------------
  //Start Button Pressed
  //---------------------------------------
  if (this->StartButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Start button is pressed." << std::endl;
    
    //Make sure Transform Node, Count Node and Image Node is selected
    if (!((TransformNode) && (CountNode) && (ImageNode)))
      {
      std::cerr << "One of the nodes was not defined" << std::endl;
      return;
      }
    
    //Check to see if Spheres is selected
    if (!SphereTypeButtonSet->GetWidget(0)->GetSelectedState())
      {
      //TODO: disable the spheres option while calculating
      if (this->mnode == NULL)
        {
        //Create a model node and add to scene
        mnode = vtkMRMLModelNode::New();       
        mnode->Modified();
        this->GetMRMLScene()->AddNode(mnode);
        this->GetMRMLScene()->Modified();
        }
      else
        {
        //Model is already created but we need to reset the display node
        vtkMRMLDisplayNode* dnode = mnode->GetDisplayNode();
        if (dnode)
          this->GetMRMLScene()->RemoveNode(dnode);
        mnode->Modified();
        this->GetMRMLScene()->Modified();
        //Reset all the point and scalar arrays
        this->GetLogic()->ClearArrays();
        }
      }

      //Check to see if the ScalarVolumeNode already exists
      if (this->scalnode == NULL)
        {
        //Create scalar node and add to the image
        scalnode = vtkMRMLScalarVolumeNode::New();       
        scalnode->Modified();
        this->GetMRMLScene()->AddNode(scalnode);
        this->GetMRMLScene()->Modified();
        } 
      else
        {
        //Node is already created we just need to reset the image
        vtkMRMLDisplayNode* scalDisp = scalnode->GetDisplayNode();
        if (scalDisp)
           this->GetMRMLScene()->RemoveNode(scalDisp);
        //Reset image data
        this->GetLogic()->SetImageData(NULL);
        scalnode->Modified();
        this->GetMRMLScene()->Modified();
        }
        
      //Set the range of values for the radiation counts
      this->GetLogic()->SetMaxRange(this->RangeEntry->GetValueAsDouble());
      //Start Timer to collect data points
      this->TimerFlag = 1;
      this->TimerInterval = this->UpdateEntry->GetValueAsInt();
      ProcessTimerEvents();
      }
  
  //---------------------------------------
  //Stop Button Pressed
  //---------------------------------------    
  else if (this->StopButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Stop button is pressed." << std::endl;
    //TODO: Disable the selection checkboxes of spheres
    //TODO: Save data to file
    if ((TransformNode) && (CountNode) && (ImageNode))
      {
      //Stop Timer to stop data point collection
      this->TimerFlag = 0;
      
      //Represent the spheres if post view was selected
      if (SphereTypeButtonSet->GetWidget(2)->GetSelectedState())
        {
        if (mnode)
          {
          mnode = this->GetLogic()->RepresentData(mnode);
          mnode->Modified();
          this->GetMRMLScene()->Modified();
          }
        }
      }
    }
    
  else if (this->TransformNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Transform node menu is selected." << std::endl;
    }
    
  else if (this->CountNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Count Node menu is selected." << std::endl;
    }
    
  else if (this->ImageNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Image Node menu is selected." << std::endl;
    }
  //Change in the Range Entry
  else if (this->RangeEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Range has been modified." << std::endl;
    this->GetLogic()->SetMaxRange(this->RangeEntry->GetValueAsDouble());
    }
  //Change in Update Rate Entry
  else if (this->RangeEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Update Rate has been modified." << std::endl;
    this->TimerInterval = this->RangeEntry->GetValueAsDouble();
    }

} 

//--------------------------------------------------------------------------
void vtkBetaProbeNavGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeNavGUI *self = reinterpret_cast<vtkBetaProbeNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeNavGUI DataCallback");
  self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkBetaProbeNavLogic::SafeDownCast(caller))
    {
    if (event == vtkBetaProbeNavLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    if ((this->TransformNode) && (this->CountNode) && (this->ImageNode))
      {
      this->GetLogic()->CollectData(this->TransformNode->GetSelected(), this->CountNode->GetSelected());
      if (this->SphereTypeButtonSet->GetWidget(1)->GetSelectedState())
        {
        mnode = this->GetLogic()->RepresentDataRT(mnode);
        }
      scalnode = this->GetLogic()->PaintImage(this->ImageNode->GetSelected(), scalnode);
      }
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "BetaProbeNav", "BetaProbeNav", NULL );

  BuildGUIForImportDataFrame();

}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:BetaProbeNav</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "BetaProbeNav" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUIForImportDataFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbeNav");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Import Data Frame");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Data Selection Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Data Selection frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  //Transform Node Menu
  vtkKWFrame *tnodeFrame = vtkKWFrame::New();
  tnodeFrame->SetParent(frame->GetFrame());
  tnodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                tnodeFrame->GetWidgetName());

  vtkKWLabel *tnodeLabel = vtkKWLabel::New();
  tnodeLabel->SetParent(tnodeFrame);
  tnodeLabel->Create();
  tnodeLabel->SetWidth(15);
  tnodeLabel->SetText("Transform Node: ");

  this->TransformNode = vtkSlicerNodeSelectorWidget::New();
  this->TransformNode->SetParent(tnodeFrame);
  this->TransformNode->Create();
  this->TransformNode->SetWidth(30);
  this->TransformNode->SetNewNodeEnabled(0);
  this->TransformNode->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->TransformNode->NoneEnabledOn();
  this->TransformNode->SetShowHidden(1);
  this->TransformNode->Create();
  this->TransformNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->TransformNode->UpdateMenu();
  this->TransformNode->SetBorderWidth(0);
  this->TransformNode->SetBalloonHelpString("Select a transform from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              tnodeLabel->GetWidgetName() , this->TransformNode->GetWidgetName());
  
  tnodeLabel->Delete();
  tnodeFrame->Delete();
  
  //Count Node Menu
  vtkKWFrame *cnodeFrame = vtkKWFrame::New();
  cnodeFrame->SetParent(frame->GetFrame());
  cnodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                cnodeFrame->GetWidgetName());

  vtkKWLabel *cnodeLabel = vtkKWLabel::New();
  cnodeLabel->SetParent(cnodeFrame);
  cnodeLabel->Create();
  cnodeLabel->SetWidth(15);
  cnodeLabel->SetText("Counts Node: ");

  this->CountNode = vtkSlicerNodeSelectorWidget::New();
  this->CountNode->SetParent(cnodeFrame);
  this->CountNode->Create();
  this->CountNode->SetWidth(30);
  this->CountNode->SetNewNodeEnabled(0);
  this->CountNode->SetNodeClass("vtkMRMLUDPServerNode", NULL, NULL, NULL);
  this->CountNode->NoneEnabledOn();
  this->CountNode->SetShowHidden(1);
  this->CountNode->Create();
  this->CountNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->CountNode->UpdateMenu();
  this->CountNode->SetBorderWidth(0);
  this->CountNode->SetBalloonHelpString("Select a count node from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              cnodeLabel->GetWidgetName() , this->CountNode->GetWidgetName());
  
  cnodeLabel->Delete();
  cnodeFrame->Delete();
  
  //Image Node Menu
  vtkKWFrame *inodeFrame = vtkKWFrame::New();
  inodeFrame->SetParent(frame->GetFrame());
  inodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                inodeFrame->GetWidgetName());

  vtkKWLabel *inodeLabel = vtkKWLabel::New();
  inodeLabel->SetParent(inodeFrame);
  inodeLabel->Create();
  inodeLabel->SetWidth(15);
  inodeLabel->SetText("Image Node: ");

  this->ImageNode = vtkSlicerNodeSelectorWidget::New();
  this->ImageNode->SetParent(inodeFrame);
  this->ImageNode->Create();
  this->ImageNode->SetWidth(30);
  this->ImageNode->SetNewNodeEnabled(0);
  this->ImageNode->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->ImageNode->NoneEnabledOn();
  this->ImageNode->SetShowHidden(1);
  this->ImageNode->Create();
  this->ImageNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ImageNode->UpdateMenu();
  this->ImageNode->SetBorderWidth(0);
  this->ImageNode->SetBalloonHelpString("Select an image node from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              inodeLabel->GetWidgetName() , this->ImageNode->GetWidgetName());
  
  inodeLabel->Delete();
  inodeFrame->Delete();
  //Update Rate Entry text box
  vtkKWFrame *updateFrame = vtkKWFrame::New();
  updateFrame->SetParent(frame->GetFrame());
  updateFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                updateFrame->GetWidgetName());

  vtkKWLabel *updateLabel = vtkKWLabel::New();
  updateLabel->SetParent(updateFrame);
  updateLabel->Create();
  updateLabel->SetWidth(15);
  updateLabel->SetText("Update Rate (ms): ");

  this->UpdateEntry = vtkKWEntry::New();
  this->UpdateEntry->SetParent(updateFrame);
  this->UpdateEntry->Create();
  this->UpdateEntry->SetWidth(8);
  this->UpdateEntry->SetRestrictValueToInteger();
  this->UpdateEntry->SetValueAsInt(500);
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              updateLabel->GetWidgetName() , this->UpdateEntry->GetWidgetName());
  
  //Start button
  vtkKWFrame *startFrame = vtkKWFrame::New();
  startFrame->SetParent(frame->GetFrame());
  startFrame->Create();
  app->Script ( "pack %s -padx 2 -pady 2",
                startFrame->GetWidgetName());
  
  this->StartButton = vtkKWPushButton::New( );
  this->StartButton ->SetParent(startFrame);
  this->StartButton ->Create( );
  this->StartButton ->SetText("Start");
  this->StartButton ->SetWidth(12);

  //Stop Button
  this->StopButton = vtkKWPushButton::New( );
  this->StopButton ->SetParent(startFrame);
  this->StopButton ->Create( );
  this->StopButton ->SetText("Stop");
  this->StopButton ->SetWidth(12);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              this->StartButton->GetWidgetName(),
              this->StopButton->GetWidgetName());

  //Clean up
  startFrame->Delete();
  frame->Delete();
  updateLabel->Delete();
  updateFrame->Delete();

  // -----------------------------------------
  // Data Representation Frame

  vtkKWFrameWithLabel *repFrame = vtkKWFrameWithLabel::New();
  repFrame->SetParent(conBrowsFrame->GetFrame());
  repFrame->Create();
  repFrame->SetLabelText ("Data Representation");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 repFrame->GetWidgetName() );

  // Sphere Representation Selection
  vtkKWFrame *sphereFrame = vtkKWFrame::New();
  sphereFrame->SetParent(repFrame->GetFrame());
  sphereFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                sphereFrame->GetWidgetName());

  vtkKWLabel *sphereLabel = vtkKWLabel::New();
  sphereLabel->SetParent(sphereFrame);
  sphereLabel->Create();
  sphereLabel->SetWidth(8);
  sphereLabel->SetText("Spheres: ");

  this->SphereTypeButtonSet = vtkKWRadioButtonSet::New();
  this->SphereTypeButtonSet->SetParent(sphereFrame);
  this->SphereTypeButtonSet->Create();
  this->SphereTypeButtonSet->PackHorizontallyOn();
  this->SphereTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->SphereTypeButtonSet->UniformColumnsOn();
  this->SphereTypeButtonSet->UniformRowsOn();

  this->SphereTypeButtonSet->AddWidget(0);
  this->SphereTypeButtonSet->GetWidget(0)->SetText("None");
  this->SphereTypeButtonSet->GetWidget(0)->SelectedStateOn();
  this->SphereTypeButtonSet->AddWidget(1);
  this->SphereTypeButtonSet->GetWidget(1)->SetText("RT");
  this->SphereTypeButtonSet->AddWidget(2);
  this->SphereTypeButtonSet->GetWidget(2)->SetText("Post");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              sphereLabel->GetWidgetName() , this->SphereTypeButtonSet->GetWidgetName());

  //Range Selection
  vtkKWFrame *rangeFrame = vtkKWFrame::New();
  rangeFrame->SetParent(repFrame->GetFrame());
  rangeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                rangeFrame->GetWidgetName());

  vtkKWLabel *rangeLabel = vtkKWLabel::New();
  rangeLabel->SetParent(rangeFrame);
  rangeLabel->Create();
  rangeLabel->SetWidth(15);
  rangeLabel->SetText("Range (max): ");

  this->RangeEntry = vtkKWEntry::New();
  this->RangeEntry->SetParent(rangeFrame);
  this->RangeEntry->Create();
  this->RangeEntry->SetWidth(8);
  this->RangeEntry->SetRestrictValueToInteger();
  this->RangeEntry->SetValueAsInt(255);
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              rangeLabel->GetWidgetName() , this->RangeEntry->GetWidgetName());
  
   //Clean up
  repFrame->Delete();
  sphereFrame->Delete();
  sphereLabel->Delete();
  rangeFrame->Delete();
  rangeLabel->Delete();
  
  conBrowsFrame->Delete();

}

//----------------------------------------------------------------------------
void vtkBetaProbeNavGUI::UpdateAll()
{
}

