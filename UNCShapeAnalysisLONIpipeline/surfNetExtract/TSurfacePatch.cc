// -*- C++ -*-
/****************************************************************************
 *
 *  class TSurfacePatch
 *
 ****************************************************************************
 *
 *  File         :  TSurfacePatch.cc
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


#include "TSurfacePatch.h"
#include "TGeometryCreator.h"
#include "TIntList.h"


TSurfacePatch::TSurfacePatch(int L, GEOMedit_list* editlist) :
   sn_(0), oberGebiet_(0), surfintset_(0), editlist_(editlist), patch_(0), numholes_(0)
{ 
   patch_ = new TVertexArray(L);  
}

TSurfacePatch::TSurfacePatch(TSurfaceNet* sn, TGebiet* oberGebiet, GEOMedit_list* editlist) :
   sn_(sn), oberGebiet_(oberGebiet), surfintset_(0), editlist_(editlist), patch_(0), numholes_(0)
{

}

void TSurfacePatch::handleBorder(TGebiet* gebiet, int& ipatch)
{
   int i, k, newcount, dbg;
   int* borderarray = 0;
   int* bt = 0;
   int L = sn_->length();
   int* gebietID = gebiet->borderID_;
   TIntList* gebietBorderList = gebiet->borderlist_;
   TVertex v;

   try {

      // make array of border indices for speed
      int BL = gebietBorderList->length();

      borderarray = new int[BL];
      bt = new int[L];
      for (i=0; i<L; ++i)
   bt[i] = -1;

      i = 0;

      TIntItem* intitem;
      forall_intlistitems(intitem, *gebietBorderList) {
   borderarray[i] = intitem->val_;
   bt[intitem->val_] = i++;
      }

      for (i=0; i < L; ++i) {
   // the following statement determines, if we are on the gebiet's border
   if ( gebietID[i] > 0 ) {

      // at the  border, we must delete now invalid references in the neighbor list
      v = sn_->GetVertex(i);
      TVertex newv(v); // new vertex with same coords

      int pred = borderarray[(bt[i]-1+BL)%BL];
      int succ = borderarray[(bt[i]+1)%BL];
      assertStr(pred >= 0, "TSurfacePatch::handleBorder : invalid pred");
      assertStr(succ >= 0, "TSurfacePatch::handleBorder : invalid succ");

      int kpred;
      for (kpred=0, dbg=0; kpred<v.count_; ++kpred) {
         if (v.neighb_[kpred] == pred)
      break;
         assertStr(dbg < 14, "TSurfacePatch::handleBorder : didn't find pred");
         dbg++;
      }

      int ksucc;
      for (ksucc=0, dbg=0; ksucc<v.count_; ++ksucc) {
         if (v.neighb_[ksucc] == succ)
      break;
         assertStr(dbg < 14, "TSurfacePatch::handleBorder : didn't find succ");
         dbg++;
      }

      for (k=ksucc, newcount=0; ; k=(k+1)%v.count_) {
         assertStr( (gebietID[v.neighb_[k]] > 0) || (surfintset_->member(v.neighb_[k])),
           "TSurfacePatch::handleBorder : invalid reference");
         newv.neighb_[newcount++] = v.neighb_[k];
         if (k==kpred)
      break;
      }
      newv.count_ = newcount;
      
      (*patch_)[ipatch++] = newv;
   }
      }
      delete [] bt;
      delete [] borderarray;
   }

  catch ( AssertWarning& aw ) {
     delete [] bt;
     delete [] borderarray;

     throw(aw);
  }

  catch ( AssertFatal& af ) {
     delete [] bt;
     delete [] borderarray;

     throw(af); 
  }

}

void TSurfacePatch::extract_patch()
{
   int* rt = 0;
   TIntset* borderintset = 0;

   try {
      assertStrWarning( oberGebiet_ != 0, "oberGebiet is NIL");
      assertStrWarning( oberGebiet_->gebietState_ == GALTERBORDER , 
         "TSurfacePatch::extract_patch : Tried to extract from unmature OberGebiet");

      int i, j, k, dbg;
      TVertex v;
      char progressMessage[80];
      int L = sn_->length();
      surfintset_ = new TIntset(L);

      // the following three id arrays of length L determine the border of our patch
      int* oberID = oberGebiet_->borderID_;
      borderintset = new TIntset(L);
      numholes_ = 0;

      for ( i=0; i<L; ++i )
   if ( oberID[i]>0 )
      (*borderintset) += i;

      // queue for BFS below
      TIntList queue;

      // we push as start indices those vertices onto the queue, which are on the left hand side of the border
      // (the border, as we get it, is in counterclockwise direction)
      // as we spread only over the direct neighbors below in the breadth first search, this scheme has been set up
      // to handle narrow "bridges" of the patch, that the BFS cannot pass
      TIntList* oberBorderList = oberGebiet_->borderlist_;
      TIntItem *it, *succ_it;
      int succ;
      forall_intlistitems(it, (*oberBorderList)) {
   succ_it = oberBorderList->cyclic_succ(it);
   succ = succ_it->val_;
   TVertex v = sn_->GetVertex(it->val_);
   for (k=0; k<v.count_; ++k) {
      if (v.neighb_[k] == succ)
         break;
   }
   int possibleStart = v.neighb_[(k+2)%v.count_];
   if ( (! queue.contains(possibleStart) ) && ( ! borderintset->member(possibleStart) ) )
      queue.push(possibleStart);
      }

      int surfcount = 0;
      int queuecount = 0;
      while ( ! queue.empty() ) {
   int inx = queue.pop();

   queuecount++;
   if ((queuecount%500) == 0)
      cout << "queue length:" << queue.length() << endl;

   if (! borderintset->member(inx) ) {
      assertStrWarning(! surfintset_->member(inx), "Point is already in surface");
      (*surfintset_) += inx;

      surfcount++;
      if ((surfcount%100) == 0) {
         sprintf(progressMessage, "%s%i", "# in surface:", surfcount);
         report_progress(progressMessage, 50);
         // cout << "surface length:" << surfcount << endl;
      }
      assertStrWarning( surfcount < L, "stupid: Tried to add more surface vertices than are in surface net");
    
      TVertex v = sn_->GetVertex(inx);
      for (k=0; k < v.count_; k+=2) { // spread over direct neighbors
         if ( (! queue.contains(v.neighb_[k])) && (! surfintset_->member(v.neighb_[k])) ) {
      queue.push(v.neighb_[k]);
         }
      }
   }
      }

      // cout << "surface 0:" << endl << (*surfintset_) << endl;
      // cout << "border 0:" << endl << (*borderintset) << endl;

      // # of surface vertices
      int npatch;
      for (i=0,npatch=0;i<L;i++) 
   if ( surfintset_->member(i) || borderintset->member(i) )
      npatch++;

      // allocate data for patch
      patch_ = new TVertexArray(npatch);


      int ipatch;

      // handle interior
      for (i=0, ipatch=0; i<L; ++i) {
   if ( surfintset_->member(i) ) {
      // the interior of the surface stays the same as before
      v = sn_->GetVertex(i);
      (*patch_)[ipatch++] = v;
   }
      }

      // handle border of OberGebiet
      this->handleBorder(oberGebiet_, ipatch);

      // fill renumber array
      rt = new int[L];
      for (i=0; i<L; ++i)
   rt[i] = -1; // dbg

      // copy interior vertices
      for (i=0, ipatch=0;i<L;i++) {
   if ( surfintset_->member(i) )
      rt[i] = ipatch++;
      }

      // copy border vertices
      for (i=0; i<L; ++i)
   if ( oberID[i] )
      rt[i] = ipatch++;

      // renumber now
      for (i=0;i<npatch;i++) {
   TVertex rv = (*patch_)[i];
   for (k=0;k<rv.count_;k++) {
      assertStr(rt[rv.neighb_[k]] >= 0, "Reference to out-of-surface vertex while renumbering");
      rv.neighb_[k] = rt[rv.neighb_[k]];
   }
   (*patch_)[i] = rv;
      }

      delete [] rt;
      delete borderintset;
   }

   catch ( AssertWarning& aw ) {
      delete [] rt;
      delete borderintset;

      throw(aw);
   }

   catch ( AssertFatal& af ) {
      delete [] rt;
      delete borderintset;
      
      throw(af); 
   }
}

int TSurfacePatch::nface() const
{
   // count faces
   int nfaces = 0;
   for (int i=0; i<patch_->size(); ++i) {
      TVertex v = (*patch_)[i];
      for (int j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
         nfaces++;
   }
      }
   }
   return (nfaces);
}

int TSurfacePatch::nedge() const
{
   int nedges = 0;
   for (int i=0; i<patch_->size(); ++i) {
      TVertex v = (*patch_)[i];
      for ( int j=0; j < v.count_; j+=2)
   nedges++;
   }

   nedges /= 2;
   return (nedges);
}
  
int TSurfacePatch::GetIndex(const TPoint3D& pt) const
{
   for (int i=0; i<patch_->size(); ++i)
      if ((*patch_)[i].GetCoords3D() == pt)
   return i;

   return -1;
}

void TSurfacePatch::ShowVertices()
{
   // display surface vertices
#ifndef NO_AVS

   TGeometryCreator gm(editlist_);
   for (int i=0;i<patch_->length();++i) {
      TVertex v = (*patch_)[i];
      TPoint3D pt = v.GetCoords3D();
      gm.MakeSphere("patch", pt, 0.11, gBlue);
   }
#endif
}

void TSurfacePatch::CopyToAVS(int* outavs)
{
   int i, iavs;
   for (i=0,iavs=0; i<patch_->size(); ++i) {
      TVertex v = (*patch_)[i];
      outavs[iavs++] = v.wh_[0];
      outavs[iavs++] = v.wh_[1];
      outavs[iavs++] = v.wh_[2];
      outavs[iavs++] = v.count_;
      for (int k=0; k<14; k++)
   outavs[iavs++] = v.neighb_[k];
   }
}

void TSurfacePatch::CopyFromAVS(const int* inavs)
{
   int i, iavs;
   for (i=0,iavs=0; i<patch_->size(); ++i) {
      TVertex v;
      v.wh_[0] = inavs[iavs++];
      v.wh_[1] = inavs[iavs++];
      v.wh_[2] = inavs[iavs++];
      v.count_ = inavs[iavs++];
      for (int k=0; k<14; k++)
   v.neighb_[k] = inavs[iavs++];
      (*patch_)[i] = v;
   }
}

void TSurfacePatch::ShowLabels()
{
#ifndef NO_AVS
    for (int i=0;i<this->length();++i) {
      TVertex v = (*patch_)[i];
      TPoint3D pt = v.GetCoords3D();
      TGeometryCreator gm(editlist_);
      gm.MakeLabel("lab", i, pt, gBlue);
   }  
#endif
}

void TSurfacePatch::print(ostream& os) const
{
  os << "{" << endl;
  (*patch_).print(os);
  os << "}" << endl;
}

  
ostream& operator<< (ostream& os, const TSurfacePatch& sp)
{
  sp.print(os);
  return os;
}
