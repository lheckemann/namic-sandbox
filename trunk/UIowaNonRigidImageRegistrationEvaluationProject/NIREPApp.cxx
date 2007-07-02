
//TODO: Fix callback function to SetSlice method. 
//      Replace vtkNIREPGridViewerWithScrollBar with vtkKWWindowsWithScrollbar
//      Get the Anaylze file to appear on the screen again
//      Add XML parser, to retrieve header/display information
//      Test the 3 orientations
/*=========================================================================
Module:$RCSfile: vtkKWNIREPGridImageViewer.h,v $
Copyright (c) NIREP, James Harris, Jeff Hawley
All rights reserved
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
#include "vtkKWApplication.h"
#include "vtkKWWindowBase.h"
#include "vtkKWLabel.h"
#include "vtkKWFrame.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>

#include <vtkImageViewer2.h> 
#include <vtkKWRenderWidget.h>
#include <vtkXMLImageDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkKWSplitFrame.h> 

#include "vtkKWNIREPGridImageViewer.h"

int MainApp(int argc, char *argv[], int option_test)
{
vtkKWApplication *app = vtkKWApplication::New();
app->SetName("NIREP");
if (option_test)
{
app->SetRegistryLevel(0);
app->PromptBeforeExitOff();
}
app->RestoreApplicationSettingsFromRegistry();
vtkKWWindowBase *win = vtkKWWindowBase::New();
app->AddWindow(win);
win->Create();
vtkKWNIREPGridImageViewer * grid = vtkKWNIREPGridImageViewer::New();
grid->SetParent(win->GetViewFrame());
grid->SetRowSize(2);
grid->SetColSize(2);
grid->Create();
grid->Add(std::string("na01.hdr"), 0, 0);
 grid->Add(std::string("na01.hdr"), 0, 1);
  grid->Add(std::string("na01.hdr"), 1, 0);
  grid->Add(std::string("na01.hdr"), 1, 1);

grid->SetRowLabel("Sagittal", 0);
grid->SetRowLabel("Transverse", 1);

grid->SetColumnLabel("Subject A", 0);
grid->SetColumnLabel("Subject B", 1);

grid->Update();

int ret = 0;
win->Display();
if (!option_test)
{
app->Start(argc, argv);
ret = app->GetExitStatus();
}
win->Close();

// Deallocate and exit 
grid->Delete();
//notebookex->Delete();
win->Delete();
app->Delete();
return ret;
}

int Start(int argc, char *argv[])
{
Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
if (!interp)
{
cerr << "Error: InitializeTcl failed" << endl ;
return 1;
}
int option_test = 0;
vtksys::CommandLineArguments args;
args.Initialize(argc, argv);
args.AddArgument(
"--test", vtksys::CommandLineArguments::NO_ARGUMENT, &option_test, "");
args.Parse();
int ret = MainApp(argc, argv, option_test);
return ret;
}
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
int argc;
char **argv;
vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(lpCmdLine, &argc, &argv);
int ret = Start(argc, argv);
for (int i = 0; i < argc; i++) { delete [] argv[i]; }
delete [] argv;
return ret;
}
#else
int main(int argc, char *argv[])
{
return Start(argc, argv);
}
#endif
