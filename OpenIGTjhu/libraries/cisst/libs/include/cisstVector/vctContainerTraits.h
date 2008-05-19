/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctContainerTraits.h,v 1.5 2005/12/23 21:27:31 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2004-11-11

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
  \brief Basic traits for the cisstVector containers
  \ingroup cisstVector
 */


#ifndef _vctContainerTraits_h
#define _vctContainerTraits_h

/*! Macro used to define multiple types based on the type of elements.
  This will define size_type, index_type, difference_type,
  stride_type, value_type, reference, const_reference, pointer,
  const_pointer, NormType (double) and AngleType (double).  Most of
  these types are introduced and named for STL compatibility.

  \param type Type of element, e.g. double, float, char.
 */
#define VCT_CONTAINER_TRAITS_TYPEDEFS(type) \
    typedef unsigned int size_type; \
    typedef unsigned int index_type; \
    typedef int difference_type; \
    typedef int stride_type; \
    typedef type value_type; \
    typedef value_type & reference; \
    typedef const value_type & const_reference; \
    typedef value_type * pointer; \
    typedef const value_type * const_pointer; \
    typedef double NormType; \
    typedef double AngleType

#endif  // _vctContainerTraits_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctContainerTraits.h,v $
// Revision 1.5  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/01/06 23:36:00  anton
// cisstVector Python: As mentioned in ticket #112, the introduction of the
// class vctContainerTraits broke the Python SWIG wrapping.  Found a solution
// for SWIG 1.3.22 (it is known that 1.3.24 DOES NOT WORK).
//
// Revision 1.1  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
//
// ****************************************************************************

