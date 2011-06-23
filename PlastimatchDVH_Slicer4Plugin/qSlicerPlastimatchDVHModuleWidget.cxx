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

// SlicerQt includes
#include "qSlicerPlastimatchDVHModuleWidget.h"
#include "ui_qSlicerPlastimatchDVHModule.h"

#include <vtkChart.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkDenseArray.h>
#include <vtkAxis.h>
#include <vtkRenderWindow.h>

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
{
}

//-----------------------------------------------------------------------------
qSlicerPlastimatchDVHModuleWidget::~qSlicerPlastimatchDVHModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::setup()
{
  Q_D(qSlicerPlastimatchDVHModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Set up chart view
  m_ChartView = new ctkVTKChartView(d->widget_chartWidgetContainer);

  QGridLayout* grid = new QGridLayout(d->widget_chartWidgetContainer);
  grid->setContentsMargins(0, 0, 0, 0);
  grid->addWidget(m_ChartView);
  d->widget_chartWidgetContainer->setLayout(grid);

  // Make connections
  connect( d->pushButton_LoadCSV, SIGNAL( pressed() ), this, SLOT( OpenCSV() ) );
  connect( d->checkBox_ShowLegend, SIGNAL( toggled(bool) ), this, SLOT( SetShowLegend(bool) ) );

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
  if (LoadCSV(fileName)) {
    d->pushButton_LoadCSV->setText(fileName);
    d->pushButton_LoadCSV->setToolTip(fileName); 
  } else {
    d->pushButton_LoadCSV->setText( tr("Invalid file!") );
  }
}

//-----------------------------------------------------------------------------
bool qSlicerPlastimatchDVHModuleWidget::LoadCSV(QString fileName)
{
  // Open file
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  QTextStream in(&file);

  // Read structure names
  QString line = in.readLine();
  QStringList names(line.split(","));

  int numberOfColumns = 0;
  QStringListIterator namesIterator(names);
  while (namesIterator.hasNext()) {
    vtkSmartPointer<vtkFloatArray> column = vtkSmartPointer<vtkFloatArray>::New();
    column->SetName(namesIterator.next().toStdString().data());
    table->AddColumn(column);
    ++numberOfColumns;
  }

  // Read lines and determine number of lines
  // Rows: values at a specific cGy (dose) ; Columns: structures
  int numberOfRows = 0;
  QStringList valueLines;
  while (!in.atEnd()) {
    QString line = in.readLine();
    valueLines.append(line);
    ++numberOfRows;
  }
  table->SetNumberOfRows(numberOfRows);

  // Read the values in a matrix
  vtkSmartPointer<vtkDenseArray<double>> matrix = vtkSmartPointer<vtkDenseArray<double>>::New();
  matrix->Resize(numberOfRows, numberOfColumns);

  int rowIndex = 0;
  QStringListIterator valueLinesIterator(valueLines);
  while (valueLinesIterator.hasNext()) {
    QStringList values(valueLinesIterator.next().split(","));
    QStringListIterator valuesIterator(values);

    int columnIndex = 0;
    while (valuesIterator.hasNext()) {
      QString valueString(valuesIterator.next());

      bool ok;
      double value = valueString.toDouble(&ok);
      if (ok) {
        matrix->SetValue(rowIndex, columnIndex, value);
      }

      ++columnIndex;
    }

    ++rowIndex;
  }

  // Fill in the X axis (column 0)
  for (int i=0; i<numberOfRows; ++i) {
    table->SetValue(i, 0, matrix->GetValue(i, 0));
  }

  // Calculate the integral of the values
  for (int j=1; j<numberOfColumns; ++j) {
    vtkSmartPointer<vtkFloatArray> sumArray = vtkSmartPointer<vtkFloatArray>::New();
    sumArray->Resize(numberOfRows);
    double actualSum = 0;

    for (int i=0; i<numberOfRows; ++i) {
      actualSum += matrix->GetValue(i, j);
      sumArray->SetValue(i, actualSum);
    }

    for (int i=0; i<numberOfRows; ++i) {
      table->SetValue(i, j, 100.0 / (sumArray->GetValue(numberOfRows-1) - sumArray->GetValue(0)) * (-sumArray->GetValue(i) + sumArray->GetValue(numberOfRows-1)));
    }
  }

  // Display plot
  m_ChartView->chart()->ClearPlots();
  m_ChartView->chart()->GetAxis(0)->SetTitle("Volume, %");
  m_ChartView->chart()->GetAxis(1)->SetTitle("Dose, Gy");

  vtkPlot *linePlot = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  for (int i=1; i<numberOfColumns; ++i) {
    linePlot->SetInput(table, 0, i);
    linePlot->SetWidth(2.0);
    linePlot = m_ChartView->chart()->AddPlot(vtkChart::LINE);
  }

  return true;
}

//-----------------------------------------------------------------------------
void qSlicerPlastimatchDVHModuleWidget::SetShowLegend(bool show)
{
  m_ChartView->chart()->SetShowLegend(show);
  m_ChartView->GetRenderWindow()->Render();
}
