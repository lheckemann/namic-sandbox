
#include "vtkMRMLTransformRecorderNode.h"


#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#include "vtkIntArray.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLLinearTransformNode.h"



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



void
vtkMRMLTransformRecorderNode
::UpdateFileFromBuffer()
{
  std::ofstream output( this->LogFileName.c_str() );
  
  if ( ! output.is_open() )
    {
    vtkErrorMacro( "Record file could not be opened!" );
    return;
    }
  
  output << "<TransformRecorderLog>" << std::endl;
  
  
    // Save transforms.
  
  for ( unsigned int i = 0; i < this->TransformsBuffer.size(); ++ i )
    {
    output << "  <log";
    output << " TimeStampSec=\"" << this->TransformsBuffer[ i ].TimeStampSec << "\"";
    output << " TimeStampNSec=\"" << this->TransformsBuffer[ i ].TimeStampNSec << "\"";
    output << " type=\"transform\"";
    output << " DeviceName=\"" << this->TransformsBuffer[ i ].DeviceName << "\"";
    output << " transform=\"" << this->TransformsBuffer[ i ].Transform << "\"";
    output << " />" << std::endl;
    }
  
  
    // Save messages.
  
  for ( unsigned int i = 0; i < this->MessagesBuffer.size(); ++ i )
    {
    output << "  <log";
    output << " TimeStampSec=\"" << this->MessagesBuffer[ i ].TimeStampSec << "\"";
    output << " TimeStampNSec=\"" << this->MessagesBuffer[ i ].TimeStampNSec << "\"";
    output << " type=\"message\"";
    output << " message=\"" << this->MessagesBuffer[ i ].Message << "\"";
    output << " />" << std::endl;
    }
  
  
  output << "</TransformRecorderLog>" << std::endl;
  output.close();
  
  this->ClearBuffer();
}


void
vtkMRMLTransformRecorderNode
::ClearBuffer()
{
  this->TransformsBuffer.clear();
  this->MessagesBuffer.clear();
}



vtkMRMLTransformRecorderNode
::vtkMRMLTransformRecorderNode()
{
  this->SetHideFromEditors( false );
  this->ObservedTransformNodeID = NULL;
  this->ObservedTransformNode   = NULL;
  
  this->ObservedConnectorNodeID = NULL;
  this->ObservedConnectorNode   = NULL;
  
  this->Recording = false;
  
  this->LogFileName = "";
  
  this->SetSaveWithScene( true );
  // this->SetAddToScene( true );
  this->SetModifiedSinceRead( true );
  
  // Initialize zero time point.
  this->Clock0 = clock();
  this->Time0  = time( NULL );
}



vtkMRMLTransformRecorderNode
::~vtkMRMLTransformRecorderNode()
{
  this->RemoveMRMLObservers();
  
  this->SetAndObserveObservedConnectorNodeID( NULL );
  
  for ( int i = 0; i < this->ObservedTransformNodes.size(); ++ i )
    {
    vtkSetAndObserveMRMLObjectMacro( this->ObservedTransformNodes[ i ], NULL );
    }
}



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



void
vtkMRMLTransformRecorderNode
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
    attName  = *(atts++);
    attValue = *(atts++);
    
    // todo: Handle observed transform nodes and connector node.
    
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
  
  for ( unsigned int i = 0; i < this->ObservedTransformNodes.size(); ++ i )
    {
    this->ObservedTransformNodes[ i ]->RemoveObservers(
        vtkMRMLTransformNode::TransformModifiedEvent );
    }
  
  this->SetRecording( node->GetRecording() );
  this->SetLogFileName( node->GetLogFileName() );
}


//----------------------------------------------------------------------------
void vtkMRMLTransformRecorderNode::UpdateReferences()
{
  Superclass::UpdateReferences();
  
    // MRML node ID's should be checked. If Scene->GetNodeByID( id ) returns NULL,
    // the reference should be deleted (set to NULL).
}



void
vtkMRMLTransformRecorderNode
::UpdateReferenceID( const char *oldID, const char *newID )
{
  Superclass::UpdateReferenceID( oldID, newID );
  
  /*
  if (    this->ObservedTransformNodeID
       && ! strcmp( oldID, this->ObservedTransformNodeID ) )
    {
    this->SetAndObserveObservedTransformNodeID( newID );
    }
  */
}



void
vtkMRMLTransformRecorderNode
::UpdateScene( vtkMRMLScene *scene )
{
   Superclass::UpdateScene( scene );
   
   // this->SetAndObserveObservedTransformNodeID( this->ObservedTransformNodeID );
}



