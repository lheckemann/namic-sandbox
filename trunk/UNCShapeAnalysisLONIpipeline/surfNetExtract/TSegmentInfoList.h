// -*- C++ -*-
/****************************************************************************
 *
 *  class TSegmentInfoList
 *
 ****************************************************************************
 *
 *  File         :  TSegmentInfoList.h
 *  Type         :  
 *  Purpose      :  a list of TSegmentInfo objects: the whole border-polygon
 *                  can be reconstructed with the indices in this list
 *
 ****************************************************************************
 *                 
 *  started      :  20 Jan 95     Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#ifndef TSEGMENTINFOLIST_H
#define TSEGMENTINFOLIST_H

#include <iostream.h>
#include "util.h"
#include "TSegmentInfo.h"

class TSegmentInfoItem {
public:
   TSegmentInfoItem(TSegmentInfo val, TSegmentInfoItem* next = 0) : val_(val), next_(next) { }

   TSegmentInfo     val_;
   TSegmentInfoItem*   next_;
}; 

class TSegmentInfoList {
public:
   TSegmentInfoList() : last_(0) { }
   ~TSegmentInfoList();
  
   TSegmentInfoItem*  succ(TSegmentInfoItem* it) const    { return it == last_ ? 0 : it->next_ ; }
   TSegmentInfoItem*  cyclic_succ(TSegmentInfoItem* it) const   { return it->next_ ; }
   TSegmentInfoItem*  first() const                 { return last_ == 0 ? 0 : last_->next_ ; }
   TSegmentInfoItem*  last() const                 { return last_ ; }
   TSegmentInfoItem*  search(TSegmentInfo val) const;
   TSegmentInfoItem*    pred(TSegmentInfoItem* it) const;
  
   int size()   const;
   int length() const                          { return size(); }
   int empty()  const                          { return last_ == 0 ? TTRUE : TFALSE; }
   int contains(TSegmentInfo val) const;
    
   void insert(TSegmentInfo val);
   void append(TSegmentInfo val);
   TSegmentInfo pop();
   void push(TSegmentInfo val)                   { insert(val); };
   void insert_after(TSegmentInfo val, TSegmentInfoItem* it);
   void remove(TSegmentInfoItem* it);

   void print(ostream& = cout) const;
   friend ostream& operator<< (ostream& os, const TSegmentInfoList& li);
  
private:
  TSegmentInfoItem* last_;
};


#define forall_segmentinfolistitems(a,l) for (a = (l).first(); a; a = (l).succ(a) )


#endif
