/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#ifndef __vtkUltrasound4DGUI_h
#define __vtkUltrasound4DGUI_h

#ifdef WIN32
#include "vtkUltrasound4DWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkUltrasound4DLogic.h"

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"

class vtkKWPushButton;
class vtkKWTopLevel;
class vtkKWFrame;
class vtkKWFileBrowserDialog;
class vtkMRMLIGTLConnectorNode;

class VTK_Ultrasound4D_EXPORT vtkUltrasound4DGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkUltrasound4DGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkUltrasound4DLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkUltrasound4DGUI ( );
  virtual ~vtkUltrasound4DGUI ( );

 private:
  vtkUltrasound4DGUI ( const vtkUltrasound4DGUI& ); // Not implemented.
  void operator = ( const vtkUltrasound4DGUI& ); //Not implemented.

  //BTX
  int GetTestImage(igtl::ImageMessage::Pointer& msg,  int i);
  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
  //ETX

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkUltrasound4DGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForLoadingRAWFiles();
  void CreateFileSelector();

  //----------------------------------------------------------------
  // OpenIGTLink Connection
  //----------------------------------------------------------------

  int CreateOpenIGTLinkServerNode();
  int StartOpenIGTLinkClient();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* VolumeRenderingButton;
  vtkKWPushButton* SelectDirectoryButton;
  vtkKWPushButton* OpenIGTLinkConnectButton;

  //----------------------------------------------------------------
  // Directory Explorer widgets
  //----------------------------------------------------------------

  vtkKWFileBrowserDialog* fileSelector;

  //----------------------------------------------------------------
  // OpenIGTLink Connection
  //----------------------------------------------------------------

  vtkMRMLIGTLConnectorNode* OpenIGTLinkServerNode;  

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkUltrasound4DLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
