/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\                                                                               /
/               Developed by the Engineering Research Center for                \
\         Computer-Integrated Surgical Systems & Technology (CISST)             /
/                                                                               \
\           Copyright(c) 1995-2001, The Johns Hopkins University                /
/                          All Rights Reserved.                                 \
\                                                                               /
/                    Author: Ankur Kapoor (kapoor@cs.jhu.edu)                   \
\                           Created on: 2004-03-05                              /
/                                                                               \
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
*/
                                                                                                                                                             
// $Id: ddiTwoLoPoMoCo.h,v 1.6 2006/05/13 21:29:21 kapoor Exp $
                                                                                                                                                             
#ifndef _ddiTwoLoPoMoCo_h
#define _ddiTwoLoPoMoCo_h

#include <cisstConfig.h>

#if CISST_HAS_LOPOMOCO || defined DOXYGEN

#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiCompositeDevice.h>

class ddiTwoLoPoMoCo: public ddiCompositeDevice
{

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    
public:
    enum {SIZE = 2};
    char RelativePathToConfigFiles[256];
    
    ddiTwoLoPoMoCo(const char* deviceName = "TwoLoPoMoCo",
       const char* component1Name = "LoPoMoCo1",
       const char* component2Name = "LoPoMoCo2",
           const char* relativePathToConfigFiles = "");
    
    ~ddiTwoLoPoMoCo();

    virtual void Configure(const char* filename = NULL);
};


CMN_DECLARE_SERVICES_INSTANTIATION(ddiTwoLoPoMoCo);


#endif // CISST_HAS_LOPOMOCO

#endif // _ddiTwoLoPoMoCo_h


// $Log: ddiTwoLoPoMoCo.h,v $
// Revision 1.6  2006/05/13 21:29:21  kapoor
// cisstDeviceInterface and LoPoMoCo: Added a relative path member, that
// is appended to the config file name while opening config file.
//
// Revision 1.5  2006/05/11 18:45:36  anton
// ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.2  2005/03/01 16:32:02  anton
// cisstDevice LoPoMoCo: Updated ddiThree and ddiFourLoPoMoCo to allow user
// defined configuration files.  See ticket #129.
//
// Revision 1.1  2005/02/28 22:35:04  anton
// cisstDeviceInterface:  Added ddiTwoLoPoMoCo.
//
