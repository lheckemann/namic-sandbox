/*==========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
  Author: Roland Goerlitz, University of Karlsruhe

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: ??$
  Date:      $Date: 2009/02/12 19:10:09$
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
  void SetImageRegion(vtkImageData* pImageData, unsigned char* pImageRegion);
  void operator = ( const vtkRealTimeNeedleDetectionGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------
  static vtkRealTimeNeedleDetectionGUI* New ();
  void Init();
  virtual void Enter();
  virtual void Exit();
  void PrintSelf (ostream& os, vtkIndent indent );
  vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

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
  vtkKWPushButton* pStartButton;
  vtkKWPushButton* pStopButton;
  vtkKWMenuButton* pSourceMenu;
  vtkKWEntry*      pScannerIDEntry;
  vtkKWEntry*      pXLowerEntry;
  vtkKWEntry*      pXUpperEntry;
  vtkKWEntry*      pYLowerEntry;
  vtkKWEntry*      pYUpperEntry;
  
  //----------------------------------------------------------------
  // MRML nodes
  //----------------------------------------------------------------
  vtkMRMLVolumeNode*  pSourceNode;   // MRML node, which Slicer received via OpenIGTLink from the Scanner
  vtkMRMLVolumeNode*  pVolumeNode;   // MRML node, which contains the processed image
    
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------
  vtkIGTDataManager*                 DataManager;
  vtkRealTimeNeedleDetectionLogic*   Logic;
  vtkCallbackCommand*               DataCallbackCommand;
  int                        CloseScene;
  int                         started;
  
  //----------------------------------------------------------------
  // Image Values
  //----------------------------------------------------------------   
  int             xLowerBound;
  int             xUpperBound;        
  int             yLowerBound;     
  int             yUpperBound;
  int             xImageRegionSize;
  int             yImageRegionSize;
  int              imageDimensions[3]; // the number of cells on x, y and z axis
  double          imageSpacing[3];
  double          imageOrigin[3];
  int             scalarSize;          // 0,1 = 0 | 2,3 (char) = 1 | 4,5 (short) = 2 | 6,7 = 4
  unsigned long    lastModified;       // saves the time of the last change of the source image
  void*            pImage;             // pointer to the image in pImageData
  ImageProcessor* pImageProcessor;
};



#endif
