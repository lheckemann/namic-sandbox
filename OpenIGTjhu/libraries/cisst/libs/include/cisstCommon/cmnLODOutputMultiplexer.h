/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnLODOutputMultiplexer.h,v 1.10 2006/01/03 03:33:04 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on: 5/20/2002

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


/*! \file 
  \brief Types for dynamic control of output messages.
*/

#ifndef _cmnLODOutputMultiplexer_h
#define _cmnLODOutputMultiplexer_h

#include <list>
#include <iostream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnMultiplexerStreambufProxy.h>

/*! \brief  Types for dynamic control of output messages. 

   This includes debugging information, error reporting, state logging etc.
   This file declares class cmnLODOutputMultiplexer. a cmnLODOutputMultiplexer
   inherits the public interface of a generic ostream, and attaches each output
   message with a Level Of Detail (LOD) descriptor.  The LOD descriptor is
   forwarded through a cmnMultiplexerStreambufProxy to a cmnLODMultiplexerStreambuf
   that peforms the actual multiplexing.
 
   Create a collection of ostream objects, and attach them to a
   cmnLODMultiplexerStreambuf.
   Create a cmnLODOutputMultiplexer and assign it with the LOD value and
   the cmnLODMultiplexerStreambuf address.
   Use the standrard ostream syntax (operator <<) to stream objects to the multiplexer.
 
   Example (assume that the object lodMultiplexerStreambuf is
   a cmnLODMultiplexerStreambuf):
 
     \code
       // The multiple output channels
       ofstream log("logfile.txt");
       windowoutputstream display;  // hypothetical class
 
       cmnLODOutputMultiplexer multiplexerOutput(&lodMultiplexetStreambuf, 3);
 
       lodMultiplexerStreambuf.AddChannel(log.rdbuf(), 5);
       lodMultiplexerStreambuf.AddChannel(windowoutputstream.rdbuf(), 2);
 
       multiplexerStreambuf << "Hello, world" << endl;  // channel the message only to 'log'
     \endcode

   \sa cmnLODMultiplexerStreambuf.h, cmnMultiplexerStreambufProxy.h,
   C++ documentation of basic_streambuf and iostream
 */
class cmnLODOutputMultiplexer : public std::ostream
{
 public:
    typedef char char_type;

    typedef cmnLODMultiplexerStreambuf<char_type> SinkType;
  
    typedef std::ostream BaseType;

    typedef SinkType::LodType LodType;
  
    /*! 
      Constructor that initializes base class and store LOD.
    */
    cmnLODOutputMultiplexer(SinkType *multiplexer, LodType lod)
        : BaseType(NULL), m_StreambufProxy(multiplexer, lod) {          
        if (multiplexer != NULL) {
            init(&m_StreambufProxy);
        }
    }
  
    /*! 
      Copy Ctor is necessary because of the call to init()
    */
    cmnLODOutputMultiplexer(const cmnLODOutputMultiplexer &other)
        : BaseType(NULL), m_StreambufProxy(other.m_StreambufProxy) {
        if (m_StreambufProxy.GetOutput() != NULL)
            init(&m_StreambufProxy);
    }

    /*! This method simply returns a reference to this object.  It is
      used when a cmnLODOutputMultiplexer is created and immediately
      operated on, to ensure that the object operated on is an l-value
      rather than r-value.  This solves compiler ambiguities and
      errors that lead to incorrect printout.  Use it where the old
      CMN_LOG_HACK would be used.  Normally, one doesn't need to call
      this method in other cases.
    */
    cmnLODOutputMultiplexer & Ref(void)
    {
        return *this;
    }

    /*! 
      Returns the Level of Detail.
     */
    LodType GetLOD(void) const {
        return m_StreambufProxy.GetLOD();
    }

    /*! 
      Sets the Level of Detail.
     */  
    void SetLOD(LodType lod) {
        m_StreambufProxy.SetLOD(lod);
    }
  

 private:
    cmnMultiplexerStreambufProxy<char_type> m_StreambufProxy;

};


#endif // _cmnLODOutputMultiplexer_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnLODOutputMultiplexer.h,v $
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
// Revision 1.6  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.5  2004/10/09 18:52:16  ofri
// Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
// warnings when it encounters #pragma warning .  Some files had that prama used
// if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
// including cmnPortability and adding the warning disable that.
//
// Revision 1.4  2004/10/08 21:55:58  ofri
// Added method cmnLODOutputMultiplexer::Ref() and used it to replace the use
// of CMN_LOG_HACK in cmnLogger.h .  The correctness of this solution was tested
// on gcc and .net2003, using the helloWorld example.  See #67
//
// Revision 1.3  2003/09/09 18:53:46  anton
// changed from double quotes to <> for all #include
//
// Revision 1.2  2003/06/23 18:49:32  anton
// remove tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:59  anton
// no message
//
//
// ****************************************************************************
