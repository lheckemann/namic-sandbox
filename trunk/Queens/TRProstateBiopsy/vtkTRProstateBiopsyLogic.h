/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTRProstateBiopsyLogic.h $
  Date:      $Date: 2008/11/06 17:56:51 $
  Version:   $Revision: 1.1 $

=========================================================================auto=*/

// .NAME vtkTRProstateBiopsyLogic - slicer logic for transrectal biopsy
// .SECTION Description
// This class manages the logic for the transrectal prostate biopsy module.


#ifndef __vtkTRProstateBiopsyLogic_h
#define __vtkTRProstateBiopsyLogic_h

#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLTRProstateBiopsyModuleNode.h"
#include "vtkSmartPointer.h"

class vtkCallbackCommand;
class vtkSlicerApplication;
class vtkTRProstateBiopsyUSBOpticalEncoder;
class vtkTRProstateBiopsyCalibrationAlgo;

class vtkMRMLModelNode;
class vtkMRMLModelDisplayNode;
class vtkLineSource;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyLogic :
  public vtkSlicerModuleLogic
{
  //BTX
public:

  enum WorkPhase {
    Calibration = 0,
    Segmentation,
    Targeting,
    Verification,
    NumPhases,
  };
  
   // Work phase keywords used in NaviTrack (defined in BRPTPRInterface.h)
   static const char* WorkPhaseKey[vtkTRProstateBiopsyLogic::NumPhases];

 //ETX


public:

  static vtkTRProstateBiopsyLogic *New();  
  vtkTypeMacro(vtkTRProstateBiopsyLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream&, vtkIndent);

   // Description: Get/Set MRML node storing parameter values
  vtkGetObjectMacro (TRProstateBiopsyModuleNode, vtkMRMLTRProstateBiopsyModuleNode);
  void SetAndObserveTRProstateBiopsyModuleNode(vtkMRMLTRProstateBiopsyModuleNode *n) 
    {
    vtkSetAndObserveMRMLNodeMacro( this->TRProstateBiopsyModuleNode, n);
    }

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
  // Add volume to MRML scene and return the MRML node.
  // If volumeType is specified, then the volume is also selected as the current Calibration
  // targeting or verification volume.
  vtkMRMLScalarVolumeNode *AddVolumeToScene(vtkSlicerApplication* app,const char *fileName, VolumeType volumeType=VOL_GENERIC);  

  //BTX
  bool SegmentRegisterMarkers(double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle, std::string &resultDetails);
  void GetCalibrationAxisCenterpoints(vtkPoints *points, int ii) { return this->CalibrationAlgo->GetAxisCenterpoints(points, ii); };
  bool AddTargetToNeedle(std::string needleType, double rasLocation[3], unsigned int & targetDescIndex);
  bool IsTargetReachable(int needleIndex, double rasLocation[3]);
  //ETX
  
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

  void ReadConfigFile();

  void SaveVolumesToExperimentFile(ostream& of);

  //BTX
  std::string GetFoRStrFromVolumeNode(vtkMRMLScalarVolumeNode *volNode);
  //ETX

  vtkImageData* GetCalibMarkerPreProcOutput(int i);

  int ShowCoverage(vtkSlicerApplication *app);

protected:

  // Description:
  // Helper method for loading a volume via the Volume module.
  vtkMRMLScalarVolumeNode *AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName);  
  
  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  // Description:
  // Updates parameters values in MRML node based on logic changes 
  void UpdateMRML();

    // Description:
  // Update logic state when MRML scene chenges
  virtual void ProcessMRMLEvents(vtkObject * caller, unsigned long event, void * callData);
  
  // Description:
  // Set window/level computation, forcing the volume type to scalar (not statistical)
  void SetAutoScaleScalarVolume(vtkMRMLScalarVolumeNode *volumeNode);

  vtkTRProstateBiopsyLogic();
  ~vtkTRProstateBiopsyLogic();
  vtkTRProstateBiopsyLogic(const vtkTRProstateBiopsyLogic&);
  void operator=(const vtkTRProstateBiopsyLogic&);

 private:

  // Description:
  // Set Slicers's 2D view orientations from the image orientation.
  void SetSliceViewFromVolume(vtkSlicerApplication *app, vtkMRMLVolumeNode *volumeNode);

  static const int PhaseTransitionMatrix[NumPhases][NumPhases];

  int CreateCoverageVolume();
  void DeleteCoverageVolume();
  int UpdateCoverageVolumeImage();
  
  int   CurrentPhase;
  int   PrevPhase;
  int   PhaseComplete;
  bool  Connected;
  bool  PhaseTransitionCheck;
  
  // optical encoder related
  vtkTRProstateBiopsyUSBOpticalEncoder *USBEncoder;
  int DeviceRotation;
  int NeedleAngle;  
  bool OpticalEncoderInitialized;
 
  vtkMRMLModelNode* NeedleTrajectoryModelNode;

  //BTX
  vtkSmartPointer<vtkTRProstateBiopsyCalibrationAlgo> CalibrationAlgo;  
  vtkSmartPointer<vtkLineSource> NeedleTrajectoryLine;
  //ETX

  vtkCallbackCommand *DataCallbackCommand;

  vtkMRMLTRProstateBiopsyModuleNode* TRProstateBiopsyModuleNode;  

};

#endif
