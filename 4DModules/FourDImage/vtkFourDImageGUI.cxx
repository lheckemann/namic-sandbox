/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH)
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/FourDImage/vtkFourDImageGUI.cxx $
  Date:      $Date: 2009-02-10 18:26:32 -0500 (Tue, 10 Feb 2009) $
  Version:   $Revision: 3674 $

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkFourDImageGUI.h"
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
#include "vtkKWMultiColumnList.h"

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

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkFourDImageGUI );
vtkCxxRevisionMacro ( vtkFourDImageGUI, "$Revision: 3674 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkFourDImageGUI::vtkFourDImageGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkFourDImageGUI::DataCallback);

  this->BundleNameCount = 0;

  this->AutoPlayFG              = 0;
  this->AutoPlayBG              = 0;
  this->AutoPlayInterval        = 10;
  this->AutoPlayIntervalCounter = 0;
  
  //----------------------------------------------------------------
  // GUI widgets
  this->ProgressDialog = NULL;
  this->SelectInputDirectoryButton    = NULL;

  // Load / Save
  this->LoadImageButton               = NULL;
  this->SelectOutputDirectoryButton   = NULL;
  this->SaveImageButton               = NULL;

  this->Active4DBundleSelectorWidget  = NULL;

  // Frame control
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundVolumeSelectorScale = NULL;
  this->AutoPlayFGButton              = NULL;
  this->AutoPlayBGButton              = NULL;
  this->AutoPlayIntervalEntry         = NULL;

  // Cropping
  this->StartCroppingButton = NULL;
  this->CropIMinSpinBox     = NULL;
  this->CropIMaxSpinBox     = NULL;
  this->CropJMinSpinBox     = NULL;
  this->CropJMaxSpinBox      = NULL;
  this->CropKMinSpinBox     = NULL;
  this->CropKMaxSpinBox     = NULL;

  // Registration
  this->InputSeriesMenu     = NULL;
  this->OutputSeriesMenu    = NULL;
  this->RegistrationFixedImageIndexSpinBox = NULL;
  this->RegistrationStartIndexSpinBox = NULL;
  this->RegistrationEndIndexSpinBox   = NULL;
  this->StartRegistrationButton       = NULL;

  this->AffineIterationsEntry  = NULL;
  this->AffineSamplesEntry     = NULL;
  this->AffineHistgramBinEntry = NULL;
  this->AffineTranslationEntry = NULL;

  this->IterationsEntry        = NULL;
  this->GridSizeEntry          = NULL;
  this->HistogramBinsEntry     = NULL;
  this->SpatialSamplesEntry    = NULL;


  //----------------------------------------------------------------
  // Time
  this->TimerFlag = 0;

  //Default parameters for Affine Registration
  this->DefaultAffineRegistrationParam.clear();
  this->DefaultAffineRegistrationParam[std::string("Iterations")]           = std::string("2000");
  this->DefaultAffineRegistrationParam[std::string("TranslationScale")]     = std::string("100");
  this->DefaultAffineRegistrationParam[std::string("HistogramBins")]        = std::string("30");
  this->DefaultAffineRegistrationParam[std::string("SpatialSamples")]       = std::string("10000");

  //Default parameters for Deformable Registration
  this->DefaultRegistrationParam.clear();
  this->DefaultRegistrationParam[std::string("Iterations")]           = std::string("20");
  this->DefaultRegistrationParam[std::string("gridSize")]             = std::string("5");
  this->DefaultRegistrationParam[std::string("HistogramBins")]        = std::string("100");
  this->DefaultRegistrationParam[std::string("SpatialSamples")]       = std::string("500000");
  //param[std::string("ConstrainDeformation")] = std::string("0");
  //param[std::string("MaximumDeformation")]   = std::string("1.0");
}

