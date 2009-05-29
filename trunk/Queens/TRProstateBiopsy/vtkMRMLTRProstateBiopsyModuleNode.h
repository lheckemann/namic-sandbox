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
#include "vtkTRProstateBiopsyCalibrationAlgo.h"

class vtkImageData;
class vtkMRMLLinearTransformNode;
class vtkMRMLFiducialListNode;
class vtkMRMLScalarVolumeNode;
//class vtkTRProstateBiopsyTargetDescriptor;
#include "vtkTRProstateBiopsyTargetDescriptor.h"

enum VolumeType
{
  VOL_GENERIC, // any other than the specific volumes 
  VOL_CALIBRATION,
  VOL_TARGETING,
  VOL_VERIFICATION
};

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
  virtual const char* GetNodeTagName() {return "TRProstateBiopsyModule";};

 
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

  //BTX
  // Description:
  // Set/Get the number of needle types and TargetingFiducialsLists, because there could be more than one lists, depending on needle type
  int GetNumberOfNeedles(){ return this->NeedlesVector.size();};
  //ETX

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
  vtkGetObjectMacro(CalibrationFiducialsListNode, vtkMRMLFiducialListNode);

  bool AddTargetToFiducialList(double targetRAS[3], unsigned int fiducialListIndex, unsigned int targetNr, int & fiducialIndex);
  bool GetTargetFromFiducialList(int fiducialListIndex, int fiducialIndex, double &r, double &a, double &s);

  void SetFiducialColor(int fiducialListIndex, int fiducialIndex, bool selected);
  // Description:
  void GetCalibrationMarker(unsigned int markerNr, double &r, double &a, double &s);
  void SetCalibrationMarker(unsigned int markerNr, double markerRAS[3]);
  void RemoveAllCalibrationMarkers();

  void ResetCalibrationData();
  const TRProstateBiopsyCalibrationData& GetCalibrationData() { return this->CalibrationData; }
  void SetCalibrationData(const TRProstateBiopsyCalibrationData& calibData) { this->CalibrationData=calibData; }

  //BTX
  // Description:
  // Get Targeting Fiducials Lists names(used in the wizard steps)
  std::string GetTargetingFiducialsListName(unsigned int index)
    {
    if (index < this->NeedlesVector.size())
      return this->NeedlesVector[index].NeedleType;
    else
      return NULL;
    }; 
  //ETX

  // Description:
  // Get MRML fiducial lists node for targeting fiducials lists
  vtkMRMLFiducialListNode *GetTargetingFiducialsList(unsigned int index)
    {
    if (index < this->NeedlesVector.size())
      return this->NeedlesVector[index].TargetingFiducialsList;
    else
      return NULL;
    };  
  
  unsigned int AddTargetDescriptor(vtkTRProstateBiopsyTargetDescriptor *target);
  vtkTRProstateBiopsyTargetDescriptor *GetTargetDescriptorAtIndex(unsigned int index);
  int GetTotalNumberOfTargets() { return this->TargetDescriptorsVector.size();};
  vtkGetMacro(CurrentTargetIndex, int);
  int SetCurrentTargetIndex(int index);

  int GetNumberOfOpenVolumes(){return this->VolumesList.size();};
  vtkMRMLScalarVolumeNode *GetVolumeNodeAtIndex(int index);
  const char *GetDiskLocationOfVolumeAtIndex(int index);
  const char *GetTypeOfVolumeAtIndex(int index);
  bool IsVolumeAtIndexActive(int index);
  void AddVolumeInformationToList(vtkMRMLScalarVolumeNode *volNode, const char *diskLocation, char *type); 

  bool GetRobotManipulatorTransform(vtkMatrix4x4* transform);

protected:
  vtkMRMLTRProstateBiopsyModuleNode();
  ~vtkMRMLTRProstateBiopsyModuleNode();
  vtkMRMLTRProstateBiopsyModuleNode(const vtkMRMLTRProstateBiopsyModuleNode&);
  void operator=(const vtkMRMLTRProstateBiopsyModuleNode&);

  TRProstateBiopsyCalibrationData CalibrationData;

  // slicer's fiducial lists nodes for calibration and targeting
  vtkMRMLFiducialListNode *CalibrationFiducialsListNode; // set using vtkSetMRMLNodeMacro
  //BTX
  // there could be more than one targeting fiducials list, depending on needle type
  std::vector<vtkTRProstateBiopsyTargetDescriptor*> TargetDescriptorsVector;

  int CurrentTargetIndex;

  typedef struct 
    {
    std::string NeedleType;
    float NeedleDepth;
    float NeedleOvershoot;
    std::string Description;
    unsigned int UniqueIdentifier;
    vtkMRMLFiducialListNode* TargetingFiducialsList;
    } NeedleDescriptorStruct; // TargetingFiducialsListsNames;

  std::vector<NeedleDescriptorStruct> NeedlesVector;

  //keep track of all the volumes that were opened, maintain a list
  typedef struct{
    std::string Type;
    std::string DiskLocation;
    bool Active;
    vtkMRMLScalarVolumeNode *VolumeNode;
  } VolumeInformationStruct;
  std::vector<VolumeInformationStruct> VolumesList;

  //ETX

  vtkMRMLScalarVolumeNode *CalibrationVolumeNode; // set using vtkSetMRMLNodeMacro
  vtkMRMLScalarVolumeNode *TargetingVolumeNode; // set using vtkSetMRMLNodeMacro
  vtkMRMLScalarVolumeNode *VerificationVolumeNode; // set using vtkSetMRMLNodeMacro

  // common parameters  
  char* VolumeInUse; 
  char* SegmentationOutputVolumeRef;
  char* SegmentationInputVolumeRef;
  char* SegmentationSeedVolumeRef;
  char* ExperimentSavePathInConfigFile;

};

#endif

