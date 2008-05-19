/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsTask.h,v 1.21 2006/06/03 00:27:45 kapoor Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************

/*!
  \file
  \brief Defines a periodic task.
  \ingroup cisstRealTime
*/

#ifndef _rtsTask_h
#define _rtsTask_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMailBox.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstRealTime/rtsStateDataTable.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <vector>
#include <map>

// forward declarations
class rtsCommand0Base;
class rtsCommand1Base;

template <class _taskType> class rtsCommand0;
template <class _taskType> class rtsCommand1;

#include <cisstRealTime/rtsExport.h>

/*!
  \ingroup cisstRealTime

  This class provides the base for implementing a periodic
  real-time loop, that has a state table to store the state at each
  'ticks' of the task, a mailbox to receive and send messages to other
  tasks, a device for input and output signals.
*/
class CISST_EXPORT rtsTask : public ddiDeviceInterface {
public:
  /*! Typedef for a vector of mailboxes. */
  typedef std::vector<osaMailBox*> MailBoxVectorType;
    
  /*! Typedef for a other (other than this) task name and corresponding
    state table pointer. */
  typedef std::map<const char *, const rtsStateDataTable *> StateDataTableMapType;
private:
  /*! Pointer to the OS independent thread object. */
  osaThread *Thread;
  
  /*! Pointer to the OS independent thread buddy, that makes the thread
    real-time on __ALL__ platforms. */
  osaThreadBuddy *ThreadBuddy;
  
  /*! A vector of mailboxes. */
  MailBoxVectorType MailBoxes;
    
  /*! A map of state tables of other tasks that this task can refer. The
    map is indexed by the name of the other task. Note that this task can
    only perform read operations on these state tables. */
  StateDataTableMapType ExternalStateDataTableMap;
    
  /*! Will be removed later if not useful. */
  std::vector<ddiDeviceInterface::ZeroArgOperationPointerBaseType> PtrsToMethodsZeroArg;
  std::vector<ddiDeviceInterface::OneArgOperationPointerBaseType> PtrsToMethodsOneArg;

  /*! The member function that is passed as 'start routine' argument for
    thread creation. */
  void *RunInternal(void* argument);

  /*! The member funtion that is executed as soon as thread gets created.
    It does some housekeeping before the user code can be executed as real
    time. */
  void StartupInternal(void);

  /*! The member function that is executed once the task terminates. This
    does some cleanup work */
  void CleanupInternal(void);

protected:
        bool ExitFlag, NewThread;

    /*! A pointer to the device so that the task can send and receive signals. */
  ddiDeviceInterface * Device;

  /*! The period of the task. */
  double Period;

  /*! True if the task is hard real time. It is always false for non-real
    time systems. */
  bool IsHardRealTime;

  /*! The state data table object to store the states of the task. */
  rtsStateDataTable StateDataTable;

  /*! True if the task is initialized, that is all housekeeping is done
    and taskobject->Start() is executed. It is guaranteed that the
   Startup would be called within Period + latency of OS. */
  bool Initialized;

  /*! True if the task is to be killed. It is guaranteed that the loop would end
    within Period + latency of OS. */
  bool EndTask;

  /*! True if the task has been started. It is guaranteed that the loop would end
    within Period + latency of OS. */
  bool Started;  // should actually be combined with Initialized into an enumerated "status" variable

  /*! True if the task has been killed. It is guaranteed that the loop would end
    within Period + latency of OS. */
  bool Terminated;  // should actually be combined with Initialized into an enumerated "status" variable

  /*! True if the task took more time to do computation than allocated time
    */
  bool OverranPeriod;  // should actually be combined with Initialized into an enumerated "status" variable

  /*! The start and end times for the task. The naming convention has been
    borrowed from MATLAB
    */
  double Tic, Toc;

  /*! The name given to the task. */
  char Name[6];

    template <class __TaskType>
        inline void AddRTSMethodOneArg(ddiCommandBase::ReturnType (__TaskType::*action)(ddiCommand1Base &),
                __TaskType *dev, const char *name, const std::type_info *info, const cmnClassServicesBase* classInputTypeServices) {
            OneArgOperations[name] = new rtsCommand1<__TaskType>(action, dev, name, info, classInputTypeServices);
        }

#if 0
  /*! Send to a msg using mailbox. */
  template <class _dataType>
  inline osaMailBox::ReturnType Send(int number, _dataType *msg,
      unsigned int size = sizeof(_dataType),
      int timeout = osaMailBox::WAITFOREVER) {
    return MailBoxes[number]->Send<_dataType>(msg, size, timeout);
  }