//---------------------------------------------------------------------------
vtkFourDImageGUI::~vtkFourDImageGUI ( )
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

  // Progress Dialog
  if (this->ProgressDialog)
    {
    this->ProgressDialog->SetParent(NULL);
    this->ProgressDialog->Delete();
    }

  // Load / Save
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

  // 4D Bundle Selector
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget->SetParent(NULL);
    this->Active4DBundleSelectorWidget->Delete();
    }

  // Frame control
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale->SetParent(NULL);
    this->ForegroundVolumeSelectorScale->Delete();
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale->SetParent(NULL);
    this->BackgroundVolumeSelectorScale->Delete();
    }
  if (this->AutoPlayFGButton)
    {
    this->AutoPlayFGButton->SetParent(NULL);
    this->AutoPlayFGButton->Delete();
    }
  if (this->AutoPlayBGButton)
    {
    this->AutoPlayBGButton->SetParent(NULL);
    this->AutoPlayBGButton->Delete();
    }
  if (this->AutoPlayIntervalEntry)
    {
    this->AutoPlayIntervalEntry->SetParent(NULL);
    this->AutoPlayIntervalEntry->Delete();
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

  // Cropping
  if (this->StartCroppingButton)
    {
    this->StartCroppingButton->SetParent(NULL);
    this->StartCroppingButton->Delete();
    }
  if (this->CropIMinSpinBox)
    {
    this->CropIMinSpinBox->SetParent(NULL);
    this->CropIMinSpinBox->Delete();
    }
  if (this->CropIMaxSpinBox)
    {
    this->CropIMaxSpinBox->SetParent(NULL);
    this->CropIMaxSpinBox->Delete();
    }
  if (this->CropJMinSpinBox)
    {
    this->CropJMinSpinBox->SetParent(NULL);
    this->CropJMinSpinBox->Delete();
    }
  if (this->CropJMaxSpinBox)
    {
    this->CropJMaxSpinBox->SetParent(NULL);
    this->CropJMaxSpinBox->Delete();
    }
  if (this->CropKMinSpinBox)
    {
    this->CropKMinSpinBox->SetParent(NULL);
    this->CropKMinSpinBox->Delete();
    }
  if (this->CropKMaxSpinBox)
    {
    this->CropKMaxSpinBox->SetParent(NULL);
    this->CropKMaxSpinBox->Delete();
    }

  // Registration
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
void vtkFourDImageGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::Enter()
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

}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "FourDImageGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->LoadImageButton)
    {
    this->LoadImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SaveImageButton)
    {
    this->SaveImageButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayFGButton)
    {
    this->AutoPlayFGButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayBGButton)
    {
    this->AutoPlayBGButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayIntervalEntry)
    {
    this->AutoPlayIntervalEntry
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

  // Cropping
  if (this->StartCroppingButton)
    {
    this->StartCroppingButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropIMinSpinBox)
    {
    this->CropIMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropIMaxSpinBox)
    {
    this->CropIMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropJMinSpinBox)
    {
    this->CropJMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropJMaxSpinBox)
    {
    this->CropJMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropKMinSpinBox)
    {
    this->CropKMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropKMaxSpinBox)
    {
    this->CropKMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // Registration
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
  if (this->AffineIterationsEntry)
    {
    this->AffineIterationsEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineSamplesEntry)
    {
    this->AffineSamplesEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineHistgramBinEntry)
    {
    this->AffineHistgramBinEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineTranslationEntry)
    {
    this->AffineTranslationEntry->GetWidget()
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
void vtkFourDImageGUI::AddGUIObservers ( )
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
  if (this->SaveImageButton)
    {
    this->SaveImageButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayFGButton)
    {
    this->AutoPlayFGButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayBGButton)
    {
    this->AutoPlayBGButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AutoPlayIntervalEntry)
    {
    this->AutoPlayIntervalEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
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
  if (this->StartCroppingButton)
    {
    this->StartCroppingButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropIMinSpinBox)
    {
    this->CropIMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropIMaxSpinBox)
    {
    this->CropIMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropJMinSpinBox)
    {
    this->CropJMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropJMaxSpinBox)
    {
    this->CropJMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropKMinSpinBox)
    {
    this->CropKMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CropKMaxSpinBox)
    {
    this->CropKMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
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

  if (this->AffineIterationsEntry)
    {
    this->AffineIterationsEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineSamplesEntry)
    {
    this->AffineSamplesEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineHistgramBinEntry)
    {
    this->AffineHistgramBinEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AffineTranslationEntry)
    {
    this->AffineTranslationEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
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
void vtkFourDImageGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkFourDImageLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::ProcessGUIEvents(vtkObject *caller,
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
    this->GetLogic()->AddObserver(vtkFourDImageLogic::ProgressDialogEvent, 
                                  this->LogicCallbackCommand);
    vtkMRML4DBundleNode* newNode = this->GetLogic()->LoadImagesFromDir(path, bundleName, this->RangeLower, this->RangeUpper);

    this->GetLogic()->RemoveObservers(vtkFourDImageLogic::ProgressDialogEvent,
                                      this->LogicCallbackCommand);

    UpdateSeriesSelectorMenus();

    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());

    if (newNode && bundleNode &&
        strcmp(newNode->GetID(), bundleNode->GetID()) == 0)  // new node is selected as the active bundle
      {
      this->Window = 1.0;
      this->Level  = 0.5;
      this->ThresholdUpper = 0.0;
      this->ThresholdLower = 1.0;

      SelectActive4DBundle(bundleNode);
      }
    }

  if (this->SaveImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectOutputDirectoryButton->GetWidget()->GetFileName();
    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      this->GetLogic()->AddObserver(vtkFourDImageLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
      this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));
      this->GetLogic()->SaveImagesToDir(path, bundleNode->GetID(), "out", "nrrd");
      this->GetLogic()->RemoveObservers(vtkFourDImageLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
      }
    }
  else if (this->Active4DBundleSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    SelectActive4DBundle(bundleNode);
    }
  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();
    if (bundleNode)
      {
      SetForeground(bundleNode->GetID(), volume);
      }
    }
  else if (this->BackgroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/ )
    {
    int volume = (int)this->BackgroundVolumeSelectorScale->GetValue();
    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      SetBackground(bundleNode->GetID(), volume);
      }
    }
  if (this->AutoPlayFGButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (!this->AutoPlayFG)
      {
      // NOTE: interval = TimerInterval * AutoPlayInterval; 
      double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
      this->AutoPlayFG              = 1;
      this->AutoPlayInterval        = (int) (interval_s * 1000.0 / (double)this->TimerInterval);  
      this->AutoPlayIntervalCounter = 0;
      this->AutoPlayFGButton->SetText ("| |");
      this->AutoPlayFGButton->Modified();
      }
    else
      {
      this->AutoPlayFG              = 0;
      this->AutoPlayFGButton->SetText (" > ");
      this->AutoPlayFGButton->Modified();
      }
    }
  if (this->AutoPlayBGButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (!this->AutoPlayBG)
      {
      // NOTE: interval = TimerInterval * AutoPlayInterval; 
      double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
      this->AutoPlayBG              = 1;
      this->AutoPlayInterval        = (int) (interval_s * 1000.0 / (double)this->TimerInterval);  
      this->AutoPlayIntervalCounter = 0;
      this->AutoPlayBGButton->SetText ("| |");
      this->AutoPlayBGButton->Modified();
      }
    else
      {
      this->AutoPlayBG              = 0;
      this->AutoPlayBGButton->SetText (" > ");
      this->AutoPlayBGButton->Modified();
      }
    }
  if (this->AutoPlayIntervalEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    // NOTE: interval = TimerInterval * AutoPlayInterval; 
    double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
    this->AutoPlayInterval = (int) (interval_s * 1000.0 / (double)this->TimerInterval);
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
  else if (this->StartCroppingButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRML4DBundleNode *bundleNode = 
      vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());

    if (bundleNode)
      {
      int imin = (int)this->CropIMinSpinBox->GetValue();
      int imax = (int)this->CropIMaxSpinBox->GetValue();
      int jmin = (int)this->CropJMinSpinBox->GetValue();
      int jmax = (int)this->CropJMaxSpinBox->GetValue();
      int kmin = (int)this->CropKMinSpinBox->GetValue();
      int kmax = (int)this->CropKMaxSpinBox->GetValue();
      
      this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));
      this->GetLogic()->AddObserver(vtkFourDImageLogic::ProgressDialogEvent, 
                                    this->LogicCallbackCommand);
      this->GetLogic()->RunSeriesCropping(bundleNode->GetID(), imin, imax, jmin, jmax, kmin, kmax);
      this->GetLogic()->RemoveObservers(vtkFourDImageLogic::ProgressDialogEvent, 
                                        this->LogicCallbackCommand);
      }
    }
  else if (this->CropIMinSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  else if (this->CropIMaxSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  else if (this->CropJMinSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  else if (this->CropJMaxSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  else if (this->CropKMinSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }
  else if (this->CropKMaxSpinBox == vtkKWSpinBox::SafeDownCast(caller)
           && event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    }

  else if (this->OutputSeriesMenu->GetMenu() == vtkKWMenu::SafeDownCast(caller)
      && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    int selected = this->OutputSeriesMenu->GetMenu()->GetIndexOfSelectedItem();
    if (selected == this->BundleNodeIDList.size())  // if "New Series" is selected"
      {
      vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
      //vtkMRMLScene* scene = this->GetMRMLScene();

      char nodeName[128];
      sprintf(nodeName, "4DBundleOut_%d", selected);
      bundleNode->SetName(nodeName);
      bundleNode->SetDescription("Created by FourDImage");

      vtkMatrix4x4* transform = vtkMatrix4x4::New();
      transform->Identity();
      bundleNode->ApplyTransform(transform);
      transform->Delete();
      this->GetMRMLScene()->AddNode(bundleNode);
      bundleNode->Delete();
      
      UpdateSeriesSelectorMenus();
      this->OutputSeriesMenu->GetMenu()->SelectItem(selected);

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

    vtkFourDImageLogic::RegistrationParametersType affineParam;
    affineParam[std::string("Iterations")]       = std::string(this->AffineIterationsEntry->GetWidget()->GetValue());
    affineParam[std::string("TranslationScale")] = std::string(this->AffineTranslationEntry->GetWidget()->GetValue());
    affineParam[std::string("HistogramBins")]    = std::string(this->AffineHistgramBinEntry->GetWidget()->GetValue());
    affineParam[std::string("SpatialSamples")]   = std::string(this->AffineSamplesEntry->GetWidget()->GetValue());

    vtkFourDImageLogic::RegistrationParametersType deformableParam;
    deformableParam[std::string("Iterations")]     = std::string(this->IterationsEntry->GetWidget()->GetValue());
    deformableParam[std::string("gridSize")]       = std::string(this->GridSizeEntry->GetWidget()->GetValue());
    deformableParam[std::string("HistogramBins")]  = std::string(this->HistogramBinsEntry->GetWidget()->GetValue());
    deformableParam[std::string("SpatialSamples")] = std::string(this->SpatialSamplesEntry->GetWidget()->GetValue());
    //param[std::string("ConstrainDeformation")] = std::string("0");
    //param[std::string("MaximumDeformation")]   = std::string("1.0");
    //param[std::string("DefaultPixelValue")]    = std::string("0");   /* 1000 - 500000, step 1000*/

    this->GetLogic()->AddObserver(vtkFourDImageLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));

    this->GetLogic()->RunSeriesRegistration(sid, eid, fid, 
                                            this->BundleNodeIDList[inputSeries].c_str(),
                                            this->BundleNodeIDList[outputSeries].c_str(),
                                            affineParam, deformableParam);

    this->GetLogic()->RemoveObservers(vtkFourDImageLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
    }
} 


//---------------------------------------------------------------------------
void vtkFourDImageGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkFourDImageGUI *self = reinterpret_cast<vtkFourDImageGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkFourDImageGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  if (event == vtkFourDImageLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkFourDImageLogic::ProgressDialogEvent)
    {
    vtkFourDImageLogic::StatusMessageType* smsg
      = (vtkFourDImageLogic::StatusMessageType*)callData;
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
void vtkFourDImageGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    UpdateSeriesSelectorMenus();
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
void vtkFourDImageGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    if (this->AutoPlayFG || this->AutoPlayBG)
      {
      this->AutoPlayIntervalCounter ++;
      if (this->AutoPlayInterval != 0 &&
          this->AutoPlayIntervalCounter % this->AutoPlayInterval == 0)
        {
        double current;
        double range[2];
        int    volume;
        this->AutoPlayIntervalCounter = 0;
          
        // increment the frame id for foreground
        if (this->AutoPlayFG)
          {
          this->ForegroundVolumeSelectorScale->GetRange(range);
          current = this->ForegroundVolumeSelectorScale->GetValue();
          current += 1;
          if (current > range[1])
            {
            current = 0.0;
            }
          this->ForegroundVolumeSelectorScale->SetValue((double)current);
          volume = (int)current;
          vtkMRML4DBundleNode *bundleNode = 
            vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
          if (bundleNode)
            {
            SetForeground(bundleNode->GetID(), volume);
            }
          }
        // increment the frame id for foreground
        if (this->AutoPlayBG)
          {
          this->BackgroundVolumeSelectorScale->GetRange(range);
          current = this->BackgroundVolumeSelectorScale->GetValue();
          current += 1;
          if (current > range[1])
            {
            current = 0.0;
            }
          this->BackgroundVolumeSelectorScale->SetValue((double)current);
          volume = (int)current;
          vtkMRML4DBundleNode *bundleNode = 
            vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
          if (bundleNode)
            {
            SetBackground(bundleNode->GetID(), volume);
            }
          }
        }
      }
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "FourDImage", "FourDImage", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadFrame(1);
  BuildGUIForActiveBundleSelectorFrame();
  BuildGUIForFrameControlFrame(0);
  BuildGUIForCroppingFrame(0);
  BuildGUIForRegistrationFrame(0);

}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForActiveBundleSelectorFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDImage");

  //  Volume to select
  this->Active4DBundleSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->Active4DBundleSelectorWidget->SetParent(page);
  this->Active4DBundleSelectorWidget->Create();
  this->Active4DBundleSelectorWidget->SetNodeClass("vtkMRML4DBundleNode", NULL, NULL, NULL);
  this->Active4DBundleSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->Active4DBundleSelectorWidget->SetBorderWidth(2);
  this->Active4DBundleSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->Active4DBundleSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->Active4DBundleSelectorWidget->SetLabelText( "Active 4D Bundle: ");
  this->Active4DBundleSelectorWidget->SetBalloonHelpString("Select a volume from the current scene.");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->Active4DBundleSelectorWidget->GetWidgetName());

}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForHelpFrame ()
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
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "FourDImage" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForLoadFrame (int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDImage");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load / Save / Active Series");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Input File Frame
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
  // Output File Frame

  vtkKWFrameWithLabel *outFrame = vtkKWFrameWithLabel::New();
  outFrame->SetParent(conBrowsFrame->GetFrame());
  outFrame->Create();
  outFrame->SetLabelText ("Output");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 outFrame->GetWidgetName() );

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

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectOutputDirectoryButton->GetWidgetName(),
               this->SaveImageButton->GetWidgetName());


  conBrowsFrame->Delete();
  inFrame->Delete();
  outFrame->Delete();
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForFrameControlFrame(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDImage");
  
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
  // Frame Control

  vtkKWFrameWithLabel *fframe = vtkKWFrameWithLabel::New();
  fframe->SetParent(conBrowsFrame->GetFrame());
  fframe->Create();
  fframe->SetLabelText ("Frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fframe->GetWidgetName() );
  
  //vtkKWFrame *sframe = vtkKWFrame::New();
  //sframe->SetParent(fframe->GetFrame());
  //sframe->Create();
  //this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
  //               sframe->GetWidgetName() );

  vtkKWFrame *fgframe = vtkKWFrame::New();
  fgframe->SetParent(fframe->GetFrame());
  fgframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fgframe->GetWidgetName() );

  this->ForegroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->ForegroundVolumeSelectorScale->SetParent(fgframe);
  this->ForegroundVolumeSelectorScale->Create();
  this->ForegroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->ForegroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->ForegroundVolumeSelectorScale->SetLabelText("FG");
  this->ForegroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->ForegroundVolumeSelectorScale->SetResolution(1.0);
  this->ForegroundVolumeSelectorScale->ExpandEntryOff();
  //this->ForegroundVolumeSelectorScale->SetWidth(30);

  this->AutoPlayFGButton = vtkKWPushButton::New ( );
  this->AutoPlayFGButton->SetParent ( fgframe );
  this->AutoPlayFGButton->Create ( );
  this->AutoPlayFGButton->SetText (" > ");
  this->AutoPlayFGButton->SetWidth (2);

  this->Script("pack %s -side right -fill x -padx 2 -pady 2", 
               this->AutoPlayFGButton->GetWidgetName());
  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->ForegroundVolumeSelectorScale->GetWidgetName());

  vtkKWFrame *bgframe = vtkKWFrame::New();
  bgframe->SetParent(fframe->GetFrame());
  bgframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 bgframe->GetWidgetName() );

  this->BackgroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->BackgroundVolumeSelectorScale->SetParent(bgframe);
  this->BackgroundVolumeSelectorScale->Create();
  this->BackgroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->BackgroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->BackgroundVolumeSelectorScale->SetLabelText("BG");
  this->BackgroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->BackgroundVolumeSelectorScale->SetResolution(1.0);
  this->BackgroundVolumeSelectorScale->ExpandEntryOff();
  //this->BackgroundVolumeSelectorScale->SetWidth(30);
  
  this->AutoPlayBGButton = vtkKWPushButton::New ( );
  this->AutoPlayBGButton->SetParent ( bgframe );
  this->AutoPlayBGButton->Create ( );
  this->AutoPlayBGButton->SetText (" > ");
  this->AutoPlayBGButton->SetWidth (2);

  this->Script("pack %s -side right -fill x -padx 2 -pady 2", 
               this->AutoPlayBGButton->GetWidgetName());
  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->BackgroundVolumeSelectorScale->GetWidgetName());

  vtkKWFrame *apframe = vtkKWFrame::New();
  apframe->SetParent(fframe->GetFrame());
  apframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 apframe->GetWidgetName() );
  
  vtkKWLabel *frlabel1 = vtkKWLabel::New();
  frlabel1->SetParent( apframe );
  frlabel1->Create();
  frlabel1->SetText("Interval: ");

  this->AutoPlayIntervalEntry = vtkKWEntry::New ( );
  this->AutoPlayIntervalEntry->SetParent( apframe );
  this->AutoPlayIntervalEntry->Create();
  this->AutoPlayIntervalEntry->SetWidth(8);
  this->AutoPlayIntervalEntry->SetRestrictValueToDouble();
  this->AutoPlayIntervalEntry->SetValueAsDouble(1.0);

  vtkKWLabel *frlabel2 = vtkKWLabel::New();
  frlabel2->SetParent( apframe );
  frlabel2->Create();
  frlabel2->SetText(" s ");

  this->Script("pack %s %s %s -side left -fill x -padx 2 -pady 2", 
               frlabel1->GetWidgetName(),
               this->AutoPlayIntervalEntry->GetWidgetName(),
               frlabel2->GetWidgetName() );

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
  //sframe->Delete();

}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForCroppingFrame(int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDImage");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Cropping");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());


  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Node");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  // i range
  vtkKWFrame *iframe = vtkKWFrame::New();
  iframe->SetParent(cframe->GetFrame());
  iframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                iframe->GetWidgetName() );
  
  vtkKWLabel *ilabel1 = vtkKWLabel::New();
  ilabel1->SetParent(iframe);
  ilabel1->Create();
  ilabel1->SetText("i range: ");

  vtkKWLabel *ilabel2 = vtkKWLabel::New();
  ilabel2->SetParent(iframe);
  ilabel2->Create();
  ilabel2->SetText("min:");

  this->CropIMinSpinBox = vtkKWSpinBox::New();
  this->CropIMinSpinBox->SetParent(iframe);
  this->CropIMinSpinBox->Create();
  this->CropIMinSpinBox->SetWidth(3);
  this->CropIMinSpinBox->SetRange(0, 0);

  vtkKWLabel *ilabel3 = vtkKWLabel::New();
  ilabel3->SetParent(iframe);
  ilabel3->Create();
  ilabel3->SetText("max:");

  this->CropIMaxSpinBox = vtkKWSpinBox::New();
  this->CropIMaxSpinBox->SetParent(iframe);
  this->CropIMaxSpinBox->Create();
  this->CropIMaxSpinBox->SetWidth(3);
  this->CropIMaxSpinBox->SetRange(0, 0);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               ilabel1->GetWidgetName(),
               ilabel2->GetWidgetName(),
               this->CropIMinSpinBox->GetWidgetName(),
               ilabel3->GetWidgetName(),
               this->CropIMaxSpinBox->GetWidgetName());

  iframe->Delete();
  ilabel1->Delete();
  ilabel2->Delete();
  ilabel3->Delete();


  // j range

  vtkKWFrame *jframe = vtkKWFrame::New();
  jframe->SetParent(cframe->GetFrame());
  jframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                jframe->GetWidgetName() );
  
  vtkKWLabel *jlabel1 = vtkKWLabel::New();
  jlabel1->SetParent(jframe);
  jlabel1->Create();
  jlabel1->SetText("j range: ");

  vtkKWLabel *jlabel2 = vtkKWLabel::New();
  jlabel2->SetParent(jframe);
  jlabel2->Create();
  jlabel2->SetText("min:");

  this->CropJMinSpinBox = vtkKWSpinBox::New();
  this->CropJMinSpinBox->SetParent(jframe);
  this->CropJMinSpinBox->Create();
  this->CropJMinSpinBox->SetWidth(3);
  this->CropJMinSpinBox->SetRange(0, 0);

  vtkKWLabel *jlabel3 = vtkKWLabel::New();
  jlabel3->SetParent(jframe);
  jlabel3->Create();
  jlabel3->SetText("max:");

  this->CropJMaxSpinBox = vtkKWSpinBox::New();
  this->CropJMaxSpinBox->SetParent(jframe);
  this->CropJMaxSpinBox->Create();
  this->CropJMaxSpinBox->SetWidth(3);
  this->CropJMaxSpinBox->SetRange(0, 0);
  
  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               jlabel1->GetWidgetName(),
               jlabel2->GetWidgetName(),
               this->CropJMinSpinBox->GetWidgetName(),
               jlabel3->GetWidgetName(),
               this->CropJMaxSpinBox->GetWidgetName());

  jframe->Delete();
  jlabel1->Delete();
  jlabel2->Delete();
  jlabel3->Delete();


  // k range

  vtkKWFrame *kframe = vtkKWFrame::New();
  kframe->SetParent(cframe->GetFrame());
  kframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                kframe->GetWidgetName() );
  
  vtkKWLabel *klabel1 = vtkKWLabel::New();
  klabel1->SetParent(kframe);
  klabel1->Create();
  klabel1->SetText("k range: ");

  vtkKWLabel *klabel2 = vtkKWLabel::New();
  klabel2->SetParent(kframe);
  klabel2->Create();
  klabel2->SetText("min:");

  this->CropKMinSpinBox = vtkKWSpinBox::New();
  this->CropKMinSpinBox->SetParent(kframe);
  this->CropKMinSpinBox->Create();
  this->CropKMinSpinBox->SetWidth(3);
  this->CropKMinSpinBox->SetRange(0, 0);

  vtkKWLabel *klabel3 = vtkKWLabel::New();
  klabel3->SetParent(kframe);
  klabel3->Create();
  klabel3->SetText("max:");

  this->CropKMaxSpinBox = vtkKWSpinBox::New();
  this->CropKMaxSpinBox->SetParent(kframe);
  this->CropKMaxSpinBox->Create();
  this->CropKMaxSpinBox->SetWidth(3);
  this->CropKMaxSpinBox->SetRange(0, 0);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               klabel1->GetWidgetName(),
               klabel2->GetWidgetName(),
               this->CropKMinSpinBox->GetWidgetName(),
               klabel3->GetWidgetName(),
               this->CropKMaxSpinBox->GetWidgetName());

  kframe->Delete();
  klabel1->Delete();
  klabel2->Delete();
  klabel3->Delete();


  // start button

  vtkKWFrame *sframe = vtkKWFrame::New();
  sframe->SetParent(cframe->GetFrame());
  sframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                sframe->GetWidgetName() );

  this->StartCroppingButton = vtkKWPushButton::New();
  this->StartCroppingButton->SetParent(sframe);
  this->StartCroppingButton->Create();
  this->StartCroppingButton->SetText("Crop");
  this->StartCroppingButton->SetWidth (12);

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               this->StartCroppingButton->GetWidgetName());

  sframe->Delete();
}


