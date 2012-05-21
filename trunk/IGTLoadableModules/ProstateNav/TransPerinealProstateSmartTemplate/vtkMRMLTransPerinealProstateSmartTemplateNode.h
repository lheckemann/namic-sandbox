/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTransPerinealProstateRobotNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLTransPerinealProstateSmartTemplateNode_h
#define __vtkMRMLTransPerinealProstateSmartTemplateNode_h

#include "vtkOpenIGTLinkIFWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLRobotNode.h"
#include "vtkMRMLStorageNode.h"

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

class VTK_PROSTATENAV_EXPORT vtkMRMLTransPerinealProstateSmartTemplateNode : public vtkMRMLRobotNode
{

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLTransPerinealProstateSmartTemplateNode *New();
  vtkTypeMacro(vtkMRMLTransPerinealProstateSmartTemplateNode,vtkMRMLRobotNode);  

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLTransPerinealProstateSmartTemplateNode* CreateNodeInstance();

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
    {return "TransPerinealProstateSmartTemplate";};

  // method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //----------------------------------------------------------------
  // Commands
  //----------------------------------------------------------------

  // Description:
  // Initialize the robot
  virtual int Init(vtkSlicerApplication* app, const char* moduleShareDir);

  vtkGetStringMacro(ScannerConnectorNodeID);
  vtkMRMLIGTLConnectorNode* GetScannerConnectorNode();
  void SetAndObserveScannerConnectorNodeID(const char *nodeID);

  vtkGetStringMacro(RobotConnectorNodeID);
  vtkMRMLIGTLConnectorNode* GetRobotConnectorNode();
  void SetAndObserveRobotConnectorNodeID(const char *nodeID);

  vtkGetStringMacro(WorkspaceModelNodeID);
  vtkMRMLModelNode* GetWorkspaceModelNode();
  void SetAndObserveWorkspaceModelNodeID(const char *nodeID);

  vtkGetStringMacro(ZFrameModelNodeID);
  vtkMRMLModelNode* GetZFrameModelNode();
  void SetAndObserveZFrameModelNodeID(const char *nodeID);

  vtkGetStringMacro(ZFrameTransformNodeID);
  vtkMRMLLinearTransformNode* GetZFrameTransformNode();
  void SetAndObserveZFrameTransformNodeID(const char *nodeID);

  vtkGetStringMacro(TemplateModelNodeID);
  vtkMRMLModelNode* GetTemplateModelNode();
  void SetAndObserveTemplateModelNodeID(const char *nodeID);

  vtkGetStringMacro(TargetModelNodeID);
  vtkMRMLModelNode* GetTargetModelNode();
  void SetAndObserveTargetModelNodeID(const char *nodeID);

  vtkGetStringMacro(CurrentTransformNodeID);
  vtkMRMLLinearTransformNode* GetCurrentTransformNode();
  void SetAndObserveCurrentTransformNodeID(const char *nodeID);

  vtkGetStringMacro(CurrentModelNodeID);
  vtkMRMLModelNode* GetCurrentModelNode();
  void SetAndObserveCurrentModelNodeID(const char *nodeID);

  void SetTargetName(const char *name);
  void SetTargetPosition(float xyz[3]);
  void GetTargetPosition(float xyz[3]);
  void SetTargetingOffset(float xyz[3]);
  void GetTargetingOffset(float xyz[3]);

  // Description:
  // Update TARGET transform.
  // If transformNodeId=NULL, it updates the TARGET transform, which is registered to
  // OpenIGTLink connector node as outgoing node.
  virtual int  MoveTo(const char *transformNodeId);
  
  virtual void SwitchStep(const char *stepName);

  virtual int OnTimer();

