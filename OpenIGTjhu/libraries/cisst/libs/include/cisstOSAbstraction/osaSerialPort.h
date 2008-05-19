/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaSerialPort.h,v 1.10 2006/05/11 03:33:47 anton Exp $

  Author(s): Anton Deguet
  Created on: 2004-12-06

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
  \brief Declaration of osaSerialPort
*/

#ifndef _osaSerialPort_h
#define _osaSerialPort_h


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstOSAbstraction/osaExport.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#else
#include <termios.h>
#endif

#include <sstream>


/*!
  \ingroup cisstOSAbstraction
  \brief Serial port.

  Create a serial port for sending and receiving data.
 */
class CISST_EXPORT osaSerialPort: public cmnGenericObject
{
    /*! Services for osaSerialPort.  This allows to register the class
      and control the level of detail for the log.

      \sa #CMN_DECLARE_SERVICES. */
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    /*! Type used to define the baud rate.
      \sa SetBaudRate */
#if (CISST_OS == CISST_WINDOWS)
    enum BaudRateType {BaudRate9600 = CBR_9600,
                       BaudRate19200 = CBR_19200,
                       BaudRate38400 = CBR_38400,
                       BaudRate57600 = CBR_57600,
                       BaudRate115200 = CBR_115200};
#else
    enum BaudRateType {BaudRate9600 = B9600,
                       BaudRate19200 = B19200,
                       BaudRate38400 = B38400,
                       BaudRate57600 = B57600,
                       BaudRate115200 = B115200};
#endif


    /*! Type used to define the character size.
      \sa SetCharacterSize */
#if (CISST_OS == CISST_WINDOWS)
    enum CharacterSizeType {CharacterSize5 = 5,
                            CharacterSize6 = 6,
                            CharacterSize7 = 7,
                            CharacterSize8 = 8};
#else
    enum CharacterSizeType {CharacterSize5 = CS5,
                            CharacterSize6 = CS6,
                            CharacterSize7 = CS7,
                            CharacterSize8 = CS8};
#endif


    /*! Type used to define the parity checking.
      \sa SetParityChecking */
#if (CISST_OS == CISST_WINDOWS)
    enum ParityCheckingType {ParityCheckingNone = NOPARITY,
                             ParityCheckingEven = EVENPARITY,
                             ParityCheckingOdd = ODDPARITY};
#else
    enum ParityCheckingType {ParityCheckingNone,
                             ParityCheckingEven,
                             ParityCheckingOdd};
#endif


    /*! Default constructor.  Set parameters to default 8N1 (8 bits
      per character, no parity checking, 1 stop bit) and 9600 baud
      rate.  This constructor doesn't start anything, use Open() to
      actually start the serial port connection. */
    osaSerialPort(void):
        PortName("Undefined"),
        IsOpenedFlag(false),
        BaudRate(BaudRate9600),
        CharacterSize(CharacterSize8),
        ParityChecking(ParityCheckingNone),
        TwoStopBits(false),
        HardwareFlowControl(false)
    {
#if (CISST_OS == CISST_WINDOWS)
        memset(&OverlappedStructureRead, 0, sizeof(OverlappedStructureRead));
        memset(&OverlappedStructureWrite, 0, sizeof(OverlappedStructureWrite));
#else // Unix
        FileDescriptor = -1;
#endif
    }

  
    /*! Destructor. */
    virtual ~osaSerialPort(void);
    
    /*! Set a different baud rate. */
    inline void SetBaudRate(const BaudRateType & baudRate) {
        BaudRate = baudRate;
        CMN_LOG_CLASS(3) << "Baud rate modified to " << BaudRate
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set a different character size. */
    inline void SetCharacterSize(const CharacterSizeType & characterSize) {
        CharacterSize = characterSize;
        CMN_LOG_CLASS(3) << "Character size modified to " << CharacterSize
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set a different parity checking. */
    inline void SetParityChecking(const ParityCheckingType & parityChecking) {
        ParityChecking = parityChecking;
        CMN_LOG_CLASS(3) << "Parity checking modified to " << ParityChecking
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Use two stop bits.  By default, one stop bit is used. */
    inline void SetTwoStopBits(const bool & twoStopBits) {
        TwoStopBits = twoStopBits;
        CMN_LOG_CLASS(3) << "Use two stop bits modified to " << TwoStopBits
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Activate/de-activate the hardware flow control. */
    inline void SetHardwareFlowControl(const bool & hardwareFlowControl) {
        HardwareFlowControl = hardwareFlowControl;
        CMN_LOG_CLASS(3) << "Hardware flow control modified to " << HardwareFlowControl
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set the port name.  For a more portable code, use SetPortNumber. */
    inline void SetPortName(const std::string & portName) {
        PortName = portName;
    }

    /*! Get the current port name. */
    inline std::string GetPortName(void) const {
        return PortName;
    }

    /*! Set the serial port name based on a number, starting from 1. */
    std::string SetPortNumber(unsigned int portNumber);

    /*! Open the serial port.  This method starts the serial port
      based on the parameters previously set (either the defaults of
      those defined by SetBaudRate, SetCharacterSize,
      SetParityChecking, SetHardwareFlowControl, ...). */
    bool Open(void);

    bool Configure(void);

    bool Close(void);

    /*! Indicates if the port has been opened or closed correctly. */
    inline bool IsOpened(void) const {
        return IsOpenedFlag;
    }

    /*! Send raw data. */
    int Write(char* data, int nBytes);
    int Write(unsigned char* data, int nBytes);
    
    /*! Receive raw data, non blocking. */
    int Read(char* data, int nBytes);
    int Read(unsigned char* data, int nBytes);
  
    /*! Sends a serial break. */
    bool WriteBreak(unsigned breakLength);  
  
    /*! Flush. */
    bool Flush(void);

private:
    // full port name
    std::string PortName;
    bool IsOpenedFlag;

    // parameters
    BaudRateType BaudRate;
    CharacterSizeType CharacterSize;
    ParityCheckingType ParityChecking;
    bool TwoStopBits;
    bool HardwareFlowControl;
    
#if (CISST_OS == CISST_WINDOWS)
    HANDLE PortHandle;
    OVERLAPPED OverlappedStructureRead, OverlappedStructureWrite;
    COMMTIMEOUTS TimeOuts;
#else // Unix
    int FileDescriptor;
#endif  
};


CMN_DECLARE_SERVICES_INSTANTIATION(osaSerialPort);


#endif // _osaSerialPort_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaSerialPort.h,v $
// Revision 1.10  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.9  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.8  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/06/24 20:56:44  alamora
// overloaded Read and Write for unsigned char* (required by the Munich
// DaVinci EE controller)
//
// Revision 1.6  2005/06/20 21:11:31  anton
// osaSerialPort: Added IsOpened() method.
//
// Revision 1.5  2005/05/03 15:35:16  anton
// osaSerialPort: Somewhat working version for Linux and Windows.  Add patch
// for ticket #142 (not using RTAI serial port, but at least compiles with RTAI)
//
// Revision 1.4  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.3  2005/02/03 19:49:22  anton
// osaSerialPort: Doxygen bug, \file used twice.
//
// Revision 1.2  2005/01/10 19:21:53  anton
// cisstOSAbastraction: Added osaExport.h and required CISST_EXPORT to compile
// as a DLL (for windows).
//
// Revision 1.1  2005/01/03 21:28:00  anton
// cisstOSAbstraction: Added preliminary version of osaSerialPort.  The API
// will change, this is undocumented and not tested.  DO NOT USE.
//
//
// ****************************************************************************
