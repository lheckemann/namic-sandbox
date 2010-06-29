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

#include "vtkStereoCalibGUI.h"
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

// #include "vtkStereoCalibCVClass.h" // 100607-komura
//chessboard
// std::vector<uchar> active[2];        // 
// std::vector<CvPoint2D32f> points[2]; // 100603-komura
//

// #include "vtkppr.h"
#include "vtkProperty.h"        // 100616-komura

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkStereoCalibGUI );
vtkCxxRevisionMacro ( vtkStereoCalibGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkStereoCalibGUI::vtkStereoCalibGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkStereoCalibGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->displayChessboardButton = NULL;
  this->TestButton22 = NULL;
  this->TestButton31 = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
 
  this->SecondaryViewerWindow = NULL;
  this->SecondaryViewerWindow2x = NULL;
 
  //----------------------------------------------------------------
  // cameraFacusPlane                                    // 
  // FocalPlaneSource = vtkPlaneSource::New();           // 
  // FocalPlaneMapper = vtkPolyDataMapper::New();        // 
  for(int i=0;i<2;i++){                                  // 
      FocalPlaneSource[i] = vtkPlaneSource::New();       // 
      FocalPlaneMapper[i] = vtkPolyDataMapper::New();    // 
  }                                                      // 100603-komura
  ExtrinsicMatrix = vtkMatrix4x4::New();
  this->Pos[0] = 0.0;
  this->Pos[1] = 0.0;
  this->Pos[2] = 0.0;
  this->Focal[0] = 0.0;
  this->Focal[1] = 0.0;
  this->Focal[2] = 0.0;
  this->F = 0.0;
  this->ViewAngle = 0.0;
  this->h=0.0;
  this->fx=0.0;
  this->fy=0.0;
  focal_length = FOCAL_LENGTH;
  
  //----------------------------------------------------------------
  //multThread
  this->ThreadID = -1;
  this->Thread = vtkMultiThreader::New();
  
  //----------------------------------------------------------------
  //cameraThread;
  makeThread = 0;
  // idata = NULL;                            // 
  // atext = vtkTexture::New();               // 
  // actor =vtkActor::New();                  // 
  // importer = vtkImageImport::New();        // 
  for(int i=0;i<2;i++){                       // 
      idata[i] = NULL;                        // 
      atext[i] = vtkTexture::New();           // 
      actor[i] =vtkActor::New();              // 
      importer[i] = vtkImageImport::New();    // 
  }                                           //
  this->displayChessboardFlag = 0;            // 
  this->captureChessboardFlag = 0;            // 
  this->stereoCalibFlag = 0;                  // 100603-komura
  
  
  // 6/6/2010 ayamada
  textActorSavePath = vtkTextActor::New();
  textActorSavePath2 = vtkTextActor::New();
  textActorSavePathH = vtkTextActor::New();
  this->saveCameraImageEntry = NULL;      
  
  //----------------------------------------------------------------
  // SecondWindowMode
  //----------------------------------------------------------------
  // 100616-komura
  this->stereoOneWindowCheckButton = NULL;
  this->stereoTwoWindowCheckButton = NULL;
  this->stereoOneWindowLayeredCheckButton = NULL;

  this->gapGraphicsBar = NULL;  //
  this->gapGraphics = 0.0;      // 
  this->rightOpacityBar = NULL; // 
  this->rightOpacity = 0.0;     //
  this->leftOpacityBar = NULL;  // 
  this->leftOpacity = 0.0;      // 100617-komura
  
  //----------------------------------------------------------------
        
  this->CVClass = vtkStereoCalibCVClass::New(); // 100621-komura

  // 100625-komura
  this->entryCheckerBoardWidth = NULL;      
  this->entryCheckerBoardHeight = NULL;     
  this->applyChessSettingButton = NULL; // 100626-komura
  this->Mutex = vtkMutexLock::New(); // 100628-komura
}

