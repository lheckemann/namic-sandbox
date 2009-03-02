/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH)
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/FourDAnalysis/vtkFourDAnalysisGUI.cxx $
  Date:      $Date: 2009-02-10 18:26:32 -0500 (Tue, 10 Feb 2009) $
  Version:   $Revision: 3674 $

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkFourDAnalysisGUI.h"
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
#include "vtkKWCheckButtonWithLabel.h"

#include "vtkKWProgressDialog.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"

#include "vtkKWProgressGauge.h"

#include "vtkCornerAnnotation.h"
#include "vtkCommandLineModuleGUI.h"

#include "vtkMRML4DBundleNode.h"
#include "vtkMRMLCurveAnalysisNode.h"


#ifdef Slicer3_USE_PYTHON
//// If debug, Python wants pythonxx_d.lib, so fake it out
//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
#include <Python.h>
//#endif

//extern "C" {
//  void init_mytkinter( Tcl_Interp* );
//  void init_slicer(void );
//}
//#include "vtkTclUtil.h"
//
#endif



//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFourDAnalysisGUI );
vtkCxxRevisionMacro ( vtkFourDAnalysisGUI, "$Revision: 3674 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkFourDAnalysisGUI::vtkFourDAnalysisGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkFourDAnalysisGUI::DataCallback);

  this->BundleNameCount = 0;

  this->IntensityCurves = vtkIntensityCurves::New();
  
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

  this->RunPlotButton       = NULL;
  this->ErrorBarCheckButton = NULL;
  this->FittingLabelMenu    = NULL;
  this->CurveScriptSelectButton  = NULL;
  this->CurveFittingStartIndexSpinBox = NULL;
  this->CurveFittingEndIndexSpinBox        = NULL;
  this->RunFittingButton    = NULL;
  this->SavePlotButton = NULL;
  this->IntensityPlot  = NULL;

  this->MapInputSeriesMenu  = NULL;
  this->MapOutputVolumePrefixEntry = NULL;
  //this->MapOutputSelector   = NULL;
  //this->MapOutputVolumeMenu = NULL;
  this->ScriptSelectButton  = NULL;
  this->RunScriptButton = NULL;


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
vtkFourDAnalysisGUI::~vtkFourDAnalysisGUI ( )
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
  if (this->RunPlotButton)
    {
    this->RunPlotButton->SetParent(NULL);
    this->RunPlotButton->Delete();
    }
  if (this->IntensityPlot)
    {
    this->IntensityPlot->SetParent(NULL);
    this->IntensityPlot->Delete();
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->SetParent(NULL);
    this->ErrorBarCheckButton->Delete();
    }
  if (this->FittingLabelMenu)
    {
    this->FittingLabelMenu->SetParent(NULL);
    this->FittingLabelMenu->Delete();
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton->SetParent(NULL);
    this->CurveScriptSelectButton->Delete();
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox->SetParent(NULL);
    this->CurveFittingStartIndexSpinBox->Delete();
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox->SetParent(NULL);
    this->CurveFittingEndIndexSpinBox->Delete();
    }
  if (this->RunFittingButton)
    {
    this->RunFittingButton->SetParent(NULL);
    this->RunFittingButton->Delete();
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->SetParent(NULL);
    this->SavePlotButton->Delete();
    }
  if (this->MapInputSeriesMenu)
    {
    this->MapInputSeriesMenu->SetParent(NULL);
    this->MapInputSeriesMenu->Delete();
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry->SetParent(NULL);
    this->MapOutputVolumePrefixEntry->Delete();
    }
  /*
  if (this->MapOutputSelector)
    {
    this->MapOutputSelector->SetParent(NULL);
    this->MapOutputSelector->Delete();
    }
  */
  /*
  if (this->MapOutputVolumeMenu)
    {
    this->MapOutputVolumeMenu->SetParent(NULL);
    this->MapOutputVolumeMenu->Delete();
    }
  */
  if (this->ScriptSelectButton)
    {
    this->ScriptSelectButton->SetParent(NULL);
    this->ScriptSelectButton->Delete();
    }
  if (this->RunScriptButton)
    {
    this->RunScriptButton->SetParent(NULL);
    this->RunScriptButton->Delete();
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
void vtkFourDAnalysisGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::Enter()
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
  vtkMRMLScene* scene = this->GetMRMLScene();

  // 4D bundle node (vtkMRML4DBundleNode)
  vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
  scene->RegisterNodeClass(bundleNode);
  bundleNode->Delete();

  // Curve analysis node (vtkMRMLCurveAnalysisNode)
  vtkMRMLCurveAnalysisNode* curveNode = vtkMRMLCurveAnalysisNode::New();
  scene->RegisterNodeClass(curveNode);
  curveNode->Delete();
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "FourDAnalysisGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::RemoveGUIObservers ( )
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

  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FittingLabelMenu)
    {
    this->FittingLabelMenu
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunFittingButton)
    {
    this->RunFittingButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapInputSeriesMenu)
    {
    this->MapInputSeriesMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  /*
  if (this->MapOutputSelector)
    {
    this->MapOutputSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  */
  /*
  if (this->MapOutputVolumeMenu)
    {
    this->MapOutputVolumeMenu->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  */
  if (this->ScriptSelectButton)
    {
    this->ScriptSelectButton->GetWidget()->GetLoadSaveDialog()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunScriptButton)
    {
    this->RunScriptButton
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
void vtkFourDAnalysisGUI::AddGUIObservers ( )
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

  if (this->RunPlotButton)
    {
    this->RunPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->GetWidget()
      ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FittingLabelMenu)
    {
    this->FittingLabelMenu
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunFittingButton)
    {
    this->RunFittingButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapInputSeriesMenu)
    {
    this->MapInputSeriesMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  /*
  if (this->MapOutputSelector)
    {
    this->MapOutputSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                                          (vtkCommand *) this->GUICallbackCommand);
    }
  */
  /*
  if (this->MapOutputVolumeMenu)
    {
    this->MapOutputVolumeMenu->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  */
  if (this->ScriptSelectButton)
    {
    this->ScriptSelectButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->RunScriptButton)
    {
    this->RunScriptButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
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
void vtkFourDAnalysisGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkFourDAnalysisLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::ProcessGUIEvents(vtkObject *caller,
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
    this->GetLogic()->AddObserver(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
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
    this->GetLogic()->RemoveObservers(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
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
    this->GetLogic()->AddObserver(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));
    this->GetLogic()->SaveImagesToDir(path, this->BundleNodeIDList[i].c_str(), "out", "nrrd");
    this->GetLogic()->RemoveObservers(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
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
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    const char* bundleID = this->BundleNodeIDList[series].c_str();
    vtkMRML4DBundleNode* bundleNode 
      = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
    int n = bundleNode->GetNumberOfFrames();
    this->CurveFittingStartIndexSpinBox->SetRange(0, n-1);
    this->CurveFittingEndIndexSpinBox->SetRange(0, n-1);
    this->CurveFittingStartIndexSpinBox->SetValue(0);
    this->CurveFittingStartIndexSpinBox->Modified();
    this->CurveFittingEndIndexSpinBox->SetValue(n-1);
    this->CurveFittingEndIndexSpinBox->Modified();
    }
  else if (this->MaskSelectMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    std::cerr << "this->MaskSelectMenu->GetMenu()" << std::endl;
    //int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    //const char* nodeID = this->MaskNodeIDList[selected].c_str();
    //SelectMask(nodeID, label);
    }
  else if (this->RunPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    const char* maskID   = this->MaskNodeIDList[selected].c_str();
    const char* bundleID = this->BundleNodeIDList[series].c_str();

    vtkMRML4DBundleNode* bundleNode 
      = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
    vtkMRMLScalarVolumeNode* maskNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(maskID));

    this->IntensityCurves->SetBundleNode(bundleNode);
    this->IntensityCurves->SetMaskNode(maskNode);

    UpdateIntensityPlot(this->IntensityCurves);

    }
  else if (this->ErrorBarCheckButton->GetWidget() == vtkKWCheckButton::SafeDownCast(caller)
           && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    if (this->ErrorBarCheckButton->GetWidget()->GetSelectedState() == 1)
      {
      this->IntensityPlot->ErrorBarOn();
      this->IntensityPlot->UpdateGraph();
      }
    else
      {
      this->IntensityPlot->ErrorBarOff();
      this->IntensityPlot->UpdateGraph();
      }
    }
  else if (this->RunFittingButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int n = this->FittingLabelMenu->GetMenu()->GetIndexOfSelectedItem();
    vtkIntArray* labels = this->IntensityCurves->GetLabelList();
    int label = labels->GetValue(n);

    vtkDoubleArray* curve = this->IntensityCurves->GetCurve(label);
    if (curve)
      {
      // Add a new vtkMRMLCurveAnalysisNode to the MRML scene
      vtkMRMLCurveAnalysisNode* curveNode = vtkMRMLCurveAnalysisNode::New();
      vtkMRMLScene* scene = this->GetMRMLScene();
      this->GetMRMLScene()->AddNode(curveNode);

      // Prepare vtkDoubleArray to pass the source cueve data
      vtkDoubleArray* srcCurve = vtkDoubleArray::New();
      srcCurve->SetNumberOfComponents( curve->GetNumberOfComponents() );

      // Check the range of the curve to fit
      int max   = curve->GetNumberOfTuples();
      int start = this->CurveFittingStartIndexSpinBox->GetValue();
      int end   = this->CurveFittingEndIndexSpinBox->GetValue();
      if (start < 0)   start = 0;
      if (end >= max)  end   = max-1;
      if (start > end) start = end;
      for (int i = start; i <= end; i ++)
        {
        double* xy = curve->GetTuple(i);
        srcCurve->InsertNextTuple(xy);
        }

      // Prepare vtkDoubleArray to receive a fitted curve from the script
      // (The size of the fitted curve should be the same as original
      // intensity curve)
      vtkDoubleArray* fittedCurve = vtkDoubleArray::New();
      fittedCurve->SetNumberOfComponents(2);
      for (int i = 0; i < max; i ++)
        {
        double* xy = curve->GetTuple(i);
        fittedCurve->InsertNextTuple(xy);
        }

      // Set source and fitted curve arrays to the curve analysis node.
      curveNode->SetSourceData(srcCurve);
      curveNode->SetFittedData(fittedCurve);

      // Get the path to the script
      const char* script = this->CurveScriptSelectButton->GetWidget()->GetFileName();

      // Call Logic to excecute the curve fitting script
      this->GetLogic()->RunCurveFitting(script, curveNode);

      // Get fitted curve
      vtkDoubleArray* resultCurve = curveNode->GetFittedData();
      // NOTE: this might not be necessary... just use the fittedCurve pointer defined above.
      if (resultCurve)
        {
        int num = resultCurve->GetNumberOfTuples();
        std::cerr << "result curve number of tuples = " << num << std::endl;
        for (int i = 0; i < num; i ++)
          {
          double* xy = resultCurve->GetTuple(i);
          std::cerr << "xy = " << xy[0]  << ", " << xy[1] << std::endl;
          }
        if (resultCurve)
          {
          UpdateIntensityPlotWithFittedCurve(this->IntensityCurves, resultCurve);
          }
        this->GetMRMLScene()->RemoveNode(curveNode);
        }
      }
    }
  else if (this->SavePlotButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller)
           && event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    const char* filename = (const char*)callData;
    int series   = this->SeriesToPlotMenu->GetMenu()->GetIndexOfSelectedItem();
    int selected = this->MaskSelectMenu->GetMenu()->GetIndexOfSelectedItem();
    //int label = (int)this->MaskSelectSpinBox->GetValue();
    const char* nodeID = this->MaskNodeIDList[selected].c_str();
    /*
    this->GetLogic()->SaveIntensityCurve(this->BundleNodeIDList[series].c_str(),
                                         nodeID, label, filename);
    */
    this->GetLogic()->SaveIntensityCurves(this->IntensityCurves, filename);
    }
  /*
  else if (this->MapOutputSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent) 
    {
    vtkMRMLScalarVolumeNode* n = vtkMRMLScalarVolumeNode::SafeDownCast(this->MapOutputSelector->GetSelected());
    if (n)
      {
      }
    }
  */
  else if (this->ScriptSelectButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller)
           && event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    }
  else if (this->RunScriptButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    const char* prefix   = this->MapOutputVolumePrefixEntry->GetValue();
    const char* filename = this->ScriptSelectButton->GetWidget()->GetFileName();
    int i = this->MapInputSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    const char* bundleID = this->BundleNodeIDList[i].c_str();
    vtkMRML4DBundleNode* bundleNode 
      = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
    if (prefix && filename)
      {
      this->GetLogic()->GenerateParameterMap(prefix, bundleNode, filename);
      }
    }
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
      bundleNode->SetDescription("Created by FourDAnalysis");

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

    vtkFourDAnalysisLogic::RegistrationParametersType param;
    param[std::string("Iterations")]     = std::string(this->IterationsEntry->GetWidget()->GetValue());
    param[std::string("gridSize")]       = std::string(this->GridSizeEntry->GetWidget()->GetValue());      /* 3 - 20, step 1*/
    param[std::string("HistogramBins")]  = std::string(this->HistogramBinsEntry->GetWidget()->GetValue()); /* 1 - 500, step 5*/
    param[std::string("SpatialSamples")] = std::string(this->SpatialSamplesEntry->GetWidget()->GetValue());/* 1000 - 500000, step 1000*/
    //param[std::string("ConstrainDeformation")] = std::string("0");
    //param[std::string("MaximumDeformation")]   = std::string("1.0");
    //param[std::string("DefaultPixelValue")]    = std::string("0");   /* 1000 - 500000, step 1000*/

    this->GetLogic()->AddObserver(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));

    this->GetLogic()->RunSeriesRegistration(sid, eid, fid, 
                                            this->BundleNodeIDList[inputSeries].c_str(),
                                            this->BundleNodeIDList[outputSeries].c_str(),
                                            param);

    this->GetLogic()->RemoveObservers(vtkFourDAnalysisLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    }
} 


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkFourDAnalysisGUI *self = reinterpret_cast<vtkFourDAnalysisGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkFourDAnalysisGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  if (event == vtkFourDAnalysisLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkFourDAnalysisLogic::ProgressDialogEvent)
    {
    vtkFourDAnalysisLogic::StatusMessageType* smsg
      = (vtkFourDAnalysisLogic::StatusMessageType*)callData;
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
void vtkFourDAnalysisGUI::ProcessMRMLEvents ( vtkObject *caller,
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
void vtkFourDAnalysisGUI::ProcessTimerEvents()
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
void vtkFourDAnalysisGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "FourDAnalysis", "FourDAnalysis", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadFrame(1);
  BuildGUIForFrameControlFrame(0);
  BuildGUIForFunctionViewer(0);
  BuildGUIForMapGenerator(0);
  BuildGUIForRegistrationFrame(0);

}


void vtkFourDAnalysisGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The 4D Analysis Module** helps you to load, view and analyze a series of 3D images (4D image),"
    "such as perfusion MRI, DCE MRI, and fMRI. "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OpenIGTLinkIF</a> for details.";
  const char *about =
    "This project is directed by Hiroto Hatabu, MD, PhD (BWH)."
    "The module is designed and implemented by Junichi Tokuda, PhD (BWH), under supports from"
    "Center for Pulmonary Functional Imaging at BWH, NCIGT and NA-MIC.";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "FourDAnalysis" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::BuildGUIForLoadFrame (int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDAnalysis");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load / Save");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
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
void vtkFourDAnalysisGUI::BuildGUIForFrameControlFrame(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDAnalysis");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Frame Control");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
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
void vtkFourDAnalysisGUI::BuildGUIForFunctionViewer(int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDAnalysis");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Function Viewer");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
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

  this->RunPlotButton = vtkKWPushButton::New();
  this->RunPlotButton->SetParent(mframe);
  this->RunPlotButton->Create();
  this->RunPlotButton->SetText ("Plot");
  this->RunPlotButton->SetWidth (4);

  this->Script("pack %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               menuLabel->GetWidgetName(),
               this->MaskSelectMenu->GetWidgetName(),
               this->RunPlotButton->GetWidgetName());
  
  // -----------------------------------------
  // Plot frame
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Intensity Plot");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->IntensityPlot = vtkKWPlotGraph::New();
  this->IntensityPlot->SetParent(frame->GetFrame());
  this->IntensityPlot->Create();
  this->IntensityPlot->SetHeight(250);

  this->IntensityPlot->AddHorizontalLine(0.5);
  this->IntensityPlot->SetAxisLineColor(1.0, 1.0, 1.0);
  this->IntensityPlot->ErrorBarOn();
  //this->IntensityPlot->ErrorBarOff();
  this->IntensityPlot->UpdateGraph();

  this->ErrorBarCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ErrorBarCheckButton->SetParent(frame->GetFrame());
  this->ErrorBarCheckButton->Create();
  this->ErrorBarCheckButton->GetWidget()->SelectedStateOff();
  this->ErrorBarCheckButton->SetLabelText("Show SD");
  
  this->Script("pack %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->IntensityPlot->GetWidgetName(),
               this->ErrorBarCheckButton->GetWidgetName());

  // -----------------------------------------
  // Curve fitting frame
  
  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Curve Fitting");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  vtkKWFrame* scriptFrame = vtkKWFrame::New();
  scriptFrame->SetParent(cframe->GetFrame());
  scriptFrame->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 scriptFrame->GetWidgetName() );

  vtkKWLabel *fittingLabelLabel = vtkKWLabel::New();
  fittingLabelLabel->SetParent(scriptFrame);
  fittingLabelLabel->Create();
  fittingLabelLabel->SetText("Label:");

  this->FittingLabelMenu = vtkKWMenuButton::New();
  this->FittingLabelMenu->SetParent(scriptFrame);
  this->FittingLabelMenu->Create();
  this->FittingLabelMenu->SetWidth(5);

  vtkKWLabel *scriptLabel = vtkKWLabel::New();
  scriptLabel->SetParent(scriptFrame);
  scriptLabel->Create();
  scriptLabel->SetText("Script:");

  this->CurveScriptSelectButton = vtkKWLoadSaveButtonWithLabel::New();
  this->CurveScriptSelectButton->SetParent(scriptFrame);
  this->CurveScriptSelectButton->Create();
  this->CurveScriptSelectButton->SetWidth(50);
  this->CurveScriptSelectButton->GetWidget()->SetText ("Load");
  this->CurveScriptSelectButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOff();

  this->Script("pack %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               fittingLabelLabel->GetWidgetName(),
               this->FittingLabelMenu->GetWidgetName(),
               scriptLabel->GetWidgetName(),
               this->CurveScriptSelectButton->GetWidgetName());

  vtkKWFrame* rangeFrame = vtkKWFrame::New();
  rangeFrame->SetParent(cframe->GetFrame());
  rangeFrame->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 rangeFrame->GetWidgetName() );

  vtkKWLabel *startLabel = vtkKWLabel::New();
  startLabel->SetParent(rangeFrame);
  startLabel->Create();
  startLabel->SetText("From:");
  
  this->CurveFittingStartIndexSpinBox = vtkKWSpinBox::New();
  this->CurveFittingStartIndexSpinBox->SetParent(rangeFrame);
  this->CurveFittingStartIndexSpinBox->Create();
  this->CurveFittingStartIndexSpinBox->SetWidth(3);

  vtkKWLabel *endLabel = vtkKWLabel::New();
  endLabel->SetParent(rangeFrame);
  endLabel->Create();
  endLabel->SetText("to:");

  this->CurveFittingEndIndexSpinBox = vtkKWSpinBox::New();
  this->CurveFittingEndIndexSpinBox->SetParent(rangeFrame);
  this->CurveFittingEndIndexSpinBox->Create();
  this->CurveFittingEndIndexSpinBox->SetWidth(3);

  this->RunFittingButton = vtkKWPushButton::New();
  this->RunFittingButton->SetParent(rangeFrame);
  this->RunFittingButton->Create();
  this->RunFittingButton->SetText ("Run");
  this->RunFittingButton->SetWidth (4);
  
  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               startLabel->GetWidgetName(),
               this->CurveFittingStartIndexSpinBox->GetWidgetName(),
               endLabel->GetWidgetName(),
               this->CurveFittingEndIndexSpinBox->GetWidgetName(),
               this->RunFittingButton->GetWidgetName());
  

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
  //this->SavePlotButton->GetWidget()->GetLoadSaveDialog()->SetDefaultExtension(".csv");

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->SavePlotButton->GetWidgetName());

  conBrowsFrame->Delete();
  msframe->Delete();
  menuLabel->Delete();
  frame->Delete();
  oframe->Delete();
  fittingLabelLabel->Delete();
  scriptLabel->Delete();

}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::BuildGUIForMapGenerator(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDAnalysis");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Map Generator");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Node Selector

  vtkKWFrameWithLabel *dframe = vtkKWFrameWithLabel::New();
  dframe->SetParent(conBrowsFrame->GetFrame());
  dframe->Create();
  dframe->SetLabelText ("Generate Map");
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                dframe->GetWidgetName() );

  vtkKWFrame *inputFrame = vtkKWFrame::New();
  inputFrame->SetParent(dframe->GetFrame());
  inputFrame->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                inputFrame->GetWidgetName() );

  vtkKWLabel *inputLabel = vtkKWLabel::New();
  inputLabel->SetParent(inputFrame);
  inputLabel->Create();
  inputLabel->SetText("Input:");

  this->MapInputSeriesMenu = vtkKWMenuButton::New();
  this->MapInputSeriesMenu->SetParent(inputFrame);
  this->MapInputSeriesMenu->Create();
  this->MapInputSeriesMenu->SetWidth(20);

  vtkKWLabel *scriptLabel = vtkKWLabel::New();
  scriptLabel->SetParent(inputFrame);
  scriptLabel->Create();
  scriptLabel->SetText("Script:");

  this->ScriptSelectButton = vtkKWLoadSaveButtonWithLabel::New();
  this->ScriptSelectButton->SetParent(inputFrame);
  this->ScriptSelectButton->Create();
  this->ScriptSelectButton->SetWidth(50);
  this->ScriptSelectButton->GetWidget()->SetText ("Load");
  this->ScriptSelectButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOff();

  this->Script("pack %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               inputLabel->GetWidgetName(),
               this->MapInputSeriesMenu->GetWidgetName(),
               scriptLabel->GetWidgetName(),
               this->ScriptSelectButton->GetWidgetName());


  vtkKWFrame *outputFrame = vtkKWFrame::New();
  outputFrame->SetParent(dframe->GetFrame());
  outputFrame->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                outputFrame->GetWidgetName());

  vtkKWLabel *outputLabel = vtkKWLabel::New();
  outputLabel->SetParent(outputFrame);
  outputLabel->Create();
  outputLabel->SetText(" Output prefix:");
  
  this->MapOutputVolumePrefixEntry = vtkKWEntry::New();
  this->MapOutputVolumePrefixEntry->SetParent(outputFrame);
  this->MapOutputVolumePrefixEntry->Create();
  //this->MapOutputVolumePrefixEntry->SetWidth(20);
  this->MapOutputVolumePrefixEntry->SetValue("ParameterMap");

  this->RunScriptButton = vtkKWPushButton::New();
  this->RunScriptButton->SetParent(outputFrame);
  this->RunScriptButton->Create();
  this->RunScriptButton->SetText ("Run Script");
  this->RunScriptButton->SetWidth (10);
  
  this->Script("pack %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               outputLabel->GetWidgetName(),
               this->MapOutputVolumePrefixEntry->GetWidgetName(),
               this->RunScriptButton->GetWidgetName());
