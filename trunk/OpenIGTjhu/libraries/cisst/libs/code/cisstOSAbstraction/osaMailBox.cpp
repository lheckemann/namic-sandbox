/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaMailBox.cpp,v 1.8 2006/03/24 21:59:29 anton Exp $

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

#include <cisstOSAbstraction/osaMailBox.h>
#include <cisstCommon/cmnLogger.h>


#if (CISST_OS == CISST_LINUX_RTAI)
osaMailBox::osaMailBox(): Incoming(0), Outgoing(0) {}
#elif (CISST_OS == CISST_WINDOWS)
osaMailBox::osaMailBox(): Incoming(0), IncomingFile(0), Outgoing(0), OutgoingFile(0) {}
#else
osaMailBox::osaMailBox(): Incoming(-1), Outgoing(-1) {}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
osaMailBox::~osaMailBox() {
    if (Outgoing) rt_mbx_delete(Outgoing);
    if (Incoming) rt_mbx_delete(Incoming);
}
#elif (CISST_OS == CISST_WINDOWS)
osaMailBox::~osaMailBox() {
    if (OutgoingFile) CloseHandle(OutgoingFile);
}
#else
osaMailBox::~osaMailBox() {
    if (Incoming >= 0) close(Incoming);
    if (Outgoing >= 0) close(Outgoing);
    if (unlink(NameIncoming) == -1) {
        int errorCode = errno;
        CMN_LOG(1) << "Class osaMailBox: unlink failed ("
                   << strerror(errorCode) << ")" << std::endl;
    }
}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
bool osaMailBox::Create(const char *name, int size) {
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameOutgoing[i] = name[i];
    }
    NameOutgoing[i+0] = 'M';
    NameOutgoing[i+1] = '\0';
    CMN_LOG(3) << "Class osaMailBox: Creating " << NameOutgoing << std::endl;
    if (rt_get_adr(nam2num(NameOutgoing)) == 0) {
        Outgoing = (MBX*)rt_mbx_init(nam2num(NameOutgoing), size);
        CMN_LOG(3) << "Class osaMailBox: Mailbox \"" << NameOutgoing << "\" created successfully." << std::endl;
        return true;
     } else {
        CMN_LOG(1) << "Class osaMailBox: Mailbox \"" << NameOutgoing << "\" creation failed." << std::endl;
        return false;
     }
}

#elif (CISST_OS == CISST_WINDOWS)
bool osaMailBox::Create(const char *name, int size) {
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameIncoming[i] = name[i];
    }
    NameIncoming[i+0] = 'M';
    NameIncoming[i+1] = '\0';
    SlotNameIncoming += "\\\\.\\mailslot\\";
    SlotNameIncoming += NameIncoming;
    CMN_LOG(3) << "Class osaMailBox: Creating " << SlotNameIncoming << std::endl;
    Incoming = CreateMailslot(SlotNameIncoming.c_str(), 
                              0,                             // no maximum message size 
                              MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
                              (LPSECURITY_ATTRIBUTES) NULL);
    if (Incoming == INVALID_HANDLE_VALUE) { 
        CMN_LOG(1) << "Class osaMailBox: CreateMailslot failed with " << GetLastError() << std::endl;
        return false; 
    } else {
        CMN_LOG(3) << "Class osaMailBox: Mailslot created successfully." << std::endl;
    }
    return true; 
}

#else
// posix
bool osaMailBox::Create(const char *name, int size) {
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameIncoming[i] = name[i];
    }
    NameIncoming[i+0] = 'M';
    NameIncoming[i+1] = '\0';

    CMN_LOG(3) << "Class osaMailBox: Creating fifo for " << NameIncoming << std::endl;

    int errorCode;
    if (mkfifo(NameIncoming, S_IRWXU) != 0) {
        errorCode = errno;
        CMN_LOG(1) << "Class osaMailBox: mkfifo failed (" << strerror(errorCode) << ")" << std::endl;
        return false;
    } else {
        CMN_LOG(3) << "Class osaMailBox: mkfifo succeeded." << std::endl;
        if ((Incoming = open(NameIncoming, O_RDONLY|O_NONBLOCK)) == -1) {
            errorCode = errno;
            CMN_LOG(1) << "Class osaMailBox: open failed on the reader's end ("
                       << strerror(errorCode) << ")" << std::endl;
            return false;
        } else {
            CMN_LOG(3) << "Class osaMailBox: open succeeded on the reader's end." << std::endl;
        }
    }
    return true;
}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
bool osaMailBox::Connect(const char *name) {
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameIncoming[i] = name[i];
    }
    NameIncoming[i+0] = 'M';
    NameIncoming[i+1] = '\0';
    Incoming = (MBX*)rt_get_adr(nam2num(NameIncoming));
    if (Incoming == 0) {
        CMN_LOG(1) << "Class osaMailBox: Couldn't connect to mailbox named: " << NameIncoming << std::endl;
        return false;
    }
    return true;
}


