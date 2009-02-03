/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkControl4DGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLColorNode.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWPushButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWMenuButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWCanvas.h"
#include "vtkKWRange.h"

#include "vtkKWProgressDialog.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkPiecewiseFunction.h"
#include "vtkDoubleArray.h"

#include "vtkKWProgressGauge.h"

#include "vtkCornerAnnotation.h"
#include "vtkCommandLineModuleGUI.h"

#include "vtkMRML4DBundleNode.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkControl4DGUI );
vtkCxxRevisionMacro ( vtkControl4DGUI, "$Revision$");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkControl4DGUI::vtkControl4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DGUI::DataCallback);

  this->BundleNameCount = 0;
  
  //----------------------------------------------------------------
  // GUI widgets
  this->ProgressDialog = NULL;
  this->SelectInputDirectoryButton    = NULL;
  this->LoadImageButton               = NULL;
  this->SaveSeriesMenu                = NULL;
  this->SelectOutputDirectoryButton   = NULL;
  this->SaveImageButton               = NULL;
  
  this->ForegroundSeriesMenu          = NULL;
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundSeriesMenu          = NULL;
  this->BackgroundVolumeSelectorScale = NULL;

  this->SeriesToPlotMenu  = NULL;
  this->MaskSelectMenu    = NULL;
  this->MaskSelectSpinBox = NULL;
  this->MaskColorCanvas   = NULL;

  this->RunPlotButton  = NULL;
  this->PlotTypeButtonSet = NULL;
  this->FunctionEditor = NULL;
  this->SavePlotButton = NULL;

  this->InputSeriesMenu  = NULL;
  this->OutputSeriesMenu = NULL;
  this->RegistrationFixedImageIndexSpinBox = NULL;
  this->RegistrationStartIndexSpinBox = NULL;
  this->RegistrationEndIndexSpinBox   = NULL;
  this->StartRegistrationButton       = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

  //Default parameters
  this->DefaultRegistrationParam.clear();
  this->DefaultRegistrationParam[std::string("Iterations")]           = std::string("20");
  this->DefaultRegistrationParam[std::string("gridSize")]             = std::string("5");
  this->DefaultRegistrationParam[std::string("HistogramBins")]        = std::string("100");
  this->DefaultRegistrationParam[std::string("SpatialSamples")]       = std::string("5000");
  //param[std::string("ConstrainDeformation")] = std::string("0");
  //param[std::string("MaximumDeformation")]   = std::string("1.0");
}

