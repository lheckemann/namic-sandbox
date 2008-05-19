/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnMultiplexerStreambufProxy.h,v 1.11 2006/01/03 03:33:04 anton Exp $

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
  \brief Description of types for dynamic control of output messages.
*/

#ifndef _cmnMultiplexerStreambufProxy_h
#define _cmnMultiplexerStreambufProxy_h

#include <list>
#include <algorithm>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLODMultiplexerStreambuf.h>


/*!
  \brief Types for dynamic control of output messages. 

  This includes debugging information,
  error reporting, state logging etc.
  This file declares *class cmnMultiplexerStreambufProxy*. It is a templated class
  derived from the standard library's basic_streambuf. But is stores a Level Of 
  Detail (LOD) descriptor that applies to each message that's output through it.
  The LOD descriptor is transferred to a cmnLODMultiplexerStreambuf object,
  that stores a list of LODed output channels, and decides which ones will
  be actually used.
  The proxy is implemented by overriding basic_streambuf output functions
  xsputn(), overflow() and sync().
 
 
   Usage:
         Include the module in your application with:
         \#include <cmnMultiplexerStreambufProxy.h>
     
     Attach a cmnMultiplexerStreambufProxy object with an ostream. The output functions
     << , put(), write() etc will operate directly on the cmnMultiplexerStreambufProxy.
     
     Example of using a cmnLODOutputMultiplexer, which is an ostream that is
     attached to a proxy.  Assume that the object lodMultiplexerStreambuf is
     a cmnLODMultiplexerStreambuf.
 
     \code
     // The multiple output channels
     ofstream log("logfile.txt");
     windowoutputstream display;    // hypothesized class
     
     lodMultiplexerStreambuf.AddChannel(log.rdbuf(), 5);
     lodMultiplexerStreambuf.AddChannel(windowoutputstream.rdbuf(), 2);
     
     cmnLODMultiplexer multiplexerOutput(&lodMultiplexetStreambuf, 3);
     
     multiplexerStreambuf << "Hello, world" << endl;  // channel the message only to 'log'
     \endcode
     
     Notes: 
     -# It is assumed that none of the output channels modifies the data arguments.
     -# cmnMultiplexerStreambufProxy does not buffer data. Instead, whenever at attempt is 
        made to use stream::put() on a stream with a multiplexer streambuf, the 
        cmnMultiplexerStreambuf::overflow() is called automatically, and it forwards the
        character to the output channels.
 
 
   \sa C++ manual on basic_ostream and basic_streambuf. cmnOutputMultiplexer.h and cmnLODMultiplexerStreambuf.h
 */
template<class _element, class _trait>
class cmnMultiplexerStreambufProxy : public std::basic_streambuf<_element, _trait>
{
 public:

    typedef cmnLODMultiplexerStreambuf<_element, _trait> ChannelType;
    typedef typename ChannelType::LodType LodType;
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
  
    /*! Constructor: initialize the true output multiplexer and the current LOD. */
    cmnMultiplexerStreambufProxy(ChannelType *output, LodType lod)
        : m_OutputChannel(output), m_LOD(lod)
        {}

    /*! Copy Ctor is unnecessary, but I wrote it anyway. */
    cmnMultiplexerStreambufProxy(const cmnMultiplexerStreambufProxy<_element, _trait> & other)
        :/* basic_streambuf<_element, _trait>( (basic_streambuf<_element, _trait> &)other),*/
        m_OutputChannel(other.GetOutput()),
        m_LOD(other.GetLOD())
        {}
  
    /*! Returns the Level of Detail. */ 
    LodType GetLOD(void) const {
        return m_LOD;
    }
  
    /*! Sets the Level of Detail. */ 
    void SetLOD(LodType lod) {
        m_LOD = lod;
    }
  
    /*! Returns a pointer to the output multiplexer. */
    ChannelType * GetOutput(void) const {
        return m_OutputChannel;
    }
  
    // Here we override the basic_streambuf methods for multiplexing.
    // This part is declared 'protected' following the declarations of 
    // basic_streambuf. See basic_streambuf documentation for more
    // details.
 protected:
  
    /*! Override the basic_streambuf xsputn to do the multiplexing. */
    virtual std::streamsize xsputn(const _element *s, std::streamsize n);
  
    /*! Override the basic_streambuf sync for multiplexing. */
    virtual int sync(void);
  
    /*! Override the basic_streambuf overflow for
      multiplexing. overflow() is called when sputc() discovers it does
      not have space in the storage buffer. In our case, it's
      always. See more on it in the basic_streambuf documentation. */
    virtual int_type overflow(int_type c = _trait::eof());
  

 private:
    ChannelType * m_OutputChannel;
    LodType m_LOD;

};


//********************************
// Template method implementation
//********************************


/*! Override the basic_streambuf xsputn to do the multiplexing. */
template<class _element, class _trait>
std::streamsize cmnMultiplexerStreambufProxy<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    return m_OutputChannel->xsputn(s, n, m_LOD);
}


/*! Override the basic_streambuf sync for multiplexing. */
template<class _element, class _trait>
int cmnMultiplexerStreambufProxy<_element, _trait>::sync()
{
    return m_OutputChannel->sync();
}


/*! Override the basic_streambuf overflow for multiplexing. overflow() is called when
 * sputc() discovers it does not have space in the storage buffer. In our case, 
 * it's always. See more on it in the basic_streambuf documentation.
 */
template<class _element, class _trait>
typename cmnMultiplexerStreambufProxy<_element, _trait>::int_type 
cmnMultiplexerStreambufProxy<_element, _trait>::overflow(int_type c)
{
    return m_OutputChannel->overflow(c, m_LOD);
}


#endif  // _cmnMultiplexerStreambufProxy_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnMultiplexerStreambufProxy.h,v $
// Revision 1.11  2006/01/03 03:33:04  anton
// cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
// cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
// Revision 1.10  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.9  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.7  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.6  2004/10/09 18:52:16  ofri
// Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
// warnings when it encounters #pragma warning .  Some files had that prama used
// if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
// including cmnPortability and adding the warning disable that.
//
// Revision 1.5  2003/09/17 17:04:52  anton
// changed template parameters to _aBetterName
//
// Revision 1.4  2003/09/17 14:18:46  anton
// removed useless cmnEXPORT in front of templated classes
//
// Revision 1.3  2003/09/09 18:53:46  anton
// changed from double quotes to <> for all #include
//
// Revision 1.2  2003/06/23 18:48:57  anton
// remove tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:59  anton
// no message
//
//
// ****************************************************************************
