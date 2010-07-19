
#include "vtkMRMLTransformRecorderNode.h"

#include <ctime>
#include <fstream>
#include <sstream>

#include "vtkIntArray.h"
#include "vtkSmartPointer.h"


// Helper functions.
// ----------------------------------------------------------------------------

bool
StringToBool( std::string str )
{
  bool var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}

// ----------------------------------------------------------------------------



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



vtkMRMLTransformRecorderNode
::vtkMRMLTransformRecorderNode()
{
  this->SetHideFromEditors( false );
  this->ObservedTransformNodeID = NULL;
  this->ObservedTransformNode = NULL;
  
  this->Recording = false;
  
  this->LogFileName = "";
  
  this->SetSaveWithScene( true );
  // this->SetAddToScene( true );
  this->SetModifiedSinceRead( true );
}



vtkMRMLTransformRecorderNode::~vtkMRMLTransformRecorderNode()
{
  this->RemoveMRMLObservers();
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
  
  of << indent << " Recording=\"" << this->Recording << "\"";
  of << indent << " LogFileName=\"" << this->LogFileName << "\"";
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
    
    if ( ! strcmp( attName, "Recording" ) )
      {
      this->SetRecording( StringToBool( std::string( attValue ) ) );
      }
    
    if ( ! strcmp( attName, "LogFileName" ) )
      {
      this->SetLogFileName( attValue );
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
  this->SetRecording( node->GetRecording() );
  this->SetLogFileName( node->GetLogFileName() );
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
  os << indent << "LogFileName: " << this->LogFileName << "\n";
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
  if ( tnode )
    {
    tnode->AddObserver( vtkMRMLTransformNode::TransformModifiedEvent, (vtkCommand*)this->MRMLCallbackCommand );
    }
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
  if (
       this->ObservedTransformNode == vtkMRMLTransformNode::SafeDownCast( caller )
       && event == vtkMRMLTransformNode::TransformModifiedEvent
     )
    {
    vtkMatrix4x4* mtr = vtkMatrix4x4::New();
    this->ObservedTransformNode->GetMatrixTransformToWorld( mtr );
    mtr->Delete();
    if ( this->Recording && this->LogFileName.size() > 1 )
      {
      this->WriteLog();
      }
    this->InvokeEvent( this->TransformChangedEvent, NULL );
    }
}


void
vtkMRMLTransformRecorderNode
::RemoveMRMLObservers()
{
  if ( this->ObservedTransformNode )
    {
    this->ObservedTransformNode->RemoveObservers( vtkMRMLTransformNode::TransformModifiedEvent );
    }
}


void
vtkMRMLTransformRecorderNode
::SetLogFileName( std::string fileName )
{
  this->LogFileName = fileName;
}


std::string
vtkMRMLTransformRecorderNode
::GetLogFileName()
{
  return this->LogFileName;
}


void
vtkMRMLTransformRecorderNode
::WriteLog()
{
  double seconds = clock() * 1.0 / CLOCKS_PER_SEC;
  
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    matrix->Identity();
  this->ObservedTransformNode->GetMatrixTransformToWorld( matrix );
  
  std::stringstream ss;
  ss << "<log ";
  ss << "time=\"" << seconds << "\" type=\"transform\" transform=\"";
  ss << matrix->GetElement( 0, 0 ) << " " << matrix->GetElement( 0, 1 ) << " " << matrix->GetElement( 0, 2 ) << " " << matrix->GetElement( 0, 3 ) << " ";
  ss << matrix->GetElement( 1, 0 ) << " " << matrix->GetElement( 1, 1 ) << " " << matrix->GetElement( 1, 2 ) << " " << matrix->GetElement( 1, 3 ) << " ";
  ss << matrix->GetElement( 2, 0 ) << " " << matrix->GetElement( 2, 1 ) << " " << matrix->GetElement( 2, 2 ) << " " << matrix->GetElement( 2, 3 ) << " ";
  ss << matrix->GetElement( 3, 0 ) << " " << matrix->GetElement( 3, 1 ) << " " << matrix->GetElement( 3, 2 ) << " " << matrix->GetElement( 3, 3 ) << " ";
  ss << "\"";
  ss << " />";
  
  const char* cs = ss.str().c_str();
  
  std::ofstream output( this->LogFileName.c_str(), std::ios_base::app );
  output << ss.str();
  output << std::endl;
  output.close();
}


void
vtkMRMLTransformRecorderNode
::CustomMessage( std::string message )
{
  double seconds = clock() * 1.0 / CLOCKS_PER_SEC;
  
  std::ofstream output( this->LogFileName.c_str(), std::ios_base::app );
  output << "<log ";
  output << "time=\"" << seconds << "\" type=\"message\" message=\"" << message;
  output << "\" >";
  output << std::endl;
  output.close();
}


void
vtkMRMLTransformRecorderNode
::SetRecording( bool newState )
{
  this->Recording = newState;
  if ( this->Recording )
    {
    this->InvokeEvent( this->RecordingStartEvent, NULL );
    }
  else
    {
    this->InvokeEvent( this->RecordingStopEvent, NULL );
    }
}

