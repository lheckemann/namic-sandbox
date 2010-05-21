
#include "vtkMRMLTransformRecorderNode.h"

#include <fstream>

#include "vtkIntArray.h"
#include "vtkSmartPointer.h"


vtkMRMLTransformRecorderNode*
vtkMRMLTransformRecorderNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLTransformRecorderNode" );
  if( ret )
    {
      return ( vtkMRMLTransformRecorderNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransformRecorderNode;
}


vtkMRMLNode*
vtkMRMLTransformRecorderNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLTransformRecorderNode" );
  if( ret )
    {
      return ( vtkMRMLTransformRecorderNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransformRecorderNode;
}


//----------------------------------------------------------------------------
vtkMRMLTransformRecorderNode::vtkMRMLTransformRecorderNode()
{
  this->SetHideFromEditors(false);
  this->ObservedTransformNodeID = NULL;
  this->ObservedTransformNode = NULL;
  
  this->Recording = false;
}


//----------------------------------------------------------------------------
vtkMRMLTransformRecorderNode::~vtkMRMLTransformRecorderNode()
{
   this->SetAndObserveObservedTransformNodeID( NULL );
}


//----------------------------------------------------------------------------
void
vtkMRMLTransformRecorderNode
::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);
  
  if (this->ObservedTransformNodeID != NULL) 
    {
    of << indent << " ObservedTransformNodeRef=\""
       << this->ObservedTransformNodeID << "\"";
    }
}

//----------------------------------------------------------------------------
void vtkMRMLTransformRecorderNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
    attName  = *(atts++);
    attValue = *(atts++);

    if ( ! strcmp( attName, "ObservedTransformNodeRef" ) ) 
      {
      this->SetAndObserveObservedTransformNodeID( NULL ); // clear any previous observers
      // do not add observers yet because updates may be wrong before reading all the xml attributes
      // observers will be added when all the attributes are read and UpdateScene is called
      this->SetObservedTransformNodeID( attValue );
      }

    }
}


//----------------------------------------------------------------------------
void vtkMRMLTransformRecorderNode::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode );
  vtkMRMLTransformRecorderNode *node = ( vtkMRMLTransformRecorderNode* ) anode;

  // Observers must be removed here, otherwise MRML updates would activate nodes on the undo stack
  this->SetAndObserveObservedTransformNodeID( NULL );
  this->SetObservedTransformNodeID( node->ObservedTransformNodeID );
}


//----------------------------------------------------------------------------
void vtkMRMLTransformRecorderNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (    this->ObservedTransformNodeID != NULL
       && this->Scene->GetNodeByID( this->ObservedTransformNodeID ) == NULL )
    {
    this->SetAndObserveObservedTransformNodeID( NULL );
    }
}


//----------------------------------------------------------------------------
void
vtkMRMLTransformRecorderNode
::UpdateReferenceID( const char *oldID, const char *newID )
{
  Superclass::UpdateReferenceID( oldID, newID );
  
  if (    this->ObservedTransformNodeID
       && ! strcmp( oldID, this->ObservedTransformNodeID ) )
    {
    this->SetAndObserveObservedTransformNodeID( newID );
    }
}


//----------------------------------------------------------------------------
void
vtkMRMLTransformRecorderNode
::UpdateScene( vtkMRMLScene *scene )
{
   Superclass::UpdateScene( scene );
   
   this->SetAndObserveObservedTransformNodeID( this->ObservedTransformNodeID );
}


//----------------------------------------------------------------------------
void vtkMRMLTransformRecorderNode::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "ObservedTransformNodeID: " <<
    (this->ObservedTransformNodeID? this->ObservedTransformNodeID: "(none)") << "\n";
}

//----------------------------------------------------------------------------
void
vtkMRMLTransformRecorderNode
::SetAndObserveObservedTransformNodeID( const char *nodeID )
{
  vtkSetAndObserveMRMLObjectMacro( this->ObservedTransformNode, NULL );
  this->SetObservedTransformNodeID( nodeID );
  vtkMRMLTransformNode *tnode = this->GetObservedTransformNode();
  vtkSetAndObserveMRMLObjectMacro(this->ObservedTransformNode, tnode);
}


//----------------------------------------------------------------------------
vtkMRMLTransformNode*
vtkMRMLTransformRecorderNode
::GetObservedTransformNode()
{
  vtkMRMLTransformNode* node = NULL;
  if (   this->GetScene()
      && this->ObservedTransformNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(
                            this->ObservedTransformNodeID );
    node = vtkMRMLTransformNode::SafeDownCast( snode );
    }
  return node;
}


void
vtkMRMLTransformRecorderNode
::ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData )
{
  if ( this->ObservedTransformNode == vtkMRMLTransformNode::SafeDownCast( caller ) )
    {
    if ( this->Recording )
      {
      std::ofstream output( "a.txt", std::ios_base::app );
      this->ObservedTransformNode->WriteXML( output, 0 );
      output << std::endl;
      output.close();
      }
    }
}
