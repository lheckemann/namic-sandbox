/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  ==============================================================================*/

#include <iostream>
#include <cstdlib>
// Qt includes

// SlicerQt includes
#include "qSlicerRobotControllerModuleWidget.h"
#include "ui_qSlicerRobotControllerModule.h"

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlClientSocket.h>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_RobotController
class qSlicerRobotControllerModuleWidgetPrivate: public Ui_qSlicerRobotControllerModule
{
public:
  qSlicerRobotControllerModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerRobotControllerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerRobotControllerModuleWidgetPrivate::qSlicerRobotControllerModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerRobotControllerModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerRobotControllerModuleWidget::qSlicerRobotControllerModuleWidget(QWidget* _parent)
  : Superclass( _parent )
    , d_ptr( new qSlicerRobotControllerModuleWidgetPrivate )
{
  this->LinearTransformNode = vtkMRMLLinearTransformNode::New();
  this->clientSocket = igtl::ClientSocket::New();
}

//-----------------------------------------------------------------------------
qSlicerRobotControllerModuleWidget::~qSlicerRobotControllerModuleWidget()
{
  if(this->LinearTransformNode)
    {
    this->LinearTransformNode->Delete();
    }

  /*
  // Create segfault
  if(this->clientSocket)
  {
  if(this->clientSocket.IsNotNull())
  {
  this->clientSocket->CloseSocket();
  }
  this->clientSocket->Delete();
  }
  */
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::setup()
{
  Q_D(qSlicerRobotControllerModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Connect button
  connect(d->ConnectButton, SIGNAL(clicked()),
          this, SLOT(onConnectButtonClicked()));

  // Send button
  connect(d->SendButton, SIGNAL(clicked()),
          this, SLOT(onSendButtonClicked()));
  d->MatrixWidget->setMRMLTransformNode(this->LinearTransformNode);

  connect(d->UpButton, SIGNAL(clicked()),
          this, SLOT(onUpButtonClicked()));
  connect(d->DownButton, SIGNAL(clicked()),
          this, SLOT(onDownButtonClicked()));
  connect(d->LeftButton, SIGNAL(clicked()),
          this, SLOT(onLeftButtonClicked()));
  connect(d->RightButton, SIGNAL(clicked()),
          this, SLOT(onRightButtonClicked()));
  connect(d->ForwardButton, SIGNAL(clicked()),
          this, SLOT(onForwardButtonClicked()));
  connect(d->BackwardButton, SIGNAL(clicked()),
          this, SLOT(onBackwardButtonClicked()));
  connect(d->HomingButton, SIGNAL(clicked()),
          this, SLOT(onHomingButtonClicked()));

}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onConnectButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  if(d->ConnectButton->isChecked())
    {
    int r = this->clientSocket->ConnectToServer(d->HostnameEntry->text().toStdString().c_str(),atoi(d->PortEntry->text().toStdString().c_str()));

    if(r != 0)
      {
      std::cerr << "Cannot connect to the server." << std::endl;
      }
    else
      {
      std::cerr << "Connexion successful." << std::endl;
      d->ConnectButton->setText("Disconnect");
      }
    }
  else
    {
    this->clientSocket->CloseSocket();
    d->ConnectButton->setText("Connect");
    }
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onSendButtonClicked()
{
  if(this->clientSocket.IsNotNull())
    {
    igtl::TransformMessage::Pointer RobotTarget;
    RobotTarget = igtl::TransformMessage::New();
    igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
    igtl::Matrix4x4 matrix;

    vtkSmartPointer<vtkMatrix4x4> M4 = vtkSmartPointer<vtkMatrix4x4>::New();

    // TODO: TransformToWorld or TransformToParent ????
    this->LinearTransformNode->GetMatrixTransformToWorld(M4);

    matrix[0][0] = M4->GetElement(0,0);
    matrix[0][1] = M4->GetElement(0,1);
    matrix[0][2] = M4->GetElement(0,2);
    matrix[0][3] = M4->GetElement(0,3);

    matrix[1][0] = M4->GetElement(1,0);
    matrix[1][1] = M4->GetElement(1,1);
    matrix[1][2] = M4->GetElement(1,2);
    matrix[1][3] = M4->GetElement(1,3);

    matrix[2][0] = M4->GetElement(2,0);
    matrix[2][1] = M4->GetElement(2,1);
    matrix[2][2] = M4->GetElement(2,2);
    matrix[2][3] = M4->GetElement(2,3);

    matrix[3][0] = M4->GetElement(3,0);
    matrix[3][1] = M4->GetElement(3,1);
    matrix[3][2] = M4->GetElement(3,2);
    matrix[3][3] = M4->GetElement(3,3);

    RobotTarget->SetDeviceName("RobotController");
    RobotTarget->SetMatrix(matrix);
    ts->GetTime();
    RobotTarget->SetTimeStamp(ts);
    RobotTarget->Pack();

    this->clientSocket->Send(RobotTarget->GetPackPointer(), RobotTarget->GetPackSize());
    }
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onUpButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(0,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(0,3,elem+1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onDownButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(0,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(0,3,elem-1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onRightButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(1,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(1,3,elem+1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onLeftButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(1,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(1,3,elem-1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onForwardButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(2,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(2,3,elem+1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onBackwardButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  double elem = d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->GetElement(2,3);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(2,3,elem-1);
  d->MatrixWidget->mrmlTransformNode()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModuleWidget::onHomingButtonClicked()
{
  Q_D(qSlicerRobotControllerModuleWidget);

  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(0,3,0);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(1,3,0);
  d->MatrixWidget->mrmlTransformNode()->GetMatrixTransformToParent()->SetElement(2,3,0);
  d->MatrixWidget->mrmlTransformNode()->Modified();


}
