/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsTaskManager.cpp,v 1.9 2006/05/11 18:46:48 anton Exp $
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

#include <cisstRealTime/rtsTaskManager.h>


CMN_IMPLEMENT_SERVICES(rtsTaskManager);


rtsTaskManager::rtsTaskManager(){
  Main = new osaThreadBuddy();
  __os_init();
}


rtsTaskManager::~rtsTaskManager(){
    this->Kill();
}


rtsTaskManager* rtsTaskManager::GetInstance(void) {
    static rtsTaskManager instance;
    return &instance;
}
void rtsTaskManager::AddTask(rtsTask *task) {
  TaskMap.insert(std::pair<const char*, rtsTask *>(task->GetName(), task));
  ConnectMailBox(task->GetName(), 0, task->GetName(), 0);
}


void rtsTaskManager::AddDevice(ddiDeviceInterface *device, const char *filename) {
  DeviceMap.insert(std::pair<std::string, ddiDeviceInterface *>(device->GetName(), device));
  // this is a good place to configure the device. we assume that
  // the devices are added in order from lowest level device a.k.a
  // hardware to higher level device
  device->Configure(filename);
}


void rtsTaskManager::Debug(std::ostream& out) const {
  TaskMapType::const_iterator iter;
  out << "Task Map: {Name, Address}" << std::endl;
  for (iter = TaskMap.begin(); iter != TaskMap.end(); ++iter) {
    out << " { " << iter->first << ", " << iter->second << " }" << std::endl;
  }
  DeviceMapType::const_iterator deviceiter;
  out << "Device Map: {Name, Address}" << std::endl;
  for (deviceiter = DeviceMap.begin(); deviceiter != DeviceMap.end(); ++deviceiter) {
    out << " { " << deviceiter->first << ", " << deviceiter->second << " }" << std::endl;
  }
  MailBoxConnectionMatrixType::const_iterator mboxiter;
  out << "MailBox Connection Matrix: {From [#], To [#]}" << std::endl;
  for (mboxiter = MailBoxConnectionMatrix.begin(); mboxiter != MailBoxConnectionMatrix.end(); ++mboxiter) {
    out << " { " << mboxiter->first.first << " [" << mboxiter->first.second << "] "
      << ", " << mboxiter->second.first << " [" << mboxiter->second.second << "] "
      << " }" << std::endl;
  }
  StateDataTableLinkMatrixType::const_iterator tableiter;
  out << "State Data Table Link Matrix: {From [Addr], To [Addr]}" << std::endl;
  for (tableiter = StateDataTableLinkMatrix.begin(); tableiter != StateDataTableLinkMatrix.end(); ++tableiter) {
    out << " { " << tableiter->first.first << " [" << tableiter->first.second << "] " 
      << ", " << tableiter->second.first << " [" << tableiter->second.second << "] "
      << " }" << std::endl;
  }
  DeviceAssociationMapType::const_iterator associationiter;
  out << "Device Association Map: {Task, Device}" << std::endl;
  for (associationiter = DeviceAssociationMap.begin(); associationiter != DeviceAssociationMap.end(); ++associationiter) {
    out << " { " << associationiter->first << ", " << associationiter->second << " }" << std::endl;
  }
}


void rtsTaskManager::ConnectMailBox(const char *fromTaskName, const int fromTaskMBox,
                                    const char *toTaskName, const int toTaskMBox) {
  TaskMapType::iterator iter;
  TaskMapType::iterator iterFrom = TaskMap.end();
  TaskMapType::iterator iterTo = TaskMap.end();
  for (iter = TaskMap.begin(); iter != TaskMap.end(); ++iter) {
    if (strncmp(iter->first, fromTaskName, 4) == 0) {
      iterFrom = iter;
    }
    if (strncmp(iter->first, toTaskName, 4) == 0) {
      iterTo = iter;
    }
  }
  if (iterTo == TaskMap.end() || iterFrom == TaskMap.end()) {
          CMN_LOG_CLASS(5) << "Couldn't find tasks (from = " << fromTaskName
                             << ", to = " << toTaskName << ")" << std::endl;
    return;
  }
  /*
      if (iterTo == iterFrom) {
    std::cout << "Cant connect same tasks" << std::endl;
    return;
  }
  */
  rtsTask *toTask = iterTo->second;
  toTask->Connect(toTaskMBox, iterFrom->first, fromTaskMBox);
  MailBoxConnectionMatrix.push_back(MailBoxConnectionType(MailBoxIdentifierType(fromTaskName, fromTaskMBox),
                                                            MailBoxIdentifierType(toTaskName, toTaskMBox)));
}


