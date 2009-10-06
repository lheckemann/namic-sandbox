/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLProstateNavManagerNode_h
#define __vtkMRMLProstateNavManagerNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkObject.h"
#include "vtkStdString.h"
#include "vtkProstateNavWin32Header.h" 

#include "vtkMRMLFiducialListNode.h"

class vtkProstateNavStep;
class vtkStringArray;
class vtkMRMLRobotNode;

class VTK_PROSTATENAV_EXPORT vtkMRMLProstateNavManagerNode : public vtkMRMLNode
{

 public:

  //----------------------------------------------------------------
  // Constants Definitions
  //----------------------------------------------------------------

  //BTX
  //ETX

 public:

  //----------------------------------------------------------------
  // Get and Set Macros
  //----------------------------------------------------------------
  //vtkGetObjectMacro ( TargetPlanList, vtkMRMLFiducialListNode );
  //vtkGetObjectMacro ( TargetCompletedList, vtkMRMLFiducialListNode );

  //void SetAndObserveRobotNode(vtkMRMLRobotNode* ptr);
  //vtkGetObjectMacro ( RobotNode, vtkMRMLRobotNode );

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLProstateNavManagerNode *New();
  vtkTypeMacro(vtkMRMLProstateNavManagerNode,vtkMRMLNode);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

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
  // Updates other nodes in the scene depending on this node
  // or updates this node if it depends on other nodes
  virtual void UpdateScene(vtkMRMLScene *);

// Description:
  // Update the stored reference to another node in the scene
  void UpdateReferenceID(const char *oldID, const char *newID);

  // Description:
  // Updates this node if it depends on other nodes 
  // when the node is deleted in the scene
  void UpdateReferences();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "ProstateNavManager";};


  // Description:
  // Method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //----------------------------------------------------------------
  // Workflow wizard step management
  //----------------------------------------------------------------

  // Description:
  // Get number of wizard steps
  int GetNumberOfSteps();

  // Description:
  // Get number of wizard steps
  const char* GetStepName(int i);
  
  // Description:
  // Switch step. Returns 0 if it is failed.
  int SwitchStep(int newStep);

  // Description:
  // Get current workflow step.
  int GetCurrentStep();

  // Description:
  // Get previous workflow step.
  int GetPreviousStep();

  //----------------------------------------------------------------
  // Target Management
  //----------------------------------------------------------------
  
  // Description:
  // Set and start observing target plan list
  //void SetAndObserveTargetPlanList(vtkMRMLFiducialListNode* ptr);
  vtkGetStringMacro(TargetPlanListNodeID);
  vtkMRMLFiducialListNode* GetTargetPlanListNode();
  void SetAndObserveTargetPlanListNodeID(const char *targetPlanListNodeID);

  // Description:
  // Set and start observing completed target list
  //void SetAndObserveTargetCompletedList(vtkMRMLFiducialListNode* ptr);
  vtkGetStringMacro(TargetCompletedListNodeID);
  vtkMRMLFiducialListNode* GetTargetCompletedListNode();
  void SetAndObserveTargetCompletedListNodeID(const char *targetCompletedListNodeID);

  vtkGetStringMacro(RobotNodeID);
  vtkMRMLRobotNode* GetRobotNode();
  void SetAndObserveRobotNodeID(const char *robotNodeID);

 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
  
  vtkMRMLProstateNavManagerNode();
  ~vtkMRMLProstateNavManagerNode();
  vtkMRMLProstateNavManagerNode(const vtkMRMLProstateNavManagerNode&);
  void operator=(const vtkMRMLProstateNavManagerNode&);


 protected:
  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------

  vtkStdString GetWorkflowStepsString();
  bool SetWorkflowStepsFromString(const vtkStdString& workflowStepsString);

  // List of workflow steps (wizard pages)
  vtkStringArray *StepList;
  
  int CurrentStep;
  int PreviousStep;
  
  //vtkMRMLFiducialListNode* TargetPlanList;
  vtkSetReferenceStringMacro(TargetPlanListNodeID);
  char *TargetPlanListNodeID;
  vtkMRMLFiducialListNode* TargetPlanListNode;

  //vtkMRMLFiducialListNode* TargetCompletedList;  
  vtkSetReferenceStringMacro(TargetCompletedListNodeID);
  char *TargetCompletedListNodeID;
  vtkMRMLFiducialListNode* TargetCompletedListNode;

  //vtkMRMLRobotNode* RobotNode;
  vtkSetReferenceStringMacro(RobotNodeID);
  char *RobotNodeID;
  vtkMRMLRobotNode* RobotNode;

};

#endif

