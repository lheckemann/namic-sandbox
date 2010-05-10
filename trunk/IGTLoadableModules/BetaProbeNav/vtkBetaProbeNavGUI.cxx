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
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLHybridNavToolNode.h"

#include "vtkCornerAnnotation.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"
#include "vtkKWCheckButton.h"

#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"


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
  this->ModelNode = NULL;
  this->ModelTypeButtonSet = NULL;
  this->ImageTypeButtonSet = NULL;
  this->DataTypeButtonSet = NULL;
  this->ImageCheckButton = NULL;
  this->RangeEntry = NULL;
  this->HeightEntry = NULL;
  this->REntry = NULL;
  this->AEntry = NULL;
  this->SEntry = NULL;
  this->ProbeDiameterEntry = NULL;
  this->UpdateEntry = NULL;
  this->DataEntryButtonSet = NULL;
  this->DataCaptureButton = NULL;
  this->StartTimeEntry = NULL;
  
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

  if (this->ModelNode)
    {
    this->ModelNode->SetParent(NULL);
    this->ModelNode->Delete();
    }
     
  if (this->ModelTypeButtonSet)
    {
    this->ModelTypeButtonSet->SetParent(NULL);
    this->ModelTypeButtonSet->Delete();
    }
    
  if (this->ImageTypeButtonSet)
    {
    this->ImageTypeButtonSet->SetParent(NULL);
    this->ImageTypeButtonSet->Delete();
    }
 
  if (this->DataTypeButtonSet)
    {
    this->DataTypeButtonSet->SetParent(NULL);
    this->DataTypeButtonSet->Delete();
    } 
    
  if (this->ImageCheckButton)
    {
    this->ImageCheckButton->SetParent(NULL);
    this->ImageCheckButton->Delete();
    }
       
  if (this->RangeEntry)
    {
    this->RangeEntry->SetParent(NULL);
    this->RangeEntry->Delete();
    }

  if (this->HeightEntry)
    {
    this->HeightEntry->SetParent(NULL);
    this->HeightEntry->Delete();
    }

  if (this->REntry)
    {
    this->REntry->SetParent(NULL);
    this->REntry->Delete();
    }

  if (this->AEntry)
    {
    this->AEntry->SetParent(NULL);
    this->AEntry->Delete();
    }

  if (this->SEntry)
    {
    this->SEntry->SetParent(NULL);
    this->SEntry->Delete();
    }

  if (this->ProbeDiameterEntry)
    {
    this->ProbeDiameterEntry->SetParent(NULL);
    this->ProbeDiameterEntry->Delete();
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry->SetParent(NULL);
    this->UpdateEntry->Delete();
    }
  
  if (this->DataEntryButtonSet)
    {
    this->DataEntryButtonSet->SetParent(NULL);
    this->DataEntryButtonSet->Delete();
    } 
    
  if (this->DataCaptureButton)
    {
    this->DataCaptureButton->SetParent(NULL);
    this->DataCaptureButton->Delete();
    }

  if (this->StartTimeEntry)
    {
    this->StartTimeEntry->SetParent(NULL);
    this->StartTimeEntry->Delete();
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
  
  if (this->ModelNode)
    {
    this->ModelNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RangeEntry)
    {
    this->RangeEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ProbeDiameterEntry)
    {
    this->ProbeDiameterEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->DataCaptureButton)
    {
    this->DataCaptureButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StartTimeEntry)
    {
    this->StartTimeEntry
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
  this->ModelNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ProbeDiameterEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->UpdateEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DataCaptureButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StartTimeEntry
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
    
    //Make sure Transform Node and Count Node are selected
    if (!((this->TransformNode->GetSelected()) && (this->CountNode->GetSelected())))
      {
      std::cerr << "One of the nodes was not defined" << std::endl;
      return;
      }

    //Set data type to output
    if (this->DataTypeButtonSet->GetWidget(0)->GetSelectedState())
      this->GetLogic()->SetActiveDataType(0);
    else if (this->DataTypeButtonSet->GetWidget(1)->GetSelectedState())
      this->GetLogic()->SetActiveDataType(1);
    else if (this->DataTypeButtonSet->GetWidget(2)->GetSelectedState())
      this->GetLogic()->SetActiveDataType(2);

    //Check to see the Model Representation Option
    if (!ModelTypeButtonSet->GetWidget(0)->GetSelectedState())
      {
      //Check if a model is selected
      if (!this->ModelNode->GetSelected())
        {
        std::cerr << "The model node was not defined" << std::endl;
        return;
        }
      else
        {
        //Build Locators for efficient point search performed in Logic class
        vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(this->ModelNode->GetSelected());
        mnode = this->GetLogic()->BuildLocators(mnode);
        
        //Activate the display node and provide options
        vtkMRMLModelDisplayNode* dn = vtkMRMLModelDisplayNode::SafeDownCast(mnode->GetDisplayNode());
        dn->SetActiveScalarName("Counts");
        dn->SetScalarVisibility(1);
        //TODO: put in color map node here
        dn->Modified();
        mnode->Modified();
        this->GetMRMLScene()->Modified();
        std::cerr << "Scene modified" << std::endl;
        }
      }
      //Check to see Image Representation Selection
      if (!ImageTypeButtonSet->GetWidget(0)->GetSelectedState())
        {
        //Check to see if the ScalarVolumeNode already exists
        if (this->scalnode == NULL)
          {
          std::cerr << "Image Node" << std::endl;
          //Create scalar node and display node and add to the image
          this->scalnode = vtkMRMLScalarVolumeNode::New();
          vtkMRMLScalarVolumeDisplayNode* dispNode = vtkMRMLScalarVolumeDisplayNode::New();
          this->scalnode->SetAndObserveDisplayNodeID(dispNode->GetID());
          dispNode->SetScene(this->GetMRMLScene());
          //Set Display Node Features
          std::cerr << "Image Node" << std::endl;
          dispNode->SetInterpolate(0);
          short range[2];
          range[0] = 0.0;
          range[1] = 256.0;
          dispNode->SetLowerThreshold(range[0]);
          dispNode->SetUpperThreshold(range[1]);
          dispNode->SetWindow(range[1] - range[0]);
          dispNode->SetLevel(0.5 * (range[1] + range[0]) );
          dispNode->SetInterpolate(0);
          dispNode->Modified();
          this->scalnode->Modified();
          std::cerr << "Image Node" << std::endl;
          //Add Model Node and Display Node to scene
          this->GetMRMLScene()->AddNode(scalnode);
          GetMRMLScene()->AddNode(dispNode);  
          this->GetMRMLScene()->Modified();
          dispNode->Delete();
          std::cerr << "Image Node" << std::endl;
          } 
        else
          {
          //Node is already created we just need to reset the image
          std::cerr << "Image Node" << std::endl;
          this->GetLogic()->SetImageData(NULL);
          scalnode->Modified();
          this->GetMRMLScene()->Modified();
          std::cerr << "Image Node" << std::endl;
          }
        }

      //Set the range of values for the radiation counts and probe diameter
      this->GetLogic()->SetMaxRange(this->RangeEntry->GetValueAsDouble());
      this->GetLogic()->SetProbeDiameter(this->ProbeDiameterEntry->GetValueAsDouble());

      //Start Timer to collect data points
      this->TimerInterval = this->UpdateEntry->GetValueAsInt();
      this->TimerFlag = 1;

      if (this->DataEntryButtonSet->GetWidget(1)->GetSelectedState())
        {
        this->ManualDataCapture = 0;
        ProcessTimerEvents();
        }
      else
        {
        this->ManualDataCapture = 1;
        }

      //Deactivate all the data options for selection
      //------------------------------------------------

      //ModelTypeButtonSet
      this->ModelTypeButtonSet->EnabledOff();
      this->ModelTypeButtonSet->UpdateEnableState();

      //Image Type Button Set
      this->ImageTypeButtonSet->EnabledOff();
      this->ImageTypeButtonSet->UpdateEnableState();

      //DataTypeButtonSet
      this->DataTypeButtonSet->EnabledOff();
      this->DataTypeButtonSet->UpdateEnableState();

      //Data EntryButtonSet
      this->DataEntryButtonSet->EnabledOff();
      this->DataEntryButtonSet->UpdateEnableState();

      //StartTime Text box
      this->StartTimeEntry->EnabledOff();
      this->StartTimeEntry->UpdateEnableState();

      std::cerr << "Disabled radio button sets" << std::endl;
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
    //Stop Timer to stop data point collection and representation
    this->TimerFlag = 0;

    //Reactivate all the data options for selection
    //------------------------------------------------

    //ModelTypeButtonSet
    this->ModelTypeButtonSet->EnabledOn();
    this->ModelTypeButtonSet->UpdateEnableState();

    //Image Type Button Set
    this->ImageTypeButtonSet->EnabledOn();
    this->ImageTypeButtonSet->UpdateEnableState();

    //DataTypeButtonSet
    this->DataTypeButtonSet->EnabledOn();
    this->DataTypeButtonSet->UpdateEnableState();

    //Data EntryButtonSet
    this->DataEntryButtonSet->EnabledOn();
    this->DataEntryButtonSet->UpdateEnableState();

    //StartTime Text box
    this->StartTimeEntry->EnabledOn();
    this->StartTimeEntry->UpdateEnableState();
    }
    
  //Change in the Range Entry
  else if (this->RangeEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Range has been modified." << std::endl;
    this->GetLogic()->SetMaxRange(this->RangeEntry->GetValueAsDouble());
    }
  //Change in Probe Diameter Entry
  else if (this->ProbeDiameterEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Probe Diameter has been modified." << std::endl;
    this->GetLogic()->SetProbeDiameter(this->ProbeDiameterEntry->GetValueAsDouble());
    }
  //Change in Update Rate Entry
  else if (this->RangeEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Update Rate has been modified." << std::endl;
    this->TimerInterval = this->RangeEntry->GetValueAsDouble();
    }
  //---------------------------------------
  //Data Capture Button pressed
  else if (this->DataCaptureButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Data capture button is pressed." << std::endl;
    
    if (this->DataEntryButtonSet->GetWidget(0)->GetSelectedState())
      {
      ProcessTimerEvents();
      }
    }

  //Change in the StartTime Entry
  else if (this->StartTimeEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "StartTime has been modified." << std::endl;
    this->GetLogic()->StartTimeStamp.assign(this->StartTimeEntry->GetValue());
    std::cerr << this->GetLogic()->StartTimeStamp << std::endl;
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
    //Collect position and radiation data
    if ((this->TransformNode->GetSelected()) && (this->CountNode->GetSelected()))
      {
      this->GetLogic()->CollectData(this->TransformNode->GetSelected(), this->CountNode->GetSelected());
      this->HeightEntry->SetValueAsDouble(this->GetLogic()->ProbeHeight);
      this->REntry->SetValueAsDouble(this->GetLogic()->interPoint[0]);
      this->AEntry->SetValueAsDouble(this->GetLogic()->interPoint[1]);
      this->SEntry->SetValueAsDouble(this->GetLogic()->interPoint[2]);
      }
    
    //Check to see if there is Model Representation
    if (this->ModelNode->GetSelected())
      {
      //Direct Representation
      if (this->ModelTypeButtonSet->GetWidget(1)->GetSelectedState())
        {
        vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(this->ModelNode->GetSelected());
        vtkMRMLHybridNavToolNode* tnode = vtkMRMLHybridNavToolNode::SafeDownCast(this->TransformNode->GetSelected());
        mnode = this->GetLogic()->PaintModel(mnode, tnode);
        mnode->Modified();
        this->GetMRMLScene()->Modified();
        }
      //Gaussian Splat Representation
      if (this->ModelTypeButtonSet->GetWidget(2)->GetSelectedState())
        {
        vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(this->ModelNode->GetSelected());
        vtkMRMLHybridNavToolNode* tnode = vtkMRMLHybridNavToolNode::SafeDownCast(this->TransformNode->GetSelected());
        mnode = this->GetLogic()->PaintModelGaussian(mnode, tnode);
        mnode->Modified();
        this->GetMRMLScene()->Modified();
        }
      }
    
    //Check to see if there is Image Representation
    if ((this->ImageNode->GetSelected()) && (this->ModelNode->GetSelected()))
      {
      //Paint pixel representation
      if (this->ImageTypeButtonSet->GetWidget(1)->GetSelectedState())
        {
        vtkMRMLScalarVolumeNode* inode = vtkMRMLScalarVolumeNode::SafeDownCast(this->ImageNode->GetSelected());
        vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(this->ModelNode->GetSelected());
        vtkMRMLHybridNavToolNode* tnode = vtkMRMLHybridNavToolNode::SafeDownCast(this->TransformNode->GetSelected());
        this->scalnode = this->GetLogic()->PaintImage(inode, this->scalnode, mnode, tnode);
        this->scalnode->Modified();
        this->GetMRMLScene()->Modified();
        }
      }
    
    if (this->ManualDataCapture == 0)
      {
      // update timer
      vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
      }  
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
  tnodeLabel->SetText("BetaProbe Node: ");

  this->TransformNode = vtkSlicerNodeSelectorWidget::New();
  this->TransformNode->SetParent(tnodeFrame);
  this->TransformNode->Create();
  this->TransformNode->SetWidth(30);
  this->TransformNode->SetNewNodeEnabled(0);
  this->TransformNode->SetNodeClass("vtkMRMLHybridNavToolNode", NULL, NULL, NULL);
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
  
  //Model Node Menu
  vtkKWFrame *mnodeFrame = vtkKWFrame::New();
  mnodeFrame->SetParent(frame->GetFrame());
  mnodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                mnodeFrame->GetWidgetName());

  vtkKWLabel *mnodeLabel = vtkKWLabel::New();
  mnodeLabel->SetParent(mnodeFrame);
  mnodeLabel->Create();
  mnodeLabel->SetWidth(15);
  mnodeLabel->SetText("Model Node: ");

  this->ModelNode = vtkSlicerNodeSelectorWidget::New();
  this->ModelNode->SetParent(mnodeFrame);
  this->ModelNode->Create();
  this->ModelNode->SetWidth(30);
  this->ModelNode->SetNewNodeEnabled(0);
  this->ModelNode->SetNodeClass("vtkMRMLModelNode", NULL, NULL, NULL);
  this->ModelNode->NoneEnabledOn();
  this->ModelNode->SetShowHidden(1);
  this->ModelNode->Create();
  this->ModelNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ModelNode->UpdateMenu();
  this->ModelNode->SetBorderWidth(0);
  this->ModelNode->SetBalloonHelpString("Select an model node from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              mnodeLabel->GetWidgetName() , this->ModelNode->GetWidgetName());
  
  mnodeLabel->Delete();
  mnodeFrame->Delete();

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

  //Model Representation Selection
  vtkKWFrame *modelFrame = vtkKWFrame::New();
  modelFrame->SetParent(repFrame->GetFrame());
  modelFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                modelFrame->GetWidgetName());

  vtkKWLabel *modelLabel = vtkKWLabel::New();
  modelLabel->SetParent(modelFrame);
  modelLabel->Create();
  modelLabel->SetWidth(15);
  modelLabel->SetText("Model Rep: ");

  this->ModelTypeButtonSet = vtkKWRadioButtonSet::New();
  this->ModelTypeButtonSet->SetParent(modelFrame);
  this->ModelTypeButtonSet->Create();
  this->ModelTypeButtonSet->PackHorizontallyOn();
  this->ModelTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->ModelTypeButtonSet->UniformColumnsOn();
  this->ModelTypeButtonSet->UniformRowsOn();

  this->ModelTypeButtonSet->AddWidget(0);
  this->ModelTypeButtonSet->GetWidget(0)->SetText("None");
  this->ModelTypeButtonSet->GetWidget(0)->SelectedStateOn();
  this->ModelTypeButtonSet->AddWidget(1);
  this->ModelTypeButtonSet->GetWidget(1)->SetText("Direct");
  this->ModelTypeButtonSet->AddWidget(2);
  this->ModelTypeButtonSet->GetWidget(2)->SetText("Gaussian");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              modelLabel->GetWidgetName() , this->ModelTypeButtonSet->GetWidgetName());

  //Image Representation Selection
  vtkKWFrame *imageFrame = vtkKWFrame::New();
  imageFrame->SetParent(repFrame->GetFrame());
  imageFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                imageFrame->GetWidgetName());

  vtkKWLabel *imageLabel = vtkKWLabel::New();
  imageLabel->SetParent(imageFrame);
  imageLabel->Create();
  imageLabel->SetWidth(15);
  imageLabel->SetText("Image Rep: ");

  this->ImageTypeButtonSet = vtkKWRadioButtonSet::New();
  this->ImageTypeButtonSet->SetParent(imageFrame);
  this->ImageTypeButtonSet->Create();
  this->ImageTypeButtonSet->PackHorizontallyOn();
  this->ImageTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ImageTypeButtonSet->UniformColumnsOn();
  this->ImageTypeButtonSet->UniformRowsOn();

  this->ImageTypeButtonSet->AddWidget(0);
  this->ImageTypeButtonSet->GetWidget(0)->SetText("None");
  this->ImageTypeButtonSet->GetWidget(0)->SelectedStateOn();
  this->ImageTypeButtonSet->AddWidget(1);
  this->ImageTypeButtonSet->GetWidget(1)->SetText("Pixel Fill");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              imageLabel->GetWidgetName() , this->ImageTypeButtonSet->GetWidgetName());
  
  //Data Type Selection
  vtkKWFrame *dataFrame = vtkKWFrame::New();
  dataFrame->SetParent(repFrame->GetFrame());
  dataFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                dataFrame->GetWidgetName());

  vtkKWLabel *dataLabel = vtkKWLabel::New();
  dataLabel->SetParent(dataFrame);
  dataLabel->Create();
  dataLabel->SetWidth(15);
  dataLabel->SetText("Data Type: ");

  this->DataTypeButtonSet = vtkKWRadioButtonSet::New();
  this->DataTypeButtonSet->SetParent(dataFrame);
  this->DataTypeButtonSet->Create();
  this->DataTypeButtonSet->PackHorizontallyOn();
  this->DataTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->DataTypeButtonSet->UniformColumnsOn();
  this->DataTypeButtonSet->UniformRowsOn();

  this->DataTypeButtonSet->AddWidget(0);
  this->DataTypeButtonSet->GetWidget(0)->SetText("Smoothed");
  this->DataTypeButtonSet->GetWidget(0)->SelectedStateOn();
  this->DataTypeButtonSet->AddWidget(1);
  this->DataTypeButtonSet->GetWidget(1)->SetText("Beta");
  this->DataTypeButtonSet->AddWidget(2);
  this->DataTypeButtonSet->GetWidget(2)->SetText("Gamma");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              dataLabel->GetWidgetName() , this->DataTypeButtonSet->GetWidgetName());
  
   //Clean up
  repFrame->Delete();
  modelLabel->Delete();
  modelFrame->Delete();
  imageLabel->Delete();
  imageFrame->Delete();
  dataLabel->Delete();
  dataFrame->Delete();
  
  // -----------------------------------------
  // Probe Data Frame
  
  vtkKWFrameWithLabel *probeFrame = vtkKWFrameWithLabel::New();
  probeFrame->SetParent(conBrowsFrame->GetFrame());
  probeFrame->Create();
  probeFrame->SetLabelText ("Probe Data");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 probeFrame->GetWidgetName() );
  
  //Probe Diamater Selection
  vtkKWFrame *diamFrame = vtkKWFrame::New();
  diamFrame->SetParent(probeFrame->GetFrame());
  diamFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                diamFrame->GetWidgetName());

  vtkKWLabel *diamLabel = vtkKWLabel::New();
  diamLabel->SetParent(diamFrame);
  diamLabel->Create();
  diamLabel->SetWidth(15);
  diamLabel->SetText("Diameter (mm): ");

  this->ProbeDiameterEntry = vtkKWEntry::New();
  this->ProbeDiameterEntry->SetParent(diamFrame);
  this->ProbeDiameterEntry->Create();
  this->ProbeDiameterEntry->SetWidth(8);
  this->ProbeDiameterEntry->SetRestrictValueToDouble();
  this->ProbeDiameterEntry->SetValueAsDouble(9.0);
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              diamLabel->GetWidgetName() , this->ProbeDiameterEntry->GetWidgetName());
  
  //Range Selection
  vtkKWFrame *rangeFrame = vtkKWFrame::New();
  rangeFrame->SetParent(probeFrame->GetFrame());
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
  
  //Height Display
  vtkKWFrame *heightFrame = vtkKWFrame::New();
  heightFrame->SetParent(probeFrame->GetFrame());
  heightFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                heightFrame->GetWidgetName());

  vtkKWLabel *heightLabel = vtkKWLabel::New();
  heightLabel->SetParent(heightFrame);
  heightLabel->Create();
  heightLabel->SetWidth(15);
  heightLabel->SetText("Height (mm): ");

  this->HeightEntry = vtkKWEntry::New();
  this->HeightEntry->SetParent(heightFrame);
  this->HeightEntry->Create();
  this->HeightEntry->SetWidth(8);
  this->HeightEntry->SetRestrictValueToDouble();
  this->HeightEntry->SetValueAsDouble(0.0);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              heightLabel->GetWidgetName() , this->HeightEntry->GetWidgetName());

  //Probe Intersection R Display
  vtkKWFrame *RFrame = vtkKWFrame::New();
  RFrame->SetParent(probeFrame->GetFrame());
  RFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                RFrame->GetWidgetName());

  vtkKWLabel *RLabel = vtkKWLabel::New();
  RLabel->SetParent(RFrame);
  RLabel->Create();
  RLabel->SetWidth(15);
  RLabel->SetText("Intersect R (mm): ");

  this->REntry = vtkKWEntry::New();
  this->REntry->SetParent(RFrame);
  this->REntry->Create();
  this->REntry->SetWidth(8);
  this->REntry->SetRestrictValueToDouble();
  this->REntry->SetValueAsDouble(0.0);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              RLabel->GetWidgetName() , this->REntry->GetWidgetName());

  //Probe Intersection A Display
  vtkKWFrame *AFrame = vtkKWFrame::New();
  AFrame->SetParent(probeFrame->GetFrame());
  AFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                AFrame->GetWidgetName());

  vtkKWLabel *ALabel = vtkKWLabel::New();
  ALabel->SetParent(AFrame);
  ALabel->Create();
  ALabel->SetWidth(15);
  ALabel->SetText("Intersect A (mm): ");

  this->AEntry = vtkKWEntry::New();
  this->AEntry->SetParent(AFrame);
  this->AEntry->Create();
  this->AEntry->SetWidth(8);
  this->AEntry->SetRestrictValueToDouble();
  this->AEntry->SetValueAsDouble(0.0);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              ALabel->GetWidgetName() , this->AEntry->GetWidgetName());

  //Probe Intersection S Display
  vtkKWFrame *SFrame = vtkKWFrame::New();
  SFrame->SetParent(probeFrame->GetFrame());
  SFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                SFrame->GetWidgetName());

  vtkKWLabel *SLabel = vtkKWLabel::New();
  SLabel->SetParent(SFrame);
  SLabel->Create();
  SLabel->SetWidth(15);
  SLabel->SetText("Intersect S (mm): ");

  this->SEntry = vtkKWEntry::New();
  this->SEntry->SetParent(SFrame);
  this->SEntry->Create();
  this->SEntry->SetWidth(8);
  this->SEntry->SetRestrictValueToDouble();
  this->SEntry->SetValueAsDouble(0.0);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              SLabel->GetWidgetName() , this->SEntry->GetWidgetName());

  //Clean up
  rangeFrame->Delete();
  rangeLabel->Delete();
  probeFrame->Delete();
  diamFrame->Delete();
  diamLabel->Delete();
  heightFrame->Delete();
  heightLabel->Delete();
  RFrame->Delete();
  RLabel->Delete();
  AFrame->Delete();
  ALabel->Delete();
  SFrame->Delete();
  SLabel->Delete();
  
  // -----------------------------------------
  // Data Entry Frame

  vtkKWFrameWithLabel *entryFrame = vtkKWFrameWithLabel::New();
  entryFrame->SetParent(conBrowsFrame->GetFrame());
  entryFrame->Create();
  entryFrame->SetLabelText ("Data Entry Options");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 entryFrame->GetWidgetName() );

  // Sphere Representation Selection
  vtkKWFrame *optionFrame = vtkKWFrame::New();
  optionFrame->SetParent(entryFrame->GetFrame());
  optionFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                optionFrame->GetWidgetName());

  vtkKWLabel *optionLabel = vtkKWLabel::New();
  optionLabel->SetParent(optionFrame);
  optionLabel->Create();
  optionLabel->SetWidth(15);
  optionLabel->SetText("Data Collection: ");

  this->DataEntryButtonSet = vtkKWRadioButtonSet::New();
  this->DataEntryButtonSet->SetParent(optionFrame);
  this->DataEntryButtonSet->Create();
  this->DataEntryButtonSet->PackHorizontallyOn();
  this->DataEntryButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->DataEntryButtonSet->UniformColumnsOn();
  this->DataEntryButtonSet->UniformRowsOn();

  this->DataEntryButtonSet->AddWidget(0);
  this->DataEntryButtonSet->GetWidget(0)->SetText("Manual");
  this->DataEntryButtonSet->GetWidget(0)->SelectedStateOn();
  this->DataEntryButtonSet->AddWidget(1);
  this->DataEntryButtonSet->GetWidget(1)->SetText("Timer");
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              optionLabel->GetWidgetName() , this->DataEntryButtonSet->GetWidgetName());
  
  //Capture button
  vtkKWFrame *captureFrame = vtkKWFrame::New();
  captureFrame->SetParent(entryFrame->GetFrame());
  captureFrame->Create();
  app->Script ( "pack %s -padx 2 -pady 2",
                captureFrame->GetWidgetName());
  
  this->DataCaptureButton = vtkKWPushButton::New( );
  this->DataCaptureButton ->SetParent(captureFrame);
  this->DataCaptureButton ->Create( );
  this->DataCaptureButton ->SetText("Capture");
  this->DataCaptureButton ->SetWidth(12);

  app->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2",
              this->DataCaptureButton->GetWidgetName());
  
  //Clean up
  optionFrame->Delete();
  optionLabel->Delete();
  entryFrame->Delete();
  captureFrame->Delete();
  
  //-----------------------------------------
  // Correction Options

   vtkKWFrameWithLabel *corrFrame = vtkKWFrameWithLabel::New();
   corrFrame->SetParent(conBrowsFrame->GetFrame());
   corrFrame->Create();
   corrFrame->SetLabelText ("Correction Options");
   this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                  corrFrame->GetWidgetName() );

   //Start Time Entry
   vtkKWFrame *startTimeFrame = vtkKWFrame::New();
   startTimeFrame->SetParent(corrFrame->GetFrame());
   startTimeFrame->Create();
   app->Script ( "pack %s -fill both -expand true",
                startTimeFrame->GetWidgetName());

   vtkKWLabel *startTimeLabel = vtkKWLabel::New();
   startTimeLabel->SetParent(startTimeFrame);
   startTimeLabel->Create();
   startTimeLabel->SetWidth(25);
   startTimeLabel->SetText("Start Time (hh:mm:ss):");

   this->StartTimeEntry = vtkKWEntry::New();
   this->StartTimeEntry->SetParent(startTimeFrame);
   this->StartTimeEntry->Create();
   this->StartTimeEntry->SetWidth(8);
   this->StartTimeEntry->SetRestrictValueToNone();
   this->StartTimeEntry->SetValue("00:00:00");

   app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                startTimeLabel->GetWidgetName() , this->StartTimeEntry->GetWidgetName());

   //Clean up
   startTimeFrame->Delete();
   startTimeLabel->Delete();
   corrFrame->Delete();

  conBrowsFrame->Delete();

}

//----------------------------------------------------------------------------
void vtkBetaProbeNavGUI::UpdateAll()
{
}