  //BTX
  virtual std::string GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle);
  //ETX

  virtual const char* GetCalibrationObjectModelId() { return GetZFrameModelNodeID(); };
  virtual const char* GetCalibrationObjectTransformId() { return GetZFrameTransformNodeID(); };
  virtual const char* GetWorkspaceObjectModelId() { return GetWorkspaceModelNodeID(); };

  virtual int PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode);
  virtual int PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode, int param1, int param2);

  virtual const char* GetWorkflowStepsString()
    {return "SetUp ZFrameCalibration SmartTemplateTargeting PointVerification"; };

  //----------------------------------------------------------------------------
  // Funcitons to obtain information about needle guiding template
  
  // Description:
  // Get position and orientation of grid hole (i, j)
  int GetHoleTransform(int i, int j, vtkMatrix4x4* matrix); 

  // Description:
  // Get needle tip position and orientation, when grid hole (i, j) is used.
  int GetNeedleTransform(int i, int j, double length, vtkMatrix4x4* matrix);
  

 protected:

  vtkMRMLTransPerinealProstateSmartTemplateNode();
  virtual ~vtkMRMLTransPerinealProstateSmartTemplateNode();
  vtkMRMLTransPerinealProstateSmartTemplateNode(const vtkMRMLTransPerinealProstateSmartTemplateNode&);
  void operator=(const vtkMRMLTransPerinealProstateSmartTemplateNode&);

  ///////////

  int  SendZFrame();

  vtkGetMacro ( Connection,              bool );
  vtkGetMacro ( ScannerWorkPhase,         int );

  const char* AddWorkspaceModel(const char* nodeName);
  const char* AddZFrameModel(const char* nodeName);
  const char* AddTemplateModel(const char* nodeName);
  const char* AddNeedleModel(const char* nodeName, double length, double diameter,
                             double color[3], double opacity);

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

  vtkSetReferenceStringMacro(RobotConnectorNodeID);
  char *RobotConnectorNodeID;
  vtkMRMLIGTLConnectorNode* RobotConnectorNode;

  vtkSetReferenceStringMacro(ZFrameModelNodeID);
  char *ZFrameModelNodeID;
  vtkMRMLModelNode* ZFrameModelNode;

  vtkSetReferenceStringMacro(ZFrameTransformNodeID); 
  char *ZFrameTransformNodeID;
  vtkMRMLLinearTransformNode* ZFrameTransformNode;  

  vtkSetReferenceStringMacro(TemplateModelNodeID);
  char *TemplateModelNodeID;
  vtkMRMLModelNode* TemplateModelNode;

  vtkSetReferenceStringMacro(WorkspaceModelNodeID);
  char *WorkspaceModelNodeID;
  vtkMRMLModelNode* WorkspaceModelNode;

  vtkSetReferenceStringMacro(TargetModelNodeID);
  char *TargetModelNodeID;
  vtkMRMLModelNode* TargetModelNode;

  vtkSetReferenceStringMacro(CurrentTransformNodeID); 
  char *CurrentTransformNodeID;
  vtkMRMLLinearTransformNode* CurrentTransformNode;

  vtkSetReferenceStringMacro(CurrentModelNodeID);
  char *CurrentModelNodeID;
  vtkMRMLModelNode* CurrentModelNode;

  // Other member variables

  vtkIGTLToMRMLCoordinate* CoordinateConverter;

  bool  Connection;  
  int   ScannerWorkPhase;

  //BTX
  std::string TargetName;
  //ETX
  float  TargetPosition[3];
  float  TargetingOffset[3];


  // Template for needle guidance
  double TemplateGridPitch[2];   // Template pitch in mm (x and y in the figure bellow)
  double TemplateNumGrids[2];    // Number of holes in each direction (m and n in the figure bellow)
  double TemplateOffset[3];      // Offset of the first hole ((0,0) in the figure bellow)
                                 // from the Z-frame center (mm) (in the Z-frame coordinate system)

  //
  // Figure: needle guiding template parameters
  //
  //   Template (holes are indicated as 'o')
  //
  //   +---------------------------------------+
  //   |         x                             |
  //   |     |<----->|                         |
  //   |  -- o       o       o   ....    o     |
  //   |  ^ (0,0)   (0,1)   (0,2)       (0,n-1)|
  //   | y|                                    |
  //   |  v                                    |
  //   |  -- o       o       o   ....    o     |
  //   |    (1,0)   (1,1)   (1,2)       (1,n-1)|
  //   |                                       |
  //   |                                       |
  //   |     o       o       o   ....    o     |
  //   |    (2,0)   (2,1)   (2,2)       (2,n-1)|
  //   |     .       .       .   .       .     |
  //   |     .       .       .     .     .     |
  //   |     .       .       .       .   .     |
  //   |                                       |
  //   |     o       o       o   ....    o     |
  //   |    (m-1,0) (m-1,1) (m-1,2)   (m-1,n-1)|
  //   |                                       |
  //   +---------------------------------------+ 
  //
  // NOTE: index (i, j) starts from (0, 0) for coding. When it displayed
  //  for clinicians, (i+1, j+1) is used. 
  //
  
};

#endif

