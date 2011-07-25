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

  int n = polyData->GetNumberOfCells();
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
    polyData->GetCellPoints(i, il);
    //std::cerr << "Point [" << i << "] = (" << x[0] << ", " << x[1] << ", " << x[2] << ")" << std::endl;
    int m = il->GetNumberOfIds();
    double ax[3];
    ax[0] = 0.0;
    ax[1] = 0.0;
    ax[2] = 0.0;

    //std::cerr << "# of points: " << m << ", cell type = " << polyData->GetCell(i)->GetCellType() << std::endl;

    for (int j = 0; j < m; j ++)
      {
      double x[3];
      polyData->GetPoint(il->GetId(j), x);
      //std::cerr << "Point [" << il->GetId(j) << "] = (" << x[0] << ", " << x[1] << ", " << x[2] << ")" << std::endl;
      ax[0] += x[0];
      ax[1] += x[1];
      ax[2] += x[2];
      }

    ax[0] /= (double)m;
    ax[1] /= (double)m;
    ax[2] /= (double)m;
    
    //int id = spoints->FindPoint(x[0], x[1], x[2]);
    int id = spoints->FindPoint(ax[0], ax[1], ax[2]);
    //std::cerr << "Point [" << id << "] = (" << ax[0] << ", " << ax[1] << ", " << ax[2] << ")" << std::endl;
    int v = (int)pd->GetComponent(id, 0);
    
    // GetCellNeighbors(id, vtkIDLists, vtkIDList.)
    
    double intensity = (v - low) * scale;
    if (intensity > 255.0) intensity = 255.0;
    unsigned char cv = (unsigned char) intensity;

    colors->InsertTuple3(i, cv, cv, cv);

    }

  polyData->GetCellData()->SetScalars(colors);

  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(polyData);
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUseCellFieldData();
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





