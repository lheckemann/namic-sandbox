// -*- C++ -*-
/****************************************************************************
 *
 *  class TList
 *
 ****************************************************************************
 *
 *  File         :  TList.h
 *  Type         :  
 *  Purpose      :  a generic C++ List of some objects
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#ifndef TLIST_H
#define TLIST_H

#include <iostream.h>
#include "util.h"

template<class Type>
class TItem {
public:
  TItem(Type val, TItem* next = 0) : val_(val), next_(next) { }

  Type       val_;
  TItem<Type>*   next_;
}; 

template<class Type>
class TList {
public:
  TList() : last_(0) { }
  ~TList();
  
  TItem<Type>*  succ(TItem<Type>* it) const    { return it == last_ ? 0 : it->next_ ; }
  TItem<Type>*  cyclic_succ(TItem<Type>* it) const   { return it->next_ ; }
  TItem<Type>*  first() const         { return last_ == 0 ? 0 : last_->next_ ; }
  TItem<Type>*  last() const         { return last_ ; }
  TItem<Type>*  search(Type val) const;
  
  int size()   const;
  int length() const          { return size(); }
  int empty()  const          { return last_ == 0 ? TTRUE : TFALSE; }
        int contains(Type val) const;
    
  void insert(Type val);
  void append(Type val);
  Type pop();
  void push(Type val)           { insert(val); };
  void insert_after(Type val, TItem<Type>* it);
  void remove(TItem<Type>* it);

  void print(ostream& = cout) const;
  
private:
  TItem<Type>* last_;
};

#define forall_listitems(a,l) for (a = (l).first(); a; a = (l).succ(a) )


#endif