  /*! Receieve using mailbox. Need to use connect first. */
  template <class _dataType>
  inline osaMailBox::ReturnType Receive(int number, _dataType *msg,
      unsigned int size = sizeof(_dataType),
      int timeout = osaMailBox::NOWAIT) {
    return MailBoxes[number]->Receive<_dataType>(msg, size, timeout);
  }
#endif

#if 1
  /*! Send to a msg using mailbox. */
  inline osaMailBox::ReturnType Send(int number, void *msg,
      unsigned int size,
      int timeout = osaMailBox::WAITFOREVER) {
    return MailBoxes[number]->Send(msg, size, timeout);
  }

  /*! Receive using mailbox. Need to use connect first. */
  inline osaMailBox::ReturnType Receive(int number, void *msg,
      unsigned int size,
      int timeout = osaMailBox::NOWAIT) {
    return MailBoxes[number]->Receive(msg, size, timeout);
  }
#endif

  /*! Add another mailbox.  By default a task is created with one
      mailbox, if more than one mailbox is needed, this should be put
      in the startup function.  It is not practical to have more than
      10 mailboxes, anyways the way they are named restricts the
      number to this.  The mailboxes are named by using the first 4
      characters of the task name, followed by a number bewteen 1-9, &
      0 (for 10), followed by M to denote that it is a mailbox.
   */
  bool AddMailBox(void);

  /*! Get the reference to the state data table of task named
      'taskname'.  The reference must be added to this task using the
      task manager
   */
  const rtsStateDataTable * GetExternalStateDataTable(const char *taskname);
#if 0
  /*! This will be removed in the future. */
  void SetUpdater(rtsStateDataId id, ddiDeviceInterface::OperationPointerBaseType & ptr);

  /*! This will be removed in the future. */
  bool UpdateState(rtsStateDataId id);
#endif
public:
  /*! Default Constructor. Does nothing */
  rtsTask(){};
  /*! Create a task with name 'name', periodicity, and a flag that
      sets if the task needs to be hard real time. Only the first 4
      characters of the name are used, the rest are quietly ignored,
      so be careful in naming. The name given to the actual thread
      that runs the task is the first 4 characters appended by a 'T'
      to denote that the name represents a task.

      The 'newThread' parameter specifies whether a new thread should
      actually be created (default is true).  Set this to false if you
      wish to use the "main" program thread for this task.  In this case,
      the isHardRealTime parameter is ignored.  The periodicity should
      generally be set to 0 (or negative) to indicate that the main thread
      is not periodic.  Also, rather than calling the Start() method,
      you should manually call the Startup(), Run() and Cleanup() methods.
   */
  rtsTask(const char *name, double periodicity, bool isHardRealTime = false,
            int sizeDataTable = 256, bool newThread = true);

  /*! Default Destructor. */
  virtual ~rtsTask();

  /*! Virtual method that gets overloaded to run the actual periodic
      task.
    */
  virtual void Run(void) = 0;

  /*! Virtual method that gets overloaded to run any initialization
      even before the thread to run the task is created. The only
      initialization done prior to this being executed is the
      creation of an empty State Data Table and a default mailbox.
   */
  //virtual void Initialize(void) = 0;

  /*! Virtual method that gets overloaded, and is run before the
      periodic task is started.
    */
  virtual void Startup(void) = 0;

  /*! Virtual method that gets overloaded, and is run after the
      task gets killed using Kill() method.
    */
  virtual void Cleanup(void) = 0;

  /*! Virtual method that gets called when the task/device needs
    to be configured. Should it take XML info?? */
  virtual void Configure(const char *filename=NULL) = 0;

  /*! Return the periodicity of the task */
  inline double GetPeriodicity(void) { return Period; };

    inline bool &GetExitFlag() {return ExitFlag;}

    /* start thread & wait for start */
    void Create(void);

  /*! End the task */
  inline void Kill(void) { CMN_LOG_CLASS(7) << "Marked " << Name << std::endl; EndTask=true; };
  
  /*! Task Status */
  inline bool Running(void) {return Initialized &! Terminated;};
  // RHT: This really should be subsumed into a routine returning an enumerated type of task status 

  /*! Resume execution of the task */
  inline void Start(void) {
    if (Initialized) {
      // this code is reached if we use stop to stop the task
      // and then issue a start
      ThreadBuddy->Resume();
    } else {
      Initialized = true;
    }
  }

  /*! Suspend the execution of the task */
  inline void Suspend(void){if (Initialized) ThreadBuddy->Suspend();};

  /*! Return a const refrence to StateDataTable, so that it can
      be used by a reader
   */
  inline const rtsStateDataTable * GetStateDataTable(void) const {return &StateDataTable;};

  /*! Return the name of the task
   */
  inline const char * GetName(void) const {return Name;};

  /*! Connect to a 'number' th mailbox of this task to 'tonumber' th mailbox of task named 'name'. */
  bool Connect(int number, const char* name, int tonumber);

