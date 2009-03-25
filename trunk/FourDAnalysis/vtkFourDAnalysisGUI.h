/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/FourDAnalysis/vtkFourDAnalysisGUI.h $
  Date:      $Date: 2009-02-03 12:05:00 -0500 (Tue, 03 Feb 2009) $
  Version:   $Revision: 3633 $

==========================================================================*/

#ifndef __vtkFourDAnalysisGUI_h
#define __vtkFourDAnalysisGUI_h

#ifdef WIN32
#include "vtkFourDAnalysisWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkDoubleArray.h"

#include "vtkFourDAnalysisLogic.h"
#include "vtkKWPlotGraph.h"
#include "vtkSlicerNodeSelectorWidget.h"

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
class vtkIntensityCurves;
class vtkKWCheckButtonWithLabel;

class VTK_FourDAnalysis_EXPORT vtkFourDAnalysisGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkFourDAnalysisGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkFourDAnalysisLogic );
  virtual void SetModuleLogic ( vtkFourDAnalysisLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  virtual void SetAndObserveModuleLogic ( vtkFourDAnalysisLogic *logic )
  { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }
  virtual void SetModuleLogic( vtkSlicerLogic *logic )
  { this->SetModuleLogic (reinterpret_cast<vtkFourDAnalysisLogic*> (logic)); }
  

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkFourDAnalysisGUI ( );
  virtual ~vtkFourDAnalysisGUI ( );

 private:
  vtkFourDAnalysisGUI ( const vtkFourDAnalysisGUI& ); // Not implemented.
  void operator = ( const vtkFourDAnalysisGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkFourDAnalysisGUI* New ();
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
  void BuildGUIForLoadFrame(int show);
  void BuildGUIForFrameControlFrame(int show);
  void BuildGUIForFunctionViewer(int show);
  void BuildGUIForMapGenerator(int show);
  void BuildGUIForCroppingFrame(int show);
  void BuildGUIForRegistrationFrame(int show);


  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

  //----------------------------------------------------------------
  // Viewer control
  //----------------------------------------------------------------

  void SetForeground(const char* bundleID, int index);
  void SetBackground(const char* bundleID, int index);
  void SetWindowLevelForCurrentFrame();
  void UpdateSeriesSelectorMenus();
  void UpdateMaskSelectMenu();
  void UpdateIntensityPlot(vtkIntensityCurves* intensityCurves);
  void UpdateIntensityPlotWithFittedCurve(vtkIntensityCurves* intensityCurves, vtkDoubleArray* array);

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

  vtkKWLoadSaveButtonWithLabel* SelectInputDirectoryButton;
  vtkKWPushButton*              LoadImageButton;
  vtkKWMenuButton*              SaveSeriesMenu;
  vtkKWLoadSaveButtonWithLabel* SelectOutputDirectoryButton;
  vtkKWPushButton*              SaveImageButton;
  
  vtkKWRange *WindowLevelRange;
  vtkKWRange *ThresholdRange;

  vtkKWMenuButton*     ForegroundSeriesMenu;
  vtkKWScaleWithEntry* ForegroundVolumeSelectorScale;
  vtkKWMenuButton*     BackgroundSeriesMenu;
  vtkKWScaleWithEntry* BackgroundVolumeSelectorScale;

  // -----------------------------------------
  // Intensity Curve

  vtkKWMenuButton*  SeriesToPlotMenu;
  vtkKWMenuButton*  MaskSelectMenu;
  vtkKWPlotGraph*   IntensityPlot;
  vtkKWCheckButtonWithLabel* ErrorBarCheckButton;

  vtkKWMenuButton* FittingLabelMenu;
  vtkKWLoadSaveButtonWithLabel* CurveScriptSelectButton;
  vtkKWSpinBox*    CurveFittingStartIndexSpinBox;
  vtkKWSpinBox*    CurveFittingEndIndexSpinBox;
  vtkKWPushButton* RunFittingButton;

  vtkKWPushButton* RunPlotButton;
  vtkKWLoadSaveButtonWithLabel* SavePlotButton;

  // -----------------------------------------
  // Parameter Map

  vtkKWMenuButton*             MapInputSeriesMenu;
  vtkSlicerNodeSelectorWidget* MapOutputSelector;
  vtkKWEntry*                  MapOutputVolumePrefixEntry;
  //vtkKWMenuButton* MapOutputVolumeMenu;
  vtkKWLoadSaveButtonWithLabel* ScriptSelectButton;
  vtkKWPushButton* RunScriptButton;

  vtkKWSpinBox*    MapIMinSpinBox;
  vtkKWSpinBox*    MapIMaxSpinBox;
  vtkKWSpinBox*    MapJMinSpinBox;
  vtkKWSpinBox*    MapJMaxSpinBox;
  vtkKWSpinBox*    MapKMinSpinBox;
  vtkKWSpinBox*    MapKMaxSpinBox;

  // -----------------------------------------
  // 4D Cropping

  vtkKWMenuButton* CropSeriesMenu;
  vtkKWPushButton* StartCroppingButton;
  vtkKWSpinBox*    CropIMinSpinBox;
  vtkKWSpinBox*    CropIMaxSpinBox;
  vtkKWSpinBox*    CropJMinSpinBox;
  vtkKWSpinBox*    CropJMaxSpinBox;
  vtkKWSpinBox*    CropKMinSpinBox;
  vtkKWSpinBox*    CropKMaxSpinBox;

  // -----------------------------------------
  // 4D Registration

  vtkKWMenuButton* InputSeriesMenu;
  vtkKWMenuButton* OutputSeriesMenu;
  vtkKWSpinBox*    RegistrationFixedImageIndexSpinBox;
  vtkKWSpinBox*    RegistrationStartIndexSpinBox;
  vtkKWSpinBox*    RegistrationEndIndexSpinBox;
  vtkKWPushButton* StartRegistrationButton;

  vtkKWEntryWithLabel* AffineIterationsEntry; 
  vtkKWEntryWithLabel* AffineSamplesEntry;    
  vtkKWEntryWithLabel* AffineHistgramBinEntry;
  vtkKWEntryWithLabel* AffineTranslationEntry;

  vtkKWEntryWithLabel* IterationsEntry;
  vtkKWEntryWithLabel* GridSizeEntry;
  vtkKWEntryWithLabel* HistogramBinsEntry;
  vtkKWEntryWithLabel* SpatialSamplesEntry;


  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  //BTX
  vtkFourDAnalysisLogic::RegistrationParametersType DefaultAffineRegistrationParam;
  vtkFourDAnalysisLogic::RegistrationParametersType DefaultRegistrationParam;
  //ETX
  
  vtkFourDAnalysisLogic *Logic;
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
  NodeIDListType BundleNodeIDList;

  int BundleNameCount; // used to name 4D bundle

  vtkIntensityCurves* IntensityCurves;

};



#endif
