/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH)
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/MotionTracker/vtkMotionTrackerGUI.cxx $
  Date:      $Date: 2009-02-10 18:26:32 -0500 (Tue, 10 Feb 2009) $
  Version:   $Revision: 3674 $ 

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkMotionTrackerGUI.h"
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
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"

#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkMRMLCameraNode.h"

#include "vtkKWPushButton.h"



#include "vtkCornerAnnotation.h"
#include "vtkMRMLLinearTransformNode.h"

// -----------

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
#include "vtkKWRadioButtonSetWithLabel.h"

#include "vtkKWProgressDialog.h"

#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"

#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWProgressGauge.h"
#include "vtkCornerAnnotation.h"
#include "vtkMRMLTimeSeriesBundleNode.h"

// for test
#include "vtkJPEGReader.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"

// 6/18/2011 ayamada
// for convert from 16bit gray scale to 8bit BGR
#define B i
#define G i+1
#define R i+2

/*
#define U (i*2)+0
#define Y1 (i*2)+1
#define V (i*2)+2
#define Y2 (i*2)+3
#define R1 (i*3)+0
#define G1 (i*3)+1
#define B1 (i*3)+2
#define R2 (i*3)+3
#define G2 (i*3)+4
#define B2 (i*3)+5
#define cut(x) (x<0?0:x>255?255:x) // based on M. Takagi.
*/

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMotionTrackerGUI );
vtkCxxRevisionMacro ( vtkMotionTrackerGUI, "$Revision: 3674 $");
//---------------------------------------------------------------------------


// 6/17/2011 ayamada
void vtkMotionTrackerGUI::Gray16toBGR24(unsigned char*& pBGR24,
 unsigned short* pGray16,
 const unsigned int width,
 const unsigned int height)
{

//int x,y;
int bgr;
int i;
//for (y=0; y<height; y++){
//for (x=0; x< width; x++){
  for (i = 0; i < width * height; i++)
{

    // 6/20/2011 ayamada
bgr = pGray16[256*256*10+i]*255/65535;

pBGR24[3*i] = bgr;
pBGR24[3*i+1] = bgr;
pBGR24[3*i+2] = bgr;

//}
}


/*

 //((UINT16 *)(src->imageData + y*src->widthStep))[x]=x*y; 
 
 int bgr;

  int r1, r2, g1, g2, b1, b2;
  int t1,t2,t3,t4,t5,t6;

  for (i = 0; i < width * height; i += 3)
  {

bgr = pGray16[i]/65536*255;

    pBGR24[i] = bgr;
    pBGR24[i+1] = bgr;
    pBGR24[i+2] = bgr;

/*
b1 = (1164*(pYUV422[Y1] - 16)                           + 2018*(pYUV422[U] - 128))/1000;
    g1 = (1164*(pYUV422[Y1] - 16) -  813*(pYUV422[V] - 128) -  391*(pYUV422[U] - 128))/1000;
    r1 = (1164*(pYUV422[Y1] - 16) + 1596*(pYUV422[V] - 128)                          )/1000;
    b2 = (1164*(pYUV422[Y2] - 16)                           + 2018*(pYUV422[U] - 128))/1000;
    g2 = (1164*(pYUV422[Y2] - 16) -  813*(pYUV422[V] - 128) -  391*(pYUV422[U] - 128))/1000;
    r2 = (1164*(pYUV422[Y2] - 16) + 1596*(pYUV422[V] - 128)                          )/1000;

    prRGB24[R1] = (unsigned char)cut(r1);
    prRGB24[G1] = (unsigned char)cut(g1);
    prRGB24[B1] = (unsigned char)cut(b1);
    prRGB24[R2] = (unsigned char)cut(r2);
    prRGB24[G2] = (unsigned char)cut(g2);
    prRGB24[B2] = (unsigned char)cut(b2);
*/
//  }

}
 



//---------------------------------------------------------------------------
vtkMotionTrackerGUI::vtkMotionTrackerGUI ( )
{

// 6/17/2011 ayamada
this->counterForShowImage = 0;

// 6/20/2011 ayamada transform node selector widget
//this->TransformNodeSelectorForSendingData = NULL;
this->TransformNodeNameEntry = NULL;
this->TransformEditorWidget = NULL;


  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkMotionTrackerGUI::DataCallback);

  this->BundleNameCount = 0;

  this->AutoPlayFG              = 0;
  this->AutoPlayBG              = 0;
  this->AutoPlayInterval        = 10;
  this->AutoPlayIntervalCounter = 0;

  //----------------------------------------------------------------
  // GUI widgets
  this->ProgressDialog = NULL;
  this->SelectInputDirectoryButton    = NULL;

  // -----------------------------------------
  // Active 4D Bundle selector
  this->ActiveTimeSeriesBundleSelectorWidget  = NULL;

  // -----------------------------------------
  // Load / Save
  this->LoadImageButton               = NULL;
  this->SelectOutputDirectoryButton   = NULL;
  this->SaveImageButton               = NULL;
  this->LoadOptionButtonSet           = NULL;
  this->LoadTimePointsEntry           = NULL;
  this->LoadSlicesEntry               = NULL;
  this->LoadChannelsEntry             = NULL;
  this->LoadFileFilterEntry           = NULL;
  this->LoadFileOrderMenu             = NULL;

  // -----------------------------------------
  // Frame control
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundVolumeSelectorScale = NULL;
  this->AutoPlayFGButton              = NULL;
  this->AutoPlayBGButton              = NULL;
  this->AutoPlayIntervalEntry         = NULL;

  // -----------------------------------------
  // Frame editor
  this->FrameList                = NULL;
  this->RemoveFrameButton        = NULL;
  this->FrameMoveUpButton        = NULL;
  this->FrameMoveDownButton      = NULL;
  this->AddFrameNodeSelector     = NULL;
  this->AddFrameNodeButton       = NULL;
  this->TimeStampMethodButtonSet = NULL;
  this->TimeStepEntry            = NULL;

  this->ImportFrameFormatEntry   = NULL;
  this->ImportFrameRangeMinEntry = NULL;
  this->ImportFrameRangeMaxEntry = NULL;
  this->ImportFrameNodeButton    = NULL;


  this->WindowLevelRange = NULL;
  this->ThresholdRange = NULL;
  this->Icons = NULL;

  //----------------------------------------------------------------
  // Time
  //this->TimerFlag = 0;


  this->TransformEditorWidget = NULL;
  
  
// 5/31/2011 ayamada
// for capture
this->StartCaptureButton = NULL;
  this->StopCaptureButton  = NULL;

  this->TimerFlag = 0;
  this->CameraActiveFlag = 0;
  this->editTransformMatrix = 0;

  this->CameraChannelLabel = NULL;
  this->CameraChannelEntry = NULL;
  this->VideoFileLabel     = NULL;
  this->VideoFileEntry     = NULL;
  this->VideoFileSelectButton = NULL;
  this->VideoSourceButtonSet = NULL;
  this->OpticalFlowStatusButtonSet = NULL;

  this->VideoImageData     = NULL;
  this->captureImageforHighGUI    = NULL;
  this->CaptureImageData     = NULL;
  this->BackgroundRenderer = NULL;

  this->OpticalFlowTrackingFlag = 0;
  this->GrayImage = NULL;
  this->PrevGrayImage = NULL;
  this->Pyramid = NULL;
  this->PrevPyramid = NULL;
  this->SwapTempImage = NULL;
  this->PyrFlag = 0;

// 6/10/2011 ayamada
this->switchImage = 0;
this->makeImageFromScanner = 0;



}


