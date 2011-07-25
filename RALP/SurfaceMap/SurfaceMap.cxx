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

#include <vtkPolyDataNormals.h>
#include <vtkDensifyPolyData.h>

#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkMath.h>

double TrilinearInterpolation(vtkStructuredPoints * spoints, double x[3]);


int main (int c , char * argv[])
{

  int result = 1;

  if (c < 5)
    {
    std::cerr << "Usage: " << argv[0] << " <Surface File (*.vtk)> <Volume File (*.vtk)> <Window> <Level>" << std::endl;
    }
  
  const char*  surfaceFile = argv[1];
  const char*  volumeFile = argv[2];
  const double window = (double)atoi(argv[3]);
  const double level  = (double)atoi(argv[4]);

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

  // Split triangl strips into triangles
  vtkSmartPointer<vtkPolyDataNormals> polyDataNormals 
    = vtkSmartPointer<vtkPolyDataNormals>::New();

  polyDataNormals->SetInput(reader->GetOutput());
  polyDataNormals->SplittingOn();
  //polyDataNormals->ComputePointNormalsOff();
  //polyDataNormals->ComputeCellNormalsOff();
  
  polyDataNormals->Update();

  //// Densify the polygons
  //vtkSmartPointer<vtkDensifyPolyData> polyDataDensified
  //  = vtkSmartPointer<vtkDensifyPolyData>::New();
  //
  //polyDataDensified->SetInput(polyDataNormals->GetOutput());
  //polyDataDensified->SetNumberOfSubdivisions(1);
  //polyDataDensified->Update();

  vtkSmartPointer<vtkPolyData> polyData = polyDataNormals->GetOutput();
  //vtkSmartPointer<vtkPolyData> polyData = polyDataDensified->GetOutput();
  //vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

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

  vtkSmartPointer<vtkStructuredPoints> spoints = vreader->GetOutput();

  spoints->GetDimensions(dim);
  std::cerr << "dimensions = ("
            << dim[0] << ", "
            << dim[1] << ", "
            << dim[2] << ")" << std::endl;

  int n = polyData->GetNumberOfPoints();
  vtkSmartPointer<vtkUnsignedCharArray> colors =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetName("Colors");
  colors->SetNumberOfComponents(3);
  colors->SetNumberOfTuples(n);

  vtkSmartPointer<vtkDataArray> pd = spoints->GetPointData()->GetScalars();
  int t = spoints->GetScalarType();
  std::cerr << "Scalr type: " << t << std::endl;
  std::cerr << "Number of Points: " << spoints->GetNumberOfPoints() << std::endl;
  std::cerr << "Number of Cells: "  << spoints->GetNumberOfCells() << std::endl;

  const double low   = level - window/2.0;
  const double scale = 255.0 / window;

  vtkSmartPointer<vtkIdList> il = vtkSmartPointer<vtkIdList>::New();

  for (int i = 0; i < n; i ++)
    {
    double x[3];
    polyData->GetPoint(i, x);

    double value = TrilinearInterpolation(spoints, x);
    double intensity = (value - low) * scale;
    if (intensity > 255.0)
      {
      intensity = 255.0;
      }
    else if (intensity < 0.0)
      {
      intensity = 0.0;
      }
    unsigned char cv = (unsigned char) intensity;

    colors->InsertTuple3(i, cv, cv, cv);

    }
  
  polyData->GetPointData()->SetScalars(colors);

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



double TrilinearInterpolation(vtkStructuredPoints * spoints, double x[3])
{
  int    ijk[3];
  double r[3];
  if (spoints->ComputeStructuredCoordinates(x, ijk, r) == 0)
    {
    std::cerr << "Point (" << x[0] << ", " << x[1] << ", " << x[2]
              << ") is outside of the volume." << std::endl;
    return 0.0;
    }
  
  int ii = ijk[0];    int jj = ijk[1];    int kk = ijk[2];
  double v000 = (double)*((short*)spoints->GetScalarPointer(ii,   jj,   kk  ));
  double v100 = (double)*((short*)spoints->GetScalarPointer(ii+1, jj,   kk  ));
  double v010 = (double)*((short*)spoints->GetScalarPointer(ii,   jj+1, kk  ));
  double v110 = (double)*((short*)spoints->GetScalarPointer(ii+1, jj+1, kk  ));
  double v001 = (double)*((short*)spoints->GetScalarPointer(ii,   jj,   kk+1));
  double v101 = (double)*((short*)spoints->GetScalarPointer(ii+1, jj,   kk+1));
  double v011 = (double)*((short*)spoints->GetScalarPointer(ii,   jj+1, kk+1));
  double v111 = (double)*((short*)spoints->GetScalarPointer(ii+1, jj+1, kk+1));
  
  double r0 = r[0]; double r1 = r[1]; double r2 = r[2];
  double value =
    v000 * (1-r0) * (1-r1) * (1-r2) +
    v100 *    r0  * (1-r1) * (1-r2) +
    v010 * (1-r0) *    r1  * (1-r2) +
    v001 * (1-r0) * (1-r1) *    r2  +
    v101 *    r0  * (1-r1) *    r2  +
    v011 * (1-r0) *    r1  *    r2  +
    v110 *    r0  *    r1  * (1-r2) +
    v111 *    r0  *    r1  *    r2;

  return value;

}


  
