/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include <sstream>

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkBetaProbeGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkCornerAnnotation.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"
#include "vtkKWEntry.h"
#include "vtkKWRadioButton.h"
#include "vtkKWPushButton.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLabelMapVolumeDisplayNode.h"
#include "vtkMatrix4x4.h"
#include "vtkCollection.h"
#include "vtkLookupTable.h"

// Type of counts to use
// 1: Beta, 2: Gamma, 3: Smoothed
#define COUNTS_TYPE 1

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkBetaProbeGUI );
vtkCxxRevisionMacro ( vtkBetaProbeGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkBetaProbeGUI::vtkBetaProbeGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  this->Capture        = NULL;
  this->Start_Button   = NULL;
  this->Stop_Button    = NULL;
  this->CounterNode    = NULL;
  this->TrackerNode    = NULL;
  this->Capture_status = NULL;
  this->FileSelector   = NULL;
  this->SelectFile     = NULL;
  this->CloseFile      = NULL;

  this->Counts         = NULL;
  this->Probe_Position = NULL;
  this->Probe_Matrix   = NULL;

  this->StartPivotCalibration   = NULL;
  this->StopPivotCalibration    = NULL;
  this->PivotCalibrationRunning = false;

  this->PivotingMatrix = vtkCollection::New();

  this->RadioBackgroundButton  = NULL;
  this->BackgroundValueEntry   = NULL;
  this->BackgroundAcceptButton = NULL;
  this->RadioTumorButton       = NULL;
  this->TumorValueEntry        = NULL;
  this->TumorAcceptButton      = NULL;
  this->RadioThresholdButton   = NULL;
  this->ThresholdValueEntry    = NULL;
  this->StartDetectionButton   = NULL;

  this->ThresholdTumorDetection = -1;

  this->BackgroundAccepted = false;
  this->TumorAccepted      = false;

  this->DataSelector  = NULL;
  this->DataToMap     = NULL;
  this->MappingButton = NULL;
  this->LabelStatus   = NULL;

  this->SetContinuousMode(false);


  this->TestButton = NULL;


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
}

