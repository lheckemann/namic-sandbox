/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRML4DVolumeNode.h,v $
  Date:      $Date: 2009/10/15 17:12:29 $
  Version:   $Revision: 1.3 $

  =========================================================================auto=*/

#ifndef __vtkUltrasound4DVolumeNode_h
#define __vtkUltrasound4DVolumeNode_h

#include "vtkUltrasound4DWin32Header.h"

#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkCollection.h"

#include <string>

class VTK_Ultrasound4D_EXPORT vtkMRML4DVolumeNode : public vtkMRMLScalarVolumeNode
{

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------
  static vtkMRML4DVolumeNode *New();
  vtkTypeMacro(vtkMRML4DVolumeNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Create instance of a HybridNavTool node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "TimeSerieContainer";};

  // Disactivate the possibility to apply non-linear transforms
  virtual bool CanApplyNonLinearTransforms() {return 0;};

  //BTX
  vtkSetMacro(SerieID, std::string);
  vtkGetMacro(SerieID, std::string);
  //ETX

  // Manage Volume Collection
  void AddVolume(vtkMRMLScalarVolumeNode* NodeToAdd);
  void InsertVolume(int position, vtkMRMLScalarVolumeNode* NodeToInsert);
  void RemoveVolume(vtkMRMLScalarVolumeNode* NodeToRemove);
  void RemoveVolume(int position);
  int GetNumberOfVolumes();

  vtkMRMLScalarVolumeNode* GetItemAsVolume(int position);


 protected:
  //----------------------------------------------------------------
  // Constructor and destructor
  //----------------------------------------------------------------
  vtkMRML4DVolumeNode();
  ~vtkMRML4DVolumeNode();
  vtkMRML4DVolumeNode(const vtkMRML4DVolumeNode&);
  void operator=(const vtkMRML4DVolumeNode&);

 public:
  //----------------------------------------------------------------
  // Variables
  //----------------------------------------------------------------
  vtkGetObjectMacro(VolumeCollection, vtkCollection);

  vtkCollection *VolumeCollection;

  //BTX
  std::string SerieID;
  //ETX

 private:

};

#endif
