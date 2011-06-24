#include "vtkMRMLLiveUltrasoundNode.h"

#include "vtkActor.h"
#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSTLReader.h"
#include "vtkTransform.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkIGTLToMRMLImage.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLSliceNode.h"



//**************** Debug: Profiling ******************** 
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
std::string TimeString( const char* msg )
{
    clock_t mytime = clock();
    std::stringstream ss;
    ss << double( mytime ) / CLOCKS_PER_SEC << "  " << msg;

    ofstream output;
    output.open( "_vtkMRMLLiveUltrasoundNode.txt", std::ios_base::app );
    output << ss.str() << std::endl;
    output.close();

    return ss.str();
}
//**************** Debug: Profiling ******************** 


// MACROS ---------------------------------------------------------------------

#define DELETE_IF_NOT_NULL(x) \
    if ( x != NULL ) \
{ \
    x->Delete(); \
    x = NULL; \
}


//------------------------------------------------------------------------------
vtkMRMLLiveUltrasoundNode* vtkMRMLLiveUltrasoundNode::New()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLiveUltrasoundNode" );
    if( ret )
    {
        return ( vtkMRMLLiveUltrasoundNode* )ret;
    }
    // If the factory was unable to create the object, then create it here.
    return new vtkMRMLLiveUltrasoundNode;
}


//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLLiveUltrasoundNode::CreateNodeInstance()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkMRMLLiveUltrasoundNode" );
    if( ret )
    {
        return ( vtkMRMLLiveUltrasoundNode* )ret;
    }
    // If the factory was unable to create the object, then create it here.
    return new vtkMRMLLiveUltrasoundNode;
}

//------------------------------------------------------------------------------
vtkMRMLLiveUltrasoundNode::vtkMRMLLiveUltrasoundNode()
{
    this->HideFromEditorsOff();
    this->SetSaveWithScene( true );
    // this->SetAddToScene( true );
    this->SetModifiedSinceRead( true );

    this->ConnectorNodeID = NULL;
    this->ConnectorNode = NULL;

    this->ModelToProbeTransform = vtkTransform::New();
    this->ModelToProbeTransform->Identity();
    this->ModelToProbeTransformNode = NULL;
    this->ModelToProbeTransformNodeID = NULL;

    this->ProbeModelToTrackerTransform = vtkTransform::New(); 
    this->ProbeModelToTrackerTransform->Identity(); 
    this->ProbeModelToTrackerTransformNode = NULL; 
    this->ProbeModelToTrackerNodeID = NULL;

    this->ImageToProbeTransform = vtkTransform::New();
    this->ImageToProbeTransform->Identity();
    this->ImageToProbeTransformNode = NULL;
    this->ImageToProbeTransformNodeID = NULL;

    this->LastTimeSec = 0;
    this->LastTimeNSec = 0;

    this->ProbeToTrackerTransform = vtkTransform::New();
    this->ProbeToTrackerTransform->Identity();

    // Create image to tracker transform
    this->ImageToTrackerTransform = vtkTransform::New();
    this->ImageToTrackerTransform->Identity(); 
    this->ImageToTrackerTransform->Concatenate( this->ProbeToTrackerTransform ); 
    this->ImageToTrackerTransform->Concatenate( this->ImageToProbeTransform ); 
    this->ImageToTrackerTransform->Update(); 

    this->LastImageData = vtkImageData::New();
    
    this->LiveImageActor = vtkImageActor::New();
    this->LiveImageActor->SetInput(this->LastImageData); 
    this->LiveImageActor->SetScale( 0.2, 0.2, 1.0 );
    this->LiveImageActor->Update(); 
                        
    // Apply probe to tracker transform to probe actor and image actor
    this->LiveImageActor->SetUserTransform(this->ImageToTrackerTransform); 

}

