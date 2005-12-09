// -*- C++ -*-
/****************************************************************************
 *
 *  class TSegmentInfo
 *
 ****************************************************************************
 *
 *  File         :  TSegmentInfo.h
 *  Type         :  
 *  Purpose      :  Information about one user-defined segment of border-
 *                  polygon
 *
 ****************************************************************************
 *                 
 *  started      :  28 Jan 95     Robert Schweikert
 *  last change  :  28 Feb 95
 *
 ****************************************************************************/
 
#ifndef TSEGMENTINFO_H
#define TSEGMENTINFO_H

class TSegmentInfo {
public:  
   TSegmentInfo() : inx_(-1) { }
   TSegmentInfo(int inx) : inx_(inx) { }

   // same when referring to same index
   friend int operator==(const TSegmentInfo& si1, const TSegmentInfo& si2) { return si1.inx_ == si2.inx_; }

   // output inx_
   friend ostream& operator<< (ostream& os, const TSegmentInfo& v) { os << v.inx_; return os;  } 

   int inx_;      // the click point
};

#endif