void
vtkMRMLTransformRecorderNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "ObservedTransformNodeID: " <<
    (this->ObservedTransformNodeID? this->ObservedTransformNodeID: "(none)") << "\n";
  os << indent << "LogFileName: " << this->LogFileName << "\n";
}



void
vtkMRMLTransformRecorderNode
::SetAndObserveObservedConnectorNodeID( const char* ConnectorNodeRef )
{
  vtkSetAndObserveMRMLObjectMacro( this->ObservedConnectorNode, NULL );
  this->SetObservedConnectorNodeID( ConnectorNodeRef );
  vtkMRMLIGTLConnectorNode* cnode = this->GetObservedConnectorNode();
  vtkSetAndObserveMRMLObjectMacro( this->ObservedConnectorNode, cnode );
  if ( cnode )
    {
    cnode->AddObserver( vtkMRMLIGTLConnectorNode::ReceiveEvent, (vtkCommand*)this->MRMLCallbackCommand );
    }
}



vtkMRMLIGTLConnectorNode*
vtkMRMLTransformRecorderNode
::GetObservedConnectorNode()
{
  vtkMRMLIGTLConnectorNode* node = NULL;
  if (    this->GetScene()
       && this->ObservedConnectorNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ObservedConnectorNodeID );
    node = vtkMRMLIGTLConnectorNode::SafeDownCast( snode );
    }
  return node;
}



