#include "vtkTclUtil.h"
#include "vtkVersion.h"
#define VTK_TCL_TO_STRING(x) VTK_TCL_TO_STRING0(x)
#define VTK_TCL_TO_STRING0(x) #x
extern "C"
{
#if (TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)
  typedef int (*vtkTclCommandType)(ClientData, Tcl_Interp *,int, CONST84 char *[]);
#else
  typedef int (*vtkTclCommandType)(ClientData, Tcl_Interp *,int, char *[]);
#endif
}

int vtkNeuroEndoscopicNavigationLogicCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkNeuroEndoscopicNavigationLogicNewCommand();
int vtkNeuroEndoscopicNavigationGUICommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[]);
ClientData vtkNeuroEndoscopicNavigationGUINewCommand();

extern Tcl_HashTable vtkInstanceLookup;
extern Tcl_HashTable vtkPointerLookup;
extern Tcl_HashTable vtkCommandLookup;
extern void vtkTclDeleteObjectFromHash(void *);
extern void vtkTclListInstances(Tcl_Interp *interp, ClientData arg);


extern "C" {int VTK_EXPORT Neuroendoscopicnavigation_SafeInit(Tcl_Interp *interp);}

extern "C" {int VTK_EXPORT Neuroendoscopicnavigation_Init(Tcl_Interp *interp);}

extern void vtkTclGenericDeleteObject(ClientData cd);


int VTK_EXPORT Neuroendoscopicnavigation_SafeInit(Tcl_Interp *interp)
{
  return Neuroendoscopicnavigation_Init(interp);
}


int VTK_EXPORT Neuroendoscopicnavigation_Init(Tcl_Interp *interp)
{

  vtkTclCreateNew(interp,const_cast<char *>("vtkNeuroEndoscopicNavigationLogic"), vtkNeuroEndoscopicNavigationLogicNewCommand,
                  vtkNeuroEndoscopicNavigationLogicCommand);
  vtkTclCreateNew(interp,const_cast<char *>("vtkNeuroEndoscopicNavigationGUI"), vtkNeuroEndoscopicNavigationGUINewCommand,
                  vtkNeuroEndoscopicNavigationGUICommand);
  char pkgName[]="NeuroEndoscopicNavigation";
  char pkgVers[]=VTK_TCL_TO_STRING(VTK_MAJOR_VERSION) "." VTK_TCL_TO_STRING(VTK_MINOR_VERSION);
  Tcl_PkgProvide(interp, pkgName, pkgVers);
  return TCL_OK;
}
