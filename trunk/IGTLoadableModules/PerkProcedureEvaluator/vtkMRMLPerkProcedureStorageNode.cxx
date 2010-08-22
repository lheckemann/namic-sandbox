
#include "vtkMRMLPerkProcedureStorageNode.h"

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkImageChangeInformation.h"
#include "vtkMRMLPerkProcedureNode.h"
#include "vtkStringArray.h"



vtkMRMLPerkProcedureStorageNode*
vtkMRMLPerkProcedureStorageNode::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureStorageNode");
  if( ret )
    {
    return ( vtkMRMLPerkProcedureStorageNode* )ret;
    }
  return new vtkMRMLPerkProcedureStorageNode;
}



vtkMRMLNode*
vtkMRMLPerkProcedureStorageNode
::CreateNodeInstance()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLPerkProcedureStorageNode" );
  if( ret )
    {
    return ( vtkMRMLPerkProcedureStorageNode* )ret;
    }
  return new vtkMRMLPerkProcedureStorageNode;
}



/**
 * Constructor.
 */
vtkMRMLPerkProcedureStorageNode::vtkMRMLPerkProcedureStorageNode()
{
}



vtkMRMLPerkProcedureStorageNode::~vtkMRMLPerkProcedureStorageNode()
{
}



void
vtkMRMLPerkProcedureStorageNode
::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML( of, nIndent );
}



void
vtkMRMLPerkProcedureStorageNode
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes( atts );
}



// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void
vtkMRMLPerkProcedureStorageNode
::Copy( vtkMRMLNode *anode )
{
  Superclass::Copy( anode );
}



void
vtkMRMLPerkProcedureStorageNode
::PrintSelf( ostream& os, vtkIndent indent )
{  
  vtkMRMLStorageNode::PrintSelf( os,indent );
}



void
vtkMRMLPerkProcedureStorageNode
::ProcessParentNode( vtkMRMLNode *parentNode )
{
  this->ReadData( parentNode );
}



int
vtkMRMLPerkProcedureStorageNode
::ReadData( vtkMRMLNode *refNode )
{
    // do not read if if we are not in the scene (for example inside snapshot)
  
  if ( ! this->GetAddToScene() || ! refNode->GetAddToScene() )
    {
    return 1;
    }

  vtkDebugMacro("Reading Perk procedure data");
  
    // test whether refNode is a valid node to hold a fiducial list
  
  if ( ! ( refNode->IsA( "vtkMRMLPerkProcedureNode" ) ) ) 
    {
    vtkErrorMacro("Reference node is not a proper vtkMRMLPerkProcedureNode");
    return 0;         
    }
  
  if ( this->GetFileName() == NULL && this->GetURI() == NULL ) 
    {
    vtkErrorMacro( "ReadData: file name and uri not set" );
    return 0;
    }
  
  Superclass::StageReadData( refNode );
  if ( this->GetReadState() != this->TransferDone )
    {
      // remote file download hasn't finished
    vtkWarningMacro( "ReadData: Read state is pending, returning." );
    return 0;
    }
  
  std::string fullName = this->GetFullNameFromFileName(); 

  if ( fullName == std::string( "" ) ) 
    {
    vtkErrorMacro( "vtkMRMLPerkProcedureStorageNode: File name not specified" );
    return 0;
    }
  
  
    // cast the input node
  
  vtkMRMLPerkProcedureNode *perkNode = NULL;
  if ( refNode->IsA( "vtkMRMLPerkProcedureNode" ) )
    {
    perkNode = dynamic_cast <vtkMRMLPerkProcedureNode *> (refNode);
    }

  if ( perkNode == NULL )
    {
    vtkErrorMacro( "ReadData: unable to cast input node " << refNode->GetID() << " to a perk procedure node" );
    return 0;
    }
  
  
    // open the file for reading input
  
  fstream fstr;
  fstr.open( fullName.c_str(), fstream::in );
  
  
  if ( fstr.is_open() )
    {
    
    fstr.close();
    }
  else
    {
    vtkErrorMacro( "ERROR opening fiducials file " << this->FileName << endl );
    return 0;
    }

 
  
  this->SetReadStateIdle();
  
    // make sure that the list node points to this storage node
  
  perkNode->SetAndObserveStorageNodeID( this->GetID() );
  
  return 1;
}



