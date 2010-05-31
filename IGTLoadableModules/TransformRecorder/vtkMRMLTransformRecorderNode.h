
#include <iostream>

#include "vtkMRMLNode.h"
// #include "vtkMRMLStorageNode.h"
#include "vtkMRMLTransformNode.h"

#include "vtkTransformRecorderWin32Header.h"


class
VTK_TransformRecorder_EXPORT
vtkMRMLTransformRecorderNode
: public vtkMRMLNode
{

public:
  
  //BTX
  // Events.
  enum {
    TransformChangedEvent = 201001
  };
  //ETX
  
  // Standard MRML node methods

  static vtkMRMLTransformRecorderNode *New();
  vtkTypeMacro( vtkMRMLTransformRecorderNode, vtkMRMLNode );
  virtual vtkMRMLNode* CreateNodeInstance();
  virtual const char* GetNodeTagName() { return "vtkMRMLTransformRecorderNode"; };
  void PrintSelf( ostream& os, vtkIndent indent );
  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  virtual void Copy( vtkMRMLNode *node );
  virtual void UpdateScene( vtkMRMLScene * );
  virtual void UpdateReferenceID( const char *oldID, const char *newID );
  void UpdateReferences();

  // Public interface

  vtkGetStringMacro( ObservedTransformNodeID );
  vtkMRMLTransformNode* GetObservedTransformNode();
  void SetAndObserveObservedTransformNodeID( const char *TransformNodeRef );
  
  vtkGetMacro( Recording, bool );
  vtkSetMacro( Recording, bool );
  
  //BTX
  void SetLogFileName( std::string fileName );
  std::string GetLogFileName();
  //ETX
  
protected:

  // Constructor/desctructor

  vtkMRMLTransformRecorderNode();
  virtual ~vtkMRMLTransformRecorderNode();
  vtkMRMLTransformRecorderNode ( const vtkMRMLTransformRecorderNode& );
  void operator=( const vtkMRMLTransformRecorderNode& );

  void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  // Protected member variables
    
  vtkSetReferenceStringMacro( ObservedTransformNodeID );
  char* ObservedTransformNodeID;
  vtkMRMLTransformNode* ObservedTransformNode;
  
  void WriteLog();
  
  //BTX
  std::string LogFileName;
  //ETX
  
  bool Recording;
  
};
