// tcl wrapper for vtkHybridNavGUI object
//
#define VTK_WRAPPING_CXX
#define VTK_STREAMS_FWD_ONLY
#include "vtkSystemIncludes.h"
#include "vtkHybridNavGUI.h"

#include "vtkTclUtil.h"
#include <vtkstd/stdexcept>
#include <vtksys/ios/sstream>

ClientData vtkHybridNavGUINewCommand()
{
  vtkHybridNavGUI *temp = vtkHybridNavGUI::New();
  return static_cast<ClientData>(temp);
}

int vtkSlicerModuleGUICppCommand(vtkSlicerModuleGUI *op, Tcl_Interp *interp,
             int argc, char *argv[]);
int VTKTCL_EXPORT vtkHybridNavGUICppCommand(vtkHybridNavGUI *op, Tcl_Interp *interp,
             int argc, char *argv[]);

int VTKTCL_EXPORT vtkHybridNavGUICommand(ClientData cd, Tcl_Interp *interp,
             int argc, char *argv[])
{
  if ((argc == 2)&&(!strcmp("Delete",argv[1]))&& !vtkTclInDelete(interp))
    {
    Tcl_DeleteCommand(interp,argv[0]);
    return TCL_OK;
    }
   return vtkHybridNavGUICppCommand(static_cast<vtkHybridNavGUI *>(static_cast<vtkTclCommandArgStruct *>(cd)->Pointer),interp, argc, argv);
}

