/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnDataObject.cpp,v 1.8 2006/05/07 04:45:26 kapoor Exp $
  
  Author(s):  Ankur Kapoor
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


#include <cisstCommon/cmnDataObject.h>

std::ostream& operator << (std::ostream& out, const cmnDataObject& data) {
  data.ToStream(out);
  return out;
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnDataObject.cpp,v $
// Revision 1.8  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.7  2006/05/05 05:05:46  kapoor
// cisstDeviceInterface wrapping: Moved GainData to ddiTypes (to be split later),
// wrappers for basic types (double, int etc) to be passed through mailbox.
//
// Revision 1.6  2006/05/02 21:18:38  anton
// Data object (cmn and ddi): Renamed StrOut to ToString for consistency.
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/08/14 00:51:26  anton
// cisstCommon: Port to gcc-4.0 (use template<> for template specialization).
//
// Revision 1.3  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/02/03 22:25:56  anton
// cmnDataObject: Added required code to specialize cmnTypeTraits<>::TypeName
//
// Revision 1.1  2004/05/04 14:07:06  anton
// Added cmnDataObject
//
//
// Revision 1.1  2004/03/16 18:02:12  kapoor
// Added cmnDataObject.
//
// ****************************************************************************
