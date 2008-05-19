/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaMailBox.h,v 1.10 2006/03/24 21:59:29 anton Exp $

  Author(s): Ankur Kapoor
  Created on: 2004-04-30

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
  \brief Declaration of osaMailBox
  \ingroup cisstOSAbstraction
*/

#ifndef _osaMailBox_h
#define _osaMailBox_h

#include <cisstCommon/cmnPortability.h>


#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_mbx.h>
#include <rtai_lxrt.h>
#include <rtai_types.h>
// end of CISST_LINUX_RTAI

#elif (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#include <string>
// end of CISST_WINDOWS

#else
 // posix
#define _XOPEN_SOURCE 600
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#endif


// Always the last file to include
#include <cisstOSAbstraction/osaExport.h>

/*!
  \brief Define a MailBox object
  \ingroup cisstOSAbstraction
 */
class CISST_EXPORT osaMailBox {

#if (CISST_OS == CISST_LINUX_RTAI)
    /*! Pointer to Incoming OS mailbox. */
    MBX *Incoming;
    /*! Pointer to Outgoing OS mailbox. */
    MBX *Outgoing;

#elif (CISST_OS == CISST_WINDOWS)
    /*! Pointer to Incoming OS mailbox. */
    HANDLE Incoming;
    HANDLE IncomingFile;
    std::string SlotNameIncoming;
    /*! Pointer to Outgoing OS mailbox. */
    HANDLE Outgoing;
    HANDLE OutgoingFile;
    std::string SlotNameOutgoing;

#else
    // posix
    int Incoming;
    int Outgoing;
#endif


    /*! The name of the incoming mailbox. */
    char NameIncoming[7];

    /*! The name of the outgoing mailbox. */
    char NameOutgoing[7];

public:
    /*! Enum type representing a timeout period of infinity and no
      wait. */
    enum TimeoutType {
        WAITFOREVER = -1,
        NOWAIT = 0
    };


    /*! Enum type for return values of a send/receive operation. */
    enum ReturnType {
        FAILED,        /*! The lock operation failed because the
                      semaphore couldn't be obtained */
        SUCCESS,    /*! The lock operation was successful */
        TIMEDOUT,    /*! The lock operation timedout after
                      waiting for specified time */
        NOCONNECTION /*! The mailbox was not connected using
                       the Connect method */
    };


    /*! Default constructor.  Does nothing. */
    osaMailBox();

    
    /*! Default distructor.  Does nothing. */
    ~osaMailBox();


    /*! Creates a mailbox.  Make available the outgoing port of the
      mailbox named 'name' so that other mailboxes can connect using
      this 'name'.
     */
    bool Create(const char* name, int size = 1024);


    /*! Connects to a mailbox.  Connect the incoming port of the
      mailbox to the outgoing port of mailbox named 'name'.
    */
    bool Connect(const char* name);

    /*! Send a large message.
       Sends a message msg to mailbox named mbx of size mbytes
       \param message Pointer to message to be sent
       \param size Size in bytes of the data that is to be transferred
       \param timeout If timeout == NOWAIT then it tries to send the message,
                        but does not block the sending thread
                 timeout == WAITFOREVER blocks the sending thread till
                        the message is sent
                 timeout > 0 blocks the sender till the message is sent or
                    timeout occurs.
       \return The method return SUCCESSFUL on success, a TIMEOUT if time
                expires or FAILED if error occurs.
     */
    osaMailBox::ReturnType Send(void *message, int size, int timeout = NOWAIT);


    /*! Send a small numeric message.
       Sends a numeric message msg to mailbox named mbx.
       This is not yet implemented and might be dropped all together.
       There is a difference in the way a generic message is sent (large
       message) and a numeric message is sent. The difference being one
       is implemented using one way mailboxes and the other through RPC's.
       \param message The numeric value that is to be sent
       \param timeout If timeout == NOWAIT then it tries to send the message,
                        but does not block the sending thread
                 timeout == WAITFOREVER blocks the sending thread till
                        the message is sent
                 timeout > 0 blocks the sender till the message is sent or
                        timeout occurs.
       \return The method return SUCCESSFUL on success, a TIMEOUT if time
            expires or FAILED if error occurs.
     */
    osaMailBox::ReturnType Send(unsigned int message, int timeout = NOWAIT);


    /*! Receive a large message.
       Receive a message msg to mailbox named mbx of size mbytes
       \param message Pointer to message where the data received is to be stored
       \param size Size in bytes of the data that is to be received
       \param timeout If timeout == NOWAIT then it tries to receive the message,
                        but does not block the receiving thread
                 timeout == WAITFOREVER blocks the receiving thread till
                        the message is sent
                 timeout > 0 blocks the receiver till the message is sent
                        or timeout occurs.
       \return The method return SUCCESSFUL on success, a TIMEOUT if time
            expires or FAILED if error occurs.
     */
    osaMailBox::ReturnType Receive(void* message, int size, int timeout = NOWAIT);


    /*! Receive a small numeric message.
       Receive a numeric message msg to mailbox named mbx.
       This is not yet implemented and might be dropped all together.
       \param message The location where the numeric value is to be stored
       \param timeout If timeout == NOWAIT then it tries to receive the message,
                        but does not block the receiving thread
                 timeout == WAITFOREVER blocks the receiving thread till
                        the message is sent
                 timeout > 0 blocks the receiver till the message is sent
                        or timeout occurs.
       \return The method return SUCCESSFUL on success, a TIMEOUT if time
                    expires or FAILED if error occurs.
     */
    osaMailBox::ReturnType Receive(unsigned int &message, int timeout = NOWAIT);
};


#endif // _osaMailBox_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaMailBox.h,v $
// Revision 1.10  2006/03/24 21:59:29  anton
// osaMailBox: Added preliminary version for unix (Linux, Darwin).  See #218.
//
// Revision 1.9  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.8  2006/03/17 03:32:31  kapoor
// osaMailBox: Fixes overflow in the name of mailbox char array.
//
// Revision 1.7  2006/03/02 22:10:27  pkaz
// cisstOSAbstraction:  added dummy implementations of osaThreadBuddy and osaMailbox for other operating systems
// (currently, implementations only exist for RTAI and Windows -- see #218).
//
// Revision 1.6  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/06/17 20:30:50  alamora
// cisstOSAbstraction and cisstRealTime: Added some required
// CISST_EXPORT to compile DLLs.
//
// Revision 1.4  2005/04/25 15:01:59  kapoor
// osaMailBox: Moved windows code for mailbox lying in my folders to main repository. This might change in future. Some limitations exists in using mailbox of windows, the main concern being the size of data transferred is limited to 454 bytes in windows.
//
// Revision 1.3  2005/04/24 16:46:30  kapoor
// osaMailBox: Connect & Addmailbox return bool flag to indicate success/failure.
//
// Revision 1.2  2005/01/10 19:21:53  anton
// cisstOSAbastraction: Added osaExport.h and required CISST_EXPORT to compile
// as a DLL (for windows).
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.6  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.5  2004/03/22 00:09:54  kapoor
// Reverted to using void * for sending messages.
//
// Revision 1.2  2004/03/04 21:26:14  kapoor
// more changes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