//---------------------------------------------------------------------------
vtkStereoCalibGUI::~vtkStereoCalibGUI ( )
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

  if (this->TestButton11)
    {
    this->TestButton11->SetParent(NULL);
    this->TestButton11->Delete();
    }

  if (this->TestButton12)
    {
    this->TestButton12->SetParent(NULL);
    this->TestButton12->Delete();
    }

  if (this->displayChessboardButton)
    {
    this->displayChessboardButton->SetParent(NULL);
    this->displayChessboardButton->Delete();
    }

  if (this->TestButton22)
    {
    this->TestButton22->SetParent(NULL);
    this->TestButton22->Delete();
    }
  if (this->TestButton31)
    {
    this->TestButton31->SetParent(NULL);
    this->TestButton31->Delete();
    }

  if(this->SecondaryViewerWindow){
      this->SecondaryViewerWindow->Withdraw();
      this->SecondaryViewerWindow->SetApplication(NULL);
      this->SecondaryViewerWindow->Delete();
      this->SecondaryViewerWindow = NULL;
  }

  if(this->SecondaryViewerWindow2x){
      this->SecondaryViewerWindow2x->Withdraw();
      this->SecondaryViewerWindow2x->SetApplication(NULL);
      this->SecondaryViewerWindow2x->Delete();
      this->SecondaryViewerWindow2x = NULL;
  }

  //----------------------------------------------------------------
  // cameraFocalPlane                               // 
  // FocalPlaneMapper->Delete();                    // 
  // FocalPlaneMapper = NULL;                       // 
  // FocalPlaneSource->Delete();                    // 
  // FocalPlaneSource = NULL;                       // 
  for(int i=0;i<2;i++){                             // 
      FocalPlaneMapper[i]->Delete();                // 
      FocalPlaneMapper[i] = NULL;                   // 
      FocalPlaneSource[i]->Delete();                // 
      FocalPlaneSource[i] = NULL;                   // 
  }                                                 // 100603-komura
  //----------------------------------------------------------------
  // cameraThread;                       // 
  // idata = NULL;                       // 
  // atext->Delete();                    // 
  // atext = NULL;                       // 
  // actor->Delete();                    // 
  // actor = NULL;                       // 
  for(int i=0;i<2;i++){                  // 
      idata[i] = NULL;                   // 
      atext[i]->Delete();                // 
      atext[i] = NULL;                   // 
      actor[i]->Delete();                // 
      actor[i] = NULL;                   // 
  }                                      // 100603-komura

  // 6/6/2010 ayamada
  this->textActorSavePath->Delete();
  this->textActorSavePath2->Delete();
  this->textActorSavePathH->Delete();
    
  // -----------------------------------------------------------
  // secondWindowModeCheckBox
  // 100616-komura
  if (this->stereoOneWindowCheckButton)
      {
          this->stereoOneWindowCheckButton->SetParent(NULL );
          this->stereoOneWindowCheckButton->Delete ( );
      }
  if (this->stereoTwoWindowCheckButton)
      {
          this->stereoTwoWindowCheckButton->SetParent(NULL );
          this->stereoTwoWindowCheckButton->Delete ( );
      }
  if (this->stereoOneWindowLayeredCheckButton)
      {
          this->stereoOneWindowLayeredCheckButton->SetParent(NULL );
          this->stereoOneWindowLayeredCheckButton->Delete ( );
      }
    
  //-----------------------------------------------------------
  //wait for releasing camera device
  // 100623-komura
  while(this->makeThread != 0){
      if(this->makeThread < 5){
          this->makeThread = 5;
      } 
      std::cerr << "wait thread end\n"  <<std::endl;
  }

  //----------------------------------------------------------
  // chess board Setting
  // 100625-komura
  if (this->entryCheckerBoardWidth)
    {
    this->entryCheckerBoardWidth->SetParent(NULL);
    this->entryCheckerBoardWidth ->Delete();
    this->entryCheckerBoardWidth = NULL;
    }
  if (this->entryCheckerBoardHeight)
    {
    this->entryCheckerBoardHeight->SetParent(NULL);
    this->entryCheckerBoardHeight->Delete();
    this->entryCheckerBoardHeight = NULL;
    }
  //----------------------------------------------------------------
  // Unregister Logic class

  this->CVClass->Delete(); //
  this->CVClass = NULL;    // 100621-komura

  this->applyChessSettingButton->Delete(); // 
  this->applyChessSettingButton = NULL;    // 100626-komura

  // 100628-komura
  if (this->Mutex)
      {
          this->Mutex->Delete();
      }

  this->SetModuleLogic ( NULL );
  
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::Enter()
{
    // Fill in
    //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
    
    if (this->TimerFlag == 0)
        {
            this->TimerFlag = 1;
            this->TimerInterval = 10;  // 100 ms
            ProcessTimerEvents();
        }
    
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "StereoCalibGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->TestButton11)
    {
    this->TestButton11
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton12)
    {
    this->TestButton12
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->displayChessboardButton)
    {
    this->displayChessboardButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton22)
    {
    this->TestButton22
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TestButton31)
    {
    this->TestButton31
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // ------------------------------------------------------------
  // secondWindowModeCheckbox
  // 100616-komura
  if (this->stereoOneWindowCheckButton)
      {
          this->stereoOneWindowCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
      }
  if (this->stereoTwoWindowCheckButton)
      {
          this->stereoTwoWindowCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
      }
  if (this->stereoOneWindowLayeredCheckButton)
      {
          this->stereoOneWindowLayeredCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
      }
  // 100617-komura
  this->gapGraphicsBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->gapGraphicsBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->gapGraphicsBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  // 100625-komura
  this->entryCheckerBoardWidth->GetWidget()->RemoveObservers ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->entryCheckerBoardHeight->GetWidget()->RemoveObservers ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  // 100626-komura
  if (this->applyChessSettingButton)
    {
    this->applyChessSettingButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::AddGUIObservers ( )
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

  this->TestButton11
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton12
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->displayChessboardButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton31
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  // -------------------------------------------------------------
  // secondWindowModeCheckBox
  // 100616-komura
  this->stereoOneWindowCheckButton
      ->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->stereoTwoWindowCheckButton
      ->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->stereoOneWindowLayeredCheckButton
      ->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
 
  // 100617-komura
  this->gapGraphicsBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->gapGraphicsBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->gapGraphicsBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->rightOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
  this->leftOpacityBar->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  // 100625-komura
  this->entryCheckerBoardWidth->GetWidget()->AddObserver ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->entryCheckerBoardHeight->GetWidget()->AddObserver ( vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  // 100626-komura
  this->applyChessSettingButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);


  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkStereoCalibGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkStereoCalibLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkStereoCalibGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->TestButton11 == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      if(this->SecondaryViewerWindow){
          this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();

          // 100622-komura
          if(stereoOneWindowCheckButton->GetSelectedState() != 1){
              this->SecondaryViewerWindow->changeSecondaryMonitorSize(640, 480);
          }
          else{
              this->SecondaryViewerWindow->changeSecondaryMonitorSize(1280, 480);
          }

          // if(makeThread == 0){  // 
          //     makeThread = 1;   // 
          //     std::cerr << "makeThread" << std::endl; // 
          // }                                           // 100613-komura
      }
      if(this->stereoTwoWindowCheckButton->GetSelectedState() == 1){
          if(this->SecondaryViewerWindow2x){
              this->SecondaryViewerWindow2x->DisplayOnSecondaryMonitor();
          }
      }
      if(makeThread == 0){
          makeThread = 1;
          std::cerr << "makeThread" << std::endl;
      }
  }
  else if (this->TestButton12 == vtkKWPushButton::SafeDownCast(caller)
    && event == vtkKWPushButton::InvokedEvent){
    std::cerr << "TestButton12 is pressed." << std::endl;
    std::cerr << "\n makeThread:No" << makeThread <<  std::endl;
    // vtkppr test;                     // 
    // test.ppr(-1);                     // 
    // if(this->SecondaryViewerWindow){ // 100616-komura
    if(this->SecondaryViewerWindow){ 
        this->SecondaryViewerWindow->Withdraw();
    }
    if(this->SecondaryViewerWindow2x){
        this->SecondaryViewerWindow2x->Withdraw();
    }
    // 100616-komura
    // if(makeThread != 0){      // 100613-komura
    //     makeThread = 4;
    // }
    if(makeThread != 0){        // 
        makeThread = 4;         // 
    }                           // 100616-komura
  }
  else if (this->displayChessboardButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
      {
          std::cerr << "displayChessboardButton is pressed." << std::endl;
          displayChessboardFlag = !displayChessboardFlag;
          if(displayChessboardFlag == 1){                                       // 
              this->displayChessboardButton->SetText ("Chessboard Finder OFF"); // 
          }                                                                 // 
          else{                                                                 // 
              this->displayChessboardButton->SetText ("Chessboard Finder ON");  // 
          }                                                                 // 100616-komura
      }
  else if (this->TestButton22 == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
        std::cerr << "TestButton22 is pressed." << std::endl;
        if(captureChessboardFlag == 0){
            captureChessboardFlag = 1;
        }
    }
  else if (this->TestButton31 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton31 is pressed." << std::endl;
    if(stereoCalibFlag == 0){
            stereoCalibFlag = 1;
        }
    }
  // ----------------------------------------------------------------
  // secondWindowModeCheckBox
  // 100616-komura
  else if (this->stereoOneWindowCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
           && event == vtkKWCheckButton::SelectedStateChangedEvent )
      {
          int checked = this->stereoOneWindowCheckButton->GetSelectedState();
          if(checked == 1){
              // this->stereoOneWindowCheckButton->SelectedStateOn();
              this->stereoTwoWindowCheckButton->SelectedStateOff();
              this->stereoOneWindowLayeredCheckButton->SelectedStateOff();
              this->SecondaryViewerWindow->SetTitle ("3D Slicer -- StereoCalib -- ");
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.5 -relheight 1",
                           this->SecondaryViewerWindow->lw->GetWidgetName());
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.5 -rely 0 -anchor nw -relwidth 0.5 -relheight 1",
                          this->SecondaryViewerWindow->rw->GetWidgetName()); 
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.0 -rely 0 -anchor nw -relwidth 0.0 -relheight 1",
                           this->SecondaryViewerWindow->mw->GetWidgetName());

              // 100618-komura
              std::cerr << this->actor[1]->GetProperty()->GetBackfaceCulling() << std::endl;
              this->actor[1]->GetProperty()->SetColor(255, 255, 255);
              this->actor[0]->GetProperty()->SetColor(255, 255, 255);

              this->SecondaryViewerWindow->changeSecondaryMonitorSize(1280, 480);   // 100622-komura

              if(this->SecondaryViewerWindow2x && makeThread != 0){
                  this->SecondaryViewerWindow2x->Withdraw();
              }

          }
          else if(this->stereoTwoWindowCheckButton->GetSelectedState() == 0 
                  && this->stereoOneWindowLayeredCheckButton->GetSelectedState() == 0){
              this->stereoOneWindowCheckButton->SelectedStateOn();
          }
      }
  else if (this->stereoTwoWindowCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
           && event == vtkKWCheckButton::SelectedStateChangedEvent )
      {
          int checked = this->stereoTwoWindowCheckButton->GetSelectedState();
          if(checked == 1){
              this->stereoOneWindowCheckButton->SelectedStateOff();
              this->stereoOneWindowLayeredCheckButton->SelectedStateOff();
              this->SecondaryViewerWindow->SetTitle ("3D Slicer -- StereoCalib Left -- ");
              this->SecondaryViewerWindow2x->SetTitle ("3D Slicer -- StereoCalib Right -- ");
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 1 -relheight 1",
                           this->SecondaryViewerWindow->lw->GetWidgetName());
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.0 -rely 0 -anchor nw -relwidth 0 -relheight 1",
                           this->SecondaryViewerWindow->rw->GetWidgetName());
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.0 -rely 0 -anchor nw -relwidth 0 -relheight 1",
                           this->SecondaryViewerWindow->mw->GetWidgetName());
              
              // 100618-komura
              // double colors[3]={0};
              // this->actor[1]->GetProperty()->GetColor(colors);
              // std::cerr << "\nR:" << colors[0] << "\nG:" << colors[1] << "\nB:" << colors[2] << std::endl;
              this->actor[1]->GetProperty()->SetColor(255, 255, 255);
              this->actor[0]->GetProperty()->SetColor(255, 255, 255);
              // this->SecondaryViewerWindow->MainFrame->SetBackgroundColor(255, 255, 255);

              this->SecondaryViewerWindow->changeSecondaryMonitorSize(640, 480);   // 100622-komura

              if(this->SecondaryViewerWindow2x && makeThread != 0){
                  this->SecondaryViewerWindow2x->DisplayOnSecondaryMonitor();
              }
          }
          else if(this->stereoOneWindowCheckButton->GetSelectedState() == 0 
                  && this->stereoOneWindowLayeredCheckButton->GetSelectedState() == 0){
              this->stereoTwoWindowCheckButton->SelectedStateOn();
          }
      }
  else if (this->stereoOneWindowLayeredCheckButton == vtkKWCheckButton::SafeDownCast(caller) 
           && event == vtkKWCheckButton::SelectedStateChangedEvent )
      {
          int checked = this->stereoOneWindowLayeredCheckButton->GetSelectedState();
          if(checked == 1){
              this->stereoOneWindowCheckButton->SelectedStateOff();
              this->stereoTwoWindowCheckButton->SelectedStateOff();
              this->SecondaryViewerWindow->SetTitle ("3D Slicer -- StereoCalib -- ");
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.0 -relheight 1",
                           this->SecondaryViewerWindow->lw->GetWidgetName());
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.0 -rely 0 -anchor nw -relwidth 0.0 -relheight 1",
                           this->SecondaryViewerWindow->rw->GetWidgetName());
              this->SecondaryViewerWindow
                  ->Script("place %s -relx 0.0 -rely 0 -anchor nw -relwidth 1 -relheight 1",
                           this->SecondaryViewerWindow->mw->GetWidgetName());

              // 100618-komura
              this->actor[1]->GetProperty()->SetColor(0, 0, 255);
              this->actor[0]->GetProperty()->SetColor(255, 0, 0);

              this->SecondaryViewerWindow->changeSecondaryMonitorSize(640, 480);   // 100622-komura

              
              if(this->SecondaryViewerWindow2x && makeThread != 0){
                  this->SecondaryViewerWindow2x->Withdraw();
              }
          }
          else if(this->stereoOneWindowCheckButton->GetSelectedState() == 0 
                  && this->stereoTwoWindowCheckButton->GetSelectedState() == 0){
              this->stereoOneWindowLayeredCheckButton->SelectedStateOn();
          }
      }
  
  // 100617-komura
  else if ( vtkKWScale::SafeDownCast(caller) == this->gapGraphicsBar->GetWidget() 
            && static_cast<int>(event) == vtkKWScale::ScaleValueChangingEvent ){
      this->gapGraphics = this->gapGraphicsBar->GetWidget()->GetValue();
  }
  else if ( vtkKWScale::SafeDownCast(caller) == this->rightOpacityBar->GetWidget() 
            && static_cast<int>(event) == vtkKWScale::ScaleValueChangingEvent ){
      this->rightOpacity = this->rightOpacityBar->GetWidget()->GetValue();
      this->actor[1]->GetProperty()->SetOpacity(this->rightOpacity);
  }
  else if ( vtkKWScale::SafeDownCast(caller) == this->leftOpacityBar->GetWidget() 
            && static_cast<int>(event) == vtkKWScale::ScaleValueChangingEvent ){
      this->leftOpacity = this->leftOpacityBar->GetWidget()->GetValue();
      this->actor[0]->GetProperty()->SetOpacity(this->leftOpacity);
  }

  // 100625-komura
  else if (
      (vtkKWEntry::SafeDownCast(caller) == this->entryCheckerBoardWidth->GetWidget()
       && static_cast<int>(event) == vtkKWEntry::EntryValueChangedEvent)
      || (vtkKWEntry::SafeDownCast(caller) == this->entryCheckerBoardHeight->GetWidget()
          && static_cast<int>(event) == vtkKWEntry::EntryValueChangedEvent)
      ){

      // 100626-komura
      // int changeSwitch = 0;
      // if(displayChessboardFlag == 1){
      //     displayChessboardFlag = 0;
      //     this->displayChessboardButton->SetText ("Chessboard Finder ON");
      //     changeSwitch = 1;
      // }

      // int width = this->entryCheckerBoardWidth->GetWidget()->GetValueAsInt();
      // int height = this->entryCheckerBoardHeight->GetWidget()->GetValueAsInt();
      // if( (abs(width - height) < 3 ) || width < 3 || height < 3 ) {
      //     std::cerr << "shoud have bigger than 2" << std::endl;
      //     this->entryCheckerBoardWidth->GetWidget()->SetValueAsInt(CVClass->cornerWidth);
      //     this->entryCheckerBoardHeight->GetWidget()->SetValueAsInt(CVClass->cornerHeight);
      // }
      // else{
          // CVClass->cornerWidth = width;
          // CVClass->cornerHeight = height;
          // CVClass->n = width * height;
      // }
      // if(changeSwitch == 1){
      //     displayChessboardFlag = 1;
      //     this->displayChessboardButton->SetText ("Chessboard Finder OFF");
      // }
      // 100626-komura
      // int width = this->entryCheckerBoardWidth->GetWidget()->GetValueAsInt();
      // int height = this->entryCheckerBoardHeight->GetWidget()->GetValueAsInt();
      // if( (abs(width - height) < 3 ) || width < 3 || height < 3 ) {
      //     std::cerr << "shoud have bigger than 2" << std::endl;
      //     this->applyChessSettingButton->SetEnabled(0);
      // }
      // else{
      //   this->applyChessSettingButton->SetEnabled(1);
      // }
  }

  // 100626-komura
  else if (this->applyChessSettingButton == vtkKWPushButton::SafeDownCast(caller)
    && event == vtkKWPushButton::InvokedEvent){
      int changeSwitch = 0;
      int width = this->entryCheckerBoardWidth->GetWidget()->GetValueAsInt();
      int height = this->entryCheckerBoardHeight->GetWidget()->GetValueAsInt();

      if( (abs(width - height) < 3 ) || width < 3 || height < 3 ) {
          std::cerr << "\nshoud have bigger than 2\n" << std::endl;
          vtkSlicerApplication::GetInstance()
            ->ErrorMessage("Error. Width or Height sholud have bigger than 2");
      }
      else{
          // if(displayChessboardFlag == 1){
          //     displayChessboardFlag = 0;
          //     this->displayChessboardButton->SetText ("Chessboard Finder ON");
          //     changeSwitch = 1;
          //    sleep(1);         // 100628-komura wait for display stop
          // }
          this->Mutex->Lock();
          CVClass->cornerWidth = width;
          CVClass->cornerHeight = height;
          CVClass->n = width * height;
          this->Mutex->Unlock();
          // if(changeSwitch == 1){
          //     displayChessboardFlag = 1;
          //     this->displayChessboardButton->SetText ("Chessboard Finder OFF");
          // }
      }
  }
}


void vtkStereoCalibGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkStereoCalibGUI *self = reinterpret_cast<vtkStereoCalibGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkStereoCalibGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkStereoCalibLogic::SafeDownCast(caller))
    {
    if (event == vtkStereoCalibLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::ProcessTimerEvents()
{
  if (this->TimerFlag){
    if(makeThread == 1){
      this->makeCameraThread("cameraThread");
      makeThread = 2;
    }
    if(makeThread == 3){
        // 100616-komura
        if(this->stereoOneWindowCheckButton->GetSelectedState()){
            this->SecondaryViewerWindow->rw->Render();
            this->SecondaryViewerWindow->lw->Render();
        }
        else if(this->stereoTwoWindowCheckButton->GetSelectedState()){
            this->SecondaryViewerWindow->lw->Render();
            this->SecondaryViewerWindow2x->rw->Render();
        }
        else if(this->stereoOneWindowLayeredCheckButton->GetSelectedState()){
            this->SecondaryViewerWindow->mw->Render();
        }
    }
      
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "StereoCalib", "StereoCalib", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForTestFrame2();
  BuildGUIForTestFrame3();      // 100603-komura


  // this->SecondaryViewerWindow = vtkSlicerSecondaryViewerWindow::New();
  this->SecondaryViewerWindow = vtkStereoCalibViewerWidget::New();
  this->SecondaryViewerWindow->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow->Create();
  this->SecondaryViewerWindow2x = vtkStereoCalibViewerWidget::New();
  this->SecondaryViewerWindow2x->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow2x->Create();
  this->SecondaryViewerWindow2x->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 1 -relheight 1", // 
                                      this->SecondaryViewerWindow2x->rw->GetWidgetName()); // 100616-komura
  this->SecondaryViewerWindow2x->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0 -relheight 1", // 
                                      this->SecondaryViewerWindow2x->lw->GetWidgetName()); // 100616-komura
  this->SecondaryViewerWindow2x->changeSecondaryMonitorSize(640, 480); // 100622-komura


}


void vtkStereoCalibGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:StereoCalib</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "StereoCalib" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("StereoCalib");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 1");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();

  vtkKWFrameWithLabel *frame2 = vtkKWFrameWithLabel::New();
  frame2->SetParent(conBrowsFrame->GetFrame());
  frame2->Create();

  vtkKWFrameWithLabel *frame3 = vtkKWFrameWithLabel::New();
  frame3->SetParent(conBrowsFrame->GetFrame());
  frame3->Create();
    
    // 6/6/2010 ayamada
    //frame->SetLabelText ("Test child frame");
    frame->SetLabelText ("Secondary Window Control");
    frame2->SetLabelText ("Secondary Window Mode");
    frame3->SetLabelText ("Secondary Window tuning");

    this->Script ( "pack %s %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                   frame2->GetWidgetName(),
                   frame3->GetWidgetName(),
                   frame->GetWidgetName() );

  // -----------------------------------------
  // Test push button

  this->TestButton11 = vtkKWPushButton::New ( );
  this->TestButton11->SetParent ( frame->GetFrame() );
  this->TestButton11->Create ( );
  this->TestButton11->SetText ("Window ON");

    // 6/6/2010 ayamada
    //this->TestButton11->SetWidth (12);
    this->TestButton11->SetWidth (15);

  this->TestButton12 = vtkKWPushButton::New ( );
  this->TestButton12->SetParent ( frame->GetFrame() );
  this->TestButton12->Create ( );
  this->TestButton12->SetText ("Window OFF (Camera Disconnection)");

    // 6/6/2010 ayamada
    //this->TestButton12->SetWidth (12);
    this->TestButton12->SetWidth (37);

    this->Script("pack %s %s -side left -padx 2 -pady 2", 
                 this->TestButton11->GetWidgetName(),
                 this->TestButton12->GetWidgetName());

    // --------------------------------------------
    // secondWindowModeCheckBox
    // 100616-komura
    this->stereoOneWindowCheckButton = vtkKWCheckButton::New();
    this->stereoOneWindowCheckButton->SetParent(frame2->GetFrame());
    this->stereoOneWindowCheckButton->Create();
    this->stereoOneWindowCheckButton->SelectedStateOn();
    this->stereoOneWindowCheckButton->SetText("OneWindow");
  
    this->stereoTwoWindowCheckButton = vtkKWCheckButton::New();
    this->stereoTwoWindowCheckButton->SetParent(frame2->GetFrame());
    this->stereoTwoWindowCheckButton->Create();
    this->stereoTwoWindowCheckButton->SelectedStateOff();
    this->stereoTwoWindowCheckButton->SetText("TwoWindow");

    this->stereoOneWindowLayeredCheckButton = vtkKWCheckButton::New();
    this->stereoOneWindowLayeredCheckButton->SetParent(frame2->GetFrame());
    this->stereoOneWindowLayeredCheckButton->Create();
    this->stereoOneWindowLayeredCheckButton->SelectedStateOff();
    this->stereoOneWindowLayeredCheckButton->SetText("Layered");

    this->Script("pack %s %s %s -side left -padx 2 -pady 2", 
                 this->stereoOneWindowCheckButton->GetWidgetName(),
                 this->stereoTwoWindowCheckButton->GetWidgetName(),
                 this->stereoOneWindowLayeredCheckButton->GetWidgetName());

    // --------------------------------------------
    // secondWindowTuning
    // 100617-komura
    this->gapGraphicsBar = vtkKWScaleWithLabel::New();
    this->gapGraphicsBar->SetParent ( frame3->GetFrame() );
    this->gapGraphicsBar->Create ( );
    this->gapGraphicsBar->SetLabelText("GapOfGraphics");
    this->gapGraphicsBar->GetWidget()->SetRange(-50,50);
    this->gapGraphicsBar->GetWidget()->SetResolution(1.0);
    this->gapGraphicsBar->SetBalloonHelpString("-> is extented gap of graphics");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", 
                   this->gapGraphicsBar->GetWidgetName());
    this->leftOpacityBar = vtkKWScaleWithLabel::New();
    this->leftOpacityBar->SetParent ( frame3->GetFrame() );
    this->leftOpacityBar->Create ( );
    this->leftOpacityBar->SetLabelText("leftOpacity");
    this->leftOpacityBar->GetWidget()->SetRange(1.0, 0);
    this->leftOpacityBar->GetWidget()->SetResolution(0.05);
    // this->leftOpacityBar->SetBalloonHelpString("-> is extented gap of graphics");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", 
                   this->leftOpacityBar->GetWidgetName());
    this->rightOpacityBar = vtkKWScaleWithLabel::New();
    this->rightOpacityBar->SetParent ( frame3->GetFrame() );
    this->rightOpacityBar->Create ( );
    this->rightOpacityBar->SetLabelText("rightOpacity");
    this->rightOpacityBar->GetWidget()->SetRange(1.0, 0);
    this->rightOpacityBar->GetWidget()->SetResolution(0.05);
    // this->rightOpacityBar->SetBalloonHelpString("-> is extented gap of graphics");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", 
                   this->rightOpacityBar->GetWidgetName());
    


    conBrowsFrame->Delete();
    frame->Delete();
    frame2->Delete();           // 1000616-komura
    frame3->Delete();           // 1000616-komura

}


