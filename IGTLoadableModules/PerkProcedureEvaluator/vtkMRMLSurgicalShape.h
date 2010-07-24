
#ifndef VTKMRMLSURGICALSHAPE_H
#define VTKMRMLSURGICALSHAPE_H


#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"


class vtkMRMLFiducialListNode;
class vtkMRMLModelNode;
class vtkPolyData;


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
  
  
    // Specific functions.
  
  virtual bool IsInside( double r, double a, double s );
  virtual bool IsInside( const double* point );
  virtual void Initialize( vtkMRMLFiducialListNode* fiducials ) = 0;
  
  
protected:
  
  virtual void AddModelNode( const char* nodeName, double r, double g, double b ) = 0;
  
  vtkMRMLSurgicalShape();
  ~vtkMRMLSurgicalShape();
  vtkMRMLSurgicalShape( const vtkMRMLSurgicalShape& );
  void operator=( const vtkMRMLSurgicalShape& );
  
  
    // MRML Model.
  
  vtkGetStringMacro( ModelNodeRef );
  vtkSetReferenceStringMacro( ModelNodeRef );
  vtkMRMLModelNode* GetModelNode();
  
  vtkPolyData*  ModelPolyData;
  char*         ModelNodeRef;  // Store in the scene.
};

#endif
