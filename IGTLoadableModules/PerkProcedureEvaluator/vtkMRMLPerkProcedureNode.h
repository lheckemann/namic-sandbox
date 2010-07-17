
#ifndef VTKMRMLPERKPROCEDURENODE_H
#define VTKMRMLPERKPROCEDURENODE_H


#include "vtkMRMLNode.h"


class
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


protected:
  
  vtkMRMLPerkProcedureNode();
  ~vtkMRMLPerkProcedureNode();


private:

  vtkMRMLPerkProcedureNode( const vtkMRMLPerkProcedureNode& );
  void operator=( const vtkMRMLPerkProcedureNode& );
};


#endif
