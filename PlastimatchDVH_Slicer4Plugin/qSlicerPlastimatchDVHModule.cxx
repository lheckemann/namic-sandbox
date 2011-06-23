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

// ExtensionTemplate Logic includes
#include <vtkSlicerPlastimatchDVHLogic.h>

// ExtensionTemplate includes
#include "qSlicerPlastimatchDVHModule.h"
#include "qSlicerPlastimatchDVHModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerPlastimatchDVHModule, qSlicerPlastimatchDVHModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPlastimatchDVHModulePrivate
{
public:
  qSlicerPlastimatchDVHModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPlastimatchDVHModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModulePrivate::qSlicerPlastimatchDVHModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPlastimatchDVHModule methods

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModule::qSlicerPlastimatchDVHModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerPlastimatchDVHModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModule::~qSlicerPlastimatchDVHModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerPlastimatchDVHModule::helpText()const
{
  return "This PlastimatchDVH module illustrates how a loadable module should "
      "be implemented.";
}

//-----------------------------------------------------------------------------
QString qSlicerPlastimatchDVHModule::acknowledgementText()const
{
  return "This work was supported by ...";
}

//-----------------------------------------------------------------------------
QIcon qSlicerPlastimatchDVHModule::icon()const
{
  return QIcon(":/Icons/PlastimatchDVH.png");
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerPlastimatchDVHModule::createWidgetRepresentation()
{
  return new qSlicerPlastimatchDVHModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerPlastimatchDVHModule::createLogic()
{
  return vtkSlicerPlastimatchDVHLogic::New();
}
