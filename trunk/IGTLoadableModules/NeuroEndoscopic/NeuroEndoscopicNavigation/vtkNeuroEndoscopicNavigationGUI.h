/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkNeuroEndoscopicNavigationGUI_h
#define __vtkNeuroEndoscopicNavigationGUI_h

#ifdef WIN32
#include "vtkNeuroEndoscopicNavigationWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkNeuroEndoscopicNavigationLogic.h"

#include <vector>               
#include <string>               
#include <algorithm>            
#include <stdio.h>              
#include <ctype.h>       

#include "vtkKWPushButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWEntryWithLabel.h"

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlClientSocket.h>

class vtkKWPushButton;
class vtkKWMenuButtonWithLabel;
class vtkKWEntryWithLabel;

class VTK_NeuroEndoscopicNavigation_EXPORT vtkNeuroEndoscopicNavigationGUI :
 public vtkSlicerModuleGUI
{
 public:
  vtkTypeRevisionMacro( vtkNeuroEndoscopicNavigationGUI, vtkSlicerModuleGUI);

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkNeuroEndoscopicNavigationLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }


 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkNeuroEndoscopicNavigationGUI();
  ~vtkNeuroEndoscopicNavigationGUI();

 private:
  vtkNeuroEndoscopicNavigationGUI(const vtkNeuroEndoscopicNavigationGUI&); // Not implemented.
  void operator = ( const vtkNeuroEndoscopicNavigationGUI& ); //Not implemented.

 /////////////
 // public  //
 /////////////
 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkNeuroEndoscopicNavigationGUI* New();
  void Init();
  virtual void Enter();
  virtual void Exit();
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers();   
  virtual void RemoveGUIObservers();

  //----------------------------------------------------------------
  // Event Handler
  //----------------------------------------------------------------
  virtual void ProcessLogicEvents(vtkObject *caller, unsigned long event, void *callData){};
  virtual void ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);

  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI();
  void BuildGUIForHelpFrame();
  void BuildNeuroEndoscopicNavigationFrame();

 protected:

  //----------------------------------------------------------------
  // GUI Widgets
  //----------------------------------------------------------------
   
    // OpenIGTLink   
  
  vtkKWEntryWithLabel          *OpenIGTLinkHost;
  vtkKWEntryWithLabel          *OpenIGTLinkPort;
  vtkKWPushButton              *OpenIGTLinkConnectButton;
  vtkKWPushButton              *OpenIGTLinkDisconnectButton;

    // RS232 

  vtkKWMenuButtonWithLabel     *RS232BitRate;
  vtkKWMenuButtonWithLabel     *RS232Parity;
  vtkKWPushButton              *RS232ApplyButton;

    // Send commands

  vtkKWMenuButtonWithLabel     *CommandType;
  vtkKWMenuButtonWithLabel     *Commands;
  vtkKWPushButton              *SendCommandButton;
  vtkKWPushButton              *StopReceivingData; 

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  vtkNeuroEndoscopicNavigationLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;

 
};

#endif 
