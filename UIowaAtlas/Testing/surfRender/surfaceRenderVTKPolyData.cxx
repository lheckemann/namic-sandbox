#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

int main( int argc, char * argv[] )
{

  if( argc != 2 ) {
    std::cout<<"USAGE: "<<argv[0]<<" surfaceToRender.vtk"<<std::endl;
    exit(-1);
  }

  vtkPolyDataReader * reader = vtkPolyDataReader::New();

  reader->SetFileName(argv[1]);

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();

  mapper->SetInputConnection(reader->GetOutputPort());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  vtkRenderer *rend1=vtkRenderer::New();
  rend1->AddActor( actor );
  rend1->SetBackground(0.1, 0.2, 0.4);
  
  vtkRenderWindow *rendWin=vtkRenderWindow::New();
  rendWin->AddRenderer(rend1);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(rendWin);

  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(style);

  iren->Initialize();
  iren->Start();

  reader->Delete();
  mapper->Delete();
  actor->Delete();
  rend1->Delete();
  rendWin->Delete();
  iren->Delete();
  style->Delete();

  return 0;
}