//---------------------------------------------------------------------------
void vtkStereoCalibGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("StereoCalib");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 2");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame2 = vtkKWFrameWithLabel::New();
  frame2->SetParent(conBrowsFrame->GetFrame());
  frame2->Create();
  // 100625-komura
  frame2->SetLabelText ("Checker Board Setting");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame2->GetWidgetName() );


  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
    
  // 6/6/2010 ayamada
  //frame->SetLabelText ("Test child frame");
  frame->SetLabelText ("Image Capture Control");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  

  //-----------------------------------------
  // 
  // 100625-komura
  frame2->SetLabelText ("Checker Board Setting");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame2->GetWidgetName() );
  
  this->entryCheckerBoardWidth = vtkKWEntryWithLabel::New();
  this->entryCheckerBoardWidth->SetParent(frame2->GetFrame());
  this->entryCheckerBoardWidth->Create();
  this->entryCheckerBoardWidth->SetWidth(45);
  this->entryCheckerBoardWidth->SetLabelWidth(30);
  this->entryCheckerBoardWidth->SetLabelText("Checker Board Width:");
  this->entryCheckerBoardWidth->GetWidget()->SetValueAsDouble(CVClass->cornerWidth);
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->entryCheckerBoardWidth->GetWidgetName());

  this->entryCheckerBoardHeight = vtkKWEntryWithLabel::New();
  this->entryCheckerBoardHeight->SetParent(frame2->GetFrame());
  this->entryCheckerBoardHeight->Create();
  this->entryCheckerBoardHeight->SetWidth(45);
  this->entryCheckerBoardHeight->SetLabelWidth(30);
  this->entryCheckerBoardHeight->SetLabelText("Checker Board Heihgt:");
  this->entryCheckerBoardHeight->GetWidget()->SetValueAsDouble(CVClass->cornerHeight);
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->entryCheckerBoardHeight->GetWidgetName());
    
  // 100626-komura
  this->applyChessSettingButton = vtkKWPushButton::New ( );
  this->applyChessSettingButton->SetParent ( frame2->GetFrame() );
  this->applyChessSettingButton->Create ( );
  this->applyChessSettingButton->SetText ("Apply Checker Board Setting");
  this->applyChessSettingButton->SetWidth (25);
  this->Script("pack %s -side left -padx 2 -pady 2",
                 this->applyChessSettingButton->GetWidgetName());

  // -----------------------------------------
  // Test push button

  this->displayChessboardButton = vtkKWPushButton::New ( );
  this->displayChessboardButton->SetParent ( frame->GetFrame() );
  this->displayChessboardButton->Create ( );
  this->displayChessboardButton->SetText ("Chessboard Finder On");
  
  //this->displayChessboardButton->SetWidth (12);
  // 6/6/2010 ayamada
  this->displayChessboardButton->SetWidth (30);

  this->TestButton22 = vtkKWPushButton::New ( );
  this->TestButton22->SetParent ( frame->GetFrame() );
  this->TestButton22->Create ( );
  this->TestButton22->SetText ("Capture");
  // 6/6/2010 ayamada
  //this->TestButton22->SetWidth (12);
  this->TestButton22->SetWidth (15);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->displayChessboardButton->GetWidgetName(),
               this->TestButton22->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
}

//---------------------------------------------------------------------------
// 100603-komura
void vtkStereoCalibGUI::BuildGUIForTestFrame3 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("StereoCalib");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 3");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
    
    // 6/6/2010 ayamada
    //frame->SetLabelText ("Test child frame");
    frame->SetLabelText ("Stereo Calibration");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

    
 // 6/6/2010 ayamada
 this->saveCameraImageEntry = vtkKWEntryWithLabel::New();
 this->saveCameraImageEntry->SetParent(frame->GetFrame());
 this->saveCameraImageEntry->Create();
 this->saveCameraImageEntry->SetWidth(45);
 this->saveCameraImageEntry->SetLabelWidth(30);
 this->saveCameraImageEntry->SetLabelText("Save Path of Calibration Matrices:");
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->saveCameraImageEntry->GetWidgetName());
    
    
  // -----------------------------------------
  // Test push button

  this->TestButton31 = vtkKWPushButton::New ( );
  this->TestButton31->SetParent ( frame->GetFrame() );
  this->TestButton31->Create ( );
  this->TestButton31->SetText ("Stereo Calibration");

    // 6/6/2010 ayamada
    //this->TestButton31->SetWidth (12);
    this->TestButton31->SetWidth (25);

  //this->Script("pack %s -side left -padx 2 -pady 2", 
  //             this->TestButton31->GetWidgetName());

    // 6/6/2010 ayamada
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->TestButton31->GetWidgetName());
    

  conBrowsFrame->Delete();
  frame->Delete();
}


