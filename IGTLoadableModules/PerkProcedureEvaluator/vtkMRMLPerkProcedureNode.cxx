
#include "vtkMRMLPerkProcedureNode.h"


#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"

#include "vtkMRMLLinearTransformNode.h"



// =============================================================


vtkTransform*
StrToTransform( std::string str )
{
  std::stringstream ss( str );
  
  double e00; ss >> e00; double e01; ss >> e01; double e02; ss >> e02; double e03; ss >> e03;
  double e10; ss >> e10; double e11; ss >> e11; double e12; ss >> e12; double e13; ss >> e13;
  double e20; ss >> e20; double e21; ss >> e21; double e22; ss >> e22; double e23; ss >> e23;
  double e30; ss >> e30; double e31; ss >> e31; double e32; ss >> e32; double e33; ss >> e33;
  
  vtkTransform* tr = vtkTransform::New(); // Needs to be deleted after using return data.
  tr->Identity();
  
  tr->GetMatrix()->SetElement( 0, 0, e00 );
  tr->GetMatrix()->SetElement( 0, 1, e01 );
  tr->GetMatrix()->SetElement( 0, 2, e02 );
  tr->GetMatrix()->SetElement( 0, 3, e03 );
  
  tr->GetMatrix()->SetElement( 1, 0, e10 );
  tr->GetMatrix()->SetElement( 1, 1, e11 );
  tr->GetMatrix()->SetElement( 1, 2, e12 );
  tr->GetMatrix()->SetElement( 1, 3, e13 );
  
  tr->GetMatrix()->SetElement( 2, 0, e20 );
  tr->GetMatrix()->SetElement( 2, 1, e21 );
  tr->GetMatrix()->SetElement( 2, 2, e22 );
  tr->GetMatrix()->SetElement( 2, 3, e23 );
  
  tr->GetMatrix()->SetElement( 3, 0, e30 );
  tr->GetMatrix()->SetElement( 3, 1, e31 );
  tr->GetMatrix()->SetElement( 3, 2, e32 );
  tr->GetMatrix()->SetElement( 3, 3, e33 );
  
  return tr;
}


// =============================================================


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
  Superclass::ReadXMLAttributes( atts );
  
  const char* attName;
  const char* attValue;
  
  while( *atts != NULL )
    {
    attName = *(atts++);
    attValue = *(atts++);
    
    if ( ! strcmp( attName, "ObservedTransformNodeID" ) )
      {
      this->SetAndObserveObservedTransformNodeID( NULL );
      this->SetObservedTransformNodeID( attValue );
      }
    }
}



void
vtkMRMLPerkProcedureNode
::WriteXML( ostream& of, int indent )
{
  Superclass::WriteXML( of, indent );
  
  vtkIndent vindent( indent );
  
  if ( this->ObservedTransformNodeID != NULL )
    {
    of << vindent << " ObservedTransformNodeID=\"" << this->ObservedTransformNodeID << "\"";
    }
}



void
vtkMRMLPerkProcedureNode
::ImportFromFile( const char* fileName )
{
  std::ifstream input( fileName );
  if ( ! input.is_open() ) return;
  input.close();
  
  
  this->ClearData();
  
  
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName );
  parser->Parse();
  
  vtkXMLDataElement* element = parser->GetRootElement();
  if ( ! element ) return;
  
  int num = element->GetNumberOfNestedElements();
  
  for ( int i = 0; i < num; ++ i )
    {
    vtkXMLDataElement* noteElement = element->GetNestedElement( i );
    if ( strcmp( noteElement->GetName(), "log" ) != 0 ) continue;
    const char* type = noteElement->GetAttribute( "type" );
    double time = atof( noteElement->GetAttribute( "time" ) );
    
    if ( strcmp( type, "message" ) == 0 )
      {
      PerkNote* note = new PerkNote; // Deleted in NoteList destructor.
      note->Time = time;
      note->Event = PerkNote::NoEvent;
      note->Message = std::string( noteElement->GetAttribute( "message" ) );
      this->NoteList.push_back( note );
      }
    
    else if ( strcmp( type, "transform" ) == 0 )
      {
      const char* traC = noteElement->GetAttribute( "transform" );
      vtkTransform* tr = StrToTransform( std::string( traC ) );
      this->TransformTimeSeries->AddRecord( time, tr );
      tr->Delete();
      }
    }
}



void
vtkMRMLPerkProcedureNode
::Copy( vtkMRMLNode* node )
{
  Superclass::Copy( node );
  vtkMRMLPerkProcedureNode* perkNode = ( vtkMRMLPerkProcedureNode* )node;
}



PerkNote*
vtkMRMLPerkProcedureNode
::GetNoteAtIndex( int index )
{
  if ( index >= 0  &&  (unsigned int)index < this->NoteList.size() )
    {
    return this->NoteList[ index ];
    }
  else
    return NULL;
}



double
vtkMRMLPerkProcedureNode
::GetTimeAtTransformIndex( int index )
{
    // Index validity checked in TransformTimeSeries.
  
  return this->TransformTimeSeries->GetTimeAtIndex( index );
}



