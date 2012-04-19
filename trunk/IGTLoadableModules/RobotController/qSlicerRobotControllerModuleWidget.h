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

#ifndef __qSlicerRobotControllerModuleWidget_h
#define __qSlicerRobotControllerModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "vtkMRMLLinearTransformNode.h"

#include "qSlicerRobotControllerModuleExport.h"
#include <igtlClientSocket.h>

class qSlicerRobotControllerModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_RobotController
class Q_SLICER_QTMODULES_ROBOTCONTROLLER_EXPORT qSlicerRobotControllerModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerRobotControllerModuleWidget(QWidget *parent=0);
  virtual ~qSlicerRobotControllerModuleWidget();

  igtl::ClientSocket::Pointer clientSocket;
  vtkMRMLLinearTransformNode* LinearTransformNode;

public slots:

protected slots:
  void onConnectButtonClicked();  
  void onSendButtonClicked();  
  void onUpButtonClicked();  
  void onDownButtonClicked();  
  void onLeftButtonClicked();  
  void onRightButtonClicked();  
  void onForwardButtonClicked();  
  void onBackwardButtonClicked();  
  void onHomingButtonClicked();  


protected:
  QScopedPointer<qSlicerRobotControllerModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerRobotControllerModuleWidget);
  Q_DISABLE_COPY(qSlicerRobotControllerModuleWidget);
};

#endif
