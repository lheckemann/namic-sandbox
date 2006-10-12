// tcl wrapper for vtkITKImageToImageFilterULUL object
//
#define VTK_STREAMS_FWD_ONLY
#include "vtkSystemIncludes.h"
#include "vtkITKImageToImageFilterULUL.h"

#include "vtkTclUtil.h"
#include <vtkstd/stdexcept>
int vtkITKImageToImageFilterCppCommand(vtkITKImageToImageFilter *op, Tcl_Interp *interp,
             int argc, char *argv[]);
int VTKTCL_EXPORT vtkITKImageToImageFilterULULCppCommand(vtkITKImageToImageFilterULUL *op, Tcl_Interp *interp,
             int argc, char *argv[]);

int VTKTCL_EXPORT vtkITKImageToImageFilterULULCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[])
{
  if ((argc == 2)&&(!strcmp("Delete",argv[1]))&& !vtkTclInDelete(interp))
    {
    Tcl_DeleteCommand(interp,argv[0]);
    return TCL_OK;
    }
   return vtkITKImageToImageFilterULULCppCommand((vtkITKImageToImageFilterULUL *)(((vtkTclCommandArgStruct *)cd)->Pointer),interp, argc, argv);
}

int VTKTCL_EXPORT vtkITKImageToImageFilterULULCppCommand(vtkITKImageToImageFilterULUL *op, Tcl_Interp *interp,
             int argc, char *argv[])
{
  int    tempi;
  double tempd;
  static char temps[80];
  int    error;

  error = 0; error = error;
  tempi = 0; tempi = tempi;
  tempd = 0; tempd = tempd;
  temps[0] = 0; temps[0] = temps[0];

  if (argc < 2)
    {
    Tcl_SetResult(interp, (char *) "Could not find requested method.", TCL_VOLATILE);
    return TCL_ERROR;
    }
  if (!interp)
    {
    if (!strcmp("DoTypecasting",argv[0]))
      {
      if (!strcmp("vtkITKImageToImageFilterULUL",argv[1]))
        {
        argv[2] = (char *)((void *)op);
        return TCL_OK;
        }
      if (vtkITKImageToImageFilterCppCommand((vtkITKImageToImageFilter *)op,interp,argc,argv) == TCL_OK)
        {
        return TCL_OK;
        }
      }
    return TCL_ERROR;
    }

  if (!strcmp("GetSuperClassName",argv[1]))
    {
    Tcl_SetResult(interp,(char *) "vtkITKImageToImageFilter", TCL_VOLATILE);
    return TCL_OK;
    }

  try
    {
  if ((!strcmp("GetClassName",argv[1]))&&(argc == 2))
    {
    const char    *temp20;
    error = 0;

    if (!error)
      {
    temp20 = (op)->GetClassName();
    if (temp20)
      {
      Tcl_SetResult(interp, (char*)temp20, TCL_VOLATILE);
      }
    else
      {
      Tcl_ResetResult(interp);
      }
      return TCL_OK;
      }
    }
  if ((!strcmp("IsA",argv[1]))&&(argc == 3))
    {
    char    *temp0;
    int      temp20;
    error = 0;

    temp0 = argv[2];
    if (!error)
      {
    temp20 = (op)->IsA(temp0);
    char tempResult[1024];
    sprintf(tempResult,"%i",temp20);
    Tcl_SetResult(interp, tempResult, TCL_VOLATILE);
      return TCL_OK;
      }
    }
  if ((!strcmp("NewInstance",argv[1]))&&(argc == 2))
    {
    vtkITKImageToImageFilterULUL  *temp20;
    int vtkITKImageToImageFilterULULCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    if (!error)
      {
    temp20 = (op)->NewInstance();
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkITKImageToImageFilterULULCommand);
      return TCL_OK;
      }
    }
  if ((!strcmp("New",argv[1]))&&(argc == 2))
    {
    vtkITKImageToImageFilterULUL  *temp20;
    int vtkITKImageToImageFilterULULCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    if (!error)
      {
    temp20 = (op)->New();
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkITKImageToImageFilterULULCommand);
      return TCL_OK;
      }
    }

  if (!strcmp("ListInstances",argv[1]))
    {
    vtkTclListInstances(interp,(ClientData)vtkITKImageToImageFilterULULCommand);
    return TCL_OK;
    }

  if (!strcmp("ListMethods",argv[1]))
    {
    vtkITKImageToImageFilterCppCommand(op,interp,argc,argv);
    Tcl_AppendResult(interp,"Methods from vtkITKImageToImageFilterULUL:\n",NULL);
    Tcl_AppendResult(interp,"  GetSuperClassName\n",NULL);
    Tcl_AppendResult(interp,"  GetClassName\n",NULL);
    Tcl_AppendResult(interp,"  IsA\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  NewInstance\n",NULL);
    Tcl_AppendResult(interp,"  New\n",NULL);
    return TCL_OK;
    }

  if (vtkITKImageToImageFilterCppCommand((vtkITKImageToImageFilter *)op,interp,argc,argv) == TCL_OK)
    {
    return TCL_OK;
    }

  if ((argc >= 2)&&(!strstr(interp->result,"Object named:")))
    {
    char temps2[256];
    sprintf(temps2,"Object named: %s, could not find requested method: %s\nor the method was called with incorrect arguments.\n",argv[0],argv[1]);
    Tcl_AppendResult(interp,temps2,NULL);
    }
    }
  catch (vtkstd::exception &e)
    {
    Tcl_AppendResult(interp, "Uncaught exception: ",  e.what(), "\n", NULL);
    return TCL_ERROR;
    }
  return TCL_ERROR;
}