#elif (CISST_OS == CISST_WINDOWS)
bool osaMailBox::Connect(const char *name) {
    // this should find handle of this mail box.
    // then open a file handle.
    // in read we receive we read from this file handle.
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameOutgoing[i] = name[i];
    }
    NameOutgoing[i+0] = 'M';
    NameOutgoing[i+1] = '\0';
    SlotNameOutgoing += "\\\\.\\mailslot\\";
    SlotNameOutgoing += NameOutgoing;
    CMN_LOG(3) << "Class osaMailBox: Creating " << SlotNameOutgoing << std::endl;
    OutgoingFile = CreateFile(SlotNameOutgoing.c_str(), 
                              GENERIC_WRITE, 
                              FILE_SHARE_READ,
                              (LPSECURITY_ATTRIBUTES) NULL, 
                              OPEN_EXISTING, 
                              FILE_ATTRIBUTE_NORMAL, 
                              (HANDLE) NULL); 
 
    if (OutgoingFile == INVALID_HANDLE_VALUE)  { 
        CMN_LOG(1) << "Class osaMailBox: CreateFile failed with " << GetLastError() << std::endl; 
        return false; 
    } else {
        CMN_LOG(3) << "Class osaMailBox: Mailslot connected successfully." << std::endl;
    }
    return true;
}

#else
// posix
bool osaMailBox::Connect(const char *name) {
    int i;
    for(i = 0; i < 5 && name[i] != '\0'; i++) {
        NameOutgoing[i] = name[i];
    }
    NameOutgoing[i+0] = 'M';
    NameOutgoing[i+1] = '\0';

    CMN_LOG(3) << "Class osaMailBox: Connecting to fifo for " << NameOutgoing << std::endl;
    if ((Outgoing = open(NameOutgoing, O_WRONLY)) == -1) {
        int errorCode = errno;
        CMN_LOG(1) << "Class osaMailBox: open failed for on the writer's end ("
                   << strerror(errorCode) << ")" << std::endl;
        return false;
    } else {
        CMN_LOG(3) << "Class osaMailBox: open succeeded on the writer's end." << std::endl;
    }
    return true;
}
#endif


/* FIXME: void pointer is C-style, look at some other means to send
generic msg.  RTTI is the obvious choice, but RTTI isnt feasible here,
coz we are sending in real-time. For now a C-style cast is being used
*/
#if (CISST_OS == CISST_LINUX_RTAI)
osaMailBox::ReturnType osaMailBox::Send(void *message, int size, int timeout) {
    ReturnType result;
    if (Outgoing == 0)
        return NOCONNECTION;
    if (timeout == NOWAIT) {
        if (rt_mbx_send_if(Outgoing, message, size) == 0) {
            result = SUCCESS;
        } else {    
            result = FAILED;
        }
    } else if (timeout == WAITFOREVER) {
        if (rt_mbx_send(Outgoing, message, size) == 0) {
            result = SUCCESS;
        } else {
            result = FAILED;
        }
    } else {
        if (rt_mbx_send_timed(Outgoing, message, size, timeout) == 0) {
            result = SUCCESS;
        } else {
            result = TIMEDOUT;
        }
    }
    return result;
}

#elif (CISST_OS == CISST_WINDOWS)
osaMailBox::ReturnType osaMailBox::Send(void *message, int size, int timeout) {
    DWORD written;
    BOOL fResult = WriteFile(OutgoingFile, 
                             message, 
                             (DWORD) size,  // add terminating null 
                             &written, 
                             (LPOVERLAPPED) NULL); 
    
    if (!fResult) { 
        CMN_LOG(5) << "Class osaMailBox: WriteFile failed with " << GetLastError() << std::endl;
        return FAILED; 
    } 
    if (written != size) {
        CMN_LOG(5) << "Class osaMailBox: WriteFile failed to write completely" << std::endl;
        return FAILED; 
    }
    return SUCCESS;
}

