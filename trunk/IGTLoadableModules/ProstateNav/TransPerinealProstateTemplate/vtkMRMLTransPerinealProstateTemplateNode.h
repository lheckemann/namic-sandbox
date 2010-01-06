/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTransPerinealProstateRobotNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLTransPerinealProstateTemplateNode_h
#define __vtkMRMLTransPerinealProstateTemplateNode_h

#include "vtkOpenIGTLinkIFWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLRobotNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"

#include "vtkObject.h"
#include "vtkProstateNavWin32Header.h" 

#include "vtkMRMLIGTLConnectorNode.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"

class vtkTransform;
class vtkIGTLToMRMLCoordinate;
class vtkSlicerApplication;

class VTK_PROSTATENAV_EXPORT vtkMRMLTransPerinealProstateTemplateNode : public vtkMRMLRobotNode
{

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLTransPerinealProstateTemplateNode *New();
  vtkTypeMacro(vtkMRMLTransPerinealProstateTemplateNode,vtkMRMLRobotNode);  

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLTransPerinealProstateTemplateNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Update the stored reference to another node in the scene
  void UpdateReferenceID(const char *oldID, const char *newID);

  // Description:
  // Updates this node if it depends on other nodes 
  // when the node is deleted in the scene
  void UpdateReferences();

  // Description:
  // Updates other nodes in the scene depending on this node
  // or updates this node if it depends on other nodes
  virtual void UpdateScene(vtkMRMLScene *);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "TransPerinealProstateTemplate";};

  // method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //----------------------------------------------------------------
  // Commands
  //----------------------------------------------------------------

  // Description:
  // Initialize the robot
  virtual int Init(vtkSlicerApplication* app);

  vtkGetStringMacro(ScannerConnectorNodeID);
  vtkMRMLIGTLConnectorNode* GetScannerConnectorNode();
  void SetAndObserveScannerConnectorNodeID(const char *nodeID);

  vtkGetStringMacro(ZFrameModelNodeID);
  vtkMRMLModelNode* GetZFrameModelNode();
  void SetAndObserveZFrameModelNodeID(const char *nodeID);

  vtkGetStringMacro(WorkspaceModelNodeID);
  vtkMRMLModelNode* GetWorkspaceModelNode();
  void SetAndObserveWorkspaceModelNodeID(const char *nodeID);

  vtkGetStringMacro(ZFrameTransformNodeID);
  vtkMRMLLinearTransformNode* GetZFrameTransformNode();
  void SetAndObserveZFrameTransformNodeID(const char *nodeID);
  
  virtual void SwitchStep(const char *stepName);

  virtual int OnTimer();

  virtual bool FindTargetingParams(vtkProstateNavTargetDescriptor *targetDesc);
  //BTX
  virtual std::string GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc);
  //ETX

  virtual const char* GetCalibrationObjectModelId() { return GetZFrameModelNodeID(); };
  virtual const char* GetCalibrationObjectTransformId() { return GetZFrameTransformNodeID(); };
  virtual const char* GetWorkspaceObjectModelId() { return GetWorkspaceModelNodeID(); };

  virtual int PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode);

  virtual const char* GetWorkflowStepsString()
    {return "SetUp ZFrameCalibration PointTargeting PointVerification TransperinealProstateTemplateManualControl"; };

 protected:

  vtkMRMLTransPerinealProstateTemplateNode();
  ~vtkMRMLTransPerinealProstateTemplateNode();
  vtkMRMLTransPerinealProstateTemplateNode(const vtkMRMLTransPerinealProstateTemplateNode&);
  void operator=(const vtkMRMLTransPerinealProstateTemplateNode&);

  ///////////

  int  SendZFrame();

  vtkGetMacro ( Connection,              bool );
  vtkGetMacro ( ScannerWorkPhase,         int );

  const char* AddWorkspaceModel(const char* nodeName);
  const char* AddZFrameModel(const char* nodeName);

  // NOTE: Since we couldn't update ScannerStatusLabelDisp and RobotStatusLabelDisp
  // directly from ProcessMRMLEvent(), we added following flags to update those GUI
  // widgets in the timer handler.
  // if flag == 0, the widget does not need to be updated()
  // if flag == 1, the connector has connected to the target
  // if flag == 2, the connector has disconnected from the target
  int ScannerConnectedFlag;

private:

  // Node references

  vtkSetReferenceStringMacro(ScannerConnectorNodeID);
  char *ScannerConnectorNodeID;
  vtkMRMLIGTLConnectorNode* ScannerConnectorNode;

  vtkSetReferenceStringMacro(ZFrameModelNodeID);
  char *ZFrameModelNodeID;
  vtkMRMLModelNode* ZFrameModelNode;

  vtkSetReferenceStringMacro(ZFrameTransformNodeID); 
  char *ZFrameTransformNodeID;
  vtkMRMLLinearTransformNode* ZFrameTransformNode;  

  vtkSetReferenceStringMacro(WorkspaceModelNodeID);
  char *WorkspaceModelNodeID;
  vtkMRMLModelNode* WorkspaceModelNode;

  // Other member variables

  vtkIGTLToMRMLCoordinate* CoordinateConverter;

  bool  Connection;  
  int   ScannerWorkPhase;
  
};

#endif

