// -*- C++ -*-
/****************************************************************************
 *
 *  class TIntset
 *
 ****************************************************************************
 *
 *  File         :  TIntset.h
 *  Type         :  
 *  Purpose      :  a set of integers 
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 

#ifndef TINTSET_H
#define TINTSET_H

enum { OFF, ON };

#include <iostream.h>

class TIntset {
public:
   TIntset(int size = 32, int = OFF);           // size determines the largest integer
                  // that TIntset needs to acommodate
                                                // set ranges from 0..(size-1)
   TIntset(const TIntset& ti);
   ~TIntset()                       { delete [] bv_; }

   void operator+= (int pos);                   // add pos to set
   void operator-= (int pos);                   // delete pos from set
   TIntset operator& (const TIntset&) const;
   TIntset operator| (const TIntset&) const;
   int member(int pos) const;                    
   void reset(); 

   int length()                                 { return size_; }
   int size()                                   { return size_; }
   
   friend ostream& operator<< (ostream&, const TIntset&);

private:
   int getOffset(int pos) const     { return( pos % 32 ); }
   int getIndex(int pos) const             { return( pos / 32 ); }

   int wordWidth_;
   int size_;
   unsigned int* bv_;
};

#endif
