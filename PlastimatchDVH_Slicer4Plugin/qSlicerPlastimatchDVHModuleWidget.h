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

#ifndef __qSlicerPlastimatchDVHModuleWidget_h
#define __qSlicerPlastimatchDVHModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerPlastimatchDVHModuleExport.h"

#include "ctkVTKChartView.h"
#include "vtkMRMLVolumeNode.h"

#include <vtkDenseArray.h>

class qSlicerPlastimatchDVHModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_PLASTIMATCHDVH_EXPORT qSlicerPlastimatchDVHModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerPlastimatchDVHModuleWidget(QWidget *parent=0);
  virtual ~qSlicerPlastimatchDVHModuleWidget();

protected slots:
    void OpenCSV();
    void SetShowLegend(bool show);
    void RenderCharts();
    void ApplySelectedVolume();

public slots:


protected:
  QScopedPointer<qSlicerPlastimatchDVHModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

  bool LoadCSVFile(QString fileName);
  bool LoadCSV(QString csvString);
  bool LoadSelectedVolumes(vtkMRMLVolumeNode* doseNode, vtkMRMLVolumeNode* structureSetNode);
  bool DisplayDVH();

protected:
  ctkVTKChartView*            m_ChartView;
  vtkDenseArray<double>*    m_ValueMatrix; // First column of the table is the X Axis indices, the rest are the doses (columns are the structures)
  QStringList*                m_StructureNames;

private:
  Q_DECLARE_PRIVATE(qSlicerPlastimatchDVHModuleWidget);
  Q_DISABLE_COPY(qSlicerPlastimatchDVHModuleWidget);
};

#endif
