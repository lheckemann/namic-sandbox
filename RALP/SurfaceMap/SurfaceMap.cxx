#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
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

  if (c < 0)
    {
    std::cerr << "Usage: " << argv[0] << " <File name (.vtk)>" << std::endl;
    }

  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(argv[1]);
  if (!reader->IsFilePolyData())
    {
    std::cerr << "File " << argv[1] << " is not polydata, cannot be read with this reader" << std::endl;
    result = 0;
    }
  reader->Update();
  if (reader->GetOutput() == NULL)
    {
    std::cerr << "Unable to read file " << argv[1] << std::endl;
    result = 0;
    }

  if (result == 0)
    {
    exit (0);
    }

  
  vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
  


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
  //mapper->SetInputConnection(tube->GetOutputPort());
  mapper->SetInput(reader->GetOutput());
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUsePointFieldData();
  //mapper->SelectColorArray("Colors");

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





