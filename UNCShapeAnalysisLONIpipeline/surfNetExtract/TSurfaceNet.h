// -*- C++ -*-
/****************************************************************************
 *
 *  class TSurfaceNet
 *
 ****************************************************************************
 *
 *  File         :  TSurfaceNet.h
 *  Type         :  
 *  Purpose      :  stores the surface net data structure
 *
 ****************************************************************************
 *                 
 *  started      :  1  Jan 95     Robert Schweikert
 *  last change  :  22 Feb 95
 *
 ****************************************************************************/
 
#ifndef TSURFACENET_H
#define TSURFACENET_H

#include <iostream.h>

#include "util_avs.h"
#include "TVertex.h"
#include "TVoxelVolume.h"
#include "TVertexArray.h"
#include "TPoint3D.h"

class TSurfaceNet {
public:
#ifndef NO_AVS
   TSurfaceNet(int L, GEOMedit_list* editlist);
   TSurfaceNet(TVoxelVolume& vol, int thresh, GEOMedit_list* editlist);
   TSurfaceNet(istream& is, GEOMedit_list* editlist);
#endif
   TSurfaceNet(TVoxelVolume& vol, int thresh);
   ~TSurfaceNet()              { delete va_; }

   int length() const                     { return va_->length(); }
   int size() const                       { return va_->length(); }
   int nface() const;

   // gets first vertex that fits
   int GetIndex(const TPoint3D& pt) const;

   // given index, get corresponding vertex
   TVertex GetVertex(int index) const     { return (*va_)[index]; }

   // distance between two vertices
   float dist(int inx1, int inx2)         { return vdist( (*va_)[inx1], (*va_)[inx2] ); }
  
   void CopyToAVS(int* outavs);
   void CopyFromAVS(const int* inavs);

   // graphics
   void ShowLabels();
  
   // output
   void read(istream& is);
   void print(ostream& os) const;
   friend ostream& operator<< (ostream& os, const TSurfaceNet& sn);

private:
   // this creates a TNetExtractor object to do the dirty work
   void vol2net(TVoxelVolume& vol);

   TVertexArray*    va_;                  // vertex data
   unsigned char    thresh_;              // threshold value

#ifndef NO_AVS
   GEOMedit_list*   editlist_;            // for ShowLabels()
#endif NO_AVS
};

#endif