//---------------------------------------------------------------------------
vtkControl4DGUI::~vtkControl4DGUI ( )
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

  if (this->ProgressDialog)
    {
    this->ProgressDialog->SetParent(NULL);
    this->ProgressDialog->Delete();
    }
  if (this->SelectInputDirectoryButton)
    {
    this->SelectInputDirectoryButton->SetParent(NULL);
    this->SelectInputDirectoryButton->Delete();
    }
  if (this->LoadImageButton)
    {
    this->LoadImageButton->SetParent(NULL);
    this->LoadImageButton->Delete();
    }
  if (this->SaveSeriesMenu)
    {
    this->SaveSeriesMenu->SetParent(NULL);
    this->SaveSeriesMenu->Delete();
    }
  if (this->SelectOutputDirectoryButton)
    {
    this->SelectOutputDirectoryButton->SetParent(NULL);
    this->SelectOutputDirectoryButton->Delete();
    }
  if (this->SaveImageButton)
    {
    this->SaveImageButton->SetParent(NULL);
    this->SaveImageButton->Delete();
    }
  if (this->ForegroundSeriesMenu)
    {
    this->ForegroundSeriesMenu->SetParent(NULL);
    this->ForegroundSeriesMenu->Delete();
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale->SetParent(NULL);
    this->ForegroundVolumeSelectorScale->Delete();
    }
  if (this->BackgroundSeriesMenu)
    {
    this->BackgroundSeriesMenu->SetParent(NULL);
    this->BackgroundSeriesMenu->Delete();
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale->SetParent(NULL);
    this->BackgroundVolumeSelectorScale->Delete();
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange->SetParent(NULL);
    this->WindowLevelRange->Delete();
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange->SetParent(NULL);
    this->ThresholdRange->Delete();
    }

  if (this->SeriesToPlotMenu)
    {
    this->SeriesToPlotMenu->SetParent(NULL);
    this->SeriesToPlotMenu->Delete();
    }
  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->SetParent(NULL);
    this->MaskSelectMenu->Delete();
    }
  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox->SetParent(NULL);
    this->MaskSelectSpinBox->Delete();
    }
  if (this->MaskColorCanvas)
    {
    this->MaskColorCanvas->SetParent(NULL);
    this->MaskColorCanvas->Delete();
    }
  if (this->RunPlotButton)
    {
    this->RunPlotButton->SetParent(NULL);
    this->RunPlotButton->Delete();
    }
  if (this->FunctionEditor)
    {
    this->FunctionEditor->SetParent(NULL);
    this->FunctionEditor->Delete();
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->SetParent(NULL);
    this->SavePlotButton->Delete();
    }
  if (this->InputSeriesMenu)
    {
    this->InputSeriesMenu->SetParent(NULL);
    this->InputSeriesMenu->Delete();
    }
  if (this->OutputSeriesMenu)
    {
    this->OutputSeriesMenu->SetParent(NULL);
    this->OutputSeriesMenu->Delete();
    }
  if (this->RegistrationFixedImageIndexSpinBox)
    {
    this->RegistrationFixedImageIndexSpinBox->SetParent(NULL);
    this->RegistrationFixedImageIndexSpinBox->Delete();
    }
  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox->SetParent(NULL);
    this->RegistrationStartIndexSpinBox->Delete();
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox->SetParent(NULL);
    this->RegistrationEndIndexSpinBox->Delete();
    }

  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton->SetParent(NULL);
    this->StartRegistrationButton->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

  // register node type to the MRML scene
  vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
  vtkMRMLScene* scene = this->GetMRMLScene();
  scene->RegisterNodeClass(bundleNode);
  bundleNode->Delete();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Control4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->LoadImageButton)
    {
    this->LoadImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SaveSeriesMenu)
    {
    this->SaveSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SaveImageButton)
    {
    this->SaveImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ForegroundSeriesMenu)
    {
    this->ForegroundSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundSeriesMenu)
    {
    this->BackgroundSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SeriesToPlotMenu)
    {
    this->SeriesToPlotMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotTypeButtonSet)
    {
    this->PlotTypeButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->PlotTypeButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->InputSeriesMenu)
    {
    this->InputSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->OutputSeriesMenu)
    {
    this->OutputSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RegistrationFixedImageIndexSpinBox)
    {
    this->RegistrationFixedImageIndexSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox
      ->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->IterationsEntry)
    {
    this->IterationsEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->GridSizeEntry)
    {
    this->GridSizeEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->HistogramBinsEntry)
    {
    this->HistogramBinsEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SpatialSamplesEntry)
    {
    this->SpatialSamplesEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if (this->LoadImageButton)
    {
    this->LoadImageButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SaveSeriesMenu)
    {
    this->SaveSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);      
    }
  if (this->SaveImageButton)
    {
    this->SaveImageButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ForegroundSeriesMenu)
    {
    this->ForegroundSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);      
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundSeriesMenu)
    {
    this->BackgroundSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);      
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SeriesToPlotMenu)
    {
    this->SeriesToPlotMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MaskSelectMenu)
    {
    this->MaskSelectMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MaskSelectSpinBox)
    {
    this->MaskSelectSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotTypeButtonSet)
    {
    this->PlotTypeButtonSet->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->PlotTypeButtonSet->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->InputSeriesMenu)
    {
    this->InputSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->OutputSeriesMenu)
    {
    this->OutputSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->RegistrationFixedImageIndexSpinBox)
    {
    this->RegistrationFixedImageIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RegistrationStartIndexSpinBox)
    {
    this->RegistrationStartIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RegistrationEndIndexSpinBox)
    {
    this->RegistrationEndIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StartRegistrationButton)
    {
    this->StartRegistrationButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->IterationsEntry)
    {
    this->IterationsEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->GridSizeEntry)
    {
    this->GridSizeEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);      
    }
  if (this->HistogramBinsEntry)
    {
    this->HistogramBinsEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SpatialSamplesEntry)
    {
    this->SpatialSamplesEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkControl4DLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->LoadImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectInputDirectoryButton->GetWidget()->GetFileName();
    const char* bundleName = this->SelectInputDirectoryButton->GetWidget()->GetText();
    this->GetLogic()->AddObserver(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    vtkMRML4DBundleNode* bnode = this->GetLogic()->LoadImagesFromDir(path, bundleName, this->RangeLower, this->RangeUpper);
    int n;
    if (bnode)
      {
      n = bnode->GetNumberOfFrames();
      }
    else
      {
      n = 0;
      }
    this->RegistrationFixedImageIndexSpinBox->SetRange(0, n);
    this->RegistrationStartIndexSpinBox->SetRange(0, n);
    this->RegistrationEndIndexSpinBox->SetRange(0, n);
    this->WindowLevelUpdateStatus.resize(n);
    this->Window = 1.0;
    this->Level  = 0.5;
    this->ThresholdUpper = 0.0;
    this->ThresholdLower = 1.0;
    this->GetLogic()->RemoveObservers(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    // Adjust range of the scale
    this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
    this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
    UpdateSeriesSelectorMenus();
    SetForeground(this->BundleNodeIDList[this->BundleNodeIDList.size()-1].c_str(), 0);
    SetBackground(this->BundleNodeIDList[this->BundleNodeIDList.size()-1].c_str(), 0);
    }

  if (this->SaveImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectOutputDirectoryButton->GetWidget()->GetFileName();
    int i = this->SaveSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    this->GetLogic()->AddObserver(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));
    this->GetLogic()->SaveImagesToDir(path, this->BundleNodeIDList[i].c_str(), "out", "nrrd");
    this->GetLogic()->RemoveObservers(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    }
  else if (this->ForegroundSeriesMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int i = this->ForegroundSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();
    const char* bundleID = this->BundleNodeIDList[i].c_str();
    vtkMRML4DBundleNode* bundleNode 
      = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
    if (bundleNode)
      {
      bundleNode->SwitchDisplayBuffer(0, volume);
      }
    //SetForeground(this->BundleNodeIDList[i].c_str(), volume);
    }
  else if (this->BackgroundSeriesMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int i = this->BackgroundSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();
    const char* bundleID = this->BundleNodeIDList[i].c_str();
    vtkMRML4DBundleNode* bundleNode 
      = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
    if (bundleNode)
      {
      bundleNode->SwitchDisplayBuffer(1, volume);
      }
    //SetBackground(this->BundleNodeIDList[i].c_str(), volume);
    }
  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    int i = this->ForegroundSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();
    SetForeground(this->BundleNodeIDList[i].c_str(), volume);
    }
  else if (this->BackgroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/ )
    {
    int i = this->BackgroundSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int volume = (int)this->BackgroundVolumeSelectorScale->GetValue();
    SetBackground(this->BundleNodeIDList[i].c_str(), volume);
    }
  else if (this->WindowLevelRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double wllow, wlhigh;
    this->WindowLevelRange->GetRange(wllow, wlhigh);
    this->Window = wlhigh - wllow;
    this->Level  = (wlhigh + wllow) / 2.0;
    SetWindowLevelForCurrentFrame();
    }
  else if (this->ThresholdRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double thlow, thhigh;
    this->ThresholdRange->GetRange(thlow, thhigh);
    this->ThresholdUpper  = thhigh; 
    this->ThresholdLower  = thlow; 
    SetWindowLevelForCurrentFrame();
    }
  else if (this->SeriesToPlotMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    }
  else if (this->MaskSelectMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
  else if (this->MaskSelectSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    SelectMask(nodeID, label);
    }
  else if (this->RunPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    vtkDoubleArray* p;
    if (this->PlotTypeButtonSet->GetWidget(0)->GetSelectedState())
      {
      p = this->GetLogic()->GetIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                              nodeID, label, vtkControl4DLogic::TYPE_MEAN);
      }
    else
      {
      p = this->GetLogic()->GetIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                              nodeID, label, vtkControl4DLogic::TYPE_SD);
      }
    UpdateFunctionEditor(p);
    }
  else if (this->PlotTypeButtonSet->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->PlotTypeButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    if (nodeID)
      {
      vtkDoubleArray* p;
      p = this->GetLogic()->GetIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                              nodeID, label, vtkControl4DLogic::TYPE_MEAN);
      UpdateFunctionEditor(p);
      }
    }
  else if (this->PlotTypeButtonSet->GetWidget(1) == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->PlotTypeButtonSet->GetWidget(1)->GetSelectedState() == 1)
    {
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();

    if (nodeID)
      {
      vtkDoubleArray* p;
      p = this->GetLogic()->GetIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                              nodeID, label, vtkControl4DLogic::TYPE_SD);
      UpdateFunctionEditor(p);
      }
    }
  else if (this->SavePlotButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller)
           && event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    const char* filename = (const char*)callData;
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    this->GetLogic()->SaveIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                         nodeID, label, filename);
    //this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->
    }
  /*
  else if (this->RegistrationFixedImageIndexSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  */
  else if (this->OutputSeriesMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int selected = this->OutputSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    if (selected == this->BundleNodeIDList.size())  // if "New Series" is selected"
      {
      vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
      vtkMRMLScene* scene = this->GetMRMLScene();

      char nodeName[128];
      sprintf(nodeName, "4DBundleOut_%d", selected);
      bundleNode->SetName(nodeName);
      bundleNode->SetDescription("Created by Control4D");

      vtkMatrix4x4* transform = vtkMatrix4x4::New();
      transform->Identity();
      bundleNode->ApplyTransform(transform);
      transform->Delete();
      this->GetMRMLScene()->AddNode(bundleNode);
      bundleNode->Delete();
      
      UpdateSeriesSelectorMenus();
      this->OutputSeriesMenu->GetMenu()->SelectItemWithSelectedValueAsInt(selected-2);
      }
    }

  else if (this->RegistrationStartIndexSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();
    if (sid < 0)
      {
      this->RegistrationStartIndexSpinBox->SetValue(0);
      this->RegistrationStartIndexSpinBox->Modified();
      }
    else if (sid > eid)
      {
      this->RegistrationStartIndexSpinBox->SetValue(eid);
      this->RegistrationStartIndexSpinBox->Modified();
      }
    }
  else if (this->RegistrationEndIndexSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();
    std::cerr << "number of frames = " << this->GetLogic()->GetNumberOfFrames() << std::endl;
    if (eid >= this->GetLogic()->GetNumberOfFrames())
      {
      this->RegistrationEndIndexSpinBox->SetValue(this->GetLogic()->GetNumberOfFrames()-1);
      this->RegistrationEndIndexSpinBox->Modified();
      }
    if (sid > eid)
      {
      this->RegistrationEndIndexSpinBox->SetValue(sid);
      this->RegistrationEndIndexSpinBox->Modified();
      }
    }
  else if (this->StartRegistrationButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int inputSeries = this->InputSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int outputSeries = this->OutputSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    int fid = (int)this->RegistrationFixedImageIndexSpinBox->GetValue();
    int sid = (int)this->RegistrationStartIndexSpinBox->GetValue();
    int eid = (int)this->RegistrationEndIndexSpinBox->GetValue();

    vtkControl4DLogic::RegistrationParametersType param;
    param[std::string("Iterations")]     = std::string(this->IterationsEntry->GetWidget()->GetValue());
    param[std::string("gridSize")]       = std::string(this->GridSizeEntry->GetWidget()->GetValue());      /* 3 - 20, step 1*/
    param[std::string("HistogramBins")]  = std::string(this->HistogramBinsEntry->GetWidget()->GetValue()); /* 1 - 500, step 5*/
    param[std::string("SpatialSamples")] = std::string(this->SpatialSamplesEntry->GetWidget()->GetValue());/* 1000 - 500000, step 1000*/
    //param[std::string("ConstrainDeformation")] = std::string("0");
    //param[std::string("MaximumDeformation")]   = std::string("1.0");
    //param[std::string("DefaultPixelValue")]    = std::string("0");   /* 1000 - 500000, step 1000*/

    this->GetLogic()->AddObserver(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));

    this->GetLogic()->RunSeriesRegistration(sid, eid, fid, 
                                            this->BundleNodeIDList[inputSeries].c_str(),
                                            this->BundleNodeIDList[outputSeries].c_str(),
                                            param);

    this->GetLogic()->RemoveObservers(vtkControl4DLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    }
} 


