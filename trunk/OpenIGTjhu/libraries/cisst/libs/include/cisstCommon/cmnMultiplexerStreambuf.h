/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnMultiplexerStreambuf.h,v 1.12 2006/01/03 03:33:04 anton Exp $

  Author(s):  Ofri Sadowsky
  Created on: 2002-04-19

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
   \brief Types for dynamic control of output messages
 */

#ifndef _cmnMultiplexerStreambuf_h
#define _cmnMultiplexerStreambuf_h

#include <list>
#include <algorithm>

#include <cisstCommon/cmnPortability.h>

/*!
  \brief A Streambuffer class that allows output to multiple streambuf objects.

  Types for dynamic control of output messages. This includes
  debugging information, error reporting, state logging etc.  This
  file declares *class cmnMultiplexerStreambuf*. It is a templated
  class derived from the standard library's basic_streambuf, with the
  main additional feature that enables channeling the output to
  multiple other streambuf objects.  The multiplexing is implemented
  by overriding basic_streambuf output functions xsputn(), overflow()
  and sync().
 
 
  Usage:
  Include the module in your application with:
  \#include "cmnMultiplexerStreambuf.h"
     
  Add output streambuf channels to the multiplexer using the
  AddChannel() method.
 
  Remove output channels from the multiplexer using the
  RemoveChannel() method.
 
  Attach a cmnMultiplexerStreambuf object with an ostream. The output
  functions << , put(), write() etc will operate directly on the
  cmnMultiplexerStreambuf.
     
  Example of using a cmnOutputMultiplexer, which is an ostream using a
  cmnMultiplexerStrembuf (an output stream with multiplexer
  streambuf):
     
  \code
  ofstream log("logfile.txt");
  windowoutputstream display;   // hypothesized class
  
  cmnOutputMultiplexer multiplexer;
  multiplexer.AddChannel(&log);
  multiplexer.AddChannel(&windowoutputstream);
  
  multiplexer << "Hello, world" << endl;  // channel the message to all streams.
  \endcode   
  
  Notes: 
  -# cmnMultiplexerStreambuf does not OWN the output channels. They are created
  and destroyed externally.
  -# The initial implementation uses a list to store the addresses of the output channels.
  There is no guarantee on the order of storage or the order of output channelling.
  -# It is assumed that none of the output channels modifies the data arguments.
  -# It is guaranteed that unique streambuf instances are stored in a single 
  cmnMultiplexerStreambuf. The AddChannel() function checks for uniqueness.
  -# cmnMultiplexerStreambuf does not buffer data. Instead, whenever at attempt is made
  to use stream::put() on a stream with a multiplexer streambuf, the 
  cmnMultiplexerStreambuf::overflow() is called automatically, and it forwards the
  character to the output channels.
  
   \sa C++ manual on basic_ostream and basic_streambuf. cmnOutputMultiplexer.h
*/
template<class _element, class _trait = std::char_traits<_element> >
class cmnMultiplexerStreambuf : public std::basic_streambuf<_element, _trait>
{
    public:
    
    typedef std::basic_streambuf<_element, _trait> ChannelType;
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
    
    /*! 
     * Type of internal data structure storing the channels.
     */
    typedef std::list<ChannelType *> ChannelContainerType;

    /*! 
     * Constructor: currently empty.
     */
    cmnMultiplexerStreambuf()
    {}
    
    /*! Add an output channel. See notes above.
     * \param channel A pointer to the output channel to be added.
     */
    void AddChannel(ChannelType *channel);

    /*! Remove an output channel.
     * \param channel A pointer to the output channel to be removed. No change occurs if
     * the pointer is not on the list of channels for this multiplexer.
     *
     */
    void RemoveChannel(ChannelType * channel);

    /*! Enable access to the channel storage, without addition or removal of channels.
     * Elements of the container can be accessed using the standard const_iterator
     * interfaces.  Note that the channels themselves are non-const, so individual
     * manipulation of each is enabled.
     */
    const ChannelContainerType & GetChannels() const
    {
        return m_ChannelContainer;
    }
    
    
    // Here we override the basic_streambuf methods for multiplexing.
    // This part is declared 'protected' following the declarations of 
    // basic_streambuf. See basic_streambuf documentation for more
    // details.
    protected:
    
    /*! Override the basic_streambuf xsputn to do the multiplexing. */
    virtual std::streamsize xsputn(const _element *s, std::streamsize n);
    
    /*! Override the basic_streambuf sync for multiplexing. */
    virtual int sync();
    
    /*! Override the basic_streambuf overflow for multiplexing. overflow() is called when
      sputc() discovers it does not have space in the storage buffer. In our case, 
      it's always. See more on it in the basic_streambuf documentation.
    */
    virtual int_type overflow(int_type c = _trait::eof());

    private:
    /*! The actual container that stores channel addresses.*/
    ChannelContainerType m_ChannelContainer;

};


//********************************
// Template method implementation
//********************************


template<class _element, class _trait>
void cmnMultiplexerStreambuf<_element, _trait>::AddChannel(ChannelType * channel)
{
    typename ChannelContainerType::iterator it = find(m_ChannelContainer.begin(), m_ChannelContainer.end(), 
                                                      channel);

    if (it == m_ChannelContainer.end()) {
        m_ChannelContainer.insert(it, channel);
    }
}


template<class _element, class _trait>
void cmnMultiplexerStreambuf<_element, _trait>::RemoveChannel(ChannelType * channel)
{
    m_ChannelContainer.remove(channel);
}


template<class _element, class _trait>
std::streamsize cmnMultiplexerStreambuf<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    std::streamsize ssize;
    typename ChannelContainerType::iterator it;
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        ssize = (*it)->sputn(s, n);
    }
    return ssize;
}


template<class _element, class _trait>
int cmnMultiplexerStreambuf<_element, _trait>::sync()
{
    typename ChannelContainerType::iterator it;
    // synchronize all the channels
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        (*it)->pubsync();
    }
    return 0;
}


template<class _element, class _trait>
typename std::basic_streambuf<_element, _trait>::int_type 
cmnMultiplexerStreambuf<_element, _trait>::overflow(int_type c)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));

    typename ChannelContainerType::iterator it;

    // multiplexing
    for (it = m_ChannelContainer.begin(); it != m_ChannelContainer.end(); it++) {
        (*it)->sputc(_trait::to_char_type(c));
    }

    // follow the basic_streambuf standard
    return _trait::not_eof(c);
}


#endif // _cmnMultiplexerStreambuf_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnMultiplexerStreambuf.h,v $
// Revision 1.12  2006/01/03 03:33:04  anton
// cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
// cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
// Revision 1.11  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.10  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.7  2004/10/09 18:52:16  ofri
// Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
// warnings when it encounters #pragma warning .  Some files had that prama used
// if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
// including cmnPortability and adding the warning disable that.
//
// Revision 1.6  2003/10/02 14:25:47  anton
// Corrected the doxygen documentation
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
// Revision 1.2  2003/06/23 18:49:17  anton
// remove tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:59  anton
// no message
//
//
// ****************************************************************************

