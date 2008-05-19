/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ireFramework.cpp,v 1.13 2006/05/09 03:29:47 kapoor Exp $

  Author(s):  Andrew LaMora, Peter Kazanzides
  Created on: 2005-02-28

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---

*/


#include <Python.h>

#include <cisstCommon/cmnCallbackStreambuf.h>
#include <cisstInteractive/ireFramework.h>

// PyTextCtrlHook
//
// This class supports displaying cmnLogger output (e.g., from CMN_LOG)
// in an IRE wxTextCtrl window.  All member functions of this class
// are static so that they can easily be passed as function pointers
// (to Python or to cmnCallbackStreambuf).  The class was created primarily
// for organizational purposes -- a namespace may have been just
// as effective.
//
// The display of cmnLogger output is accomplished as follows:
//
//    1. The C++ code (LaunchIREShellInstance) initializes the ireLogger module,
//       starts the Python interpreter, and launches the IRE.
//    
//    2. During initialization (in ireMain.py), the IRE calls the
//       PyTextCtrlHook::SetTextOutput C++ function to store the address of a Python
//       callback function (such as wx.LogMessage or wx.TextCtrl.AppendText).
//       It also specifies whether newlines should be stripped from the strings
//       before they are passed to Python (required when calling wx.LogMessage).
//
//    3. The PyTextCtrlHook::SetTextOutput C++ code instantiates a new
//       cmnCallbackStreambuf, passing it the PyTextCtrlHook::PrintLog callback function.
//       After creating the cmnCallbackStreambuf instance, the C++ code adds it as a new
//       channel to the cmnLogger multiplexer.
//
//    4. When the C++ code logs a message, it is sent to the cmnCallbackStreambuf
//       class.  This class calls the callback function (PyTextCtrlHook::PrintLog)
//       whenever a newline is encountered or the buffer is full.
//
//    5. PyTextCtrlHook::PrintLog removes newlines (if Strip_nl is true), converts
//       the character string to a Python object, and then calls the Python callback
//       function that was specified when PyTextCtrlHook::SetTextOutput was called.
//       This will cause the text to be written to the wx.TextCtrl object.
//       Note that if a "built-in" (i.e., C) function is specified for the callback,
//       the C function will be called directly via PyCFunction_Call().  In other words,
//       PrintLog will directly call the wxPython DLL, rather than via the Python interpreter.
//
//    6. The Python code calls PyTextCtrlHook::ClearTextOutput when the TextCtrl
//       window is destroyed.
//
// This method is needed because:
//
//    1. Although wxTextCtrl can be derived from a streambuf (via multiple inheritance),
//       that feature is often disabled during compilation of wxWidgets/wxPython.
//
//    2. Even if wxTextCtrl was derived from a streambuf, Swig would have to be able to wrap it.
//       Alternatively, the wxTextCtrl object could be passed to C++, but this would require
//       the C++ code to know the definition of the wxTextCtrl class.  This would make the C++
//       code dependent on wxWidgets, which is not desirable.
//
 

class PyTextCtrlHook {
  static PyObject* PythonFunc;   // the Python callback function
  static bool Strip_nl;          // whether or not to strip newlines
  static PyMethodDef Methods[];
    static cmnCallbackStreambuf<char> *Streambuf;
    static cmnLODMultiplexerStreambuf<char>::LodType LoD;

public:
  // Specify the Python callback function and whether or not to strip newlines
  static PyObject* SetTextOutput(PyObject* self, PyObject* args);
  // Remove the Python callback function
  static PyObject* ClearTextOutput(PyObject* self, PyObject* args);

    // Get the channel level of detail
    static PyObject* GetLoD(PyObject* self, PyObject* args);
    // Set the channel level of detail
    static PyObject* SetLoD(PyObject* self, PyObject* args);

  // The C++ callback function that is passed to cmnCallbackStreambuf
  static void PrintLog(char *str, int len);
  
  // Initialize the Python module
  static void InitModule(char* name);