//---------------------------------------------------------------------------
void vtkControl4DGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DGUI *self = reinterpret_cast<vtkControl4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  if (event == vtkControl4DLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkControl4DLogic::ProgressDialogEvent)
    {
    vtkControl4DLogic::StatusMessageType* smsg
      = (vtkControl4DLogic::StatusMessageType*)callData;
    if (smsg->show)
      {
      if (!this->ProgressDialog)
        {
        this->ProgressDialog = vtkKWProgressDialog::New();
        this->ProgressDialog->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->SetMasterWindow(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->Create();
        }
      this->ProgressDialog->SetMessageText(smsg->message.c_str());
      this->ProgressDialog->UpdateProgress(smsg->progress);
      this->ProgressDialog->Display();
      }
    else
      {
      if (this->ProgressDialog)
        {
        this->ProgressDialog->SetParent(NULL);
        this->ProgressDialog->Delete();
        this->ProgressDialog = NULL;
        }
      }
    }

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    UpdateSeriesSelectorMenus();
    UpdateMaskSelectMenu();
    }
  else if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
  /*
  else if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent)
    {
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);
    }
  */
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessTimerEvents()
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
void vtkControl4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Control4D", "Control4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadFrame();
  BuildGUIForFrameControlFrame();
  BuildGUIForFunctionViewer();
  BuildGUIForRegistrationFrame();

}


