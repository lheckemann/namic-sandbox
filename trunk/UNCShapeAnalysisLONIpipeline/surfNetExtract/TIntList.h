// -*- C++ -*-
/****************************************************************************
 *
 *  class TIntList
 *
 ****************************************************************************
 *
 *  File         :  TIntList.h
 *  Type         :  
 *  Purpose      :  singly-linked list of integers
 *
 ****************************************************************************
 *                 
 *  started      :  20 Jan 95     Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#ifndef TINTLIST_H
#define TINTLIST_H

#include <iostream.h>
#include "util.h"

class TIntItem {
public:
   TIntItem(int val, TIntItem* next = 0) : val_(val), next_(next) { }

   int     val_;
   TIntItem*   next_;
}; 

class TIntList {
public:
   TIntList() : last_(0) { }
   ~TIntList();
  
   TIntItem*  succ(TIntItem* it) const    { return it == last_ ? 0 : it->next_ ; }
   TIntItem*  cyclic_succ(TIntItem* it) const   { return it->next_ ; }
   TIntItem*  first() const         { return last_ == 0 ? 0 : last_->next_ ; }
   TIntItem*  last() const         { return last_ ; }
   TIntItem*  search(int val) const;
  
   int size()   const;
   int length() const          { return size(); }
   int empty()  const          { return last_ == 0 ? TTRUE : TFALSE; }
   int contains(int val) const;
    
   void insert(int val);
   void append(int val);
   int pop();
   void push(int val)           { insert(val); };
   void insert_after(int val, TIntItem* it);
   void remove(TIntItem* it);

   void print(ostream& = cout) const;
   friend ostream& operator<< (ostream& os, const TIntList& li);
  
private:
  TIntItem* last_;
};


#define forall_intlistitems(a,l) for (a = (l).first(); a; a = (l).succ(a) )


#endif