    // Cleanup
    static void Cleanup();
};

PyObject* PyTextCtrlHook::PythonFunc = 0;
bool PyTextCtrlHook::Strip_nl = false;
cmnCallbackStreambuf<char> *PyTextCtrlHook::Streambuf = 0;
cmnLODMultiplexerStreambuf<char>::LodType PyTextCtrlHook::LoD = CMN_LOG_DEFAULT_LOD;

// Specify the Python callback function and whether or not to strip newlines.
// Note that the second parameter (flag) is optional (defaults to false).
PyObject* PyTextCtrlHook::SetTextOutput(PyObject* self, PyObject* args)
{
  PyObject* func;
  PyObject* flag = Py_False;    // default value is false
    int iLoD = (int)LoD;
  if (PyArg_ParseTuple(args,"O|Oi",&func,&flag,&iLoD)) {
    if (!PyCallable_Check(func)) {
      PyErr_SetString(PyExc_TypeError, "expected a callable for parameter 1.");
      return NULL;
    }
    Strip_nl = false;
    if (flag == Py_True)
      Strip_nl = true;
    else if (flag != Py_False) {
      PyErr_SetString(PyExc_TypeError, "boolean expected for parameter 2.");
      return NULL;
    }
    Py_XINCREF(func);         // Save reference to callback
    Py_XDECREF(PythonFunc);   // Release any previous callback
    PythonFunc = func;
        LoD = (cmnLODMultiplexerStreambuf<char>::LodType) iLoD;
        if (!Streambuf)
            Streambuf = new cmnCallbackStreambuf<char>(PrintLog);
#if ANKUR
      cmnLogger::GetMultiplexer()->AddChannel(Streambuf, LoD);
#endif
    Py_INCREF(Py_None);
    return Py_None;
  }
  return NULL;
}

// Remove the Python callback function
PyObject* PyTextCtrlHook::ClearTextOutput(PyObject* self, PyObject* args)
{
    Cleanup();
  Py_INCREF(Py_None);
  return Py_None;
}

// Get the channel level of detail
PyObject* PyTextCtrlHook::GetLoD(PyObject* self, PyObject* args)
{
  // If the channel is active, get the LOD from the multiplexer,
  // just in case it was changed from within the C++ code (very unlikely).
  if (Streambuf)
        cmnLogger::GetMultiplexer()->GetChannelLOD(Streambuf, LoD);
    return Py_BuildValue("i", (int)LoD);
}

// Set the channel level of detail
PyObject* PyTextCtrlHook::SetLoD(PyObject* self, PyObject* args)
{
    int iLoD;
  if (!PyArg_ParseTuple(args,"i",&iLoD)) {
       PyErr_SetString(PyExc_TypeError, "integer type expected");
    return NULL;
  }
    LoD = (cmnLODMultiplexerStreambuf<char>::LodType) iLoD;
    if (Streambuf)
        cmnLogger::GetMultiplexer()->SetChannelLOD(Streambuf, LoD);
  Py_INCREF(Py_None);
  return Py_None;
}


// The C++ callback function that is passed to cmnCallbackStreambuf.
// It calls the Python callback function stored in PythonFunc.
// Parameters:
//     str:  a null-terminated string
//     len:  the length of the string
void PyTextCtrlHook::PrintLog(char *str, int len)
{
  PyObject* args;
  PyObject* result;

  if (PythonFunc) {
    // Remove newline if Strip_nl is true
    if (Strip_nl && (str[len-1] == '\n'))
      str[len-1] = 0;
    // Convert the C string to a Python string
        args = Py_BuildValue("(s)", str);
        // Call the callback function
        if (PyCFunction_Check(PythonFunc)) {
            // If it is a (wrapped) C function, call it directly.
            try {
                result = PyCFunction_Call(PythonFunc, args, 0);
            }
            catch(...) {
                std::cout << "PyCFunction_Call exception" << std::endl;
            }
        }
        else
            // Call the Python function (via the interpreter)
            result = PyEval_CallObject(PythonFunc, args);
    Py_DECREF(args);
    // If the result is NULL, we had a problem.  Do not write
    // to CMN_LOG because that could lead to an infinite loop
    // (since the logging functions call this function).
    if (result == NULL)
      std::cout << "CallObject returned NULL" << std::endl;
    else
      Py_DECREF(result);
  }
}

