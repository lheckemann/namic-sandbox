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


#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLScene.h"

#include "vtkTRProstateBiopsyModule.h"
#include "vtkMRMLTRProstateBiopsyModuleNode.h"

#include <vector>
#include "itkPoint.h"

class vtkCallbackCommand;
class vtkSlicerApplication;
class vtkSlicerBaseLogic;
class vtkMRMLFiducialListNode;
class vtkScalarVolumeNode;

class vtkTRProstateBiopsyUSBOpticalEncoder;
class vtkTRProstateBiopsyTargetDescriptor;

//BTX
#define TRPB_CALIBRATION "CALIBRATION"
#define TRPB_TARGETING "TARGETING"
#define TRPB_SEGMENTATION "SEGMENTATION"
#define TRPB_VERIFICATION "VERIFICATION"
//ETX

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
  vtkTypeRevisionMacro(vtkTRProstateBiopsyLogic, vtkSlicerModuleLogic);
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
  // Add volume to MRML scene and return the MRML node.
  vtkMRMLScalarVolumeNode *AddVolumeToScene(vtkSlicerApplication* app,
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
  
  bool DoubleEqual(double val1, double val2);

  void SegmentRegisterMarkers(double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle);
  void SegmentAxis(int nAxis, double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle, double P1[3], double v1[3]);
  bool SegmentCircle(float originToBeChanged[3],const double normal[3],  double thresh, double fidDims[3], double radius, bool bUseRadius, double initialAngle);
  bool CalculateCircleCenter(vtkImageData *inData, unsigned int *tempStorage, int tempStorageSize, double nThersholdVal, double nRadius, double *gx, double *gy, double *gz, int nVotedNeeded, bool lDebug);
  void RemoveOutliners(double P_[3], double v_[3], double def1[3],double def2[3]);
  void FindProbe(double P1[3], double P2[3], double v1[3], double v2[3], double I1[3], double I2[3], double initialAngle);
  //BTX
  void Linefinder(double P_[3], double v_[3], std::vector<itk::Point<double,3> > CoordVector);
  //ETX

  bool FindTargetingParams(vtkTRProstateBiopsyTargetDescriptor *target);
  bool RotatePoint(double H_before[3], double rotation_rad, double alpha_rad, double mainaxis[3], double I[3], /*out*/double H_after[3]);

  //BTX
  bool AddTargetToNeedle(std::string needleType, double rasLocation[3], unsigned int & targetDescIndex);
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

 private:
  
  static const int PhaseTransitionMatrix[NumPhases][NumPhases];
  
  int   CurrentPhase;
  int   PrevPhase;
  int   PhaseComplete;
  bool  Connected;
  bool  PhaseTransitionCheck;
  
  
  bool  Connection;  
  
  // optical encoder related
  vtkTRProstateBiopsyUSBOpticalEncoder *USBEncoder;
  int DeviceRotation;
  int NeedleAngle;  
  bool OpticalEncoderInitialized;

  //BTX
  //typedef itk::Point<double, 3> PointType;
  std::vector<itk::Point<double,3> > CoordinatesVector;
  //ETX
  

 protected:
  
  vtkTRProstateBiopsyLogic();
  ~vtkTRProstateBiopsyLogic();
  vtkTRProstateBiopsyLogic(const vtkTRProstateBiopsyLogic&);
  void operator=(const vtkTRProstateBiopsyLogic&);
  
  vtkMRMLTRProstateBiopsyModuleNode* TRProstateBiopsyModuleNode;

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

   // Description:
  // Updates parameters values in MRML node based on logic changes 
  void UpdateMRML();
  
  vtkCallbackCommand *DataCallbackCommand;

};

#endif


  