//---------------------------------------------------------------------------
vtkMotionTrackerGUI::~vtkMotionTrackerGUI ( )
{

  this->RemoveGUIObservers(); // this calls RemoveLogic Observers
  //----------------------------------------------------------------
  // Remove observers on logic
//  this->RemoveLogicObservers ( );
  this->SetLogic( vtkObjectPointer(&this->Logic), NULL );

  
  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

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
  if (this->LoadOptionButtonSet)
    {
    this->LoadOptionButtonSet->SetParent(NULL);
    this->LoadOptionButtonSet->Delete();
    }
  if (this->LoadTimePointsEntry)
    {
    this->LoadTimePointsEntry->SetParent(NULL);
    this->LoadTimePointsEntry->Delete();
    }
  if (this->LoadSlicesEntry)
    {
    this->LoadSlicesEntry->SetParent(NULL);
    this->LoadSlicesEntry->Delete();
    }
  if (this->LoadChannelsEntry)
    {
    this->LoadChannelsEntry->SetParent(NULL);
    this->LoadChannelsEntry->Delete();
    }
  if (this->LoadFileFilterEntry)
    {
    this->LoadFileFilterEntry->SetParent(NULL);
    this->LoadFileFilterEntry->Delete();
    }
  if (this->LoadFileOrderMenu)
    {
    this->LoadFileOrderMenu->SetParent(NULL);
    this->LoadFileOrderMenu->Delete();
    }

  // 4D Bundle Selector
  if (this->ActiveTimeSeriesBundleSelectorWidget)
    {
    this->ActiveTimeSeriesBundleSelectorWidget->SetParent(NULL);
    this->ActiveTimeSeriesBundleSelectorWidget->Delete();
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

  // Frame editor
  if (this->FrameList)
    {
    this->FrameList->SetParent(NULL);
    this->FrameList->Delete();
    }
  if (this->RemoveFrameButton)
    {
    this->RemoveFrameButton->SetParent(NULL);
    this->RemoveFrameButton->Delete();
    }
  if (this->FrameMoveUpButton)
    {
    this->FrameMoveUpButton->SetParent(NULL);
    this->FrameMoveUpButton->Delete();
    }
  if (this->FrameMoveDownButton)
    {
    this->FrameMoveDownButton->SetParent(NULL);
    this->FrameMoveDownButton->Delete();
    }
  if (this->AddFrameNodeSelector)
    {
    this->AddFrameNodeSelector->SetParent(NULL);
    this->AddFrameNodeSelector->Delete();
    }
  if (this->AddFrameNodeButton)
    {
    this->AddFrameNodeButton->SetParent(NULL);
    this->AddFrameNodeButton->Delete();
    }
  if (this->TimeStampMethodButtonSet)
    {
    this->TimeStampMethodButtonSet->SetParent(NULL);
    this->TimeStampMethodButtonSet->Delete();
    }
  if (this->TimeStepEntry)
    {
    this->TimeStepEntry->SetParent(NULL);
    this->TimeStepEntry->Delete();
    }

  if (this->ImportFrameFormatEntry)
    {
    this->ImportFrameFormatEntry->SetParent(NULL);
    this->ImportFrameFormatEntry->Delete();
    }
  if (this->ImportFrameRangeMinEntry)
    {
    this->ImportFrameRangeMinEntry->SetParent(NULL);
    this->ImportFrameRangeMinEntry->Delete();
    }
  if (this->ImportFrameRangeMaxEntry)
    {
    this->ImportFrameRangeMaxEntry->SetParent(NULL);
    this->ImportFrameRangeMaxEntry->Delete();
    }
  if (this->ImportFrameNodeButton)
    {
    this->ImportFrameNodeButton->SetParent(NULL);
    this->ImportFrameNodeButton->Delete();
    }

  // Icons
  if (this->Icons)
    {
    this->Icons->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

}

//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::TearDownGUI()
{
  //----------------------------------------------------------------
  // Remove Observers
  
  this->RemoveGUIObservers();
}

//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
this->TimerInterval = 300;  // 100 ms 
    ProcessTimerEvents();
    }


}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "MotionTrackerGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->ActiveTimeSeriesBundleSelectorWidget)
    {
    this->ActiveTimeSeriesBundleSelectorWidget
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }
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
  if (this->LoadOptionButtonSet)
    {
    this->LoadOptionButtonSet->GetWidget()->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->LoadOptionButtonSet->GetWidget()->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadTimePointsEntry)
    {
    this->LoadTimePointsEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadSlicesEntry)
    {
    this->LoadSlicesEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadChannelsEntry)
    {
    this->LoadChannelsEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadFileFilterEntry)
    {
    this->LoadFileFilterEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
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
  if (this->RemoveFrameButton)
    {
    this->RemoveFrameButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FrameMoveUpButton)
    {
    this->FrameMoveUpButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FrameMoveDownButton)
    {
    this->FrameMoveDownButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AddFrameNodeSelector)
    {
    this->AddFrameNodeSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AddFrameNodeButton)
    {
    this->AddFrameNodeButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (TimeStampMethodButtonSet)
    {
    TimeStampMethodButtonSet->GetWidget()->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    TimeStampMethodButtonSet->GetWidget()->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TimeStepEntry)
    {
    this->TimeStepEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ImportFrameFormatEntry)
    {
    this->ImportFrameFormatEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameRangeMinEntry)
    {
    this->ImportFrameRangeMinEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameRangeMaxEntry)
    {
    this->ImportFrameRangeMaxEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameNodeButton)
    {
    this->ImportFrameNodeButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


// 5/31/2011 ayamada
  if (this->VideoSourceButtonSet)
{
    this->VideoSourceButtonSet->GetWidget(0)
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->VideoSourceButtonSet->GetWidget(1)
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
}
  if (this->VideoFileSelectButton)
{
    this->VideoFileSelectButton
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
}
  if (this->StartCaptureButton)
{
    this->StartCaptureButton
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
}

  if (this->TransformEditorWidget)
  {
    this->TransformEditorWidget
    ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  }
  
  
  
  if (this->StopCaptureButton)
{
    this->StopCaptureButton
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
}
  if (this->OpticalFlowStatusButtonSet)
{
    this->OpticalFlowStatusButtonSet->GetWidget(0)
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->OpticalFlowStatusButtonSet->GetWidget(1)
->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
}

  this->AddLogicObservers();



  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::AddGUIObservers ( )
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

  if (this->ActiveTimeSeriesBundleSelectorWidget)
    {
    this->ActiveTimeSeriesBundleSelectorWidget
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->ActiveTimeSeriesBundleSelectorWidget
      ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }
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

  if (this->LoadOptionButtonSet)
    {
    this->LoadOptionButtonSet->GetWidget()->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->LoadOptionButtonSet->GetWidget()->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadTimePointsEntry)
    {
    this->LoadTimePointsEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadSlicesEntry)
    {
    this->LoadSlicesEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadChannelsEntry)
    {
    this->LoadChannelsEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->LoadFileFilterEntry)
    {
    this->LoadFileFilterEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
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
  if (this->RemoveFrameButton)
    {
    this->RemoveFrameButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FrameMoveUpButton)
    {
    this->FrameMoveUpButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FrameMoveDownButton)
    {
    this->FrameMoveDownButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AddFrameNodeSelector)
    {
    this->AddFrameNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->AddFrameNodeButton)
    {
    this->AddFrameNodeButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TimeStampMethodButtonSet)
    {
    this->TimeStampMethodButtonSet->GetWidget()->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->TimeStampMethodButtonSet->GetWidget()->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TimeStepEntry)
    {
    this->TimeStepEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ImportFrameFormatEntry)
    {
    this->ImportFrameFormatEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameRangeMinEntry)
    {
    this->ImportFrameRangeMinEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameRangeMaxEntry)
    {
    this->ImportFrameRangeMaxEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportFrameNodeButton)
    {
    this->ImportFrameNodeButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }


// 5/31/2011 ayamada
  if (this->VideoSourceButtonSet)
{
    this->VideoSourceButtonSet->GetWidget(0)
->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->VideoSourceButtonSet->GetWidget(1)
->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
}
  if (this->VideoFileSelectButton)
{
    this->VideoFileSelectButton
->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}
  if (this->TransformNodeSelector)
{
    this->TransformNodeSelector
->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
(vtkCommand *)this->GUICallbackCommand );
    this->TransformNodeSelector
->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
(vtkCommand *)this->GUICallbackCommand );
}
  if (this->StartCaptureButton)
{
    this->StartCaptureButton
->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}
  if (this->TransformEditorWidget)
  {
    this->TransformEditorWidget
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  }  
  if (this->StopCaptureButton)
{
    this->StopCaptureButton
->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}

  if (this->OpticalFlowStatusButtonSet)
{
    this->OpticalFlowStatusButtonSet->GetWidget(0)
->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->OpticalFlowStatusButtonSet->GetWidget(1)
->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
}




  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkMotionTrackerLogic::StatusUpdateEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
vtkMRMLTimeSeriesBundleNode *vtkMotionTrackerGUI::GetActiveTimeSeriesBundleNode()
{
  vtkMRMLTimeSeriesBundleNode *bn = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());

  if ( bn != NULL )
    {
    return (bn );
    }
  else
    {
    //--- create a new bundle node.
    bn = this->Logic->AddNewFourDBundleNode();
    if ( bn )
      {
      this->ActiveTimeSeriesBundleSelectorWidget->
        InvokeEvent ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, NULL);
      this->ActiveTimeSeriesBundleSelectorWidget->SetSelected(bn);
      return ( bn );
      }
    else
      {
      return NULL;
      }
    }
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkMotionTrackerLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::HandleMouseEvent(vtkSlicerInteractorStyle *vtkNotUsed(style))
{
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *vtkNotUsed(callData))
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }


  if (this->ActiveTimeSeriesBundleSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    
    std::cerr << "vtkSlicerNodeSelectorWidget::NodeSelectedEvent" << std::endl;

    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());

    if (bundleNode && bundleNode->GetDisplayBufferNode(0) == NULL)
      {
      this->GetLogic()->AddDisplayBufferNode(bundleNode, 0);
      }
    if (bundleNode && bundleNode->GetDisplayBufferNode(1) == NULL)
      {
      this->GetLogic()->AddDisplayBufferNode(bundleNode, 1);
      }

    SelectActiveTimeSeriesBundle(bundleNode);
    }

  else if (this->ActiveTimeSeriesBundleSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NewNodeEvent) 
    {

    std::cerr << "vtkSlicerNodeSelectorWidget::NewNodeEvent" << std::endl;

    // Do nothing here. Display Node will be added when NodeSelectEvent is issued.
    }

  else if (this->LoadImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectInputDirectoryButton->GetWidget()->GetFileName();
    const char* bundleName = this->SelectInputDirectoryButton->GetWidget()->GetText();
    this->GetLogic()->AddObserver(vtkMotionTrackerLogic::ProgressDialogEvent, 
                                  this->LogicCallbackCommand);

    vtkMRMLTimeSeriesBundleNode* newNode = NULL;
    if (this->LoadOptionButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1) // Automatic
      {
      newNode = this->GetLogic()->LoadImagesFromDir(path, bundleName);
      }
    else // Manual
      {
      int nFrames = this->LoadTimePointsEntry->GetWidget()->GetValueAsInt();
      int nSlices = this->LoadSlicesEntry->GetWidget()->GetValueAsInt();
      int nChannels = this->LoadChannelsEntry->GetWidget()->GetValueAsInt();

      const char* filter = this->LoadFileFilterEntry->GetWidget()->GetValue();
      const char* order = this->LoadFileOrderMenu->GetWidget()->GetValue();
      
      std::vector<vtkMRMLTimeSeriesBundleNode*> newNodeList;
      newNodeList.clear();
      for (int c = 0; c < nChannels; c ++)
        {
        char cBundleName[256];
        sprintf(cBundleName, "%s_%d", bundleName, c);
        newNodeList.push_back(this->GetLogic()->LoadImagesFromDir(path, cBundleName, order, filter,
                                                                  nFrames, nSlices, nChannels, 0));
        }
      if (newNodeList.size() > 0)
        {
        newNode = newNodeList[0];
        }
      }

    this->GetLogic()->RemoveObservers(vtkMotionTrackerLogic::ProgressDialogEvent,
                                      this->LogicCallbackCommand);

    //UpdateSeriesSelectorMenus();
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());

    if (newNode && bundleNode &&
        strcmp(newNode->GetID(), bundleNode->GetID()) == 0)  // new node is selected as the active bundle
      {
      this->Window = 1.0;
      this->Level  = 0.5;
      this->ThresholdUpper = 0.0;
      this->ThresholdLower = 1.0;

      SelectActiveTimeSeriesBundle(bundleNode);
      }
    }

  else if (this->SaveImageButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    const char* path = this->SelectOutputDirectoryButton->GetWidget()->GetFileName();
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      this->GetLogic()->AddObserver(vtkMotionTrackerLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
      this->GetLogic()->SetApplication(vtkSlicerApplication::SafeDownCast(this->GetApplication()));
      this->GetLogic()->SaveImagesToDir(path, bundleNode->GetID(), "out", "nrrd");
      this->GetLogic()->RemoveObservers(vtkMotionTrackerLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
      }
    }

  else if (this->LoadOptionButtonSet->GetWidget()->GetWidget(0)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->LoadOptionButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1)
    {
    // Automatic configuration
    this->LoadTimePointsEntry->EnabledOff();
    this->LoadSlicesEntry->EnabledOff();
    this->LoadChannelsEntry->EnabledOff();
    this->LoadFileFilterEntry->EnabledOff();
    this->LoadFileOrderMenu->EnabledOff();
    }

  else if (this->LoadOptionButtonSet->GetWidget()->GetWidget(1)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->LoadOptionButtonSet->GetWidget()->GetWidget(1)->GetSelectedState() == 1)
    {
    // Manual configuration
    this->LoadTimePointsEntry->EnabledOn();
    this->LoadSlicesEntry->EnabledOn();
    this->LoadChannelsEntry->EnabledOn();
    this->LoadFileFilterEntry->EnabledOn();
    this->LoadFileOrderMenu->EnabledOn();
    }

  if (this->LoadTimePointsEntry->GetWidget() == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    //double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
    }
  
  if (this->LoadSlicesEntry->GetWidget() == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    //double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
    }
  
  if (this->LoadChannelsEntry->GetWidget() == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    //double interval_s = this->AutoPlayIntervalEntry->GetValueAsDouble();
    }

  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
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
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
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
      this->AutoPlayFGButton->SetImageToIcon(this->Icons->GetPauseIcon());
      this->AutoPlayFGButton->Modified();
      }
    else
      {
      this->AutoPlayFG              = 0;
      this->AutoPlayFGButton->SetImageToIcon(this->Icons->GetPlayIcon());
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
      this->AutoPlayBGButton->SetImageToIcon(this->Icons->GetPauseIcon());
      this->AutoPlayBGButton->Modified();
      }
    else
      {
      this->AutoPlayBG              = 0;
      this->AutoPlayBGButton->SetImageToIcon(this->Icons->GetPlayIcon());
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
  
  
  
  

  if (this->FrameMoveUpButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
    if ( bundleNode == NULL )
      {
      return;
      }

    int selected = this->FrameList->GetWidget()->GetIndexOfFirstSelectedRow();
    int nframe = bundleNode->GetNumberOfFrames();

    if (selected > 0 && selected < nframe)
      {
      vtkMRMLNode* node = bundleNode->GetFrameNode(selected);
      bundleNode->RemoveFrame(selected);
      bundleNode->InsertFrame(selected-1, node->GetID());
      }
    UpdateTimeStamp(bundleNode->GetID());
    UpdateFrameList(bundleNode->GetID(), selected-1);
    }
  if (this->FrameMoveDownButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
    if (bundleNode == NULL )
      {
      return;
      }
    int selected = this->FrameList->GetWidget()->GetIndexOfFirstSelectedRow();
    int nframe = bundleNode->GetNumberOfFrames();

    if (selected < nframe-1 && selected >= 0)
      {
      vtkMRMLNode* node = bundleNode->GetFrameNode(selected);
      bundleNode->RemoveFrame(selected);
      bundleNode->InsertFrame(selected+1, node->GetID());
      }
    UpdateTimeStamp(bundleNode->GetID());
    UpdateFrameList(bundleNode->GetID(), selected+1);
    }
  if (this->RemoveFrameButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
    if (bundleNode == NULL)
      {
      return;
      }
    int selected = this->FrameList->GetWidget()->GetIndexOfFirstSelectedRow();
    int nframe = bundleNode->GetNumberOfFrames();
    if (selected < nframe && selected >= 0)
      {
      bundleNode->RemoveFrame(selected);
      }
    UpdateTimeStamp(bundleNode->GetID());
    UpdateFrameList(bundleNode->GetID(), selected-1);
    }
  if (this->AddFrameNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    }

  if (this->AddFrameNodeButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {

  // 6/10/2011 ayamada
  this->switchImage = 1;


    vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
    if (bundleNode == NULL)
      {
      return;
      }
    
    int selectedColumn = this->FrameList->GetWidget()->GetIndexOfFirstSelectedRow();
    vtkMRMLNode  *selectedVolumeNode = 
      vtkMRMLScalarVolumeNode::SafeDownCast(this->AddFrameNodeSelector->GetSelected());
    int nframe = bundleNode->GetNumberOfFrames();

    if (selectedColumn < nframe)
      {
      bundleNode->InsertFrame(selectedColumn, selectedVolumeNode->GetID());
      }
    else if (selectedColumn == nframe)
      {
      bundleNode->AddFrame(selectedVolumeNode->GetID());
      }

    if (nframe == 0)
      {
      // Tentatively, UpdateDisplayBufferNode() is called 
      this->GetLogic()->UpdateDisplayBufferNode(bundleNode, 0);
      this->GetLogic()->UpdateDisplayBufferNode(bundleNode, 1);

// ayamada
//this->GetLogic()->MRMLToOpenCV();

      }

    UpdateTimeStamp(bundleNode->GetID());
    UpdateFrameList(bundleNode->GetID(), selectedColumn + 1);

    }

  else if (this->TimeStampMethodButtonSet->GetWidget()->GetWidget(0)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->TimeStampMethodButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1)
    {
    this->TimeStepEntry->EnabledOn();
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      UpdateTimeStamp(bundleNode->GetID());
      UpdateFrameList(bundleNode->GetID());
      }
    }

  else if (this->TimeStampMethodButtonSet->GetWidget()->GetWidget(1)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->TimeStampMethodButtonSet->GetWidget()->GetWidget(1)->GetSelectedState() == 1)
    {
    this->TimeStepEntry->EnabledOff();
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      UpdateTimeStamp(bundleNode->GetID());
      UpdateFrameList(bundleNode->GetID());
      }
    }

  else if (this->TimeStepEntry == vtkKWEntry::SafeDownCast(caller)
           && event ==  vtkKWEntry::EntryValueChangedEvent)
    {
    if (this->TimeStampMethodButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1)
      {
      vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
      if (bundleNode)
        {
        return;
        }
      UpdateTimeStamp(bundleNode->GetID());
      UpdateFrameList(bundleNode->GetID());
      }
    }

  else if (this->ImportFrameFormatEntry == vtkKWEntry::SafeDownCast(caller)
           && event ==  vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->ImportFrameRangeMinEntry == vtkKWEntry::SafeDownCast(caller)
           && event ==  vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->ImportFrameRangeMaxEntry == vtkKWEntry::SafeDownCast(caller)
           && event ==  vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->ImportFrameNodeButton == vtkKWPushButton::SafeDownCast(caller)
           && event ==  vtkKWPushButton::InvokedEvent)
    {
    const char* format = this->ImportFrameFormatEntry->GetValue();
    int min            = this->ImportFrameRangeMinEntry->GetValueAsInt();
    int max            = this->ImportFrameRangeMaxEntry->GetValueAsInt();
    
    vtkMRMLTimeSeriesBundleNode *bundleNode = this->GetActiveTimeSeriesBundleNode();
    if (bundleNode == NULL)
      {
      return;
      }

    // Adjust the range
    if (min < 0)
      {
      min = 0;
      this->ImportFrameRangeMinEntry->SetValueAsInt(min);
      }
    if (min > max)
      {
      max = min;
      }

    ////// TODO: we should validate 'format' here.

    // imoprt the frames
    ImportFramesFromScene(bundleNode->GetID(), format, min, max);

    int nframe = bundleNode->GetNumberOfFrames();
    if (nframe > 0)
      {
      // Tentatively, UpdateDisplayBufferNode() is called 
      this->GetLogic()->UpdateDisplayBufferNode(bundleNode, 0);
      this->GetLogic()->UpdateDisplayBufferNode(bundleNode, 1);
      }

    UpdateTimeStamp(bundleNode->GetID());
    UpdateFrameList(bundleNode->GetID(), 0);

    }


