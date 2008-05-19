/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsStateDataTable.h,v 1.12 2006/07/07 04:10:26 pkaz Exp $
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
  \brief Defines the state data table.
  \ingroup cisstRealTime
*/

#ifndef _rtsStateDataTable_h
#define _rtsStateDataTable_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiTypes.h>

#include <cisstRealTime/rtsStateDataArrayBase.h>
#include <cisstRealTime/rtsStateDataArray.h>
#include <cisstRealTime/rtsTimeIndex.h>


#include <vector>
#include <iostream>

// Always include last
#include <cisstRealTime/rtsExport.h>

/*! rtsStateDataId.  Unique identifier for the columns of the State
  Data Table.  Typedef'ed to an int */
typedef int rtsStateDataId;

/*!
  \ingroup cisstRealTime

  The state data table is the storage for the state of the task that
  the table is associated with. It is a heterogenous circular buffer
  and can contain data of any type so long as it is derived from
  cmnDataObject.  The state data table also resolves conflicts
  between reads and writes to the state, by ensuring that the reader
  head is always one behind the write head. To ensure this we have an
  assumption here that there is only one writer, though there can be
  multiple readers. State Data Table is also refered as Data Table or
  State Table elsewhere in the documentation.
 */
class CISST_EXPORT rtsStateDataTable {
 protected:
  /*! The number of rows of the state data table. */
  unsigned int HistoryLength;
  
  /*! The number of columns of the data table. */
  unsigned int NumberStateData;
  
  /*! The index of the writer in the data table. */
  unsigned int IndexWriter;
  
  /*! The index of the reader in the table. */
  unsigned int IndexReader;
  
  /*! The vector contains pointers to individual columns. */
  std::vector<rtsStateDataArrayBase*> StateVector;

    /*! The vector contains pointers to the current values
      of elements that are to be added to the state when we
      advance. (Note: currently, the StateVectorElements are
      not automatically added in Advance.)
      */
    std::vector<cmnDataObject *> StateVectorElements;
    
  /*! The columns entries can be accessed by name. This vector
    stores the names corresponding to the columns. */
  std::vector<std::string> StateVectorDataNames;

  /*! This vector of boolean indicates if the data in the
     corresponding column under the write head is valid. */
  std::vector<bool> DataValid;

  /*! This sets the default policy on copying the previous data on
    the advance of the write head, if no valid data was written. If
    true data gets copied */
  std::vector<bool> Copy;

  /*! The vector contains the time stamp in counts or ticks per
    period of the task that the state table is associated with. */
  std::vector<rtsTimeTicks> Ticks;
  
 public:
  /*! Constructor. Constructs a state table with a default
    size of 256 rows. */
  rtsStateDataTable(int size = 256):
    HistoryLength(size), NumberStateData(0), IndexWriter(0),IndexReader(0),
    StateVector(NumberStateData), StateVectorDataNames(NumberStateData),
    DataValid(NumberStateData, false), Copy(NumberStateData, true),
        Ticks(size, rtsTimeTicks(0)) {}

  /*! Default destructor. Does nothing */
  ~rtsStateDataTable() {}

  /*! Get a handle for data to be used by a reader.  All the const
      methods, that can be called from a reader and writer. */
  rtsTimeIndex GetIndexReader(void) const;
    
  /*! Verifies if the data is valid. */
  inline bool ValidateReadIndex(const rtsTimeIndex &timeIndex) const {
        return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
    }
    
  /*! Read specified data to be used by a reader */
  bool ReadFromReader(rtsStateDataId id, const rtsTimeIndex &timeIndex,
                        cmnDataObject &obj) const;
    
  /*! Read specified data to be used by a reader */
  bool ReadFromReader(const char *name, const rtsTimeIndex &timeIndex,
                        cmnDataObject &obj);

  /*! Add an element to the table. Should be called during startup
      of a real time loop.  All the non-const methods, that can be
      called from a writer only. */
  template <class _elementType>
    _elementType *NewElement(const char *name = 0, bool copy = true);

  /*! Get a handle for data to be used by a writer */
  rtsTimeIndex GetIndexWriter(void);

  /*! Read specified data to be used by a writer */
  bool ReadFromWriter(rtsStateDataId id, const rtsTimeIndex &timeIndex, cmnDataObject &obj);

