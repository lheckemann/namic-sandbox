/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTRProstateBiopsyModuleNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLTRProstateBiopsyModuleNode_h
#define __vtkMRMLTRProstateBiopsyModuleNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include <vector>

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"

#include "vtkTRProstateBiopsyModule.h"

class vtkImageData;
class vtkMRMLLinearTransformNode;
class vtkMRMLFiducialListNode;
class vtkMRMLScalarVolumeNode;
class vtkTRProstateBiopsyTargetDescriptor;


class VTK_TRPROSTATEBIOPSY_EXPORT vtkMRMLTRProstateBiopsyModuleNode : public vtkMRMLNode
{
  public:
  static vtkMRMLTRProstateBiopsyModuleNode *New();
  vtkTypeMacro(vtkMRMLTRProstateBiopsyModuleNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create instance of a PS node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "PS";};

 
  // common:

  // Description:
  // Get/Set volume in use
  vtkGetStringMacro(VolumeInUse);
  vtkSetStringMacro(VolumeInUse);

   // Description:
  // Get/Set output volume MRML Id
  vtkGetStringMacro(SegmentationOutputVolumeRef);
  vtkSetStringMacro(SegmentationOutputVolumeRef);

  // Description:
  // Get/Set output volume MRML Id
  vtkGetStringMacro(SegmentationInputVolumeRef);
  vtkSetStringMacro(SegmentationInputVolumeRef);

  // Description:
  // Get/Set output volume MRML Id
  vtkGetStringMacro(SegmentationSeedVolumeRef);
  vtkSetStringMacro(SegmentationSeedVolumeRef);

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);


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
  // Set/Get the number of needle types and TargetingFiducialsLists, because there could be more than one lists, depending on needle type
  vtkGetMacro(NumberOfNeedles, unsigned int);
  vtkSetMacro(NumberOfNeedles, unsigned int);

  //BTX
  // Description:
  // get/set methods for storing needle information
  void SetNeedleType(unsigned int needleIndex, std::string type);
  std::string GetNeedleType(unsigned int needleIndex);
  //ETX

  //BTX
  // Description:
  // get/set methods for storing needle information
  void SetNeedleDescription(unsigned int needleIndex, std::string desc);
  std::string GetNeedleDescription(unsigned int needleIndex);
  //ETX


  // Description:
  // get/set methods for storing needle information
  void SetNeedleDepth(unsigned int needleIndex, float depth);
  float GetNeedleDepth(unsigned int needleIndex);

  // Description:
  // get/set methods for storing needle information
  void SetNeedleOvershoot(unsigned int needleIndex, float overshoot);
  float GetNeedleOvershoot(unsigned int needleIndex);

  // Description:
  // get/set methods for storing needle information
  void SetNeedleUID(unsigned int needleIndex, unsigned int uid);
  unsigned int GetNeedleUID(unsigned int needleIndex);

  // Description:
  // get/set experiment save path as read from config file
  vtkSetStringMacro(ExperimentSavePathInConfigFile);
  vtkGetStringMacro(ExperimentSavePathInConfigFile);

  // Description:
  // Get Calibration Fiducials (used in the wizard steps)
  vtkGetObjectMacro(CalibrationFiducialsList, vtkMRMLFiducialListNode);
  void SetCalibrationFiducialsList(vtkMRMLFiducialListNode *);

  bool AddTargetToFiducialList(double targetRAS[3], unsigned int fiducialListIndex, unsigned int targetNr, int & fiducialIndex);
  bool GetTargetFromFiducialList(int fiducialListIndex, int fiducialIndex, double &r, double &a, double &s);

  void SetFiducialColor(int fiducialListIndex, int fiducialIndex, bool selected);
  // Description:
  void GetCalibrationMarker(unsigned int markerNr, double &r, double &a, double &s);
  void SetCalibrationMarker(unsigned int markerNr, double markerRAS[3]);

  // Description 
  // storing registration results
  // angle alpha between two axes in degrees
  vtkSetMacro(AxesAngleDegrees, double);
  vtkGetMacro(AxesAngleDegrees, double);

  // Description 
  // storing registration results
  // registration angle in degrees
  vtkSetMacro(RobotRegistrationAngleDegrees, double);
  vtkGetMacro(RobotRegistrationAngleDegrees, double);

  // Description 
  // storing registration results
  // axes distance
  vtkSetMacro(AxesDistance, double);
  vtkGetMacro(AxesDistance, double);

  // Description 
  // storing registration results
  // I1
  vtkSetVector3Macro(I1, double);
  vtkGetVector3Macro(I1, double);

  // Description 
  // storing registration results
  // I1
  vtkSetVector3Macro(I2, double);
  vtkGetVector3Macro(I2, double);

  // Description 
  // storing registration results
  // I1
  vtkSetVector3Macro(v1, double);
  vtkGetVector3Macro(v1, double);

  // Description 
  // storing registration results
  // I1
  vtkSetVector3Macro(v2, double);
  vtkGetVector3Macro(v2, double);

  // Description 
  // storing registration results
  // axes distance
  vtkSetMacro(FiducialSegmentationRegistrationComplete, bool);
  vtkGetMacro(FiducialSegmentationRegistrationComplete, bool);

  //BTX
  // Description:
  // Get Targeting Fiducials Lists names(used in the wizard steps)
  std::string GetTargetingFiducialsListName(unsigned int index)
    {
    if (index < this->NumberOfNeedles)
      return this->TargetingFiducialsListsNames[index];
    else
      return NULL;
    }; 
  void SetTargetingFiducialsListName(std::string listName, unsigned int index)
    {
    if (index < this->NumberOfNeedles)
      this->TargetingFiducialsListsNames[index] = listName; 
    }; 
  //ETX

  // Description:
  // Get MRML fiducial lists node for targeting fiducials lists
  vtkMRMLFiducialListNode *GetTargetingFiducialsList(unsigned int index)
    {
    if (index < this->NumberOfNeedles)
      return this->TargetingFiducialsListsVector[index];
    else
      return NULL;
    };  
  void SetTargetingFiducialsList(vtkMRMLFiducialListNode *targetList, unsigned int index);
  
  unsigned int AddTargetDescriptor(vtkTRProstateBiopsyTargetDescriptor *target);
  vtkTRProstateBiopsyTargetDescriptor *GetTargetDescriptorAtIndex(unsigned int index);
  int GetTotalNumberOfTargets() { return this->TargetDescriptorsVector.size();};

  void SetupNeedlesList();
  void SetupTargetingFiducialsList();
  


  int GetNumberOfOpenVolumes(){return this->VolumesList.size();};
  vtkMRMLScalarVolumeNode *GetVolumeNodeAtIndex(int index);
  char *GetDiskLocationOfVolumeAtIndex(int index);
  char *GetTypeOfVolumeAtIndex(int index);
  bool IsVolumeAtIndexActive(int index);
  void AddVolumeInformationToList(vtkMRMLScalarVolumeNode *volNode, const char *diskLocation, char *type);

