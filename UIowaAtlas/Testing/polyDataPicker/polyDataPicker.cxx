#include "vtkPolyDataMapper.h"
#include "vtkCellPicker.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCommand.h"
#include "vtkCamera.h"
#include "vtkPolyDataReader.h"
#include "vtkActor.h"

#include <sstream>

vtkCellPicker *picker;
vtkActor2D *textActor;
vtkTextMapper *textMapper;
vtkRenderWindow *renWin;

//extern "C" int Annotatepicker_Init(Tcl_Interp *interp);

//void annotatePick()
class AnnotatePick : public vtkCommand
{
public:
  static AnnotatePick *New()
  {
    return new AnnotatePick;
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    if(picker->GetCellId() < 0)
    {
      textActor->VisibilityOff();
    }
    else
    {
      double *selPt = picker->GetSelectionPoint();

      double *pickPos = picker->GetPickPosition();

      std::ostringstream oss;
      oss << "(" << pickPos[0] << ", " << pickPos[1] << ", " << pickPos[2] << ")";

      textMapper->SetInput(oss.str().c_str());
      textActor->SetPosition(selPt[0], selPt[1]);
      textActor->VisibilityOn();
    }
    renWin->Render();
  }
  AnnotatePick() { }
};

int main( int argc, char * argv[] )
{
/*
  // Initialize TCL
  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);

  if(!interp)
  {
    std::cerr << "Error: Initializing TCL failed." << std::endl;
    exit(-1);
  }

  // Initialize our Tcl library (i.e. our classes wrapped in Tcl).
  // This *is* required for the C++ methods to be used as callbacks.
  // See comment at the top of this file.
  Polydatapicker_Init(interp);
*/

  if(argc != 2)
  {
    std::cout << "USAGE: " << argv[0] << " vtkPolyDataToRender" << std::endl;
  }

  vtkPolyDataReader *reader = vtkPolyDataReader::New();
  reader->SetFileName(argv[1]);

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  //vtkCellPicker *picker = vtkCellPicker::New();
  picker = vtkCellPicker::New();
  AnnotatePick * myPick = AnnotatePick::New();
  picker->AddObserver( vtkCommand::EndPickEvent, myPick);

  // Create a text mapper and actor to display the results of picking.
  //vtkTextMapper *textMapper = vtkTextMapper::New();
  textMapper = vtkTextMapper::New();
  vtkTextProperty *tprop = textMapper->GetTextProperty();
    tprop->SetFontFamilyToArial();
    tprop->SetFontSize(10);
    tprop->BoldOn();
    tprop->ShadowOn();
    tprop->SetColor(1.0, 0.0, 0.0);

  //vtkActor2D *textActor = vtkActor2D::New();
  textActor = vtkActor2D::New();
  textActor->VisibilityOff();
  textActor->SetMapper(textMapper);

  // Create the Renderer, RenderWindow, and RenderWindowInteractor
  vtkRenderer *ren1 = vtkRenderer::New();

  //vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren1);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);
    iren->SetPicker(picker);

  // Add the actors to the renderer, set the background and size
  ren1->AddActor2D(textActor);
  ren1->AddActor(actor);
  ren1->SetBackground(1.0, 1.0, 1.0);
  renWin->SetSize(300, 300);

  ren1->ResetCamera();

  vtkCamera *cam1 = ren1->GetActiveCamera();
  cam1->Zoom(1.2);

  iren->Initialize();
  iren->Start();

  picker->Pick(85, 126, 0, ren1);

  picker->Delete();
  myPick->Delete();
  textMapper->Delete();
  textActor->Delete();
  ren1->Delete();
  renWin->Delete();
  iren->Delete();
  reader->Delete();
  mapper->Delete();
  actor->Delete();

  return 0;
}
