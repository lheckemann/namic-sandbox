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

// SlicerQt includes
#include "qSlicerMultiVolumesUltrasoundModuleWidget.h"
#include "ui_qSlicerMultiVolumesUltrasoundModule.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_MultiVolumesUltrasound
class qSlicerMultiVolumesUltrasoundModuleWidgetPrivate: public Ui_qSlicerMultiVolumesUltrasoundModule
{
public:
  qSlicerMultiVolumesUltrasoundModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMultiVolumesUltrasoundModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModuleWidgetPrivate::qSlicerMultiVolumesUltrasoundModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMultiVolumesUltrasoundModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModuleWidget::qSlicerMultiVolumesUltrasoundModuleWidget(QWidget* _parent)
  : Superclass( _parent )
    , d_ptr( new qSlicerMultiVolumesUltrasoundModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerMultiVolumesUltrasoundModuleWidget::~qSlicerMultiVolumesUltrasoundModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerMultiVolumesUltrasoundModuleWidget::setup()
{
  Q_D(qSlicerMultiVolumesUltrasoundModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  this->setMRMLScene(this->mrmlScene());

  connect(d->playVolumesButton, SIGNAL(clicked()),
          this, SLOT(onPlayVolumesButtonClicked()));
  connect(d->pauseVolumesButton, SIGNAL(clicked()),
          this, SLOT(onPauseVolumesButtonClicked()));

  // No volumes to begin
  d->playVolumesButton->setText("(0)");
}

//-----------------------------------------------------------------------------
void qSlicerMultiVolumesUltrasoundModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerMultiVolumesUltrasoundModuleWidget);

  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
          d->OIGTLConnectorNodeSelector,SLOT(setMRMLScene(vtkMRMLScene*)));
  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
          d->MultiVolumeContainerNodeSelector,SLOT(setMRMLScene(vtkMRMLScene*)));

  if (d->OIGTLConnectorNodeSelector)
    {
    d->OIGTLConnectorNodeSelector->setMRMLScene(scene);
    }
  if (d->MultiVolumeContainerNodeSelector)
    {
    d->MultiVolumeContainerNodeSelector->setMRMLScene(scene);
    }
}

//-----------------------------------------------------------------------------
void qSlicerMultiVolumesUltrasoundModuleWidget::onPlayVolumesButtonClicked()
{
  Q_D(qSlicerMultiVolumesUltrasoundModuleWidget);

  std::cerr << "(LC) onPlayVolumesButtonClicked()" << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerMultiVolumesUltrasoundModuleWidget::onPauseVolumesButtonClicked()
{
  Q_D(qSlicerMultiVolumesUltrasoundModuleWidget);

  std::cerr << "(LC) onPauseVolumesButtonClicked()" << std::endl;
}