// 5/31/2011
  if (this->VideoSourceButtonSet->GetWidget(0)
      == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent
      && this->VideoSourceButtonSet->GetWidget(0)->GetSelectedState() == 1)
{
    this->CameraChannelEntry->EnabledOn();
    this->VideoFileSelectButton->EnabledOff();
    this->VideoFileEntry->EnabledOff();
}
  else if (this->VideoSourceButtonSet->GetWidget(1)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->VideoSourceButtonSet->GetWidget(1)->GetSelectedState() == 1)
{
    this->CameraChannelEntry->EnabledOff();
    this->VideoFileSelectButton->EnabledOn();
    this->VideoFileEntry->EnabledOn();
}
  else if (this->VideoFileSelectButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
{
    vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
    fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
    fbrowse->Create();
    fbrowse->SetFileTypes("{{Video} {.mov .avi}}");
    fbrowse->MultipleSelectionOff();
    if (fbrowse->Invoke())
{
      const char* path = fbrowse->GetFileName();
      this->VideoFileEntry->SetValue(path);
}
    fbrowse->Delete();
}
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
{
    /*
 vtkMRMLCameraNode* cameraNode = 
 vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
 if (this->SecondaryViewerWindow)
 {
 this->SecondaryViewerWindow->SetCameraNode(cameraNode);
 }
 */
}
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NewNodeEvent) 
{
    /*
 vtkMRMLCameraNode* cameraNode = 
 vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
 if (this->SecondaryViewerWindow)
 {
 this->SecondaryViewerWindow->SetCameraNode(cameraNode);
 }
 */
}
  else if (this->StartCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
{
  
  // 6/22/2011 ayamada
  this->editTransformMatrix = 1;
  
    if (this->VideoSourceButtonSet->GetWidget(0)->GetSelectedState())
{ // Camera is used as a video source
      int channel = this->CameraChannelEntry->GetValueAsInt();
      if (this->StartCamera(channel, NULL))
{ // Success
        this->StartCaptureButton->EnabledOff();
        this->StopCaptureButton->EnabledOn();
}
      else
{
}
}
    else
{ // File is used as a video source
      if (this->StartCamera(-1, this->VideoFileEntry->GetValue()))
{
        this->StartCaptureButton->EnabledOff();
        this->StopCaptureButton->EnabledOn();
}
}
}
  else if (this->StopCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
{
    this->StopCamera();
    this->StartCaptureButton->EnabledOn();
    this->StopCaptureButton->EnabledOff();
}
  else if (this->OpticalFlowStatusButtonSet->GetWidget(0)
 == vtkKWRadioButton::SafeDownCast(caller)
 && event == vtkKWRadioButton::SelectedStateChangedEvent
 && this->OpticalFlowStatusButtonSet->GetWidget(0)->GetSelectedState() == 1)
{
    this->OpticalFlowTrackingFlag = 1;
}
  else if (this->OpticalFlowStatusButtonSet->GetWidget(1)
 == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->OpticalFlowStatusButtonSet->GetWidget(1)->GetSelectedState() == 1)
{
    this->OpticalFlowTrackingFlag = 0;
}


  

} 


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::DataCallback(vtkObject *vtkNotUsed(caller), 
                                    unsigned long vtkNotUsed(eid),
                                    void *clientData, void *vtkNotUsed(callData))
{
  vtkMotionTrackerGUI *self = reinterpret_cast<vtkMotionTrackerGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkMotionTrackerGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::ProcessLogicEvents ( vtkObject *vtkNotUsed(caller),
                                             unsigned long event, void *callData )
{
  if (event == vtkMotionTrackerLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkMotionTrackerLogic::ProgressDialogEvent)
    {
    vtkMotionTrackerLogic::StatusMessageType* smsg
      = (vtkMotionTrackerLogic::StatusMessageType*)callData;
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
void vtkMotionTrackerGUI::ProcessMRMLEvents ( vtkObject *vtkNotUsed(caller),
                                            unsigned long event, void *vtkNotUsed(callData))
{
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    //vtkMRMLNode* node =  vtkMRMLNode::SafeDownCast((vtkObject*)callData);
    //if (node && strcmp(node->GetNodeTagName(), "TimeSeriesBundle") == 0)
    //  {
    //  vtkMRMLTimeSeriesBundleNode *bundleNode = 
    //    vtkMRMLTimeSeriesBundleNode::SafeDownCast(node);
    //  SelectActiveTimeSeriesBundle(bundleNode);
    //  }
    }
  else if (event == vtkMRMLScene::SceneCloseEvent)
    {
    UpdateFrameList(NULL);
    }
  
  
  
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::ProcessTimerEvents()
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
          vtkMRMLTimeSeriesBundleNode *bundleNode = 
            vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
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
          vtkMRMLTimeSeriesBundleNode *bundleNode = 
            vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
          if (bundleNode)
            {
            SetBackground(bundleNode->GetID(), volume);
            }
          }
        }
      }

   // 5/31/2011 ayamada
   if (this->CameraActiveFlag)
   {
   // 6/9/2011 ayamada
   //vtkMRMLScalarVolumeNode *volumeNode = 
   //vtkMRMLScalarVolumeNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
   CameraHandler();
   }

      
      // 6/21/2011 ayamada
      // to obtain specified transform data 
      this->OriginalTrackerNode = vtkMRMLLinearTransformNode::SafeDownCast(this->TransformEditorWidget->GetTransformEditSelectorWidget()->GetSelected());    //adding at 09. 12. 24. - smkim

      if (! this->OriginalTrackerNode)
      {
        //return;
      }else{

        vtkMatrix4x4* transform;
        //transform = transformNode->GetMatrixTransformToParent();
        transform = this->OriginalTrackerNode->GetMatrixTransformToParent();
        
        
    
        if (transform)
        {
          // set volume orientation
          /*
          cameraMatrix[1] = transform->GetElement(0, 0);
          cameraMatrix[1] = transform->GetElement(0, 1);
          cameraMatrix[2] = transform->GetElement(0, 2);
          */
          transform->SetElement(0, 0, 50);
          transform->SetElement(0, 1, 50);
          transform->SetElement(0, 2, 50);
          transform->SetElement(0, 3, 50);
                    
 }
        
        
      }
      

    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUI ( )
{

  // register node type to the MRML scene
  // 4D bundle node (vtkMRMLTimeSeriesBundleNode)
  vtkMRMLScene* scene = this->GetMRMLScene();
  if ( scene )
    {
    vtkMRMLTimeSeriesBundleNode* bundleNode = vtkMRMLTimeSeriesBundleNode::New();
    scene->RegisterNodeClass(bundleNode);
    bundleNode->Delete();
    }
  
  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "MotionTracker", "MotionTracker", NULL );

  this->Icons = vtkMotionTrackerIcons::New();

  BuildGUIForHelpFrame();
  BuildGUIForActiveBundleSelectorFrame();
  BuildGUIForLoadFrame(1);
  BuildGUIForFrameControlFrame(0);
  BuildGUIForFrameFrameEditor(0);

// 5/31/2011 ayamada
  BuildGUIForWindowConfigurationFrame();
  BuildGUIForTransformation();
  
  BuildGUIForOpticalFlowFrame();


}

