// -*- C++ -*-
/****************************************************************************
 *
 *  class TIntList
 *
 ****************************************************************************
 *
 *  File         :  TIntList.cc
 *  int         :  
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
#include "TIntList.h"

TIntList::~TIntList() {
   if (last_) {
      TIntItem* ptr = last_->next_;
      last_->next_ = 0;
      while (ptr) {
   TIntItem* tmp = ptr;
   ptr = ptr->next_;
   delete tmp;
      }
   }
}

TIntItem* TIntList::search(int val) const
{
   TIntItem* p = 0;
   forall_intlistitems(p, *this)
      if (p->val_ == val)
   break;
  
   return p;
}

int TIntList::contains(int val) const
{
   TIntItem* p = 0;
   forall_intlistitems(p, *this)
      if (p->val_ == val)
   return 1;
  
   return 0;
}
 
int TIntList::size() const
{
   int i = 0;
   TIntItem* a;
   forall_intlistitems(a, *this)
      i++;
  
   return i;
}

void TIntList::insert(int val) {
   TIntItem* a = new TIntItem(val);
   if (last_)
      a->next_ = last_->next_;
   else
      last_ = a;
   last_->next_ = a;
}

void TIntList::append(int val)
{
   TIntItem* a = new TIntItem(val); 
   if (last_) {
      a->next_ = last_->next_;
      last_ = last_->next_ = a;
   }
   else
      last_ = a->next_ = a;
}

int TIntList::pop()
{
   assertStr(last_ != 0, "TIntList::pop : nothing in list to pop");
   int retval;
  
   TIntItem* f = last_->next_;
   if (f == last_)
    last_ = 0;
   else 
      last_->next_ = f->next_;
   
   retval = f->val_;
   delete f;
   return retval;
}
 
void TIntList::remove(TIntItem* it)
{
   TIntItem* before = it;
  
   while (before->next_ != it) before=before->next_;
   before->next_ = it->next_;
   if (last_ == it) last_ = before;
   if (before == it) last_ = 0;
   delete it;
}

void TIntList::insert_after(int val, TIntItem* it)
{
   TIntItem* a = new TIntItem(val);
   a->next_ = it->next_;
   it->next_ = a;
   if (it == last_) last_ = a;
}
 
void TIntList::print(ostream& os) const
{
   int first = TTRUE;
   if(!empty()) {
      for(TIntItem* ptr = last_->next_;; ptr = ptr->next_) {
   if(first) first = TFALSE;
   else      os << " , ";
   os << ptr->val_;
   if (ptr == last_) 
      break;
      }
   }
}

 
ostream& operator<< (ostream& os, const TIntList& li) 
{
   li.print(os); 
   return os; 
}


