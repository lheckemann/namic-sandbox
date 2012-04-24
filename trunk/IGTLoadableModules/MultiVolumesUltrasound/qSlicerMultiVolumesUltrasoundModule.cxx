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

// MultiVolumesUltrasound Logic includes
#include <vtkSlicerMultiVolumesUltrasoundLogic.h>

// MultiVolumesUltrasound includes
#include "qSlicerMultiVolumesUltrasoundModule.h"
#include "qSlicerMultiVolumesUltrasoundModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerMultiVolumesUltrasoundModule, qSlicerMultiVolumesUltrasoundModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_MultiVolumesUltrasound
class qSlicerMultiVolumesUltrasoundModulePrivate
{
public:
  qSlicerMultiVolumesUltrasoundModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMultiVolumesUltrasoundModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModulePrivate::qSlicerMultiVolumesUltrasoundModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMultiVolumesUltrasoundModule methods

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModule::qSlicerMultiVolumesUltrasoundModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMultiVolumesUltrasoundModulePrivate)
{
}

//-----------------------------------------------------------------------------
QStringList qSlicerMultiVolumesUltrasoundModule::categories()const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModule::~qSlicerMultiVolumesUltrasoundModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMultiVolumesUltrasoundModule::helpText()const
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
QString qSlicerMultiVolumesUltrasoundModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community...";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMultiVolumesUltrasoundModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (Organization)");
  // moduleContributors << QString("Richard Roe (Organization2)");
  // ...
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMultiVolumesUltrasoundModule::icon()const
{
  return QIcon(":/Icons/MultiVolumesUltrasound.png");
}

//-----------------------------------------------------------------------------
void qSlicerMultiVolumesUltrasoundModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerMultiVolumesUltrasoundModule::createWidgetRepresentation()
{
  return new qSlicerMultiVolumesUltrasoundModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMultiVolumesUltrasoundModule::createLogic()
{
  return vtkSlicerMultiVolumesUltrasoundLogic::New();
}
