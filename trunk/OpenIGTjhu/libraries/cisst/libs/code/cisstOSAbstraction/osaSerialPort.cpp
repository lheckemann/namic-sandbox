/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaSerialPort.cpp,v 1.9 2006/05/11 03:33:47 anton Exp $

  Author(s): Anton Deguet
  Created on: 2004-12-10

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


#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstOSAbstraction/osaTime.h>

#if (CISST_OS == CISST_WINDOWS)

#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#endif



// implement services with a default log LoD of 3
CMN_IMPLEMENT_SERVICES(osaSerialPort);




std::string osaSerialPort::SetPortNumber(unsigned int portNumber) {
    std::stringstream portName;
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
    portName << "/dev/ttyS" << (portNumber - 1);
#endif
#if (CISST_OS == CISST_WINDOWS)
    portName << "COM" << portNumber;
#endif
    PortName = portName.str();
    CMN_LOG_CLASS(3) << "Port name set to " << PortName
                     << " based on port number " << portNumber << std::endl;
    return PortName;
}




#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Open(void) {
    CMN_LOG_CLASS(3) << "Start Open for port " << PortName << std::endl;
    // check that the port is not already opened
    if (IsOpenedFlag) {
        CMN_LOG_CLASS(1) << "Can not re-open an opened port " << PortName << std::endl;
        return false;
    }
    // create the port handle
    PortHandle = CreateFile(PortName.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0, // do not share access
                            0, // handle cannot be inherited
                            OPEN_EXISTING,
                            FILE_FLAG_OVERLAPPED,
                            NULL // always for serial port
                            );
    if (PortHandle == INVALID_HANDLE_VALUE) {
        CMN_LOG_CLASS(1) << "Error opening port (" << PortName << ")" << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS(3) << "Correct port handle for " << PortName << std::endl;
    }

    // create the overlapped events, remember to close them
    OverlappedStructureRead.hEvent = CreateEvent(NULL, true, false, NULL);
    if (OverlappedStructureRead.hEvent == NULL) {
        CMN_LOG_CLASS(1) << "Error creating overlapped read event handle for " << PortName << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS(3) << "Correct overlapped read event handle for " << PortName << std::endl;
    }
    OverlappedStructureWrite.hEvent = CreateEvent(NULL, true, false, NULL);
    if (OverlappedStructureWrite.hEvent == NULL) {
        CMN_LOG_CLASS(1) << "Error creating overlapped write event handle for " << PortName << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS(3) << "Correct overlapped write event handle for " << PortName << std::endl;
    }
    // configure using the current parameters (baud rate, etc.)
    IsOpenedFlag = true;
    IsOpenedFlag = Configure();
    return IsOpenedFlag;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