/*
  MapOutputSelector = vtkSlicerNodeSelectorWidget::New();
  MapOutputSelector->ShowHiddenOn();
  MapOutputSelector->SetNodeClass("vtkMRMLScalarVolumeNode", "Volume", title.c_str(), title.c_str());
  MapOutputSelector->SetNewNodeEnabled(1);
  MapOutputSelector->SetParent( moduleFrame->GetFrame() );
  MapOutputSelector->Create();
  MapOutputSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  MapOutputSelector->SetSelected(NULL);  // force empt select
  MapOutputSelector->SetBorderWidth(2);
  MapOutputSelector->SetReliefToFlat();
  MapOutputSelector->SetLabelText( "Parameter set");
  std::string nodeSelectorBalloonHelp = "select an output volume node from the current mrml scene.";
  MapOutputSelector->SetBalloonHelpString(nodeSelectorBalloonHelp.c_str());
  */

  /*
  this->MapOutputVolumeMenu = vtkKWMenuButton::New();
  this->MapOutputVolumeMenu->SetParent(dframe->GetFrame());
  this->MapOutputVolumeMenu->Create();
  this->MapOutputVolumeMenu->SetWidth(20);
  */

  dframe->Delete();
  inputFrame->Delete();
  outputFrame->Delete();
  inputLabel->Delete();
  outputLabel->Delete();

}


