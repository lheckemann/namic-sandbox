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
#include <QtPlugin>

// LiveUltrasound Logic includes
#include <vtkSlicerLiveUltrasoundLogic.h>

// LiveUltrasound includes
#include "qSlicerLiveUltrasoundModule.h"
#include "qSlicerLiveUltrasoundModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerLiveUltrasoundModule, qSlicerLiveUltrasoundModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LiveUltrasound
class qSlicerLiveUltrasoundModulePrivate
{
public:
  qSlicerLiveUltrasoundModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModulePrivate::qSlicerLiveUltrasoundModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModule methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModule::qSlicerLiveUltrasoundModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLiveUltrasoundModulePrivate)
{
}

//-----------------------------------------------------------------------------
QString qSlicerLiveUltrasoundModule::category()const
{
  return "Developer Tools";
}

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModule::~qSlicerLiveUltrasoundModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLiveUltrasoundModule::helpText()const
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
QString qSlicerLiveUltrasoundModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community...";
}

//-----------------------------------------------------------------------------
QIcon qSlicerLiveUltrasoundModule::icon()const
{
  return QIcon(":/Icons/LiveUltrasound.png");
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerLiveUltrasoundModule::createWidgetRepresentation()
{
  return new qSlicerLiveUltrasoundModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLiveUltrasoundModule::createLogic()
{
  return vtkSlicerLiveUltrasoundLogic::New();
}
