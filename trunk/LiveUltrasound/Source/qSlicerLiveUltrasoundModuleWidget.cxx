/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerLiveUltrasoundModuleWidget.h"
#include "ui_qSlicerLiveUltrasoundModule.h"
#include "vtkSlicerLiveUltrasoundLogic.h"
#include "vtkMRMLLiveUltrasoundNode.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LiveUltrasound
class qSlicerLiveUltrasoundModuleWidgetPrivate: public Ui_qSlicerLiveUltrasoundModule
{
 Q_DECLARE_PUBLIC(qSlicerLiveUltrasoundModuleWidget);
protected:
  qSlicerLiveUltrasoundModuleWidget* const q_ptr;
public:

  qSlicerLiveUltrasoundModuleWidgetPrivate(qSlicerLiveUltrasoundModuleWidget& object);
  ~qSlicerLiveUltrasoundModuleWidgetPrivate();

  vtkSlicerLiveUltrasoundLogic* logic() const;

protected slots:

protected:

private:
};

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidgetPrivate::qSlicerLiveUltrasoundModuleWidgetPrivate(qSlicerLiveUltrasoundModuleWidget& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidgetPrivate::~qSlicerLiveUltrasoundModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerLiveUltrasoundLogic* qSlicerLiveUltrasoundModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerLiveUltrasoundModuleWidget);
  return vtkSlicerLiveUltrasoundLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidget::qSlicerLiveUltrasoundModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLiveUltrasoundModuleWidgetPrivate(*this) )
{
    this->liveUltrasoundNode = 0; 
}

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidget::~qSlicerLiveUltrasoundModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
    this->Superclass::setMRMLScene(scene);

    // find parameters node or create it if there is no one in the scene
    if (this->liveUltrasoundNode == 0)
    {
        vtkMRMLLiveUltrasoundNode *liveusnode = 0;
        vtkMRMLNode *node = scene->GetNthNodeByClass(0, "vtkMRMLLiveUltrasoundNode");
        if (node == 0)
        {
            liveusnode = vtkMRMLLiveUltrasoundNode::New();
            scene->AddNode(liveusnode);
            this->setLiveUltrasoundDataNode(liveusnode);
            liveusnode->Delete();
        }
        else 
        {
            liveusnode = vtkMRMLLiveUltrasoundNode::SafeDownCast(node);
            this->setLiveUltrasoundDataNode(liveusnode);
        }
    }

    this->updateWidget(); 
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setup()
{
  Q_D(qSlicerLiveUltrasoundModuleWidget);
  d->setupUi(this);
  this->Superclass::setup(); 

  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->OpenIGTLinkNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));
  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->DataNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));
  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ProbeTransformNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));
  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ImageToProbeTransformNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));
  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ModelToProbeTransformNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));


  QObject::connect(d->DataNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                             SLOT( setLiveUltrasoundDataNode(vtkMRMLNode*) ) ); 

  QObject::connect(d->ImageToProbeTransformNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                             SLOT( setImageToProbeTransformNode(vtkMRMLNode*) ) ); 

  QObject::connect(d->ProbeTransformNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                             SLOT( setProbeTransformNode(vtkMRMLNode*) ) ); 

   QObject::connect(d->ModelToProbeTransformNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, 
                                             SLOT( setModelToProbeTransformNode(vtkMRMLNode*) ) ); 

  {
      Q_D(qSlicerLiveUltrasoundModuleWidget);
      vtkSlicerLiveUltrasoundLogic *l = vtkSlicerLiveUltrasoundLogic::SafeDownCast(d->logic());
      vtkMRMLScene* scene = l->GetMRMLScene();
      emit mrmlSceneChanged(scene); 
  } 
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::enter()
{
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::exit()
{
  this->Superclass::exit();
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::updateWidget()
{
    if( !this->liveUltrasoundNode )
    {
        return;
    }

    Q_D(qSlicerLiveUltrasoundModuleWidget);
    vtkMRMLLiveUltrasoundNode *n = this->liveUltrasoundNode;

    if(d->DataNodeSelector->currentNode() != (vtkMRMLNode*)n)
    {
        d->DataNodeSelector->setCurrentNode((vtkMRMLNode*)n);
    }

    if(d->ImageToProbeTransformNodeSelector->currentNode() != (vtkMRMLNode*) n->GetImageToProbeTransformNode())
    {
        d->ImageToProbeTransformNodeSelector->setCurrentNode((vtkMRMLNode*)n->GetImageToProbeTransformNode());
    }

    if(d->ModelToProbeTransformNodeSelector->currentNode() != (vtkMRMLNode*) n->GetModelToProbeTransformNode())
    {
        d->ModelToProbeTransformNodeSelector->setCurrentNode((vtkMRMLNode*)n->GetModelToProbeTransformNode());
    }

    if(d->ProbeTransformNodeSelector->currentNode() != (vtkMRMLNode*) n->GetProbeToTrackerTransformNode())
    {
        d->ProbeTransformNodeSelector->setCurrentNode((vtkMRMLNode*)n->GetProbeToTrackerTransformNode());
    }

}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setLiveUltrasoundDataNode(vtkMRMLNode *node)
{
    vtkMRMLLiveUltrasoundNode *liveUsNode = vtkMRMLLiveUltrasoundNode::SafeDownCast(node);

    // each time the node is modified, the logic creates tracks
    vtkSlicerLiveUltrasoundLogic *liveUsLogic = 
        vtkSlicerLiveUltrasoundLogic::SafeDownCast(this->d_ptr->logic());
    if (liveUsLogic && this->mrmlScene())
    {
        liveUsLogic->SetAndObserveLiveUltrasoundNode(liveUsNode);
    }

    this->liveUltrasoundNode = liveUsNode;
    // each time the node is modified, the qt widgets are updated
    this->qvtkReconnect(this->liveUltrasoundNode, liveUsNode, 
        vtkCommand::ModifiedEvent, this, SLOT(updateWidget()));

    this->updateWidget();
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setImageToProbeTransformNode(vtkMRMLNode *node)
{
    if ( this->liveUltrasoundNode )
    {
        this->liveUltrasoundNode->SetAndObserveImageToProbeTransformNodeID( node ? node->GetID() : NULL );
    }
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setProbeTransformNode(vtkMRMLNode *node)
{
    if ( this->liveUltrasoundNode )
    {
        this->liveUltrasoundNode->SetAndObserveProbeToTrackerTransformNodeID( node ? node->GetID() : NULL );
    }
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setModelToProbeTransformNode(vtkMRMLNode *node)
{
    if ( this->liveUltrasoundNode )
    {
        this->liveUltrasoundNode->SetAndObserveModelToProbeTransformNodeID( node ? node->GetID() : NULL );
    }
}
