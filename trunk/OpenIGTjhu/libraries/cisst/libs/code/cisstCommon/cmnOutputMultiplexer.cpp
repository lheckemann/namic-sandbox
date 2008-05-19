/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnOutputMultiplexer.cpp,v 1.8 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on: 2002-04-18

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


#include <algorithm>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnOutputMultiplexer.h>


// Add an output channel. See notes above.
// Parameters:
//   channel    - a pointer to the output channel to be added.
//
// Return value: *this
std::ostream & cmnOutputMultiplexer::AddChannel(ChannelType * channel)
{
  ChannelContainerType::iterator it = std::find(m_ChannelContainer.begin(), m_ChannelContainer.end(), 
                                                  channel);

    if (it == m_ChannelContainer.end()) {
        m_ChannelContainer.insert(it, channel);
        m_Streambuf.AddChannel( channel->rdbuf(), 0 );
    }

    return (*this);
}


// Remove an output channel.
// Parameters:
//   channel    - a pointer to the output channel to be removed. No change occurs if
//                the pointer is not on the list of channels for this multiplexer
//
// Return value: *this
std::ostream & cmnOutputMultiplexer::RemoveChannel(ChannelType * channel)
{
    m_ChannelContainer.remove(channel);
    m_Streambuf.RemoveChannel( channel->rdbuf() );
    return (*this);
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnOutputMultiplexer.cpp,v $
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2004/10/09 18:52:49  ofri
// Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
// warnings when it encounters #pragma warning .  Some files had that prama used
// if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
// including cmnPortability and adding the warning disable that.
//
// Revision 1.5  2004/08/20 20:09:21  anton
// cisstCommon: Corrected bug introduced by [709] (find requires std::find)
//
// Revision 1.4  2004/08/19 21:04:02  anton
// cmnOutputMultiplexer: Removed using namespace std.
//
// Revision 1.3  2003/09/09 18:50:14  anton
// changed from double quotes to <> for all #include
//
// Revision 1.2  2003/06/23 20:53:51  anton
// removed tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:56  anton
// no message
//
//
// ****************************************************************************
