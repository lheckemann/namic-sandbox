/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnOutputMultiplexer.h,v 1.10 2006/01/03 03:33:04 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on:  4/18/2002

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
  \brief Declaration of cmnOutputMultiplexer class. 
 */

#ifndef _cmnOutputMultiplexer_h
#define _cmnOutputMultiplexer_h

#include <list>
#include <iostream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>
#include <cisstCommon/cmnExport.h> 


/*!
  \brief Creates a collection of objects that channels output to multiple sinks.

  Types for dynamic control of output messages. This includes
  debugging information, error reporting, state logging etc.  This
  file declares *class cmnOutputMultiplexer*. a cmnOutputMultiplexer
  inherits the public interface of a generic ostream, and channels the
  output to multiple sinks.
 
 
  Usage:
     Include the module in your application with:
     \#include "cmnOutputMultiplexer.h"
     
     Create a collection of ostream objects, and attach them to a cmnOutputMultiplexer.
     Use the standrard ostream syntax (operator <<) to stream objects to the multiplexer.

     \code
       ofstream log("logfile.txt");
       windowoutputstream display;  // hypothetical class
 
       cmnOutputMultiplexer multiplexer;
       multiplexer.AddChannel(&log);
       multiplexer.AddChannel(&windowoutputstream);
 
       multiplexer << "Hello, world" << endl;  // channel the message ot all streams.
     \endcode

     Notes: 
     -# cmnOutputMultiplexer does not OWN the output streams. They are created
     and destroyed externally.
     -# The initial implementation uses a list to store the addresses of the output channels.
     There is no guarantee on the order of storage or the order of output channelling.
     -# It is assumed that none of the output channels modifies the data arguments.
     -# The multiplexer does not buffer any output. There is no implementation for seekp()
        and tellp().
     -# It is guaranteed that at most one instance of an ostream object is stored as a channel
        in a single multiplexer. The AddChannel() function checks for uniqueness.
*/
class CISST_EXPORT cmnOutputMultiplexer : public std::ostream
{
 public:
    typedef char char_type;
    typedef std::ostream ChannelType;
  
    /*! Type of internal data structure storing the channels. */
    typedef std::list<ChannelType *> ChannelContainerType;
  
    /*! Default constructor - initialize base class. */ 
    cmnOutputMultiplexer() : ChannelType(&m_Streambuf) {}
  
    /*! Add an output channel. See notes above.
      \param channel A pointer to the output channel to be added.
     */
    ChannelType & AddChannel(ChannelType * channel);
  
    /*! Remove an output channel.
      \param channel A pointer to the output channel to be removed. No change occurs if
      the pointer is not on the list of channels for this multiplexer.
      \return channel Returns *this, The output channel.
     */
    ChannelType & RemoveChannel(ChannelType * channel);
  

    /*! Enable access to the channel storage, without addition or removal of channels.
      Elements of the container can be accessed using the standard const_iterator
      interfaces.  Note that the channels themselves are non-const, so individual
      manipulation of each is enabled.
     */
    const ChannelContainerType & GetChannels() const {
        return m_ChannelContainer;
    }
  
 protected:
    ChannelContainerType m_ChannelContainer;
  
    cmnLODMultiplexerStreambuf<char_type> m_Streambuf;
};


#endif // _cmnOutputMultiplexer_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnOutputMultiplexer.h,v $
// Revision 1.10  2006/01/03 03:33:04  anton
// cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
// cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
// Revision 1.9  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/04/06 21:55:48  alamora
// CISST_EXPORT symbols for listing contents in shared library (IRE)
//
// Revision 1.5  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.4  2004/10/09 18:52:16  ofri
// Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
// warnings when it encounters #pragma warning .  Some files had that prama used
// if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
// including cmnPortability and adding the warning disable that.
//
// Revision 1.3  2003/09/09 18:53:46  anton
// changed from double quotes to <> for all #include
//
// Revision 1.2  2003/06/23 18:48:12  anton
// remove tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:59  anton
// no message
//
//
// ****************************************************************************


