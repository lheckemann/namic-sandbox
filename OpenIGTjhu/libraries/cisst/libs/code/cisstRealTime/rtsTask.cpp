/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsTask.cpp,v 1.18 2006/05/30 19:35:04 kapoor Exp $
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

#include <cisstCommon/cmnExport.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstRealTime/rtsTask.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMailBox.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTime.h>

#include <iostream>
#include <string>


rtsTask::rtsTask(const char* name, double periodicity, bool isHardRealTime, int sizeDataTable, bool newThread):
  ddiDeviceInterface(name), Thread(0), ThreadBuddy(0),
    ExitFlag(false),
    NewThread(newThread),
    Device(NULL),
  Period(periodicity), IsHardRealTime(isHardRealTime),
  StateDataTable(sizeDataTable), Initialized(false),
  EndTask(false), Started(false), Terminated(false), OverranPeriod(false),
  Tic(0), Toc(0)
{
  int i;
  char mailBoxName[6];
    if (NewThread) {
      Thread = new osaThread();
      if (!Thread) {
          CMN_LOG(1) << "Class rtsTask: " << CMN_LOG_DETAILS
                       << "OOPS! There was a problem in creating task. Dont know what to do!" << std::endl;
      }
    }
    else {
        // In this case (not creating a new thread), we will use the MAIN
        // thread, so we create the thread buddy here.
        ThreadBuddy = new osaThreadBuddy();
        ThreadBuddy->Create(Name, Period);
        Initialized = true;
        // Set the Started flag true in case anyone calls WaitToStart().
        // Note that the user will have to explicity call the Startup()
        // method if this method is needed.
        Started = true;
    }
  osaMailBox *mailBox = new osaMailBox();
  if (!mailBox) {
      CMN_LOG(1) << "Class rtsTask: " << CMN_LOG_DETAILS
                   << "OOPS! There was a problem in creating task. Dont know what to do!" << std::endl;
  }
  EndTask = false;
  Terminated = false; // rht
  for (i = 0; i < 4 && name[i] != '\0'; i++) {
    Name[i] = name[i];
    mailBoxName[i] = name[i];
  }
  Name[i + 0] = '\0';
  mailBoxName[i + 0] = '1'; mailBoxName[i + 1] = '\0';
  mailBox->Create(mailBoxName);
  MailBoxes.push_back(mailBox);
}

void rtsTask::Create(void)
{
    if (NewThread) {
      CMN_LOG(7) << "Class rtsTask: Creating thread " << Name << std::endl;
      Thread->Create<rtsTask, void*>(this, &rtsTask::RunInternal);
    }
}

rtsTask::~rtsTask() {
  CMN_LOG(3) << "Class rtsTask: Base class destructor" << std::endl;
//#if (CISST_OS == CISST_LINUX_RTAI)
        //probably this should also kill the proxy if there is one
        //should also be killing the task if it was not already killed
        //anyway have to revisit killing of task
        CMN_LOG(7) << "Class rtsTask: Deleting task " << Name << std::endl;
        //If the task was waiting on a queue, i.e. semaphore, mailbox,
        //etc, it is removed from such a queue and messaging tasks
        //pending on its message queue are unblocked with an error return.

        if (!Terminated) {
            if (Thread) {
                if (IsStarted()) {
                    // First, we try to kill the thread by setting EndFlag true
                    Kill();
                    // Now, wait for 2 periods to see if it was killed
                    osaTime::Sleep((long)(2*Period*1e-6)); // period is in ns
                }
                if (!IsTerminated()) {
                    // If thread not dead, delete it.
                    Thread->Delete();
                    CMN_LOG(7) << "class rtsTask: deleting thread" << std::endl;
                    // Now, wait for it to finish (should have a timeout)
                    WaitToTerminate();
                    //It is safe to call CleanupInternal() more than once.
                    //Should we call the user-supplied Cleanup()?
                    CleanupInternal();
                }
            }
            else  {
                // this handles the MAIN thread
                //Should we call the user-supplied Cleanup()?
                CleanupInternal();
            }
        }
        //if (ThreadBuddy) delete ThreadBuddy;
        CMN_LOG(7) << "Class rtsTask: ThreadBuddy deleted" << std::endl;
        //wait for thread to complete
        //pThread->Wait();
        //if (Device) delete Device;
        //for (unsigned int i=0; i<MailBoxes.size(); i++) {
  //                if (MailBoxes[i]) delete MailBoxes[i];
  //}
  CMN_LOG(7) << "Class rtsTask: MailBoxes deleted" << std::endl;
        //if (pThread) delete pThread;
//#endif
//#if (CISST_OS == CISST_WINDOWS)
//#endif
}


