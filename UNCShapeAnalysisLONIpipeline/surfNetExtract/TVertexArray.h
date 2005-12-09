// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertexArray
 *
 ****************************************************************************
 *
 *  File         :  TVertexArray.h
 *  Type         :  
 *  Purpose      :  container for TVertex
 *
 ****************************************************************************
 *                 
 *  started      :  20 Jan 95     Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#ifndef TVERTEXARRAY_H
#define TVERTEXARRAY_H

#include <iostream.h>
#include <stdlib.h>

#include "util.h"
#include "TVertex.h"

enum { not_found = -1 };

class TVertexArray {
public: 
   TVertexArray(int sz = 10)           { init(0, sz); }
   TVertexArray(TVertex *ar, int sz)    { init(ar, sz); }
   TVertexArray(const TVertexArray& iA) { init(iA.ia_, iA.size_); }
   ~TVertexArray()         { delete [] ia_; }

   TVertexArray& operator= (const TVertexArray&);
   int size()               { return size_; }
   int length()              { return size_; }

   TVertex& operator[] (int ix)           
                      { assertStr((ix>=0) && (ix<size_), "TVertexArray index violation"); return ia_[ix]; }
   const TVertex& operator[] (int ix) const
                      { assertStr((ix>=0) && (ix<size_), "TVertexArray index violation"); return ia_[ix]; }

   int find(TVertex) const;

   void print(ostream& = cout) const;
   friend ostream& operator<< (ostream& os, const TVertexArray& ar);

protected:
   void init(const TVertex*, int);

   int size_;  
   TVertex* ia_;
};

#endif