void rtsTaskManager::LinkDataTables(const char *fromTaskName, const char *toTaskName, const char *filename) {
  TaskMapType::iterator iter;
  TaskMapType::iterator iterFrom = TaskMap.end();
  TaskMapType::iterator iterTo = TaskMap.end();
  for (iter = TaskMap.begin(); iter != TaskMap.end(); ++iter) {
    if (strncmp(iter->first, fromTaskName, 4) == 0) {
      iterFrom = iter;
    }
    if (strncmp(iter->first, toTaskName, 4) == 0) {
      iterTo = iter;
    }
  }
  if (iterTo == TaskMap.end() || iterFrom == TaskMap.end()) {
    CMN_LOG_CLASS(5) << "Couldn't find tasks" << std::endl;
    return;
  }
  if (iterTo == iterFrom) {
    CMN_LOG_CLASS(5) << "Can't connect same tasks" << std::endl;
    return;
  }
  rtsTask *fromTask = iterFrom->second;
  rtsTask *toTask = iterTo->second;
  const rtsStateDataTable *fromStateDataTable = fromTask->GetStateDataTable();
  const rtsStateDataTable *toStateDataTable = toTask->GetStateDataTable();
  toTask->AddExternalStateDataTable(iterFrom->first, fromStateDataTable);
  StateDataTableLinkMatrix.push_back(StateDataTableLinkType(StateDataTableIdentifierType(fromTaskName, fromStateDataTable),
                  StateDataTableIdentifierType(toTaskName, toStateDataTable)));
  toTask->Configure(filename);
}


void rtsTaskManager::AssociateDevice(const char *taskName, std::string deviceName, const char *filename) {
  DeviceMapType::iterator deviceiter = DeviceMap.end();
  TaskMapType::iterator iter;
  TaskMapType::iterator taskiter = TaskMap.end();
  for (iter = TaskMap.begin(); iter != TaskMap.end(); ++iter) {
    if (strncmp(iter->first, taskName, 4) == 0) {
      taskiter = iter;
    }
  }
  if (taskiter == TaskMap.end()) {
        CMN_LOG_CLASS(5) << "Can't find task: " << taskName << std::endl;
    return;
  }
  deviceiter = DeviceMap.find(deviceName);
  if (deviceiter == DeviceMap.end()) {
        CMN_LOG_CLASS(5) << "Can't find device: " << deviceName << std::endl;
    return;
  }
  rtsTask *task = taskiter->second;
  ddiDeviceInterface *device = deviceiter->second;
  DeviceAssociationMap.insert(std::pair<const char *, std::string>(taskiter->first, deviceiter->first));
  task->AssociateDevice(device);
  //this is a good place to call task configure, becuase we know that
  //the device to be associated is ready, Adddevice should have done
  //that.
  task->Configure(filename);
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsTaskManager.cpp,v $
// Revision 1.9  2006/05/11 18:46:48  anton
// rtsTaskManager: Update for new class registration.
//
// Revision 1.8  2006/03/02 06:14:23  pkaz
// cisstRealTime:  added newThread parameter to rtsTask constructor and undid checkin [1842].
// Fixed typos in comments and added more info to log messages.
//
// Revision 1.7  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/06/20 20:11:14  alamora
// Moved definition of GetInstance() to the cpp to avoid dllimport error C2491 on VC7; declarations are OK
//
// Revision 1.5  2005/04/04 20:40:52  kapoor
// rtsTaskManager:  Add ostream << operator.  Had to make the Debug(ostream)
// method const (cleaner).  Also, Debug() should be renamed ToStream() in the
// futures.
//
// Revision 1.4  2005/03/01 16:00:15  anton
// cisstRealTime: Converted std::cout to CMN_LOG.  See ticket #130.
//
// Revision 1.3  2005/02/28 22:40:44  anton
// rtsTaskManager: Added Kill method, doesn't seem to work.  For Ankur.
//
// Revision 1.2  2004/10/30 00:52:40  kapoor
// Added argument to pass a filename to configure the task/device.
//
// Revision 1.1  2004/05/05 21:16:03  anton
// Added missing files for cisstRealTime from Ankur
// Corrected bug #28
//
//
// Revision 1.7  2004/03/22 00:11:57  kapoor
// Mailbox zero is now a loopback, that is outmailbox #0 is connected to inmailbox
// #0 for every task.
//
// Revision 1.6  2004/03/19 16:01:28  kapoor
// Added AssociateDevice and AddDevice methods.
//
// Revision 1.5  2004/03/16 17:45:35  kapoor
// Added ThreadBuddy class to more RTAI dependent stuff to cisstOSAbstraction
// from cisstRealTime
//
// Revision 1.4  2004/03/05 05:13:47  kapoor
// Changed Send and Receive methods to be templated, as a result it was required to increase the stack size to 16Kb from 8Kb. Is it good or bad?
//
// Revision 1.3  2004/03/05 04:56:16  kapoor
// Fixed from-to of ConnectMailBox method; They were swaped.
//
// Revision 1.2  2004/03/04 21:26:11  kapoor
// more changes.
//
// Revision 1.1  2004/03/04 07:39:15  kapoor
// lots of changes.
//
// ****************************************************************************