bool osaSerialPort::Open(void) {
    CMN_LOG_CLASS(3) << "Start Open for port " << PortName << std::endl;
    // check that the port is not already opened
    if (IsOpenedFlag) {
        CMN_LOG_CLASS(1) << "Can not re-open an opened port " << PortName << std::endl;
        return false;
    }
    // create the file descriptor
    FileDescriptor = open(PortName.c_str(), O_RDWR);
    if (FileDescriptor < 0) {
        CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Error opening port (" << PortName << ")" << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS(3) << "Correct file descriptor for port " << PortName << std::endl;
    }
    // configure using the current parameters (baud rate, etc.)
    IsOpenedFlag = true;
    IsOpenedFlag = Configure();
    return IsOpenedFlag;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Close(void)
{
    if (IsOpenedFlag) {
        // close event to avoid leak
        if (OverlappedStructureRead.hEvent != NULL) {
            CloseHandle(OverlappedStructureRead.hEvent);
        }
        if (OverlappedStructureWrite.hEvent != NULL) {
            CloseHandle(OverlappedStructureWrite.hEvent);
        }
        // close port handle
        CloseHandle(PortHandle);
        IsOpenedFlag = false;
        CMN_LOG_CLASS(3) << "Port " << PortName << " sucessfully closed." << std::endl;
    } else {
        CMN_LOG_CLASS(2) << "Attempt to close an already close port " << PortName << std::endl;
        return false;
    }
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
bool osaSerialPort::Close(void)
{
    if (IsOpenedFlag) {
        close(FileDescriptor);
        IsOpenedFlag = false;
        CMN_LOG_CLASS(3) << "Port " << PortName << " sucessfully closed." << std::endl;
    } else {
        CMN_LOG_CLASS(2) << "Attempt to close an already close port " << PortName << std::endl;
        return false;
    }
    return true;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Configure(void) { 
    
    CMN_LOG_CLASS(3) << "Start Configure for port " << PortName << std::endl;
    
  
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Can not Configure a closed port " << PortName << std::endl;
        return false;
    }
    
    DCB portOptions = {0};
    if (!GetCommState(PortHandle, &portOptions)) {
        CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Unable to retrieve current settings for " << PortName << std::endl;
        return false;
    } else {
        // set baud rate
        portOptions.BaudRate = this->BaudRate;
        
        // set character size
        portOptions.ByteSize = this->CharacterSize;

        // set parity
        if (this->ParityChecking == ParityCheckingNone) {
            portOptions.fParity = false;
        } else {
            portOptions.fParity = true;
        }
        portOptions.Parity = this->ParityChecking;

        // set number of stop bits
        if (this->TwoStopBits) {
            portOptions.StopBits = TWOSTOPBITS;
        } else {
            portOptions.StopBits = ONESTOPBIT;
        }

        // try to apply these settings
        if (!SetCommState(PortHandle, &portOptions)) {
            CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Unable to apply current settings for " << PortName << std::endl;
            return false;
        }
        
        // set up for overlapped I/O
        TimeOuts.ReadIntervalTimeout = MAXDWORD ;
        TimeOuts.ReadTotalTimeoutMultiplier = 0;
        TimeOuts.ReadTotalTimeoutConstant = 0;
        TimeOuts.WriteTotalTimeoutMultiplier = 0;
        SetCommTimeouts(PortHandle, &TimeOuts);

        // get any early notifications
        SetCommMask(PortHandle, EV_RXCHAR);
  
        // set comm buffer sizes
        SetupComm(PortHandle, 2048, 1024);

        // purge
        PurgeComm(PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    }

    CMN_LOG_CLASS(3) << "End of Configure for port " << PortName << std::endl;
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
bool osaSerialPort::Configure(void) { 
    
    CMN_LOG_CLASS(3) << "Start Configure for port " << PortName << std::endl;

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Can not Configure a closed port " << PortName << std::endl;
        return false;
    }

    // read current termio settings  
    struct termios portOptions;
    tcgetattr(FileDescriptor, &portOptions);

    // set input and output speed
    cfsetispeed(&portOptions, this->BaudRate);
    cfsetospeed(&portOptions, this->BaudRate);
    
    // set character size  
    portOptions.c_cflag &= ~CSIZE;
    portOptions.c_cflag |= this->CharacterSize; // 5, 6, 7 or 8 data bits

    // set parity checking
    switch (this->ParityChecking) {
    case ParityCheckingNone:
        portOptions.c_cflag &= ~(PARENB); // disable parity
        portOptions.c_iflag &= ~(INPCK); // disable input parity check
        break;
    case ParityCheckingOdd:
        portOptions.c_cflag |= PARODD;
        portOptions.c_cflag |= PARENB;
        portOptions.c_iflag |= INPCK; // enable input parity check
        break;
    case ParityCheckingEven:
        portOptions.c_cflag |= PARENB;
        portOptions.c_cflag &= ~PARODD;
        portOptions.c_iflag |= INPCK; // enable input parity check
        break;
    default:
        CMN_LOG_CLASS(1) << CMN_LOG_DETAILS << "Fatal error on port " << PortName << std::endl;
    }

    // set stop bit to 1 or 2
    if (this->TwoStopBits) {
        portOptions.c_cflag |= CSTOPB; // 2 stop bits
    } else {
        portOptions.c_cflag &= ~CSTOPB; // 1 stop bit  
    }

    // defaults we don't modify
    portOptions.c_cflag |= CLOCAL; // modem control off  
    portOptions.c_cflag |= CREAD; // enable reads from port  
    portOptions.c_lflag &= ~ICANON; // turn off line by line mode  
    portOptions.c_lflag &= ~ECHO; // no echo of TX characters  
    portOptions.c_lflag &= ~ISIG; // no input ctrl char checking  
    portOptions.c_iflag &= ~ICRNL; // do not map CR to NL on in  
    portOptions.c_oflag &= ~OCRNL; // do not map CR to NL on out  
    portOptions.c_oflag &= ~OPOST; // no output ctrl char checking 
    portOptions.c_iflag &= ~IGNCR; // allow CR characters 
    // set minimum number of characters read() returns on  
    // set read delay, time = VTIME x 100 mS  
    portOptions.c_cc[VMIN] = 0; // read can return with no chars  
    portOptions.c_cc[VTIME] = 1; // read waits this much for chars 

    // apply changes
    tcsetattr(FileDescriptor, TCSADRAIN, &portOptions);
    CMN_LOG_CLASS(3) << "End of Configure for port " << PortName << std::endl;
    return true;
}
#endif






osaSerialPort::~osaSerialPort(void)
{
    if (IsOpenedFlag) {
        this->Close();
    }
}






#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Write(char* data, int nBytes)
{
    DWORD numBytes = 0;
    bool sent = false; // by default, assume it will fail

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    // Issue write.
    if (!WriteFile(PortHandle, data, nBytes, &numBytes, &OverlappedStructureWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            CMN_LOG_CLASS(5) << "WriteFile failed (not delayed) for " << PortName << std::endl;
        } else {
            CMN_LOG_CLASS(7) << "Write is pending for " << PortName << std::endl;
            if (!GetOverlappedResult(PortHandle, &OverlappedStructureWrite, &numBytes, TRUE)) {
                CMN_LOG_CLASS(5) << "GetOverlappedResult failed for Write on port " << PortName << std::endl;
            } else {
                CMN_LOG_CLASS(7) << "Write operation completed successfully for " << PortName << std::endl;
                sent = true;
            }
        }
    } else {
        CMN_LOG_CLASS(7) << "Immediate write operation completed successfully for " << PortName << std::endl;
        sent = true;
    }

    return sent?numBytes:0;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
int osaSerialPort::Write(char* data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = write(FileDescriptor, data, nBytes);
    CMN_LOG_CLASS(8) << "Wrote " << data << std::endl
                     << "(" << nBytes << " bytes) on port " << PortName << std::endl;
    return numBytes;
}
#endif



#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Write(unsigned char* data, int nBytes)
{
    DWORD numBytes = 0;
    bool sent = false; // by default, assume it will fail

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    // Issue write.
    if (!WriteFile(PortHandle, data, nBytes, &numBytes, &OverlappedStructureWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            CMN_LOG_CLASS(5) << "WriteFile failed (not delayed) for " << PortName << std::endl;
        } else {
            CMN_LOG_CLASS(7) << "Write is pending for " << PortName << std::endl;
            if (!GetOverlappedResult(PortHandle, &OverlappedStructureWrite, &numBytes, TRUE)) {
                CMN_LOG_CLASS(5) << "GetOverlappedResult failed for Write on port " << PortName << std::endl;
            } else {
                CMN_LOG_CLASS(7) << "Write operation completed successfully for " << PortName << std::endl;
                sent = true;
            }
        }
    } else {
        CMN_LOG_CLASS(7) << "Immediate write operation completed successfully for " << PortName << std::endl;
        sent = true;
    }

    return sent?numBytes:0;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
int osaSerialPort::Write(unsigned char* data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = write(FileDescriptor, data, nBytes);
    CMN_LOG_CLASS(8) << "Wrote " << data << std::endl
                     << "(" << nBytes << " bytes) on port " << PortName << std::endl;
    return numBytes;
}
#endif






#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Read(char* data, int nBytes)
{
    BOOL fReadStat;
    COMSTAT ComStat;
    DWORD dwErrorFlags, dwLength, dwError;

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    // only try to read number of bytes in queue 
    ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
    
    dwLength = std::min((DWORD) nBytes, ComStat.cbInQue);

    if (dwLength > 0) {
        fReadStat = ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead);
        if (!fReadStat) {
            if (GetLastError() == ERROR_IO_PENDING) {
                CMN_LOG_CLASS(6) << "IO Pending in Read" << std::endl;
                // We have to wait for read to complete.
                // Read is right now set up to return nothing if the port
                // is empty, the while loop is ineffective
                while (!GetOverlappedResult(PortHandle, &OverlappedStructureRead, &dwLength, true)) {
                    dwError = GetLastError();
                    if (dwError == ERROR_IO_INCOMPLETE) {
                        continue;
                    } else {
                        // an error occurred, try to recover
                        ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
                        CMN_LOG_CLASS(5) << "I/O error occured in read" << std::endl;
                        break;
                    } // else
                } // while
            } else {
                // some other error occurred
                dwLength = -1;
                ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
            } // else
        } // if freadstat
    } // if dwlength
    return dwLength;
}
#endif


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
int osaSerialPort::Read(char* data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = read(FileDescriptor, data, nBytes); // get chars if there

    // this is only to log a message of level 8
    const short lod = 8;
    char* message;
    // create a message only if this is required.  based on CMN_LOG_CLASS code
    if ((lod > cmnLogger::GetLoD()) || (lod > Services()->GetLoD())) {
        message = (char*) malloc(sizeof(char) * (numBytes + 2));
        memcpy(message, data, numBytes);
        message[numBytes] = '\0';
        CMN_LOG_CLASS(lod) << "Read " << message << std::endl
                           << "(" << numBytes << " bytes)" << std::endl;
    } else {
        CMN_LOG_CLASS(lod) << "Nothing to read" << std::endl;
    }
    return numBytes;
}
#endif


#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Read(unsigned char* data, int nBytes)
{
    BOOL fReadStat;
    COMSTAT ComStat;
    DWORD dwErrorFlags, dwLength, dwError;

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    // only try to read number of bytes in queue 
    ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
    
    dwLength = std::min((DWORD) nBytes, ComStat.cbInQue);

    if (dwLength > 0) {
        fReadStat = ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead);
        if (!fReadStat) {
            if (GetLastError() == ERROR_IO_PENDING) {
                CMN_LOG_CLASS(6) << "IO Pending in Read" << std::endl;
                // We have to wait for read to complete.
                // Read is right now set up to return nothing if the port
                // is empty, the while loop is ineffective
                while (!GetOverlappedResult(PortHandle, &OverlappedStructureRead, &dwLength, true)) {
                    dwError = GetLastError();
                    if (dwError == ERROR_IO_INCOMPLETE) {
                        continue;
                    } else {
                        // an error occurred, try to recover
                        ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
                        CMN_LOG_CLASS(5) << "I/O error occured in read" << std::endl;
                        break;
                    } // else
                } // while
            } else {
                // some other error occurred
                dwLength = -1;
                ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
            } // else
        } // if freadstat
    } // if dwlength
    return dwLength;
}
#endif


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
int osaSerialPort::Read(unsigned char* data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = read(FileDescriptor, data, nBytes); // get chars if there

    // this is only to log a message of level 8
    const short lod = 8;
    char* message;
    // create a message only if this is required.  based on CMN_LOG_CLASS code
    if ((lod > cmnLogger::GetLoD()) || (lod > Services()->GetLoD())) {
        message = (char*) malloc(sizeof(char) * (numBytes + 2));
        memcpy(message, data, numBytes);
        message[numBytes] = '\0';
        CMN_LOG_CLASS(lod) << "Read " << message << std::endl
                           << "(" << numBytes << " bytes)" << std::endl;
    } else {
        CMN_LOG_CLASS(lod) << "Nothing to read" << std::endl;
    }
    return numBytes;
}
#endif



#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::WriteBreak(unsigned int breakLength)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not WriteBreak on a closed port " << PortName << std::endl;
        return false;
    }
    if (!SetCommBreak(PortHandle)) {
        CMN_LOG_CLASS(5) << "Unable to set break on port  " << PortName << std::endl;        
        return false;
    }
    osaTime::Sleep(breakLength);
    if (!ClearCommBreak(PortHandle)) {
        CMN_LOG_CLASS(5) << "Unable to clear break on port  " << PortName << std::endl;        
        return false;
    }
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
bool osaSerialPort::WriteBreak(unsigned int breakLength)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not WriteBreak on a closed port " << PortName << std::endl;
        return false;
    }
    if (tcsendbreak(FileDescriptor, breakLength) < 0) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Serial break failed" << std::endl;
        return false;
    }
    CMN_LOG_CLASS(7) << "Wrote a serial break on port " << PortName << std::endl;
    return true;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Flush(void)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Flush a closed port " << PortName << std::endl;
        return false;
    }
    if (!FlushFileBuffers(PortHandle)) {
        CMN_LOG_CLASS(5) << "Unable to flush port  " << PortName << std::endl;        
        return false;
    }
    CMN_LOG_CLASS(7) << "Wrote a serial break on port " << PortName << std::endl;
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN)
bool osaSerialPort::Flush(void)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS(5) << CMN_LOG_DETAILS << "Can not Flush a closed port " << PortName << std::endl;
        return false;
    }
    tcflush(FileDescriptor, TCIOFLUSH);
    return true;
}
#endif



// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaSerialPort.cpp,v $
// Revision 1.9  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.8  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.7  2005/11/29 14:35:57  anton
// osaSerialPort.cpp: Missing log in send serial break on Windows.
//
// Revision 1.6  2005/11/24 02:32:38  anton
// cisstOSAbstraction: Added compilation flags for Darwin (see #194).
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/06/24 20:57:16  alamora
// overloaded Read and Write for unsigned char* (required by the Munich
// DaVinci EE controller)
//
// Revision 1.3  2005/06/17 21:28:12  anton
// osaSerialPort: Added missing CISST_OS == CISST_LINUX_RTAI
//
// Revision 1.2  2005/05/03 15:35:16  anton
// osaSerialPort: Somewhat working version for Linux and Windows.  Add patch
// for ticket #142 (not using RTAI serial port, but at least compiles with RTAI)
//
// Revision 1.1  2005/01/03 21:28:00  anton
// cisstOSAbstraction: Added preliminary version of osaSerialPort.  The API
// will change, this is undocumented and not tested.  DO NOT USE.
//
//
// ****************************************************************************
