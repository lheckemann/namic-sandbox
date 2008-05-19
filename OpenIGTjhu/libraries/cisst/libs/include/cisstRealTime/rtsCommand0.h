/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsCommand0.h,v 1.3 2006/05/07 04:45:26 kapoor Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2005-05-03
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
  \brief Define an internal command for cisstRealTime
  \ingroup cisstRealTime
*/


#ifndef _rtsInternalCommand_h
#define _rtsInternalCommand_h

#include <cisstRealTime/rtsCommand0Base.h>
#include <cisstRealTime/rtsTask.h>



/*!
  \ingroup cisstRealTime

  Templated class for internal commands for task.  Now tasks are a bit
  strange. You want to have commands esp. the one that write to the
  task data, e.g updating the goals for a PID loop to be syncronized
  with the task period. So a simple method such as those used for
  writing to a hardware device will not work.  The workaround is to
  write these commands to a mailbox. Tasks use mailbox numbered zero,
  for these internal commands. The execute method of this command
  packs the data to be written into the internal command along with
  the operation to be performed and puts it on the mailbox (used here
  as a queue). The periodic loop of the task at the appropriate time,
  de-queues the command.
 */
template <class _receiverType>
class rtsCommand0 : public rtsCommand0Base {
 public:
    typedef ddiCommandBase::ReturnType (_receiverType::*ActionType)(ddiCommandBase &);
    ActionType Action;

 private:
    rtsTask *Task;
    PackedTypeArray Data;

 public:
    // packed array could be of different size
    rtsCommand0():rtsCommand0Base("??", ddiCommandBase::UNKNOWN) {}

    /*! rtsCommand0 takes an id to identify the command while
      debugging since it difficult to store a variable size string. */
    rtsCommand0(ActionType action, rtsTask *task, std::string name):
        rtsCommand0Base(name, ddiCommandBase::RTS_COMMAND_0),
        Action(action), Task(task) {
    }

    virtual ~rtsCommand0() {}

    virtual std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    virtual void ToStream(std::ostream& out) const {
        out << " Command Type: " << this->Type << ":";
        if (Task) out << Task->GetName();
        out << "::" << this->Name;
    }

    /*! for now we need to convert cmnMessageObject to packed array */
    virtual void SetData() {
        // flush the packed array, so that the object is clean this is
        // incase someone is using the same command with a new data
        Data.flush();
    }

    template <class __dataType>
        __dataType* GetData(void) {
            __dataType *pobj = new __dataType();
            return pobj;
        }

    virtual ddiCommandBase::ReturnType Execute() {
        Data.flush();
        if (Task) {
            return Task->QueueCommand(this, sizeof(*this));
        } else {
            return ddiCommandBase::NO_DEVICE;
        }
    }
    inline ActionType GetAction(void) {return Action;}
};


#endif // _rtsInternalCommand_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsCommand0.h,v $
// Revision 1.3  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.2  2006/05/04 05:46:45  kapoor
// Extended command0,1 to tasks. Appears to work. More testing needed.
//
// Revision 1.1  2006/05/03 18:38:09  kapoor
// *** empty log message ***
//
//
// ****************************************************************************
