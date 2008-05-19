/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsCommand0Base.h,v 1.3 2006/05/07 04:45:26 kapoor Exp $
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
  \brief Defines and internal command for rtsTask
  \ingroup cisstRealTime
*/

#ifndef _rtsCommand0Base_h
#define _rtsCommand0Base_h

#include <cisstDeviceInterface/ddiCommandBase.h>

#define RTS_DECLARE_COMMAND0(commandName) \
public:\
    ddiCommandBase::ReturnType commandName();\
protected:\
    ddiCommandBase::ReturnType commandName##Protected (ddiCommandBase &msg);\

#define RTS_ADD_METHOD_COMMAND0(className, commandName) \
    this->AddMethodZeroArg(&className::commandName##Protected, this, #commandName);

#define RTS_BEGIN_IMPLEMENT_COMMAND0(className, commandName) \
ddiCommandBase::ReturnType className::commandName() {\
    static rtsCommand0Base* cmdObj = this->GetMethodZeroArg(#commandName);\
        return cmdObj->Execute();\
}    \
ddiCommandBase::ReturnType className::commandName##Protected (ddiCommandBase &msg) {

#define RTS_END_IMPLEMENT_COMMAND0 }

/*!
  \ingroup cisstRealTime

  Base class for internal commands for task. Now tasks are a bit
  strange. You want to have commands esp. the one that write to the
  task data, e.g updating the goals for a PID loop to be syncronized
  with the task period. So a simple method such as those used for
  writing to a hardware device will not work.  The workaround is to
  write these commands to a mailbox. Tasks use mailbox numbered zero,
  for these internal commands. The execute method of this command
  packs the data to be written into the internal command along with
  the operation to be performed and puts it on the mailbox (used here
  as a queue). The periodic loop of the task at the appropriate time,
  de-queues the command.  The base class exists so that rtsTask can
  compile without knowing about the template paramets.
 */
class rtsCommand0Base : public ddiCommandBase {
    typedef ddiCommandBase BaseType;
public:
  /*! Default constructor. Does nothing. */
  rtsCommand0Base():BaseType() {}
    rtsCommand0Base(std::string name, BaseType::CommandType type):
        BaseType(name, type) {}

  /*! Default destructor. Does nothing. */
  virtual ~rtsCommand0Base() {}

  /*! Dump output to stream. */
  virtual std::string ToString() const = 0;
  virtual void ToStream(std::ostream& out) const = 0;

  /*! Execute the command. 
    The execute method for internal commands is different. This does not
    directly execute the operation on the object. Instead it queues the
    operation and the object argument by first packing it into a fixed 
    size object and then putting it on the queue for the task. Packing
    is important so that we can handle different objects at the receiveing
    end without bothering about the size. To understand the need for
    fixed size for data transfer it is important to understand the 
    operation of mailboxes, pipes or sockets. For transfers the object
    must be "deep" copied, that it copied elementwise and then sent on 
    the medium for transfer. Not knowing the size at sending end is okay 
    because it can be easily found. But at the receive end all we get is 
    a big blob of memory with some bits set or unset. To decipher where 
    is the begning or end, we must not only know the size but also know 
    how the data is arranged in memory. This is easy if the objects to be 
    transferred are know in advance at both ends. Most likely this is not 
    the case as one would like to send chars, doubles, long etc all along 
    the same channel/medium. Hence the need to pack data.
    Now comming to question of queuing. For task, we want to ensure that
    the operation is execute in sync with the task itself. To do this
    we queue the operation and let the task de-queue it at an appropriate
    time and execute it.
    \param obj The data to be operated on.
   */
  virtual ddiCommandBase::ReturnType Execute() = 0;
};


#endif // _rtsCommand0Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsCommand0Base.h,v $
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
