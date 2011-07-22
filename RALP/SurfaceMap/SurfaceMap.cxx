#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetAttributes.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTubeFilter.h>

#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkMath.h>


int main (int c , char * argv[])
{

  int result = 1;

  if (c < 3)
    {
    std::cerr << "Usage: " << argv[0] << " <Surface File (*.vtk)> <Volume File (*.vtk)>" << std::endl;
    }
  
  const char* surfaceFile = argv[1];
  const char* volumeFile = argv[2];

  // Load sufrace model
  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(surfaceFile);
  if (!reader->IsFilePolyData())
    {
    std::cerr << "File " << surfaceFile << " is not polydata, cannot be read with this reader" << std::endl;
    result = 0;
    }
  reader->Update();
  if (reader->GetOutput() == NULL)
    {
    std::cerr << "Unable to read file " << surfaceFile << std::endl;
    result = 0;
    }

  if (result == 0)
    {
    exit (0);
    }

  vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

  // Load volume image
  vtkSmartPointer<vtkStructuredPointsReader> vreader = vtkSmartPointer<vtkStructuredPointsReader>::New();
  vreader->SetFileName(volumeFile);
  vreader->Update();  
  if (vreader->GetOutput() == NULL)
    {
    std::cerr << "Unable to read file " << volumeFile << std::endl;
    result = 0;
    }
  if (result == 0)
    {
    exit (0);
    }

  int dim[3];
  vreader->GetOutput()->GetDimensions(dim);
  std::cerr << "dimensions = ("
            << dim[0] << ", "
            << dim[1] << ", "
            << dim[2] << ")" << std::endl;

  vtkSmartPointer<vtkStructuredPoints> spoints = vreader->GetOUtput();

  int n = polyData->GetNumberOfPoints();
  vtkSmartPointer<vtkUnsignedCharArray> colors =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetName("Colors");
  colors->SetNumberOfComponents(3);
  colors->SetNumberOfTuples(n);

  for (int i = 0; i < n; i ++)
    {
    double x[3];
    polyData->GetPoint(i, x);
    std::cerr << "Point [" << i << "] = (" << x[0] << ", " << x[1] << ", " << x[2] << ")" << std::endl;
    }

  for (int i = 0; i < n ;i++)
    {
    colors->InsertTuple3(i,
                       int(255 * i/ (n - 1)),
                       0,
                       int(255 * (n - 1 - i)/(n - 1)) );
    }

  polyData->GetPointData()->AddArray(colors);

  //polyData->SetPoints(points);
  //polyData->SetLines(lines);
  // Varying tube radius using sine-function
  //vtkSmartPointer<vtkDoubleArray> tubeRadius =
  //  vtkSmartPointer<vtkDoubleArray>::New();
  //tubeRadius->SetName("TubeRadius");
  //tubeRadius->SetNumberOfTuples(nV);
  //for (i=0 ;i<nV ; i++)
  //  {
  //  tubeRadius->SetTuple1(i,
  //                        rT1 + (rT2 - rT1) * sin(vtkMath::Pi() * i / (nV - 1)));
  //  }
  //polyData->GetPointData()->AddArray(tubeRadius);
  //polyData->GetPointData()->SetActiveScalars("TubeRadius");

  // RBG array (could add Alpha channel too I guess...)
  // Varying from blue to red
  //vtkSmartPointer<vtkUnsignedCharArray> colors =
  //  vtkSmartPointer<vtkUnsignedCharArray>::New();
  //colors->SetName("Colors");
  //colors->SetNumberOfComponents(3);
  //colors->SetNumberOfTuples(nV);
  //for (i = 0; i < nV ;i++)
  //  {
  //  colors->InsertTuple3(i,
  //                     int(255 * i/ (nV - 1)),
  //                     0,
  //                     int(255 * (nV - 1 - i)/(nV - 1)) );
  //  }
  //polyData->GetPointData()->AddArray(colors);

  //vtkSmartPointer<vtkTubeFilter> tube
  //  = vtkSmartPointer<vtkTubeFilter>::New();
  //tube->SetInput(polyData);
  //tube->SetNumberOfSides(nTv);
  //tube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();

  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(polyData);
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUsePointFieldData();
  mapper->SelectColorArray("Colors");

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);
  renderer->SetBackground(.2, .3, .4);

  // Make an oblique view
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();

  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderWindowInteractor>
    iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();

  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(renderer);
  renWin->SetSize(500, 500);
  renWin->Render();

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  iren->SetInteractorStyle(style);

  iren->Start();

  return EXIT_SUCCESS;
}