//---------------------------------------------------------------------------
vtkBetaProbeGUI::~vtkBetaProbeGUI ( )
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

  if (this->Capture)
    {
    this->Capture->SetParent(NULL);
    this->Capture->Delete();
    }

  if (this->Start_Button)
    {
    this->Start_Button->SetParent(NULL);
    this->Start_Button->Delete();
    }

  if (this->Stop_Button)
    {
    this->Stop_Button->SetParent(NULL);
    this->Stop_Button->Delete();
    }

  if (this->CounterNode)
    {
    this->CounterNode->SetParent(NULL);
    this->CounterNode->Delete();
    }

  if (this->TrackerNode)
    {
    this->TrackerNode->SetParent(NULL);
    this->TrackerNode->Delete();
    }

  if (this->Capture_status)
    {
    this->Capture_status->SetParent(NULL);
    this->Capture_status->Delete();
    }

  if (this->FileSelector)
    {
    this->FileSelector->SetParent(NULL);
    this->FileSelector->Delete();
    }

  if (this->SelectFile)
    {
    this->SelectFile->SetParent(NULL);
    this->SelectFile->Delete();
    }

  if (this->CloseFile)
    {
    this->CloseFile->SetParent(NULL);
    this->CloseFile->Delete();
    }

  if(this->Probe_Matrix)
    {
    this->Probe_Matrix->Delete();
    }


  if(this->StartPivotCalibration)
    {
    this->StartPivotCalibration->SetParent(NULL);
    this->StartPivotCalibration->Delete();
    }

  if(this->StopPivotCalibration)
    {
    this->StopPivotCalibration->SetParent(NULL);
    this->StopPivotCalibration->Delete();
    }

  if(this->PivotingMatrix)
    {
    this->PivotingMatrix->Delete();
    }


  if (this->RadioBackgroundButton)
    {
    this->RadioBackgroundButton->SetParent(NULL);
    this->RadioBackgroundButton->Delete();
    }
  if (this->BackgroundValueEntry)
    {
    this->BackgroundValueEntry->SetParent(NULL);
    this->BackgroundValueEntry->Delete();
    }
  if (this->BackgroundAcceptButton)
    {
    this->BackgroundAcceptButton->SetParent(NULL);
    this->BackgroundAcceptButton->Delete();
    }


  if (this->RadioTumorButton)
    {
    this->RadioTumorButton->SetParent(NULL);
    this->RadioTumorButton->Delete();
    }
  if (this->TumorValueEntry)
    {
    this->TumorValueEntry->SetParent(NULL);
    this->TumorValueEntry->Delete();
    }
  if (this->TumorAcceptButton)
    {
    this->TumorAcceptButton->SetParent(NULL);
    this->TumorAcceptButton->Delete();
    }


  if (this->RadioThresholdButton)
    {
    this->RadioThresholdButton->SetParent(NULL);
    this->RadioThresholdButton->Delete();
    }
  if (this->ThresholdValueEntry)
    {
    this->ThresholdValueEntry->SetParent(NULL);
    this->ThresholdValueEntry->Delete();
    }

  if(this->StartDetectionButton)
    {
    this->StartDetectionButton->SetParent(NULL);
    this->StartDetectionButton->Delete();
    }


  if(this->DataSelector)
    {
    this->DataSelector->SetParent(NULL);
    this->DataSelector->Delete();
    }



  if(this->MappingButton)
    {
    this->MappingButton->SetParent(NULL);
    this->MappingButton->Delete();
    }

  if(this->LabelStatus)
    {
    this->LabelStatus->SetParent(NULL);
    this->LabelStatus->Delete();
    }


  if(this->TestButton)
    {
    this->TestButton->SetParent(NULL);
    this->TestButton->Delete();
    }




  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

  if(this->BetaProbeCountsWithTimestamp.is_open())
    {
    this->BetaProbeCountsWithTimestamp.close();
    }
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::Enter()
{

  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 1000;  // 1000 ms

    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "BetaProbeGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::RemoveGUIObservers ( )
{

  if (this->Capture)
    {
    this->Capture
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->Start_Button)
    {
    this->Start_Button
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->Stop_Button)
    {
    this->Stop_Button
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CounterNode)
    {
    this->CounterNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TrackerNode)
    {
    this->TrackerNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->FileSelector)
    {
    this->FileSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SelectFile)
    {
    this->SelectFile
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CloseFile)
    {
    this->CloseFile
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }



  if (this->StartPivotCalibration)
    {
    this->StartPivotCalibration
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopPivotCalibration)
    {
    this->StopPivotCalibration
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  if (this->RadioBackgroundButton)
    {
    this->RadioBackgroundButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundValueEntry)
    {
    this->BackgroundValueEntry->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundAcceptButton)
    {
    this->BackgroundAcceptButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RadioTumorButton)
    {
    this->RadioTumorButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TumorValueEntry)
    {
    this->TumorValueEntry->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TumorAcceptButton)
    {
    this->TumorAcceptButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RadioThresholdButton)
    {
    this->RadioThresholdButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdValueEntry)
    {
    this->ThresholdValueEntry->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if(this->StartDetectionButton)
    {
    this->StartDetectionButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }



  if(this->DataSelector)
    {
    this->DataSelector->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->MappingButton)
    {
    this->MappingButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  if(this->TestButton)
    {
    this->TestButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers
  if(this->Capture)
    {
    this->Capture
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->Start_Button)
    {
    this->Start_Button
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->Stop_Button)
    {
    this->Stop_Button
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->CounterNode)
    {
    this->CounterNode
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->TrackerNode)
    {
    this->TrackerNode
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->FileSelector)
    {
    this->FileSelector
      ->AddObserver(vtkKWFileBrowserDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->SelectFile)
    {
    this->SelectFile
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->CloseFile)
    {
    this->CloseFile
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }



  if(this->StartPivotCalibration)
    {
    this->StartPivotCalibration
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->StopPivotCalibration)
    {
    this->StopPivotCalibration
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->RadioBackgroundButton->AddObserver ( vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->BackgroundValueEntry->AddObserver ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->BackgroundAcceptButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->RadioTumorButton->AddObserver ( vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->TumorValueEntry->AddObserver ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->TumorAcceptButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->RadioThresholdButton->AddObserver ( vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->ThresholdValueEntry->AddObserver ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->StartDetectionButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->DataSelector->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->MappingButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );


  this->TestButton->AddObserver ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkBetaProbeGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkBetaProbeLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::ProcessGUIEvents(vtkObject *caller,
                                       unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }


  if (this->Capture == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->Counts && this->Probe_Position)
      {
      this->Capture_Data();
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }
    }

  if (this->Start_Button == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->Counts && this->Probe_Position)
      {
      // Synchronize UDPServerNode and BetaProbe timers
      this->TimerInterval = this->Counts->GetTInterval();

      this->SetContinuousMode(true);
      this->Capture->SetState(0);
      this->Start_Button->SetState(0);
      this->Stop_Button->SetState(1);

      this->StartPivotCalibration->SetState(0);
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }
    }

  if (this->Stop_Button == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->Counts && this->Probe_Position)
      {
      this->SetContinuousMode(false);
      this->Capture->SetState(1);
      this->Start_Button->SetState(1);
      this->Stop_Button->SetState(0);

      this->StartPivotCalibration->SetState(1);
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }
    }

  if (this->SelectFile == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    this->FileSelector = vtkKWFileBrowserDialog::New();
    this->FileSelector->SetApplication(this->GetApplication());
    this->FileSelector->Create();
    this->FileSelector->SaveDialogOn();
    this->FileSelector->Invoke();

    if (this->FileSelector->GetStatus()==vtkKWDialog::StatusOK)
      {
      std::string filename = this->FileSelector->GetFileName();
      if(this->BetaProbeCountsWithTimestamp.is_open())
        {
        this->BetaProbeCountsWithTimestamp.close();
        }
      this->BetaProbeCountsWithTimestamp.open(filename.c_str());
      if(this->BetaProbeCountsWithTimestamp.is_open())
        {
        std::stringstream open_file_succeed;
        open_file_succeed << "File opened [" << filename.c_str() << "]";
        this->Capture_status->SetText(open_file_succeed.str().c_str());
        }
      else
        {
        this->Capture_status->SetText("Failed to open file");
        }
      }
    }

  if (this->CloseFile == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->BetaProbeCountsWithTimestamp.is_open())
      {
      std::string filename_c = this->FileSelector->GetFileName();
      this->BetaProbeCountsWithTimestamp.close();
      std::stringstream close_file_succeed;
      close_file_succeed << "File closed [" << filename_c.c_str() << "]";
      this->Capture_status->SetText(close_file_succeed.str().c_str());
      }
    if(this->FileSelector)
      {
      this->FileSelector->Delete();
      this->FileSelector = NULL;
      }
    }

  if (this->CounterNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->CounterNode->GetSelected())
      {
      this->Counts = vtkMRMLUDPServerNode::SafeDownCast(this->CounterNode->GetSelected());

      if(this->Counts)
        {
        // Synchronize UDPServerNode and BetaProbe timers
        this->TimerInterval = this->Counts->GetTInterval();
        }

      std::stringstream counter_selected;
      counter_selected << this->CounterNode->GetSelected()->GetName();
      counter_selected << " selected";
      this->Capture_status->SetText(counter_selected.str().c_str());
      }
    }

  if (this->TrackerNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->TrackerNode->GetSelected())
      {
      this->Probe_Position = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerNode->GetSelected());
      std::stringstream tracker_selected;
      tracker_selected << this->TrackerNode->GetSelected()->GetName();
      tracker_selected << " selected";
      this->Capture_status->SetText(tracker_selected.str().c_str());
      }
    }


  // ******** PIVOT CALIBRATION *********

  if (this->StartPivotCalibration == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->PivotingMatrix)
      {
      this->PivotingMatrix->RemoveAllItems();

      if(this->Probe_Position && (this->GetPivotCalibrationRunning()==false))
        {
        this->StartPivotCalibration->SetState(0);
        this->StopPivotCalibration->SetState(1);
        this->SetPivotCalibrationRunning(true);

        this->Start_Button->SetState(0);
        this->Capture->SetState(0);
        }
      }
    }

  if (this->StopPivotCalibration == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->GetPivotCalibrationRunning())
      {
      this->StartPivotCalibration->SetState(1);
      this->StopPivotCalibration->SetState(0);
      this->SetPivotCalibrationRunning(false);

      this->Start_Button->SetState(1);
      this->Capture->SetState(1);
      // Start Pivot Calibration Here
      if(this->PivotingMatrix)
        {
        // Get the Average Pcal vector
        double pcal[3];
        this->GetLogic()->PivotCalibration(this->PivotingMatrix,pcal);

        std::cout << "Pcal : (" << pcal[0] << "," << pcal[1] << "," << pcal[2] << ")" << std::endl;
        }

      // Deleting all matrixes in the vtkCollection and all references to these matrixes
      for(int i=0; i<this->PivotingMatrix->GetNumberOfItems();i++)
        {
        this->PivotingMatrix->GetItemAsObject(i)->Delete();
        }
      this->PivotingMatrix->RemoveAllItems();

      // To avoid a double free when the destructor is called
      this->Probe_Matrix = NULL;
      }
    }

  // Background Value
  if(this->RadioBackgroundButton == vtkKWRadioButton::SafeDownCast(caller)
     && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    }

  if(this->BackgroundValueEntry == vtkKWEntry::SafeDownCast(caller)
     && event == vtkKWEntry::EntryValueChangedEvent)
    {
    if(this->BackgroundValueEntry->GetValueAsInt() >= 0 && this->TumorValueEntry->GetValueAsInt() >= 0)
      {
      if(this->ThresholdValueEntry)
        {
        int DetectionThreshold = (this->BackgroundValueEntry->GetValueAsInt() + this->TumorValueEntry->GetValueAsInt()) / 2.0;
        this->ThresholdValueEntry->SetValueAsInt(DetectionThreshold);
        }
      }
    }

  if(this->BackgroundAcceptButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->RadioBackgroundButton->GetSelectedState())
      {
      if(this->BackgroundAccepted)
        {
        this->BackgroundAccepted = false;
        this->BackgroundAcceptButton->SetText("Accept");
        }
      else
        {
        this->BackgroundAccepted = true;
        this->BackgroundAcceptButton->SetText("Reset");
        }
      }
    }

  // Tumor Value
  if(this->RadioTumorButton == vtkKWRadioButton::SafeDownCast(caller)
     && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    }

  if(this->TumorValueEntry == vtkKWEntry::SafeDownCast(caller)
     && event == vtkKWEntry::EntryValueChangedEvent)
    {
    if(this->BackgroundValueEntry->GetValueAsInt() && this->TumorValueEntry->GetValueAsInt())
      {
      if(this->ThresholdValueEntry)
        {
        int DetectionThreshold = (this->BackgroundValueEntry->GetValueAsInt() + this->TumorValueEntry->GetValueAsInt()) / 2.0;
        this->ThresholdValueEntry->SetValueAsInt(DetectionThreshold);
        }
      }
    }

  if(this->TumorAcceptButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->RadioTumorButton->GetSelectedState())
      {
      if(this->TumorAccepted)
        {
        this->TumorAccepted = false;
        this->TumorAcceptButton->SetText("Accept");
        }
      else
        {
        this->TumorAccepted = true;
        this->TumorAcceptButton->SetText("Reset");
        }
      }
    }

  // Threshold Value
  if(this->RadioThresholdButton == vtkKWRadioButton::SafeDownCast(caller)
     && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    }

  if(this->ThresholdValueEntry == vtkKWEntry::SafeDownCast(caller)
     && event == vtkKWEntry::EntryValueChangedEvent)
    {
    this->ThresholdTumorDetection = this->ThresholdValueEntry->GetValueAsInt();
    }

  if(this->StartDetectionButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->RadioThresholdButton->GetSelectedState())
      {
      if(this->GetLogic()->GetDetectionRunning())
        {
        this->GetLogic()->SetDetectionRunning(false);
        this->StartDetectionButton->SetText("Start Detection");
        this->GetLogic()->StopTumorDetection();
        }
      else
        {
        this->GetLogic()->SetDetectionRunning(true);
        this->StartDetectionButton->SetText("Stop Detection");
        if(this->ThresholdTumorDetection > 0 && this->Counts)
          {
          this->GetLogic()->SetUDPServerNode(this->Counts);
          this->GetLogic()->StartTumorDetection(this->ThresholdTumorDetection);
          }
        }
      }
    }


  if(this->DataSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
     event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->DataToMap = vtkMRMLScalarVolumeNode::SafeDownCast(this->DataSelector->GetSelected());
    }


  if(this->MappingButton == vtkKWPushButton::SafeDownCast(caller) &&
     event == vtkKWPushButton::InvokedEvent)
    {
    if(this->Probe_Position &&
       this->Counts &&
       this->DataToMap)
      {

      if(!this->GetLogic()->GetMappingRunning())
        {
        // Color
        vtkMRMLColorTableNode *colorNode = vtkMRMLColorTableNode::New();
        colorNode->SetTypeToUser();
        colorNode->SetNumberOfColors(512);
        //colorNode->GetLookupTable()->SetNumberOfColors(512);
        //colorNode->GetLookupTable()->SetNumberOfTableValues(512);
        colorNode->GetLookupTable()->SetHueRange(1, 0);
        colorNode->GetLookupTable()->SetSaturationRange(1, 1);
        colorNode->GetLookupTable()->SetValueRange(0, 1);
        colorNode->GetLookupTable()->SetAlphaRange(0.5, 0.5);

        if(this->BackgroundAccepted && this->TumorAccepted)
          {
          int backgroundValue = this->BackgroundValueEntry->GetValueAsInt();
          int tumorValue = this->TumorValueEntry->GetValueAsInt();
          colorNode->GetLookupTable()->SetTableRange(backgroundValue, tumorValue);
          }
        else
          {
          colorNode->GetLookupTable()->SetTableRange(0, 512);
          }

        colorNode->GetLookupTable()->Build();
        colorNode->GetLookupTable()->SetTableValue(0, 0.0, 0.0, 0.0, 0.0);
        colorNode->SetHideFromEditors(0);
        colorNode->Modified();
        this->GetMRMLScene()->AddNode(colorNode);
        this->GetLogic()->SetColorNode(colorNode);

        // Mapped Volume
        std::stringstream volumemapname;
        volumemapname << this->DataToMap->GetName() << "-map";


        //TODO: Add imData as class member and add button to create new map
        //      So, would be able to stop and restart mapping on same map
        vtkImageData* imData = vtkImageData::New();
        imData->SetDimensions(this->DataToMap->GetImageData()->GetDimensions());
        imData->SetExtent(this->DataToMap->GetImageData()->GetExtent());
        imData->SetOrigin(this->DataToMap->GetImageData()->GetOrigin());
        imData->SetSpacing(this->DataToMap->GetImageData()->GetSpacing());
        imData->SetNumberOfScalarComponents(this->DataToMap->GetImageData()->GetNumberOfScalarComponents());
        imData->SetScalarTypeToDouble();
        imData->AllocateScalars();

        double* dest = (double*)imData->GetScalarPointer();
        int* dim = imData->GetDimensions();
        if(dest)
          {
          memset(dest, 0x00, dim[0]*dim[1]*dim[2]*sizeof(double));
          imData->Update();
          }
        imData->Modified();

        vtkMRMLLabelMapVolumeDisplayNode *displayMappedVolume = vtkMRMLLabelMapVolumeDisplayNode::New();
        displayMappedVolume->SetImageData(imData);
        displayMappedVolume->SetAndObserveColorNodeID(colorNode->GetID());
        displayMappedVolume->Modified();
        this->GetMRMLScene()->AddNode(displayMappedVolume);

        vtkMRMLScalarVolumeNode *mappedVolume = vtkMRMLScalarVolumeNode::New();
        mappedVolume->SetAndObserveImageData(imData);
        mappedVolume->SetName(volumemapname.str().c_str());
        mappedVolume->SetAndObserveDisplayNodeID(displayMappedVolume->GetID());
        mappedVolume->LabelMapOn();
        mappedVolume->SetOrigin(this->DataToMap->GetOrigin());
        mappedVolume->SetSpacing(this->DataToMap->GetSpacing());
        mappedVolume->Modified();
        this->GetMRMLScene()->AddNode(mappedVolume);
        this->CenterImage(mappedVolume);

        this->GetLogic()->SetMappedVolume(mappedVolume);

        // RAS to IJK Matrix
        vtkMatrix4x4* RASToIJKMatrix = vtkMatrix4x4::New();
        this->DataToMap->GetRASToIJKMatrix(RASToIJKMatrix);
        this->GetLogic()->SetRASToIJKMatrix(RASToIJKMatrix);

        // Data Direction (LPS, RAS, ...)
        vtkMatrix4x4* IJKToRASDirectionMatrix = vtkMatrix4x4::New();
        this->DataToMap->GetIJKToRASDirectionMatrix(IJKToRASDirectionMatrix);
        this->GetLogic()->SetIJKToRASDirectionMatrix(IJKToRASDirectionMatrix);

        // Set Logic
        this->GetLogic()->SetPositionTransform(this->Probe_Position);
        this->GetLogic()->SetUDPServerNode(this->Counts);
        this->GetLogic()->SetDataToMap(this->DataToMap);
        this->GetLogic()->SetCountsType(COUNTS_TYPE);

        // Start Mapping Thread
        this->GetLogic()->StartMapping();

        // Delete
        imData->Delete();
        RASToIJKMatrix->Delete();
        IJKToRASDirectionMatrix->Delete();
        mappedVolume->Delete();
        displayMappedVolume->Delete();
        //LUT->Delete();
        colorNode->Delete();
        }
      else
        {
        this->GetLogic()->StopMapping();
        }

      if(this->GetLogic()->GetMappingRunning())
        {
        this->MappingButton->SetReliefToSunken();
        }
      else
        {
        this->MappingButton->SetReliefToGroove();
        }
      }
    }



  if(this->TestButton == vtkKWPushButton::SafeDownCast(caller) &&
     event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLScalarVolumeNode* VolumeToSend = vtkMRMLScalarVolumeNode::New();
    vtkImageData* DataToSend = vtkImageData::New();
    DataToSend->SetDimensions(50,4,1);
    DataToSend->SetExtent(0,49,0,3,0,0);
    DataToSend->SetSpacing(1.0, 1.0, 1.0);
    DataToSend->SetOrigin(0.0, 0.0, 0.0);
    DataToSend->SetNumberOfScalarComponents(1);
    DataToSend->SetScalarTypeToDouble();
    DataToSend->AllocateScalars();

    for(int j=0; j<4; j++)
      {
      for(int i=0; i<50; i++)
        {
        double* dest = (double*) DataToSend->GetScalarPointer(i,j,0);
        if(dest)
          {
          memset(dest, (double)(i/(j+1)), sizeof(double));
          }
        }
      }
    DataToSend->Update();
    VolumeToSend->SetAndObserveImageData(DataToSend);

    this->GetMRMLScene()->AddNode(VolumeToSend);
    VolumeToSend->Delete();
    DataToSend->Delete();
    }

}


