/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#ifndef __vtkFourDUsEndoNavGUI_h
#define __vtkFourDUsEndoNavGUI_h

#ifdef WIN32
#include "vtkFourDUsEndoNavWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkFourDUsEndoNavLogic.h"

#include "vtkIGTDataManager.h"
#include "vtkIGTPat2ImgRegistration.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include <string>

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

class vtkFourDUsEndoNavStepBase;

class vtkTransform;
 
#define NUM_PHASES 3

// Description:    
// This class implements Slicer's Volumes GUI
//
class VTK_FourDUsEndoNav_EXPORT vtkFourDUsEndoNavGUI : public vtkSlicerModuleGUI
{
 public:
  
  virtual void Register(vtkObject *o) { Superclass::Register(o); };
  virtual void UnRegister(vtkObject *o) { Superclass::UnRegister(o); };

  //BTX
  enum {
    SLICE_PLANE_RED    = 0,
    SLICE_PLANE_YELLOW = 1,
    SLICE_PLANE_GREEN  = 2
  };
  enum {
    SLICE_RTIMAGE_NONE      = 0,
    SLICE_RTIMAGE_PERP      = 1,
    SLICE_RTIMAGE_INPLANE90 = 2,
    SLICE_RTIMAGE_INPLANE   = 3
  };
  
  static const double WorkPhaseColor[vtkFourDUsEndoNavLogic::NumPhases][NUM_PHASES];
  static const double WorkPhaseColorActive[vtkFourDUsEndoNavLogic::NumPhases][NUM_PHASES];
  static const double WorkPhaseColorDisabled[vtkFourDUsEndoNavLogic::NumPhases][NUM_PHASES];
  static const char* WorkPhaseStr[vtkFourDUsEndoNavLogic::NumPhases];
  //ETX
  
 public:
  // Description:    
  // Usual vtk class functions
  static vtkFourDUsEndoNavGUI* New (  );
  vtkTypeRevisionMacro ( vtkFourDUsEndoNavGUI, vtkSlicerModuleGUI );
  void PrintSelf (ostream& os, vtkIndent indent );
  
  //SendDATANavitrack
  // Description:    
  // Get methods on class members (no Set methods required)
  vtkGetObjectMacro ( Logic, vtkFourDUsEndoNavLogic );
  
  // Description:
  // API for setting VolumeNode, VolumeLogic and
  // for both setting and observing them.
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { this->SetLogic ( vtkObjectPointer (&this->Logic), logic ); }
  void SetAndObserveModuleLogic ( vtkFourDUsEndoNavLogic *logic )
  { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }
  // Description: 
  // Get wizard widget
  vtkGetObjectMacro(WizardWidget, vtkKWWizardWidget);

  // Description:    
  // This method builds the IGTDemo module GUI
  virtual void BuildGUI ( );

  // Description:    
  // This method builds the IGTDemo module GUI
  virtual void TearDownGUI ( );
  
  // Description:
  // Add/Remove observers on widgets in the GUI
  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );

  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  // Description: 
  // Get the categorization of the module.
  const char *GetCategory() const { return "IGT"; }
  
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
  vtkFourDUsEndoNavGUI ( );
  virtual ~vtkFourDUsEndoNavGUI ( );
  
  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  vtkKWPushButtonSet *WorkPhaseButtonSet;

  //----------------------------------------------------------------
  // Wizard Frame
  
  vtkKWWizardWidget *WizardWidget;
  vtkFourDUsEndoNavStepBase **WizardSteps;
  
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkFourDUsEndoNavLogic *Logic;

  vtkIGTDataManager *DataManager;
  vtkIGTPat2ImgRegistration *Pat2ImgReg;
  vtkCallbackCommand *DataCallbackCommand;

  //BTX
  std::string LocatorModelID;
  std::string LocatorModelID_new;
  //ETX
  
  //----------------------------------------------------------------
  // Target Fiducials
  //----------------------------------------------------------------

  void UpdateAll();
  
 private:

  vtkFourDUsEndoNavGUI ( const vtkFourDUsEndoNavGUI& ); // Not implemented.
  void operator = ( const vtkFourDUsEndoNavGUI& ); //Not implemented.
  
  void BuildGUIForWorkPhaseFrame();
  void BuildGUIForWizardFrame();
  void BuildGUIForHelpFrame();

  int  ChangeWorkPhase(int phase, int fChangeWizard=0);
  void ChangeSlicePlaneDriver(int slice, const char* driver);


  int Entered;
  
};



#endif