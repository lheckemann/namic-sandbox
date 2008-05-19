/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsStateDataTable.cpp,v 1.10 2006/07/07 04:10:26 pkaz Exp $
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

#include <cisstRealTime/rtsStateDataTable.h>

#include <iostream>
#include <string>

/* All the const methods that can be called from reader or writer */

rtsTimeIndex rtsStateDataTable::GetIndexReader(void) const {
  int tmp = IndexReader;
  return rtsTimeIndex(tmp, Ticks[tmp], HistoryLength);
}


bool rtsStateDataTable::ReadFromReader(rtsStateDataId id, const rtsTimeIndex& timeIndex, cmnDataObject &obj) const {
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "ReadFromReader: obj must be created using NewElement " << obj.TypeName() << std::endl;
        return false;
    }
  // timeIndex refers to some time and index  tuple of the past.
  if (!StateVector[id]) {
    CMN_LOG(1) << "ReadFromReader: No state data array corresponding to given id: " << id << ", " << obj.TypeName() << std::endl;
    return false;
  }
  StateVector[id]->Get(timeIndex.Index(), obj);
    obj.Index = id;
  return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
}


bool rtsStateDataTable::ReadFromReader(const char *name, const rtsTimeIndex& timeIndex, cmnDataObject &obj) {
  for (unsigned int i = 0; i < StateVectorDataNames.size(); i++) {
    if (StateVectorDataNames[i] == name) {
      return ReadFromReader(i, timeIndex, obj);
    }
  }
  return false;
}

/* All the non-const methods that can be called from writer only */

rtsTimeIndex rtsStateDataTable::GetIndexWriter(void) {
  return rtsTimeIndex(IndexWriter, Ticks[IndexWriter], HistoryLength);
}


bool rtsStateDataTable::ReadFromWriter(rtsStateDataId id, const rtsTimeIndex& timeIndex, cmnDataObject &obj) {
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "ReadFromWriter: obj must be created using NewElement " << obj.TypeName() << std::endl;
        return false;
    }
  if ((timeIndex.Index() == (int)IndexWriter) && (!DataValid[id]))
    return false;
  bool result = (Ticks[timeIndex.Index()] == timeIndex.Ticks());
  if (result) {
    if (!StateVector[id]) {
      CMN_LOG(1) << "ReadFromWriter: No state data array corresponding to given id: " << id << ", " << obj.TypeName() << std::endl;
      return false;
    }
    }
    StateVector[id]->Get(timeIndex.Index(), obj);
    obj.Index = id;
  return result;
}

bool rtsStateDataTable::Write(rtsStateDataId id, const cmnDataObject &obj) {
  bool result;
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "Write: obj must be created using NewElement " << obj.TypeName() << std::endl;
        return false;
    }
  if (!StateVector[id]) {
    CMN_LOG(1) << "Write: No state data array corresponding to given id: " << id << ", " << obj.TypeName() << std::endl;
    return false;
  }
  result = StateVector[id]->Set(IndexWriter, obj);
  if (result) {
    DataValid[id] = true;
  } else {
    CMN_LOG(1) << "Error setting data array value in id: " << id << std::endl;
  }
  return result;
}

void rtsStateDataTable::Advance(void) {
  unsigned int i;
  int tmpIndex;
  /* If for all cases, IndexReader is behind IndexWriter, we don't
     need critical sections. This is based on the assumption that
     there is only one Writer that has access to Advance method and
     this is the only place where IndexReader is modified.  Oh ya!
     another assumption, we don't have a buffer of size less than 3.
   */

  /* So far IndexReader < IndexWriter.
     The following block doesn't modify IndexReader,
     so the above condition still holds.
   */
  tmpIndex = IndexWriter;
  IndexWriter = (IndexWriter + 1) % HistoryLength;
  Ticks[IndexWriter] = Ticks[tmpIndex] + 1;
  for(i = 1; i <= NumberStateData; i++) {
    if (DataValid[i]) {
      DataValid[i] = false;
    } else {
      //should this be the previous one, there is nothing below indexwriter??
    #if 0
      if (StateVector[i]) StateVector[i]->Copy(IndexWriter, tmpIndex);
    #endif
      if (Copy[i]) if (StateVector[i]) StateVector[i]->Copy(tmpIndex, IndexReader);
    }
  }
  /*
     So far so good. IndexReader < IndexWriter.
     If we assume(can we?) atomic copy then we are done,
     since this is the only place where IndexReader
     is changed.
   */
  IndexReader = tmpIndex;
}