void vtkBetaProbeGUI::DataCallback(vtkObject *caller,
                                   unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeGUI *self = reinterpret_cast<vtkBetaProbeGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::ProcessLogicEvents ( vtkObject *caller,
                                           unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkBetaProbeLogic::SafeDownCast(caller))
    {
    if (event == vtkBetaProbeLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::ProcessMRMLEvents ( vtkObject *caller,
                                          unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  if(event == vtkMRMLScalarVolumeNode::ImageDataModifiedEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {

    // **********************
    // Detection counts

    if(this->Counts)
      {
      if(this->RadioBackgroundButton->GetSelectedState() && !this->BackgroundAccepted)
        {
        // Background selected, update value of entry
        if(this->BackgroundValueEntry)
          {
          switch(COUNTS_TYPE){
          case 1:  this->BackgroundValueEntry->SetValueAsInt(this->Counts->GetBetaCounts()); break;
          case 2:  this->BackgroundValueEntry->SetValueAsInt(this->Counts->GetGammaCounts()); break;
          case 3:  this->BackgroundValueEntry->SetValueAsInt(this->Counts->GetSmoothedCounts()); break;
          default: this->BackgroundValueEntry->SetValueAsInt(this->Counts->GetSmoothedCounts()); break;
          }
          }
        }
      else if(this->RadioTumorButton->GetSelectedState() && !this->TumorAccepted)
        {
        // Tumor selected, update value of entry
        if(this->TumorValueEntry)
          {
          switch(COUNTS_TYPE){
          case 1:  this->TumorValueEntry->SetValueAsInt(this->Counts->GetBetaCounts()); break;
          case 2:  this->TumorValueEntry->SetValueAsInt(this->Counts->GetGammaCounts()); break;
          case 3:  this->TumorValueEntry->SetValueAsInt(this->Counts->GetSmoothedCounts()); break;
          default: this->TumorValueEntry->SetValueAsInt(this->Counts->GetSmoothedCounts()); break;
          }
          }
        }
      }

    if(this->GetContinuousMode())
      {
      this->Capture_Data();
      }


    if(this->GetLogic()->GetMappingRunning())
      {
      if(this->GetLogic()->GetMappedVolume())
        {
        this->GetLogic()->GetMappedVolume()->Modified();
        this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->Render();
        this->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->Render();
        this->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->Render();
        }
      }


    if(this->LabelStatus)
      {
      if(this->GetLogic()->GetMappingRunning())
        {
        this->LabelStatus->SetText("ON");
        }
      else
        {
        this->LabelStatus->SetText("OFF");
        }
      }


    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(),
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
    }

  if(this->GetPivotCalibrationRunning())
    {
    if(this->PivotingMatrix && this->Probe_Position)
      {
      this->Capture_Tracker_Position();
      }
    }

}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "BetaProbe", "BetaProbe", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForPivotCalibration();
  BuildGUIForNodes();
  BuildGUIForCapturingDataFromBetaProbe();
  BuildGUIForDetection();
  BuildGUIForMapping();
}


void vtkBetaProbeGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:BetaProbe</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "BetaProbe" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkBetaProbeGUI::BuildGUIForNodes()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbe");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Nodes");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  vtkKWFrame *frame1 = vtkKWFrame::New();
  frame1->SetParent(conBrowsFrame->GetFrame());
  frame1->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                frame1->GetWidgetName() );

  this->TrackerNode = vtkSlicerNodeSelectorWidget::New();
  this->TrackerNode->SetParent(frame1);
  this->TrackerNode->Create();
  this->TrackerNode->SetWidth(30);
  this->TrackerNode->SetNewNodeEnabled(0);
  this->TrackerNode->SetNodeClass("vtkMRMLLinearTransformNode",NULL,NULL,NULL);
  this->TrackerNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->TrackerNode->UpdateMenu();

  vtkKWLabel *trackerLabel = vtkKWLabel::New();
  trackerLabel->SetParent(frame1);
  trackerLabel->Create();
  trackerLabel->SetText("Tracker Node:");
  trackerLabel->SetAnchorToWest();

  app->Script("pack %s %s -fill x -side top -padx 2 -pady 2",
              trackerLabel->GetWidgetName(),
              this->TrackerNode->GetWidgetName());

  trackerLabel->Delete();
  frame1->Delete();

  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(conBrowsFrame->GetFrame());
  frame2->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                frame2->GetWidgetName() );


  vtkKWLabel *countLabel = vtkKWLabel::New();
  countLabel->SetParent(frame1);
  countLabel->Create();
  countLabel->SetText("Count Node:");
  countLabel->SetAnchorToWest();

  this->CounterNode = vtkSlicerNodeSelectorWidget::New();
  this->CounterNode->SetParent(frame2);
  this->CounterNode->Create();
  this->CounterNode->SetWidth(30);
  this->CounterNode->SetNewNodeEnabled(0);
  this->CounterNode->SetNodeClass("vtkMRMLUDPServerNode",NULL,NULL,NULL);
  this->CounterNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->CounterNode->UpdateMenu();

  app->Script("pack %s %s -fill x -side top -padx 2 -pady 2",
              countLabel->GetWidgetName(),
              this->CounterNode->GetWidgetName());

  countLabel->Delete();
  frame2->Delete();

  conBrowsFrame->Delete();

}

