/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsStateDataArrayBase.h,v 1.2 2005/09/26 15:41:47 anton Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************

/*!
  \file
  \brief Defines a base class the state data array.
  \ingroup cisstRealTime
*/

#ifndef _rtsStateDataArrayBase_h
#define _rtsStateDataArrayBase_h

#include <cisstCommon/cmnDataObject.h>

/*!
  \ingroup cisstRealTime

  Abstract base class for state data arrays.

  \sa rtsStateDataArray */
class rtsStateDataArrayBase {
protected:
  /*! Protected constructor. Does nothing. */
  rtsStateDataArrayBase(){};

public:
  /*! Default destructor. Does nothing. */
  virtual ~rtsStateDataArrayBase() {};

  /*! Overloaded subscript operator. */
  virtual cmnDataObject& operator[](int num) = 0;

  /*! Overloaded subscript operator. */
  virtual const cmnDataObject& operator[](int num) const = 0;

  /*! Create the array of data. */
  virtual rtsStateDataArrayBase* Create(int size) = 0;

  /*! Copy data from one index to another. */
  virtual void Copy(int to, int from) = 0;

  /*! Get data from array. */
  virtual bool Get(int index, cmnDataObject& data) const = 0;

  /*! Set data in array. */
  virtual bool Set(int index, const cmnDataObject& data) = 0;
};


#endif // _rtsStateDataArrayBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsStateDataArrayBase.h,v $
// Revision 1.2  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.4  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.3  2004/03/16 17:41:54  kapoor
// Changed to cmdDataObject.
//
// Revision 1.2  2004/03/04 07:31:17  kapoor
// lots of fixes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