int
vtkMRMLPerkProcedureStorageNode
::WriteData( vtkMRMLNode *refNode )
{
  
    // test whether refNode is a valid node to hold a volume
  
  if ( !( refNode->IsA( "vtkMRMLPerkProcedureNode") ) )
    {
    vtkErrorMacro( "Reference node is not a proper vtkMRMLFiducialListNode" );
    return 0;         
    }

  if ( this->GetFileName() == NULL ) 
    {
    vtkErrorMacro( "WriteData: file name is not set" );
    return 0;
    }

  std::string fullName = this->GetFullNameFromFileName();
  if ( fullName == std::string( "" ) ) 
    {
    vtkErrorMacro( "vtkMRMLFiducialListStorageNode: File name not specified" );
    return 0;
    }
  
  // cast the input node
  vtkMRMLPerkProcedureNode *perkNode = NULL;
  if ( refNode->IsA( "vtkMRMLPerkProcedureNode" ) )
    {
    perkNode = dynamic_cast < vtkMRMLPerkProcedureNode* > ( refNode );
    }

  if ( perkNode == NULL )
    {
    vtkErrorMacro( "WriteData: unable to cast input node " << refNode->GetID() << " to a known perk procedure node");
    return 0;
    }
  
  
    // open the file for writing
  
  fstream of;
  of.open( fullName.c_str(), fstream::out );
  
  if ( ! of.is_open() )
    {
    vtkErrorMacro( "WriteData: unable to open file " << fullName.c_str() << " for writing" );
    return 0;
    }
  
  
  of << "<PerkProcedure>" << std::endl;
  
  // e.g. <log time="6824.48" type="message" message="Operator does alignment outside." />
  
    // Notes.
  
  for ( int i = 0; i < perkNode->GetNumberOfNotes(); ++ i )
    {
    PerkNote* note = perkNode->GetNoteAtIndex( i );
    of << "<log time=\"" << note->Time << "\" type=\"message\" ";
    of << "message=\"" << note->Message << "\" />" << std::endl;
    }
  
  // e.g. <log time="14040.5" type="transform"
  //       transform="-0.120413 0.328934 -0.992316 -114.316
  //                   0.982893 -0.304633 -0.220249 -107.265
  //                  -0.356106 -0.952045 -0.272373 172.544
  //                   0 0 0 1 " />
  
    // Transforms.
  
  for ( int index = 0; index < perkNode->GetNumberOfTransforms(); ++ index )
    {
    vtkTransform* transform = perkNode->GetTransformAtTransformIndex( index );
    double time = perkNode->GetTimeAtTransformIndex( index );
    of << "<log time=\"" << time << "\" type=\"transform\" ";
    of << "transform=\"";
    for ( int row = 0; row < 4; ++ row )
      {
      for ( int col = 0; col < 4; ++ col )
        {
        of << transform->GetMatrix()->GetElement( row, col ) << " ";
        }
      }
    of << "\" />" << std::endl;
    }
  
  
  
  of << "</PerkProcedure>" << std::endl;
  
  
  of.close();

  Superclass::StageWriteData( refNode );
  
  return 1;
}



int
vtkMRMLPerkProcedureStorageNode
::SupportedFileType( const char *fileName )
{
    // check to see which file name we need to check
  
  std::string name;
  if ( fileName )
    {
    name = std::string( fileName );
    }
  else if ( this->FileName != NULL )
    {
    name = std::string( this->FileName );
    }
  else if (this->URI != NULL)
    {
    name = std::string(this->URI);
    }
  else
    {
    vtkWarningMacro( "SupportedFileType: no file name to check" );
    return 0;
    }
  
  std::string::size_type loc = name.find_last_of(".");
  if( loc == std::string::npos ) 
    {
    vtkErrorMacro( "SupportedFileType: no file extension specified" );
    return 0;
    }
  std::string extension = name.substr( loc );

  vtkDebugMacro( "SupportedFileType: extension = " << extension.c_str() );
  if ( extension.compare( ".xml" ) == 0 )
    {
    return 1;
    }
  else
    {
    return 0;
    }
}



void
vtkMRMLPerkProcedureStorageNode
::InitializeSupportedWriteFileTypes()
{
  this->SupportedWriteFileTypes->InsertNextValue( "Perk Procedure XML (.xml)" );
}