  /*! Add a reference to the State Data Table of task named 'taskname'. */
  void AddExternalStateDataTable(const char* taskname, const rtsStateDataTable* otherTable);

  /*! Associate a device with this task. Ideally the device could be another lower level task
    or a hardware device. */
  inline void AssociateDevice(ddiDeviceInterface* device) {Device = device;}

  /*! Queue a command for execution.
    \param cmd command to be executed
    \param size size of command
   */
  inline ddiCommandBase::ReturnType QueueCommand(rtsCommand1Base *cmd, unsigned int size) {
    if (this->Send(0, cmd, size, osaMailBox::NOWAIT) == osaMailBox::SUCCESS) {
      return ddiCommandBase::DEV_OK;
    } else {
      return ddiCommandBase::NO_MAILBOX;
    }
  }

  /*! De-queue a command from the mailbox
    \param cmd Place holder for the command
    \param size size of command
   */
  inline bool DequeueCommand(rtsCommand1Base *cmd, unsigned int size) {
    if (this->Receive(0, cmd, size, osaMailBox::NOWAIT) == osaMailBox::SUCCESS) {
      return true;
    } else { 
      return false;
    }
  }

  /*! Wait for task to finish. Always use this before deleting the task pointer and
    after issuing a task kill. This would ensure proper cleanup of task
    \param timeoutms The wait can be timed out if needed to prevent nasty hangs
    \returns  return true if the task terminated without timeout happening
                     false if timeout occured and task did not finish
    */
  bool WaitToTerminate(unsigned long timeoutms = 0);

  /*! Wait for task to start.
    */
  bool WaitToStart();

  /*! return true if task is started
    */
  bool IsStarted() {return Started;}

  /*! return true if task is terminated
   */
  bool IsTerminated() {return Terminated;}

  /*! return true if task is marked for killing
   */
  bool IsEndTask() {return EndTask;}

  /*! return true if task overran allocated period
   */
  bool IsOverranPeriod() {return OverranPeriod;}

    /*! Return true if thread is periodic.  Currently, returns true if
      the thread was created with a period > 0. */
    bool IsPeriodic() const { return Period > 0; }

  /*! reset overran period flag
    */
  void ResetOverranPeriod() {OverranPeriod = false;}
#if 0
  /*!
    Add a read operation to the map
    \param name A string describing the operation. Later this would be used to access the operation
    \param enumValue A enumeration for the read operation.
    \param action A pointer to method that would be called related to the operation. The pointer to
    method must have the following decleration
        void _taskType::NameOfMethod(cmnDataObject &obj);
    \param obj The object to which the Method belongs. Simply call it with 'this'
   */
  template <class _taskType>
  void AddReadOperation(const char *name, unsigned int enumValue,
      bool (_taskType::*action)(cmnDataObject &),
      _taskType *obj);

  /*!
    Add a write operation to the map
    \param name A string describing the operation. Later this would be used to access the operation
    \param enumValue An enumeration for the write operation.
    \param action A pointer to method that would be called related to the operation. The pointer to
    method method have the following declaration
        void _taskType::NameOfMethod(rtsInternalCommandBase &msg);
    \param obj The object to which the Method belongs. Simply call it with 'this'
   */
  template <class _taskType>
  void AddWriteOperation(const char *name, unsigned int enumValue, 
      bool (_taskType::*action)(rtsInternalCommandBase &),
      _taskType *obj);

  /*!
    Add a write operation to the map
    \param name A string describing the operation. Later this would be used to access the operation
    \param enumValue An enumeration for the write operation.
    \param action A pointer to method that would be called related to the operation. The pointer to
    method method have the following declaration
        void _taskType::NameOfMethod(cmnDataObject &from, cmnDataObject &to);
    \param obj The object to which the Method belongs. Simply call it with 'this'
   */
  template <class _taskType>
  void AddConversionOperation(const char *name, unsigned int enumValue, 
      bool (_taskType::*action)(cmnDataObject &, cmnDataObject &),
      _taskType *obj);
#endif

};

#if 0
template <class _taskType>
void rtsTask::AddReadOperation(const char *name, unsigned int enumValue,
    bool (_taskType::*action)(cmnDataObject &),
    _taskType *obj)
{
  if (enumValue > ReadOperations.size()) {
    ReadOperations.resize(ReadOperations.size() + _taskType::LAST_READ);
  }
  ReadNameMap[name] = enumValue;
  ReadOperations[enumValue] = new ddiCommand<_taskType>(action, obj, name);
}

template <class _taskType>
void rtsTask::AddWriteOperation(const char *name, unsigned int enumValue, 
    bool (_taskType::*action)(rtsInternalCommandBase &),
    _taskType *obj)
{
  if (enumValue > WriteOperations.size()) {
    WriteOperations.resize(WriteOperations.size() + _taskType::LAST_WRITE);
  }
  WriteNameMap[name] = enumValue;
  WriteOperations[enumValue] = new rtsInternalCommand<_taskType>(action, obj, name);
}