//---------------------------------------------------------------------------
void vtkFourDImageGUI::BuildGUIForRegistrationFrame(int show)
{
  
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("FourDImage");
  
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
  
  eframe->Delete();
  

  // -----------------------------------------
  // Parameter Frame
  
  vtkKWFrameWithLabel *pframe = vtkKWFrameWithLabel::New();
  pframe->SetParent(conBrowsFrame->GetFrame());
  pframe->Create();
  pframe->SetLabelText ("Parameters");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 pframe->GetWidgetName() );


  this->AffineIterationsEntry = vtkKWEntryWithLabel::New();
  this->AffineIterationsEntry->SetParent( pframe->GetFrame());
  this->AffineIterationsEntry->SetLabelText ("(Affine) Iterations:");
  this->AffineIterationsEntry->Create();
  this->AffineIterationsEntry->GetWidget()->SetWidth(20);
  this->AffineIterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->AffineIterationsEntry->GetWidget()
    ->SetValue(this->DefaultAffineRegistrationParam[std::string("Iterations")].c_str());

  this->AffineSamplesEntry = vtkKWEntryWithLabel::New();
  this->AffineSamplesEntry->SetParent( pframe->GetFrame());
  this->AffineSamplesEntry->SetLabelText ("(Affine) Spatial Samples (1000-50000):");
  this->AffineSamplesEntry->Create();
  this->AffineSamplesEntry->GetWidget()->SetWidth(20);
  this->AffineSamplesEntry->GetWidget()->SetRestrictValueToInteger();
  this->AffineSamplesEntry->GetWidget()
    ->SetValue(this->DefaultAffineRegistrationParam[std::string("SpatialSamples")].c_str());

  this->AffineHistgramBinEntry = vtkKWEntryWithLabel::New();
  this->AffineHistgramBinEntry->SetParent( pframe->GetFrame());
  this->AffineHistgramBinEntry->SetLabelText ("(Affine) Histgrams Bin (1-500):");
  this->AffineHistgramBinEntry->Create();
  this->AffineHistgramBinEntry->GetWidget()->SetWidth(20);
  this->AffineHistgramBinEntry->GetWidget()->SetRestrictValueToInteger();
  this->AffineHistgramBinEntry->GetWidget()
    ->SetValue(this->DefaultAffineRegistrationParam[std::string("HistogramBins")].c_str());
  
  this->AffineTranslationEntry = vtkKWEntryWithLabel::New();
  this->AffineTranslationEntry->SetParent( pframe->GetFrame());
  this->AffineTranslationEntry->SetLabelText ("(Affine) Translation scaling (10-5000):");
  this->AffineTranslationEntry->Create();
  this->AffineTranslationEntry->GetWidget()->SetWidth(20);
  this->AffineTranslationEntry->GetWidget()->SetRestrictValueToInteger();
  this->AffineTranslationEntry->GetWidget()
    ->SetValue(this->DefaultAffineRegistrationParam[std::string("TranslationScale")].c_str());
  

  this->IterationsEntry = vtkKWEntryWithLabel::New();
  this->IterationsEntry->SetParent( pframe->GetFrame());
  this->IterationsEntry->SetLabelText ("(Deformable)Iterations:");
  this->IterationsEntry->Create();
  this->IterationsEntry->GetWidget()->SetWidth(20);
  this->IterationsEntry->GetWidget()->SetRestrictValueToInteger();
  this->IterationsEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("Iterations")].c_str());

  this->GridSizeEntry = vtkKWEntryWithLabel::New();
  this->GridSizeEntry->SetParent( pframe->GetFrame());
  this->GridSizeEntry->SetLabelText ("(Deformable)Grid size (3-20):");
  this->GridSizeEntry->Create();
  this->GridSizeEntry->GetWidget()->SetWidth(20);
  this->GridSizeEntry->GetWidget()->SetRestrictValueToInteger();
  this->GridSizeEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("gridSize")].c_str());

  this->HistogramBinsEntry = vtkKWEntryWithLabel::New();
  this->HistogramBinsEntry->SetParent( pframe->GetFrame());
  this->HistogramBinsEntry->SetLabelText ("(Deformable)HistogramBinEntry (1-500):");
  this->HistogramBinsEntry->Create();
  this->HistogramBinsEntry->GetWidget()->SetWidth(20);
  this->HistogramBinsEntry->GetWidget()->SetRestrictValueToInteger();
  this->HistogramBinsEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("HistogramBins")].c_str());

  this->SpatialSamplesEntry = vtkKWEntryWithLabel::New();
  this->SpatialSamplesEntry->SetParent( pframe->GetFrame());
  this->SpatialSamplesEntry->SetLabelText ("(Deformable)SpatialSamplesEntry (1000 - 500000):");
  this->SpatialSamplesEntry->Create();
  this->SpatialSamplesEntry->GetWidget()->SetWidth(20);
  this->SpatialSamplesEntry->GetWidget()->SetRestrictValueToInteger();
  this->SpatialSamplesEntry->GetWidget()->SetValue(this->DefaultRegistrationParam[std::string("SpatialSamples")].c_str());
  
  this->Script("pack %s %s %s %s %s %s %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->AffineIterationsEntry->GetWidgetName(),
               this->AffineSamplesEntry->GetWidgetName(),
               this->AffineHistgramBinEntry->GetWidgetName(),
               this->AffineTranslationEntry->GetWidgetName(),
               this->IterationsEntry->GetWidgetName(),
               this->GridSizeEntry->GetWidgetName(),
               this->HistogramBinsEntry->GetWidgetName(),
               this->SpatialSamplesEntry->GetWidgetName());

  fromLabel->Delete();
  toLabel->Delete();
  pframe->Delete();
}