//------------------------------------------------------------------------------
vtkMRMLLiveUltrasoundNode::~vtkMRMLLiveUltrasoundNode()
{
    this->RemoveMRMLObservers();

    this->SetProbeModelToTrackerNodeID( NULL );
    this->SetModelToProbeTransformNodeID( NULL ); 
    this->SetConnectorNodeID( NULL ); 

    this->SetAndObserveConnectorNodeID( NULL );
    this->SetAndObserveImageToProbeTransformNodeID( NULL );
    this->SetAndObserveModelToProbeTransformNodeID( NULL );
    this->SetAndObserveProbeModelToTrackerTransformNodeID( NULL ); 

    this->SetProbeToTrackerTransform( NULL ); 
    this->SetProbeModelToTrackerTransform( NULL ); 
    this->SetModelToProbeTransform( NULL ); 
    this->SetImageToProbeTransform( NULL );
    this->SetImageToTrackerTransform( NULL ); 

    if ( this->LastImageData != NULL )
    {
        this->LastImageData->Delete(); 
        this->LastImageData = NULL; 
    }

    if ( this->LiveImageActor != NULL )
    {
        this->LiveImageActor->Delete(); 
        this->LiveImageActor = NULL; 
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::WriteXML( ostream& of, int nIndent )
{
    Superclass::WriteXML( of, nIndent );

    vtkIndent indent( nIndent );

    /*if ( this->ConnectorNodeID != NULL )
    {
        of << indent << " ConnectorNodeID=\"" << this->ConnectorNodeID << "\"\n";
    }*/

    if ( this->ProbeModelToTrackerNodeID != NULL )
    {
        of << indent << " ProbeModelToTrackerNodeID=\"" << this->ProbeModelToTrackerNodeID << "\"\n";
    }

    if ( this->ImageToProbeTransformNodeID != NULL )
    {
        of << indent << " ImageToProbeTransformNodeID=\"" << this->ImageToProbeTransformNodeID << "\"";
    }

    if ( this->ModelToProbeTransformNodeID != NULL )
    {
        of << indent << " ModelToProbeTransformNodeID=\"" << this->ModelToProbeTransformNodeID << "\"";
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::ReadXMLAttributes( const char** atts )
{
    Superclass::ReadXMLAttributes( atts );

    // Read all MRML node attributes from two arrays of names and values
    const char* attName;
    const char* attValue;

    while (*atts != NULL)
    {
        attName  = *(atts++);
        attValue = *(atts++);

        if ( !strcmp( attName, "ConnectorNodeID" ) )
        {
            this->SetAndObserveConnectorNodeID( NULL );
            this->SetConnectorNodeID( attValue );
        }

        if ( !strcmp( attName, "ProbeModelToTrackerNodeID" ) )
        {
            this->SetProbeModelToTrackerNodeID( attValue );
        }

        if ( !strcmp( attName, "ImageToProbeTransformNodeID" ) )
        {
            this->SetAndObserveImageToProbeTransformNodeID( NULL );
            this->SetImageToProbeTransformNodeID( attValue );
        }

        if ( !strcmp( attName, "ModelToProbeTransformNodeID" ) )
        {
            this->SetAndObserveModelToProbeTransformNodeID( NULL );
            this->SetModelToProbeTransformNodeID( attValue );
        }
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::Copy( vtkMRMLNode *anode )
{  
    Superclass::Copy( anode );
    vtkMRMLLiveUltrasoundNode *node = ( vtkMRMLLiveUltrasoundNode* ) anode;

    //this->SetAndObserveConnectorNodeID( NULL );
    this->SetConnectorNodeID( node->ConnectorNodeID );
    
    this->SetAndObserveProbeModelToTrackerTransformNodeID( NULL ); 
    this->SetProbeModelToTrackerNodeID( node->ProbeModelToTrackerNodeID );

    this->SetAndObserveImageToProbeTransformNodeID( NULL );
    this->SetImageToProbeTransformNodeID( node->ImageToProbeTransformNodeID );

    this->SetAndObserveModelToProbeTransformNodeID( NULL );
    this->SetModelToProbeTransformNodeID( node->ModelToProbeTransformNodeID );
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::UpdateReferences()
{
    Superclass::UpdateReferences();

    if ( this->ConnectorNodeID != NULL && this->Scene->GetNodeByID( this->ConnectorNodeID ) == NULL )
    {
        this->SetConnectorNodeID( NULL );
    }

    if ( this->ProbeModelToTrackerNodeID != NULL && this->Scene->GetNodeByID( this->ProbeModelToTrackerNodeID ) == NULL )
    {
        this->SetProbeModelToTrackerNodeID( NULL );
    }

    if ( this->ImageToProbeTransformNodeID != NULL && this->Scene->GetNodeByID( this->ImageToProbeTransformNodeID ) == NULL )
    {
        this->SetAndObserveImageToProbeTransformNodeID( NULL );
    }

    if ( this->ModelToProbeTransformNodeID != NULL && this->Scene->GetNodeByID( this->ModelToProbeTransformNodeID ) == NULL )
    {
        this->SetAndObserveModelToProbeTransformNodeID( NULL );
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::UpdateReferenceID( const char *oldID, const char *newID )
{
    Superclass::UpdateReferenceID( oldID, newID );

    if ( this->ConnectorNodeID && !strcmp( oldID, this->ConnectorNodeID ) )
    {
        this->SetAndObserveConnectorNodeID( newID );
    }

    if ( this->ProbeModelToTrackerNodeID && !strcmp( oldID, this->ProbeModelToTrackerNodeID ) )
    {
        this->SetProbeModelToTrackerNodeID( newID );
    }

    if ( this->ImageToProbeTransformNodeID && !strcmp( oldID, this->ImageToProbeTransformNodeID ) )
    {
        this->SetAndObserveImageToProbeTransformNodeID( newID );
    }

    if ( this->ModelToProbeTransformNodeID && !strcmp( oldID, this->ModelToProbeTransformNodeID ) )
    {
        this->SetAndObserveModelToProbeTransformNodeID( newID );
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::UpdateScene( vtkMRMLScene *scene )
{
    Superclass::UpdateScene( scene );

    this->SetAndObserveConnectorNodeID( this->ConnectorNodeID );
    this->SetAndObserveImageToProbeTransformNodeID( this->ImageToProbeTransformNodeID );
    this->SetAndObserveModelToProbeTransformNodeID( this->ModelToProbeTransformNodeID );
    this->SetAndObserveProbeModelToTrackerTransformNodeID( this->ProbeModelToTrackerNodeID ); 
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::PrintSelf( ostream& os, vtkIndent indent )
{
    vtkMRMLNode::PrintSelf(os,indent);

    os << indent << "ConnectorNodeID: " << ( this->ConnectorNodeID ? this->ConnectorNodeID : "(none)" ) << "\n";
    os << indent << "ProbeModelToTrackerNodeID: " << (this->ProbeModelToTrackerNodeID ? this->ProbeModelToTrackerNodeID : "(none)" ) << "\n";
    os << indent << "ImageToProbeTransformNodeID: " << (this->ImageToProbeTransformNodeID ? this->ImageToProbeTransformNodeID : "(none)") << "\n";
    os << indent << "ModelToProbeTransformNodeID: " << (this->ModelToProbeTransformNodeID ? this->ModelToProbeTransformNodeID : "(none)") << "\n";
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::StartOpenIGTLinkIFServer()
{
    if ( !this->GetConnectorNode() )
    {
        vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::New();
        this->GetScene()->AddNode(connector);
        this->SetAndObserveConnectorNodeID( connector->GetID() ); 
        connector->Delete();
    }

    vtkMRMLIGTLConnectorNode* connector = this->GetConnectorNode();
    if ( connector && connector->GetState() == vtkMRMLIGTLConnectorNode::STATE_OFF )
    {
        vtkIGTLToMRMLImage* imageConverter = vtkIGTLToMRMLImage::New();
        connector->RegisterMessageConverter(imageConverter); 
        connector->SetType(vtkMRMLIGTLConnectorNode::TYPE_SERVER);
        connector->Start();
        connector->Modified();
    }
}


//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::StopOpenIGTLinkIFServer()
{
     vtkMRMLIGTLConnectorNode* connector = this->GetConnectorNode();
    if ( connector && connector->GetState() != vtkMRMLIGTLConnectorNode::STATE_OFF )
    {
        connector->Stop();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::CheckIncomingOpenIGTLinkData()
{
    vtkMRMLIGTLConnectorNode* cnode = this->GetConnectorNode(); 
    if ( cnode )
    {
        cnode->ImportEventsFromEventBuffer();
        cnode->ImportDataFromCircularBuffer(); 
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::SetAndObserveConnectorNodeID( const char* ConnectorNodeRef )
{
    vtkSetAndObserveMRMLObjectMacro( this->ConnectorNode, NULL );
    if ( ConnectorNodeRef != NULL )
    {
        this->SetConnectorNodeID( ConnectorNodeRef );
        vtkMRMLIGTLConnectorNode* cnode = this->GetConnectorNode();
        vtkSetAndObserveMRMLObjectMacro( this->ConnectorNode, cnode );
        if ( cnode )
        {
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::ConnectedEvent, (vtkCommand*)this->MRMLCallbackCommand );
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::ReceiveEvent, (vtkCommand*)this->MRMLCallbackCommand );
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::DisconnectedEvent, (vtkCommand*)this->MRMLCallbackCommand );
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::ActivatedEvent, (vtkCommand*)this->MRMLCallbackCommand );
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::DeactivatedEvent, (vtkCommand*)this->MRMLCallbackCommand );
        }
    }
}

//------------------------------------------------------------------------------
vtkMRMLIGTLConnectorNode* vtkMRMLLiveUltrasoundNode::GetConnectorNode()
{
    vtkMRMLIGTLConnectorNode* node = NULL;
    if (    this->GetScene()
        && this->ConnectorNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ConnectorNodeID );
        node = vtkMRMLIGTLConnectorNode::SafeDownCast( snode );
    }
    return node;
}

//------------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLLiveUltrasoundNode::GetProbeModelToTrackerTransformNode()
{
    vtkMRMLLinearTransformNode* node = NULL;
    if (    this->GetScene()
        && this->ProbeModelToTrackerNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ProbeModelToTrackerNodeID);
        node = vtkMRMLLinearTransformNode::SafeDownCast( snode );
    }
    return node;
}

//------------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLLiveUltrasoundNode::GetImageToProbeTransformNode()
{
    vtkMRMLLinearTransformNode* node = NULL;
    if (    this->GetScene()
        && this->ImageToProbeTransformNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ImageToProbeTransformNodeID );
        node = vtkMRMLLinearTransformNode::SafeDownCast( snode );
    }
    return node;
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::SetAndObserveProbeModelToTrackerTransformNodeID( const char* transformNodeRef )
{
    vtkSetAndObserveMRMLObjectMacro( this->ProbeModelToTrackerTransformNode, NULL );
    if ( transformNodeRef != NULL )
    {
        this->SetProbeModelToTrackerNodeID( transformNodeRef );
        vtkMRMLLinearTransformNode* tnode = this->GetProbeModelToTrackerTransformNode();

        vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
        events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
        vtkSetAndObserveMRMLObjectEventsMacro( this->ProbeModelToTrackerTransformNode, tnode, events );

        if ( tnode != NULL )
        {
            this->ProbeModelToTrackerTransform->SetMatrix( tnode->GetMatrixTransformToParent() );
            this->ProbeModelToTrackerTransform->Update();
        }
    }
}


//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::SetAndObserveImageToProbeTransformNodeID( const char* transformNodeRef )
{
    vtkSetAndObserveMRMLObjectMacro( this->ImageToProbeTransformNode, NULL );
    if ( transformNodeRef != NULL )
    {
        this->SetImageToProbeTransformNodeID( transformNodeRef );
        vtkMRMLLinearTransformNode* tnode = this->GetImageToProbeTransformNode();

        vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
        events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
        vtkSetAndObserveMRMLObjectEventsMacro( this->ImageToProbeTransformNode, tnode, events );

        if ( tnode != NULL )
        {
            this->ImageToProbeTransform->SetMatrix( tnode->GetMatrixTransformToParent() );
            this->ImageToProbeTransform->Update();
        }
    }
}

//------------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLLiveUltrasoundNode::GetModelToProbeTransformNode()
{
    vtkMRMLLinearTransformNode* node = NULL;
    if (    this->GetScene()
        && this->ModelToProbeTransformNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ModelToProbeTransformNodeID );
        node = vtkMRMLLinearTransformNode::SafeDownCast( snode );
    }
    return node;
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::SetAndObserveModelToProbeTransformNodeID( const char* transformNodeRef )
{
    vtkSetAndObserveMRMLObjectMacro( this->ModelToProbeTransformNode, NULL );
    this->SetModelToProbeTransformNodeID( transformNodeRef );
    vtkMRMLLinearTransformNode* tnode = this->GetModelToProbeTransformNode();

    vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
    events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
    vtkSetAndObserveMRMLObjectEventsMacro( this->ModelToProbeTransformNode, tnode, events );

    if ( tnode != NULL )
    {
        this->ModelToProbeTransform->SetMatrix( tnode->GetMatrixTransformToParent() );
        this->ModelToProbeTransform->Update();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData )
{
    Superclass::ProcessMRMLEvents( caller, event, callData );

    if (    this->ConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
        && event == vtkMRMLIGTLConnectorNode::ConnectedEvent )
    {
        vtkDebugMacro ("**** OpenIGTLinkIF client connected...");

    }
    else if (    this->ConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
        && event == vtkMRMLIGTLConnectorNode::DisconnectedEvent )
    {
        vtkDebugMacro ("**** OpenIGTLinkIF client disconnected...");

    }
    else if (    this->ConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
        && event == vtkMRMLIGTLConnectorNode::ActivatedEvent )
    {
        vtkDebugMacro ("**** OpenIGTLinkIF server activated...");

    } 
    else if (    this->ConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
        && event == vtkMRMLIGTLConnectorNode::DeactivatedEvent )
    {
        vtkDebugMacro ("**** OpenIGTLinkIF server deactivated...");

    }
    else if (    this->ConnectorNode == vtkMRMLIGTLConnectorNode::SafeDownCast( caller )
        && event == vtkMRMLIGTLConnectorNode::ReceiveEvent )
    {
        vtkDebugMacro ("**** OpenIGTLinkIF image data received...");

        int numIncomingNodes = this->ConnectorNode->GetNumberOfIncomingMRMLNodes();
        for ( int nodeIndex = 0; nodeIndex < numIncomingNodes; ++ nodeIndex )
        {
            int sec(-1), nsec(-1);

            vtkMRMLNode* node = this->ConnectorNode->GetIncomingMRMLNode( nodeIndex );

            vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast( node );
            if ( volumeNode )
            {
                this->ConnectorNode->LockIncomingMRMLNode( node );
                this->ConnectorNode->GetIGTLTimeStamp( node, sec, nsec );
                this->ConnectorNode->UnlockIncomingMRMLNode( node );

                if ( this->LastTimeSec != sec && this->LastTimeNSec != nsec )
                {
                    vtkSmartPointer< vtkMatrix4x4 > mat = vtkSmartPointer< vtkMatrix4x4 >::New();
                    volumeNode->GetIJKToRASMatrix( mat );
                    this->ProbeToTrackerTransform->SetMatrix( mat ); 
                    this->ProbeToTrackerTransform->Modified(); 

                    this->ProbeModelToTrackerTransform->Identity(); 
                    this->ProbeModelToTrackerTransform->Concatenate( this->ProbeToTrackerTransform ); 
                    this->ProbeModelToTrackerTransform->Concatenate( this->ModelToProbeTransform ); 
                    this->GetProbeModelToTrackerTransformNode()->GetMatrixTransformToParent()->DeepCopy( this->ProbeModelToTrackerTransform->GetMatrix() ); 
                    this->GetProbeModelToTrackerTransformNode()->Modified();

                    this->LastTimeSec = sec;
                    this->LastTimeNSec = nsec;
                    this->LastImageData->DeepCopy( volumeNode->GetImageData() );
                    this->LastImageData->Modified(); 
                    this->LiveImageActor->Update();
                    this->InvokeEvent(NewFrameArrivedEvent); 
                }
            }

            vtkMRMLLinearTransformNode* tNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
            if ( tNode != NULL )
            {
                // vtkSmartPointer< vtkMatrix4x4 > mat = vtkSmartPointer< vtkMatrix4x4 >::New();
                // tNode->GetMatrixTransformToWorld( mat );
                // this->UpdateLiveModels( NULL, mat );
            }
        }
    }
    else if (    this->ModelToProbeTransformNode == vtkMRMLLinearTransformNode::SafeDownCast( caller )
        && event == vtkMRMLLinearTransformNode::TransformModifiedEvent )
    {
        this->ModelToProbeTransform->SetMatrix( this->ModelToProbeTransformNode->GetMatrixTransformToParent() );
        this->ModelToProbeTransform->Update();
    }
    else if (    this->ImageToProbeTransformNode == vtkMRMLLinearTransformNode::SafeDownCast( caller )
        && event == vtkMRMLLinearTransformNode::TransformModifiedEvent )
    {
        this->ImageToProbeTransform->SetMatrix( this->ImageToProbeTransformNode->GetMatrixTransformToParent() );
        this->ImageToProbeTransform->Update();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::RemoveMRMLObservers()
{
    if ( this->ConnectorNode )
    {
        this->ConnectorNode->RemoveObservers( vtkMRMLIGTLConnectorNode::ReceiveEvent );
    }
}