protected:
  vtkMRMLTRProstateBiopsyModuleNode();
  ~vtkMRMLTRProstateBiopsyModuleNode();
  vtkMRMLTRProstateBiopsyModuleNode(const vtkMRMLTRProstateBiopsyModuleNode&);
  void operator=(const vtkMRMLTRProstateBiopsyModuleNode&);

  // registration results
  double AxesDistance;
  double RobotRegistrationAngleDegrees;
  double AxesAngleDegrees;
  double I1[3];
  double I2[3]; 
  double v1[3];
  double v2[3];
  bool FiducialSegmentationRegistrationComplete;
  // slicer's fiducial lists nodes for calibration and targeting
  vtkMRMLFiducialListNode *CalibrationFiducialsList;
  //BTX
  // there could be more than one targeting fiducials list, depending on needle type
  std::vector<vtkMRMLFiducialListNode *> TargetingFiducialsListsVector;
  std::vector<vtkTRProstateBiopsyTargetDescriptor *> TargetDescriptorsVector;
  //unsigned int NumberOfTargetingFiducialLists; this should be equal to number of needle types
  std::vector<std::string> TargetingFiducialsListsNames;
  //keep track of all the volumes that were opened, maintain a list
  typedef struct{
      char *Type;
      char *DiskLocation;
      bool Active;
      vtkMRMLScalarVolumeNode *VolumeNode;
  }VolumeInformationStruct;
  std::vector<VolumeInformationStruct *> VolumesList;
  //ETX

  void InitializeFiducialListNode();

  //BTX
  // there could be more than one needles
  typedef struct 
    {
    std::string NeedleType;
    float NeedleDepth;
    float NeedleOvershoot;
    std::string Description;
    unsigned int UniqueIdentifier;
    }NeedleDescriptorStruct;

  std::vector<NeedleDescriptorStruct *> NeedlesVector;
  unsigned int NumberOfNeedles; 
  //ETX
   
  // common parameters  
  char* VolumeInUse; 
  char* SegmentationOutputVolumeRef;
  char* SegmentationInputVolumeRef;
  char* SegmentationSeedVolumeRef;
  char* ExperimentSavePathInConfigFile;
  vtkMRMLScalarVolumeNode *CalibrationVolumeNode;
  vtkMRMLScalarVolumeNode *TargetingVolumeNode;
  vtkMRMLScalarVolumeNode *VerificationVolumeNode;

};

#endif

