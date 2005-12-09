// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertexList
 *
 ****************************************************************************
 *
 *  File         :  TVertexList.cc
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
#include "TVertexList.h"

TVertexList::~TVertexList() {
   if (last_) {
      TVertexItem* ptr = last_->next_;
      last_->next_ = 0;
      while (ptr) {
   TVertexItem* tmp = ptr;
   ptr = ptr->next_;
   delete tmp;
      }
   }
}

TVertexItem* TVertexList::search(TVertex val) const
{
   TVertexItem* p = 0;
   forall_vertexlistitems(p, *this)
      if (p->val_ == val)
   break;
  
   return p;
}

int TVertexList::contains(TVertex val) const
{
   TVertexItem* p = 0;
   forall_vertexlistitems(p, *this)
      if (p->val_ == val)
   return 1;
  
   return 0;
}
 
int TVertexList::size() const
{
   int i = 0;
   TVertexItem* a;
   forall_vertexlistitems(a, *this)
      i++;
  
   return i;
}

void TVertexList::insert(TVertex val) {
   TVertexItem* a = new TVertexItem(val);
   if (last_)
      a->next_ = last_->next_;
   else
      last_ = a;
   last_->next_ = a;
}

void TVertexList::append(TVertex val)
{
   TVertexItem* a = new TVertexItem(val); 
   if (last_) {
      a->next_ = last_->next_;
      last_ = last_->next_ = a;
   }
   else
      last_ = a->next_ = a;
}

TVertex TVertexList::pop()
{
   assertStr(last_ != 0, "TVertexList::pop : nothing in list to pop");
  
   TVertexItem* f = last_->next_;
   if (f == last_)
    last_ = 0;
   else 
      last_->next_ = f->next_;

   static TVertexItem * last = NULL;
   if (last) delete last;

   last = f;

   return f->val_;
}
 
void TVertexList::remove(TVertexItem* it)
{
   TVertexItem* before = it;
  
   while (before->next_ != it) before=before->next_;
   before->next_ = it->next_;
   if (last_ == it) last_ = before;
   if (before == it) last_ = 0;
   delete it;
}

void TVertexList::insert_after(TVertex val, TVertexItem* it)
{
   TVertexItem* a = new TVertexItem(val);
   a->next_ = it->next_;
   it->next_ = a;
   if (it == last_) last_ = a;
}
 
void TVertexList::print(ostream& os) const
{
   int first = TTRUE;
   if(!empty()) {
      for(TVertexItem* ptr = last_->next_;; ptr = ptr->next_) {
   if (first) first = TFALSE;
   else      os << ", \n";
   os << ptr->val_;
   if (ptr == last_) 
      break;
      }
   }
   os << endl;
}

 
ostream& operator<< (ostream& os, const TVertexList& li) 
{
   li.print(os); 
   return os; 
}


