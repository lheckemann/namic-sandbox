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
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

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
  return tr("Developer Tools");
}

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModule::~qSlicerLiveUltrasoundModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLiveUltrasoundModule::helpText()const
{
  QString help = tr("See <a>https://www.assembla.com/spaces/LiveUltrasound/wiki</a> for details (project members access only)."); 
  return help;
}

//-----------------------------------------------------------------------------
QString qSlicerLiveUltrasoundModule::acknowledgementText()const
{
  return tr("This work was supported by NAMIC.");
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
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->
    RegisterDisplayableManager("vtkMRMLLiveUltrasoundDisplayableManager");
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
