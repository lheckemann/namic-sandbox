/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#ifndef __vtkTRProstateBiopsyGUI_h
#define __vtkTRProstateBiopsyGUI_h

#include "vtkSlicerModuleGUI.h"
#include "vtkTRProstateBiopsy.h"

class vtkKWPushButton;
class vtkKWPushButtonSet;
class vtkKWEntryWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWMenuButton;
class vtkKWCheckButton;
class vtkKWScaleWithEntry;
class vtkKWEntry;
class vtkKWFrame;
class vtkKWEntryWithLabel;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWWizardWidget;

class vtkCallbackCommand;
class vtkTransform;

class vtkTRProstateBiopsyLogic;
class vtkTRProstateBiopsyStep;
class vtkIGTDataManager;
class vtkIGTPat2ImgRegistration;
class vtkSlicerInteractorStyle;

#include "vtkTRProstateBiopsyLogic.h"

// Description:    
// This class implements Slicer's Volumes GUI
//
class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyGUI :
  public vtkSlicerModuleGUI
{
 public:
  //BTX
  enum {
    SLICE_DRIVER_USER    = 0,
    SLICE_DRIVER_LOCATOR = 1,
    SLICE_DRIVER_RTIMAGE = 2
  };
  enum {
    SLICE_PLANE_RED    = 0,
    SLICE_PLANE_YELLOW = 1,
    SLICE_PLANE_GREEN  = 2
  };
  enum {
    SLICE_RTIMAGE_PERP      = 0,
    SLICE_RTIMAGE_INPLANE90 = 1,
    SLICE_RTIMAGE_INPLANE   = 2
  };
  
  static const double WorkPhaseColor[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const double WorkPhaseColorActive[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const double WorkPhaseColorDisabled[vtkTRProstateBiopsyLogic::NumPhases][3];
  static const char* WorkPhaseStr[vtkTRProstateBiopsyLogic::NumPhases];
  //ETX
  
 public:
  // Description:    
  // Usual vtk class functions
  static vtkTRProstateBiopsyGUI* New (  );
  vtkTypeRevisionMacro ( vtkTRProstateBiopsyGUI, vtkSlicerModuleGUI );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Description:    
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro(Logic, vtkTRProstateBiopsyLogic);
  vtkSetObjectMacro(Logic, vtkTRProstateBiopsyLogic);

  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  void SetModuleLogic(vtkSlicerLogic *logic);

  //void SetAndObserveModuleLogic ( vtkTRProstateBiopsyLogic *logic )
  //{ this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }
  // Description: 
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);

  // Get Target Fiducials (used in the wizard steps)
  vtkGetStringMacro ( FiducialListNodeID );
  vtkSetStringMacro ( FiducialListNodeID );
  vtkGetObjectMacro ( FiducialListNode, vtkMRMLFiducialListNode );
  vtkSetObjectMacro ( FiducialListNode, vtkMRMLFiducialListNode );

  // Description:    
  // This method builds the IGTDemo module GUI
  virtual void BuildGUI ( );
  
  // Description:
  // Add/Remove observers on widgets in the GUI
  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );

  void AddLogicObservers ( );
  void RemoveLogicObservers ( );
  
  
  // Description:
  // Class's mediator methods for processing events invoked by
  // either the Logic, MRML or GUI.    
  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( );
  virtual void Exit ( );
  
  void Init();

  //BTX
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //ETX
  
 protected:
  vtkTRProstateBiopsyGUI ( );
  virtual ~vtkTRProstateBiopsyGUI ( );
  
  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  vtkKWPushButtonSet *WorkPhaseButtonSet;

  //----------------------------------------------------------------
  // Wizard Frame
  
  vtkKWWizardWidget *WizardWidget;
  vtkTRProstateBiopsyStep **WizardSteps;
  
  // Module logic and mrml pointers

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkTRProstateBiopsyLogic *Logic;

  vtkIGTDataManager *DataManager;
  vtkIGTPat2ImgRegistration *Pat2ImgReg;
  vtkCallbackCommand *DataCallbackCommand;

  int SliceDriver0;
  int SliceDriver1;
  int SliceDriver2;

  // Access the slice windows
  vtkSlicerSliceLogic *Logic0;
  vtkSlicerSliceLogic *Logic1;
  vtkSlicerSliceLogic *Logic2;
  vtkMRMLSliceNode *SliceNode0;
  vtkMRMLSliceNode *SliceNode1;
  vtkMRMLSliceNode *SliceNode2;
  vtkSlicerSliceControllerWidget *Control0;
  vtkSlicerSliceControllerWidget *Control1;
  vtkSlicerSliceControllerWidget *Control2;

  //----------------------------------------------------------------
  // Target Fiducials
  //----------------------------------------------------------------

  char *FiducialListNodeID;
  vtkMRMLFiducialListNode *FiducialListNode;

  void UpdateAll();
  
 private:

  vtkTRProstateBiopsyGUI ( const vtkTRProstateBiopsyGUI& ); // Not implemented.
  void operator = ( const vtkTRProstateBiopsyGUI& ); //Not implemented.
  
  void BuildGUIForWorkPhaseFrame();
  void BuildGUIForWizardFrame();
  void BuildGUIForHelpFrame();
  
  int  ChangeWorkPhase(int phase, int fChangeWizard=0);
  void ChangeSlicePlaneDriver(int slice, const char* driver);
  
};



#endif