// 5/31/2011 ayamada
//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForWindowConfigurationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Motion Track Configuration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Video Import frame
  
  vtkKWFrame *importframe = vtkKWFrame::New();
  importframe->SetParent(conBrowsFrame->GetFrame());
  importframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
 importframe->GetWidgetName() );

  vtkKWLabel *sourceLabel = vtkKWLabel::New();
  sourceLabel->SetParent(importframe);
  sourceLabel->Create();
  sourceLabel->SetWidth(8);
  sourceLabel->SetText("Source: ");

  this->VideoSourceButtonSet = vtkKWRadioButtonSet::New();
  this->VideoSourceButtonSet->SetParent(importframe);
  this->VideoSourceButtonSet->Create();
  this->VideoSourceButtonSet->PackHorizontallyOn();
  this->VideoSourceButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->VideoSourceButtonSet->UniformColumnsOn();
  this->VideoSourceButtonSet->UniformRowsOn();

  this->VideoSourceButtonSet->AddWidget(0);
  this->VideoSourceButtonSet->GetWidget(0)->SetText("CAM");
  this->VideoSourceButtonSet->AddWidget(1);
  this->VideoSourceButtonSet->GetWidget(1)->SetText("File");
  this->VideoSourceButtonSet->GetWidget(0)->SelectedStateOn();
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              sourceLabel->GetWidgetName() , this->VideoSourceButtonSet->GetWidgetName());

  importframe->Delete();

  vtkKWFrame *camchframe = vtkKWFrame::New();
  camchframe->SetParent(conBrowsFrame->GetFrame());
  camchframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
 camchframe->GetWidgetName() );

  this->CameraChannelLabel = vtkKWLabel::New();
  this->CameraChannelLabel->SetParent(camchframe);
  this->CameraChannelLabel->Create();
  this->CameraChannelLabel->SetWidth(8);
  this->CameraChannelLabel->SetText("CAM #: ");

  this->CameraChannelEntry = vtkKWEntry::New();
  this->CameraChannelEntry->SetParent(camchframe);
  this->CameraChannelEntry->Create();
  this->CameraChannelEntry->SetRestrictValueToInteger();
  this->CameraChannelEntry->SetWidth(18);
  this->CameraChannelEntry->SetValue("0");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->CameraChannelLabel->GetWidgetName(),
              this->CameraChannelEntry->GetWidgetName());

  camchframe->Delete();
  

  vtkKWFrame* fileFrame = vtkKWFrame::New();
  fileFrame->SetParent(conBrowsFrame->GetFrame());
  fileFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
 fileFrame->GetWidgetName());

  this->VideoFileLabel = vtkKWLabel::New();
  this->VideoFileLabel->SetParent(fileFrame);
  this->VideoFileLabel->Create();
  this->VideoFileLabel->SetWidth(8);
  this->VideoFileLabel->SetText("File: ");

  this->VideoFileEntry = vtkKWEntry::New();
  this->VideoFileEntry->SetParent(fileFrame);
  this->VideoFileEntry->Create();
  this->VideoFileEntry->SetWidth(30);

  this->VideoFileSelectButton = vtkKWPushButton::New ( );
  this->VideoFileSelectButton->SetParent ( fileFrame );
  this->VideoFileSelectButton->Create ( );
  this->VideoFileSelectButton->SetText ("Browse");
  this->VideoFileSelectButton->SetWidth (12);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->VideoFileLabel->GetWidgetName(),
              this->VideoFileEntry->GetWidgetName(),
              this->VideoFileSelectButton->GetWidgetName());

  this->VideoFileLabel->EnabledOff();
  this->VideoFileEntry->EnabledOff();
  this->VideoFileSelectButton->EnabledOff();


  fileFrame->Delete();


  // -----------------------------------------
  // Push buttons

  vtkKWFrame *cvframe = vtkKWFrame::New();
  cvframe->SetParent(conBrowsFrame->GetFrame());
  cvframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
cvframe->GetWidgetName() );

  this->StartCaptureButton = vtkKWPushButton::New ( );
  this->StartCaptureButton->SetParent ( cvframe );
  this->StartCaptureButton->Create ( );
  this->StartCaptureButton->SetText ("ON");
  this->StartCaptureButton->SetWidth (12);

  this->StopCaptureButton = vtkKWPushButton::New ( );
  this->StopCaptureButton->SetParent ( cvframe );
  this->StopCaptureButton->Create ( );
  this->StopCaptureButton->SetText ("OFF");
  this->StopCaptureButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->StartCaptureButton->GetWidgetName(),
               this->StopCaptureButton->GetWidgetName());

  this->StartCaptureButton->EnabledOn();
  this->StopCaptureButton->EnabledOff();
  
  
  // -----------------------------------------
  // Transform node name entry
  // 5/6/2010 ayamada
