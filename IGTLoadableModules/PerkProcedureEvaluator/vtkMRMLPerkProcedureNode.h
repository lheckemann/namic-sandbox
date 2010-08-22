
#ifndef VTKMRMLPERKPROCEDURENODE_H
#define VTKMRMLPERKPROCEDURENODE_H


#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif

#include <iostream>
#include <string>
#include <vector>

#include "vtkSmartPointer.h"

#include "vtkMRMLBoxShape.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLStorableNode.h"

#include "vtkMRMLPerkProcedureStorageNode.h"

#include "vtkTransformTimeSeries.h"


class vtkMRMLLinearTransformNode;



//BTX
/**
 * One record in the perk procedure log.
 */
class
VTK_PerkProcedureEvaluator_EXPORT
PerkNote
{
public:
  
  enum EventType
    {
    NoEvent = 0,
    PierceSkin,
    MoveForward,
    Retract,
    AdjustAngle,
    PullOut
    };
  
  double Time;
  EventType Event;
  std::string Message;
};
//ETX



// ==================================================================



class
VTK_PerkProcedureEvaluator_EXPORT
vtkMRMLPerkProcedureNode
: public vtkMRMLStorableNode
{

public:

  static vtkMRMLPerkProcedureNode* New();
  vtkTypeMacro( vtkMRMLPerkProcedureNode, vtkMRMLStorableNode );
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkMRMLNode* CreateNodeInstance();
  
  
    // transform utility functions
  
  virtual bool CanApplyNonLinearTransforms() { return true; }
  virtual void ApplyTransform(vtkMatrix4x4* transformMatrix) {}
  virtual void ApplyTransform(vtkAbstractTransform* transform) {}
  
  
  vtkSetStringMacro( FileName );
  vtkGetStringMacro( FileName );
  
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  
  void ImportFromFile( const char* fileName );
  
  virtual void Copy( vtkMRMLNode* node );
  
  virtual const char* GetNodeTagName() {
    return "PerkProcedureNode";
  }
  
  
  int GetNumberOfNotes() {
    return this->NoteList.size();
  }
  
  int GetNumberOfTransforms() {
    return this->TransformTimeSeries->GetNumberOfRecords();
  }
  
  PerkNote* GetNoteAtIndex( int index );
  double GetTimeAtTransformIndex( int index );
  vtkTransform* GetTransformAtTransformIndex( int index );
  
  vtkGetStringMacro( ObservedTransformNodeID );
  vtkMRMLLinearTransformNode* GetObservedTransformNode();
  void SetAndObserveObservedTransformNodeID( const char *TransformNodeRef );
  
  vtkGetStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* GetNeedleTransformNode();
  void SetAndObserveNeedleTransformNodeID( const char *TransformNodeRef );
  
  
    // Storage.
  
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode()
    {
    return vtkMRMLPerkProcedureStorageNode::New();
    };
  
  virtual const char* GetDefaultWriteFileExtension()
    {
    return "xml";
    };
  
  
  bool IsNeedleInsideBody();
  
    
    // Models.
  
  vtkGetStringMacro( BoxShapeID );
  vtkMRMLBoxShape* GetBoxShapeNode();
  void SetAndObserveBoxShapeID( const char* boxShapeRef );
  void BoxShapeFromFiducials( vtkMRMLFiducialListNode* fiducials );
  
  
  
  void SetNoteIndex( int ind );
  vtkGetMacro( NoteIndex, int );
  
  void SetTransformIndex( int ind ) {
    if ( ind >= 0 && ind < this->TransformTimeSeries->GetNumberOfRecords() )
      {
      this->TransformIndex = ind;
      }
    this->UpdateTransform();
  }
  
  vtkGetMacro( TransformIndex, int );
  
  
    // For measurements.
  
  void MarkIndexBegin();
  void MarkIndexEnd();
  vtkGetMacro( IndexBegin, int );
  vtkGetMacro( IndexEnd, int );
  
  void SetPlan( vtkMRMLFiducialListNode* fiducials );
  
  
  vtkGetMacro( TotalTime, double );
  vtkGetMacro( PathInside, double );
  vtkGetMacro( TimeInside, double );
  vtkGetMacro( AngleFromAxial, double );
  vtkGetMacro( AngleInAxial, double );
  
  
  void UpdateTransformIndex();
  void UpdateMeasurements();
  void UpdateTransform();
  

protected:
  
  vtkMRMLPerkProcedureNode();
  ~vtkMRMLPerkProcedureNode();
  
  void RemoveMRMLObservers();
  

private:

  vtkMRMLPerkProcedureNode( const vtkMRMLPerkProcedureNode& );
  void operator=( const vtkMRMLPerkProcedureNode& );
  
  void ClearData();
  
  char* FileName;
  
  //BTX
  std::vector< PerkNote* > NoteList;
  int NoteIndex;
  
  vtkTransformTimeSeries* TransformTimeSeries;
  int TransformIndex;
  //ETX
  
  
    // Transform: Needle tip to Calibration.
  
  char* ObservedTransformNodeID;
  vtkSetReferenceStringMacro( ObservedTransformNodeID );
  vtkMRMLLinearTransformNode* ObservedTransformNode;
  
    // Transform: Needle tip to World (RAS).
  char* NeedleTransformNodeID;
  vtkSetReferenceStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* NeedleTransformNode;
  
  
    // Models.
  
  char* BoxShapeID;
  vtkSetReferenceStringMacro( BoxShapeID );
  vtkMRMLBoxShape* BoxShape;
  
  
    // For measurements.
  
  int IndexBegin;
  int IndexEnd;
  
  double PlanEntryPoint[ 4 ];
  double PlanTargetPoint[ 4 ];
  
    // Measurements.
  
  double TotalTime;
  double PathInside;
  double TimeInside;
  double AngleFromAxial;
  double AngleInAxial;
};


#endif