  /*! Write specified data. Please note our assumption that there is
      only one writer for the table. */
  bool Write(rtsStateDataId id, const cmnDataObject &obj);

  /*! Advance the pointers of the circular buffer. Note this is the
      only method protected by critical sections. Since we are double
      buffering, we don't expect to run into problems of data
      contention elsewhere, so long as the write index is not the same
      as the read index.
   */
  void Advance(void);

  /*! Return a const reference to the state data table. Used to access
    this state data table for tasks other than the owner. */
  const rtsStateDataTable & GetStateDataTable(void) {
        return *this;
    }

    /*! For debugging, dumps the current data table to output
      stream. */
    void Debug(std::ostream& out);

    /*! For debugging, dumps some values of the current data table to
      output stream. */
    void Debug(std::ostream& out, unsigned int * listColumn, unsigned int number);

    /*! This method is to dump the state data table in the csv format, 
        allowing easy import into matlab.
        Assumes that individual columns are also printed in csv format.
     By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
     value i.e, those rows that have been written to at least once.
     */
    void CSVWrite(std::ostream& out, bool nonZeroOnly = false);
    void CSVWrite(std::ostream& out, unsigned int * listColumn, unsigned int number, bool nonZeroOnly = false);

};


template <class _elementType>
_elementType *rtsStateDataTable::NewElement(const char *name, bool copy) {
    rtsStateDataArray<_elementType> *elementHistory = new rtsStateDataArray<_elementType>(HistoryLength);
    _elementType *element = new _elementType();
    StateVectorElements.push_back(element);
    StateVector.push_back(elementHistory);
    DataValid.push_back(false);
    Copy.push_back(copy);
    if (name) StateVectorDataNames.push_back(name);
    else StateVectorDataNames.push_back("");
    NumberStateData = StateVector.size();
    element->Index = NumberStateData - 1;
    return element;
}

#endif // _rtsStateDataTable_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsStateDataTable.h,v $
// Revision 1.12  2006/07/07 04:10:26  pkaz
// rtsStateDataTable: fixed minor typos in documentation.
//
// Revision 1.11  2006/06/05 03:11:21  kapoor
// cisstRealTime: Mods to state data table. This mod eliminates the need to
// maintain the enumerations in the class. But this requires one to maintain
// a member variable to hold the current state.
//
// Revision 1.10  2006/05/02 20:37:01  anton
// ddi and rts: Modified #include to use newly created ddiTypes.h
//
// Revision 1.9  2006/03/17 15:41:32  kapoor
// stRealTime: Modified the CVSWrite method to print only nonzero entries. See
// also checkin [1876].
//
// Revision 1.8  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/06/19 22:09:42  kapoor
// cisstRealTime: Added methods csvwrite, so that data dump can be imported in MATLAB
//
// Revision 1.6  2005/06/17 22:45:25  anton
// cisst libraries: Added some CISST_EXPORT required to compile DLLs
//
// Revision 1.5  2005/04/16 21:34:42  kapoor
// Now uses dynamic vectors instead of std vector. Still retainins the name
// because changing the name would lead to HUGE modifications in real time
// code.
//
// Revision 1.4  2005/03/10 23:15:03  anton
// rtsStateDataTable: Added Debug method to dump selected fields.
//
// Revision 1.3  2004/10/30 00:59:10  kapoor
// GetIndexReader should be const.
//
// Revision 1.2  2004/09/22 14:17:56  anton
// rtsStateDataTable: Class name was specified within its scope to declare
// rtsStateDataTable::GetReference (Intel CC warning).
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.8  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.7  2004/03/22 00:08:15  kapoor
// Added bool vector - Copy, if set the default action on Advance is to copy
// using the previous value.
//
// Revision 1.6  2004/03/19 16:00:25  kapoor
// Added const and non-const methods to get const and non-const reference to
// a particular data in the StateTable.
//
// Revision 1.5  2004/03/16 17:41:22  kapoor
// Changed to cmnDataObject. AddElement now takes a string to identify the
// respective column in StateTable.
//
// Revision 1.4  2004/03/04 21:26:17  kapoor
// more changes.
//
// Revision 1.3  2004/03/04 08:10:02  kapoor
// Re-ordered data member initializers.
//
// Revision 1.2  2004/03/04 07:31:17  kapoor
// lots of fixes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
