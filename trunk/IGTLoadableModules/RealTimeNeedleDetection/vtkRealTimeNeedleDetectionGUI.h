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

#include "vtkIGTDataManager.h"

#include "vtkRealTimeNeedleDetectionLogic.h"
#include "ImageProcessor.h"
#include "vtkCylinderSource.h"
#include <time.h> //TODO: take that out when done measuring

class vtkKWPushButton;

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
  void ProcessImage(vtkImageData* pImageData, int i); // Function to evoke changes in the image in every itaration | not used anymore
  void GetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion);
  void SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion, bool above);
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
  vtkKWPushButton*      pStartButton;
  vtkKWPushButton*      pStopButton;
  vtkKWCheckButton*     pShowNeedleButton;
  vtkKWMenuButton*      pSourceMenu;
  vtkKWEntry*           pScannerIDEntry;
  vtkKWScaleWithEntry*  pThresholdScale;
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
  vtkMRMLVolumeNode*          pVolumeNode;          // MRML node, which contains the processed image
  vtkMRMLModelNode*           pNeedleModelNode;     // MRML node, which contains the detected needle displayed as transform
  vtkMRMLLinearTransformNode* pNeedleTransformNode;
  vtkMRMLLinearTransformNode* pScanPlaneNormalNode;
  //TODO:Steve Use a global scene pointer?
    
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  vtkIGTDataManager*                DataManager;
  vtkRealTimeNeedleDetectionLogic*  Logic;
  vtkCallbackCommand*               DataCallbackCommand;
  int                        CloseScene;    // TODO: Do I need that?
  int                        started;       // flag whether to process the new image from the scanner
  int                        ROIpresent;    // flag whether a MRMLROINode is present and the coordinate input has to be disabled
  int                        showNeedle;    // flag whether to show the pNeedleNode in Slicer
  //TODO: Do I really  need the scan plane?
  int                        scanPlane;     // variable to indicate the scan plane: AXIAL, CORONAL, SAGITTAL
  int                        needleOrigin;  // variable to indicate the direction the needle enters the image from 
                                            // = LEFT, RIGHT, POSTERIOR, ANTERIOR, INFERIOR, SUPERIOR 
  
  //----------------------------------------------------------------
  // Image Values
  //----------------------------------------------------------------   
  double          needleDetectionThreshold;
  int             initialXLowerBound;
  int             initialXUpperBound;        
  int             initialYLowerBound;     
  int             initialYUpperBound;
  int             currentXLowerBound;
  int             currentXUpperBound;        
  int             currentYLowerBound;     
  int             currentYUpperBound;
  int             currentXImageRegionSize;
  int             currentYImageRegionSize;
  int             imageDimensions[3]; // the number of cells on x, y and z axis
  double          imageSpacing[3];
  double          imageOrigin[3];
  int             scalarSize;         // 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  unsigned long   lastModified;       // saves the time of the last change of the source image
  void*           pImage;             // pointer to the image in pImageData
  ImageProcessor* pImageProcessor;
};



#endif
