/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnLogger.h,v 1.10 2006/01/03 03:33:04 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2004-08-31

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
  \brief Declaration of cmnLogger amd macros for logging
  \ingroup cisstCommon
*/

#ifndef _cmnLogger_h
#define _cmnLogger_h

#include <string>
#include <vector>
#include <fstream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLODOutputMultiplexer.h>
#include <cisstCommon/cmnMultiplexerStreambuf.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>
#include <cisstCommon/cmnExport.h>




/*! This macro is used to log human readable information within a
  registered class.  It can only be used within the scope of a method
  part of a class registered in the cmnClassRegister using the macros
  #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h.  For a non registered class of a global
  function, please use #CMN_LOG.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that two conditions are met:

  - The level of detail associated to the message must be lower or
    equal to the level of detail used to filter the messages at the
    global level.  This global level of detail is set via the
    cmnLogger.

  - The level of detail associated to the message must be lower or
    equal to the level of detail used to filter the messages by the
    class itself.  To modify the class specific level of detail, see
    cmnClassServicesBase and cmnClassRegister

  The macro creates an output stream if the level of detail of the
  message satisfies the two criteria defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG_CLASS(4) << "This is a message of LoD 4" << this->x << std::endl;
  \endcode

  \param lod The log level of detail of the message.
*/
#define CMN_LOG_CLASS(lod) \
    ((lod > cmnLogger::GetLoD()) || (lod > Services()->GetLoD()))?\
        (void*)0:\
        (cmnLODOutputMultiplexer(cmnLogger::GetMultiplexer(), lod).Ref()) << "Class " << Services()->GetName() << ": "
 


/*! This macro is used to log human readable information within the
  scope of a global function (e.g. main()).  It can also be used in
  classes which are not registered in cmnClassRegister (see also
  macros #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES declared in
  cmnClassRegister.h).  For a registered class, please use
  #CMN_LOG_CLASS.

  The message is streamed along with a Level of Detail to cmnLogger
  provided that the level of detail associated to the message is lower
  or equal to the level of detail used to filter the messages at the
  global level.  This global level of detail is set via the cmnLogger.

  The macro creates an output stream if the level of detail of the
  message satisfies the criterion defined above.  It can be used as
  any other output stream:

  \code
  CMN_LOG(1) << "This is a message of LoD 1" << argc << std::endl;
  \endcode

  \param lod The log level of detail of the message.
*/
#define CMN_LOG(lod) \
    (lod > cmnLogger::GetLoD())? \
    (void*)0: \
    (cmnLODOutputMultiplexer(cmnLogger::GetMultiplexer(), lod).Ref())




/*! This macro is used to add useful information to Log macros
  (#CMN_LOG_CLASS and #CMN_LOG). It provides the additional
  functionality of logging the file and line number at which the
  message was generated.

  For example:
  \code
  CMN_LOG(1) << CMN_LOG_DETAILS << "Error occurs" << std::endl;
  \endcode

  For any error with a very common message, this helps the programmer
  to quickly find the origin of the message in the source code.
*/
#define CMN_LOG_DETAILS \
    "File: " << __FILE__ << " Line: " << __LINE__ << " - "


#define CMN_LOG_DEFAULT_LOD 5


