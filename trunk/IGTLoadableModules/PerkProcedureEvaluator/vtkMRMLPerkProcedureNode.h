
#ifndef VTKMRMLPERKPROCEDURENODE_H
#define VTKMRMLPERKPROCEDURENODE_H


#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif

#include <iostream>
#include <string>
#include <vector>

#include "vtkSmartPointer.h"

#include "vtkMRMLNode.h"

#include "vtkTransformTimeSeries.h"



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
: public vtkMRMLNode
{

public:

  static vtkMRMLPerkProcedureNode* New();
  vtkTypeMacro( vtkMRMLPerkProcedureNode, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkMRMLNode* CreateNodeInstance();
  
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
  
  PerkNote* GetNoteAtIndex( int index );
  

protected:
  
  vtkMRMLPerkProcedureNode();
  ~vtkMRMLPerkProcedureNode();


private:

  vtkMRMLPerkProcedureNode( const vtkMRMLPerkProcedureNode& );
  void operator=( const vtkMRMLPerkProcedureNode& );
  
  void ClearData();
  
  
  //BTX
  std::vector< PerkNote* > NoteList;
  vtkTransformTimeSeries* TransformTimeSeries;
  //ETX
};


#endif