//---------------------------------------------------------------------------
void vtkFourDAnalysisGUI::BuildGUIForRegistrationFrame(int show)
{
  
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDAnalysis");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Registration");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
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
void vtkFourDAnalysisGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkFourDAnalysisGUI::SetForeground(const char* bundleID, int index)
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
void vtkFourDAnalysisGUI::SetBackground(const char* bundleID, int index)
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
void vtkFourDAnalysisGUI::SetWindowLevelForCurrentFrame()
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
void vtkFourDAnalysisGUI::UpdateSeriesSelectorMenus()
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

  if (this->ForegroundSeriesMenu && this->BackgroundSeriesMenu
      && this->SeriesToPlotMenu && this->MapInputSeriesMenu
      && this->InputSeriesMenu && this->OutputSeriesMenu && this->SaveSeriesMenu)
    {
    this->ForegroundSeriesMenu->GetMenu()->DeleteAllItems();
    this->BackgroundSeriesMenu->GetMenu()->DeleteAllItems();
    this->SeriesToPlotMenu->GetMenu()->DeleteAllItems();
    this->MapInputSeriesMenu->GetMenu()->DeleteAllItems();
    this->InputSeriesMenu->GetMenu()->DeleteAllItems();
    this->OutputSeriesMenu->GetMenu()->DeleteAllItems();
    this->SaveSeriesMenu->GetMenu()->DeleteAllItems();
    
    std::vector<std::string>::iterator siter;
    for (siter = names.begin(); siter != names.end(); siter ++)
      {
      this->ForegroundSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->BackgroundSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->SeriesToPlotMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->MapInputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->InputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->OutputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->SaveSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      }
    }

  // OutputSeriesMenu has "New Series Button"
  this->OutputSeriesMenu->GetMenu()->AddRadioButton("New Series");

}


