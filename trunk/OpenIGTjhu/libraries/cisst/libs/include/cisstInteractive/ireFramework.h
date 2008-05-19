/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ireFramework.h,v 1.9 2006/02/01 22:40:36 pkaz Exp $

  Author(s): Andrew LaMora
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


/*!
  \file
  \brief Declaration of ireFramework for managing Python IRE tools
*/


#ifndef _ireFramework_h
#define _ireFramework_h

#include <stdexcept>

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnThrow.h>


#include <cisstInteractive/ireExport.h>


/*! 
  \brief Class to manage the preparation of Python for the
  Interactive Robot Environment, and for launching the IRE IDE and
  shell.

  \ingroup cisstInteractive

  The goal of the IRE is to be mindlessly simple to incorporate in
  already complicated applications.

  The purpose of this class is to abstract the mechanics of preparing
  and launching Python in general and the IRE in particular from the
  end user.  It is also used to ensure creation of a single extended
  Python shell with global scope.

  This class is instantiated, and runs as a singleton object.  The
  method is borrowed from Design Principles and the CISST Logger.

  \note The IRE is *not* thread-safe; all threads will run the IRE in
  the Python shell created by this singleton class.  This behavior is
  DESIRED; we are attempting to prevent multiple copies of the Object
  Register from being created from within the same application.  Note
  that it is theoretically possible for the user to nevertheless
  create a new copy of the Object Register referenced by this
  application from within a separate Python shell (the IRE IDE permits
  spawning new shells).
*/
class CISST_EXPORT ireFramework {

private:
  //static PyObject* pInstance;
  // NewPythonThread is true if a new Python thread should be (or was) started for the IRE
  static bool NewPythonThread;
    // IsActiveFlag is true if the IRE is active and it was not started in
  // a new Python thread (i.e., if NewPythonThread is false).  If NewPythonThread is true,
  // then it is necessary to check whether the IRE Python thread is alive.
  static bool IsActiveFlag;

    void InitShellInstance(void);

    void FinalizeShellInstance(void);
    
    void LaunchIREShellInstance(char *startup);

  void JoinIREShellInstance(double timeout);

  bool IsActiveInstance();
    
    
    // The only constructor for this singleton object
protected:
  /*! Constructor for this singleton object.  Initialize the Python
      subsystem upon creation.  */
  ireFramework() {
      InitShellInstance(); 
  };
    
  /*! Dereferences everything and cleans Python shell .  */
  ~ireFramework() {
      FinalizeShellInstance();
  }
public:
  
  /*! Return a pointer to the instantiated instance of this object*/
  static ireFramework* Instance(void);

  
  /*! Initializes the Python environment.  Called from the
      constructor, the user needs never call this function explicitly.
      As ireFramework is a singleton object, the underlying Python
      shell will have application scope ONLY.  The "irepy" package
      will remain instantiated for the duration of the application.
      All modules loaded and variables created from this class will
      remain in scope; note that this does NOT include objects created
      from within the IRE IDE shell. */
  static inline void InitShell(void)  throw(std::runtime_error) {
      Instance()->InitShellInstance();
  }
  
  
  /*! Called explicitly from the class destructor, it removes all
      references to the Python shell.  Python is then free to clean
      itself up.  If the application wishes to restart the IRE,
    it will be necessary to explicitly call InitShell(). */
  static inline void FinalizeShell(void)  throw(std::runtime_error) {
      Instance()->FinalizeShellInstance();
  }

  
  /*!  Loads the "irepy" Python IRE package and
      calls the "launch()" method to launch the GUI.

      \note Although the "irepy" package itself remains loaded for the
      duration of the application, any variables/objects instantiated
      within the IRE interpreter are contained in a separate,
      session-scope interpreter; they will not be available from the
      C++ environment after the IRE GUI is shut down.

    The Python shell will first run the "startup" string.  This can
      be used, for example, to import application-specific Python scripts.
    The "startup" string is not declared const because that would not
    be compatible with PySys_SetArgv().

      If the newPythonThread parameter is true, the IRE will be started in
    a separate thread.  Note, however, that the C++ code will have to
    periodically call JoinIREShell(); otherwise, the C++ program will
    not relinquish any execution time to the IRE thread.
    An alternate implementation would be to create the IRE thread in
    the C++ code before launching the IRE.  This, however, is less
    portable and would best be implemented using cisstOSAbstraction. */
      
  static inline void LaunchIREShell(char *startup = "", bool newPythonThread = false) throw(std::runtime_error) {
    NewPythonThread = newPythonThread;
      Instance()->LaunchIREShellInstance(startup);
  }

  /*! Wait for IRE shell to finish (if started in a new Python thread).  Specify a negative
      timeout to wait forever. */
  static inline void JoinIREShell(double timeout) {
    Instance()->JoinIREShellInstance(timeout);
  }

    /*! Check whether IRE shell is active.  If the IRE was started in a new Python thread, this
        method checks whether the thread is alive.  Otherwise, it checks whether the LaunchIREShell()
        method is still being executed (obviously only possible if it was called by another C++ thread).
    
        \note If LaunchIREShell() was called from a different C++ thread (dedicated to launching the IRE),
        it is better to check whether that thread is still running because otherwise this method will
        return false if that thread has not yet been started or has not yet completed the IRE launch. */
    static inline bool IsActive() {
        return Instance()->IsActiveInstance();
    }

    /*! Check whether the Python interpreter has been initialized (i.e., the ireFramework instance has
        been created and FinalizeIREShell() has not been called). */
    static bool IsInitialized();
  
};


#endif // _ireFramework_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ireFramework.h,v $
//  Revision 1.9  2006/02/01 22:40:36  pkaz
//  ireFramework.h:  removed doxygen "see also" references to private member functions.
//
//  Revision 1.8  2006/01/26 21:56:29  pkaz
//  ireFramework:  Added IsActive() and IsInitialized() methods;
//  changed new_thread to NewPythonThread for clarify.
//
//  Revision 1.7  2006/01/20 19:26:01  anton
//  ireFramework: Removed type IREException and use cmnThrow instead.
//
//  Revision 1.6  2006/01/14 08:57:30  pkaz
//  IRE:  major cleanup of ireLogger module; moved some code to cmnCallbackStreambuf.h (new file).
//
//  Revision 1.5  2006/01/11 16:32:13  pkaz
//  IRE: Added character string parameter to LaunchIREShell(), which gets executed when Python shell is started.
//
//  Revision 1.4  2006/01/11 06:57:57  anton
//  ireFramework.h: Minor Doxygen updates.
//
//  Revision 1.3  2005/12/21 08:21:10  pkaz
//  IRE: added support for running the IRE in a separate thread (uses Python threading module).
//
//  Revision 1.2  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.1  2005/06/20 22:16:07  anton
//  Interactive Research Environment: Moved all IRE code to cisstInteractive
//
//
//  Revision 1.7  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.6  2005/04/09 03:51:20  anton
//  cmnIRE: Added missing #include <stdexcept> for unix, move doxygen documentation
//  above the corresponding class.
//
//  Revision 1.5  2005/04/07 16:35:28  alamora
//  Created IREException class, modified to handle exceptions
//
//  Revision 1.4  2005/04/03 13:03:40  anton
//  cmnIRE: Added return type void (g++ warning for ISO compliance)
//
//  Revision 1.3  2005/03/08 20:18:56  alamora
//  Modified for proper CVS logging
//
//  Revision 1.2  2005/03/08 20:16:57  alamora
//  Modified for proper CVS logging
//