//----------------------------------------------------------------------------
void vtkStereoCalibGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------

//////////////////////
// CameraFocusPlane //
//////////////////////

void vtkStereoCalibGUI::CameraFocusPlane(vtkCamera * cam, double Ratio)
{

    cam->GetPosition(this->Pos);
    cam->GetFocalPoint(this->Focal);
    
    this->ExtrinsicMatrix->DeepCopy(cam->GetViewTransformMatrix());
    this->ExtrinsicMatrix->Invert();
    this->F = sqrt(vtkMath::Distance2BetweenPoints(this->Pos, this->Focal));
    this->ViewAngle = cam->GetViewAngle() / 2.0;
    this->h = 2.0 * tan(vtkMath::RadiansFromDegrees(this->ViewAngle / 2.0)) * F;
    this->fx = ( 2.0*this->h * Ratio * this->focal_point_x ) / 640.0;
    this->fy = ( 2.0*this->h * this->focal_point_y ) / 480.0;
    
    // this->FocalPlaneSource->SetOrigin(-this->h * Ratio, -this->h, -this->F);    // 
    // this->FocalPlaneSource->SetPoint1(this->h * Ratio, -this->h, -this->F);     // 
    // this->FocalPlaneSource->SetPoint2(-this->h * Ratio, this->h, -this->F);     // 
    // this->FocalPlaneSource->SetCenter(0.0, 0.0, -this->F);                     // 
    for(int i=0;i<2;i++){                                                          // 
        this->FocalPlaneSource[i]->SetOrigin(-this->h * Ratio, -this->h, -this->F);// 
        this->FocalPlaneSource[i]->SetPoint1(this->h * Ratio, -this->h, -this->F); // 
        this->FocalPlaneSource[i]->SetPoint2(-this->h * Ratio, this->h, -this->F); // 
        this->FocalPlaneSource[i]->SetCenter(0.0, 0.0, -this->F);
        
        // 100617-komura
        if(i==0){
            this->FocalPlaneSource[i]->SetCenter(-this->gapGraphics, 0.0, -this->F);
        }
        else{
            this->FocalPlaneSource[i]->SetCenter(this->gapGraphics, 0.0, -this->F);
        }
            
    }                                                                              // 100603-komura
    


}
//----------------------------------------------------------------------------

//////////////////////
// makeCameraThread //
//////////////////////
int vtkStereoCalibGUI::makeCameraThread(const char* nodeName)
{
    
    // 5/5/2010 ayamada    
    // for videoOverlay
    
    vtkMRMLModelNode           *locatorModel;
    vtkMRMLModelDisplayNode    *locatorDisp;
    
    
    locatorModel = vtkMRMLModelNode::New();
    locatorDisp = vtkMRMLModelDisplayNode::New();
    
    GetMRMLScene()->SaveStateForUndo();
    GetMRMLScene()->AddNode(locatorDisp);
    GetMRMLScene()->AddNode(locatorModel);  
    
    locatorDisp->SetScene(GetMRMLScene());
    
    locatorModel->SetName(nodeName);    
    locatorModel->SetScene(GetMRMLScene());
    locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
    locatorModel->SetHideFromEditors(0);
             
     //------------------------------------------------------------------
     //  Setting up camera paramethers for rendering
     //------------------------------------------------------------------
     
     fileCamera = vtkCamera::New();
     fileCamera->SetViewUp (0.0, 0.0, 1.0);
     fileCamera->SetClippingRange( 0, 5 * focal_length );
     fileCamera->SetPosition ( 0.0, focal_length / 4.0, 0.0 );
     fileCamera->SetFocalPoint (0.0, 0.0, 0.0);
     fileCamera->SetViewAngle ( FOA );
     fileCamera->Zoom(1.0); 

 //------------------------------------------------------------------
 //   starting thread for capturing camera image
 //------------------------------------------------------------------
  std::cerr << "\n\nmakeThread OK\n\n" << std::endl;

 this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &vtkStereoCalibGUI::thread_CameraThread, this);
 locatorDisp->Delete();     
    return 1;
    
}


