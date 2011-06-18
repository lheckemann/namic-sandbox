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

#ifndef __qSlicerLiveUltrasoundModuleWidget_h
#define __qSlicerLiveUltrasoundModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerLiveUltrasoundModuleExport.h"

class qSlicerLiveUltrasoundModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_LiveUltrasound
class Q_SLICER_QTMODULES_LIVEULTRASOUND_EXPORT qSlicerLiveUltrasoundModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerLiveUltrasoundModuleWidget(QWidget *parent=0);
  virtual ~qSlicerLiveUltrasoundModuleWidget();

public slots:


protected:
  QScopedPointer<qSlicerLiveUltrasoundModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerLiveUltrasoundModuleWidget);
  Q_DISABLE_COPY(qSlicerLiveUltrasoundModuleWidget);
};

#endif
