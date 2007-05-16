// tcl wrapper for vtkITKBayesianClassificationImageFilter object
//
#define VTK_STREAMS_FWD_ONLY
#include "vtkSystemIncludes.h"
#include "vtkITKBayesianClassificationImageFilter.h"

#include "vtkTclUtil.h"
#include <vtkstd/stdexcept>

ClientData vtkITKBayesianClassificationImageFilterNewCommand()
{
  vtkITKBayesianClassificationImageFilter *temp = vtkITKBayesianClassificationImageFilter::New();
  return ((ClientData)temp);
}

int vtkITKImageToImageFilterUSUSCppCommand(vtkITKImageToImageFilterUSUS *op, Tcl_Interp *interp,
             int argc, char *argv[]);
int VTKTCL_EXPORT vtkITKBayesianClassificationImageFilterCppCommand(vtkITKBayesianClassificationImageFilter *op, Tcl_Interp *interp,
             int argc, char *argv[]);

int VTKTCL_EXPORT vtkITKBayesianClassificationImageFilterCommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[])
{
  if ((argc == 2)&&(!strcmp("Delete",argv[1]))&& !vtkTclInDelete(interp))
    {
    Tcl_DeleteCommand(interp,argv[0]);
    return TCL_OK;
    }
   return vtkITKBayesianClassificationImageFilterCppCommand((vtkITKBayesianClassificationImageFilter *)(((vtkTclCommandArgStruct *)cd)->Pointer),interp, argc, argv);
}