template <class _taskType>
void rtsTask::AddConversionOperation(const char *name, unsigned int enumValue, 
    bool (_taskType::*action)(cmnDataObject &, cmnDataObject &),
    _taskType *obj)
{
  if (enumValue > ConversionOperations.size()) {
    ConversionOperations.resize(ConversionOperations.size() + _taskType::LAST_CONVERSION);
  }
  ConversionNameMap[name] = enumValue;
  ConversionOperations[enumValue] = new ddiCommand2<_taskType>(action, obj, name);
}
#endif

#endif // _rtsTask_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsTask.h,v $
// Revision 1.21  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.20  2006/05/30 19:35:04  kapoor
// cisstRealTime: fixes possible race conditions in creation of tasks.
// See ticket #227.
//
// Revision 1.19  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.18  2006/05/06 20:41:33  kapoor
// cisstRealTime: Fixed support for non-periodic & (new thread/no-new thread)
// options. This fixes the case when there should be a new thread but non
// periodic. See also [1846] & [1905].
//
// Revision 1.17  2006/05/04 05:46:45  kapoor
// Extended command0,1 to tasks. Appears to work. More testing needed.
//
// Revision 1.16  2006/05/03 18:36:44  kapoor
// Extended command0,1 to tasks. Work in progress...
//
// Revision 1.15  2006/04/28 21:36:47  pkaz
// osaThreadBuddy and rtsTask:  fixed support for using the "main" thread in an rtsTask.
// Mailboxes did not work with the previous commit [1846].  The fix required support
// for non-periodic threads in osaThreadBuddy.
//
// Revision 1.14  2006/03/17 15:44:14  kapoor
// cisstRealTime: Added a bool ExitFlag. Classes derrived from the task can
// use this to exit the task from within the task, by setting it to true.
//
// Revision 1.13  2006/03/02 06:14:23  pkaz
// cisstRealTime:  added newThread parameter to rtsTask constructor and undid checkin [1842].
// Fixed typos in comments and added more info to log messages.
//
// Revision 1.12  2006/02/24 22:20:56  pkaz
// rtsTask.h:  Added new constructor (protected method).
//
// Revision 1.11  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.10  2005/07/29 00:04:42  kapoor
// cisstRealTime: Changed debug output format. Remove '{' and '}'.
//
// Revision 1.9  2005/06/17 20:30:50  alamora
// cisstOSAbstraction and cisstRealTime: Added some required
// CISST_EXPORT to compile DLLs.
//
// Revision 1.8  2005/06/16 19:15:04  txia
// cisstRealTime: Corrected circular dependencies with a couple of forward
// declarations.  Previous version didn't compile with gcc 3.4.
//
// Revision 1.7  2005/05/03 17:06:22  anton
// rtsTask: Allow WaitToTerminate() with no timeout (default value = 0) plus
// some CMN_LOG to replace std::cout.
//
// Revision 1.6  2005/04/24 16:46:30  kapoor
// osaMailBox: Connect & Addmailbox return bool flag to indicate success/failure.
//
// Revision 1.5  2005/04/23 00:34:28  kapoor
// rtsTask: Added a flag to tell if the execution of task overran allocated
// period.
//
// Revision 1.4  2005/04/19 23:58:23  kapoor
// cisstRealTime: Added new methods AddRead/Write/ConversionOperation to add
// commands to the respective maps.
//
// Revision 1.3  2005/04/16 21:27:02  kapoor
// Added WaitToTerminate to check if thread has completed. This was earlier done by destructor only.
//
// Revision 1.2  2005/03/17 23:16:28  kapoor
// Real-Time: CISST library ThreadBuddy bug (see ticket #135)
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.10  2004/04/08 05:56:41  kapoor
// Period is now a double.
//
// Revision 1.9  2004/03/26 06:02:52  kapoor
// Added queue and de-queue and rtsInternalCommand
//
// Revision 1.8  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.7  2004/03/22 00:07:15  kapoor
// Returned to using void * for sending messages.
//
// Revision 1.6  2004/03/19 15:59:03  kapoor
// Added AssociateDevice and AddDevice methods.
//
// Revision 1.5  2004/03/16 17:39:39  kapoor
// Added ThreadBuddy class to more RTAI dependent stuff to cisstOSAbstraction
// from cisstRealTime
//
// Revision 1.4  2004/03/05 05:13:47  kapoor
// Changed Send and Receive methods to be templated, as a result it was required to increase the stack size to 16Kb from 8Kb. Is it good or bad?
//
// Revision 1.3  2004/03/04 21:26:17  kapoor
// more changes.
//
// Revision 1.2  2004/03/04 07:31:17  kapoor
// lots of fixes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
