// -*- C++ -*-
/****************************************************************************
 *
 *  class TSegmentInfoList
 *
 ****************************************************************************
 *
 *  File         :  TSegmentInfoList.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  20 Jan 95     Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#include <iostream.h>

#include "util.h"
#include "TSegmentInfoList.h"

TSegmentInfoList::~TSegmentInfoList() {
   if (last_) {
      TSegmentInfoItem* ptr = last_->next_;
      last_->next_ = 0;
      while (ptr) {
   TSegmentInfoItem* tmp = ptr;
   ptr = ptr->next_;
   delete tmp;
      }
   }
}

TSegmentInfoItem* TSegmentInfoList::search(TSegmentInfo val) const
{
   TSegmentInfoItem* p = 0;
   forall_segmentinfolistitems(p, *this)
      if (p->val_ == val)
   break;
  
   return p;
}

int TSegmentInfoList::contains(TSegmentInfo val) const
{
   TSegmentInfoItem* p = 0;
   forall_segmentinfolistitems(p, *this)
      if (p->val_ == val)
   return 1;
  
   return 0;
}
 
int TSegmentInfoList::size() const
{
   int i = 0;
   TSegmentInfoItem* a;
   forall_segmentinfolistitems(a, *this)
      i++;
  
   return i;
}

void TSegmentInfoList::insert(TSegmentInfo val) {
   TSegmentInfoItem* a = new TSegmentInfoItem(val);
   if (last_)
      a->next_ = last_->next_;
   else
      last_ = a;
   last_->next_ = a;
}

void TSegmentInfoList::append(TSegmentInfo val)
{
   TSegmentInfoItem* a = new TSegmentInfoItem(val); 
   if (last_) {
      a->next_ = last_->next_;
      last_ = last_->next_ = a;
   }
   else
      last_ = a->next_ = a;
}

TSegmentInfo TSegmentInfoList::pop()
{
   assertStr(last_ != 0, "TSegmentInfoList::pop : nothing in list to pop");
  
   TSegmentInfoItem* f = last_->next_;
   if (f == last_)
    last_ = 0;
   else 
      last_->next_ = f->next_;
  
   static TSegmentInfoItem * last = NULL;
   if (last) delete last;
   last = f;

   return f->val_;
}
 
void TSegmentInfoList::remove(TSegmentInfoItem* it)
{
   TSegmentInfoItem* before = it;
  
   while (before->next_ != it) before=before->next_;
   before->next_ = it->next_;
   if (last_ == it) last_ = before;
   if (before == it) last_ = 0;
   delete it;
}
 
TSegmentInfoItem* TSegmentInfoList::pred(TSegmentInfoItem* it) const
{
   TSegmentInfoItem* before = it;
   while (before->next_ != it) before=before->next_;

   return(before);
}

void TSegmentInfoList::insert_after(TSegmentInfo val, TSegmentInfoItem* it)
{
   TSegmentInfoItem* a = new TSegmentInfoItem(val);
   a->next_ = it->next_;
   it->next_ = a;
   if (it == last_) last_ = a;
}
 
void TSegmentInfoList::print(ostream& os) const
{
   int first = TTRUE;
   if(!empty()) {
      for(TSegmentInfoItem* ptr = last_->next_;; ptr = ptr->next_) {
   if(first) first = TFALSE;
   else      os << " , ";
   os << ptr->val_;
   if (ptr == last_) 
      break;
      }
   }
}

 
ostream& operator<< (ostream& os, const TSegmentInfoList& li) 
{
   li.print(os); 
   return os; 
}