int VTKTCL_EXPORT vtkITKBayesianClassificationImageFilterCppCommand(vtkITKBayesianClassificationImageFilter *op, Tcl_Interp *interp,
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
      if (!strcmp("vtkITKBayesianClassificationImageFilter",argv[1]))
        {
        argv[2] = (char *)((void *)op);
        return TCL_OK;
        }
      if (vtkITKImageToImageFilterUSUSCppCommand((vtkITKImageToImageFilterUSUS *)op,interp,argc,argv) == TCL_OK)
        {
        return TCL_OK;
        }
      }
    return TCL_ERROR;
    }

  if (!strcmp("GetSuperClassName",argv[1]))
    {
    Tcl_SetResult(interp,(char *) "vtkITKImageToImageFilterUSUS", TCL_VOLATILE);
    return TCL_OK;
    }

  try
    {
  if ((!strcmp("New",argv[1]))&&(argc == 2))
    {
    vtkITKBayesianClassificationImageFilter  *temp20;
    int vtkITKBayesianClassificationImageFilterCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    if (!error)
      {
    temp20 = (op)->New();
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkITKBayesianClassificationImageFilterCommand);
      return TCL_OK;
      }
    }
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
    vtkITKBayesianClassificationImageFilter  *temp20;
    int vtkITKBayesianClassificationImageFilterCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    if (!error)
      {
    temp20 = (op)->NewInstance();
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkITKBayesianClassificationImageFilterCommand);
      return TCL_OK;
      }
    }
  if ((!strcmp("SafeDownCast",argv[1]))&&(argc == 3))
    {
    vtkObject  *temp0;
    vtkITKBayesianClassificationImageFilter  *temp20;
    int vtkITKBayesianClassificationImageFilterCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    temp0 = (vtkObject *)(vtkTclGetPointerFromObject(argv[2],(char *) "vtkObject",interp,error));
    if (!error)
      {
    temp20 = (op)->SafeDownCast(temp0);
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkITKBayesianClassificationImageFilterCommand);
      return TCL_OK;
      }
    }
  if ((!strcmp("SetNumberOfClasses",argv[1]))&&(argc == 3))
    {
    int      temp0;
    error = 0;

    if (Tcl_GetInt(interp,argv[2],&tempi) != TCL_OK) error = 1;
    temp0 = tempi;
    if (!error)
      {
    op->SetNumberOfClasses(temp0);
    Tcl_ResetResult(interp);
      return TCL_OK;
      }
    }
  if ((!strcmp("GetNumberOfClasses",argv[1]))&&(argc == 2))
    {
    int      temp20;
    error = 0;

    if (!error)
      {
    temp20 = (op)->GetNumberOfClasses();
    char tempResult[1024];
    sprintf(tempResult,"%i",temp20);
    Tcl_SetResult(interp, tempResult, TCL_VOLATILE);
      return TCL_OK;
      }
    }
  if ((!strcmp("SetNumberOfSmoothingIterations",argv[1]))&&(argc == 3))
    {
    int      temp0;
    error = 0;

    if (Tcl_GetInt(interp,argv[2],&tempi) != TCL_OK) error = 1;
    temp0 = tempi;
    if (!error)
      {
    op->SetNumberOfSmoothingIterations(temp0);
    Tcl_ResetResult(interp);
      return TCL_OK;
      }
    }
  if ((!strcmp("GetNumberOfSmoothingIterations",argv[1]))&&(argc == 2))
    {
    int      temp20;
    error = 0;

    if (!error)
      {
    temp20 = (op)->GetNumberOfSmoothingIterations();
    char tempResult[1024];
    sprintf(tempResult,"%i",temp20);
    Tcl_SetResult(interp, tempResult, TCL_VOLATILE);
      return TCL_OK;
      }
    }
  if ((!strcmp("SetMaskImage",argv[1]))&&(argc == 3))
    {
    vtkImageData  *temp0;
    error = 0;

    temp0 = (vtkImageData *)(vtkTclGetPointerFromObject(argv[2],(char *) "vtkImageData",interp,error));
    if (!error)
      {
    op->SetMaskImage(temp0);
    Tcl_ResetResult(interp);
      return TCL_OK;
      }
    }
  if ((!strcmp("GetMaskImage",argv[1]))&&(argc == 2))
    {
    vtkImageData  *temp20;
    int vtkImageDataCommand(ClientData, Tcl_Interp *, int, char *[]);
    error = 0;

    if (!error)
      {
    temp20 = (op)->GetMaskImage();
      vtkTclGetObjectFromPointer(interp,(void *)temp20,vtkImageDataCommand);
      return TCL_OK;
      }
    }
  if ((!strcmp("SetMaskValue",argv[1]))&&(argc == 3))
    {
    int      temp0;
    error = 0;

    if (Tcl_GetInt(interp,argv[2],&tempi) != TCL_OK) error = 1;
    temp0 = tempi;
    if (!error)
      {
    op->SetMaskValue(temp0);
    Tcl_ResetResult(interp);
      return TCL_OK;
      }
    }
  if ((!strcmp("GetMaskValue",argv[1]))&&(argc == 2))
    {
    int      temp20;
    error = 0;

    if (!error)
      {
    temp20 = (op)->GetMaskValue();
    char tempResult[1024];
    sprintf(tempResult,"%i",temp20);
    Tcl_SetResult(interp, tempResult, TCL_VOLATILE);
      return TCL_OK;
      }
    }

  if (!strcmp("ListInstances",argv[1]))
    {
    vtkTclListInstances(interp,(ClientData)vtkITKBayesianClassificationImageFilterCommand);
    return TCL_OK;
    }

  if (!strcmp("ListMethods",argv[1]))
    {
    vtkITKImageToImageFilterUSUSCppCommand(op,interp,argc,argv);
    Tcl_AppendResult(interp,"Methods from vtkITKBayesianClassificationImageFilter:\n",NULL);
    Tcl_AppendResult(interp,"  GetSuperClassName\n",NULL);
    Tcl_AppendResult(interp,"  New\n",NULL);
    Tcl_AppendResult(interp,"  GetClassName\n",NULL);
    Tcl_AppendResult(interp,"  IsA\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  NewInstance\n",NULL);
    Tcl_AppendResult(interp,"  SafeDownCast\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  SetNumberOfClasses\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  GetNumberOfClasses\n",NULL);
    Tcl_AppendResult(interp,"  SetNumberOfSmoothingIterations\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  GetNumberOfSmoothingIterations\n",NULL);
    Tcl_AppendResult(interp,"  SetMaskImage\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  GetMaskImage\n",NULL);
    Tcl_AppendResult(interp,"  SetMaskValue\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  GetMaskValue\n",NULL);
    return TCL_OK;
    }

  if (vtkITKImageToImageFilterUSUSCppCommand((vtkITKImageToImageFilterUSUS *)op,interp,argc,argv) == TCL_OK)
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
