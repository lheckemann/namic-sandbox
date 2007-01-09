#include "vtkWFDynamicWizard.h"

#include "vtkKWApplication.h"
#include "vtkKWLabel.h"

// We define several classes in this example, and we want to be able to use
// their C++ methods as callbacks for our user interface. To do so, we created
// a library and wrapped it automatically for the Tcl language, which is used
// as a bridge between C++ objects at run-time. An initialization function is
// automatically created in this library to allow classes and C++ methods to
// be used as commands and callbacks inside the Tcl interpreter; let's *not* 
// forget to declare *and* call this function right after we initialize the Tcl
// interpreter in our application. The name of this function is built from the
// library name in lower-case (except for the first letter) and suffixed with
// "_Init" (for example: KWCallbacksExampleLib => Kwcallbacksexamplelib_Init).
// This whole process is required to use C++ methods as callbacks; it is not
// needed if you use VTK's C++ command/observer pattern directly, which is
// demonstrated in KWWidgets's C++ 'Callbacks' example.

extern "C" int Wfdynamicwizardlib_Init(Tcl_Interp *interp);

int my_main(int argc, char *argv[])
{
  // Initialize Tcl

  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
  if (!interp)
    {
    cerr << "Error: InitializeTcl failed" << endl ;
    return 1;
    }

  // Initialize our Tcl library (i.e. our classes wrapped in Tcl).
  // This *is* required for the C++ methods to be used as callbacks.
  // See comment at the top of this file.

  Wfdynamicwizardlib_Init(interp);

  // Create the application
  // If --test was provided, ignore all registry settings, and exit silently
  // Restore the settings that have been saved to the registry, like
  // the geometry of the user interface so far.
  
  vtkKWApplication *app = vtkKWApplication::New();
  app->SetName("WFDynamicClient");
  app->RestoreApplicationSettingsFromRegistry();
  app->PromptBeforeExitOff();
  
  vtkWFDynamicWizard *wfTW = vtkWFDynamicWizard::New();
  wfTW->SetApplication(app);
  wfTW->Create();
  if(wfTW->ConnectToWFEngine())
      wfTW->Invoke();
  
  wfTW->SaveState();
  wfTW->Delete();
  // Start the application
  // If --test was provided, do not enter the event loop and run this example
  // as a non-interactive test for software quality purposes.
  
  app->Exit();
  app->Delete();
  
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
