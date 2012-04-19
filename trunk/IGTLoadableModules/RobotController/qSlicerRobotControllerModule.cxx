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

// Qt includes
#include <QtPlugin>

// RobotController Logic includes
#include <vtkSlicerRobotControllerLogic.h>

// RobotController includes
#include "qSlicerRobotControllerModule.h"
#include "qSlicerRobotControllerModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerRobotControllerModule, qSlicerRobotControllerModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_RobotController
class qSlicerRobotControllerModulePrivate
{
public:
  qSlicerRobotControllerModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerRobotControllerModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerRobotControllerModulePrivate::qSlicerRobotControllerModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerRobotControllerModule methods

//-----------------------------------------------------------------------------
qSlicerRobotControllerModule::qSlicerRobotControllerModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerRobotControllerModulePrivate)
{
}

//-----------------------------------------------------------------------------
QStringList qSlicerRobotControllerModule::categories()const
{
  return QStringList() << "Developer Tools";
}

//-----------------------------------------------------------------------------
qSlicerRobotControllerModule::~qSlicerRobotControllerModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerRobotControllerModule::helpText()const
{
  QString help = 
    "This template module is meant to be used with the"
    "with the ModuleWizard.py script distributed with the"
    "Slicer source code (starting with version 4)."
    "Developers can generate their own source code using the"
    "wizard and then customize it to fit their needs.";
  return help;
}

//-----------------------------------------------------------------------------
QString qSlicerRobotControllerModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community...";
}

//-----------------------------------------------------------------------------
QStringList qSlicerRobotControllerModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (Organization)");
  // moduleContributors << QString("Richard Roe (Organization2)");
  // ...
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerRobotControllerModule::icon()const
{
  return QIcon(":/Icons/RobotController.png");
}

//-----------------------------------------------------------------------------
void qSlicerRobotControllerModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerRobotControllerModule::createWidgetRepresentation()
{
  return new qSlicerRobotControllerModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerRobotControllerModule::createLogic()
{
  return vtkSlicerRobotControllerLogic::New();
}