vtkMRMLLinearTransformNode*
vtkMRMLPerkProcedureNode
::GetObservedTransformNode()
{
  vtkMRMLLinearTransformNode* node = NULL;
  if ( this->GetScene() && this->ObservedTransformNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ObservedTransformNodeID );
    node = vtkMRMLLinearTransformNode::SafeDownCast( snode );
    }
  return node;
}



void
vtkMRMLPerkProcedureNode
::SetAndObserveObservedTransformNodeID( const char *nodeID )
{
  vtkSetAndObserveMRMLObjectMacro( this->ObservedTransformNode, NULL );
  this->SetObservedTransformNodeID( nodeID );
  vtkMRMLLinearTransformNode *tnode = this->GetObservedTransformNode();
  vtkSetAndObserveMRMLObjectMacro(this->ObservedTransformNode, tnode);
  if ( tnode )
    {
    tnode->AddObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand );
    }
}



vtkMRMLLinearTransformNode*
vtkMRMLPerkProcedureNode
::GetNeedleTransformNode()
{
  vtkMRMLLinearTransformNode* node = NULL;
  if ( this->GetScene() && this->NeedleTransformNodeID != NULL )
    {
    vtkMRMLNode* cnode = this->GetScene()->GetNodeByID( this->NeedleTransformNodeID );
    node = vtkMRMLLinearTransformNode::SafeDownCast( cnode );
    }
  return node;
}



void
vtkMRMLPerkProcedureNode
::SetAndObserveNeedleTransformNodeID( const char *TransformNodeRef )
{
  vtkSetAndObserveMRMLObjectMacro( this->NeedleTransformNode, NULL );
  this->SetNeedleTransformNodeID( TransformNodeRef );
  vtkMRMLLinearTransformNode* cnode = this->GetNeedleTransformNode();
  vtkSetAndObserveMRMLObjectMacro( this->NeedleTransformNode, cnode );
  if ( cnode )
    {
    cnode->AddObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent, (vtkCommand*)(this->MRMLCallbackCommand) );
    }
}



void
vtkMRMLPerkProcedureNode
::SetNoteIndex( int ind )
{
  if ( ind < 0 ) return;
  else if ( ind >= (int)( this->NoteList.size() ) ) return;
  
  this->NoteIndex = ind;
  
  this->UpdateTransformIndex();
}



void
vtkMRMLPerkProcedureNode
::UpdateTransformIndex()
{
  PerkNote* note = this->GetNoteAtIndex( this->NoteIndex );
  double time = note->Time;
  
  int numRecords = this->TransformTimeSeries->GetNumberOfRecords();
  if ( numRecords < 1 ) return;
  
  int index = 0;
  
  while ( this->TransformTimeSeries->GetTimeAtIndex( index ) < time )
    {
    ++ index;
    if ( index == numRecords ) break;
    }
  
  if ( index < numRecords ) this->TransformIndex = index;
  else this->TransformIndex = numRecords - 1;
  
  
    // Update transform.
  
  if ( this->NeedleTransformNode )
    {
    vtkTransform* transform = this->TransformTimeSeries->GetTransformAtIndex( this->TransformIndex );
    vtkMatrix4x4* needleMtx = this->NeedleTransformNode->GetMatrixTransformToParent();
    
    for ( int i = 0; i < 4; i ++ ) 
      {
      for ( int j = 0; j < 4; j ++ )
        {
        needleMtx->SetElement( i, j, ( transform->GetMatrix()->GetElement( i, j ) ) );
        }
      }
    }
}



vtkMRMLPerkProcedureNode
::vtkMRMLPerkProcedureNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  this->TransformTimeSeries = vtkTransformTimeSeries::New();
  
  this->ObservedTransformNode = NULL;
  this->ObservedTransformNodeID = NULL;
  this->NeedleTransformNode = NULL;
  this->NeedleTransformNodeID = NULL;
  
  this->NoteIndex = -1;
  this->TransformIndex = -1;
}



vtkMRMLPerkProcedureNode
::~vtkMRMLPerkProcedureNode()
{
  this->ClearData();
  
  this->TransformTimeSeries->Delete();
  this->TransformTimeSeries = NULL;
  
  this->RemoveMRMLObservers();
  
  this->SetAndObserveObservedTransformNodeID( NULL );
  this->SetAndObserveNeedleTransformNodeID( NULL );
}



void
vtkMRMLPerkProcedureNode
::RemoveMRMLObservers()
{
  if (    this->NeedleTransformNode
       && this->NeedleTransformNode->HasObserver( vtkMRMLLinearTransformNode::TransformModifiedEvent ) )
    {
    this->NeedleTransformNode->RemoveObservers( vtkMRMLLinearTransformNode::TransformModifiedEvent );
    }
  
  if (    this->ObservedTransformNode
       && this->ObservedTransformNode->HasObserver( vtkMRMLTransformNode::TransformModifiedEvent ) )
    {
    this->ObservedTransformNode->RemoveObservers( vtkMRMLTransformNode::TransformModifiedEvent );
    }
}



void
vtkMRMLPerkProcedureNode
::ClearData()
{
  for ( unsigned int index = 0; index < this->NoteList.size(); ++ index )
    {
    delete this->NoteList[ index ];
    this->NoteList[ index ] = NULL;
    }
  
  this->NoteList.clear();
}

