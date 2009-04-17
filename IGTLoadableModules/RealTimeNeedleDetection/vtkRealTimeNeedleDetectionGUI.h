/*==========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
  Author: Roland Goerlitz, University of Karlsruhe

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: ??$
  Date:      $Date: 2009/03/09 09:10:09$
  Version:   $Revision: 1.00$

==========================================================================*/

#ifndef __vtkRealTimeNeedleDetectionGUI_h
#define __vtkRealTimeNeedleDetectionGUI_h

#ifdef WIN32
#include "vtkRealTimeNeedleDetectionWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkRealTimeNeedleDetectionLogic.h"
#include "ImageProcessor.h"
#include "vtkCylinderSource.h"
#include <time.h> //TODO: take that out when done measuring

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;  //TODO:Steve I needed to add this line to make the selector work. Why do I not need classes for the slider, etc widget?

class VTK_RealTimeNeedleDetection_EXPORT vtkRealTimeNeedleDetectionGUI : public vtkSlicerModuleGUI
{
 public:
  vtkTypeRevisionMacro (vtkRealTimeNeedleDetectionGUI, vtkSlicerModuleGUI);

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------
  vtkGetObjectMacro (Logic, vtkRealTimeNeedleDetectionLogic);
  void SetModuleLogic (vtkSlicerLogic* logic)
  { 
    this->SetLogic (vtkObjectPointer (&this->Logic), logic);
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------
  vtkRealTimeNeedleDetectionGUI ( );
  virtual ~vtkRealTimeNeedleDetectionGUI ( );

 private:
  vtkRealTimeNeedleDetectionGUI(const vtkRealTimeNeedleDetectionGUI& ); // Not implemented.
  void GetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion);
  void SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion, int position);
  void orientOutputImage(double fovI, double fovJ, double fovK);
  void operator = (const vtkRealTimeNeedleDetectionGUI&); //Not implemented.
  void MakeNeedleModel();
  double diffclock(clock_t clock1,clock_t clock2);

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------
  static vtkRealTimeNeedleDetectionGUI* New ();
  void Init();
  virtual void Enter();
  virtual void Exit();
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
  virtual void ProcessLogicEvents (vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents (vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents (vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------
  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForGeneralParameters();
  void BuildGUIForDebugParameters();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------
  void UpdateAll();
  void UpdateGUI();


 protected:  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------
  vtkSlicerNodeSelectorWidget*  pVolumeSelector;
  vtkKWRadioButtonSet*          pEntryPointButtonSet;
  vtkKWRadioButtonSet*          pDebugModeButtonSet;
  vtkKWScaleWithEntry*          pThresholdScale;
  vtkKWScaleWithEntry*          pIntensityScale;
  vtkKWScaleWithEntry*          pGaussScale;
  vtkKWEntry*                   pDilateEntry;
  vtkKWEntry*                   pErodeEntry;
  vtkKWPushButton*              pStartButton;
  vtkKWPushButton*              pStopButton;
  vtkKWCheckButton*             pShowNeedleButton;  
  vtkKWEntry*           pXLowerEntry;
  vtkKWEntry*           pXUpperEntry;
  vtkKWEntry*           pYLowerEntry;
  vtkKWEntry*           pYUpperEntry;
  vtkKWEntry*           pZLowerEntry;
  vtkKWEntry*           pZUpperEntry;
  
  //----------------------------------------------------------------
  // MRML nodes
  //----------------------------------------------------------------
  vtkMRMLVolumeNode*          pSourceNode;          // MRML node, which Slicer received via OpenIGTLink from the Scanner
  vtkMRMLVolumeNode*          pOutputNode;          // MRML node, which contains the debug image
  vtkMRMLModelNode*           pNeedleModelNode;     // MRML node, which contains the displayable detected needle
  vtkMRMLLinearTransformNode* pNeedleTransformNode; // MRML node, which contains a transform of the detected needle
  vtkMRMLLinearTransformNode* pScanPlaneNormalNode; // MRML node, which contains a transform of the normal to the detected needle
    
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  vtkRealTimeNeedleDetectionLogic*  Logic;
  vtkCallbackCommand*               DataCallbackCommand;
  int                        CloseScene;    // TODO: Do I need that?
  int                        debug;         // variable to indicate the debug mode 0: no pOutputNode/no images written to file, 1:pOutputNode/no images written to file, 2:pOutputNode/images written to file 
  int                        started;       // flag whether to process the new image from the scanner or not
  int                        ROIpresent;    // flag whether a MRMLROINode is present and the coordinate input has to be disabled or not
  int                        showNeedle;    // flag whether to show the pNeedleNode in Slicer or not
  int                        needleOrigin;  // variable to indicate the direction the needle enters the image from 
                                            // = PATIENTLEFT, PATIENTRIGHT, PATIENTPOSTERIOR, PATIENTANTERIOR, PATIENTINFERIOR, PATIENTSUPERIOR 
  
  //----------------------------------------------------------------
  // Image Values
  //----------------------------------------------------------------   
  double          needleDetectionThreshold;
  double          needleIntensity;
  double          gaussVariance;
  int             imageRegionSize[2];   //these are the current values for the 2D image region that gets processed in the ImageProcessor
  int             imageRegionLower[2];  //these are the current values for the 2D image region that gets processed in the ImageProcessor
  int             imageRegionUpper[2];  //these are the current values for the 2D image region that gets processed in the ImageProcessor
  //TODO: Can I make the "imageRegion.." arrays local variables? 
  int             currentLowerBound[3];
  int             currentUpperBound[3];        
  int             currentImageRegionSize[3];
  int             imageDimensions[3]; // the number of cells on x, y and z axis
  double          imageSpacing[3];
  double          imageOrigin[3];
  int             scalarSize;         // 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  unsigned long   lastModified;       // saves the time of the last change of the source image
  void*           pImage;             // pointer to the actual image in pImageData received via openIGTLink
  ImageProcessor* pImageProcessor;
};



#endif