//----------------------------------------------------------------------------
void vtkFourDImageGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkFourDImageGUI::SelectActive4DBundle(vtkMRML4DBundleNode* bundleNode)
{
  if (bundleNode == NULL)
    {
    return;
    }

  int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();

  // frame control
  int n = bundleNode->GetNumberOfFrames();
  this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  bundleNode->SwitchDisplayBuffer(0, volume);
  bundleNode->SwitchDisplayBuffer(1, volume);
  
  // set spin box ranges for cropping
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));
  if (volumeNode)
    {
    vtkImageData* imageData = volumeNode->GetImageData();
    int* dimensions = imageData->GetDimensions();
    
    // Cropping
    this->CropIMinSpinBox->SetRange(0, dimensions[0]-1);
    this->CropIMinSpinBox->SetValue(0);
    this->CropIMaxSpinBox->SetRange(0, dimensions[0]-1);
    this->CropIMaxSpinBox->SetValue(dimensions[0]-1);
    this->CropJMinSpinBox->SetRange(0, dimensions[1]-1);
    this->CropJMinSpinBox->SetValue(0);
    this->CropJMaxSpinBox->SetRange(0, dimensions[1]-1);
    this->CropJMaxSpinBox->SetValue(dimensions[1]-1);
    this->CropKMinSpinBox->SetRange(0, dimensions[2]-1);
    this->CropKMinSpinBox->SetValue(0);
    this->CropKMaxSpinBox->SetRange(0, dimensions[2]-1);
    this->CropKMaxSpinBox->SetValue(dimensions[2]-1);
    }

  // Registration
  this->RegistrationFixedImageIndexSpinBox->SetRange(0, n);
  this->RegistrationStartIndexSpinBox->SetRange(0, n);
  this->RegistrationEndIndexSpinBox->SetRange(0, n);

  this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  SetForeground(bundleNode->GetID(), 0);
  SetBackground(bundleNode->GetID(), 0);
}



