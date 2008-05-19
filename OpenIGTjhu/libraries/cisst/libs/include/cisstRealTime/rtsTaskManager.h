/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsTaskManager.h,v 1.10 2006/05/11 18:46:48 anton Exp $
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
  \brief Define the real time task manager
  \ingroup cisstRealTime
*/


#ifndef _rtsTaskManager_h
#define _rtsTaskManager_h

#include <cisstRealTime/rtsTask.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>

#include <map>
#include <ostream>
#include <string>

#include <cisstRealTime/rtsExport.h>

/*!
  \ingroup cisstRealTime

  This should now be changed to rtsSystemsManager. A useful system
  would have more than one task and device, and they would need to
  communicate with each other.  For such a system the TaskManager
  plays the role of a manger. It is used to associate devices to
  tasks, connect mailboxes, link state tables of tasks and configure
  devices or tasks.
*/
class CISST_EXPORT rtsTaskManager: public cmnGenericObject {
    
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    
    /*! Buddy for main(). */
  osaThreadBuddy *Main;

public:
  /*! Typedef for task name and pointer map. */
  typedef std::map<const char *, rtsTask *> TaskMapType;
    
  /*! Typedef for mailbox identifier. */
  typedef std::pair<const char *, int> MailBoxIdentifierType;
  //typedef std::multimap<const _MailBoxIdentifierType, const _MailBoxIdentifierType> _MailBoxConnectionMatrixType;

  /*! Typedef for mailbox connection identifier. */
  typedef std::pair<MailBoxIdentifierType, MailBoxIdentifierType> MailBoxConnectionType;
    
  /*! Typedef for vector of mailbox connections. */
  typedef std::vector<MailBoxConnectionType> MailBoxConnectionMatrixType;
    
  /*! Typedef for state data table identifier. */
  typedef std::pair<const char *, const rtsStateDataTable *> StateDataTableIdentifierType;
    
  /*! Typedef for state data linkage identifiers. */
  typedef std::pair<StateDataTableIdentifierType, StateDataTableIdentifierType> StateDataTableLinkType;
    
  /*! Typedef for vector of state data links. */
  typedef std::vector<StateDataTableLinkType> StateDataTableLinkMatrixType;

  /*! Typedef for device name and pointer map. */
  typedef std::map<std::string, ddiDeviceInterface *> DeviceMapType;

  /*! Typedef for task and device association. */
  typedef std::map<const char *, std::string> DeviceAssociationMapType;

protected:
  /*! Mapping of task name (key) and pointer to rtsTask object. */
  TaskMapType TaskMap;

  /*! Mapping of device name (key) and pointer to ddiDeviceInterface
      object. */
  DeviceMapType DeviceMap;

  /*! Mapping of task name (key) and associated device name. */
  DeviceAssociationMapType DeviceAssociationMap;

  /*! Vector of mailbox connections. Each connection is identified
    by a ordered pair of mailbox identifiers. MailBox identifiers
    are a combination of task name and the mailbox number. */
  MailBoxConnectionMatrixType MailBoxConnectionMatrix;

  /*! Vector of state table linkages between tasks. Each linkage is
    identified by a ordered pair of state data table
    identifiers. State data table identifiers are a combination of
    task name and pointer to state data table. */
  StateDataTableLinkMatrixType StateDataTableLinkMatrix;

  /*! Constructor.  Protected because this is a singleton. Creates a
    buddy for the main(). Does OS specific initlization to start
    realtime operations. */
  rtsTaskManager();
    
  /*! Destructor.  Deleted the buddy for main(). Does os specific
    cleanup. */
  virtual ~rtsTaskManager();

public:
  /*! Create the static instance of this class. */
  static rtsTaskManager*  GetInstance(void) ;
  /*static rtsTaskManager*  GetInstance(void) {
        static rtsTaskManager instance;
        return &instance;
    }*/
    
  /*! Put a task under the control of the Manager. */
  void AddTask(rtsTask *task);

  /*! Put a device under the control of the Manager. */
  void AddDevice(ddiDeviceInterface *device, const char *filename = NULL);
    
