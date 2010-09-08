
#ifndef VTKMRMLBOXSHAPE_H
#define VTKMRMLBOXSHAPE_H


#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLSurgicalShape.h"

#include "vtkTransform.h"


/**
 * Brick shape determined by two fiducials (min and max coordinates).
 */
class
vtkMRMLBoxShape
: public vtkMRMLSurgicalShape
{
public:
  
  static vtkMRMLBoxShape *New();
  vtkTypeMacro( vtkMRMLBoxShape, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual vtkMRMLNode* CreateNodeInstance();

  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  virtual void Copy( vtkMRMLNode *node );
  
  virtual const char* GetNodeTagName() {
    return "MRMLBoxShape";
  }
  
  virtual void UpdateScene( vtkMRMLScene *scene ) {
     Superclass::UpdateScene( scene );
  };
  
  
    // Specific functions.
  
  virtual bool IsInside( double r, double a, double s );
  virtual bool IsInside( const double* point );
  
  virtual void Initialize( vtkMRMLFiducialListNode* fiducials );
  
  
    // Own functions.
  
  bool GetEntryPoint( vtkTransform* tr, double* entry );
  
  
protected:
  
  virtual void AddModelNode( const char* nodeName, double r, double g, double b );
  
  vtkMRMLBoxShape();
  vtkMRMLBoxShape( const vtkMRMLBoxShape& );
  void operator=( const vtkMRMLBoxShape& );


private:
  
  double MinR;
  double MaxR;
  double MinA;
  double MaxA;
  double MinS;
  double MaxS;
  
  bool Initialized;
};

#endif
