

// Slicer includes
#include "vtkMRMLLiveUltrasoundDisplayableManager.h"

// MRML includes
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLViewNode.h"

// VTK includes
#include "vtkImageData.h"
#include "vtkInteractorStyle.h"
#include "vtkMatrix4x4.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"

// Convenient macro
#define VTK_CREATE(type, name) \
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMRMLLiveUltrasoundDisplayableManager);
vtkCxxRevisionMacro (vtkMRMLLiveUltrasoundDisplayableManager, "$Revision: 1.0 $");


bool vtkMRMLLiveUltrasoundDisplayableManager::First = true;


vtkMRMLLiveUltrasoundDisplayableManager::vtkMRMLLiveUltrasoundDisplayableManager()
{

    this->LiveUltrasoundLogic =  vtkSlicerLiveUltrasoundLogic::New();

    this->DisplayedNode = NULL;

    this->ProcessingMRMLFlag = 0;
    this->UpdatingFromMRML = 0;

}

vtkMRMLLiveUltrasoundDisplayableManager::~vtkMRMLLiveUltrasoundDisplayableManager()
{
    if (this->LiveUltrasoundLogic)
    {
        this->LiveUltrasoundLogic->Delete();
        this->LiveUltrasoundLogic = NULL;
    }
}

void vtkMRMLLiveUltrasoundDisplayableManager::PrintSelf(std::ostream &os, vtkIndent indent)
{
    Superclass::PrintSelf(os, indent); 
}

/*
* return values:
* -1: requested mapper not supported
*  0: invalid input parameter
*  1: success
*/
int vtkMRMLLiveUltrasoundDisplayableManager::SetupImageActorFromNode(vtkMRMLLiveUltrasoundNode* node)
{
    if (node == NULL)
    {
        return 0;
    }

    vtkImageActor* actor = node->GetLiveImageActor(); 
    vtkRenderer* renderer = this->GetRenderer();

    if ( actor == 0 || renderer == 0 )
    {
        return 0; 
    }

    renderer->AddActor(actor); 

    return 1;
}

