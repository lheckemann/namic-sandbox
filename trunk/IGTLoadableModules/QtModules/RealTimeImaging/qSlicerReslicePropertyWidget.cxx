
// Qt includes
#include <QButtonGroup>

#include "qSlicerReslicePropertyWidget.h"
#include "ui_qSlicerReslicePropertyWidget.h"

// MRMLWidgets includes
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceControllerWidget.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDViewControllerWidget.h>

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"

// MRMLLogic includes
#include "vtkMRMLLayoutLogic.h"

// VTK includes
#include <vtkCollection.h>
#include <vtkSmartPointer.h>


class qSlicerReslicePropertyWidgetPrivate;
class vtkMRMLNode;
class vtkObject;


//------------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerReslicePropertyWidgetPrivate : public Ui_qSlicerReslicePropertyWidget
{
  Q_DECLARE_PUBLIC(qSlicerReslicePropertyWidget);
protected:
  qSlicerReslicePropertyWidget* const q_ptr;
public:
  qSlicerReslicePropertyWidgetPrivate(qSlicerReslicePropertyWidget& object);
  void init();

  QButtonGroup statusButtonGroup;
  QButtonGroup methodButtonGrouop;
  QButtonGroup orientationButtonGroup;

  enum {
    STATUS_OFF,
    STATUS_ON,
  };

  enum {
    METHOD_POSITION,
    METHOD_ORIENTATION,
  };
  
  enum {
    ORIENTATION_INPLANE,
    ORIENTATION_INPLANE90,
    ORIENTATION_TRANSVERSE,
  };

  vtkMRMLScene * scene;
  vtkMRMLSliceNode * sliceNode;
  vtkMRMLNode  * driverNode;

};

//------------------------------------------------------------------------------
qSlicerReslicePropertyWidgetPrivate::qSlicerReslicePropertyWidgetPrivate(qSlicerReslicePropertyWidget& object)
  : q_ptr(&object)
{
  scene = NULL;
  sliceNode = NULL;
  driverNode = NULL;
}

//------------------------------------------------------------------------------
void qSlicerReslicePropertyWidgetPrivate::init()
{
  Q_Q(qSlicerReslicePropertyWidget);
  this->setupUi(q);

  //QObject::connect(this->ConnectorNameEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(this->ConnectorStateCheckBox, SIGNAL(toggled(bool)),
  //                 q, SLOT(startCurrentIGTLConnector(bool)));
  //QObject::connect(this->ConnectorHostNameEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(this->ConnectorPortEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(&this->ConnectorTypeButtonGroup, SIGNAL(buttonClicked(int)),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //

  this->statusButtonGroup.addButton(this->offRadioButton, STATUS_OFF);
  this->statusButtonGroup.addButton(this->onRadioButton, STATUS_ON);
  this->offRadioButton->setChecked(true);
  this->methodButtonGrouop.addButton(this->positionRadioButton, METHOD_POSITION);
  this->methodButtonGrouop.addButton(this->orientationRadioButton, METHOD_ORIENTATION);
  this->positionRadioButton->setChecked(true);
  this->orientationButtonGroup.addButton(this->inPlaneRadioButton, ORIENTATION_INPLANE);
  this->orientationButtonGroup.addButton(this->inPlane90RadioButton, ORIENTATION_INPLANE90);
  this->orientationButtonGroup.addButton(this->transverseRadioButton, ORIENTATION_TRANSVERSE);
  this->inPlaneRadioButton->setChecked(true);
  
  QObject::connect(this->driverNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode)),
                   q, SLOT(setDriverNode()));
  

}

//------------------------------------------------------------------------------
qSlicerReslicePropertyWidget::qSlicerReslicePropertyWidget(QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerReslicePropertyWidgetPrivate(*this))
{
  Q_D(qSlicerReslicePropertyWidget);
  d->init();
}

//------------------------------------------------------------------------------
qSlicerReslicePropertyWidget::~qSlicerReslicePropertyWidget()
{
}

//------------------------------------------------------------------------------
void qSlicerReslicePropertyWidget::setSliceViewName(const QString& newSliceViewName)
{
  this->setTitle(newSliceViewName);
}

//------------------------------------------------------------------------------
void qSlicerReslicePropertyWidget::setMRMLSliceNode(vtkMRMLSliceNode* newSliceNode)
{
  Q_D(qSlicerReslicePropertyWidget);

  //d->SliceLogic->SetSliceNode(newSliceNode);
  d->sliceNode = newSliceNode;
  if (newSliceNode && newSliceNode->GetScene())
    {
    this->setMRMLScene(newSliceNode->GetScene());
    }
}

//----------------------------------------------------------------------------
void qSlicerReslicePropertyWidget::setMRMLScene(vtkMRMLScene * newScene)
{
  Q_D(qSlicerReslicePropertyWidget);

  if (d->scene != newScene)
    {
    d->scene = newScene;
    if (d->driverNodeSelector)
      {
      d->driverNodeSelector->setMRMLScene(newScene);
      }
    }
}


//------------------------------------------------------------------------------
void qSlicerReslicePropertyWidget::setDriverNode(vtkMRMLNode * newNode)
{
  Q_D(qSlicerReslicePropertyWidget);

  if (d->driverNode != newNode)
    {

    // reconnect current event listener
    //foreach(int evendId, QList<int>()
    //        << vtkCommand::ModifiedEvent
    //        << vtkMRMLIGTLConnectorNode::ActivatedEvent
    //        << vtkMRMLIGTLConnectorNode::ConnectedEvent
    //        << vtkMRMLIGTLConnectorNode::DisconnectedEvent
    //        << vtkMRMLIGTLConnectorNode::DeactivatedEvent)
    //  {
    //  qvtkReconnect(d->IGTLConnectorNode, connectorNode, evendId,
    //                this, SLOT(onMRMLNodeModified()));
    //  }

    // add new event listener
    d->driverNode = newNode;
    }
}


//------------------------------------------------------------------------------
void qSlicerReslicePropertyWidget::onMRMLNodeModified()
{
  Q_D(qSlicerReslicePropertyWidget);
  if (!d->driverNode)
    {
    return;
    }
  //d->ConnectorNameEdit->setText(d->IGTLConnectorNode->GetName());
  //d->ConnectorHostNameEdit->setText(d->IGTLConnectorNode->GetServerHostname());
  //d->ConnectorPortEdit->setText(QString("%1").arg(d->IGTLConnectorNode->GetServerPort()));
  //int type = d->IGTLConnectorNode->GetType();
  //d->ConnectorNotDefinedRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);
  //d->ConnectorServerRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_SERVER);
  //d->ConnectorClientRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_CLIENT);
  //
  //setStateEnabled(d, type != vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);
  //
  //bool deactivated = d->IGTLConnectorNode->GetState() == vtkMRMLIGTLConnectorNode::STATE_OFF;
  //if (deactivated)
  //  {
  //  setNameEnabled(d, true);
  //  setTypeEnabled(d, true);
  //  setHostnameEnabled(d, type == vtkMRMLIGTLConnectorNode::TYPE_CLIENT);
  //  setPortEnabled(d, type != vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);
  //  }
  //else
  //  {
  //  setNameEnabled(d, false);
  //  setTypeEnabled(d, false);
  //  setHostnameEnabled(d, false);
  //  setPortEnabled(d, false);
  //  }
  //d->ConnectorStateCheckBox->setChecked(!deactivated);
}

