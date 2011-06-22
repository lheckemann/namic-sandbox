#include "vtkMRMLLiveUltrasoundNode.h"

#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSTLReader.h"
#include "vtkTransform.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLLinearTransformNode.h"
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
    //this->ConnectorNode = NULL;

    this->ProbeModelNodeID = NULL;

    this->ProbeToTrackerTransform = vtkTransform::New(); 
    this->ProbeToTrackerTransform->Identity(); 
    this->ProbeToTrackerTransformNode = NULL; 
    this->ProbeToTrackerNodeID = NULL;

    this->ImageToProbeTransform = vtkTransform::New();
    this->ImageToProbeTransform->Identity();
    this->ImageToProbeTransformNode = NULL;
    this->ImageToProbeTransformNodeID = NULL;

    this->ModelToProbeTransform = vtkTransform::New();
    this->ModelToProbeTransform->Identity();
    this->ModelToProbeTransformNode = NULL;
    this->ModelToProbeTransformNodeID = NULL;

    this->LastTimeSec = 0;
    this->LastTimeNSec = 0;

    this->ViewerRenderer = NULL;

    this->LastTrackerToProbeTransform = vtkTransform::New();
    this->LastTrackerToProbeTransform->Identity();

    this->LastImageData = vtkImageData::New();

    this->ProbeModel     = vtkPolyData::New();
    this->ProbeActor     = vtkActor::New();
    
    this->LiveImageActor = vtkImageActor::New();
}

