// -*- C++ -*-
/****************************************************************************
 *
 *  class TPoint3D
 *
 ****************************************************************************
 *
 *  File         :  TPoint3D.h
 *  Type         :  
 *  Purpose      :  points with integer and float coordinates
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#ifndef TPOINT3D_H
#define TPOINT3D_H

#include <math.h>

#define SQUARE(x) ((x)*(x))

class TPoint3D {
public:
   TPoint3D()              { x_ = 0;     y_ = 0;    z_ = 0;     }
   TPoint3D(int x, int y, int z)  { x_ = x;     y_ = y;     z_ = z;     }
   TPoint3D(int wh[3])            { x_ = wh[0]; y_ = wh[1]; z_ = wh[2]; }

   void Set(int x, int y, int z)        { x_ = x;     y_ = y;     z_ = z; }

   friend int operator== (const TPoint3D& p1, const TPoint3D& p2)
                                        { return ((p1.x_ == p2.x_) && (p1.y_ == p2.y_) && (p1.z_ == p2.z_)) ; }
   friend int operator!= (const TPoint3D& p1, const TPoint3D& p2)
                { return (!(p1 == p2)); }
   friend int operator<= (const TPoint3D& p1, const TPoint3D& p2)
                { return ((p1.x_ <= p2.x_) && (p1.y_ <= p2.y_) && (p1.z_ <= p2.z_)) ;  }
   friend int operator>= (const TPoint3D& p1, const TPoint3D& p2)
                { return ((p1.x_ >= p2.x_) && (p1.y_ >= p2.y_) && (p1.z_ >= p2.z_)) ;  }

   friend float pdist(const TPoint3D& p1, const TPoint3D& p2) 
           { return sqrt( SQUARE(p1.x_-  p2.x_) + SQUARE(p1.y_ - p2.y_) + SQUARE(p1.z_ - p2.z_) ); }

  
   int x_, y_, z_;
};


class TPointFloat3D {
public:
   TPointFloat3D()                  { x_ = 0;     y_ = 0;     z_ = 0;     }
   TPointFloat3D(float x, float y, float z)   { x_ = x;     y_ = y;     z_ = z;     }
   TPointFloat3D(float wh[3])                { x_ = wh[0]; y_ = wh[1]; z_ = wh[2]; }
   TPointFloat3D(int wh[3])                { x_ = wh[0]; y_ = wh[1]; z_ = wh[2]; }

   void Set(float x, float y, float z)        { x_ = x;     y_ = y;     z_ = z;     }

   friend int operator== (const TPointFloat3D& p1, const TPointFloat3D& p2)
                                        { return ((p1.x_ == p2.x_) && (p1.y_ == p2.y_) && (p1.z_ == p2.z_)) ; }
   friend int operator!= (const TPointFloat3D& p1, const TPointFloat3D& p2)
                { return (!(p1 == p2)); }
   friend int operator<= (const TPointFloat3D& p1, const TPointFloat3D& p2)
                { return ((p1.x_ <= p2.x_) && (p1.y_ <= p2.y_) && (p1.z_ <= p2.z_)) ;  }
   friend int operator>= (const TPointFloat3D& p1, const TPointFloat3D& p2)
                { return ((p1.x_ >= p2.x_) && (p1.y_ >= p2.y_) && (p1.z_ >= p2.z_)) ;  }

   friend float pdist(const TPointFloat3D& p1, const TPointFloat3D& p2) 
           { return sqrt( SQUARE(p1.x_-  p2.x_) + SQUARE(p1.y_ - p2.y_) + SQUARE(p1.z_ - p2.z_) ); }

  
   float x_, y_, z_;
};


#endif