void
vtkMRMLTransformRecorderNode
::ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData )
{
  
  
    // Handle IGT Connector events. ----------------------------------------
  
  if ( this->ObservedConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
       && event == vtkMRMLIGTLConnectorNode::ReceiveEvent )
    {
    int numIncomingNodes = this->ObservedConnectorNode->GetNumberOfIncomingMRMLNodes();
    std::vector< vtkMRMLLinearTransformNode* > transformNodes;
    for ( int nodeIndex = 0; nodeIndex < numIncomingNodes; ++ nodeIndex )
      {
      vtkMRMLNode* node = this->ObservedConnectorNode->GetIncomingMRMLNode( nodeIndex );
      vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
      if ( transformNode != NULL )
        {
        transformNodes.push_back( transformNode );
        }
      }
    
      // Update the vector of observed transform nodes.
    
    bool updateTransforms = false;
    if ( this->ObservedTransformNodes.size() != transformNodes.size() )
      {
      updateTransforms = true;
      }
    else
      {
      for ( unsigned int i = 0; i < transformNodes.size(); ++ i )
        {
        if ( strcmp( transformNodes[ i ]->GetID(), this->ObservedTransformNodes[ i ]->GetID() ) != 0 )
          {
          updateTransforms = true;
          }
        }
      }
    
        // Remove existing observers.
        // Add new observers.
      
    if ( updateTransforms )
      {
      
      for ( unsigned int i = 0; i < this->ObservedTransformNodes.size(); ++ i )
        {
        this->ObservedTransformNodes[ i ]->RemoveObservers(
            vtkMRMLTransformNode::TransformModifiedEvent );
        vtkSetAndObserveMRMLObjectMacro( this->ObservedTransformNodes[ i ], NULL );
        }
      
      this->ObservedTransformNodes.clear();
      
      for ( unsigned int i = 0; i < transformNodes.size(); ++ i )
        {
        this->ObservedTransformNodes.push_back( NULL );
        vtkSetAndObserveMRMLObjectMacro( this->ObservedTransformNodes[ i ], transformNodes[ i ] );
        this->ObservedTransformNodes[ i ]->AddObserver( vtkMRMLTransformNode::TransformModifiedEvent,
                                                        (vtkCommand*)this->MRMLCallbackCommand );
        }
      }
    }
  
    // -----------------------------------------------------------------------
  
  
    // Check if we have a valid list of which transform should be recorded.
    // If we don't, record every transform.
  
  bool recordAll = false;
  if ( this->TransformSelections.size() != this->ObservedTransformNodes.size() )
    {
    recordAll = true;
    }
  
  
    // Handle modified event of any observed transform node. ------------------------
  
  for ( unsigned int tIndex = 0; tIndex < this->ObservedTransformNodes.size(); ++ tIndex )
    {
    if ( this->ObservedTransformNodes[ tIndex ] == vtkMRMLTransformNode::SafeDownCast( caller )
         && event == vtkMRMLTransformNode::TransformModifiedEvent )
      {
      int sec = 0;
      int nsec = 0;
      vtkSmartPointer< vtkMatrix4x4 > m = vtkSmartPointer< vtkMatrix4x4 >::New();
      std::string deviceName;
      
      this->ObservedConnectorNode->LockIncomingMRMLNode( this->ObservedTransformNodes[ tIndex ] );
      this->ObservedConnectorNode->GetIGTLTimeStamp( this->ObservedTransformNodes[ tIndex ], sec, nsec );
      vtkMRMLLinearTransformNode* ltn = vtkMRMLLinearTransformNode::SafeDownCast(
        this->ObservedTransformNodes[ tIndex ] );
      if ( ltn != NULL )
        {
        m->DeepCopy( ltn->GetMatrixTransformToParent() );
        }
      else
        {
        vtkErrorMacro( "Non linear transform received from IGT connector!" );
        }
      deviceName = std::string( this->ObservedTransformNodes[ tIndex ]->GetName() );
      this->ObservedConnectorNode->UnlockIncomingMRMLNode( this->ObservedTransformNodes[ tIndex ] );
      
      int record = 1;
      if ( ! recordAll )
        {
        record = this->TransformSelections[ tIndex ];
        }
      
      if ( record )
        {
        std::stringstream mss;
        for ( int row = 0; row < 4; ++ row )
          {
          for ( int col = 0; col < 4; ++ col )
            {
            mss << m->GetElement( row, col ) << " ";
            }
          }
        
        if ( sec == 0 && nsec == 0 )
          {
          clock_t clock1 = clock();
          // double seconds = this->Time0 + double( clock1 - this->Clock0 ) / CLOCKS_PER_SEC;
          double seconds = double( clock1 - this->Clock0 ) / CLOCKS_PER_SEC;
          sec = floor( seconds );
          nsec = ( seconds - sec ) * 1e9;
          }
        else if ( sec > this->Time0 )
          {
          sec -= this->Time0;
          }
        
        TransformRecord rec;
          rec.DeviceName = deviceName;
          rec.TimeStampSec = sec;
          rec.TimeStampNSec = nsec;
          rec.Transform = mss.str();
        
        if ( this->Recording )
          {
          this->TransformsBuffer.push_back( rec );
          this->InvokeEvent( this->TransformChangedEvent, NULL );
          }
        
        } // if ( record )
      }
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
  
  if ( this->ObservedConnectorNode )
    {
    this->ObservedConnectorNode->RemoveObservers( vtkMRMLIGTLConnectorNode::ReceiveEvent );
    }
  
  
  for ( int i = 0; i < this->ObservedTransformNodes.size(); ++ i )
    {
    this->ObservedTransformNodes[ i ]->RemoveObservers( vtkMRMLTransformNode::TransformModifiedEvent );
    }
}



/**
 * @param selections Should contain as many elements as the number of incoming
 *        transforms throught the active connector. Order follows the order in
 *        the connector node. 0 means transform is not tracked, 1 means it's tracked.
 */
void
vtkMRMLTransformRecorderNode
::SetTransformSelections( std::vector< int > selections )
{
  this->TransformSelections.clear();
  
  for ( int i = 0; i < selections.size(); ++ i )
    {
    this->TransformSelections.push_back( selections[ i ] );
    }
}



void
vtkMRMLTransformRecorderNode
::SetLogFileName( std::string fileName )
{
  this->LogFileName = fileName;
}



void
vtkMRMLTransformRecorderNode
::SaveIntoFile( std::string fileName )
{
  this->LogFileName = fileName;
  this->UpdateFileFromBuffer();
}



std::string
vtkMRMLTransformRecorderNode
::GetLogFileName()
{
  return this->LogFileName;
}



void
vtkMRMLTransformRecorderNode
::CustomMessage( std::string message )
{
  clock_t clock1 = clock();
  double seconds = double( clock1 - this->Clock0 ) / CLOCKS_PER_SEC;
  // double seconds = this->Time0 + double( clock1 - this->Clock0 ) / CLOCKS_PER_SEC;
  
  long int sec = floor( seconds );
  int nsec = ( seconds - sec ) * 1e9;
  
  MessageRecord rec;
    rec.Message = message;
    rec.TimeStampSec = sec;
    rec.TimeStampNSec = nsec;
  
  this->MessagesBuffer.push_back( rec );
}



unsigned int
vtkMRMLTransformRecorderNode
::GetTransformsBufferSize()
{
  return this->TransformsBuffer.size();
}



unsigned int
vtkMRMLTransformRecorderNode
::GetMessagesBufferSize()
{
  return this->MessagesBuffer.size();
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

