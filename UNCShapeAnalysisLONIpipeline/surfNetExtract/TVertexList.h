// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertexList
 *
 ****************************************************************************
 *
 *  File         :  TVertexList.h
 *  Type         :  
 *  Purpose      :  list of TVertex objects, This is used, if you do not know
 *                  in advance how many vertices you get (e.g. when generating
 *                  the surface net data structure). As soon as you have a 
 *                  complete structure in the list, copy it to a TVertexArray
 *
 ****************************************************************************
 *                 
 *  started      :  20 Jan 95     Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#ifndef TVERTEXLIST_H
#define TVERTEXLIST_H

#include <iostream.h>

#include "util.h"
#include "TVertex.h"

class TVertexItem {
public:
   TVertexItem(TVertex val, TVertexItem* next = 0) : val_(val), next_(next) { }

   TVertex      val_;
   TVertexItem*  next_;
}; 

class TVertexList {
public:
   TVertexList() : last_(0) { }
   ~TVertexList();
  
   TVertexItem*  succ(TVertexItem* it) const    { return it == last_ ? 0 : it->next_ ; }
   TVertexItem*  cyclic_succ(TVertexItem* it) const   { return it->next_ ; }
   TVertexItem*  first() const         { return last_ == 0 ? 0 : last_->next_ ; }
   TVertexItem*  last() const         { return last_ ; }
   TVertexItem*  search(TVertex val) const;
  
   int size()   const;
   int length() const          { return size(); }
   int empty()  const          { return last_ == 0 ? TTRUE : TFALSE; }
   int contains(TVertex val) const;
    
   void insert(TVertex val);
   void append(TVertex val);
   TVertex pop();
   void push(TVertex val)           { insert(val); };
   void insert_after(TVertex val, TVertexItem* it);
   void remove(TVertexItem* it);

   void print(ostream& = cout) const;
   friend ostream& operator<< (ostream& os, const TVertexList& li);
  
private:
  TVertexItem* last_;
};


#define forall_vertexlistitems(a,l) for (a = (l).first(); a; a = (l).succ(a) )


#endif
