
#include "vtkMRMLSurgicalShape.h"


#include "vtkPolyData.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLModelNode.h"



vtkMRMLSurgicalShape
::vtkMRMLSurgicalShape()
{
  this->ModelPolyData = vtkPolyData::New();
  this->ModelNodeRef = NULL;
}



vtkMRMLSurgicalShape
::~vtkMRMLSurgicalShape()
{
  // this->SetModelNodeRef( NULL );
  
  if ( this->ModelPolyData )
    {
    this->ModelPolyData->Delete();
    this->ModelPolyData = NULL;
    }
}



void
vtkMRMLSurgicalShape
::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML( of, nIndent );
}



void
vtkMRMLSurgicalShape
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes( atts );
}



void
vtkMRMLSurgicalShape
::Copy( vtkMRMLNode *anode )
{
  Superclass::Copy( anode );
}



void
vtkMRMLSurgicalShape
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf(os,indent);
}



bool
vtkMRMLSurgicalShape
::IsInside( double r, double a, double s )
{
  return false;
}



bool
vtkMRMLSurgicalShape
::IsInside( const double* point )
{
  return false;
}



vtkMRMLModelNode*
vtkMRMLSurgicalShape
::GetModelNode()
{
  if ( this->GetScene()  &&  this->ModelNodeRef != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast( this->GetScene()->GetNodeByID( this->ModelNodeRef ) );
    }
  
  return NULL;
}
