/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

// .NAME vtkTRProstateBiopsyLogic - slicer logic for transrectal biopsy
// .SECTION Description
// This class manages the logic for the transrectal prostate biopsy module.


#ifndef __vtkTRProstateBiopsyLogic_h
#define __vtkTRProstateBiopsyLogic_h

#include "vtkTRProstateBiopsyWin32Header.h"
#include "vtkSlicerModuleLogic.h"

class vtkCallbackCommand;
class vtkSlicerApplication;
class vtkSlicerBaseLogic;
class vtkMRMLFiducialListNode;
class vtkScalarVolumeNode;

class vtkTRProstateBiopsyUSBOpticalEncoder;

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
  
   // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)
   static const char* WorkPhaseKey[vtkTRProstateBiopsyLogic::NumPhases];

 //ETX

public:
  
  static vtkTRProstateBiopsyLogic *New();  
  vtkTypeRevisionMacro(vtkTRProstateBiopsyLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream&, vtkIndent);

  // Description:
  // The name of the Module.
  vtkGetStringMacro(ModuleName);
  vtkSetStringMacro(ModuleName);

  // Description:
  // Get parameters related to work phase.
  vtkGetMacro(CurrentPhase,         int);
  vtkGetMacro(PrevPhase,            int);
  vtkGetMacro(PhaseTransitionCheck, bool);
  vtkSetMacro(PhaseTransitionCheck, bool);

  // Description:
  // Methods related to the workflow
  int  SwitchWorkPhase(int);
  int  IsPhaseTransitable(int);
  int WorkPhaseStringToID(const char* string);

  // Description:
  // Get Calibration Fiducials (used in the wizard steps)
  vtkGetStringMacro(CalibrationFiducialListNodeID);
  vtkSetStringMacro(CalibrationFiducialListNodeID);
  vtkGetObjectMacro(CalibrationFiducialListNode, vtkMRMLFiducialListNode);
  void SetCalibrationFiducialListNode(vtkMRMLFiducialListNode *);

  // Description:
  // Get Target Fiducials (used in the wizard steps)
  vtkGetStringMacro(TargetFiducialListNodeID);
  vtkSetStringMacro(TargetFiducialListNodeID);
  vtkGetObjectMacro(TargetFiducialListNode, vtkMRMLFiducialListNode);
  void SetTargetFiducialListNode(vtkMRMLFiducialListNode *);

  // Description:
  // Get Calibration Volume.
  vtkGetObjectMacro(CalibrationVolumeNode, vtkMRMLScalarVolumeNode);
  void SetCalibrationVolumeNode(vtkMRMLScalarVolumeNode *);

  // Description:
  // Get Targeting Volume.
  vtkGetObjectMacro(TargetingVolumeNode, vtkMRMLScalarVolumeNode);
  void SetTargetingVolumeNode(vtkMRMLScalarVolumeNode *);

  // Description:
  // Get Verification Volume.
  vtkGetObjectMacro(VerificationVolumeNode, vtkMRMLScalarVolumeNode);
  void SetVerificationVolumeNode(vtkMRMLScalarVolumeNode *);


  // State of the MRML slice nodes for calibration.
  //vtkGetStringMacro(CalibrationSliceNodeXML);
  //vtkSetStringMacro(CalibrationSliceNodeXML);

  // Description:
  // Load the calibration volume and return the MRML node.
  vtkMRMLScalarVolumeNode *AddCalibrationVolume(vtkSlicerApplication* app,
                                                const char *fileName);

  // Description:
  // Load the targeting volume and return the MRML node.
  vtkMRMLScalarVolumeNode *AddTargetingVolume(vtkSlicerApplication* app,
                                              const char *fileName);

  // Description:
  // Load the verification volume and return the MRML node.
  vtkMRMLScalarVolumeNode *AddVerificationVolume(vtkSlicerApplication* app,
                                                 const char *fileName);

  // Description:
  // Helper method for loading a volume via the Volume module.
  vtkMRMLScalarVolumeNode *AddArchetypeVolume(vtkSlicerApplication* app,
                                              const char* fileName,
                                              const char *volumeName);

  // Description:
  // Set Slicers's 2D view orientations from the image orientation.
  void SetSliceViewFromVolume(vtkSlicerApplication *app,
                              vtkMRMLVolumeNode *volumeNode);

  // Description:
  // Get USB optical encoder
  vtkGetObjectMacro(USBEncoder, vtkTRProstateBiopsyUSBOpticalEncoder);

  vtkGetMacro(DeviceRotation, int);
  vtkSetMacro(DeviceRotation, int);

  vtkGetMacro(NeedleAngle, int);
  vtkSetMacro(NeedleAngle, int);

  void InitializeOpticalEncoder();
  void ResetOpticalEncoder();

  bool IsOpticalEncoderInitialized(){return this->OpticalEncoderInitialized;};


 private:
  
  static const int PhaseTransitionMatrix[NumPhases][NumPhases];
  
  int   CurrentPhase;
  int   PrevPhase;
  int   PhaseComplete;
  bool  Connected;
  bool  PhaseTransitionCheck;
  
  char *CalibrationFiducialListNodeID;
  vtkMRMLFiducialListNode *CalibrationFiducialListNode;
  char *TargetFiducialListNodeID;
  vtkMRMLFiducialListNode *TargetFiducialListNode;

  vtkMRMLScalarVolumeNode *CalibrationVolumeNode;
  vtkMRMLScalarVolumeNode *TargetingVolumeNode;
  vtkMRMLScalarVolumeNode *VerificationVolumeNode;

  //char *CalibrationSliceNodeXML;

  bool  Connection;  
  
  // optical encoder related
  vtkTRProstateBiopsyUSBOpticalEncoder *USBEncoder;
  int DeviceRotation;
  int NeedleAngle;  
  bool OpticalEncoderInitialized;

  
  

 protected:
  
  vtkTRProstateBiopsyLogic();
  ~vtkTRProstateBiopsyLogic();
  vtkTRProstateBiopsyLogic(const vtkTRProstateBiopsyLogic&);
  void operator=(const vtkTRProstateBiopsyLogic&);
  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();
  
  vtkCallbackCommand *DataCallbackCommand;

};

#endif


  
