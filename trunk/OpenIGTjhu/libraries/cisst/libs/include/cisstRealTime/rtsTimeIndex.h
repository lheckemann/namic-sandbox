/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsTimeIndex.h,v 1.3 2005/10/07 02:25:44 anton Exp $
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
  \brief Defines a time index used for rtsStateDataTable.
*/

#ifndef _rtsTimeIndex_h
#define _rtsTimeIndex_h

/*! TimeTicks are typedef'ed as unsigned long */
typedef unsigned long rtsTimeTicks;

/*!  \brief The time indexing of state data is provided by the
  rtsTimeIndex class.

  \ingroup cisstRealTime

  Each iteration of the Task Loop increments a tick counter, which is
  stored as an unsigned long.  (rtsTimeTicks). A rtsTimeIndex object
  contains the following information: The tick value is stored in the
  rtsTimeIndex class to act as a data validity check. This is used to
  ensure that the circular buffer has not wrapped around and
  overwritten the index entry between the time that the rtsTimeIndex
  object was created and the time it was used.
 */
class rtsTimeIndex {

  /*! The index into the set of circular buffers corresponding to
      the time */
  int TimeIndex;

  /*! The tick value corresponding to the time. */
  rtsTimeTicks TimeTicks;

  /*! The size of the circular buffer. */
  int Length;

public:
  /*! Default constructor. Does nothing. */
  rtsTimeIndex(): TimeIndex(0), TimeTicks(0), Length(0) {}
    
  /*! Default constructor. Does nothing. */
  rtsTimeIndex(int index, rtsTimeTicks ticks, int length):
        TimeIndex(index), TimeTicks(ticks), Length(length) {}
    
  /*! Default destructor. Does nothing. */
  ~rtsTimeIndex() {}

  /*! Return the index into the circular buffer.*/
  int Index(void) const {
        return TimeIndex;
    }

  /*! Return the ticks corresponding to the time. */
  rtsTimeTicks Ticks(void) const {
        return TimeTicks;
    }

  /*! 
      Note that the increment operators are not defined for
      this class, since we dont want future times.
   */
  rtsTimeIndex& operator--() {
    TimeTicks--;
    TimeIndex--;
    if (TimeIndex<0) TimeIndex=Length-1;
    return *this;
  }

  /*! Overloaded operator */
  rtsTimeIndex& operator-=(int number) {
    TimeTicks-=number;
    TimeIndex=(TimeIndex-number)%Length;
    return *this;
  }

  /*! Overloaded operator */
  rtsTimeIndex operator-(int number) {
    rtsTimeIndex tmp = *this;
    return (tmp -= number);
  }

  /*! The comparison operators */
  bool operator==(const rtsTimeIndex& that) const {
    return (TimeIndex == that.TimeIndex && TimeTicks == that.TimeTicks);
  }

  /*! Overloaded operator */
  bool operator!=(const rtsTimeIndex& that) const {
    return !(*this == that);
  }
};


#endif // _rtsTimeIndex_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsTimeIndex.h,v $
// Revision 1.3  2005/10/07 02:25:44  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.2  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.2  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