// Cleanup:  remove logger channel, delete Streambuf object,
//           remove Python callback
void PyTextCtrlHook::Cleanup()
{
    if (Streambuf) {
        cmnLogger::GetMultiplexer()->RemoveChannel(Streambuf);
        delete Streambuf;
        Streambuf = 0;
    }
    if (PythonFunc) {
      Py_XDECREF(PythonFunc);   // Release any previous callback
      PythonFunc = 0;
    }
}

PyMethodDef PyTextCtrlHook::Methods[] = {
  { "SetTextOutput", SetTextOutput, METH_VARARGS,"SetTextOutput(func, strip=False, LoD=current-lod)\n\n"
      "   Set callback function (func) for logging text.\n"
      "   Function should accept a single string parameter.\n"
      "   Set strip True to strip newlines from log message\n   (needed if wxLog class is used).\n"
          "   LoD is channel level of detail (defaults to current setting)."
 },
  { "ClearTextOutput", ClearTextOutput, METH_NOARGS, "Clear callback function for logging text."},
    { "GetLoD", GetLoD, METH_NOARGS, "Get the channel level of detail."},
    { "SetLoD", SetLoD, METH_VARARGS, "Set the channel level of detail."},
  { NULL, NULL, 0, NULL }
};

void PyTextCtrlHook::InitModule(char* name)
{
    Py_InitModule(name, Methods);
}

// ****************************************************************************
// ireFramework static member data
//
bool ireFramework::NewPythonThread = false;
bool ireFramework::IsActiveFlag = false;

// pInstance was removed from the class so that ireFramework.h does not
// need to include <Python.h>.  This avoids some compiler warnings on Linux.
//PyObject* ireFramework::pInstance = 0;
static PyObject* pInstance = 0;

// ****************************************************************************
// ireFramework::Instance
//
// Return a reference to this one, only, instantiated singleton object.
//
ireFramework* ireFramework::Instance(void) {
    // create a static variable
    static ireFramework instance;
    return &instance;
}

// ****************************************************************************
// ireFramework::InitShellInstance
//
// Initialize the Python shell
//
void ireFramework::InitShellInstance(void)
{
    Py_Initialize();
}

// ****************************************************************************
// ireFramework::FinalizeShellInstance
//
// Finalize (cleanup) the Python shell.  Also cleans up the PyTextCtrlHook.
//
void ireFramework::FinalizeShellInstance(void)
{
    PyTextCtrlHook::Cleanup();
    Py_Finalize();
}