//////////////////
// cameraThread //
//////////////////
void *vtkStereoCalibGUI::thread_CameraThread(void* t)//100603-komura
{

    std::cerr << "\n\nmakeThread Start\n\n" << std::endl;
    // 5/15/ayamada
    
    int deviceNum = 0;          // 100608-komura

    CvCapture* capture[2] = {0};
    IplImage* captureImage[2];
    IplImage* RGBImage[2];
    IplImage* captureImageTmp[2];
     
    // double D1_mono[4]; // 100608-komura 
    // CvMat _D1_mono;                  // 
    // _D1_mono = cvMat(1, 4, CV_64F, D1_mono ); // 

    double M1[3][3], M2[3][3], D2[5]; // 
    CvMat _M1;                  // 
    CvMat _M2;                  //
    CvMat _D1;                  // 
    CvMat _D2;                  // 
    _M1 = cvMat(3, 3, CV_64F, M1 ); // 
    _M2 = cvMat(3, 3, CV_64F, M2 ); // 
    _D2 = cvMat(1, 5, CV_64F, D2 ); // 100607-komura
    int calibTest = 0;              // 
    
    captureImage[2] = NULL;
    RGBImage[2] = NULL;
    captureImageTmp[2] = NULL;
    
    
    vtkMultiThreader::ThreadInfo* vinfo = 
        static_cast<vtkMultiThreader::ThreadInfo*>(t);
    vtkStereoCalibGUI* pGUI = 
        static_cast<vtkStereoCalibGUI*>(vinfo->UserData);

    // vtkStereoCalibCVClass CVClass; // 100607-komura
    
    
    int i=0;
    while(i<=7){// 5/16/2010 ayamada
        if( (NULL==(capture[deviceNum] = cvCaptureFromCAM(i)))) // 10.01.25 ayamada
            {
                std::cerr << "\n\nCan Not Find First Camera:"<< i << std::endl;
                std::cerr << "\n\n ya\n"<< i << std::endl;
                i++;                
            }
        else{
            std::cerr << "\n\nConnected First Camera Device No:" << i << std::endl;
            deviceNum = 1;
            i++;
            break;
        }
    }
    
    while(i<=7){//100613-komura
        if( (NULL==(capture[deviceNum] = cvCaptureFromCAM(i)))) // 10.01.25 ayamada
            {
                std::cerr << "\n\nCan Not Find Second Camera:"<< i << std::endl;
                i++;                
            }
        else{
            std::cerr << "\n\nConnected second Camera Device No:" << i << std::endl;
            deviceNum = 2;
            break;
        }
    }
    if(i==8 && deviceNum == 0){ // 100613-komura
        fprintf(stdout, "\nCan Not Find Camera Device!!\n");
        pGUI->makeThread = 0;  
        for(int n=0;n<2;n++){   // 
            if(capture[n] != NULL){ // 
                cvReleaseCapture(&capture[n]);  
            }
        }
        std::cerr << "makeThread No:"<< pGUI->makeThread << "\n" <<std::endl;
        return NULL;
    }
    
    // 100625-komura
    double *D1;
    D1 = new double[deviceNum + 3];
    _D1 = cvMat(1, deviceNum + 3, CV_64F, D1 ); 

 
    // if(capture[1] != NULL){     // 
    //     deviceNum = 2;          // 
    // }                           // 
    // else{                       // 
    //     if(capture[0] != NULL){ // 
    //     deviceNum = 1;          // 
    //     }                       // 
    // }                           // 100613-komura

    // for(n=0;n<2;n++){           // 
    for(int n=0;n<deviceNum;n++){   // 100608-komura
        if(capture[n] != NULL){
            while(1){
                if(NULL == (captureImageTmp[n] = cvQueryFrame( capture[n] ))){
                    sleep(2);
                    std::cerr << "\nCan Not Take A Picture\n" << std::endl;
                    continue;
                }  
              
                pGUI->CVClass->imageSize = cvGetSize( captureImageTmp[n] );
                captureImage[n] = cvCreateImage(pGUI->CVClass->imageSize, IPL_DEPTH_8U,3); 
                RGBImage[n] = cvCreateImage(pGUI->CVClass->imageSize, IPL_DEPTH_8U, 3);
                pGUI->CVClass->imageSize = cvGetSize( captureImageTmp[n] );
                cvFlip(captureImageTmp[n], captureImage[n], 0);
                cvCvtColor( captureImage[n], RGBImage[n], CV_BGR2RGB);
                pGUI->idata[n] = (unsigned char*) RGBImage[n]->imageData;
                pGUI->importer[n]->SetWholeExtent(0,pGUI->CVClass->imageSize.width-1,0,pGUI->CVClass->imageSize.height-1,0,0);
                pGUI->importer[n]->SetDataExtentToWholeExtent();
                pGUI->importer[n]->SetDataScalarTypeToUnsignedChar();
                pGUI->importer[n]->SetNumberOfScalarComponents(3);
                pGUI->importer[n]->SetImportVoidPointer(pGUI->idata[n]);
                pGUI->atext[n]->SetInputConnection(pGUI->importer[n]->GetOutputPort());
                pGUI->atext[n]->InterpolateOn(); 
                pGUI->importer[n]->Update();
                break;
            }
          
            pGUI->planeRatio = VIEW_SIZE_X / VIEW_SIZE_Y;
            pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);
            pGUI->FocalPlaneMapper[n]->SetInput(pGUI->FocalPlaneSource[n]->GetOutput());
            pGUI->actor[n]->SetMapper(pGUI->FocalPlaneMapper[n]);
            pGUI->actor[n]->SetUserMatrix(pGUI->ExtrinsicMatrix);
            pGUI->actor[n]->SetTexture(pGUI->atext[n]);
        }
    }
    // pGUI->SecondaryViewerWindow->lw->GetRenderer()->AddActor(pGUI->actor[0]);               // 
    // fprintf(stdout, "\nget camera handle 1\n");                                             // 
    // pGUI->SecondaryViewerWindow->lw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );    // 
    // // pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor[1]);            // 
    // fprintf(stdout, "\nget camera handle 2\n");                                             // 
    // // pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 
    // pGUI->SecondaryViewerWindow2x->lw->GetRenderer()->AddActor(pGUI->actor[1]);             // 
    // fprintf(stdout, "\nget camera handle 1\n");                                             // 
    // pGUI->SecondaryViewerWindow2x->lw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );  // 100616-komura

    pGUI->SecondaryViewerWindow->lw->GetRenderer()->AddActor(pGUI->actor[0]); // 
    pGUI->SecondaryViewerWindow->lw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 
    pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor[1]); // 
    pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 
    pGUI->SecondaryViewerWindow->mw->GetRenderer()->AddActor(pGUI->actor[0]); // 
    pGUI->SecondaryViewerWindow->mw->GetRenderer()->AddActor(pGUI->actor[1]); // 
    pGUI->SecondaryViewerWindow->mw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 

    pGUI->SecondaryViewerWindow2x->lw->GetRenderer()->AddActor(pGUI->actor[0]); // 
    pGUI->SecondaryViewerWindow2x->lw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 
    pGUI->SecondaryViewerWindow2x->rw->GetRenderer()->AddActor(pGUI->actor[1]); // 
    pGUI->SecondaryViewerWindow2x->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 100616-komura

    CvMat* mx1 = cvCreateMat( pGUI->CVClass->imageSize.height,          // 
                              pGUI->CVClass->imageSize.width, CV_32F ); // 
    CvMat* my1 = cvCreateMat( pGUI->CVClass->imageSize.height,          // 
                              pGUI->CVClass->imageSize.width, CV_32F ); // 
    CvMat* mx2 = cvCreateMat( pGUI->CVClass->imageSize.height,          // 
                              pGUI->CVClass->imageSize.width, CV_32F ); // 
    CvMat* my2 = cvCreateMat( pGUI->CVClass->imageSize.height,          // 
                              pGUI->CVClass->imageSize.width, CV_32F ); // 100607-komura

    pGUI->makeThread = 3;
    while(pGUI->makeThread < 4){
        if(pGUI->captureChessboardFlag == 1){
            pGUI->captureChessboardFlag = 2;
        }
        // for(n=0;n<2;n++){       // 
        for(int n=0;n<deviceNum;n++){  // 100608-komura
            if(capture[n] != NULL){
                pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio); 
                pGUI->FocalPlaneMapper[n]->SetInput(pGUI->FocalPlaneSource[n]->GetOutput());
                pGUI->actor[n]->SetMapper(pGUI->FocalPlaneMapper[n]);
                pGUI->actor[n]->SetUserMatrix(pGUI->ExtrinsicMatrix);
                captureImageTmp[n] = cvQueryFrame( capture[n] );
                if(pGUI->captureChessboardFlag == 2){
                    pGUI->CVClass->chessLoad(captureImageTmp[n], n);                   // 
                    // pGUI->chessLoad(captureImageTmp[n], pGUI->imageSize, n); // 100607-komura
                }
                if(pGUI->displayChessboardFlag == 1){
                    pGUI->Mutex->Lock();
                    pGUI->CVClass->displayChessboard(captureImageTmp[n]);
                    pGUI->Mutex->Unlock();
                    // pGUI->displayChessboard(captureImageTmp[n]); // 100607-komura
                }        

                if(calibTest == 1){     // 
                    if(deviceNum == 2){ // 100608-komura

                        if(n==0){                                                     // 
                            cvRemap( captureImageTmp[n], captureImage[n], mx1, my1 ); // 
                            cvCopy(captureImage[n], captureImageTmp[n]);              // 
                        }                                                             // 
                        if(n==1){                                                     // 
                            cvRemap( captureImageTmp[n], captureImage[n], mx2, my2 ); //  
                            cvCopy(captureImage[n], captureImageTmp[n]);              // 
                        }                                                             // 
                    }                                                                 // 100607-komura

                    else if(deviceNum == 1){                                                // 
                        cvUndistort2(captureImageTmp[n],captureImage[n], &_M1, &_D1 ); // 
                        cvCopy(captureImage[n], captureImageTmp[n]);                        //
                        // std::cerr << "\n\n mono\n\n" << std::endl;                       // 100608-komura
                    }
                }
                cvFlip(captureImageTmp[n], captureImage[n], 0);
                cvCvtColor( captureImage[n], RGBImage[n], CV_BGR2RGB);
                pGUI->idata[n] = (unsigned char*) RGBImage[n]->imageData;
                pGUI->importer[n]->Modified();
            }
        }
        if(pGUI->captureChessboardFlag == 2){ 
            pGUI->captureChessboardFlag = 0;
        }
        if(pGUI->stereoCalibFlag == 1){
            pGUI->makeThread = 2;
            if(deviceNum == 2){
                pGUI->CVClass->stereoCalib(_M1, _M2, _D1, _D2); // 
                pGUI->CVClass->displayStereoCalib(mx1,mx2,my1,my2); // 100608-komura
            }
            else if(deviceNum == 1){
                pGUI->CVClass->monoCalib(_M1, _D1); // 
            }
            // pGUI->stereoCalib(); // 100607-komura
            // 6/6/2010 ayamada
            if(pGUI->saveCameraImageEntry->GetWidget()->GetValue() != NULL){
                //this->snapShotShutter=1;
                pGUI->snapShotNumber++;
                
                sprintf(pGUI->snapShotSavePath,"%sIntrinsics_%d.xml"
                        ,pGUI->saveCameraImageEntry->GetWidget()->GetValue(),pGUI->snapShotNumber);
                //textActorSavePath->SetInput(this->snapShotSavePath);
                cvSave(pGUI->snapShotSavePath, &_M1);        
                
                sprintf(pGUI->snapShotSavePath2,"%sDistortion_%d.xml"
                        ,pGUI->saveCameraImageEntry->GetWidget()->GetValue(),pGUI->snapShotNumber);
                cvSave(pGUI->snapShotSavePath2, &_D1);        
                fprintf(stdout, "Save Camera1 parametor\n");
                
                pGUI->snapShotNumber++;
                
                if(deviceNum == 2){ // 100608-komura
                    // sprintf(pGUI->snapShotSavePath2,"%sDistortion_%d.xml"
                    //         ,pGUI->saveCameraImageEntry->GetWidget()->GetValue(),pGUI->snapShotNumber);
                    // cvSave(pGUI->snapShotSavePath, &_D1);        
                    // fprintf(stdout, "Save Camera1 parametor\n");
                    
                    // pGUI->snapShotNumber++;
                    
                    sprintf(pGUI->snapShotSavePath,"%sIntrinsics_%d.xml"
                            ,pGUI->saveCameraImageEntry->GetWidget()->GetValue(),pGUI->snapShotNumber);
                    //textActorSavePath->SetInput(this->snapShotSavePath);
                    cvSave(pGUI->snapShotSavePath, &_M2);        
                    sprintf(pGUI->snapShotSavePath2,"%sDistortion_%d.xml"
                            ,pGUI->saveCameraImageEntry->GetWidget()->GetValue(),pGUI->snapShotNumber);
                    cvSave(pGUI->snapShotSavePath2, &_D2);        
                    fprintf(stdout, "Save Camera2 parametor\n");   
                }

            }else{
                
                fprintf(stdout, "Please decide the save path!!\n");        
                
            }

            // if(deviceNum == 2){                               //
            //     CVClass.displayStereoCalib(mx1,mx2,my1,my2); // 
            // }                                                // 100608-komura

            calibTest = 1;
            
    
            pGUI->stereoCalibFlag = 0;
            pGUI->makeThread = 3;
        }
    }
  
    std::cerr << "thread was finished!!" << std::endl;
    
    if(mx1 != NULL){            // 
        cvReleaseMat( &mx1 );   // 
    }                           // 
    if(my1 != NULL){            // 
        cvReleaseMat( &my1 );   // 
    }                           // 
    if(mx2 != NULL){            // 
        cvReleaseMat( &mx2 );   // 
    }                           // 
    if(my2 != NULL){            // 
        cvReleaseMat( &my2 );   // 
    }                           // 100607-komura

    for(int n=0;n<2;n++){
        if(capture[n] != NULL){ 
            cvReleaseCapture(&capture[n]);  
        }
    }

    delete [] D1;               // 100625-komura

    pGUI->makeThread = 0;  
    std::cerr << "makeThread No:"<< pGUI->makeThread << "\n" <<std::endl;
    return NULL;
  
}




