
#include "vtkMRMLSurgicalShape.h"



vtkMRMLSurgicalShape
::vtkMRMLSurgicalShape()
{
  
}



vtkMRMLSurgicalShape
::~vtkMRMLSurgicalShape()
{
  
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
