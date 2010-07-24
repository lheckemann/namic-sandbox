
#ifndef VTKMRMLSURGICALSHAPE_H
#define VTKMRMLSURGICALSHAPE_H


#include "vtkMRMLNode.h"


/**
 * Superclass for surgical shapes.
 */
class
vtkMRMLSurgicalShape
: public vtkMRMLNode
{
public:
  
  static vtkMRMLSurgicalShape *New(){ return NULL; };
  vtkTypeMacro( vtkMRMLSurgicalShape, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual vtkMRMLNode* CreateNodeInstance() = 0;

  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  virtual void Copy( vtkMRMLNode *node );
  
  virtual const char* GetNodeTagName() = 0;
  
  virtual void UpdateScene( vtkMRMLScene *scene ) {
     Superclass::UpdateScene( scene );
  };
  
protected:

  vtkMRMLSurgicalShape();
  ~vtkMRMLSurgicalShape();
  vtkMRMLSurgicalShape( const vtkMRMLSurgicalShape& );
  void operator=( const vtkMRMLSurgicalShape& );
  
};

#endif
