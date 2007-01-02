#include "WFConfigEditor.h"

#include <vtkObjectFactory.h>
#include <vtkKWApplication.h>
#include <vtkKWWidget.h>
#include <vtkKWLabel.h>
#include <vtkKWListBoxWithScrollbars.h>

#include <string>

vtkStandardNewMacro( WFConfigEditor );
vtkCxxRevisionMacro(WFConfigEditor, "$Revision: 1.5 $");

WFConfigEditor::WFConfigEditor()
{
 vtkKWApplication *app = vtkKWApplication::New();
   
   app->SetName("WFConfigEditor");
   app->RestoreApplicationSettingsFromRegistry();
   app->PromptBeforeExitOff();

   // Create our wizard dialog and invoke it

   WFConfigEditor *dlg = new WFConfigEditor;
   dlg->SetApplication(app);
   dlg->Create();
      
   dlg->InitializeGUI();
   
   dlg->Invoke();

   dlg->Delete();

   // Deallocate and exit

   app->Exit();
   app->Delete();
   
}

WFConfigEditor::~WFConfigEditor()
{
 
}

void WFConfigEditor::InitializeGUI()
{
 //create Config GUI Elements and add Observers
 
 vtkKWWidget *parent = vtkKWDialog::GetFrame();
 
 wfCfgMCL = vtkKWMultiColumnListWithScrollbars::New();
 wfCfgMCL->SetParent(parent);
 wfCfgMCL->Create();
  //wfCfgLB->
 wfCfgMCL->SetBalloonHelpString("known Workflows");
 
 parent->GetApplication()->Script("pack %s -side top -anchor nws -fill x", 
   wfCfgMCL->GetWidgetName());
 
 // Create a set of pushbutton
 // An easy way to create a bunch of related widgets without allocating
 // them one by one

 this->wfCfgBtnSetBottom = vtkKWPushButtonSet::New();
 this->wfCfgBtnSetBottom->SetParent(parent);
 this->wfCfgBtnSetBottom->Create();
 this->wfCfgBtnSetBottom->SetBorderWidth(2);
 this->wfCfgBtnSetBottom->SetReliefToGroove();
 this->wfCfgBtnSetBottom->SetWidgetsPadX(1);
 this->wfCfgBtnSetBottom->SetWidgetsPadY(1);
 this->wfCfgBtnSetBottom->SetPadX(1);
 this->wfCfgBtnSetBottom->SetPadY(1);
 this->wfCfgBtnSetBottom->ExpandWidgetsOn();
 this->wfCfgBtnSetBottom->SetMaximumNumberOfWidgetsInPackingDirection(1);

 //embed buttons in the set
 this->wfCfgBtnLoad = this->wfCfgBtnSetBottom->AddWidget(0);
 this->wfCfgBtnLoad->SetText("Load Config");
 this->wfCfgBtnSave = this->wfCfgBtnSetBottom->AddWidget(1);
 this->wfCfgBtnSave->SetText("Save Config");
// this->wfCfgBtnSetBottom->Script(
//      "pack %s -side right -anchor nw -expand n -padx 2 -pady 6", 
//      this->wfCfgBtnSave->GetWidgetName());
 this->wfCfgBtnClose = this->wfCfgBtnSetBottom->AddWidget(2);
 this->wfCfgBtnClose->SetText("Close");
// this->wfCfgBtnSetBottom->Script(
//      "pack %s -side right -anchor nw -expand n -padx 2 -pady 6", 
//      this->wfCfgBtnClose->GetWidgetName());
 
//   char buffer[50];
//   for (int id = 0; id < 9; id++)
//     {
//     sprintf(buffer, "Push button %d", id);
//     vtkKWPushButton *pushbutton = pushbutton_set->AddWidget(id);
//     pushbutton->SetText(buffer);
//     pushbutton->SetBackgroundColor(
//       vtkMath::HSVToRGB((double)id / 8.0, 0.3, 0.75));
//     pushbutton->SetBalloonHelpString(
//       "This pushbutton is part of a unique set (a vtkKWPushButtonSet), "
//       "which provides an easy way to create a bunch of related widgets "
//       "without allocating them one by one. The widgets can be layout as a "
//       "NxM grid. Each button is assigned a different color.");
//     }
//
//   pushbutton_set->GetWidget(0)->SetText("I'm the first button");

 parent->GetApplication()->Script(
     "pack %s -side bottom -anchor se -expand n -padx 2 -pady 6", 
     this->wfCfgBtnSetBottom->GetWidgetName());
 
 this->SetSize(500,this->wfCfgMCL->GetWidth() + this->wfCfgBtnSetBottom->GetWidth());
 
}

int my_main(int argc, char *argv[])
{
  
 Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
 if (!interp)
 {
     cerr << "Error: InitializeTcl failed" << endl ;
     return 1;
 }
 
 WFConfigEditor *wfCE = new WFConfigEditor;
 return 0;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
  int argc;
  char **argv;
  vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(
    lpCmdLine, &argc, &argv);
  int ret = my_main(argc, argv);
  for (int i = 0; i < argc; i++) { delete [] argv[i]; }
  delete [] argv;
  return ret;
}
#else
int main(int argc, char *argv[])
{
  return my_main(argc, argv);
}
#endif