//----------------------------------------------------------------------------
void vtkFourDAnalysisGUI::UpdateMaskSelectMenu()
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
void vtkFourDAnalysisGUI::UpdateIntensityPlot(vtkIntensityCurves* intensityCurves)
{
  this->IntensityCurves->Update();
  vtkMRMLScalarVolumeNode* node = this->IntensityCurves->GetMaskNode();
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

  vtkIntArray* labels = this->IntensityCurves->GetLabelList();
  int n = labels->GetNumberOfTuples();

  this->IntensityPlot->ClearPlot();
  for (int i = 0; i < n; i ++)
    {
    int label = labels->GetValue(i);
    vtkDoubleArray* values = this->IntensityCurves->GetCurve(label);
    int id = this->IntensityPlot->AddPlot(values, "1");

    double color[3];
    lt->GetColor(label, color);
    if (color[0] > 0.99 && color[1] > 0.99  && color[2] > 0.99)
      {
      // if the line color is white, change the color to black
      color[0] = 0.0;
      color[1] = 0.0;
      color[2] = 0.0;
      }
    this->IntensityPlot->SetColor(id, color[0], color[1], color[2]);
    }
  //this->IntensityPlot->AddVerticalLine(180.0);
  this->IntensityPlot->AutoRangeOn();
  this->IntensityPlot->UpdateGraph();

  // Update FittingLabelMenu
  if (this->FittingLabelMenu)
    {
    this->FittingLabelMenu->GetMenu()->DeleteAllItems();
    for (int i = 0; i < n; i ++)
      {
      char str[256];
      sprintf(str, "%d", labels->GetValue(i));
      this->FittingLabelMenu->GetMenu()->AddRadioButton(str);
      }
    }
}