//----------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnLiveUltrasoundNodeModified(vtkMRMLLiveUltrasoundNode* dnode)
{
    if (dnode)
    {
        this->InitializePipelineFromDisplayNode(dnode);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::Create()
{
    this->OnCreate();
}

//----------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnCreate()
{
    if (this->GetMRMLViewNode() &&
        this->GetMRMLViewNode()->HasObserver(
        vtkMRMLViewNode::GraphicalResourcesCreatedEvent,
        (vtkCommand *) this->GetMRMLCallbackCommand()))
    {
        this->GetMRMLViewNode()->AddObserver(
            vtkMRMLViewNode::GraphicalResourcesCreatedEvent,
            (vtkCommand *) this->GetMRMLCallbackCommand());
    }

    this->UpdateDisplayNodeList();

    //this->OnLiveUltrasoundNodeModified();
}

//----------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::ProcessMRMLEvents(vtkObject *caller,
                                                                unsigned long event,
                                                                void *callData)
{
    if (this->ProcessingMRMLFlag || this->UpdatingFromMRML)
    {
        return;
    }
    this->ProcessingMRMLFlag = 1;

    vtkMRMLNode *node = NULL;

    // Observe ViewNode, Scenario Node, and Parameter node for modify events
    if (event == vtkCommand::ModifiedEvent && this->GetMRMLScene())
    {
        node = reinterpret_cast<vtkMRMLNode *>(caller);

        if (node->IsA("vtkMRMLLiveUltrasoundNode") )
        {
            this->OnLiveUltrasoundNodeModified(vtkMRMLLiveUltrasoundNode::SafeDownCast(node));
        }
    }
    else if (event == vtkMRMLLiveUltrasoundNode::NewFrameArrivedEvent )
    {
        //this->GetRenderer()->GetRenderWindow()->Render(); 
        this->RequestRender(); 
    }
    else if (event == vtkMRMLScene::NodeAddedEvent)
    {
        node = reinterpret_cast<vtkMRMLNode *>(callData);
        this->OnMRMLSceneNodeAddedEvent(node);
    }
    else if (event == vtkMRMLScene::NodeRemovedEvent)
    {
        node = reinterpret_cast<vtkMRMLNode *>(callData);
        this->OnMRMLSceneNodeRemovedEvent(node);
    }
    else if (event == vtkMRMLScene::SceneRestoredEvent)
    {
        this->OnMRMLSceneRestoredEvent();
    }
    else if (event == vtkMRMLScene::SceneImportedEvent)
    {
        this->OnMRMLSceneImportedEvent();
    }
    else if (event == vtkMRMLScene::SceneClosedEvent)
    {
        this->OnMRMLSceneClosedEvent();
    }
    else if (event == vtkMRMLScene::SceneAboutToBeClosedEvent)
    {
        this->OnMRMLSceneAboutToBeClosedEvent();
    }

    this->ProcessingMRMLFlag = 0;
}


//----------------------------------------------------------------------------
//initialize pipeline from a loaded or user selected parameters node
void vtkMRMLLiveUltrasoundDisplayableManager::InitializePipelineFromDisplayNode(vtkMRMLLiveUltrasoundNode* node)
{
    if (!node)
    {
        return;
    }

    if (!node->HasObserver(vtkMRMLLiveUltrasoundNode::NewFrameArrivedEvent, (vtkCommand *) this->GetMRMLCallbackCommand() ) )
    {
        node->AddObserver(vtkMRMLLiveUltrasoundNode::NewFrameArrivedEvent, (vtkCommand *) this->GetMRMLCallbackCommand() ); 
    }

    this->SetupImageActorFromNode(node);
    this->DisplayedNode = node;

    vtkRenderWindow* renderWindow = this->GetRenderer()->GetRenderWindow();
    renderWindow->SetDesiredUpdateRate(0);
    vtkRenderWindowInteractor* renderWindowInteractor = renderWindow ? renderWindow->GetInteractor() : 0;
    if (renderWindowInteractor)
    {
        renderWindowInteractor->SetDesiredUpdateRate(0);
    }
    // TODO: do the connection at initialization time
    vtkInteractorObserver* interactorStyle = renderWindowInteractor ? renderWindowInteractor->GetInteractorStyle() : 0;
    if (interactorStyle)
    {
        if (!interactorStyle->HasObserver(vtkCommand::StartInteractionEvent,
            (vtkCommand*) this->GetLogicCallbackCommand()))
        {
            interactorStyle->AddObserver(vtkCommand::StartInteractionEvent,
                (vtkCommand*) this->GetLogicCallbackCommand());
            interactorStyle->AddObserver(vtkCommand::EndInteractionEvent,
                (vtkCommand*) this->GetLogicCallbackCommand());
        }
    }

}

//----------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::ProcessLogicEvents(
    vtkObject * caller, unsigned long event, void * vtkNotUsed(callData))
{
    vtkInteractorStyle* interactorStyle = vtkInteractorStyle::SafeDownCast(caller);
    if (interactorStyle)
    {
        if (event == vtkCommand::StartInteractionEvent)
        {

        }
        else if (event == vtkCommand::EndInteractionEvent)
        {

        }
    }
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneAboutToBeClosedEvent()
{
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneClosedEvent()
{
    // Clean
    this->SetUpdateFromMRMLRequested(1);
    this->RemoveDisplayNodes();
    //this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneImportedEvent()
{
    // UpdateFromMRML will be executed only if there has been some actions
    // during the import that requested it (don't call
    // SetUpdateFromMRMLRequested(1) here, it should be done somewhere else
    // maybe in OnMRMLSceneNodeAddedEvent, OnMRMLSceneNodeRemovedEvent or
    // OnMRMLDisplayableModelNodeModifiedEvent).
    this->OnCreate();
    //this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneRestoredEvent()
{
    // UpdateFromMRML will be executed only if there has been some actions
    // during the restoration that requested it (don't call
    // SetUpdateFromMRMLRequested(1) here, it should be done somewhere else
    // maybe in OnMRMLSceneNodeAddedEvent, OnMRMLSceneNodeRemovedEvent or
    // OnMRMLDisplayableModelNodeModifiedEvent).
    this->OnCreate();
    //this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneNodeAddedEvent(vtkMRMLNode* node)
{
    if (!node->IsA("vtkMRMLVolumeNode") &&
        !node->IsA("vtkMRMLLiveUltrasoundNode") 
        )
    {
        return;
    }

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->GetIsUpdating())
    {
        return;
    }

    if (node->IsA("vtkMRMLLiveUltrasoundNode") )
    {
        this->AddDisplayNode(vtkMRMLLiveUltrasoundNode::SafeDownCast(node));
    }
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::OnMRMLSceneNodeRemovedEvent(vtkMRMLNode* node)
{
    assert(node);

    if (!node->IsA("vtkMRMLVolumeNode") &&
        !node->IsA("vtkMRMLLiveUltrasoundNode")
        )
    {
        return;
    }

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->GetIsUpdating())
    {
        return;
    }

    if (node->IsA("vtkMRMLLiveUltrasoundNode"))
    {
        this->RemoveDisplayNode(vtkMRMLLiveUltrasoundNode::SafeDownCast(node));
    }
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::UpdateFromMRML()
{
    if (this->ProcessingMRMLFlag || this->UpdatingFromMRML)
    {
        return;
    }
    this->UpdatingFromMRML = 1;
    //this->InitializePipelineFromDisplayNode();
    this->UpdatingFromMRML = 0;
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::UpdateDisplayNodeList()
{
    std::vector<vtkMRMLNode *> nodes;
    if (this->GetMRMLScene())
    {
        this->GetMRMLScene()->GetNodesByClass(
            "vtkMRMLLiveUltrasoundNode", nodes);
    }

    for (unsigned int i = 0; i < nodes.size(); ++i)
    {
        this->AddDisplayNode(vtkMRMLLiveUltrasoundNode::SafeDownCast(nodes[i]));
    }
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::AddDisplayNode(vtkMRMLLiveUltrasoundNode *dnode)
{
    if (!dnode || this->DisplayNodes.count(std::string(dnode->GetID())) )
    {
        return;
    }

    vtkEventBroker *broker = vtkEventBroker::GetInstance();
    ///TODO: have vtkEventBroker::AddObservation to uniquely add an observation
    std::vector< vtkObservation *> observations =
        broker->GetObservations(dnode, vtkCommand::ModifiedEvent,
        this, this->GetMRMLCallbackCommand());
    if (observations.size() == 0)
    {
        broker->AddObservation(dnode,vtkCommand::ModifiedEvent,
            this, this->GetMRMLCallbackCommand());
    }
    this->DisplayNodes[dnode->GetID()] = dnode;
    this->OnLiveUltrasoundNodeModified(dnode);
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::RemoveDisplayNode(vtkMRMLLiveUltrasoundNode *dnode)
{
    vtkEventBroker *broker = vtkEventBroker::GetInstance();
    ///TODO: have vtkEventBroker::AddObservation to uniquely add an observation
    std::vector< vtkObservation *> observations =
        broker->GetObservations(dnode, vtkCommand::ModifiedEvent,
        this, this->GetMRMLCallbackCommand());
    if (observations.size() != 0)
    {
        broker->RemoveObservations(observations);
    }
    this->DisplayNodes.erase(dnode->GetID());

    if (dnode == this->DisplayedNode)
    {
        this->DisplayedNode = 0;
    }
}

//---------------------------------------------------------------------------
void vtkMRMLLiveUltrasoundDisplayableManager::RemoveDisplayNodes()
{
    while (!this->DisplayNodes.empty())
    {
        this->RemoveDisplayNode(this->DisplayNodes.begin()->second);
    }
}