void vtkControl4DGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The OpenIGTLink Interface Module** helps you to manage OpenIGTLink connections:"
    "OpenIGTLink is an open network protocol for communication among software / hardware "
    "for image-guided therapy. See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OpenIGTLinkIF</a> for details.";
  const char *about =
    "The module is designed and implemented by Junichi Tokuda for Brigham and Women's Hospital."
    "This work is supported by NCIGT, NA-MIC and BRP \"Enabling Technologies for MRI-Guided Prostate Intervention\" project.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Control4D" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForLoadFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load / Save");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Select File Frame
  vtkKWFrameWithLabel *inFrame = vtkKWFrameWithLabel::New();
  inFrame->SetParent(conBrowsFrame->GetFrame());
  inFrame->Create();
  inFrame->SetLabelText ("Input");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 inFrame->GetWidgetName() );
  
  this->SelectInputDirectoryButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectInputDirectoryButton->SetParent(inFrame->GetFrame());
  this->SelectInputDirectoryButton->Create();
  this->SelectInputDirectoryButton->SetWidth(50);
  this->SelectInputDirectoryButton->GetWidget()->SetText ("Browse Input Directory");
  this->SelectInputDirectoryButton->GetWidget()->TrimPathFromFileNameOn();
  this->SelectInputDirectoryButton->GetWidget()->GetLoadSaveDialog()->ChooseDirectoryOn();
  /*
    this->SelectInputDirectoryButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes(
    "{ {ProstateNav} {*.dcm} }");
  */
  this->SelectInputDirectoryButton->GetWidget()->GetLoadSaveDialog()
    ->RetrieveLastPathFromRegistry("OpenPath");

  this->LoadImageButton = vtkKWPushButton::New ( );
  this->LoadImageButton->SetParent ( inFrame->GetFrame() );
  this->LoadImageButton->Create ( );
  this->LoadImageButton->SetText ("Load Series");
  this->LoadImageButton->SetWidth (12);

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectInputDirectoryButton->GetWidgetName(),
               this->LoadImageButton->GetWidgetName());

  // -----------------------------------------
  // Select File Frame

  vtkKWFrameWithLabel *outFrame = vtkKWFrameWithLabel::New();
  outFrame->SetParent(conBrowsFrame->GetFrame());
  outFrame->Create();
  outFrame->SetLabelText ("Output");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 outFrame->GetWidgetName() );

  this->SaveSeriesMenu = vtkKWMenuButton::New();
  this->SaveSeriesMenu->SetParent(outFrame->GetFrame());
  this->SaveSeriesMenu->Create();
  this->SaveSeriesMenu->SetWidth(10);

  this->SelectOutputDirectoryButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectOutputDirectoryButton->SetParent(outFrame->GetFrame());
  this->SelectOutputDirectoryButton->Create();
  this->SelectOutputDirectoryButton->SetWidth(50);
  this->SelectOutputDirectoryButton->GetWidget()->SetText ("Browse Output Directory");
  this->SelectOutputDirectoryButton->GetWidget()->GetLoadSaveDialog()->ChooseDirectoryOn();
  this->SelectOutputDirectoryButton->GetWidget()->GetLoadSaveDialog()
    ->RetrieveLastPathFromRegistry("OpenPath");
  
  this->SaveImageButton = vtkKWPushButton::New ( );
  this->SaveImageButton->SetParent ( outFrame->GetFrame() );
  this->SaveImageButton->Create ( );
  this->SaveImageButton->SetText ("Save Series");
  this->SaveImageButton->SetWidth (12);

  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SaveSeriesMenu->GetWidgetName(),
               this->SelectOutputDirectoryButton->GetWidgetName(),
               this->SaveImageButton->GetWidgetName());

  conBrowsFrame->Delete();
  inFrame->Delete();
  outFrame->Delete();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFrameControlFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Frame Control");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Foreground child frame

  vtkKWFrameWithLabel *fframe = vtkKWFrameWithLabel::New();
  fframe->SetParent(conBrowsFrame->GetFrame());
  fframe->Create();
  fframe->SetLabelText ("Frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fframe->GetWidgetName() );
  
  vtkKWFrame *mframe = vtkKWFrame::New();
  mframe->SetParent(fframe->GetFrame());
  mframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 mframe->GetWidgetName() );
  
  vtkKWLabel *fgLabel = vtkKWLabel::New();
  fgLabel->SetParent(mframe);
  fgLabel->Create();
  fgLabel->SetText("FG: ");

  this->ForegroundSeriesMenu = vtkKWMenuButton::New();
  this->ForegroundSeriesMenu->SetParent(mframe);
  this->ForegroundSeriesMenu->Create();
  this->ForegroundSeriesMenu->SetWidth(10);
  /*
  this->ForegroundSeriesMenu->GetMenu()->AddRadioButton("Original");
  this->ForegroundSeriesMenu->GetMenu()->AddRadioButton("Registered");
  this->ForegroundSeriesMenu->SetValue("Original");
  */

  vtkKWLabel *bgLabel = vtkKWLabel::New();
  bgLabel->SetParent(mframe);
  bgLabel->Create();
  bgLabel->SetText("BG: ");

  this->BackgroundSeriesMenu = vtkKWMenuButton::New();
  this->BackgroundSeriesMenu->SetParent(mframe);
  this->BackgroundSeriesMenu->Create();
  this->BackgroundSeriesMenu->SetWidth(10);
  /*
  this->BackgroundSeriesMenu->GetMenu()->AddRadioButton("Original");
  this->BackgroundSeriesMenu->GetMenu()->AddRadioButton("Registered");
  this->BackgroundSeriesMenu->SetValue("Original");
  */

  this->Script("pack %s %s %s %s -side left -fill x -padx 2 -pady 2", 
               fgLabel->GetWidgetName(),
               this->ForegroundSeriesMenu->GetWidgetName(),
               bgLabel->GetWidgetName(),
               this->BackgroundSeriesMenu->GetWidgetName());
  
  vtkKWFrame *sframe = vtkKWFrame::New();
  sframe->SetParent(fframe->GetFrame());
  sframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 sframe->GetWidgetName() );

  this->ForegroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->ForegroundVolumeSelectorScale->SetParent(sframe);
  this->ForegroundVolumeSelectorScale->Create();
  this->ForegroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->ForegroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->ForegroundVolumeSelectorScale->SetLabelText("FG");
  this->ForegroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->ForegroundVolumeSelectorScale->SetResolution(1.0);
  this->ForegroundVolumeSelectorScale->SetWidth(30);

  this->BackgroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->BackgroundVolumeSelectorScale->SetParent(sframe);
  this->BackgroundVolumeSelectorScale->Create();
  this->BackgroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->BackgroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->BackgroundVolumeSelectorScale->SetLabelText("BG");
  this->BackgroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->BackgroundVolumeSelectorScale->SetResolution(1.0);
  this->BackgroundVolumeSelectorScale->SetWidth(30);
  
  this->Script("pack %s %s -side top -fill x -padx 2 -pady 2", 
               this->ForegroundVolumeSelectorScale->GetWidgetName(),
               this->BackgroundVolumeSelectorScale->GetWidgetName());

  // -----------------------------------------
  // Contrast control

  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Contrast");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  vtkKWFrame *lwframe = vtkKWFrame::New();
  lwframe->SetParent(cframe->GetFrame());
  lwframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 lwframe->GetWidgetName() );

  vtkKWLabel *lwLabel = vtkKWLabel::New();
  lwLabel->SetParent(lwframe);
  lwLabel->Create();
  lwLabel->SetText("Window/Level: ");

  this->WindowLevelRange = vtkKWRange::New();
  this->WindowLevelRange->SetParent(lwframe);
  this->WindowLevelRange->Create();
  this->WindowLevelRange->SymmetricalInteractionOn();
  this->WindowLevelRange->EntriesVisibilityOff ();  
  this->WindowLevelRange->SetWholeRange(0.0, 1.0);
  /*
  this->WindowLevelRange->SetCommand(this, "ProcessWindowLevelCommand");
  this->WindowLevelRange->SetStartCommand(this, "ProcessWindowLevelStartCommand");
  */
  this->Script("pack %s %s -side left -anchor nw -expand yes -fill x -padx 2 -pady 2",
               lwLabel->GetWidgetName(),
               this->WindowLevelRange->GetWidgetName());

  vtkKWFrame *thframe = vtkKWFrame::New();
  thframe->SetParent(cframe->GetFrame());
  thframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 thframe->GetWidgetName() );

  vtkKWLabel *thLabel = vtkKWLabel::New();
  thLabel->SetParent(thframe);
  thLabel->Create();
  thLabel->SetText("Threashold:   ");

  this->ThresholdRange = vtkKWRange::New();
  this->ThresholdRange->SetParent(thframe);
  this->ThresholdRange->Create();
  this->ThresholdRange->SymmetricalInteractionOff();
  this->ThresholdRange->EntriesVisibilityOff ();
  this->ThresholdRange->SetWholeRange(0.0, 1.0);
  /*
  this->ThresholdRange->SetCommand(this, "ProcessThresholdCommand");
  this->ThresholdRange->SetStartCommand(this, "ProcessThresholdStartCommand");
  */
  this->Script("pack %s %s -side left -anchor w -expand y -fill x -padx 2 -pady 2", 
               thLabel->GetWidgetName(),
               this->ThresholdRange->GetWidgetName());


  // -----------------------------------------
  // Delete pointers

  conBrowsFrame->Delete();
  fframe->Delete();
  mframe->Delete();
  sframe->Delete();
  fgLabel->Delete();
  bgLabel->Delete();

}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForFunctionViewer()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Function Viewer");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Mask selector frame
  
  vtkKWFrameWithLabel *msframe = vtkKWFrameWithLabel::New();
  msframe->SetParent(conBrowsFrame->GetFrame());
  msframe->Create();
  msframe->SetLabelText ("Mask Selector");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 msframe->GetWidgetName() );

  vtkKWFrame* sframe = vtkKWFrame::New();
  sframe->SetParent(msframe->GetFrame());
  sframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 sframe->GetWidgetName() );
  
  vtkKWLabel *seriesLabel = vtkKWLabel::New();
  seriesLabel->SetParent(sframe);
  seriesLabel->Create();
  seriesLabel->SetText("Series: ");

  this->SeriesToPlotMenu = vtkKWMenuButton::New();
  this->SeriesToPlotMenu->SetParent(sframe);
  this->SeriesToPlotMenu->Create();
  this->SeriesToPlotMenu->SetWidth(20);
  /*
  this->SeriesToPlotMenu->GetMenu()->AddRadioButton("Original");
  this->SeriesToPlotMenu->GetMenu()->AddRadioButton("Registered");
  this->SeriesToPlotMenu->SetValue("Original");
  */

  this->Script("pack %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               seriesLabel->GetWidgetName(),
               this->SeriesToPlotMenu->GetWidgetName());

  vtkKWFrame* mframe = vtkKWFrame::New();
  mframe->SetParent(msframe->GetFrame());
  mframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 mframe->GetWidgetName() );

  vtkKWLabel *menuLabel = vtkKWLabel::New();
  menuLabel->SetParent(mframe);
  menuLabel->Create();
  menuLabel->SetText("Mask: ");

  this->MaskSelectMenu = vtkKWMenuButton::New();
  this->MaskSelectMenu->SetParent(mframe);
  this->MaskSelectMenu->Create();
  this->MaskSelectMenu->SetWidth(20);

  this->MaskSelectSpinBox = vtkKWSpinBox::New();
  this->MaskSelectSpinBox->SetParent(mframe);
  this->MaskSelectSpinBox->Create();
  this->MaskSelectSpinBox->SetWidth(3);
  this->MaskSelectSpinBox->SetRange(0, 127);

  this->MaskColorCanvas = vtkKWCanvas::New();
  this->MaskColorCanvas->SetParent(mframe);
  this->MaskColorCanvas->Create();
  this->MaskColorCanvas->SetWidth(15);
  this->MaskColorCanvas->SetHeight(15);
  this->MaskColorCanvas->SetBackgroundColor(0.0, 0.0, 0.0);
  this->MaskColorCanvas->SetBorderWidth(2);
  this->MaskColorCanvas->SetReliefToSolid();

  this->RunPlotButton = vtkKWPushButton::New();
  this->RunPlotButton->SetParent(mframe);
  this->RunPlotButton->Create();
  this->RunPlotButton->SetText ("Plot");
  this->RunPlotButton->SetWidth (4);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               menuLabel->GetWidgetName(),
               this->MaskSelectMenu->GetWidgetName(),
               this->MaskSelectSpinBox->GetWidgetName(),
               this->MaskColorCanvas->GetWidgetName(),
               this->RunPlotButton->GetWidgetName());
  
  // -----------------------------------------
  // Plot frame
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Intensity Plot");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->PlotTypeButtonSet = vtkKWRadioButtonSet::New();
  this->PlotTypeButtonSet->SetParent(frame->GetFrame());
  this->PlotTypeButtonSet->Create();
  this->PlotTypeButtonSet->PackHorizontallyOn();
  this->PlotTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->PlotTypeButtonSet->UniformColumnsOn();
  this->PlotTypeButtonSet->UniformRowsOn();

  this->PlotTypeButtonSet->AddWidget(0);
  this->PlotTypeButtonSet->GetWidget(0)->SetText("Mean");
  this->PlotTypeButtonSet->AddWidget(1);
  this->PlotTypeButtonSet->GetWidget(1)->SetText("SD");

  this->PlotTypeButtonSet->GetWidget(0)->SelectedStateOn();

  this->FunctionEditor = vtkKWPiecewiseFunctionEditor::New();
  this->FunctionEditor->SetParent(frame->GetFrame());
  this->FunctionEditor->Create();
  this->FunctionEditor->SetReliefToGroove();
  this->FunctionEditor->SetPadX(2);
  this->FunctionEditor->SetPadY(2);
  this->FunctionEditor->ParameterRangeVisibilityOff();
  this->FunctionEditor->SetCanvasHeight(200);
  this->FunctionEditor->ReadOnlyOn();
  this->FunctionEditor->SetPointRadius(2);
  this->FunctionEditor->ParameterTicksVisibilityOn();
  this->FunctionEditor->SetParameterTicksFormat("%-#4.2f");
  this->FunctionEditor->ValueTicksVisibilityOn();
  this->FunctionEditor->SetValueTicksFormat("%-#4.2f");

  this->Script("pack %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->PlotTypeButtonSet->GetWidgetName(),
               this->FunctionEditor->GetWidgetName());

  // -----------------------------------------
  // Output frame
  
  vtkKWFrameWithLabel *oframe = vtkKWFrameWithLabel::New();
  oframe->SetParent(conBrowsFrame->GetFrame());
  oframe->Create();
  oframe->SetLabelText ("Output");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 oframe->GetWidgetName() );

  this->SavePlotButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SavePlotButton->SetParent(oframe->GetFrame());
  this->SavePlotButton->Create();
  this->SavePlotButton->SetWidth(50);
  this->SavePlotButton->GetWidget()->SetText ("Save");
  this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOn();
  this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->SetDefaultExtension(".csv");

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->SavePlotButton->GetWidgetName());

  conBrowsFrame->Delete();
  msframe->Delete();
  menuLabel->Delete();
  frame->Delete();
  oframe->Delete();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForRegistrationFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Registration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Data

  vtkKWFrameWithLabel *dframe = vtkKWFrameWithLabel::New();
  dframe->SetParent(conBrowsFrame->GetFrame());
  dframe->Create();
  dframe->SetLabelText ("Node");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 dframe->GetWidgetName() );

  vtkKWLabel *inputLabel = vtkKWLabel::New();
  inputLabel->SetParent(dframe->GetFrame());
  inputLabel->Create();
  inputLabel->SetText("Input:");

  this->InputSeriesMenu = vtkKWMenuButton::New();
  this->InputSeriesMenu->SetParent(dframe->GetFrame());
  this->InputSeriesMenu->Create();
  this->InputSeriesMenu->SetWidth(20);

  vtkKWLabel *outputLabel = vtkKWLabel::New();
  outputLabel->SetParent(dframe->GetFrame());
  outputLabel->Create();
  outputLabel->SetText(" Output:");
  
  this->OutputSeriesMenu = vtkKWMenuButton::New();
  this->OutputSeriesMenu->SetParent(dframe->GetFrame());
  this->OutputSeriesMenu->Create();
  this->OutputSeriesMenu->SetWidth(20);

  this->Script("pack %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               inputLabel->GetWidgetName(),
               this->InputSeriesMenu->GetWidgetName(),
               outputLabel->GetWidgetName(),
               this->OutputSeriesMenu->GetWidgetName());

  // -----------------------------------------
  // Execution
  
  vtkKWFrameWithLabel *eframe = vtkKWFrameWithLabel::New();
  eframe->SetParent(conBrowsFrame->GetFrame());
  eframe->Create();
  eframe->SetLabelText ("Execution");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 eframe->GetWidgetName() );

  vtkKWLabel *fixedLabel = vtkKWLabel::New();
  fixedLabel->SetParent(eframe->GetFrame());
  fixedLabel->Create();
  fixedLabel->SetText("Fixed:");

  this->RegistrationFixedImageIndexSpinBox = vtkKWSpinBox::New();
  this->RegistrationFixedImageIndexSpinBox->SetParent(eframe->GetFrame());
  this->RegistrationFixedImageIndexSpinBox->Create();
  this->RegistrationFixedImageIndexSpinBox->SetWidth(3);

  vtkKWLabel *fromLabel = vtkKWLabel::New();
  fromLabel->SetParent(eframe->GetFrame());
  fromLabel->Create();
  fromLabel->SetText(" From:");

  this->RegistrationStartIndexSpinBox = vtkKWSpinBox::New();
  this->RegistrationStartIndexSpinBox->SetParent(eframe->GetFrame());
  this->RegistrationStartIndexSpinBox->Create();
  this->RegistrationStartIndexSpinBox->SetWidth(3);

  vtkKWLabel *toLabel = vtkKWLabel::New();
  toLabel->SetParent(eframe->GetFrame());
  toLabel->Create();
  toLabel->SetText(" to:");

  this->RegistrationEndIndexSpinBox = vtkKWSpinBox::New();
  this->RegistrationEndIndexSpinBox->SetParent(eframe->GetFrame());
  this->RegistrationEndIndexSpinBox->Create();
  this->RegistrationEndIndexSpinBox->SetWidth(3);

  this->StartRegistrationButton = vtkKWPushButton::New();
  this->StartRegistrationButton->SetParent(eframe->GetFrame());
  this->StartRegistrationButton->Create();
  this->StartRegistrationButton->SetText ("Run");
  this->StartRegistrationButton->SetWidth (4);

  this->Script("pack %s %s %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               fixedLabel->GetWidgetName(),
               this->RegistrationFixedImageIndexSpinBox->GetWidgetName(),
               fromLabel->GetWidgetName(),
               this->RegistrationStartIndexSpinBox->GetWidgetName(),
               toLabel->GetWidgetName(),
               this->RegistrationEndIndexSpinBox->GetWidgetName(),
               this->StartRegistrationButton->GetWidgetName());
  
  // -----------------------------------------
  // Parameter Frame
  
  vtkKWFrameWithLabel *pframe = vtkKWFrameWithLabel::New();
  pframe->SetParent(conBrowsFrame->GetFrame());
  pframe->Create();
  pframe->SetLabelText ("Parameters");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 pframe->GetWidgetName() );

  this->IterationsEntry = vtkKWEntryWithLabel::New();
  this->IterationsEntry->SetParent( pframe->GetFrame());
  this->IterationsEntry->SetLabelText ("Iterations:");
  this->IterationsEntry->Create();
  this->IterationsEntry->GetWidget()->SetWidth(20);
  this->IterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->IterationsEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("Iterations")].c_str());

  this->GridSizeEntry = vtkKWEntryWithLabel::New();
  this->GridSizeEntry->SetParent( pframe->GetFrame());
  this->GridSizeEntry->SetLabelText ("Grid size (3-20):");
  this->GridSizeEntry->Create();
  this->GridSizeEntry->GetWidget()->SetWidth(20);
  this->GridSizeEntry->GetWidget()->SetRestrictValueToInteger();
  this->GridSizeEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("gridSize")].c_str());

  this->HistogramBinsEntry = vtkKWEntryWithLabel::New();
  this->HistogramBinsEntry->SetParent( pframe->GetFrame());
  this->HistogramBinsEntry->SetLabelText ("HistogramBinEntry (1-500):");
  this->HistogramBinsEntry->Create();
  this->HistogramBinsEntry->GetWidget()->SetWidth(20);
  this->HistogramBinsEntry->GetWidget()->SetRestrictValueToInteger();
  this->HistogramBinsEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("HistogramBins")].c_str());

  this->SpatialSamplesEntry = vtkKWEntryWithLabel::New();
  this->SpatialSamplesEntry->SetParent( pframe->GetFrame());
  this->SpatialSamplesEntry->SetLabelText ("SpatialSamplesEntry (1000 - 500000):");
  this->SpatialSamplesEntry->Create();
  this->SpatialSamplesEntry->GetWidget()->SetWidth(20);
  this->SpatialSamplesEntry->GetWidget()->SetRestrictValueToInteger();
  this->SpatialSamplesEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("SpatialSamples")].c_str());
  
  this->Script("pack %s %s %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->IterationsEntry->GetWidgetName(),
               this->GridSizeEntry->GetWidgetName(),
               this->HistogramBinsEntry->GetWidgetName(),
               this->SpatialSamplesEntry->GetWidgetName());

  eframe->Delete();
  fromLabel->Delete();
  toLabel->Delete();
  pframe->Delete();
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetForeground(const char* bundleID, int index)
{

  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRML4DBundleNode* bundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return;
    }

  vtkMRMLVolumeNode* volNode;
  //volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetFrameNode(index));
  bundleNode->SwitchDisplayBuffer(0, index);
  volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetDisplayBufferNode(0));

  if (volNode)
    {
    //std::cerr << "volume node name  = " <<  volNode->GetName() << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetForegroundVolumeID(volNode->GetID());
        }
      }
    //SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetBackground(const char* bundleID, int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRML4DBundleNode* bundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return;
    }

  vtkMRMLVolumeNode* volNode;
  //volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetFrameNode(index));
  bundleNode->SwitchDisplayBuffer(1, index);
  volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetDisplayBufferNode(1));

  if (volNode)
    {
    //std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");          
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetBackgroundVolumeID(volNode->GetID());
        }
      }
    //SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetWindowLevelForCurrentFrame()
{

  vtkMRMLSliceCompositeNode *cnode = 
    vtkMRMLSliceCompositeNode::SafeDownCast (this->GetMRMLScene()->GetNthNodeByClass (0, "vtkMRMLSliceCompositeNode"));
  if (cnode != NULL)
    {
    const char* fgNodeID = cnode->GetForegroundVolumeID();
    const char* bgNodeID = cnode->GetBackgroundVolumeID();
    if (fgNodeID)
      {
      vtkMRMLVolumeNode* fgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(fgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(fgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //fgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    if (bgNodeID && strcmp(fgNodeID, bgNodeID) != 0)
      {
      vtkMRMLVolumeNode* bgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(bgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //bgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateSeriesSelectorMenus()
{

  // generate a list of 4D Bundles
  std::vector<vtkMRMLNode*> nodes;
  std::vector<std::string>  names;

  this->GetApplicationLogic()->GetMRMLScene()->GetNodesByClass("vtkMRML4DBundleNode", nodes);

  this->BundleNodeIDList.clear();
  names.clear();
  std::vector<vtkMRMLNode*>::iterator niter;
  for (niter = nodes.begin(); niter != nodes.end(); niter ++)
    {
    this->BundleNodeIDList.push_back((*niter)->GetID());
    names.push_back((*niter)->GetName());
    }

  if (this->ForegroundSeriesMenu && this->BackgroundSeriesMenu && this->SeriesToPlotMenu
      && this->InputSeriesMenu && this->OutputSeriesMenu && this->SaveSeriesMenu)
    {
    this->ForegroundSeriesMenu->GetMenu()->DeleteAllItems();
    this->BackgroundSeriesMenu->GetMenu()->DeleteAllItems();
    this->SeriesToPlotMenu->GetMenu()->DeleteAllItems();
    this->InputSeriesMenu->GetMenu()->DeleteAllItems();
    this->OutputSeriesMenu->GetMenu()->DeleteAllItems();
    this->SaveSeriesMenu->GetMenu()->DeleteAllItems();
    
    std::vector<std::string>::iterator siter;
    for (siter = names.begin(); siter != names.end(); siter ++)
      {
      this->ForegroundSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->BackgroundSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->SeriesToPlotMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->InputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->OutputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->SaveSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      }
    }

  // OutputSeriesMenu has "New Series Button"
  this->OutputSeriesMenu->GetMenu()->AddRadioButton("New Series");

}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateMaskSelectMenu()
{

  if (!this->MaskSelectMenu)
    {
    return;
    }

  const char* className = this->GetMRMLScene()->GetClassNameByTag("Volume");
  std::vector<vtkMRMLNode*> nodes;
  this->GetMRMLScene()->GetNodesByClass(className, nodes);

  std::vector<vtkMRMLNode*>::iterator iter;
  if (nodes.size() > 0)
    {
    this->MaskSelectMenu->GetMenu()->DeleteAllItems();
    this->MaskNodeIDList.clear();
    for (iter = nodes.begin(); iter != nodes.end(); iter ++)
      {
      //(*iter)->GetName();
      //nodeInfo.nodeID = (*iter)->GetID();
      char str[256];

      vtkMRMLScalarVolumeNode* node =
        vtkMRMLScalarVolumeNode::SafeDownCast(*iter);
        //vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID((*iter)->GetID()));
      if (node != NULL && node->GetLabelMap())
        {
        //sprintf(str, "%s (%s)", (*iter)->GetName(), (*iter)->GetID());
        sprintf(str, "%s", (*iter)->GetName());
        this->MaskSelectMenu->GetMenu()->AddRadioButton(str);
        std::string id((*iter)->GetID());
        this->MaskNodeIDList.push_back(id);
        
        vtkIntArray* events;
        events = vtkIntArray::New();
        events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent); 
        vtkMRMLNode *nd = NULL; // TODO: is this OK?
        vtkSetAndObserveMRMLNodeEventsMacro(nd,node,events);
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SelectMask(const char* nodeID, int label)
{

  vtkMRMLScalarVolumeNode* node =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));

  if (node == NULL || !node->GetLabelMap())
    {
    return;
    }

  vtkMRMLVolumeDisplayNode* dnode = node->GetVolumeDisplayNode();
  if (dnode == NULL)
    {
    return;
    }

  vtkMRMLColorNode* cnode = dnode->GetColorNode();
  if (cnode == NULL)
    {
    return;
    }

  vtkLookupTable* lt = cnode->GetLookupTable();
  double l, h;

  l = lt->GetRange()[0];
  h = lt->GetRange()[1];
  if (label < l)
    {
    this->MaskSelectSpinBox->SetValue(l);
    label = (int)l;
    }
  else if (label > h)
    {
    this->MaskSelectSpinBox->SetValue(h);
    label = (int)h;
    }
  double color[3];
  lt->GetColor(label, color);
  if (this->MaskColorCanvas)
    {
    this->MaskColorCanvas->SetBackgroundColor(color[0], color[1], color[2]);
    }
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::UpdateFunctionEditor(vtkDoubleArray* data)
{
  if (data == NULL)
    {
    std::cerr << "no data..." << std::endl;
    return;
    }

  int n = data->GetNumberOfTuples();
  std::cerr << "number of components = " << n << std::endl;
  if (n > 0)
    {
    vtkPiecewiseFunction* tfun = vtkPiecewiseFunction::New();
    for (int i = 0; i < n; i ++)
      {
      tfun->AddPoint((double)i, data->GetValue(i));
      }
    double range[2];
    data->GetRange(range);
    this->FunctionEditor->SetWholeParameterRange(0.0, (double)(n-1));
    this->FunctionEditor->SetVisibleParameterRange(0.0, (double)(n-1));
    this->FunctionEditor->SetWholeValueRange(0.0, range[1]);
    this->FunctionEditor->SetVisibleValueRange(0.0, range[1]);
    this->FunctionEditor->SetPiecewiseFunction(tfun);
    //this->FunctionEditor->SetVisibleParameterRangeToWholeParameterRange ();
    //this->FunctionEditor->SetWholeParameterRangeToFunctionRange();
    this->FunctionEditor->Update();
    }
}