//---------------------------------------------------------------------------
void vtkBetaProbeGUI::BuildGUIForPivotCalibration()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbe");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Pivot Calibration");
  conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Pivoting Calibration");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  vtkKWFrame *frame1 = vtkKWFrame::New();
  frame1->SetParent(frame->GetFrame());
  frame1->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                frame1->GetWidgetName() );

  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                frame2->GetWidgetName() );

  this->StartPivotCalibration = vtkKWPushButton::New();
  this->StartPivotCalibration->SetParent(frame2);
  this->StartPivotCalibration->Create();
  this->StartPivotCalibration->SetText("Start Pivot Calibration");
  this->StartPivotCalibration->SetWidth(29);

  this->StopPivotCalibration = vtkKWPushButton::New();
  this->StopPivotCalibration->SetParent(frame2);
  this->StopPivotCalibration->Create();
  this->StopPivotCalibration->SetText("Stop Pivot Calibration");
  this->StopPivotCalibration->SetWidth(29);
  this->StopPivotCalibration->SetState(0);

  app->Script("pack %s %s -fill x -side left -padx 2 -pady 2",
              this->StartPivotCalibration->GetWidgetName(),
              this->StopPivotCalibration->GetWidgetName());


  frame1->Delete();
  frame2->Delete();
  frame->Delete();
  conBrowsFrame->Delete();


}

