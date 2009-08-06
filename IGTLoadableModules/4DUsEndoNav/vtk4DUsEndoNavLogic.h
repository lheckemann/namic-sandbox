/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

// .NAME vtk4DUsEndoNavLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtk4DUsEndoNavLogic_h
#define __vtk4DUsEndoNavLogic_h

#include <pthread.h>

#include "vtk4DUsEndoNavWin32Header.h"

#include "vtkKWTkUtilities.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLSliceNode.h"

class vtk4DUsEndoNavGUI;

class VTK_4DUsEndoNav_EXPORT vtk4DUsEndoNavLogic : public vtkSlicerModuleLogic
{

 public:
  //BTX
  enum WorkPhase {
    StartUp = 0,
    Planning,
    Calibration,
    //Targeting,
    //Manual,
    //Emergency,
    NumPhases,
  };
  enum {  // Events
    LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
    //SliceUpdateEvent        = 50002,
  };

  //ETX
  
  // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)
  static const char* WorkPhaseKey[vtk4DUsEndoNavLogic::NumPhases];

 public:
  
  static vtk4DUsEndoNavLogic *New();
  
  vtkTypeRevisionMacro(vtk4DUsEndoNavLogic,vtkObject);
  
  vtkGetMacro ( CurrentPhase,         int );
  vtkGetMacro ( PrevPhase,            int );
  vtkGetMacro ( PhaseTransitionCheck, bool );
  vtkSetMacro ( PhaseTransitionCheck, bool );
  
  vtkGetMacro ( Connection,              bool );
  vtkGetMacro ( RobotWorkPhase,           int );
  vtkGetMacro ( ScannerWorkPhase,         int );


  void SetGUI(vtk4DUsEndoNavGUI* gui) { this->GUI = gui; };
  vtk4DUsEndoNavGUI* GetGUI()         { return this->GUI; };

  void PrintSelf(ostream&, vtkIndent);
  
  
  int  Enter();
  int  SwitchWorkPhase(int);
  void  TimerHandler();
  int  IsPhaseTransitionable(int);
  
  int  ConnectTracker(const char* filename);
  int  DisconnectTracker();
  
  int  RobotStop();
  int  RobotMoveTo(float px, float py, float pz,
                   float nx, float ny, float nz,
                   float tx, float ty, float tz);
  int  RobotMoveTo(float position[3], float orientation[4]);

  int  RobotMoveTo();
  int  SendZFrame();
  
  int  ScanStart();
  int  ScanPause();
  int  ScanStop();
  
  //BTX
  //Image* ReadCalibrationImage(const char* filename, int* width, int* height,
  //                            std::vector<float>& position, std::vector<float>& orientation);
  //ETX
  
  int WorkPhaseStringToID(const char* string);

  const char* GetRobotCommandNodeID()    { return this->RobotCommandNodeID.c_str();   };
  const char* GetRobotTargetNodeID()     { return this->RobotTargetNodeID.c_str();    };
  const char* GetZFrameTransformNodeID() { return this->ZFrameTransformNodeID.c_str();};
  const char* GetZFrameModelNodeID()     { return this->ZFrameModelNodeID.c_str();};

 protected:
  
  vtk4DUsEndoNavLogic();
  ~vtk4DUsEndoNavLogic();
  vtk4DUsEndoNavLogic(const vtk4DUsEndoNavLogic&);
  void operator=(const vtk4DUsEndoNavLogic&);
  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);

  void UpdateAll();
  void UpdateSliceDisplay();
  void UpdateLocator();


  const char* AddZFrameModel(const char* nodeName);

  vtkCallbackCommand *DataCallbackCommand;

  
 private:
  
  vtk4DUsEndoNavGUI* GUI;

  static const int PhaseTransitionMatrix[NumPhases][NumPhases];
  
  int   CurrentPhase;
  int   PrevPhase;
  int   PhaseComplete;
  bool  Connected;
  bool  PhaseTransitionCheck;
  bool  RealtimeImageUpdate;

  int   TimerOn;

  //BTX
  std::string RobotCommandNodeID;   
  std::string RobotTargetNodeID;    
  std::string ZFrameTransformNodeID;
  std::string ZFrameModelNodeID;
  //ETX


  //----------------------------------------------------------------
  // Locator
  //----------------------------------------------------------------

  // Junichi Tokuda on 11/27/2007:
  // What's a difference between LocatorMatrix and Locator Transform???

  bool  Connection;  
  int   RobotWorkPhase;
  int   ScannerWorkPhase;

  

};

#endif


  
