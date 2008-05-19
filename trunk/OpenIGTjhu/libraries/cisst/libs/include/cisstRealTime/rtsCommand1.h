/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsCommand1.h,v 1.5 2006/06/03 00:27:45 kapoor Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2005-05-02
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

#include <cisstRealTime/rtsCommand1Base.h>
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
class rtsCommand1 : public rtsCommand1Base {
    typedef rtsCommand1Base BaseType;
 public:
    typedef ddiCommandBase::ReturnType (_receiverType::*ActionType)(ddiCommand1Base &);
    ActionType Action;

 private:
    rtsTask *Task;
    PackedTypeArray Data;
 public:
    // packed array could be of different size
    rtsCommand1():BaseType("??", ddiCommandBase::UNKNOWN, 0, 0) {}
    /*! rtsCommand1 takes an id to identify the command while
      debugging since it difficult to store a variable size string. */
    rtsCommand1(ActionType action, rtsTask *task, std::string name,
            const std::type_info *info, const cmnClassServicesBase* classInputTypeServices):
        BaseType(name, ddiCommandBase::RTS_COMMAND_1, info, classInputTypeServices),
        Action(action), Task(task) {
    }

    virtual ~rtsCommand1() {}

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
    virtual void SetData(cmnDataObject &obj) {
        // flush the packed array, so that the object is clean this is
        // incase someone is using the same command with a new data
        Data.flush();
        // pack the object. each object should have pack and unpack
        // methods
        obj.pack(Data);
    }

    template <class __dataType>
        __dataType* GetData(void) {
            __dataType *pobj = new __dataType();
            pobj->unpack(Data);
            return pobj;
        }

    virtual ddiCommandBase::ReturnType Execute(cmnDataObject &obj) {
        if (TypeInfo == 0) return ddiCommandBase::BAD_COMMAND;
        if (&typeid(obj) != TypeInfo) return ddiCommandBase::BAD_INPUT;
        Data.flush();
        obj.pack(Data);
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
// $Log: rtsCommand1.h,v $
// Revision 1.5  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.4  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
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
