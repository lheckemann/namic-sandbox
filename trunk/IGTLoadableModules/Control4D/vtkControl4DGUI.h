/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#ifndef __vtkControl4DGUI_h
#define __vtkControl4DGUI_h

#ifdef WIN32
#include "vtkControl4DWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkDoubleArray.h"

#include "vtkControl4DLogic.h"

#include <map>

class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWPiecewiseFunctionEditor;
class vtkKWMenuButton;
class vtkKWSpinBox;
class vtkKWCanvas;
class vtkKWProgressDialog;
class vtkKWRadioButtonSet;
class vtkKWRadioButton;
class vtkKWRange;


class VTK_Control4D_EXPORT vtkControl4DGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkControl4DGUI, vtkSlicerModuleGUI );

  //BTX
  typedef std::map<std::string, std::string> RegistrationParametersType;
  //ETX

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkControl4DLogic );
  virtual void SetModuleLogic ( vtkControl4DLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  virtual void SetAndObserveModuleLogic ( vtkControl4DLogic *logic )
  { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }
  virtual void SetModuleLogic( vtkSlicerLogic *logic )
  { this->SetModuleLogic (reinterpret_cast<vtkControl4DLogic*> (logic)); }
  

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkControl4DGUI ( );
  virtual ~vtkControl4DGUI ( );

 private:
  vtkControl4DGUI ( const vtkControl4DGUI& ); // Not implemented.
  void operator = ( const vtkControl4DGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkControl4DGUI* New ();
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
  void BuildGUIForLoadFrame();
  void BuildGUIForFrameControlFrame();
  void BuildGUIForFunctionViewer();
  void BuildGUIForRegistrationFrame();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  //----------------------------------------------------------------
  // Viewer control
  //----------------------------------------------------------------

  void SetForeground(int index);
  void SetBackground(int index);
  void SetWindowLevelForCurrentFrame();
  void UpdateMaskSelectMenu();
  void SelectMask(const char* nodeID, int label);
  void UpdateFunctionEditor(vtkDoubleArray* data);

  //----------------------------------------------------------------
  // Registration
  //----------------------------------------------------------------
  //BTX
  int  RunSeriesRegistration(int sIndex, int eIndex, int kIndex, RegistrationParametersType& param);
  int  RunRegistration(vtkMRMLScalarVolumeNode* fixedNode, vtkMRMLScalarVolumeNode* movingNode, vtkMRMLScalarVolumeNode* outputNode,
                       RegistrationParametersType& param);

  //ETX

 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWProgressDialog *ProgressDialog;

  vtkKWLoadSaveButtonWithLabel* SelectImageButton;
  vtkKWPushButton* LoadImageButton;
  
  vtkKWRange *WindowLevelRange;
  vtkKWRange *ThresholdRange;

  vtkKWScaleWithEntry* ForegroundVolumeSelectorScale;
  vtkKWScaleWithEntry* BackgroundVolumeSelectorScale;

  vtkKWMenuButton* MaskSelectMenu;
  vtkKWSpinBox*    MaskSelectSpinBox;
  vtkKWCanvas*     MaskColorCanvas;

  vtkKWPushButton* RunPlotButton;
  vtkKWRadioButtonSet* PlotTypeButtonSet;
  vtkKWPiecewiseFunctionEditor* FunctionEditor;
  vtkKWLoadSaveButtonWithLabel* SavePlotButton;

  vtkKWSpinBox*    RegistrationStartIndexSpinBox;
  vtkKWSpinBox*    RegistrationEndIndexSpinBox;
  vtkKWPushButton* StartRegistrationButton;

  vtkKWEntryWithLabel* IterationsEntry;
  vtkKWEntryWithLabel* GridSizeEntry;
  vtkKWEntryWithLabel* HistogramBinsEntry;
  vtkKWEntryWithLabel* SpatialSamplesEntry;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  RegistrationParametersType DefaultRegistrationParam;
  
  vtkControl4DLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  double  RangeLower;
  double  RangeUpper;

  double  Window;
  double  Level;
  double  ThresholdUpper;
  double  ThresholdLower;

  //BTX
  typedef std::vector<int> WindowLevelUpdateStatusType;
  typedef std::vector<std::string> NodeIDListType;
  //ETX
  
  WindowLevelUpdateStatusType WindowLevelUpdateStatus;
  NodeIDListType MaskNodeIDList;

};



#endif
