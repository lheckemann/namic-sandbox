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

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LiveUltrasound
class qSlicerLiveUltrasoundModuleWidgetPrivate: public Ui_qSlicerLiveUltrasoundModule
{
public:
  qSlicerLiveUltrasoundModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidgetPrivate::qSlicerLiveUltrasoundModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLiveUltrasoundModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidget::qSlicerLiveUltrasoundModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLiveUltrasoundModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerLiveUltrasoundModuleWidget::~qSlicerLiveUltrasoundModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLiveUltrasoundModuleWidget::setup()
{
  Q_D(qSlicerLiveUltrasoundModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
}