/*
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
  
  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Transforms");
  conBrowsFrame->CollapseFrame(); // 100729-komura
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
  
  this->TransformEditorWidget = vtkSlicerTransformEditorWidget::New ( );
  this->TransformEditorWidget->SetAndObserveMRMLScene(this->GetMRMLScene() );
  this->TransformEditorWidget->SetParent ( conBrowsFrame->GetFrame() );
  this->TransformEditorWidget->Create ( );
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", this->TransformEditorWidget->GetWidgetName(), conBrowsFrame->GetFrame()->GetWidgetName());
*/  
/*  
  this->TransformNodeNameEntry = vtkKWEntryWithLabel::New();
  this->TransformNodeNameEntry->SetParent(cvframe);
  this->TransformNodeNameEntry->Create();
  this->TransformNodeNameEntry->SetWidth(40);
  this->TransformNodeNameEntry->SetLabelWidth(30);
  this->TransformNodeNameEntry->SetLabelText("Input (Transform) Node Name:");
  this->TransformNodeNameEntry->GetWidget()->SetValue ( "Tracker" );
  this->Script( "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", this->TransformNodeNameEntry->GetWidgetName());
*/
  

  cvframe->Delete();


  /*
 vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
 fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
 fbrowse->Create();
 fbrowse->SetFileTypes("{{Array data} {.txt .csv}}");
 fbrowse->MultipleSelectionOff();
 if (fbrowse->Invoke())
 {
 const char* path = fbrowse->GetFileName();
 ImportPlotNode(path);
 }
 fbrowse->Delete();
 */


  
  // -----------------------------------------
  // Push buttons

  /*
 this->ShowSecondaryWindowButton = vtkKWPushButton::New ( );
 this->ShowSecondaryWindowButton->SetParent ( switchframe );
 this->ShowSecondaryWindowButton->Create ( );
 this->ShowSecondaryWindowButton->SetText ("ON");
 this->ShowSecondaryWindowButton->SetWidth (12);
 
 this->HideSecondaryWindowButton = vtkKWPushButton::New ( );
 this->HideSecondaryWindowButton->SetParent ( switchframe );
 this->HideSecondaryWindowButton->Create ( );
 this->HideSecondaryWindowButton->SetText ("OFF");
 this->HideSecondaryWindowButton->SetWidth (12);
 
 this->Script("pack %s %s -side left -padx 2 -pady 2", 
 this->ShowSecondaryWindowButton->GetWidgetName(),
 this->HideSecondaryWindowButton->GetWidgetName());
 */

  conBrowsFrame->Delete();
  //switchframe->Delete();

}

void vtkMotionTrackerGUI::BuildGUIForTransformation()
{
  // 5/6/2010 ayamada
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
  
  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Transforms");
  conBrowsFrame->CollapseFrame(); // 100729-komura
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
  
  // -----------------------------------------
  // Test child frame
  
  this->TransformEditorWidget = vtkSlicerTransformEditorWidget::New ( );
  this->TransformEditorWidget->SetAndObserveMRMLScene(this->GetMRMLScene() );
  this->TransformEditorWidget->SetParent ( conBrowsFrame->GetFrame() );
  this->TransformEditorWidget->Create ( );
  this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", this->TransformEditorWidget->GetWidgetName(), conBrowsFrame->GetFrame()->GetWidgetName());
  
  conBrowsFrame->Delete();
  
}



//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForOpticalFlowFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Optical Flow");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Video Import frame
  
  vtkKWFrame *statusframe = vtkKWFrame::New();
  statusframe->SetParent(conBrowsFrame->GetFrame());
  statusframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
 statusframe->GetWidgetName() );

  vtkKWLabel *statusLabel = vtkKWLabel::New();
  statusLabel->SetParent(statusframe);
  statusLabel->Create();
  statusLabel->SetWidth(8);
  statusLabel->SetText("Status: ");

  this->OpticalFlowStatusButtonSet = vtkKWRadioButtonSet::New();
  this->OpticalFlowStatusButtonSet->SetParent(statusframe);
  this->OpticalFlowStatusButtonSet->Create();
  this->OpticalFlowStatusButtonSet->PackHorizontallyOn();
  this->OpticalFlowStatusButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->OpticalFlowStatusButtonSet->UniformColumnsOn();
  this->OpticalFlowStatusButtonSet->UniformRowsOn();

  this->OpticalFlowStatusButtonSet->AddWidget(0);
  this->OpticalFlowStatusButtonSet->GetWidget(0)->SetText("Enabled");
  this->OpticalFlowStatusButtonSet->AddWidget(1);
  this->OpticalFlowStatusButtonSet->GetWidget(1)->SetText("Disabled");
  this->OpticalFlowStatusButtonSet->GetWidget(1)->SelectedStateOn();
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              statusLabel->GetWidgetName() , this->OpticalFlowStatusButtonSet->GetWidgetName());
  
  statusframe->Delete();


  // -----------------------------------------
  // Node select frame

  vtkKWFrame *selectframe = vtkKWFrame::New();
  selectframe->SetParent(conBrowsFrame->GetFrame());
  selectframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
selectframe->GetWidgetName() );

  this->TransformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->TransformNodeSelector->SetParent(selectframe);
  this->TransformNodeSelector->Create();
  this->TransformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  this->TransformNodeSelector->SetNewNodeEnabled(1);
  this->TransformNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->TransformNodeSelector->SetBorderWidth(2);
  this->TransformNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->TransformNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->TransformNodeSelector->SetLabelText( "Transform : ");
  this->TransformNodeSelector->SetBalloonHelpString("Select or create a camera.");

  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
               this->TransformNodeSelector->GetWidgetName());

}



//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForActiveBundleSelectorFrame ()
{
//  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");

  //  Volume to select
  this->ActiveTimeSeriesBundleSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->ActiveTimeSeriesBundleSelectorWidget->SetParent(page);
  this->ActiveTimeSeriesBundleSelectorWidget->Create();
  this->ActiveTimeSeriesBundleSelectorWidget->SetNodeClass("vtkMRMLTimeSeriesBundleNode", NULL,
                                                           NULL, "TimeSeriesBundle");
  this->ActiveTimeSeriesBundleSelectorWidget->SetNewNodeEnabled(1);
  this->ActiveTimeSeriesBundleSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->ActiveTimeSeriesBundleSelectorWidget->SetBorderWidth(2);
  this->ActiveTimeSeriesBundleSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->ActiveTimeSeriesBundleSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->ActiveTimeSeriesBundleSelectorWidget->SetLabelText( "Active 4D Bundle: ");
  this->ActiveTimeSeriesBundleSelectorWidget->SetBalloonHelpString("Select a volume from the current scene.");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ActiveTimeSeriesBundleSelectorWidget->GetWidgetName());

}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The 4D Analysis Module** helps you to load, view and analyze a series of 3D images (4D image),"
    "such as perfusion MRI, DCE MRI, and fMRI. "
    "See <a>http://www.slicer.org/slicerWiki/index.php/Modules:MotionTracker-Documentation-3.6</a> for more details.";
  const char *about =
    "The module is designed and implemented by Junichi Tokuda, PhD (BWH), under supports from"
    "Center for Pulmonary Functional Imaging at BWH, NCIGT, NA-MIC, and NAC.";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "MotionTracker" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForLoadFrame (int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
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
  // Input File Frame
  vtkKWFrameWithLabel *inFrame = vtkKWFrameWithLabel::New();
  inFrame->SetParent(conBrowsFrame->GetFrame());
  inFrame->Create();
  inFrame->SetLabelText ("Input");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 inFrame->GetWidgetName() );
  
  vtkKWFrame *selframe = vtkKWFrame::New();
  selframe->SetParent(inFrame->GetFrame());
  selframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 0 -pady 0",
                 selframe->GetWidgetName() );

  this->SelectInputDirectoryButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectInputDirectoryButton->SetParent(selframe);
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
  this->LoadImageButton->SetParent ( selframe );
  this->LoadImageButton->Create ( );
  this->LoadImageButton->SetText ("Load Series");
  this->LoadImageButton->SetWidth (12);

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectInputDirectoryButton->GetWidgetName(),
               this->LoadImageButton->GetWidgetName());

  this->LoadOptionButtonSet = vtkKWRadioButtonSetWithLabel::New ( );
  this->LoadOptionButtonSet->SetParent ( inFrame->GetFrame() );
  this->LoadOptionButtonSet->Create ( );
  this->LoadOptionButtonSet->SetLabelText("Load option:");
  this->LoadOptionButtonSet->GetWidget()->PackHorizontallyOn ( );
  
  vtkKWRadioButton* bt0 = this->LoadOptionButtonSet->GetWidget()->AddWidget(0);
  vtkKWRadioButton* bt1 = this->LoadOptionButtonSet->GetWidget()->AddWidget(1);

  bt0->SetText("Automatic");
  bt1->SetText("Manual");
  bt0->SelectedStateOn();

  this->Script("pack %s -side top -anchor w -fill x -padx 2 -pady 2", 
               this->LoadOptionButtonSet->GetWidgetName());

  this->LoadTimePointsEntry = vtkKWEntryWithLabel::New();
  this->LoadTimePointsEntry->SetParent( inFrame->GetFrame() );
  this->LoadTimePointsEntry->Create();
  this->LoadTimePointsEntry->SetLabelText("Time points:");
  this->LoadTimePointsEntry->GetWidget()->SetWidth(10);
  this->LoadTimePointsEntry->GetWidget()->SetRestrictValueToInteger();
  this->LoadTimePointsEntry->GetWidget()->SetValueAsInt(1);
  this->LoadTimePointsEntry->EnabledOff();
  this->LoadTimePointsEntry->ExpandWidgetOff();
  this->LoadTimePointsEntry->SetLabelWidth(18);
  
  this->LoadSlicesEntry = vtkKWEntryWithLabel::New();
  this->LoadSlicesEntry->SetParent( inFrame->GetFrame() );
  this->LoadSlicesEntry->Create();
  this->LoadSlicesEntry->SetLabelText("# of slices:");
  this->LoadSlicesEntry->GetWidget()->SetWidth(10);
  this->LoadSlicesEntry->GetWidget()->SetRestrictValueToInteger();
  this->LoadSlicesEntry->GetWidget()->SetValueAsInt(1);
  this->LoadSlicesEntry->EnabledOff();
  this->LoadSlicesEntry->ExpandWidgetOff();
  this->LoadSlicesEntry->SetLabelWidth(18);

  this->LoadChannelsEntry = vtkKWEntryWithLabel::New();
  this->LoadChannelsEntry->SetParent( inFrame->GetFrame() );
  this->LoadChannelsEntry->Create();
  this->LoadChannelsEntry->SetLabelText("# of channels:");
  this->LoadChannelsEntry->GetWidget()->SetWidth(10);
  this->LoadChannelsEntry->GetWidget()->SetRestrictValueToInteger();
  this->LoadChannelsEntry->GetWidget()->SetValueAsInt(1);
  this->LoadChannelsEntry->EnabledOff();
  this->LoadChannelsEntry->ExpandWidgetOff();
  this->LoadChannelsEntry->SetLabelWidth(18);

  this->LoadFileFilterEntry = vtkKWEntryWithLabel::New();
  this->LoadFileFilterEntry->SetParent( inFrame->GetFrame() );
  this->LoadFileFilterEntry->Create();
  this->LoadFileFilterEntry->SetLabelText("Filter:");
  this->LoadFileFilterEntry->GetWidget()->SetWidth(10);
  this->LoadFileFilterEntry->GetWidget()->SetValue("*");
  this->LoadFileFilterEntry->EnabledOff();
  this->LoadFileFilterEntry->ExpandWidgetOff();
  this->LoadFileFilterEntry->SetLabelWidth(18);

  this->LoadFileOrderMenu = vtkKWMenuButtonWithLabel::New();
  this->LoadFileOrderMenu->SetParent(inFrame->GetFrame());
  this->LoadFileOrderMenu->Create();
  this->LoadFileOrderMenu->SetLabelText("Ordering:");
  this->LoadFileOrderMenu->GetWidget()->SetWidth(40);
  this->LoadFileOrderMenu->EnabledOff();
  this->LoadFileOrderMenu->ExpandWidgetOff();
  this->LoadFileOrderMenu->SetLabelWidth(18);
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("T-S-C");
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("T-C-S");
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("S-T-C");
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("S-C-T");
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("C-T-S");
  this->LoadFileOrderMenu->GetWidget()->GetMenu()->AddRadioButton("C-S-T");
  this->LoadFileOrderMenu->GetWidget()->SetValue("T-S-C");

  this->Script("pack %s %s %s %s %s -side top -anchor w -fill x -padx 20 -pady 2", 
               this->LoadTimePointsEntry->GetWidgetName(),
               this->LoadSlicesEntry->GetWidgetName(),
               this->LoadChannelsEntry->GetWidgetName(),
               this->LoadFileFilterEntry->GetWidgetName(),
               this->LoadFileOrderMenu->GetWidgetName());

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


  selframe->Delete();
  conBrowsFrame->Delete();
  inFrame->Delete();
  outFrame->Delete();
}


