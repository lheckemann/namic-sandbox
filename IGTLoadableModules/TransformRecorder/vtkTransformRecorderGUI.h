/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkTransformRecorderGUI_h
#define __vtkTransformRecorderGUI_h

#ifdef WIN32
#include "vtkTransformRecorderWin32Header.h"
#endif

#include <vector>
#include <string>

#include "vtkCallbackCommand.h"
#include "vtkSmartPointer.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWPushButton.h"

#include "vtkSlicerModuleGUI.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkTransformRecorderLogic.h"

#include "vtkMRMLTransformRecorderNode.h"

class vtkKWLoadSaveButtonWithLabel;
class vtkKWLabel;


// Quick message buttons. -----------------------------------------------------

static const int BUTTON_COUNT = 5;
static const char* BUTTON_TEXTS[ BUTTON_COUNT ] =
  {
  "Alignment outside",
  "Pierce skin",
  "Retract inside",
  "Adjust angle inside",
  "Needle placed"
  };
static const char* BUTTON_MESSAGES[ BUTTON_COUNT ] =
  {
  "Operator does alignment outside.",
  "Operator does pierce skin.",
  "Operator does retract inside.",
  "Operator does adjust angle inside.",
  "Operator placed the needle."
  };

// ----------------------------------------------------------------------------



class VTK_TransformRecorder_EXPORT vtkTransformRecorderGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkTransformRecorderGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkTransformRecorderLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  
  
  vtkSetObjectMacro( ModuleNode, vtkMRMLTransformRecorderNode );
  
  
 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkTransformRecorderGUI ( );
  virtual ~vtkTransformRecorderGUI ( );

  void UpdateGUI();
  
 private:
  vtkTransformRecorderGUI ( const vtkTransformRecorderGUI& ); // Not implemented.
  void operator = ( const vtkTransformRecorderGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkTransformRecorderGUI* New ();
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
  void RemoveMRMLObservers();

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

protected:
  
  void SelectLogFile();
  

public:  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  
  void BuildGUIForIOFrame();
  void BuildGUIForControlsFrame();
  void BuildGUIForMonitorFrame();
  

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
    
  vtkSlicerNodeSelectorWidget* ModuleNodeSelector;
  vtkSlicerNodeSelectorWidget* TransformSelector;
  vtkKWLoadSaveButtonWithLabel* FileSelectButton;
  vtkKWLabel* LogFileLabel;
  
  vtkKWPushButton* StartButton;
  vtkKWPushButton* StopButton;
  vtkKWEntry* CustomEntry;
  vtkKWPushButton* CustomButton;
  
  //BTX
  std::vector< vtkKWPushButton* > MessageButtons;
  //ETX
  
  vtkKWLabel* StatusLabel;
  vtkKWLabel* TranslationLabel;
  
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkTransformRecorderLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;


private:
  
  void SetAndObserveModuleNodeID( const char *nodeID );
  vtkMRMLTransformRecorderNode* GetModuleNode();

  vtkSetStringMacro( ModuleNodeID );
  char* ModuleNodeID;
  vtkMRMLTransformRecorderNode* ModuleNode;

};



#endif