void rtsStateDataTable::Debug(std::ostream& out) {
  out << "State Table: " << std::endl;
  unsigned int i;
  out << "Ticks : ";
  for (i = 1; i <= NumberStateData - 1; i++) {
    if (!StateVectorDataNames[i].empty()) out << "["
      << i << "]" << ((DataValid[i])?"T ":"F ")
      << StateVectorDataNames[i].c_str() << " : ";
  }
  out << "[" << i << "]" << ((DataValid[i])?"T ":"F ") << StateVectorDataNames[i].c_str() << std::endl;
  for (i = 0; i < HistoryLength; i++) {
    out << i << ": ";
    out << Ticks[i] << ": ";
    for (unsigned int j = 1; j <= NumberStateData; j++)  {
      if (StateVector[j]) {
          out << " ["
        << j << "] " 
        << (*StateVector[j])[i]
        << " : ";
      }
    }
    if (i==IndexReader)
      out << "<-- Index for Reader";
    if (i==IndexWriter)
      out << "<== Index for Writer";
    out << std::endl;
  }
}


void rtsStateDataTable::Debug(std::ostream& out, unsigned int *listColumn, unsigned int number) {
    unsigned int i, j;

    for (i = 0; i < number; i++) {
  if (!StateVectorDataNames[listColumn[i]].empty()) {
      out << "["
    << listColumn[i] << "]" << ((DataValid[listColumn[i]])?"T ":"F ")
    << StateVectorDataNames[listColumn[i]].c_str() << " : ";
  }
    }
    out << std::endl;

    for (i = 0; i < HistoryLength; i++) {
  out << i << " ";
  out << Ticks[i] << " ";
  for (j = 0; j < number; j++) {
      if (listColumn[j] <= NumberStateData && StateVector[listColumn[j]]) {
    out << " [" << listColumn[j] << "] "
        <<(*StateVector[listColumn[j]])[i] << " : ";
      }
  }
  if (i == IndexReader) {
      out << "<-- Index for Reader";
  }
  if (i == IndexWriter) {
      out << "<== Index for Writer";
  }
  out << std::endl;
    }
}

// This method is to dump the state data table in the csv format, allowing easy import into matlab.
// Assumes that individual columns are also printed in csv format.

// By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
// value i.e, those rows that have been written to at least once.
void rtsStateDataTable::CSVWrite(std::ostream& out, bool nonZeroOnly) {
    unsigned int i;
    for (i = 0; i < HistoryLength; i++) {
        bool toSave = true;
        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
        if (toSave) {
            out << i << " " << Ticks[i] << " ";
            for (unsigned int j = 0; j < StateVector.size(); j++)  {
                if (StateVector[j]) {
                    out << (*StateVector[j])[i] << " ";
                }
            }
            out << std::endl;
        }
    }
}

void rtsStateDataTable::CSVWrite(std::ostream& out, unsigned int *listColumn, unsigned int number, bool nonZeroOnly) {
    unsigned int i, j;

    for (i = 0; i < HistoryLength; i++) {
        bool toSave = true;
        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
        if (toSave) {
            out << i << " " << Ticks[i] << " ";
            for (j = 0; j < number; j++) {
                if (listColumn[j] < StateVector.size() && StateVector[listColumn[j]]) {
                    out << (*StateVector[listColumn[j]])[i] << " ";
                }
            }
            out << std::endl;
        }
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsStateDataTable.cpp,v $
// Revision 1.10  2006/07/07 04:10:26  pkaz
// rtsStateDataTable: fixed minor typos in documentation.
//
// Revision 1.9  2006/06/22 03:09:15  kapoor
// cisstRealTime: Fixed bounds error while writing state table to file.
//
// Revision 1.8  2006/06/05 03:11:21  kapoor
// cisstRealTime: Mods to state data table. This mod eliminates the need to
// maintain the enumerations in the class. But this requires one to maintain
// a member variable to hold the current state.
//
// Revision 1.7  2006/03/17 15:22:03  kapoor
// cisstRealTime: Modified the CVSWrite method to print only nonzero entries.
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/06/19 22:09:42  kapoor
// cisstRealTime: Added methods csvwrite, so that data dump can be imported in MATLAB
//
// Revision 1.4  2005/04/23 00:33:03  kapoor
// rtsStateDataTable: Added time stamp (in ticks) to debug outwith with
// selected columns.
//
// Revision 1.3  2005/03/10 23:15:03  anton
// rtsStateDataTable: Added Debug method to dump selected fields.
//
// Revision 1.2  2004/10/30 00:52:40  kapoor
// Added argument to pass a filename to configure the task/device.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.7  2004/04/08 06:13:05  kapoor
// Fixed cry baby windows. Operator << is not overloaded correctly to accept std::string on windows implementation of STL
//
// Revision 1.6  2004/03/22 00:12:58  kapoor
// Added bool vector Copy.
//
// Revision 1.5  2004/03/19 16:02:42  kapoor
// Added const and non-const methods to get const and non-const reference to
// StateTable data.
//
// Revision 1.4  2004/03/16 17:48:05  kapoor
// Added GetReference Method. Changed couts.
//
// Revision 1.3  2004/03/04 21:26:11  kapoor
// more changes.
//
// Revision 1.2  2004/03/04 07:39:15  kapoor
// lots of changes.
//
// Revision 1.1.1.1  2004/03/02 04:05:02  kapoor
// Importing source into local tree
//
// ****************************************************************************