void *rtsTask::RunInternal(void*) {
  StartupInternal();
  // ak replace this with a semaphore
    CMN_LOG(3) << Name << ": Wait for start." << std::endl;
  while (!this->Initialized) {
    ThreadBuddy->WaitForPeriod();
  }
  this->Startup();
  Started = true;
  // allow no more stack allocation. allowing this will
  // break realtime performance.
  // local all pages too
  //ThreadBuddy->LockStack();
  if (IsHardRealTime) {
    ThreadBuddy->MakeHardRealTime();
  }
  while (!this->EndTask) {
    Tic = osaTime::GetCurrentTime();
    this->Run();
    Toc = osaTime::GetCurrentTime();
    if ((Toc - Tic) > Period) OverranPeriod = true;
    ThreadBuddy->WaitForRemainingPeriod();
  }
  if (IsHardRealTime) {
    ThreadBuddy->MakeSoftRealTime();
  }
  ThreadBuddy->UnlockStack();
  CMN_LOG(10) << "End of task " << Name << std::endl;
  this->Cleanup();
  CleanupInternal();
  Terminated = true;
  return this;
}


void rtsTask::StartupInternal(void) {
    CMN_LOG(3) << "Class rtsTask: Inside StartupInternal for " << Name << std::endl;
  ThreadBuddy = new osaThreadBuddy();
  ThreadBuddy->Create(Name, Period);
    if (!ThreadBuddy->IsPeriodic()) {
        // is the task is not periodic, wait for period wont work and RunInternal would
        // wait for every
        Initialized = true;
    }
}


void rtsTask::CleanupInternal() {
        //If the task was waiting on a queue, i.e. semaphore, mailbox,
        //etc, it is removed from such a queue and messaging tasks
        //pending on its message queue are unblocked with an error return.
  if (ThreadBuddy) {
    delete ThreadBuddy;
    ThreadBuddy = NULL;
  }
  for (unsigned int i = 0; i < MailBoxes.size(); i++) {
    if (MailBoxes[i]) {
      delete MailBoxes[i];
      MailBoxes[i] = NULL;
    }
  }
  CMN_LOG_CLASS(3) << "Done base class CleanupInternal " << Name << std::endl;
}


bool rtsTask::AddMailBox(void) {
  int i;
  char mailBoxName[6];
  for (i = 0; i < 4 && Name[i] != '\0'; i++) {
    mailBoxName[i] = Name[i];
  }
  int size = MailBoxes.size();
  if (size>10) return false;
  mailBoxName[i + 0] = (size<9) ? '1' + size: '0'; mailBoxName[i + 1] = '\0';
  osaMailBox *mailBox = new osaMailBox();
  bool returnValue = true;
  if (mailBox) {
    returnValue = mailBox->Create(mailBoxName);
  } else {
      CMN_LOG_CLASS(1) << CMN_LOG_DETAILS
                         << "Couldn't get pointer to mailbox" << std::endl;
      returnValue = false;
  }
  MailBoxes.push_back(mailBox);
  return returnValue;
}


bool rtsTask::Connect(int number, const char* name, int toNumber) {
  if (toNumber > 10) return false;
  char tmpName[6];
  int i;
  for (i = 0; i < 4 && name[i] != '\0'; i++) {
    tmpName[i] = name[i];
  }
  tmpName[i+0] = (toNumber<9)?'1'+toNumber:'0'; tmpName[i+1]='\0';
  //this should return or throw??
  return MailBoxes[number]->Connect(tmpName);
}


const rtsStateDataTable * rtsTask::GetExternalStateDataTable(const char* taskName) {
  StateDataTableMapType::iterator iter;
  for (iter = ExternalStateDataTableMap.begin(); iter != ExternalStateDataTableMap.end(); ++iter) {
    if (strncmp(iter->first, taskName, 4)==0) {
      return iter->second;
    }
  }
  CMN_LOG_CLASS(1) << CMN_LOG_DETAILS
                     << "Couldn't find specified task: " << taskName << std::endl;
  return 0;
}