//---------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForFrameControlFrame(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Control");
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
  this->AutoPlayFGButton->SetImageToIcon(this->Icons->GetPlayIcon());
  //this->AutoPlayFGButton->SetWidth (10);

  this->Script("pack %s -side right -anchor w -padx 2 -pady 2", 
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
  this->AutoPlayBGButton->SetImageToIcon(this->Icons->GetPlayIcon());
  //this->AutoPlayBGButton->SetWidth (10);

  this->Script("pack %s -side right -anchor w -padx 2 -pady 2", 
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
  fgframe->Delete();
  bgframe->Delete();
  apframe->Delete();
  frlabel1->Delete();
  frlabel2->Delete();
  cframe->Delete();
  lwframe->Delete();
  lwLabel->Delete();
  thframe->Delete();
  thLabel->Delete();
  //sframe->Delete();

}

//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::BuildGUIForFrameFrameEditor(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("MotionTracker");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Editor");
  // 6/20/2011 ayamada
/*
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
 */
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Frame Editor

  vtkKWFrameWithLabel *listframe = vtkKWFrameWithLabel::New();
  listframe->SetParent(conBrowsFrame->GetFrame());
  listframe->Create();
  listframe->SetLabelText ("Frame List");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 listframe->GetWidgetName() );
  
  this->FrameList = vtkKWMultiColumnListWithScrollbars::New();
  this->FrameList->SetParent(listframe->GetFrame());
  this->FrameList->Create();
  this->FrameList->GetWidget()->SetHeight(10);
  this->FrameList->GetWidget()->SetSelectionTypeToRow();
  this->FrameList->GetWidget()->SetSelectionModeToSingle();
  this->FrameList->GetWidget()->MovableRowsOff();
  this->FrameList->GetWidget()->MovableColumnsOff();
  this->FrameList->GetWidget()->AddColumn("Frame #");
  this->FrameList->GetWidget()->AddColumn("Node name");
  this->FrameList->GetWidget()->AddColumn("Time stamp");
  this->FrameList->GetWidget()->SetColumnWidth(0, 10);
  this->FrameList->GetWidget()->SetColumnWidth(1, 20);
  this->FrameList->GetWidget()->SetColumnWidth(2, 10);
  this->FrameList->GetWidget()->SetColumnAlignmentToLeft(1);
  this->FrameList->GetWidget()->ColumnEditableOff(0);
  this->FrameList->GetWidget()->SetSelectionTypeToRow();
  this->FrameList->GetWidget()->SetSelectionModeToSingle();
  this->FrameList->GetWidget()->MovableRowsOff();
  this->FrameList->GetWidget()->MovableColumnsOff();
  this->FrameList->GetWidget()->ColumnEditableOn(2);

  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 this->FrameList->GetWidgetName() );

  vtkKWFrame *moveframe = vtkKWFrame::New();
  moveframe->SetParent(listframe->GetFrame());
  moveframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 moveframe->GetWidgetName() );

  this->FrameMoveUpButton = vtkKWPushButton::New ( );
  this->FrameMoveUpButton->SetParent ( moveframe );
  this->FrameMoveUpButton->Create ( );
  this->FrameMoveUpButton->SetText ("Move up");
  this->FrameMoveUpButton->SetWidth (9);

  this->FrameMoveDownButton = vtkKWPushButton::New ( );
  this->FrameMoveDownButton->SetParent ( moveframe );
  this->FrameMoveDownButton->Create ( );
  this->FrameMoveDownButton->SetText ("Move down");
  this->FrameMoveDownButton->SetWidth (9);

  this->RemoveFrameButton = vtkKWPushButton::New ( );
  this->RemoveFrameButton->SetParent ( moveframe );
  this->RemoveFrameButton->Create ( );
  this->RemoveFrameButton->SetText ("Remove");
  this->RemoveFrameButton->SetWidth (9);

  this->Script("pack %s %s %s -side left -fill x -expand y -padx 2 -pady 2", 
               this->FrameMoveUpButton->GetWidgetName(),
               this->FrameMoveDownButton->GetWidgetName(),
               this->RemoveFrameButton->GetWidgetName());

  vtkKWFrame *addframe = vtkKWFrame::New();
  addframe->SetParent(listframe->GetFrame());
  addframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 addframe->GetWidgetName() );
  
  this->AddFrameNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->AddFrameNodeSelector->SetParent(addframe);
  this->AddFrameNodeSelector->Create();
  this->AddFrameNodeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->AddFrameNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->AddFrameNodeSelector->SetBorderWidth(2);
  this->AddFrameNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->AddFrameNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->AddFrameNodeSelector->SetLabelText( "Add node : ");
  this->AddFrameNodeSelector->SetBalloonHelpString("Select a volume to add from the current scene.");
  
  this->AddFrameNodeButton = vtkKWPushButton::New ( );
  this->AddFrameNodeButton->SetParent ( addframe );
  this->AddFrameNodeButton->Create ( );
  this->AddFrameNodeButton->SetText ("Add");
  this->AddFrameNodeButton->SetWidth (5);

  this->Script("pack %s -side right -anchor w -padx 2 -pady 2",
               this->AddFrameNodeButton->GetWidgetName());
  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->AddFrameNodeSelector->GetWidgetName());

  vtkKWFrame *tsframe = vtkKWFrame::New();
  tsframe->SetParent(listframe->GetFrame());
  tsframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 tsframe->GetWidgetName() );

  this->TimeStampMethodButtonSet = vtkKWRadioButtonSetWithLabel::New ( );
  this->TimeStampMethodButtonSet->SetParent ( tsframe );
  this->TimeStampMethodButtonSet->Create ( );
  this->TimeStampMethodButtonSet->SetLabelText ("Time step:");
  this->TimeStampMethodButtonSet->GetWidget()->PackHorizontallyOn ( );
  
  vtkKWRadioButton* bt0 = this->TimeStampMethodButtonSet->GetWidget()->AddWidget(0);
  vtkKWRadioButton* bt1 = this->TimeStampMethodButtonSet->GetWidget()->AddWidget(1);

  bt0->SetText("Uniform");
  bt1->SetText("Non-uniform");
  bt1->SelectedStateOn();

  this->Script("pack %s -side left -fill x -expand y -padx 2 -pady 2", 
               this->TimeStampMethodButtonSet->GetWidgetName());

  vtkKWFrame *tseframe = vtkKWFrame::New();
  tseframe->SetParent(listframe->GetFrame());
  tseframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 tseframe->GetWidgetName() );

  vtkKWLabel *tslabell = vtkKWLabel::New();
  tslabell->SetParent(tseframe);
  tslabell->Create();
  tslabell->SetText("Step:");
  
  this->TimeStepEntry = vtkKWEntry::New();
  this->TimeStepEntry->SetParent(tseframe);
  this->TimeStepEntry->SetRestrictValueToInteger();
  this->TimeStepEntry->Create();
  this->TimeStepEntry->SetWidth(8);
  this->TimeStepEntry->SetRestrictValueToInteger();
  this->TimeStepEntry->SetValueAsInt(0);
  
  vtkKWLabel *tslabelr = vtkKWLabel::New();
  tslabelr->SetParent(tseframe);
  tslabelr->Create();
  tslabelr->SetText("ms");

  this->Script("pack %s %s %s -side right -anchor w -padx 2 -pady 2",
               tslabelr->GetWidgetName(),
               this->TimeStepEntry->GetWidgetName(),
               tslabell->GetWidgetName());

  vtkKWFrameWithLabel *importframe = vtkKWFrameWithLabel::New();
  importframe->SetParent(conBrowsFrame->GetFrame());
  importframe->Create();
  importframe->SetLabelText ("Import frames");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 importframe->GetWidgetName() );
  
  vtkKWFrame *formatframe = vtkKWFrame::New();
  formatframe->SetParent(importframe->GetFrame());
  formatframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 formatframe->GetWidgetName() );

  vtkKWLabel *formatlabel = vtkKWLabel::New();
  formatlabel->SetParent(formatframe);
  formatlabel->Create();
  formatlabel->SetText("Node name:");

  this->ImportFrameFormatEntry = vtkKWEntry::New();
  this->ImportFrameFormatEntry->SetParent(formatframe);
  this->ImportFrameFormatEntry->Create();
  this->ImportFrameFormatEntry->SetWidth(8);
  this->ImportFrameFormatEntry->SetValue("IMAGE_%03d");

  this->Script("pack %s -side left -anchor w -padx 2 -pady 2",
               formatlabel->GetWidgetName());

  this->Script("pack %s -side left -fill x -expand y -padx 2 -pady 2", 
               this->ImportFrameFormatEntry->GetWidgetName());
  

  vtkKWFrame *rangeframe = vtkKWFrame::New();
  rangeframe->SetParent(importframe->GetFrame());
  rangeframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 rangeframe->GetWidgetName() );

  vtkKWLabel *rangelabel1 = vtkKWLabel::New();
  rangelabel1->SetParent(rangeframe);
  rangelabel1->Create();
  rangelabel1->SetText("Range:  from ");

  this->ImportFrameRangeMinEntry = vtkKWEntry::New();
  this->ImportFrameRangeMinEntry->SetParent(rangeframe);
  this->ImportFrameRangeMinEntry->SetRestrictValueToInteger();
  this->ImportFrameRangeMinEntry->Create();
  this->ImportFrameRangeMinEntry->SetWidth(4);
  this->ImportFrameRangeMinEntry->SetValueAsInt(0);

  vtkKWLabel *rangelabel2 = vtkKWLabel::New();
  rangelabel2->SetParent(rangeframe);
  rangelabel2->Create();
  rangelabel2->SetText(" to ");

  this->ImportFrameRangeMaxEntry = vtkKWEntry::New();
  this->ImportFrameRangeMaxEntry->SetParent(rangeframe);
  this->ImportFrameRangeMaxEntry->SetRestrictValueToInteger();
  this->ImportFrameRangeMaxEntry->Create();
  this->ImportFrameRangeMaxEntry->SetWidth(4);
  this->ImportFrameRangeMaxEntry->SetValueAsInt(0);

  this->ImportFrameNodeButton = vtkKWPushButton::New ( );
  this->ImportFrameNodeButton->SetParent ( rangeframe );
  this->ImportFrameNodeButton->Create ( );
  this->ImportFrameNodeButton->SetText ("Import");
  this->ImportFrameNodeButton->SetWidth (6);

  this->Script("pack %s %s %s %s %s -side left -anchor w -padx 2 -pady 2",
               rangelabel1->GetWidgetName(),
               this->ImportFrameRangeMinEntry->GetWidgetName(),
               rangelabel2->GetWidgetName(),
               this->ImportFrameRangeMaxEntry->GetWidgetName(),
               this->ImportFrameNodeButton->GetWidgetName());

  conBrowsFrame->Delete();
  listframe->Delete();
  moveframe->Delete();
  addframe->Delete();
  tsframe->Delete();
  tseframe->Delete();
  tslabell->Delete();
  tslabelr->Delete();
  importframe->Delete();
  formatframe->Delete();
  formatlabel->Delete();
  rangeframe->Delete();
  rangelabel1->Delete();
  rangelabel2->Delete();

}