//---------------------------------------------------------------------------
void vtkBetaProbeGUI::BuildGUIForCapturingDataFromBetaProbe()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbe");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Capturing Data from Beta Probe");
  conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Capturing");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  vtkKWFrame *frame5 = vtkKWFrame::New();
  frame5->SetParent(frame->GetFrame());
  frame5->Create();
  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
              frame5->GetWidgetName());

  this->SelectFile = vtkKWPushButton::New();
  this->SelectFile->SetParent(frame5);
  this->SelectFile->Create();
  this->SelectFile->SetText("Select output file");
  this->SelectFile->SetWidth(30);

  this->CloseFile = vtkKWPushButton::New();
  this->CloseFile->SetParent(frame5);
  this->CloseFile->Create();
  this->CloseFile->SetText("Close file");
  this->CloseFile->SetWidth(30);

  app->Script("pack %s %s -fill x -side left -padx 2 -pady 2",
              this->SelectFile->GetWidgetName(),
              this->CloseFile->GetWidgetName());


  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                frame2->GetWidgetName() );


  this->Capture = vtkKWPushButton::New();
  this->Capture->SetParent(frame2);
  this->Capture->Create();
  this->Capture->SetText("Capture");
  this->Capture->SetWidth(18);

  this->Start_Button = vtkKWPushButton::New();
  this->Start_Button->SetParent(frame2);
  this->Start_Button->Create();
  this->Start_Button->SetState(1);
  this->Start_Button->SetText("Start");
  this->Start_Button->SetWidth(18);

  this->Stop_Button = vtkKWPushButton::New();
  this->Stop_Button->SetParent(frame2);
  this->Stop_Button->Create();
  this->Stop_Button->SetState(0);
  this->Stop_Button->SetText("Stop");
  this->Stop_Button->SetWidth(19);

  app->Script("pack %s %s %s -fill x -side left -padx 2 -pady 2",
              this->Capture->GetWidgetName(),
              this->Start_Button->GetWidgetName(),
              this->Stop_Button->GetWidgetName());


  vtkKWFrame *frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();
  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
              frame3->GetWidgetName());


  this->Capture_status = vtkKWLabel::New();
  this->Capture_status->SetParent(frame3);
  this->Capture_status->Create();

  app->Script("pack %s -fill x -side top -padx 2 -pady 2",
              this->Capture_status->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame5->Delete();
}