#else
osaMailBox::ReturnType osaMailBox::Send(void *message, int size, int timeout) {
    CMN_LOG(7) << "Class osaMailBox: Send" << std::endl;
    if (Outgoing < 0) {
        CMN_LOG(5) << "Class osaMailBox: Send failed, not connected." << std::endl;
        return NOCONNECTION;
    }
    if (write(Outgoing, message, size) == -1) {
        int errorCode = errno;
        CMN_LOG(5) << "Class osaMailBox: error during write ("
                   << strerror(errorCode) << ")" << std::endl;
        return FAILED;
    }
    return SUCCESS;
}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
osaMailBox::ReturnType osaMailBox::Receive(void *message, int size, int timeout) {
    ReturnType result;
    if (Incoming == 0)
        return  NOCONNECTION;
    if (timeout == NOWAIT) {
        if (rt_mbx_receive_if(Incoming, message, size) != size) {;
            result = SUCCESS;
        } else {    
            result = FAILED;
        }
    } else if (timeout == WAITFOREVER) {
        if (rt_mbx_receive(Incoming, message, size) == size) {
            result = SUCCESS;
        } else {
            result = FAILED;
        }
    } else {
        if (rt_mbx_receive_timed(Incoming, message, size, timeout) == size) {
            result = SUCCESS;
        } else {
            result = TIMEDOUT;
        }
    }
    return result;
}

#elif (CISST_OS == CISST_WINDOWS)
osaMailBox::ReturnType osaMailBox::Receive(void *message, int size, int timeout) {
    // in windows we just ignore the time out, anyway it is not a real-time system!
    DWORD msgSize, nMsg, sizeToRead, sizeRead;
    sizeToRead = (DWORD) size;
    OVERLAPPED ov;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
    if( NULL == hEvent )
        return FAILED;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;
    
    BOOL fResult = GetMailslotInfo(Incoming, // mailslot handle 
                                   (LPDWORD) NULL,               // no maximum message size 
                                   &msgSize,                   // size of next message 
                                   &nMsg,                    // number of messages 
                                   (LPDWORD) NULL);              // no read time-out 
    if (!fResult) { 
        CMN_LOG(1) << "Class osaMailBox: GetMailslotInfo failed with " << GetLastError() << std::endl;
        return FAILED; 
    } 
    
    if (msgSize == MAILSLOT_NO_MESSAGE) { 
        CMN_LOG(10) << "Class osaMailBox: There are no waiting messages." << std::endl; 
        return FAILED; 
    } 
    fResult = ReadFile(Incoming, 
                       message, 
                       sizeToRead, 
                       &sizeRead, 
                       &ov); 
    if (sizeToRead != sizeRead) {
        CMN_LOG(1) << "Class osaMailBox: ReadFile failed to write completely." << std::endl;
        return FAILED; 
    }
    if (!fResult) { 
        CMN_LOG(1) << "Class osaMailBox: ReadFile failed with " << GetLastError() << std::endl; 
        return SUCCESS; 
    } 
    return SUCCESS;
}


#else
osaMailBox::ReturnType osaMailBox::Receive(void *message, int size, int timeout) {
    CMN_LOG(7) << "Class osaMailBox: Receive" << std::endl;
    if (Incoming < 0) {
        CMN_LOG(5) << "Class osaMailBox: Receive failed, not connected." << std::endl;
        return NOCONNECTION;
    }
    if (read(Incoming, message, size) == -1) {
        int errorCode = errno;
        CMN_LOG(5) << "Class osaMailBox: error during read ("
                   << strerror(errorCode) << ")" << std::endl;
        return FAILED;
    }
    return SUCCESS;
}
#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaMailBox.cpp,v $
// Revision 1.8  2006/03/24 21:59:29  anton
// osaMailBox: Added preliminary version for unix (Linux, Darwin).  See #218.
//
// Revision 1.7  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.6  2006/03/02 22:10:27  pkaz
// cisstOSAbstraction:  added dummy implementations of osaThreadBuddy and osaMailbox for other operating systems
// (currently, implementations only exist for RTAI and Windows -- see #218).
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/05/03 15:37:02  anton
// cisstOSAbstraction: Cleanup some log (replaced printf, cout by CMN_LOG)
//
// Revision 1.3  2005/04/25 00:40:26  kapoor
// osaMailBox: Moved windows code for mailbox lying in my folders to main repository.
// This might change in future. Some limitations exists in using mailbox of windows,
// the main concern being the size of data transferred is limited to 454 bytes in windows.
//
// Revision 1.2  2005/04/24 16:46:30  kapoor
// osaMailBox: Connect & Addmailbox return bool flag to indicate success/failure.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.6  2004/04/08 06:09:11  kapoor
// Added win32 implementation
//
// Revision 1.5  2004/03/22 00:14:08  kapoor
// Reverted to using void * for sending messages.
//
// Revision 1.3  2004/03/04 21:26:03  kapoor
// more changes.
//
// Revision 1.2  2004/03/04 07:05:27  kapoor
// removed couts.
//
// Revision 1.1.1.1  2004/03/02 04:05:02  kapoor
// Importing source into local tree
//
// ****************************************************************************