//------------------------------------------------------------------
///////////////////////////
// StereoCalib_LoadImage //
///////////////////////////
/*
void vtkStereoCalibGUI::chessLoad(IplImage* frame, CvSize &imageSize, int lr){//100603-komura
    int i, n = CORNER_WIDTH*CORNER_HEIGHT, j, N = 0;
    std::vector<CvPoint2D32f> temp(n);
    int count = 0, result=0;
    std::vector<CvPoint2D32f>& pts = points[lr];
    
    IplImage* img = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1); 
    cvCvtColor(frame, img, CV_BGR2GRAY);

    result = cvFindChessboardCorners( img, cvSize(CORNER_WIDTH, CORNER_HEIGHT ),
                                      &temp[0], &count,
                                      CV_CALIB_CB_ADAPTIVE_THRESH |
                                      CV_CALIB_CB_NORMALIZE_IMAGE
                                      );
    N = pts.size();
    pts.resize(N + n, cvPoint2D32f(0,0));
    active[lr].push_back((uchar)result);
    //assert( result != 0 );
    if( result ){
        //Calibration will suffer without subpixel interpolation
        cvFindCornerSubPix( img, &temp[0], count,
                            cvSize(11, 11), cvSize(-1,-1),
                            cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,
                                           30, 0.01) );
        copy( temp.begin(), temp.end(), pts.begin() + N );
    }
    // char test[255];
    // sprintf(test,"/home/masai/Desktop/test%d.bmp",lr);
    // cvSaveImage( test, img );
    cvReleaseImage( &img );
}
*/

//------------------------------------------------------------------
///////////////////////
// StereoCalib_solve //
///////////////////////
/*
void vtkStereoCalibGUI::stereoCalib()//100603-komura
{
    int c;
    bool isVerticalStereo = false;//OpenCV can handle left-right
    //or up-down camera arrangements
    const float squareSize = 1.f; //Set this to your actual square size
    int i, j, nframes, n = CORNER_WIDTH*CORNER_HEIGHT, N = 0;
    std::vector<CvPoint3D32f> objectPoints;
    std::vector<int> npoints;
    // ARRAY AND VECTOR STORAGE:
    double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
    CvMat _M1 = cvMat(3, 3, CV_64F, M1 );
    CvMat _M2 = cvMat(3, 3, CV_64F, M2 );
    CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
    CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
    CvMat _R = cvMat(3, 3, CV_64F, R );
    CvMat _T = cvMat(3, 1, CV_64F, T );
    CvMat _E = cvMat(3, 3, CV_64F, E );
    CvMat _F = cvMat(3, 3, CV_64F, F );
    // HARVEST CHESSBOARD 3D OBJECT POINT LIST:
    
    nframes = active[0].size();//Number of good chessboads found
    objectPoints.resize(nframes*n);
    for( i = 0; i < CORNER_HEIGHT; i++ ){
        for( j = 0; j < CORNER_WIDTH; j++ ){
            objectPoints[i*CORNER_WIDTH + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
        }
    }
    for( i = 1; i < nframes; i++ ){
        copy( objectPoints.begin(), objectPoints.begin() + n,
              objectPoints.begin() + i*n );
    }
    npoints.resize(nframes,n);
    N = nframes*n;
    CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
    CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
    cvSetIdentity(&_M1);
    cvSetIdentity(&_M2);
    cvZero(&_D1);
    cvZero(&_D2);

// CALIBRATE THE STEREO CAMERAS
    std::cerr << "\n\nRunning stereo calibration ...\n\n" << std::endl;
    cvStereoCalibrate( &_objectPoints, &_imagePoints1,
         &_imagePoints2, &_npoints,
         &_M1, &_D1, &_M2, &_D2,
         imageSize, &_R, &_T, &_E, &_F,
         cvTermCriteria(CV_TERMCRIT_ITER+
          CV_TERMCRIT_EPS, 100, 1e-5),
         CV_CALIB_FIX_ASPECT_RATIO +
         CV_CALIB_ZERO_TANGENT_DIST +
         CV_CALIB_SAME_FOCAL_LENGTH );
    std::cerr << "\n\n stereo calibration finish \n\n" << std::endl;


    // cvSave("/home/masai/Desktop/Intrimsics_1.xml", &_M1);
    // cvSave("/home/masai/Desktop/Distortion_1.xml", &_D1);
    // fprintf(stdout, "Save Camera1 parametor\n");
    // cvSave("/home/masai/Desktop/Intrimsics_2.xml", &_M2);
    // cvSave("/home/masai/Desktop/Distortion_2.xml", &_D2);
    // fprintf(stdout, "Save Camera2 parametor\n");

    
    // 6/6/2010 ayamada
    if(this->saveCameraImageEntry->GetWidget()->GetValue() != NULL){
        //this->snapShotShutter=1;
        this->snapShotNumber++;
        
        sprintf(this->snapShotSavePath,"%sIntrimsics_%d.xml"
                ,this->saveCameraImageEntry->GetWidget()->GetValue(),this->snapShotNumber);
        //textActorSavePath->SetInput(this->snapShotSavePath);
        cvSave(this->snapShotSavePath, &_M1);        
        sprintf(this->snapShotSavePath2,"%sDistortion_%d.xml"
                ,this->saveCameraImageEntry->GetWidget()->GetValue(),this->snapShotNumber);
        cvSave(this->snapShotSavePath, &_D1);        
        fprintf(stdout, "Save Camera1 parametor\n");

        this->snapShotNumber++;

        sprintf(this->snapShotSavePath,"%sIntrimsics_%d.xml"
                ,this->saveCameraImageEntry->GetWidget()->GetValue(),this->snapShotNumber);
        //textActorSavePath->SetInput(this->snapShotSavePath);
        cvSave(this->snapShotSavePath, &_M2);        
        sprintf(this->snapShotSavePath2,"%sDistortion_%d.xml"
                ,this->saveCameraImageEntry->GetWidget()->GetValue(),this->snapShotNumber);
        cvSave(this->snapShotSavePath, &_D2);        
        fprintf(stdout, "Save Camera2 parametor\n");        
        
    }else{
    
        fprintf(stdout, "Please decide the save path!!\n");        
        
    }    
    
    
}
*/
/*
void vtkStereoCalibGUI::displayStereoCalib(int N){
    printf(" done\n");
    // CALIBRATION QUALITY CHECK
    // because the output fundamental matrix implicitly
    // includes all the output information,
    // we can check the quality of calibration using the
    // epipolar geometry constraint: m2^t*F*m1=0
    std::vector<CvPoint3D32f> lines[2];
    points[0].resize(N);
    points[1].resize(N);
    _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    lines[0].resize(N);
    lines[1].resize(N);
    CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
    CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
//Always work in undistorted space
    cvUndistortPoints( &_imagePoints1, &_imagePoints1,
         &_M1, &_D1, 0, &_M1 );
    cvUndistortPoints( &_imagePoints2, &_imagePoints2,
         &_M2, &_D2, 0, &_M2 );
    cvComputeCorrespondEpilines( &_imagePoints1, 1, &_F, &_L1 );
    cvComputeCorrespondEpilines( &_imagePoints2, 2, &_F, &_L2 );
    double avgErr = 0;
    for( i = 0; i < N; i++ )
    {
        double err = fabs(points[0][i].x*lines[1][i].x +
     points[0][i].y*lines[1][i].y + lines[1][i].z)
            + fabs(points[1][i].x*lines[0][i].x +
     points[1][i].y*lines[0][i].y + lines[0][i].z);
        avgErr += err;
    }
    printf( "avg err = %g\n", avgErr/(nframes*n) );
    //COMPUTE AND DISPLAY RECTIFICATION
    CvMat* mx1 = cvCreateMat( imageSize.height,
                              imageSize.width, CV_32F );
    CvMat* my1 = cvCreateMat( imageSize.height,
         imageSize.width, CV_32F );
    CvMat* mx2 = cvCreateMat( imageSize.height,         
         imageSize.width, CV_32F );
    CvMat* my2 = cvCreateMat( imageSize.height,
         imageSize.width, CV_32F );

    double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
    CvMat _R1 = cvMat(3, 3, CV_64F, R1);
    CvMat _R2 = cvMat(3, 3, CV_64F, R2);
    // IF BY CALIBRATED (BOUGUET'S METHOD)
    CvMat _P1 = cvMat(3, 4, CV_64F, P1);
    CvMat _P2 = cvMat(3, 4, CV_64F, P2);
    cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,
                     &_R, &_T,
                     &_R1, &_R2, &_P1, &_P2, 0,
                     0 );//CV_CALIB_ZERO_DISPARITY
    isVerticalStereo = fabs(P2[1][3]) > fabs(P2[0][3]);
    //Precompute maps for cvRemap()
    cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
    cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);
}
    // cvReleaseMat( &mx1 );
    // cvReleaseMat( &my1 );
    // cvReleaseMat( &mx2 );
    // cvReleaseMat( &my2 );

*/


