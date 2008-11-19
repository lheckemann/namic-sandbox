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
  // Get the number of TargetingFiducialsLists, because there could be more than one lists, depending on needle type
  vtkGetMacro(NumberOfTargetingFiducialLists, unsigned int);
  
  // Description:
  // Get Calibration Fiducials (used in the wizard steps)
  vtkGetObjectMacro(CalibrationFiducialsList, vtkMRMLFiducialListNode);
  void SetCalibrationFiducialsList(vtkMRMLFiducialListNode *);

  //BTX
  // Description:
  // Get Targeting Fiducials Lists names(used in the wizard steps)
  std::string GetTargetingFiducialsListName(unsigned int index)
    {
    if (index < this->NumberOfTargetingFiducialLists)
      return this->TargetingFiducialsListsNames[index];
    else
      return NULL;
    }; 
  void SetTargetingFiducialsListName(std::string listName, unsigned int index)
    {
    if (index < this->NumberOfTargetingFiducialLists)
      this->TargetingFiducialsListsNames[index] = listName; 
    }; 
  //ETX

  // Description:
  // Get MRML fiducial lists node for targeting fiducials lists
  vtkMRMLFiducialListNode *GetTargetingFiducialsList(unsigned int index)
    {
    if (index < this->NumberOfTargetingFiducialLists)
      return this->TargetingFiducialsListsVector[index];
    else
      return NULL;
    };  
  void SetTargetingFiducialsList(vtkMRMLFiducialListNode *targetList, unsigned int index);
  //BTX
  void AddTargetingFiducialsList(vtkMRMLFiducialListNode *targetList, std::string listName);
  //ETX

protected:
  vtkMRMLTRProstateBiopsyModuleNode();
  ~vtkMRMLTRProstateBiopsyModuleNode();
  vtkMRMLTRProstateBiopsyModuleNode(const vtkMRMLTRProstateBiopsyModuleNode&);
  void operator=(const vtkMRMLTRProstateBiopsyModuleNode&);

  
  // slicer's fiducial lists nodes for calibration and targeting
  vtkMRMLFiducialListNode *CalibrationFiducialsList;
  //BTX
  // there could be more than one targeting fiducials list, depending on needle type
  std::vector<vtkMRMLFiducialListNode *> TargetingFiducialsListsVector;
  unsigned int NumberOfTargetingFiducialLists;
  std::vector<std::string> TargetingFiducialsListsNames;
  //ETX

  void InitializeFiducialListNode();

   
  // common parameters  
  char* VolumeInUse; 

  
  

  vtkMRMLScalarVolumeNode *CalibrationVolumeNode;
  vtkMRMLScalarVolumeNode *TargetingVolumeNode;
  vtkMRMLScalarVolumeNode *VerificationVolumeNode;

};

#endif

