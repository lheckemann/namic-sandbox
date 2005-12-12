// -*- C++ -*-
/****************************************************************************
 *
 *  class TSurfacePatch
 *
 ****************************************************************************
 *
 *  File         :  TSurfacePatch.h
 *
 *  Purpose      :  given a TGebiet (represents border polygon), 
 *                  spread over surface to determint actual patch
 *
 ****************************************************************************
 *                 
 *  started      :  4 Jan 95     Robert Schweikert
 *  last change  :  21 Feb 95
 *
 ****************************************************************************/
 
#ifndef TSURFACEPATCH_H
#define TSURFACEPATCH_H

#include <iostream.h>

#ifdef NO_AVS
typedef void GEOMedit_list;
#else
#include "util_avs.h"
#endif NO_AVS
#include "TPoint3D.h"
#include "TSurfaceNet.h"
#include "TIntList.h"
#include "TIntset.h"
#include "TVertexArray.h"
#include "TGebiet.h"

class TSurfacePatch {
public:
   TSurfacePatch(int L, GEOMedit_list* editlist);
   TSurfacePatch(TSurfaceNet* sn, TGebiet* oberGebiet, GEOMedit_list* editlist);
   ~TSurfacePatch()            { delete patch_; delete surfintset_; }

   // extract the patch
   void handleBorder(TGebiet* gebiet, int& ipatch);
   void extract_patch();

   int length() const                         { return patch_->length(); }
   int size() const                           { return patch_->length(); }
   int nvert() const                          { return patch_->length(); }
   int nface() const;
   int nedge() const;

   int GetIndex(const TPoint3D& pt) const;
   TVertex GetVertex(int index)               { return (*patch_)[index]; }

   TVertex&       operator[] (int i)          { return (*patch_)[i]; }
   const TVertex& operator[] (int i) const    { return (*patch_)[i]; }

   TPoint3D GetCoords3D(int index)            { return (*patch_)[index].GetCoords3D(); }

   float dist(int inx1, int inx2)             { return vdist( (*patch_)[inx1], (*patch_)[inx2] ); }

   // nface() doesn't count the surrounding "face"
   int SatisfiesEuler()                       
       { return ( this->nvert() - this->nedge() + this->nface() + numholes_ == 1 ); }
   void SetNumHoles(int numholes)             { numholes_ = numholes; }

   // AVS graphics
   void ShowVertices();
   void ShowLabels();

   void CopyToAVS(int* outavs);
   void CopyFromAVS(const int* inavs);

   void print(ostream& os) const;
   friend ostream& operator<< (ostream& os, const TSurfacePatch& sn);

private:
   TSurfaceNet*   sn_;                       // pointer to net representing the whole object
   TGebiet*       oberGebiet_;               // oberGebiet holds information about the border polygon
   TIntset*       surfintset_;               // used during extract_patch() to hold interior vertices


   TVertexArray*  patch_;                    // the actual vertex data

   int            numholes_;                 // number of holes in Patch, 
               // used in SatisfiesEuler method to adjust for holes

   GEOMedit_list* editlist_;                 // for ShowVertices() and ShowLabels()
};

#endif