void rtsTask::AddExternalStateDataTable(const char* taskName, const rtsStateDataTable* otherTable) {
  ExternalStateDataTableMap.insert(std::pair<const char* ,const rtsStateDataTable*>(taskName, otherTable));
}

#if 0
void rtsTask::SetUpdater(rtsStateDataId id, ddiDeviceInterface::OperationPointerBaseType &ptr)  {
  PtrsToDataMethods.resize(id + 1);
  PtrsToDataMethods[id] = ptr;
}


bool rtsTask::UpdateState(rtsStateDataId id) {
  bool result;
  if (PtrsToDataMethods[id] != NULL) {
    result = (Device->*(PtrsToDataMethods[id]))(StateDataTable.GetReference(id));
  } else {
    result = false;
  }
  return result;
}
#endif

bool rtsTask::WaitToTerminate(unsigned long timeOutms) {
  CMN_LOG_CLASS(7) << "WaitToTerminate " << Name << std::endl;
    if (Thread) {
        if (timeOutms == 0) {
            Thread->Wait();
        } else {
            // Add a way to interrupt if timeOut expires
            Thread->Wait();
        }
    }
  return true;
}


bool rtsTask::WaitToStart() {
  while (!Started) {
    osaTime::Sleep((long)(Period * 1e-6)); // period is in ns
  }
  return true;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsTask.cpp,v $
// Revision 1.18  2006/05/30 19:35:04  kapoor
// cisstRealTime: fixes possible race conditions in creation of tasks.
// See ticket #227.
//
// Revision 1.17  2006/05/06 20:41:33  kapoor
// cisstRealTime: Fixed support for non-periodic & (new thread/no-new thread)
// options. This fixes the case when there should be a new thread but non
// periodic. See also [1846] & [1905].
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
// Revision 1.13  2006/03/02 22:07:48  pkaz
// rtsTask.cpp:  changed order of data initialization to avoid compiler warning.
//
// Revision 1.12  2006/03/02 06:14:23  pkaz
// cisstRealTime:  added newThread parameter to rtsTask constructor and undid checkin [1842].
// Fixed typos in comments and added more info to log messages.
//
// Revision 1.11  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.10  2005/06/19 22:06:16  kapoor
// cisstRealTime: BUG FIX: Mising UnlockStack added when we go hard real time ends
//
// Revision 1.9  2005/05/03 17:06:22  anton
// rtsTask: Allow WaitToTerminate() with no timeout (default value = 0) plus
// some CMN_LOG to replace std::cout.
//
// Revision 1.8  2005/04/24 16:46:30  kapoor
// osaMailBox: Connect & Addmailbox return bool flag to indicate success/failure.
//
// Revision 1.7  2005/04/23 00:34:28  kapoor
// rtsTask: Added a flag to tell if the execution of task overran allocated
// period.
//
// Revision 1.6  2005/04/16 21:27:02  kapoor
// Added WaitToTerminate to check if thread has completed. This was earlier done by destructor only.
//
// Revision 1.5  2005/03/17 23:16:12  kapoor
// Real-Time: CISST library ThreadBuddy bug (see ticket #135)
//
// Revision 1.4  2005/03/01 16:00:15  anton
// cisstRealTime: Converted std::cout to CMN_LOG.  See ticket #130.
//
// Revision 1.3  2005/02/28 22:43:34  anton
// cisstDevices and real-time:  Added Log (see #130).
//
// Revision 1.2  2004/10/30 00:52:40  kapoor
// Added argument to pass a filename to configure the task/device.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.8  2004/04/08 06:08:52  kapoor
// Period is now a double
//
// Revision 1.7  2004/03/22 00:31:00  kapoor
// *** empty log message ***
//
// Revision 1.6  2004/03/19 16:02:02  kapoor
// Changed member name device to Device.
//
// Revision 1.5  2004/03/16 17:46:16  kapoor
// *** empty log message ***
//
// Revision 1.4  2004/03/05 05:13:47  kapoor
// Changed Send and Receive methods to be templated, as a result it was required to increase the stack size to 16Kb from 8Kb. Is it good or bad?
//
// Revision 1.3  2004/03/04 21:26:11  kapoor
// more changes.
//
// Revision 1.2  2004/03/04 07:39:15  kapoor
// lots of changes.
//
// Revision 1.1.1.1  2004/03/02 04:05:02  kapoor
// Importing source into local tree
//
// ****************************************************************************