//----------------------------------------------------------------------------
void vtkFourDAnalysisGUI::UpdateIntensityPlotWithFittedCurve(vtkIntensityCurves* intensityCurves, vtkDoubleArray* array)
{
  this->IntensityCurves->Update();
  vtkMRMLScalarVolumeNode* node = this->IntensityCurves->GetMaskNode();
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

  vtkIntArray* labels = this->IntensityCurves->GetLabelList();
  int n = labels->GetNumberOfTuples();

  this->IntensityPlot->ClearPlot();
  for (int i = 0; i < n; i ++)
    {
    int label = labels->GetValue(i);
    vtkDoubleArray* values = this->IntensityCurves->GetCurve(label);
    int id = this->IntensityPlot->AddPlot(values, "1");

    double color[3];
    lt->GetColor(label, color);
    if (color[0] > 0.99 && color[1] > 0.99  && color[2] > 0.99)
      {
      // if the line color is white, change the color to black
      color[0] = 0.0;
      color[1] = 0.0;
      color[2] = 0.0;
      }
    this->IntensityPlot->SetColor(id, color[0], color[1], color[2]);
    }

  if (array)
    {
    int id = this->IntensityPlot->AddPlot(array, "Fitted");
    double color[3];
    color[0] = 1.0;
    color[1] = 0.0;
    color[2] = 0.0;
    this->IntensityPlot->SetColor(id, color[0], color[1], color[2]);
    }
  
  //this->IntensityPlot->AddVerticalLine(180.0);
  this->IntensityPlot->AutoRangeOn();
  this->IntensityPlot->UpdateGraph();
}