//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::SelectActiveTimeSeriesBundle(vtkMRMLTimeSeriesBundleNode* bundleNode)
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

  SetForeground(bundleNode->GetID(), 0);
  SetBackground(bundleNode->GetID(), 0);

  UpdateFrameList(bundleNode->GetID());
  
}



//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::SetForeground(const char* bundleID, int index)
{

  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

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
    std::cerr << "volume node name  = " <<  volNode->GetName() << std::endl;
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
void vtkMotionTrackerGUI::SetBackground(const char* bundleID, int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

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
void vtkMotionTrackerGUI::SetWindowLevelForCurrentFrame()
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
        double r[2];
        fgNode->GetImageData()->GetScalarRange(r);
        //double lower = this->RangeLower;
        //double upper = this->RangeUpper;
        double lower = r[0];
        double upper = r[1];
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
        double r[2];
        bgNode->GetImageData()->GetScalarRange(r);
        //double lower = this->RangeLower;
        //double upper = this->RangeUpper;
        double lower = r[0];
        double upper = r[1];
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
//void vtkMotionTrackerGUI::UpdateSeriesSelectorMenus()
//{
//
//  // generate a list of 4D Bundles
//  std::vector<vtkMRMLNode*> nodes;
//  std::vector<std::string>  names;
//
//  this->GetApplicationLogic()->GetMRMLScene()->GetNodesByClass("vtkMRMLTimeSeriesBundleNode", nodes);
//
//  this->BundleNodeIDList.clear();
//  names.clear();
//  std::vector<vtkMRMLNode*>::iterator niter;
//  for (niter = nodes.begin(); niter != nodes.end(); niter ++)
//    {
//    this->BundleNodeIDList.push_back((*niter)->GetID());
//    names.push_back((*niter)->GetName());
//    }
//
//}


//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::UpdateFrameList(const char* bundleID, int selectColumn)
{
  int selected;
  if (selectColumn < 0)
    {
    selected = this->FrameList->GetWidget()->GetIndexOfFirstSelectedRow();
    }
  else
    {
    selected = selectColumn;
    }

  if (bundleID == NULL)
    {
    this->FrameList->GetWidget()->DeleteAllRows();
    }

  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return;
    }
  
  if (!this->FrameList)
    {
    return;
    }

  // Adjust number of rows
  // The last one row is for adding a new frame at the end of the list
  int numFrames = bundleNode->GetNumberOfFrames();
  int numRows = this->FrameList->GetWidget()->GetNumberOfRows();
  if (numRows < numFrames+1)
    {
    this->FrameList->GetWidget()->AddRows(numFrames+1 - numRows);
    }
  else if (numRows > numFrames+1)
    {
    int ndel = numRows - (numFrames+1);
    for (int i = 0; i < ndel; i ++)
      {
      this->FrameList->GetWidget()->DeleteRow(0);
      }
    }

  // Show info
  int timestamp;
  char str[256];
  for (int i = 0; i < numFrames; i ++)
    {
    vtkMRMLVolumeNode* volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
    this->FrameList->GetWidget()->SetCellTextAsInt(i, 0, i);
    this->FrameList->GetWidget()->SetCellText(i, 1, volNode->GetName());
    vtkMRMLTimeSeriesBundleNode::TimeStamp ts;
    timestamp = bundleNode->GetTimeStamp(i, &ts);
    if ( timestamp > 0 )
      {
      double tm = (double)ts.second + (double)ts.nanosecond / 1000000000.0;
      sprintf(str, "%f", tm);
      this->FrameList->GetWidget()->SetCellText(i, 2, str);
      }
    else
      {
      this->FrameList->GetWidget()->SetCellText(i, 2, "0.000");
      }
    }

  // The last one row ("new" row)
  this->FrameList->GetWidget()->SetCellText(numFrames, 0, "---");
  this->FrameList->GetWidget()->SetCellText(numFrames, 1, "(New)");
  this->FrameList->GetWidget()->SetCellText(numFrames, 2, "---");

  if (selected < 0)
    {
    selected = 0;
    }
  else if (selected > numFrames)
    {
    selected = numFrames-1;
    }
  this->FrameList->GetWidget()->ClearSelection();
  this->FrameList->GetWidget()->SelectRow(selected);
  
  // Make sure that the 3rd column is editable
  this->FrameList->GetWidget()->ColumnEditableOn(2);

  // Update frame control
  int n = bundleNode->GetNumberOfFrames();
  this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);

}

//----------------------------------------------------------------------------
void vtkMotionTrackerGUI::UpdateTimeStamp(const char* bundleID)
{
  // int step = 1000000000;  // ns
  int step_s  = this->TimeStepEntry->GetValueAsInt() / 1000;    // ms -> s
  int step_ns = (this->TimeStepEntry->GetValueAsInt() % 1000) * 1000000; // ms -> ns

  // Get 4D bundle node
  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));
  int numFrames = bundleNode->GetNumberOfFrames();

  // Get the method
  bool fUniform = this->TimeStampMethodButtonSet->GetWidget()->GetWidget(0)->GetSelectedState();

  if (fUniform == 1)    // uniform time-stamp distribution
    {
    vtkMRMLTimeSeriesBundleNode::TimeStamp ts;
    ts.second = 0;
    ts.nanosecond = 0;

    for (int i = 0; i < numFrames; i ++)
      {
      bundleNode->SetTimeStamp(i, &ts);

      ts.nanosecond += step_ns;
      ts.second     += step_s + ts.nanosecond / 1000000000;
      ts.nanosecond %= 1000000000;
      
      }
    }
  else    // non-uniform time-stamp distribution (under development)
    {
    }
}