/*!  \brief Class to glue the class register, the output multiplexer
  and the message level of detail to form the logging entity of cisst.

  \ingroup cisstCommon

  cmnLogger is defined as a singleton, i.e. there is only one instance
  of cmnLogger.  This unique object maintains the data required for
  the cisst logging system.  The desing of the logging system
  relies on:

  - Human readable messages are associated to a Level of Detail.

  - The messages are sent and filtered based on their level of detail.
    The macros used to send the messages actually check if the message
    is to be sent or not before creating them in order to improve
    performances (see #CMN_LOG_CLASS and #CMN_LOG).

  - The messages are sent to a multiplexer owned by cmnLogger.  This
    multiplexer allows to send the message (along with their level of
    detail) to multiple output streams.

  To filter the messages, the user can use:

  - The global level of detail defined in cmnLogger.  Any message with
    a level of detail greater than the global level of detail will be
    ignored (at minimal execution time cost).  This level of detail is
    used by #CMN_LOG_CLASS and #CMN_LOG.  To modify the global level
    of detail, use cmnLogger::SetLoD(newLoD).

  - Each class defines a level of detail.  This allows to tune the log
    based on the user's needs.  One can for example allow all the
    messages (from errors to warnings) from a given class and block
    all the messages from every other class.  As for the global level
    of detail, if a message's level of detail is greater than the
    class level of detail, it will be ignored (at minimal execution
    time cost).  This level of detail is used only be #CMN_LOG_CLASS.
    To modify a class level of detail, use either the class register
    with cmnClassRegister::SetLoD("className", newLoD) or the class
    services with object.Services()->SetLoD(newLoD).

  - The output streams level of details.  Each output stream has its
    own level of detail and will stream only the messages with a level
    of details lesser or equal to his.  This allows for example to log
    everything to a file while printing only the high priority ones to
    std::cout.  To set the level of detail of an output stream, use
    cmnLogger::GetMultiplexer()->AddChannel(newStream, newLoD).

  \sa cmnClassRegister cmnClassServicesBase cmnLODOutputMultiplexer 
*/
class CISST_EXPORT cmnLogger {

 public:
    /*! Type used to define the logging level of detail. */
    typedef short LoDType;

 private:
    /*! Global Level of Detail used to filter all messages.

      As for the signification of the different level of details, they
      depend on each class implementation.  Nevertheless, it's
      recommended to use some values between 0 and 10, 10 being the
      highest level of detail which corresponds to the maximum amount
      of logging.  Also, for the programmers of new classes, we
      recommend the following levels:

      - 1: Errors during the initialization.
      - 2: Warnings during the initialization.
      - 3 and 4: Extra messages during the initialization.
      - 5: Errors during normal operations.
      - 6: Warnings during normal operations.
      - 7 and above: Verbose to very verbose.

      The idea is that for most classes, important errors happens
      during the initialization (constructor, opening a serial port,
      configuring some hardware device, open a grapical context, etc.)
      and during the normal operations, time can become critical.
      Therefore a level 5 would log a lot of information at the
      beginning and only the critical messages during the normal
      operations.
    */
    LoDType LoD;

    /*! Default output file used to log */
    // std::ofstream DefaultLogFile;
    
    /*! Single multiplexer used to stream the log out */    
    cmnLODMultiplexerStreambuf<char> LoDMultiplexerStreambuf;

    /*! Instance specific implementation of SetLoD.  \sa SetLoD */
    inline void SetLoDInstance(LoDType lod) {
        CMN_LOG(2) << "Class cmnLogger: Level of Detail set to " << lod << std::endl;
        LoD = lod;
    }

    /*! Instance specific implementation of GetLoD.  \sa GetLoD */
    inline LoDType GetLoDInstance(void) const {
        return LoD;
    }

    /*! Instance specific implementation of GetMultiplexer.
      \sa GetMultiplexer */
    inline cmnLODMultiplexerStreambuf<char>* GetMultiplexerInstance(void) {
        return &(LoDMultiplexerStreambuf);
    }

    /*! Create and get a pointer on the default log file. */
    std::ofstream* DefaultLogFile(char * defaultLogFileName = "cisstLog.txt");

    /*! Instance specific implementation of HaltDefaultLog. */
    inline void HaltDefaultLogInstance(void) {
  LoDMultiplexerStreambuf.RemoveChannel(*(DefaultLogFile()));
    }

    /*! Instance specific implementation of ResumeDefaultLog. */
    inline void ResumeDefaultLogInstance(LoDType newLoD = CMN_LOG_DEFAULT_LOD) {
  LoDMultiplexerStreambuf.AddChannel(*(DefaultLogFile()), newLoD);
    }


 protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the class register is a singleton. */
    cmnLogger(char* defaultLogFileName = "cisstLog.txt");

 public:
    /*! The log is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated log is created at the first call of
      this method since it is a static variable declared in this
      method's scope. 

      \return A pointer to the log. */
    static cmnLogger* Instance(void);


    /*! Set the global Level of Detail to filter the log messages.
     
      \param lod The global level of detail used to filter the log.

      \sa SetLoDInstance */
    static void SetLoD(LoDType lod) {
        Instance()->SetLoDInstance(lod);
    }

    /*! Get the global Level of Detail used to filter the log
      messages.
      
      \return The global level of detail used to filter the log.
      
      \sa GetLoDInstance */
    static LoDType GetLoD(void) {
        return Instance()->GetLoDInstance();
    }

    /*! Returns the cmnLODMultiplexerStreambuf directly. This allows
      manipulation of the streambuffer for operations such as adding or
      deleting channels for the stream..
   
      \return cmnLODMultiplexerStreambuf<char>* The Streambuffer.

      \sa GetMultiplexerInstance
    */
    static cmnLODMultiplexerStreambuf<char>* GetMultiplexer(void) {
        return Instance()->GetMultiplexerInstance();
    }
 

    /*! Disable the default log file "cisstLog.txt".  This method
      removes the default log from the output list of the multiplexer
      but doesn't close the default log file. */ 
    static void HaltDefaultLog(void) {
  Instance()->HaltDefaultLogInstance();
    }

    
    /*! Resume the default log file.  By default, the log is enabled
      (this is the default behavior of the cmnLogger constructor) but
      this can be halted by using HaltDefaultLog().  Using
      ResumeDefaultLog() allows to resume the log to "cisstLog.txt"
      without losing previous logs. */
    static void ResumeDefaultLog(LoDType newLoD = CMN_LOG_DEFAULT_LOD) {
  Instance()->ResumeDefaultLogInstance(newLoD);
    }

};


#endif // _cmnLogger_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnLogger.h,v $
// Revision 1.10  2006/01/03 03:33:04  anton
// cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
// cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
// Revision 1.9  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/07/01 04:00:03  anton
// cisstCommon: Removed macro CMN_LOG_HACK and class cmnOutputInhibitor as
// specified in tickets #67 and #146.
//
// Revision 1.7  2005/06/03 18:23:28  anton
// cmnLogger.h: Added missing "inline" to solve multiple implementation issues.
//
// Revision 1.6  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.5  2005/02/11 19:50:44  anton
// cmnLogger: Added two methods to halt/resume the log to the default file.
//
// Revision 1.4  2004/10/08 21:55:58  ofri
// Added method cmnLODOutputMultiplexer::Ref() and used it to replace the use
// of CMN_LOG_HACK in cmnLogger.h .  The correctness of this solution was tested
// on gcc and .net2003, using the helloWorld example.  See #67
//
// Revision 1.3  2004/10/06 19:34:24  anton
// cmnLogger: Curiously enough, it seems that .net 2003 requires what we used
// to call a hack.  Since 3 our of 4 compilers require this, it might not be
// a hach after all.
//
// Revision 1.2  2004/09/03 19:32:03  anton
// cisstCommon: Documentation of the new log and class registration system,
// see check-in [718]
//
// Revision 1.1  2004/09/01 21:12:02  anton
// cisstCommon: Major update of the class register and the logging system.
// The class register is now a singleton (as in "Design Patterns") and doesn't
// store any information related to the logging (i.e. global lod, multiplexer
// for the output).  The data related to the log is now regrouped in the
// singleton cmnLogger.  This code is still fairly experimental (but tested)
// and the documentation is missing.  These changes should solve the tickets
// #30, #38 and #46.
//
//
// ****************************************************************************