void vtkBetaProbeGUI::BuildGUIForDetection()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbe");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Detection");
  conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // Detection calibration frame
  // -----------------------------------------

  vtkKWFrame *calibrationCountsFrame = vtkKWFrame::New();
  calibrationCountsFrame->SetParent(conBrowsFrame->GetFrame());
  calibrationCountsFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               calibrationCountsFrame->GetWidgetName());

  this->RadioBackgroundButton = vtkKWRadioButton::New();
  this->RadioBackgroundButton->SetParent(calibrationCountsFrame);
  this->RadioBackgroundButton->Create();
  this->RadioBackgroundButton->SetText("Background");
  this->RadioBackgroundButton->SetValueAsInt(1);

  vtkKWFrame* BackgroundFrame = vtkKWFrame::New();
  BackgroundFrame->SetParent(calibrationCountsFrame);
  BackgroundFrame->Create();

  this->BackgroundValueEntry = vtkKWEntry::New();
  this->BackgroundValueEntry->SetParent(BackgroundFrame);
  this->BackgroundValueEntry->Create();
  this->BackgroundValueEntry->SetWidth(5);
  this->BackgroundValueEntry->ReadOnlyOn();

  this->BackgroundAcceptButton = vtkKWPushButton::New();
  this->BackgroundAcceptButton->SetParent(BackgroundFrame);
  this->BackgroundAcceptButton->Create();
  this->BackgroundAcceptButton->SetText("Accept");

  this->Script("pack %s %s -side left -anchor w -padx 2 -pady 2",
               this->BackgroundValueEntry->GetWidgetName(),
               this->BackgroundAcceptButton->GetWidgetName());

  this->Script("pack %s %s -side top -anchor w -padx 2 -pady 2",
               this->RadioBackgroundButton->GetWidgetName(),
               BackgroundFrame->GetWidgetName());

  BackgroundFrame->Delete();


  this->RadioTumorButton = vtkKWRadioButton::New();
  this->RadioTumorButton->SetParent(calibrationCountsFrame);
  this->RadioTumorButton->Create();
  this->RadioTumorButton->SetText("Tumor");
  this->RadioTumorButton->SetValueAsInt(2);


  vtkKWFrame* TumorFrame = vtkKWFrame::New();
  TumorFrame->SetParent(calibrationCountsFrame);
  TumorFrame->Create();

  this->TumorValueEntry = vtkKWEntry::New();
  this->TumorValueEntry->SetParent(TumorFrame);
  this->TumorValueEntry->Create();
  this->TumorValueEntry->SetWidth(5);
  this->TumorValueEntry->ReadOnlyOn();

  this->TumorAcceptButton = vtkKWPushButton::New();
  this->TumorAcceptButton->SetParent(TumorFrame);
  this->TumorAcceptButton->Create();
  this->TumorAcceptButton->SetText("Accept");

  this->Script("pack %s %s -side left -anchor w -padx 2 -pady 2",
               this->TumorValueEntry->GetWidgetName(),
               this->TumorAcceptButton->GetWidgetName());

  this->Script("pack %s %s -side top -anchor w -padx 2 -pady 2",
               this->RadioTumorButton->GetWidgetName(),
               TumorFrame->GetWidgetName());

  TumorFrame->Delete();


  this->RadioThresholdButton = vtkKWRadioButton::New();
  this->RadioThresholdButton->SetParent(calibrationCountsFrame);
  this->RadioThresholdButton->Create();
  this->RadioThresholdButton->SetText("Threshold");
  this->RadioThresholdButton->SetValueAsInt(3);

  vtkKWFrame* ThresholdFrame = vtkKWFrame::New();
  ThresholdFrame->SetParent(calibrationCountsFrame);
  ThresholdFrame->Create();

  this->ThresholdValueEntry = vtkKWEntry::New();
  this->ThresholdValueEntry->SetParent(ThresholdFrame);
  this->ThresholdValueEntry->Create();
  this->ThresholdValueEntry->SetWidth(5);
  this->ThresholdValueEntry->ReadOnlyOn();

  this->StartDetectionButton = vtkKWPushButton::New();
  this->StartDetectionButton->SetParent(ThresholdFrame);
  this->StartDetectionButton->Create();
  this->StartDetectionButton->SetText("Start Detection");



  this->TestButton = vtkKWPushButton::New();
  this->TestButton->SetParent(ThresholdFrame);
  this->TestButton->Create();
  this->TestButton->SetText("Create Data");



  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2",
               this->ThresholdValueEntry->GetWidgetName(),
               this->StartDetectionButton->GetWidgetName(),
               this->TestButton->GetWidgetName());

  this->Script("pack %s %s -side top -anchor w -padx 2 -pady 2",
               this->RadioThresholdButton->GetWidgetName(),
               ThresholdFrame->GetWidgetName());

  ThresholdFrame->Delete();

  this->RadioTumorButton->SetVariableName(this->RadioBackgroundButton->GetVariableName());
  this->RadioThresholdButton->SetVariableName(this->RadioBackgroundButton->GetVariableName());

  this->Script("pack %s -side top -anchor w -padx 2 -pady 2",
               this->RadioBackgroundButton->GetWidgetName());

  calibrationCountsFrame->Delete();
  conBrowsFrame->Delete();

}

