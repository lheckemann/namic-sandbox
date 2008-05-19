/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsExport.h,v 1.2 2005/09/26 15:41:47 anton Exp $
//
//  Author(s):  Anton Deguet
//  Created on: 2005-01-07
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2005, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  or Dr. Gabor Fichtinger, CISST Engineering Director, at gabor@cs.jhu.edu.
//  
// ****************************************************************************

/*! \file
    \brief Macros to export the symbols of cisstOSAbstraction (in a Dll).
*/

// check if this module is build as a DLL
#ifdef cisstRealTime_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif

// include common defines
#include <cisstCommon/cmnExportMacros.h>

// avoid impact on other modules
#undef CISST_THIS_LIBRARY_AS_DLL


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: rtsExport.h,v $
//  Revision 1.2  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.1  2005/06/17 20:30:50  alamora
//  cisstOSAbstraction and cisstRealTime: Added some required
//  CISST_EXPORT to compile DLLs.
//
//
// ****************************************************************************