//----------------------------------------------------------------------------
void vtkFourDImageGUI::SetForeground(const char* bundleID, int index)
{

  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
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
void vtkFourDImageGUI::SetBackground(const char* bundleID, int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
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
void vtkFourDImageGUI::SetWindowLevelForCurrentFrame()
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
void vtkFourDImageGUI::UpdateSeriesSelectorMenus()
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

  if (this->InputSeriesMenu && this->OutputSeriesMenu)
    {
    this->InputSeriesMenu->GetMenu()->DeleteAllItems();
    this->OutputSeriesMenu->GetMenu()->DeleteAllItems();
    
    std::vector<std::string>::iterator siter;
    for (siter = names.begin(); siter != names.end(); siter ++)
      {
      this->InputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      this->OutputSeriesMenu->GetMenu()->AddRadioButton((*siter).c_str());
      }
    }

  // OutputSeriesMenu has "New Series Button"
  this->OutputSeriesMenu->GetMenu()->AddRadioButton("New Series");

}


////----------------------------------------------------------------------------
//const char* vtkFourDImageGUI::GetActive4DBundleNodeID()
//{
//  vtkMRML4DBundleNode *bundleNode = 
//    vtkMRML4DBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
//  if (bundleNode)
//    {
//    return bundleNode->GetID();
//    }
//  else
//    {
//    return NULL:
//    }
//}
