
#ifndef VTKMRMLPRISMSHAPE_H
#define VTKMRMLPRISMSHAPE_H


#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLSurgicalShape.h"

#include "vtkTransform.h"

#include "PerkProcedureEvaluatorHelper.h"


/**
 * Brick shape determined by two fiducials (min and max coordinates).
 */
class
vtkMRMLPrismShape
: public vtkMRMLSurgicalShape
{
public:
  
  static vtkMRMLPrismShape *New();
  vtkTypeMacro( vtkMRMLPrismShape, vtkMRMLNode );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual vtkMRMLNode* CreateNodeInstance();

  virtual void ReadXMLAttributes( const char** atts );
  virtual void WriteXML( ostream& of, int indent );
  
  virtual void Copy( vtkMRMLNode *node );
  
  virtual const char* GetNodeTagName() {
    return "MRMLPrismShape";
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
  
  vtkMRMLPrismShape();
  vtkMRMLPrismShape( const vtkMRMLPrismShape& );
  void operator=( const vtkMRMLPrismShape& );


private:
  
  void ComputePlanes();
  
  
    // These points determine the prism. 3 pairs (P0,P1), (P0,P2) and (P0,P3)
    // determine the three primary planes. Normals of these planes point to the
    // side where the prism is. Regardless of the order of the P points.
  
  double P0[ 3 ];
  double P1[ 3 ];
  double P2[ 3 ];
  double P3[ 3 ];
  
  
    // 6 planes of the prism are defined by plane equations:
    // Ax + By + Cz + D = 0
  
  double A[ 3 ];
  double B[ 3 ];
  double C[ 3 ];
  double D[ 6 ];
  
  
  bool Initialized;
};

#endif