// ****************************************************************************
// LaunchIREShellInstance
//
// This method imports necessary modules and launches the IRE GUI.  It should
// be called AFTER InitShell.  Returns True on successs, else false; usually
// indicates a library is missing (wxPython?)
//
// Modules Loaded:
//  - irepy
//
// Developers should take care to DECREF (decrease the reference counts)
// of any PyObject s that use hard references.
//
void ireFramework::LaunchIREShellInstance(char *startup) {
  //start python
  char* python_args[] = { "IRE", startup };

  IsActiveFlag = false;

  // Initialize ireLogger module, which is used for the cmnLogger output window
  PyTextCtrlHook::InitModule("ireLogger");

  PySys_SetArgv(2, python_args);
  
  PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;

  pName = PyString_FromString("irepy");
    
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  pInstance = 0;

  if (pModule != NULL) {
    // pDict is borrowed - no need to decref
    pDict = PyModule_GetDict(pModule);

    if (NewPythonThread) {
      PyObject* pClass = PyDict_GetItemString(pDict, "IreThread");

      // Create an instance of the class
      if (PyCallable_Check(pClass))
      {
        pInstance = PyObject_CallObject(pClass, NULL); 
      }
      if (pInstance)
          PyObject_CallMethod(pInstance, "start", NULL);
      else
        cmnThrow(std::runtime_error("LaunchIREShellInstance: Could not start IRE thread."));
    }
    else {  
      // pFunc is borrowed
      pFunc = PyDict_GetItemString(pDict, "launch");

      if (pFunc && PyCallable_Check(pFunc)) {
        pArgs = PyTuple_New(0);

        IsActiveFlag = true;
        // pValue is NOT borrowed - remember to decref
        pValue = PyObject_CallObject(pFunc, pArgs);
      }
      else {
        if (PyErr_Occurred()) {

          cmnThrow(std::runtime_error("LaunchIREShellInstance: Call to the launch() function failed."));
        }
      }

      //decref all PyObjects
      Py_DECREF(pValue);
      Py_DECREF(pArgs);
    }
    Py_DECREF(pModule);
  }
  else {
    //PyErr_Print();
    cmnThrow(std::runtime_error("LaunchIREShellInstance: import irepy has failed.  Check libraries."));
  }
  // Set IsActiveFlag false on exit.  If the IRE was started in a new Python thread (i.e., if
  // NewPythonThread is true), then IsActive() will check whether the thread is still alive.
  IsActiveFlag = false;
}

void ireFramework::JoinIREShellInstance(double timeout)
{
  if (NewPythonThread && IsActiveInstance()) {
    if (timeout >= 0)
      PyObject_CallMethod(pInstance, "join", "(f)", timeout);
    else
      PyObject_CallMethod(pInstance, "join", NULL);
  }
}

bool ireFramework::IsActiveInstance()
{
  bool ret = IsActiveFlag;
  if (NewPythonThread && pInstance) {
    PyObject *result;
    result = PyObject_CallMethod(pInstance, "isAlive", NULL);
    ret = (result == Py_True);
    Py_DECREF(result);
  }
  return ret;
}

bool ireFramework::IsInitialized()
{
  return (Py_IsInitialized() != 0);
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ireFramework.cpp,v $
// Revision 1.13  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.12  2006/01/26 21:53:14  pkaz
// ireFramework:  Added IsActive() and IsInitialized() methods;
// changed new_thread to NewPythonThread for clarify.
//
// Revision 1.11  2006/01/25 04:26:24  pkaz
// ireFramework.cpp:  minor documentation changes.
//
// Revision 1.10  2006/01/20 19:26:01  anton
// ireFramework: Removed type IREException and use cmnThrow instead.
//
// Revision 1.9  2006/01/17 22:41:56  pkaz
// IRE:  Added ability to directly call (wrapped) C function in wxPython DLL to display cmnLogger output.
//
// Revision 1.8  2006/01/17 07:52:25  pkaz
// IRE:  Added LoD control and enable/disable checkbox to logger panel.
// Complete rewrite of ireLogCtrl.py.
//
// Revision 1.7  2006/01/14 08:57:30  pkaz
// IRE:  major cleanup of ireLogger module; moved some code to cmnCallbackStreambuf.h (new file).
//
// Revision 1.6  2006/01/11 16:22:17  pkaz
// IRE: Added character string parameter to LaunchIREShell(), which gets executed when Python shell is started.
//
// Revision 1.5  2006/01/11 05:11:51  pkaz
// ireFramework.cpp:  minor cleanup.
//
// Revision 1.4  2006/01/10 09:25:08  pkaz
// IRE: Added display of cmnLogger output.  Work in process.
//
// Revision 1.3  2005/12/21 08:21:10  pkaz
// IRE: added support for running the IRE in a separate thread (uses Python threading module).
//
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2005/06/20 22:16:07  anton
// Interactive Research Environment: Moved all IRE code to cisstInteractive
//
//
// ****************************************************************************
