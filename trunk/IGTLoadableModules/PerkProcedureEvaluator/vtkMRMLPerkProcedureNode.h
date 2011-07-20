
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
  double Time;
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
  virtual const char* GetNodeTagName() { return "PerkProcedureNode"; }
  
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  virtual void Copy( vtkMRMLNode* node );
  virtual void UpdateScene( vtkMRMLScene* );
  virtual void UpdateReferenceID( const char* oldID, const char* newID );
  void UpdateReferences();
  
  
  vtkSetStringMacro( FileName );
  vtkGetStringMacro( FileName );
  void ImportFromFile( const char* fileName );
  
  
    // transform utility functions
  
  virtual bool CanApplyNonLinearTransforms() { return true; }
  virtual void ApplyTransform(vtkMatrix4x4* transformMatrix) {}
  virtual void ApplyTransform(vtkAbstractTransform* transform) {}
  
  
  int GetNumberOfNotes() { return this->NoteList.size(); }
  int GetNumberOfTransforms() { return this->TransformTimeSeries->GetNumberOfRecords(); }
  
  PerkNote* GetNoteAtIndex( int index );
  double GetRelativeTimeAtNoteIndex( int index );
  double GetTimeAtTransformIndex( int index );
  double GetRelativeTimeAtTransformIndex( int index );
  vtkTransform* GetTransformAtTransformIndex( int index );
  
  
    // Storage.
  
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode();
  
  
  virtual const char* GetDefaultWriteFileExtension() { return "xml"; };
  
  
  bool IsNeedleInsideBody();
  
    
    // Models.
  
  void BoxShapeFromFiducials( vtkMRMLFiducialListNode* fiducials );
  
  
  
  void SetNoteIndex( int ind );
  vtkGetMacro( NoteIndex, int );
  
  void SetTransformIndex( int ind );
  
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
  vtkGetMacro( SurfaceInside, double );
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
  
  double ApproximateSurface( double* e1, double* e2, double* t1, double* t2 );
  
  char* FileName;
  
  //BTX
  std::vector< PerkNote* > NoteList;
  int NoteIndex;
  
  vtkTransformTimeSeries* TransformTimeSeries;
  int TransformIndex;
  //ETX
  
  
    // Reference to the needle tip to calibration transform.
  
public:
  vtkGetStringMacro( NeedleCalibrationTransformNodeID );
  vtkMRMLLinearTransformNode* GetNeedleCalibrationTransformNode();
  void SetAndObserveNeedleCalibrationTransformNodeID( const char *TransformNodeRef );
private:  
  char* NeedleCalibrationTransformNodeID;
  vtkSetReferenceStringMacro( NeedleCalibrationTransformNodeID );
  vtkMRMLLinearTransformNode* NeedleCalibrationTransformNode;
  
  
    // Transform: Needle tip.
    // It should either be given respective to World (RAS), or the NeedleCalibrationTransform
    // has to be used to transform it to the World coordinate system.
  
public:
  vtkGetStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* GetNeedleTransformNode();
  void SetAndObserveNeedleTransformNodeID( const char *TransformNodeRef );
private:
  char* NeedleTransformNodeID;
  vtkSetReferenceStringMacro( NeedleTransformNodeID );
  vtkMRMLLinearTransformNode* NeedleTransformNode;
  
  
    // Reference to the Box model.
  
public:
  vtkGetStringMacro( BoxShapeID );
  vtkMRMLBoxShape* GetBoxShapeNode();
  void SetAndObserveBoxShapeID( const char* boxShapeRef );
private:
  char* BoxShapeID;
  vtkSetReferenceStringMacro( BoxShapeID );
  vtkMRMLBoxShape* BoxShape;
  
  
    // For measurements.
  
  int IndexBegin;
  int IndexEnd;
  
  double PlanEntryPoint[ 4 ];
  double PlanTargetPoint[ 4 ];
  bool PlanReady;
  
    // Measurements.
  
  double TotalTime;
  double PathInside;
  double TimeInside;
  double SurfaceInside;
  double AngleFromAxial;
  double AngleInAxial;
};


#endif