//----------------------------------------------------------------------------
void vtkBetaProbeGUI::BuildGUIForMapping()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbe");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Mapping");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  this->DataSelector = vtkSlicerNodeSelectorWidget::New();
  this->DataSelector->SetParent(conBrowsFrame->GetFrame());
  this->DataSelector->Create();
  this->DataSelector->SetWidth(30);
  this->DataSelector->SetNewNodeEnabled(0);
  this->DataSelector->SetNodeClass("vtkMRMLScalarVolumeNode",NULL,NULL,NULL);
  this->DataSelector->SetMRMLScene(this->GetMRMLScene());
  this->DataSelector->UpdateMenu();

  this->MappingButton = vtkKWPushButton::New();
  this->MappingButton->SetParent(conBrowsFrame->GetFrame());
  this->MappingButton->Create();
  this->MappingButton->SetText("Mapping");


  vtkKWFrame* centerFrame = vtkKWFrame::New();
  centerFrame->SetParent(conBrowsFrame->GetFrame());
  centerFrame->Create();

  vtkKWFrame* fStatus = vtkKWFrame::New();
  fStatus->SetParent(centerFrame);
  fStatus->Create();

  this->Script("pack %s -side top -anchor center -padx 2 -pady 2",
               fStatus->GetWidgetName());

  vtkKWLabel* sLabel = vtkKWLabel::New();
  sLabel->SetParent(fStatus);
  sLabel->Create();
  sLabel->SetText("Status: ");
  sLabel->SetAnchorToCenter();

  this->LabelStatus = vtkKWLabel::New();
  this->LabelStatus->SetParent(fStatus);
  this->LabelStatus->Create();
  this->LabelStatus->SetText("OFF");
  this->LabelStatus->SetAnchorToCenter();

  this->Script("pack %s %s -side left -anchor w -padx 2 -pady 2",
               sLabel->GetWidgetName(),
               this->LabelStatus->GetWidgetName());

  this->Script("pack %s %s %s -side top -expand y -fill x -anchor w -padx 2 -pady 2",
               this->DataSelector->GetWidgetName(),
               this->MappingButton->GetWidgetName(),
               centerFrame->GetWidgetName());

  fStatus->Delete();
  centerFrame->Delete();
  sLabel->Delete();
  conBrowsFrame->Delete();

}