int VTKTCL_EXPORT vtkHybridNavGUICppCommand(vtkHybridNavGUI *op, Tcl_Interp *interp,
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
    Tcl_SetResult(interp,const_cast<char *>("Could not find requested method."), TCL_VOLATILE);
    return TCL_ERROR;
    }
  if (!interp)
    {
    if (!strcmp("DoTypecasting",argv[0]))
      {
      if (!strcmp("vtkHybridNavGUI",argv[1]))
        {
        argv[2] = static_cast<char *>(static_cast<void *>(op));
        return TCL_OK;
        }
      if (vtkSlicerModuleGUICppCommand(static_cast<vtkSlicerModuleGUI *>(op),interp,argc,argv) == TCL_OK)
        {
        return TCL_OK;
        }
      }
    return TCL_ERROR;
    }

  if (!strcmp("GetSuperClassName",argv[1]))
    {
    Tcl_SetResult(interp,const_cast<char *>("vtkSlicerModuleGUI"), TCL_VOLATILE);
    return TCL_OK;
    }

  try
    {
  if ((!strcmp("GetClassName",argv[1]))&&(argc == 2))
    {
    const char    *temp20;
    temp20 = (op)->GetClassName();
    if (temp20)
      {
      Tcl_SetResult(interp, const_cast<char *>(temp20), TCL_VOLATILE);
      }
    else
      {
      Tcl_ResetResult(interp);
      }
    return TCL_OK;
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
    vtkHybridNavGUI  *temp20;
    temp20 = (op)->NewInstance();
      vtkTclGetObjectFromPointer(interp,(void *)(temp20),"vtkHybridNavGUI");
    return TCL_OK;
    }
  if ((!strcmp("SafeDownCast",argv[1]))&&(argc == 3))
    {
    vtkObject  *temp0;
    vtkHybridNavGUI  *temp20;
    error = 0;

    temp0 = (vtkObject *)(vtkTclGetPointerFromObject(argv[2],const_cast<char *>("vtkObject"),interp,error));
    if (!error)
    {
    temp20 = (op)->SafeDownCast(temp0);
      vtkTclGetObjectFromPointer(interp,(void *)(temp20),"vtkHybridNavGUI");
    return TCL_OK;
    }
    }
  if ((!strcmp("GetLogic",argv[1]))&&(argc == 2))
    {
    vtkHybridNavLogic  *temp20;
    temp20 = (op)->GetLogic();
      vtkTclGetObjectFromPointer(interp,(void *)(temp20),"vtkHybridNavLogic");
    return TCL_OK;
    }
  if ((!strcmp("SetModuleLogic",argv[1]))&&(argc == 3))
    {
    vtkSlicerLogic  *temp0;
    error = 0;

    temp0 = (vtkSlicerLogic *)(vtkTclGetPointerFromObject(argv[2],const_cast<char *>("vtkSlicerLogic"),interp,error));
    if (!error)
    {
    op->SetModuleLogic(temp0);
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
    }
  if ((!strcmp("New",argv[1]))&&(argc == 2))
    {
    vtkHybridNavGUI  *temp20;
    temp20 = (op)->New();
      vtkTclGetObjectFromPointer(interp,(void *)(temp20),"vtkHybridNavGUI");
    return TCL_OK;
    }
  if ((!strcmp("Init",argv[1]))&&(argc == 2))
    {
    op->Init();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("Enter",argv[1]))&&(argc == 2))
    {
    op->Enter();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("Exit",argv[1]))&&(argc == 2))
    {
    op->Exit();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("AddGUIObservers",argv[1]))&&(argc == 2))
    {
    op->AddGUIObservers();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("RemoveGUIObservers",argv[1]))&&(argc == 2))
    {
    op->RemoveGUIObservers();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("AddLogicObservers",argv[1]))&&(argc == 2))
    {
    op->AddLogicObservers();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("RemoveLogicObservers",argv[1]))&&(argc == 2))
    {
    op->RemoveLogicObservers();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("ProcessTimerEvents",argv[1]))&&(argc == 2))
    {
    op->ProcessTimerEvents();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("HandleMouseEvent",argv[1]))&&(argc == 3))
    {
    vtkSlicerInteractorStyle  *temp0;
    error = 0;

    temp0 = (vtkSlicerInteractorStyle *)(vtkTclGetPointerFromObject(argv[2],const_cast<char *>("vtkSlicerInteractorStyle"),interp,error));
    if (!error)
    {
    op->HandleMouseEvent(temp0);
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
    }
  if ((!strcmp("BuildGUI",argv[1]))&&(argc == 2))
    {
    op->BuildGUI();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("BuildGUIForHelpFrame",argv[1]))&&(argc == 2))
    {
    op->BuildGUIForHelpFrame();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("BuildGUIForToolFrame",argv[1]))&&(argc == 2))
    {
    op->BuildGUIForToolFrame();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("BuildGUIForCalibrationFrame",argv[1]))&&(argc == 2))
    {
    op->BuildGUIForCalibrationFrame();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }
  if ((!strcmp("UpdateAll",argv[1]))&&(argc == 2))
    {
    op->UpdateAll();
    Tcl_ResetResult(interp);
    return TCL_OK;
    }

  if (!strcmp("ListInstances",argv[1]))
    {
    vtkTclListInstances(interp,(ClientData)(vtkHybridNavGUICommand));
    return TCL_OK;
    }

  if (!strcmp("ListMethods",argv[1]))
    {
    vtkSlicerModuleGUICppCommand(op,interp,argc,argv);
    Tcl_AppendResult(interp,"Methods from vtkHybridNavGUI:\n",NULL);
    Tcl_AppendResult(interp,"  GetSuperClassName\n",NULL);
    Tcl_AppendResult(interp,"  GetClassName\n",NULL);
    Tcl_AppendResult(interp,"  IsA\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  NewInstance\n",NULL);
    Tcl_AppendResult(interp,"  SafeDownCast\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  GetLogic\n",NULL);
    Tcl_AppendResult(interp,"  SetModuleLogic\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  New\n",NULL);
    Tcl_AppendResult(interp,"  Init\n",NULL);
    Tcl_AppendResult(interp,"  Enter\n",NULL);
    Tcl_AppendResult(interp,"  Exit\n",NULL);
    Tcl_AppendResult(interp,"  AddGUIObservers\n",NULL);
    Tcl_AppendResult(interp,"  RemoveGUIObservers\n",NULL);
    Tcl_AppendResult(interp,"  AddLogicObservers\n",NULL);
    Tcl_AppendResult(interp,"  RemoveLogicObservers\n",NULL);
    Tcl_AppendResult(interp,"  ProcessTimerEvents\n",NULL);
    Tcl_AppendResult(interp,"  HandleMouseEvent\t with 1 arg\n",NULL);
    Tcl_AppendResult(interp,"  BuildGUI\n",NULL);
    Tcl_AppendResult(interp,"  BuildGUIForHelpFrame\n",NULL);
    Tcl_AppendResult(interp,"  BuildGUIForToolFrame\n",NULL);
    Tcl_AppendResult(interp,"  BuildGUIForCalibrationFrame\n",NULL);
    Tcl_AppendResult(interp,"  UpdateAll\n",NULL);
    return TCL_OK;
    }

  if (!strcmp("DescribeMethods",argv[1]))
    {
    if(argc>3) {
      Tcl_SetResult ( interp, const_cast<char*>("Wrong number of arguments: object DescribeMethods <MethodName>"), TCL_VOLATILE ); 
      return TCL_ERROR;
 }
    if(argc==2) {

  Tcl_DString dString, dStringParent;

  Tcl_DStringInit ( &dString );

  Tcl_DStringInit ( &dStringParent );
    vtkSlicerModuleGUICppCommand(op,interp,argc,argv);
    Tcl_DStringGetResult ( interp, &dStringParent );
    Tcl_DStringAppend ( &dString, Tcl_DStringValue ( &dStringParent ), -1 );
    Tcl_DStringAppendElement ( &dString, "GetClassName" );
    Tcl_DStringAppendElement ( &dString, "IsA" );
    Tcl_DStringAppendElement ( &dString, "NewInstance" );
    Tcl_DStringAppendElement ( &dString, "SafeDownCast" );
    Tcl_DStringAppendElement ( &dString, "GetLogic" );
    Tcl_DStringAppendElement ( &dString, "SetModuleLogic" );
    Tcl_DStringAppendElement ( &dString, "New" );
    Tcl_DStringAppendElement ( &dString, "Init" );
    Tcl_DStringAppendElement ( &dString, "Enter" );
    Tcl_DStringAppendElement ( &dString, "Exit" );
    Tcl_DStringAppendElement ( &dString, "AddGUIObservers" );
    Tcl_DStringAppendElement ( &dString, "RemoveGUIObservers" );
    Tcl_DStringAppendElement ( &dString, "AddLogicObservers" );
    Tcl_DStringAppendElement ( &dString, "RemoveLogicObservers" );
    Tcl_DStringAppendElement ( &dString, "ProcessTimerEvents" );
    Tcl_DStringAppendElement ( &dString, "HandleMouseEvent" );
    Tcl_DStringAppendElement ( &dString, "BuildGUI" );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForHelpFrame" );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForToolFrame" );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForCalibrationFrame" );
    Tcl_DStringAppendElement ( &dString, "UpdateAll" );
  Tcl_DStringResult ( interp, &dString );
  Tcl_DStringFree ( &dString );
  Tcl_DStringFree ( &dStringParent );
    return TCL_OK;
    }
    if(argc==3) {
      Tcl_DString dString;
      int SuperClassStatus;
    SuperClassStatus = vtkSlicerModuleGUICppCommand(op,interp,argc,argv);
    if ( SuperClassStatus == TCL_OK ) { return TCL_OK; }
    /* Starting function: GetClassName */
    if ( strcmp ( argv[2], "GetClassName" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "GetClassName" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for GetClassName */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "const char *GetClassName ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for GetClassName */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: IsA */
    if ( strcmp ( argv[2], "IsA" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "IsA" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringAppendElement ( &dString, "string" );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for IsA */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "int IsA (const char *name);" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for IsA */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: NewInstance */
    if ( strcmp ( argv[2], "NewInstance" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "NewInstance" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for NewInstance */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI *NewInstance ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for NewInstance */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: SafeDownCast */
    if ( strcmp ( argv[2], "SafeDownCast" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "SafeDownCast" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringAppendElement ( &dString, "vtkObject" );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for SafeDownCast */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI *SafeDownCast (vtkObject* o);" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for SafeDownCast */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: GetLogic */
    if ( strcmp ( argv[2], "GetLogic" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "GetLogic" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for GetLogic */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavLogic *GetLogic ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for GetLogic */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: SetModuleLogic */
    if ( strcmp ( argv[2], "SetModuleLogic" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "SetModuleLogic" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringAppendElement ( &dString, "vtkSlicerLogic" );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for SetModuleLogic */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void SetModuleLogic (vtkSlicerLogic *logic);this SetLogic vtkObjectPointer this Logic logic " );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for SetModuleLogic */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: New */
    if ( strcmp ( argv[2], "New" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "New" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for New */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "static vtkHybridNavGUI *New ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for New */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: Init */
    if ( strcmp ( argv[2], "Init" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "Init" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for Init */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void Init ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for Init */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: Enter */
    if ( strcmp ( argv[2], "Enter" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "Enter" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for Enter */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "virtual void Enter ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for Enter */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: Exit */
    if ( strcmp ( argv[2], "Exit" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "Exit" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for Exit */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "virtual void Exit ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for Exit */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: AddGUIObservers */
    if ( strcmp ( argv[2], "AddGUIObservers" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "AddGUIObservers" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for AddGUIObservers */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "virtual void AddGUIObservers ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for AddGUIObservers */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: RemoveGUIObservers */
    if ( strcmp ( argv[2], "RemoveGUIObservers" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "RemoveGUIObservers" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for RemoveGUIObservers */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "virtual void RemoveGUIObservers ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for RemoveGUIObservers */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: AddLogicObservers */
    if ( strcmp ( argv[2], "AddLogicObservers" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "AddLogicObservers" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for AddLogicObservers */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void AddLogicObservers ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for AddLogicObservers */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: RemoveLogicObservers */
    if ( strcmp ( argv[2], "RemoveLogicObservers" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "RemoveLogicObservers" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for RemoveLogicObservers */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void RemoveLogicObservers ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for RemoveLogicObservers */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: ProcessTimerEvents */
    if ( strcmp ( argv[2], "ProcessTimerEvents" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "ProcessTimerEvents" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for ProcessTimerEvents */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void ProcessTimerEvents ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for ProcessTimerEvents */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: HandleMouseEvent */
    if ( strcmp ( argv[2], "HandleMouseEvent" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "HandleMouseEvent" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringAppendElement ( &dString, "vtkSlicerInteractorStyle" );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for HandleMouseEvent */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void HandleMouseEvent (vtkSlicerInteractorStyle *style);" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for HandleMouseEvent */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: BuildGUI */
    if ( strcmp ( argv[2], "BuildGUI" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "BuildGUI" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for BuildGUI */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "virtual void BuildGUI ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for BuildGUI */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: BuildGUIForHelpFrame */
    if ( strcmp ( argv[2], "BuildGUIForHelpFrame" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForHelpFrame" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for BuildGUIForHelpFrame */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void BuildGUIForHelpFrame ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for BuildGUIForHelpFrame */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: BuildGUIForToolFrame */
    if ( strcmp ( argv[2], "BuildGUIForToolFrame" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForToolFrame" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for BuildGUIForToolFrame */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void BuildGUIForToolFrame ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for BuildGUIForToolFrame */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: BuildGUIForCalibrationFrame */
    if ( strcmp ( argv[2], "BuildGUIForCalibrationFrame" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "BuildGUIForCalibrationFrame" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for BuildGUIForCalibrationFrame */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void BuildGUIForCalibrationFrame ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for BuildGUIForCalibrationFrame */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
    /* Starting function: UpdateAll */
    if ( strcmp ( argv[2], "UpdateAll" ) == 0 ) {
    Tcl_DStringInit ( &dString );
    Tcl_DStringAppendElement ( &dString, "UpdateAll" );
    /* Arguments */
    Tcl_DStringStartSublist ( &dString );
    Tcl_DStringEndSublist ( &dString );
    /* Documentation for UpdateAll */
    Tcl_DStringAppendElement ( &dString, "" );
    Tcl_DStringAppendElement ( &dString, "void UpdateAll ();" );
    Tcl_DStringAppendElement ( &dString, "vtkHybridNavGUI" );
    /* Closing for UpdateAll */

    Tcl_DStringResult ( interp, &dString );
    Tcl_DStringFree ( &dString );
    return TCL_OK;
    }
   Tcl_SetResult ( interp, const_cast<char*>("Could not find method"), TCL_VOLATILE ); 
   return TCL_ERROR;
   }
 }

  if (vtkSlicerModuleGUICppCommand(static_cast<vtkSlicerModuleGUI *>(op),interp,argc,argv) == TCL_OK)
    {
    return TCL_OK;
    }
    }
  catch (vtkstd::exception &e)
    {
    Tcl_AppendResult(interp, "Uncaught exception: ",  e.what(), "\n", NULL);
    return TCL_ERROR;
    }
  return TCL_ERROR;
}
