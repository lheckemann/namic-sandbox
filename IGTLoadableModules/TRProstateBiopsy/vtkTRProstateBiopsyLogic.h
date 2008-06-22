/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

// .NAME vtkTRProstateBiopsyLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkTRProstateBiopsyLogic_h
#define __vtkTRProstateBiopsyLogic_h

#include "vtkTRProstateBiopsyWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLFiducialListNode.h"

//BTX
#define TRPB_CALIBRATION "CALIBRATION"
#define TRPB_TARGETING "TARGETING"
#define TRPB_VERIFICATION "VERIFICATION"
//ETX

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyLogic :
  public vtkSlicerModuleLogic
{
  //BTX
public:

  enum WorkPhase {
    Calibration = 0,
    Targeting,
    Verification,
    NumPhases,
  };
  enum ImageOrient{
    SLICE_RTIMAGE_PERP      = 0,
    SLICE_RTIMAGE_INPLANE90 = 1,
    SLICE_RTIMAGE_INPLANE   = 2
  };
  enum {  // Events
    LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
    SliceUpdateEvent        = 50002,
  };
  
   // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)
   static const char* WorkPhaseKey[vtkTRProstateBiopsyLogic::NumPhases];

 //ETX

public:
  
  static vtkTRProstateBiopsyLogic *New();  
  vtkTypeRevisionMacro(vtkTRProstateBiopsyLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream&, vtkIndent);

  // The name of the Module---this is used to construct proc invocations
  vtkGetStringMacro (ModuleName);
  vtkSetStringMacro (ModuleName);

  // Parameters related to work phase
  vtkGetMacro ( CurrentPhase,         int );
  vtkGetMacro ( PrevPhase,            int );
  vtkGetMacro ( PhaseTransitionCheck, bool );
  vtkSetMacro ( PhaseTransitionCheck, bool );

  // Methods related to the workflow
  int  SwitchWorkPhase(int);
  int  IsPhaseTransitable(int);
  
  int WorkPhaseStringToID(const char* string);

  // Get transforms for this module
  vtkGetObjectMacro ( LocatorTransform, vtkTransform );
  vtkGetObjectMacro ( LocatorMatrix,    vtkMatrix4x4 );

  // Add a unique realtime volume node for the module
  void AddRealtimeVolumeNode(vtkSlicerApplication* app, const char* name);

 private:
  
  static const int PhaseTransitionMatrix[NumPhases][NumPhases];
  
  int   CurrentPhase;
  int   PrevPhase;
  int   PhaseComplete;
  bool  Connected;
  bool  PhaseTransitionCheck;
  
  vtkMatrix4x4*         LocatorMatrix;
  vtkTransform*         LocatorTransform;

  vtkMRMLVolumeNode     *VolumeNode;
  vtkSlicerVolumesLogic *VolumesLogic;

  bool  Connection;  
  
 protected:
  
  vtkTRProstateBiopsyLogic();
  ~vtkTRProstateBiopsyLogic();
  vtkTRProstateBiopsyLogic(const vtkTRProstateBiopsyLogic&);
  void operator=(const vtkTRProstateBiopsyLogic&);
  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();
  vtkMRMLVolumeNode* AddVolumeNode(vtkSlicerVolumesLogic*, const char*);
  
  vtkCallbackCommand *DataCallbackCommand;

};

#endif


  
