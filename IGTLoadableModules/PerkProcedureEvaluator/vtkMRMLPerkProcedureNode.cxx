
#include "vtkMRMLPerkProcedureNode.h"


#include <fstream>
#include <iostream>
#include <vector>



vtkMRMLPerkProcedureNode*
vtkMRMLPerkProcedureNode
::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureNode" );
  if ( ret )
    {
    return ( vtkMRMLPerkProcedureNode* )ret;
    }
  return new vtkMRMLPerkProcedureNode;
}



void
vtkMRMLPerkProcedureNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}



vtkMRMLNode*
vtkMRMLPerkProcedureNode
::CreateNodeInstance()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureNode" );
  if ( ret )
    {
    return ( vtkMRMLPerkProcedureNode* )ret;
    }
  return new vtkMRMLPerkProcedureNode;
}



void
vtkMRMLPerkProcedureNode
::ReadXMLAttributes( const char** atts )
{

}



void
vtkMRMLPerkProcedureNode
::WriteXML( ostream& of, int indent )
{

}



void
vtkMRMLPerkProcedureNode
::ImportFromFile( const char* fileName )
{
  std::ifstream input( fileName );
  if ( ! input.is_open() ) return;
  
  
  
  input.close();
}



void
vtkMRMLPerkProcedureNode
::Copy( vtkMRMLNode* node )
{
  Superclass::Copy( node );
  vtkMRMLPerkProcedureNode* perkNode = ( vtkMRMLPerkProcedureNode* )node;
}



vtkMRMLPerkProcedureNode
::vtkMRMLPerkProcedureNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
}



vtkMRMLPerkProcedureNode
::~vtkMRMLPerkProcedureNode()
{
  
}