//------------------------------------------------------------------------------
vtkMRMLLiveUltrasoundNode::~vtkMRMLLiveUltrasoundNode()
{
    this->RemoveMRMLObservers();

    this->SetProbeModelNodeID( NULL );
    this->SetProbeToTrackerNodeID( NULL );
    this->SetModelToProbeTransformNodeID( NULL ); 
    this->SetConnectorNodeID( NULL ); 

    this->SetAndObserveConnectorNodeID( NULL );
    this->SetAndObserveImageToProbeTransformNodeID( NULL );
    this->SetAndObserveModelToProbeTransformNodeID( NULL );
    this->SetAndObserveProbeToTrackerTransformNodeID( NULL ); 

    this->SetProbeToTrackerTransform( NULL ); 
    this->SetModelToProbeTransform( NULL ); 
    this->SetImageToProbeTransform( NULL ); 

    if ( this->ViewerRenderer != NULL )
    {
        // this->ViewerRenderer->RemoveActor( this->ProbeActor );
    }


    if ( this->LastTrackerToProbeTransform != NULL )
    {
        this->LastTrackerToProbeTransform->Delete(); 
        this->LastTrackerToProbeTransform = NULL; 
    }

    if ( this->LastImageData != NULL )
    {
        this->LastImageData->Delete(); 
        this->LastImageData = NULL; 
    }

    if ( this->ProbeModel != NULL )
    {
        this->ProbeModel->Delete(); 
        this->ProbeModel = NULL; 
    }
    
    if ( this->ProbeActor != NULL )
    {
        this->ProbeActor->Delete(); 
        this->ProbeActor = NULL; 
    }

 

    if ( this->LiveImageActor != NULL )
    {
        this->LiveImageActor->Delete(); 
        this->LiveImageActor = NULL; 
    }
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::Init()
{
    //qSlicerCoreApplication::

    //vtkSlicerViewerWidget* viewer = this->SlicerApplication->GetApplicationGUI()->GetActiveViewerWidget();
   // this->ViewerRenderer = viewer->GetMainViewer()->GetRenderer();

    // Place live image actor in the viewer.
    this->ViewerRenderer->AddActor( this->LiveImageActor );
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::StartReceiveServer()
{

}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::StopReceiveServer()
{

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

    if ( this->ProbeModelNodeID != NULL )
    {
        of << indent << " ProbeModelNodeID=\"" << this->ProbeModelNodeID << "\"\n";
    }

    if ( this->ProbeToTrackerNodeID != NULL )
    {
        of << indent << " ProbeToTrackerNodeID=\"" << this->ProbeToTrackerNodeID << "\"\n";
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

        if ( !strcmp( attName, "ProbeModelNodeID" ) )
        {
            this->SetProbeModelNodeID( attValue );
        }

        if ( !strcmp( attName, "ProbeToTrackerNodeID" ) )
        {
            this->SetProbeToTrackerNodeID( attValue );
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

    this->SetProbeModelNodeID( node->ProbeModelNodeID );
    
    this->SetAndObserveProbeToTrackerTransformNodeID( NULL ); 
    this->SetProbeToTrackerNodeID( node->ProbeToTrackerNodeID );

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

    if ( this->ProbeModelNodeID != NULL && this->Scene->GetNodeByID( this->ProbeModelNodeID ) == NULL )
    {
        this->SetProbeModelNodeID( NULL );
    }

    if ( this->ProbeToTrackerNodeID != NULL && this->Scene->GetNodeByID( this->ProbeToTrackerNodeID ) == NULL )
    {
        this->SetProbeToTrackerNodeID( NULL );
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

    if ( this->ProbeModelNodeID && !strcmp( oldID, this->ProbeModelNodeID ) )
    {
        this->SetProbeModelNodeID( newID );
    }

    if ( this->ProbeToTrackerNodeID && !strcmp( oldID, this->ProbeToTrackerNodeID ) )
    {
        this->SetProbeToTrackerNodeID( newID );
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
}

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::PrintSelf( ostream& os, vtkIndent indent )
{
    vtkMRMLNode::PrintSelf(os,indent);

    os << indent << "ConnectorNodeID: " << ( this->ConnectorNodeID ? this->ConnectorNodeID : "(none)" ) << "\n";
    os << indent << "ProbeModelNodeID: " << ( this->ProbeModelNodeID ? this->ProbeModelNodeID : "(none)" ) << "\n";
    os << indent << "ProbeToTrackerNodeID: " << (this->ProbeToTrackerNodeID ? this->ProbeToTrackerNodeID : "(none)" ) << "\n";
    os << indent << "ImageToProbeTransformNodeID: " << (this->ImageToProbeTransformNodeID ? this->ImageToProbeTransformNodeID : "(none)") << "\n";
    os << indent << "ModelToProbeTransformNodeID: " << (this->ModelToProbeTransformNodeID ? this->ModelToProbeTransformNodeID : "(none)") << "\n";
}

//------------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLLiveUltrasoundNode::GetProbeModelNode()
{
    vtkMRMLModelNode* node = NULL;
    if ( this->GetScene() && this->ProbeModelNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ProbeModelNodeID );
        node = vtkMRMLModelNode::SafeDownCast( snode );
    }
    return node;
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
            cnode->AddObserver( vtkMRMLIGTLConnectorNode::ReceiveEvent, (vtkCommand*)this->MRMLCallbackCommand );
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
vtkMRMLLinearTransformNode* vtkMRMLLiveUltrasoundNode::GetProbeToTrackerTransformNode()
{
    vtkMRMLLinearTransformNode* node = NULL;
    if (    this->GetScene()
        && this->ProbeToTrackerNodeID != NULL )
    {
        vtkMRMLNode* snode = this->GetScene()->GetNodeByID( this->ProbeToTrackerNodeID);
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
void vtkMRMLLiveUltrasoundNode::SetAndObserveProbeToTrackerTransformNodeID( const char* transformNodeRef )
{
    vtkSetAndObserveMRMLObjectMacro( this->ProbeToTrackerTransformNode, NULL );
    if ( transformNodeRef != NULL )
    {
        this->SetProbeToTrackerNodeID( transformNodeRef );
        vtkMRMLLinearTransformNode* tnode = this->GetProbeToTrackerTransformNode();

        vtkSmartPointer< vtkIntArray > events = vtkSmartPointer< vtkIntArray >::New();
        events->InsertNextValue( vtkMRMLLinearTransformNode::TransformModifiedEvent );
        vtkSetAndObserveMRMLObjectEventsMacro( this->ProbeToTrackerTransformNode, tnode, events );

        if ( tnode != NULL )
        {
            this->ProbeToTrackerTransform->SetMatrix( tnode->GetMatrixTransformToParent() );
            this->ProbeToTrackerTransform->Update();
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
void vtkMRMLLiveUltrasoundNode ::SetAndObserveModelToProbeTransformNodeID( const char* transformNodeRef )
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
        && event == vtkMRMLIGTLConnectorNode::ReceiveEvent )
    {
        int numIncomingNodes = this->ConnectorNode->GetNumberOfIncomingMRMLNodes();
        for ( int nodeIndex = 0; nodeIndex < numIncomingNodes; ++ nodeIndex )
        {
            int sec = -1;
            int nsec = -1;

            vtkMRMLNode* node = this->ConnectorNode->GetIncomingMRMLNode( nodeIndex );

            vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast( node );
            if ( volumeNode != NULL )
            {
                this->ConnectorNode->LockIncomingMRMLNode( node );
                this->ConnectorNode->GetIGTLTimeStamp( node, sec, nsec );
                this->ConnectorNode->UnlockIncomingMRMLNode( node );

                if ( this->LastTimeSec != sec || this->LastTimeNSec != nsec )
                {
                    vtkSmartPointer< vtkMatrix4x4 > mat = vtkSmartPointer< vtkMatrix4x4 >::New();
                    volumeNode->GetIJKToRASMatrix( mat );
                    this->LastTrackerToProbeTransform->SetMatrix( mat );
                    this->LastTrackerToProbeTransform->Update();

                    this->LastTimeSec = sec;
                    this->LastTimeNSec = nsec;
                    this->LastImageData->DeepCopy( volumeNode->GetImageData() );
                    this->UpdateLiveModels( volumeNode, mat );
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
    

    if (    this->ModelToProbeTransformNode == vtkMRMLLinearTransformNode::SafeDownCast( caller )
        && event == vtkMRMLLinearTransformNode::TransformModifiedEvent )
    {
        this->ModelToProbeTransform->SetMatrix( this->ModelToProbeTransformNode->GetMatrixTransformToParent() );
        this->ModelToProbeTransform->Update();
        this->UpdateLiveModels( NULL, NULL );
    }


    if (    this->ImageToProbeTransformNode == vtkMRMLLinearTransformNode::SafeDownCast( caller )
        && event == vtkMRMLLinearTransformNode::TransformModifiedEvent )
    {
        this->ImageToProbeTransform->SetMatrix( this->ImageToProbeTransformNode->GetMatrixTransformToParent() );
        this->ImageToProbeTransform->Update();
        this->UpdateLiveModels( NULL, NULL );
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

//------------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundNode::UpdateLiveModels( vtkMRMLVolumeNode* volumeNode, vtkMatrix4x4* mat )
{
    // Check input.
    if ( this->ProbeModel == NULL ) 
    {
        return;
    }

    // Get probe polydata from selected probe model node.
    vtkPolyData* probePolyData = NULL;
    vtkMRMLModelNode* probeModelNode = this->GetProbeModelNode();
    if ( probeModelNode != NULL )
    {
        probePolyData = probeModelNode->GetPolyData();
    }

    vtkSmartPointer< vtkTransform > tTrackerToProbe = vtkSmartPointer< vtkTransform >::New();
    tTrackerToProbe->PostMultiply();


    // TODO: If too much time has passed, hide models!


    if ( mat == NULL )
    {
        tTrackerToProbe->SetMatrix( this->LastTrackerToProbeTransform->GetMatrix() );
    }
    else
    {
        tTrackerToProbe->SetMatrix( mat );
    }

    tTrackerToProbe->Update();


    vtkSmartPointer< vtkTransform > tTrackerToModel = vtkSmartPointer< vtkTransform >::New();
    tTrackerToModel->Identity();
    tTrackerToModel->Concatenate( tTrackerToProbe );
    tTrackerToModel->Concatenate( this->ModelToProbeTransform );
    tTrackerToModel->Update();

    vtkSmartPointer< vtkTransform > tTrackerToImage = vtkSmartPointer< vtkTransform >::New();
    tTrackerToImage->Identity();
    tTrackerToImage->Concatenate( tTrackerToProbe );
    tTrackerToImage->Concatenate( this->ImageToProbeTransform );
    tTrackerToImage->Update();


    // Update observed tracker to probe-model tarnsform node.

    if ( this->ProbeToTrackerNodeID != NULL )
    {
        vtkMatrix4x4* mTrackerToProbeInScene = this->GetProbeToTrackerTransformNode()->GetMatrixTransformToParent();
        mTrackerToProbeInScene->DeepCopy( tTrackerToModel->GetMatrix() );
        this->GetProbeToTrackerTransformNode()->Modified();
    }


    // Update the live image actor position.

    vtkImageData* volume;

    if (    volumeNode == NULL
        || volumeNode->GetImageData() == NULL )
    {
        volume = this->LastImageData;
    }
    else
    {
        volume = volumeNode->GetImageData();
    }

    this->LiveImageActor->SetInput( volume );
    // TODO: scaling should be defined in ImageToProbe transform 
    this->LiveImageActor->SetScale( 0.2, 0.2, 1.0 );

    this->LiveImageActor->SetUserTransform( tTrackerToImage );

}

