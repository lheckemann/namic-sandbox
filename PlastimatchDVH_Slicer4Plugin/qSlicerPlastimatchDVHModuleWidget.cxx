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
#include <QFileDialog>
#include <QTimer>

// SlicerQt includes
#include "qSlicerPlastimatchDVHModuleWidget.h"
#include "ui_qSlicerPlastimatchDVHModule.h"

#include <vtkChart.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkAxis.h>
#include <vtkRenderWindow.h>
#include <vtkImageExport.h>
#include <vtkImageFlip.h>
#include <vtkContextMouseEvent.h>

#include <itkImage.h>

#include "dvh.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPlastimatchDVHModuleWidgetPrivate: public Ui_qSlicerPlastimatchDVHModule
{
public:
  qSlicerPlastimatchDVHModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPlastimatchDVHModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModuleWidgetPrivate::qSlicerPlastimatchDVHModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPlastimatchDVHModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModuleWidget::qSlicerPlastimatchDVHModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerPlastimatchDVHModuleWidgetPrivate )
  , m_ChartView( NULL )
  , m_ValueMatrix( NULL )
  , m_StructureNames( NULL )
{
}

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModuleWidget::~qSlicerPlastimatchDVHModuleWidget()
{
  if (m_ValueMatrix != NULL) {
    m_ValueMatrix->Delete();
    m_ValueMatrix = NULL;
  }

  if (m_ChartView != NULL) {
    delete m_ChartView;
    m_ChartView = NULL;
  }

  if (m_StructureNames != NULL) {
    delete m_StructureNames;
    m_StructureNames = NULL;
  }
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::setup()
{
  Q_D(qSlicerPlastimatchDVHModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Make connections
  connect( d->pushButton_LoadCSV, SIGNAL( pressed() ), this, SLOT( OpenCSV() ) );
  connect( d->checkBox_ShowLegend, SIGNAL( toggled(bool) ), this, SLOT( SetShowLegend(bool) ) );
  connect( d->pushButton_ApplyVolume, SIGNAL( pressed() ), this, SLOT( ApplySelectedVolume() ) );
  //connect( d->MRMLNodeComboBox_SelectVolume, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( ???(vtkMRMLNode*) ) );

  // Set up chart view
  m_ChartView = new ctkVTKChartView(d->widget_chartWidgetContainer);

  QGridLayout* grid = new QGridLayout(d->widget_chartWidgetContainer);
  grid->setContentsMargins(0, 0, 0, 0);
  grid->addWidget(m_ChartView);
  d->widget_chartWidgetContainer->setLayout(grid);

  m_ChartView->chart()->AddPlot(vtkChart::LINE);
  m_ChartView->chart()->SetTitle("Empty (select volume or load CSV file)");
  m_ChartView->chart()->GetAxis(0)->SetTitle("Volume, %");
  m_ChartView->chart()->GetAxis(1)->SetTitle("Dose, Gy");
  QTimer::singleShot(100, this, SLOT(RenderCharts())); // Without this the chart is all black

  /*
  // Create test plot
  // Create a table with some points in it...
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
  arrX->SetName("X Axis");
  table->AddColumn(arrX);
  vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
  arrC->SetName("Cosine");
  table->AddColumn(arrC);
  vtkSmartPointer<vtkFloatArray> arrS = vtkSmartPointer<vtkFloatArray>::New();
  arrS->SetName("Sine");
  table->AddColumn(arrS);
  vtkSmartPointer<vtkFloatArray> arrS2 = vtkSmartPointer<vtkFloatArray>::New();
  arrS2->SetName("Sine2");
  table->AddColumn(arrS2);
  // Test charting with a few more points...
  int numPoints = 69;
  float inc = 7.5 / (numPoints-1);
  table->SetNumberOfRows(numPoints);
  for (int i = 0; i < numPoints; ++i)
    {
    table->SetValue(i, 0, i * inc);
    table->SetValue(i, 1, cos(i * inc) + 0.0);
    table->SetValue(i, 2, sin(i * inc) + 0.0);
    table->SetValue(i, 3, sin(i * inc) + 0.5);
    }

  // Add multiple line plots, setting the colors etc
  vtkPlot *line = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  line->SetInput(table, 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  line->SetInput(table, 0, 2);
  line->SetColor(255, 0, 0, 255);
  line->SetWidth(5.0);
  line = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  line->SetInput(table, 0, 3);
  line->SetColor(0, 0, 255, 255);
  line->SetWidth(4.0);
  */
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::OpenCSV()
{
  Q_D(qSlicerPlastimatchDVHModuleWidget);

  // File open dialog for selecting phantom definition xml
  QString filter = QString( tr( "CSV files ( *.csv );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Open CSV containing a DVH" ) ), ".", filter);
  if (fileName.isNull()) {
    return;
  }

  // Load csv
  if (LoadCSVFile(fileName)) {
    d->pushButton_LoadCSV->setText(fileName);
    d->pushButton_LoadCSV->setToolTip(fileName); 
  } else {
    d->pushButton_LoadCSV->setText( tr("Invalid file!") );
  }
}

//-----------------------------------------------------------------------------
bool qSlicerPlastimatchDVHModuleWidget::LoadCSV(QString csvString)
{
  QStringList lines(csvString.split("\n"));

  m_StructureNames = new QStringList(lines.at(0).split(","));

  int numberOfColumns = m_StructureNames->size();
  int numberOfRows = lines.size() - 1 - (lines.last().isEmpty() ? 1 : 0); // the first line is the name of structures, the rest are the values. If the last line is empty (eol at the end of the file) then skip it too

  if (m_ValueMatrix != NULL) {
    m_ValueMatrix->Delete();
  }

  m_ValueMatrix = vtkDenseArray<double>::New();
  m_ValueMatrix->Resize(numberOfRows, numberOfColumns);

  int rowIndex = 0;
  QStringListIterator valueLinesIterator(lines);
  while (valueLinesIterator.hasNext()) {
    if (! valueLinesIterator.hasPrevious()) {
      valueLinesIterator.next();
      continue; // skip the first line (it contains the name of the structures)
    }

    QStringList values(valueLinesIterator.next().split(","));
    QStringListIterator valuesIterator(values);

    int columnIndex = 0;
    while (valuesIterator.hasNext()) {
      QString valueString(valuesIterator.next());

      bool ok;
      double value = valueString.toDouble(&ok);
      if (ok) {
        m_ValueMatrix->SetValue(rowIndex, columnIndex, value);
      } else {
        m_ValueMatrix->SetValue(rowIndex, columnIndex, -1.0);
      }

      ++columnIndex;
    }

    ++rowIndex;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool qSlicerPlastimatchDVHModuleWidget::LoadCSVFile(QString fileName)
{
  // Open file
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream in(&file);

  if (! LoadCSV(in.readAll())) {
    return false;
  }

  return DisplayDVH();
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::SetShowLegend(bool show)
{
  m_ChartView->chart()->SetShowLegend(show);
  m_ChartView->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::RenderCharts()
{
  m_ChartView->GetRenderWindow()->Render();
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::ApplySelectedVolume()
{
  // The body of this function should be in the Logic class?

  // Get the vtkImageData from selected MRML node
  Q_D(qSlicerPlastimatchDVHModuleWidget);

  // Get and parse the dose volume
  vtkMRMLNode* doseNode = d->MRMLNodeComboBox_DoseVolume->currentNode();
  if (doseNode == NULL) {
    return; // TODO error report
  }
  vtkMRMLVolumeNode* doseVolumeNode = dynamic_cast<vtkMRMLVolumeNode*>(doseNode);
  if (doseVolumeNode == NULL) {
    return;
  }

  // Get and parse the structure set
  vtkMRMLNode* structureSetNode = d->MRMLNodeComboBox_StructureSet->currentNode();
  if (structureSetNode == NULL) {
    return;
  }
  vtkMRMLVolumeNode* structureSetVolumeNode = dynamic_cast<vtkMRMLVolumeNode*>(structureSetNode);
  if (structureSetVolumeNode == NULL) {
    return;
  }

  // Load volumes into Plastimatch and display DVH
  LoadSelectedVolumes(doseVolumeNode, structureSetVolumeNode);
}

//-----------------------------------------------------------------------------
bool qSlicerPlastimatchDVHModuleWidget::LoadSelectedVolumes(vtkMRMLVolumeNode* doseNode, vtkMRMLVolumeNode* structureSetNode)
{
  // Get vtkImageData from nodes
  vtkSmartPointer<vtkImageData> doseImageData = doseNode->GetImageData();

  if (doseImageData == NULL) {
    return false;
  }

  vtkSmartPointer<vtkImageData> structureSetImageData = structureSetNode->GetImageData();
  if (structureSetImageData == NULL) {
    return false;
  }

  typedef unsigned short ImageType;
  int extent[6];

  // Convert dose vtkImageData to itk Image
  vtkSmartPointer<vtkImageFlip> doseImageFlipy = vtkSmartPointer<vtkImageFlip>::New(); 
  doseImageFlipy->SetInput(doseImageData); 
  doseImageFlipy->SetFilteredAxis(1); 
  doseImageFlipy->Update(); 

  vtkSmartPointer<vtkImageExport> doseImageExport = vtkSmartPointer<vtkImageExport>::New(); 
  doseImageExport->ImageLowerLeftOff();
  doseImageExport->SetInput(doseImageFlipy->GetOutput()); 
  doseImageExport->Update(); 

  doseImageData->GetExtent(extent);
  itk::Image<ImageType, 3>::Pointer doseImageDataExported = itk::Image<ImageType, 3>::New();
  itk::Image<ImageType, 3>::SizeType doseSize = {extent[1] - extent[0] + 1, extent[3] - extent[2] + 1, extent[5] - extent[4] + 1};
  itk::Image<ImageType, 3>::IndexType doseStart = {0,0,0};
  itk::Image<ImageType, 3>::RegionType doseRegion;
  doseRegion.SetSize(doseSize);
  doseRegion.SetIndex(doseStart);
  doseImageDataExported->SetRegions(doseRegion);
  doseImageDataExported->Allocate();

  /* GCS: The following call causes Slicer 4 to crash on linux */

  memcpy(doseImageDataExported->GetBufferPointer(), doseImageExport->GetPointerToData(), doseImageExport->GetDataMemorySize());

  // Convert structure set vtkImageData to itk Image
  vtkSmartPointer<vtkImageFlip> structureSetImageFlipy = vtkSmartPointer<vtkImageFlip>::New(); 
  structureSetImageFlipy->SetInput(structureSetImageData); 
  structureSetImageFlipy->SetFilteredAxis(1); 
  structureSetImageFlipy->Update(); 

  vtkSmartPointer<vtkImageExport> structureSetImageExport = vtkSmartPointer<vtkImageExport>::New(); 
  structureSetImageExport->ImageLowerLeftOff();
  structureSetImageExport->SetInput(structureSetImageFlipy->GetOutput()); 
  structureSetImageExport->Update(); 

  structureSetImageData->GetExtent(extent);
  itk::Image<ImageType, 3>::Pointer structureSetImageDataExported = itk::Image<ImageType, 3>::New();
  itk::Image<ImageType, 3>::SizeType structureSetSize = {extent[1] - extent[0] + 1, extent[3] - extent[2] + 1, extent[5] - extent[4] + 1};
  itk::Image<ImageType, 3>::IndexType structureSetStart = {0,0,0};
  itk::Image<ImageType, 3>::RegionType structureSetRegion;
  structureSetRegion.SetSize(structureSetSize);
  structureSetRegion.SetIndex(structureSetStart);
  structureSetImageDataExported->SetRegions(structureSetRegion);
  structureSetImageDataExported->Allocate();

  memcpy(structureSetImageDataExported->GetBufferPointer(), structureSetImageExport->GetPointerToData(), structureSetImageExport->GetDataMemorySize());

  // Pass the itk Image to Plastimatch DVH calculator
  Dvh_parms parms;
  parms.cumulative = 0;
  std::string dvhStdString = dvh_execute(structureSetImageDataExported, doseImageDataExported, &parms);

  QString dvhString = QString::fromStdString(dvhStdString);

  if (! LoadCSV(dvhString)) {
    return false;
  }

  // Display DVH
  return DisplayDVH();
}

//-----------------------------------------------------------------------------
bool qSlicerPlastimatchDVHModuleWidget::DisplayDVH()
{
  int numberOfRows = m_ValueMatrix->GetExtents()[0].GetSize();
  int numberOfColumns = m_ValueMatrix->GetExtents()[1].GetSize();

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  // Set the structure names
  QStringListIterator namesIterator(*m_StructureNames);
  while (namesIterator.hasNext()) {
    vtkSmartPointer<vtkFloatArray> column = vtkSmartPointer<vtkFloatArray>::New();
    column->SetName(namesIterator.next().toStdString().data());
    table->AddColumn(column);
  }

  table->SetNumberOfRows(numberOfRows);

  // Fill in the X axis (column 0)
  for (int i=0; i<numberOfRows; ++i) {
    table->SetValue(i, 0, m_ValueMatrix->GetValue(i, 0));
  }

  // Calculate the integral of the values
  for (int j=1; j<numberOfColumns; ++j) {
    vtkSmartPointer<vtkFloatArray> sumArray = vtkSmartPointer<vtkFloatArray>::New();
    sumArray->Resize(numberOfRows);
    double actualSum = 0;

    for (int i=0; i<numberOfRows; ++i) {
      actualSum += m_ValueMatrix->GetValue(i, j);
      sumArray->SetValue(i, actualSum);
    }

    for (int i=0; i<numberOfRows; ++i) {
      table->SetValue(i, j, 100.0 / (sumArray->GetValue(numberOfRows-1) - sumArray->GetValue(0)) * (-sumArray->GetValue(i) + sumArray->GetValue(numberOfRows-1)));
    }
  }

  // Display plot
  m_ChartView->chart()->ClearPlots();
  m_ChartView->chart()->SetTitle("DVH");

  vtkPlot *linePlot = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  for (int i=1; i<numberOfColumns; ++i) {
    linePlot->SetInput(table, 0, i);
    linePlot->SetWidth(2.0);
    linePlot = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  }

  // Set chart properties
  m_ChartView->chart()->SetActionToButton(vtkChart::SELECT, vtkContextMouseEvent::LEFT_BUTTON);
  m_ChartView->chart()->SetActionToButton(vtkChart::ZOOM, vtkContextMouseEvent::RIGHT_BUTTON);

  return true;
}