//------------------------------------------------------------------
/*
////////////////////////////////
// displayChessboard_findFlag //
////////////////////////////////
int vtkStereoCalibGUI::createFindChessboardCornersFlag() {//100603-komura
   int flag = 0;
   
   if ( ADAPTIVE_THRESH != 0 ) {
       flag = flag | CV_CALIB_CB_ADAPTIVE_THRESH;
   }
   if ( NORMALIZE_IMAGE != 0 ) {
       flag = flag | CV_CALIB_CB_NORMALIZE_IMAGE;
   }
   if ( FILTER_QUADS != 0 ) {
       flag = flag | CV_CALIB_CB_FILTER_QUADS;
   }
   
   return flag;
}
*/
/*
//------------------------------------------------------------------
///////////////////////
// displayChessboard //
///////////////////////
void vtkStereoCalibGUI::displayChessboard(IplImage* frame){//100603-komura
  CvPoint2D32f corners[CORNER_NUMBER];
  // IplImage *grayImage = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_8U, 1 );
  int cornerCount;               // number of finding coners  
  int findChessboardCornersFlag; // cvFindChessboardCorners of flag
  int findFlag;                  // flag of finding all coners
  
  // create flag for cvChessboardCorners
  findChessboardCornersFlag = createFindChessboardCornersFlag();
  IplImage *halfImage = cvCreateImage (cvSize (frame->width / 2, frame->height / 2), 
                                       frame->depth, frame->nChannels);
  cvResize (frame, halfImage, CV_INTER_AREA);
  
  // find coner
  findFlag=cvFindChessboardCorners(
                                   halfImage,
                                   cvSize( CORNER_WIDTH, CORNER_HEIGHT ),
                                   corners,
                                   &cornerCount,
                                   findChessboardCornersFlag
                                   );
  for(int i=0;i<CORNER_NUMBER;i++){
    corners[i].x = corners[i].x * 2;
    corners[i].y = corners[i].y * 2;
  }
  cvDrawChessboardCorners( frame, cvSize( CORNER_WIDTH, CORNER_HEIGHT ), corners, cornerCount, findFlag );
  cvReleaseImage(&halfImage);
}
*/




























//----------------------------------------------------------------------------

//////////////////
// cameraThread //
//////////////////
// void *vtkStereoCalibGUI::thread_CameraThread(void* t)
// {
    
//     std::cerr << "\n\nmakeThread Start\n\n" << std::endl;
//     // 5/15/ayamada
//     CvCapture* capture = 0;
//     IplImage* captureImage;
//     IplImage* RGBImage;
//     IplImage* captureImageTmp;
    
//     captureImage = NULL;
//     RGBImage = NULL;
//     captureImageTmp = NULL;
    
    
//     vtkMultiThreader::ThreadInfo* vinfo = 
//         static_cast<vtkMultiThreader::ThreadInfo*>(t);
//     vtkStereoCalibGUI* pGUI = 
//         static_cast<vtkStereoCalibGUI*>(vinfo->UserData);
 
  
//     int i=0;
//     while(i<=10){// 5/16/2010 ayamada
//         if( (NULL==(capture = cvCaptureFromCAM(i)))) // 10.01.25 ayamada
//             {
//                 std::cerr << "\n\nCan Not Find A Camera" << std::endl;
//                 i++;                
//             }else{
//             std::cerr << "\n\nConnected Camera Device No:" << i << std::endl;
//             break;
//         }
//     }
//     if(i==11){
//         fprintf(stdout, "\nCan Not Find Camera Device!!\n");
//     }
  
//     if(capture != NULL){
    
//         while(1){
//             if(NULL == (captureImageTmp = cvQueryFrame( capture ))){
//                 sleep(2);
//                 std::cerr << "\nCan Not Take A Picture\n" << std::endl;
//                 continue;
//             }  
      
//             pGUI->imageSize = cvGetSize( captureImageTmp );
//             captureImage = cvCreateImage(pGUI->imageSize, IPL_DEPTH_8U,3); 
//             RGBImage = cvCreateImage(pGUI->imageSize, IPL_DEPTH_8U, 3);
//             pGUI->imageSize = cvGetSize( captureImageTmp );
//             cvFlip(captureImageTmp, captureImage, 0);
//             cvCvtColor( captureImage, RGBImage, CV_BGR2RGB);
//             pGUI->idata = (unsigned char*) RGBImage->imageData;
//             pGUI->importer->SetWholeExtent(0,pGUI->imageSize.width-1,0,pGUI->imageSize.height-1,0,0);
//             pGUI->importer->SetDataExtentToWholeExtent();
//             pGUI->importer->SetDataScalarTypeToUnsignedChar();
//             pGUI->importer->SetNumberOfScalarComponents(3);
//             pGUI->importer->SetImportVoidPointer(pGUI->idata);
//             pGUI->atext->SetInputConnection(pGUI->importer->GetOutputPort());
//             pGUI->atext->InterpolateOn(); 
//             pGUI->importer->Update();
//             break;
//         }
  
//         pGUI->planeRatio = VIEW_SIZE_X / VIEW_SIZE_Y;
//         pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);
//         pGUI->FocalPlaneMapper->SetInput(pGUI->FocalPlaneSource->GetOutput());
//         pGUI->actor->SetMapper(pGUI->FocalPlaneMapper);
//         pGUI->actor->SetUserMatrix(pGUI->ExtrinsicMatrix);
//         pGUI->actor->SetTexture(pGUI->atext);
//         pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor);
//         fprintf(stdout, "\nget camera handle\n");
//         pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );
    
//     }
//     pGUI->makeThread = 3;
//     while(pGUI->makeThread < 4){
//         if(capture != NULL){
//             pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio); 
//             pGUI->FocalPlaneMapper->SetInput(pGUI->FocalPlaneSource->GetOutput());
//             pGUI->actor->SetMapper(pGUI->FocalPlaneMapper);
//             pGUI->actor->SetUserMatrix(pGUI->ExtrinsicMatrix);
//             captureImageTmp = cvQueryFrame( capture );
//             cvFlip(captureImageTmp, captureImage, 0);        
//             cvCvtColor( captureImage, RGBImage, CV_BGR2RGB);
//             pGUI->idata = (unsigned char*) RGBImage->imageData;
//             pGUI->importer->Modified();
//         }
//     }
  
//     std::cerr << "thread was finished!!" << std::endl;

//     if(capture != NULL){ 
//         cvReleaseCapture(&capture);  
//     }
//     pGUI->makeThread = 0; 
//     std::cerr << "makeThread No:"<< pGUI->makeThread << "\n" <<std::endl;
//     return NULL;
        
// }
// //------------------------------------------------------------------