//----------------------------------------------------------------------------
int vtkMotionTrackerGUI::ImportFramesFromScene(const char* bundleID, const char* format, int min, int max)
{

  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return 0;
    }
  
  // check the ragen of the numbers
  if (min < 0 || min > max)
    {
    return 0;
    }

  for (int i = min; i <= max; i ++)
    {
    char nodename[256];
    sprintf(nodename, format, i);

    // The module always takes the first node in the list, if there are multiple files with a same name.
    vtkCollection* collection = this->GetMRMLScene()->GetNodesByName(nodename);
    int nItems = collection->GetNumberOfItems();
    if (nItems > 0)
      {
      vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(collection->GetItemAsObject(0));
      if (node)
        {
        bundleNode->AddFrame(node->GetID());
        }
      }
    }

  return 1;

}


//----------------------------------------------------------------------------
const char* vtkMotionTrackerGUI::GetActiveTimeSeriesBundleNodeID()
{
  vtkMRMLTimeSeriesBundleNode *bundleNode = 
    vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->ActiveTimeSeriesBundleSelectorWidget->GetSelected());
  if (bundleNode)
    {
    return bundleNode->GetID();
    }
  else
    {
    return NULL;
    }
}



// 5/31/2011 ayamada
//----------------------------------------------------------------------------
int vtkMotionTrackerGUI::ViewerBackgroundOn(vtkSlicerViewerWidget* vwidget, vtkImageData* imageData)
{
  
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
{
    if (rwidget->GetNumberOfRenderers() == 1)
{
      this->BackgroundRenderer = vtkRenderer::New();
      this->BackgroundActor = vtkImageActor::New();
      this->BackgroundActor->SetInput(imageData);
      this->BackgroundRenderer->AddActor(this->BackgroundActor);
      this->BackgroundRenderer->InteractiveOff();
      this->BackgroundRenderer->SetLayer(0);

      // Adjust camera position so that image covers the draw area.

      this->BackgroundActor->Modified();
      rwidget->GetNthRenderer(0)->SetLayer(1);
      rwidget->AddRenderer(this->BackgroundRenderer);
      rwindow->Render();

      vtkCamera* camera = this->BackgroundRenderer->GetActiveCamera();
      double x, y, z;
      camera->GetPosition(x, y, z);
      camera->SetViewAngle(90.0);//90.0
      camera->SetPosition(x, y, y); 

      // The following code fixes a issue that
      // video doesn't show up on the viewer.
      vtkCamera* fcamera = rwidget->GetNthRenderer(0)->GetActiveCamera();
      if (fcamera)
{
        fcamera->Modified();
}

      return 1;
}
}
  return 0;
}


//----------------------------------------------------------------------------
int vtkMotionTrackerGUI::ViewerBackgroundOff(vtkSlicerViewerWidget* vwidget)
{
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
{
    if (rwidget->GetNumberOfRenderers() > 1)
{
      rwidget->RemoveNthRenderer(1);
      rwidget->GetNthRenderer(0)->SetLayer(0);
      rwindow->Render();
      this->BackgroundRenderer = NULL;
      this->BackgroundActor = NULL;
}
}

  return 0;

}  




//----------------------------------------------------------------------------
// Start Camera
// if channel = -1, OpenCV will read image from the video file specified by path
int vtkMotionTrackerGUI::StartCamera(int channel, const char* path)
{

  this->capture      = NULL;
  this->captureImage = NULL;
  this->RGBImage     = NULL;
  this->undistortionImage = NULL;
  this->imageSize.width = 0;
  this->imageSize.height = 0;

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);

  if (channel < 0 && path != NULL)
{
    this->capture = cvCaptureFromAVI( path );
}
  else 
{
    this->capture = cvCaptureFromCAM(channel);
}

  if (this->capture == NULL)
{
    return 0;
}


  this->CameraActiveFlag = 1;

  if (!this->VideoImageData)
{
// initialization for obtain image from OpenCV
// 6/12/2011 ayamada
    this->VideoImageData = vtkImageData::New();
    this->VideoImageData->SetDimensions(64, 64, 1);
    this->VideoImageData->SetExtent(0, 63, 0, 63, 0, 0 );
    this->VideoImageData->SetSpacing(1.0, 1.0, 1.0);
    this->VideoImageData->SetOrigin(0.0, 0.0, 0.0);
    this->VideoImageData->SetNumberOfScalarComponents(3);
    this->VideoImageData->SetScalarTypeToUnsignedChar();
    this->VideoImageData->AllocateScalars();
}
  this->VideoImageData->Update();
  ViewerBackgroundOn(vwidget, this->VideoImageData);

  return 1;
} 


//----------------------------------------------------------------------------
// Stop Camera thread
int vtkMotionTrackerGUI::StopCamera()
{
  this->CameraActiveFlag = 0;

  cvReleaseCapture(&(this->capture));

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
  ViewerBackgroundOff(vwidget);

  return 1;
} 


#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
int vtkMotionTrackerGUI::CameraHandler()
{
  IplImage* captureImageTmp = NULL;
  //IplImage* contour_gray = NULL;
  
  CvSize   newImageSize;
  CvSize   newCaptureImageSize;


  if (this->capture)
  {

    // 5/15/2010 ayamada
    if(NULL == (captureImageTmp = cvQueryFrame( capture )))
    {
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      return 0;
    }

    // 5/6/2010 ayamada creating RGB image and capture image
    newCaptureImageSize = cvGetSize( captureImageTmp );
    newImageSize = cvGetSize( captureImageTmp );
  
  vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->AddFrameNodeSelector->GetSelected());
  vtkImageData* imageData = volumeNode->GetImageData();
  this->CaptureImageData = volumeNode->GetImageData();
  
  int vImageSize[3];
  
  // 5/6/2010 ayamada creating RGB image and capture image
  this->CaptureImageData->GetDimensions(vImageSize);
  imageData->GetDimensions(vImageSize);
  
  // check if the image size is differ from the initial one
  if (
            
      vImageSize[0] != this->imageSize.width ||
      vImageSize[1] != this->imageSize.height
      
      )
  
  {
    
    this->imageSize.width = 256;//newImageSize.width;
    this->imageSize.height = 256;//newImageSize.height;
    newImageSize.width = 256;
    newImageSize.height = 256;
    
    
    this->VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
    this->VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
    this->VideoImageData->SetNumberOfScalarComponents(3);
    this->VideoImageData->SetScalarTypeToUnsignedChar();
    this->VideoImageData->AllocateScalars();
    this->VideoImageData->Update();
    
    // 6/17/2011 ayamada
    // for 16bit unsigned image
    this->CaptureImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
    //this->CaptureImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
    this->CaptureImageData->SetNumberOfScalarComponents(1);
    // 6/19/2011 ayamada
    this->CaptureImageData->SetScalarTypeToShort();
    this->CaptureImageData->AllocateScalars();
    this->CaptureImageData->Update();
    
    // 6/19/2011 ayamada IplImage structure
    this->ImageFromScannerTmp = cvCreateImage(this->imageSize, IPL_DEPTH_8U, 3);
    this->ImageFromScanner = cvCreateImage(this->imageSize, IPL_DEPTH_8U, 3);
    this->captureImage = cvCreateImage(this->imageSize, IPL_DEPTH_8U,3);
    this->RGBImage = cvCreateImage(this->imageSize, IPL_DEPTH_8U, 3);
    this->captureImageforHighGUI = cvCreateImage(this->imageSize, IPL_DEPTH_8U, 3);
    this->undistortionImage = cvCreateImage( this->imageSize, IPL_DEPTH_8U, 3);


    
    vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
    ViewerBackgroundOff(vwidget);
    ViewerBackgroundOn(vwidget, this->VideoImageData);

  }

  
  // 6/20/2011 ayamada
  IplImage* contour_gray = cvCreateImage( this->imageSize, IPL_DEPTH_8U, 1);
  
  this->CaptureImageData = volumeNode->GetImageData();
  
  // convert coloer from Gray16 to BGR24
  unsigned char* pOut = (unsigned char*)this->ImageFromScannerTmp->imageData;
  unsigned short* pIn = (unsigned short*) imageData->GetScalarPointer();
  this->Gray16toBGR24(pOut,pIn,256,256);
  
  
  int dsize = newImageSize.width*newImageSize.height*3;
  cvFlip(this->ImageFromScannerTmp, this->ImageFromScanner,-1);
  
  /*
  cvCvtColor((void*)this->ImageFromScanner, contour_gray, CV_RGB2GRAY);
  cvAdaptiveThreshold (contour_gray, contour_gray, 255, 
                       CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 5.0); 
  
  cvSmooth(contour_gray,contour_gray, CV_MEDIAN, 5);
  
  cvCvtColor(contour_gray, this->ImageFromScanner, CV_GRAY2RGB);
  */
  
  memcpy((void*)this->VideoImageData->GetScalarPointer(), (void*)this->ImageFromScanner->imageData, 256*256*3);
  
  if (this->VideoImageData && this->BackgroundRenderer)
  {
    this->VideoImageData->Modified();
    this->BackgroundRenderer->GetRenderWindow()->Render();
  
  }

//  cvReleaseImage(&contour_gray);
//  cvReleaseImage(&captureImageTmp);

  
}
  
  
  return 1;

}


//----------------------------------------------------------------------------
int vtkMotionTrackerGUI::ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n)
{
  float threshold = 5.0;
  CvPoint2D32f mean;

  mean.x = 0.0;
  mean.y = 0.0;

  // Use 10% vectors to calculate translation
  for (int i = 0; i < n; i ++)
{
    float x = vector[i].x;
    float y = vector[i].y;
    float len = sqrtf(x*x + y*y);
    if (len > threshold)
{
      mean.x += x;
      mean.y += y;
}
}
  mean.x /= (float)n;
  mean.y /= (float)n;

  vtkMRMLLinearTransformNode* transformNode = 
vtkMRMLLinearTransformNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
  if (transformNode)
{
    vtkMatrix4x4* transform = transformNode->GetMatrixTransformToParent();
    if (transform)
{
      float x = transform->GetElement(0, 3) - mean.x*2.0;
      float y = transform->GetElement(2, 3) + mean.y*2.0;
      transform->SetElement(0, 3, x);
      transform->SetElement(2, 3, y);
}
}

  return 1;
}