//----------------------------------------------------------------------------
void vtkBetaProbeGUI::UpdateAll()
{
}

void vtkBetaProbeGUI::Capture_Data()
{
  if(this->BetaProbeCountsWithTimestamp.is_open())
    {
    this->Probe_Matrix = vtkMatrix4x4::New();

    time_t now = time(0);
    struct tm *current = localtime(&now);
    char mytime[20];
    sprintf(mytime, "%.2d:%.2d:%.2d", current->tm_hour, current->tm_min, current->tm_sec);

    this->Probe_Position->GetMatrixTransformToWorld(this->Probe_Matrix);
    this->BetaProbeCountsWithTimestamp << this->Counts->GetSmoothedCounts()   << "\t\t"
                                       << this->Counts->GetBetaCounts()       << "\t\t"
                                       << this->Counts->GetGammaCounts()      << "\t\t"
                                       << this->Probe_Matrix->GetElement(0,3) << "\t\t"
                                       << this->Probe_Matrix->GetElement(1,3) << "\t\t"
                                       << this->Probe_Matrix->GetElement(2,3) << "\t\t"
                                       << mytime
                                       << std::endl;

    this->Probe_Matrix->Delete();
    this->Probe_Matrix = NULL;

    this->Capture_status->SetText("Data captured");
    }
  else
    {
    this->Capture_status->SetText("Select a file first");
    }
}

void vtkBetaProbeGUI::Capture_Tracker_Position()
{

  this->Probe_Matrix = vtkMatrix4x4::New();
  this->Probe_Position->GetMatrixTransformToWorld(this->Probe_Matrix);

  this->PivotingMatrix->AddItem(this->Probe_Matrix);

  std::cout << "Number of Matrix4x4: " << this->PivotingMatrix->GetNumberOfItems() << std::endl;

}





void vtkBetaProbeGUI::CenterImage(vtkMRMLVolumeNode *volumeNode)
{
  if ( volumeNode )
    {
    vtkImageData *image = volumeNode->GetImageData();
    if (image)
      {
      vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
      volumeNode->GetIJKToRASMatrix(ijkToRAS);

      double dimsH[4];
      double rasCorner[4];
      int *dims = image->GetDimensions();
      dimsH[0] = dims[0] - 1;
      dimsH[1] = dims[1] - 1;
      dimsH[2] = dims[2] - 1;
      dimsH[3] = 0.;
      ijkToRAS->MultiplyPoint(dimsH, rasCorner);

      double origin[3];
      int i;
      for (i = 0; i < 3; i++)
        {
        origin[i] = -0.5 * rasCorner[i];
        }
      volumeNode->SetDisableModifiedEvent(1);
      volumeNode->SetOrigin(origin);
      volumeNode->SetDisableModifiedEvent(0);
      volumeNode->InvokePendingModifiedEvent();

      ijkToRAS->Delete();
      }
    }
}