  /*! Connect output of a mailbox of a task with the input of a
    mailbox of a task.  It is assumed that the device and task are
    under TaskManger's control.
  
      \param fromTaskName Name of task that will function as sender.
    \param fromTaskMBox MailBox number of the 'from' task that will be used
    for communication.
    \param toTaskName Name of the task that will function as receiver.
    \param toTaskMBox MailBox number of the 'to' task that will be used
    for communication.
    */
  void ConnectMailBox(const char *fromTaskName, const int fromTaskMBox,
                        const char *toTaskName, const int toTaskMBox);

  /*! Link a state table of one task with another.  Note that
    linking does not allow the other tasks to write to that table,
    the other tasks can use the table for reading only.  It is
    assumed that the device and task are under TaskManger's control.
      
    \param fromTaskName Name of the task that is the write of the state table.
    \param toTaskName Name of the task that is the reader of the state table.
    */
  void LinkDataTables(const char *fromTaskName, const char *toTaskName, const char *filename=NULL);
    
  /*! Associate a device with a task.  This also executes the
    configure method to the task so that it can (re)configure
    itself. It is assumed that the device and task are under
    TaskManger's control.
      
      \param taskName Name of the task that is to be associated with the device
      \param deviceName Name of the device that is to be associated with the task.
    */
  void AssociateDevice(const char *taskName, std::string deviceName, const char *filename=NULL);

  /*! For debugging. Dumps to stream the maps maintained by the manager.
    Here is a typical output: 
    <CODE>
    Task Map: {Name, Address}
    { BSVO, 0x80a59a0 }
    { TRAJ, 0x81e3080 }
    { DISP, 0x81e38d8 }
    Device Map: {Name, Address}
    { BSVO, 0x80a59a0 }
    { LoPoMoCo, 0x81e40e0 }
    MailBox Connection Matrix: {From [#], To [#]}
    { BSVO [0] , BSVO [0]  }
    { DISP [0] , DISP [0]  }
    { TRAJ [0] , TRAJ [0]  }
    State Data Table Link Matrix: {From [Addr], To [Addr]}
    { BSVO [0x80a5a88] , DISP [0x81e39c0]  }
    { DISP [0x81e39c0] , BSVO [0x80a5a88]  }
    Device Association Map: {Task, Device}
    { BSVO, LoPoMoCo }
    { TRAJ, BSVO }
    </CODE>
   */
  void Debug(std::ostream& out) const;
    
  inline void Kill(void) {
      __os_exit();
      if (Main) delete Main;
  }
};

inline std::ostream & operator << (std::ostream & output,
                                   const rtsTaskManager & taskManager) {
    taskManager.Debug(output);
    return output;
}


CMN_DECLARE_SERVICES_INSTANTIATION(rtsTaskManager)


#endif // _rtsTaskManager_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsTaskManager.h,v $
// Revision 1.10  2006/05/11 18:46:48  anton
// rtsTaskManager: Update for new class registration.
//
// Revision 1.9  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/06/20 18:22:44  alamora
// Moved definition of GetInstance() to the cpp to avoid dllimport error C2491 on VC7; declarations are OK
//
// Revision 1.7  2005/06/17 20:30:50  alamora
// cisstOSAbstraction and cisstRealTime: Added some required
// CISST_EXPORT to compile DLLs.
//
// Revision 1.6  2005/04/04 20:40:52  kapoor
// rtsTaskManager:  Add ostream << operator.  Had to make the Debug(ostream)
// method const (cleaner).  Also, Debug() should be renamed ToStream() in the
// futures.
//
// Revision 1.5  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.4  2005/03/01 16:00:15  anton
// cisstRealTime: Converted std::cout to CMN_LOG.  See ticket #130.
//
// Revision 1.3  2005/02/28 22:40:44  anton
// rtsTaskManager: Added Kill method, doesn't seem to work.  For Ankur.
//
// Revision 1.2  2004/10/30 00:59:38  kapoor
// Added argument to pass filename to the configure method.
//
// Revision 1.1  2004/05/05 21:16:03  anton
// Added missing files for cisstRealTime from Ankur
// Corrected bug #28
//
//
// Revision 1.4  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.3  2004/03/19 15:57:50  kapoor
// Added AddDevice and AssociatedDevice methods.
//
// Revision 1.2  2004/03/16 17:37:37  kapoor
// Added ThreadBuddy class to more RTAI dependent stuff to cisstOSAbstraction
// from cisstRealTime
//
// Revision 1.1  2004/03/04 07:41:16  kapoor
// added task manager functionality.
//
// ****************************************************************************
