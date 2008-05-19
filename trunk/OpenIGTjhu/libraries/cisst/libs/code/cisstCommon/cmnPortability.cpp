/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPortability.cpp,v 1.8 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2003-09-08

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


#include <cisstCommon/cmnPortability.h>

const std::string cmnOperatingSystemsStrings[] = {"Undefined", 
                                                  "Windows",
                                                  "Linux",
                                                  "RT-Linux",
                                                  "IRIX",
                                                  "SunOS",
                                                  "Linux-RTAI",
                                                  "CYGWIN",
              "Darwin"
};

const std::string cmnCompilersStrings[] = {"Undefined",
                                           "gcc",
                                           "VisualC++ 6",
                                           ".NET 7",
                                           "SGI CC",
                                           "Sun CC",
                                           "Intel CC",
                                           ".NET 2003"};


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnPortability.cpp,v $
//  Revision 1.8  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.7  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.6  2004/12/28 05:30:22  anton
//  cisst: Added some code to compile under Darwin (i.e. Mac OS X).  Tested
//  with cisstCommon, cisstVector and cisstNumerical (with cnetlib).
//
//  Revision 1.5  2004/09/24 03:55:30  ofri
//  cmnPortability: Added identifiers for CYGWIN gcc compiler
//
//  Revision 1.4  2004/08/12 20:25:36  anton
//  cmnPortability: Added some preliminary code to support .Net 2003
//
//  Revision 1.3  2004/02/18 22:11:03  anton
//  Added Linux-RTAI
//
//  Revision 1.2  2003/09/12 17:51:12  anton
//  changed name of OSs to match uname -s
//
//  Revision 1.1  2003/09/09 18:50:47  anton
//  creation
//
//
// ****************************************************************************
