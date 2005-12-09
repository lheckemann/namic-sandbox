// -*- C++ -*-
/****************************************************************************
 *
 *  class TSurfaceNet
 *
 ****************************************************************************
 *
 *  File         :  TSurfaceNet.cc
 *
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95    Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 
#include "TVertexList.h"
#include "TVertex.h"
#include "TSurfaceNet.h"
#include "TNetExtractor.h"
#include "TGeometryCreator.h"

#ifndef NO_AVS
TSurfaceNet::TSurfaceNet(int L, GEOMedit_list* editlist) : editlist_(editlist) 
{
   // allocate data for a surface net (actual surface net data should be fetched with a CopyFromAVS call)
   thresh_ = 0;
   va_ = new TVertexArray(L);
}

TSurfaceNet::TSurfaceNet(TVoxelVolume& vol, int thresh, GEOMedit_list* editlist)
{
   // build a surface net using a TNetExtractor object
   assertStr( ( thresh >= 0 ) && ( thresh <= 255 ), "TSurfaceNet::TSurfaceNet : wrong thresh value");
   thresh_ = (unsigned char) thresh;
   va_ = 0;
   editlist_ = editlist;
   this->vol2net(vol);
}

TSurfaceNet::TSurfaceNet(istream& is, GEOMedit_list* editlist)
{
   // build a surface net from input stream
   thresh_ = 0;
   va_ = 0;
   editlist_ = editlist;
   this->read(is);
}
#endif

TSurfaceNet::TSurfaceNet(TVoxelVolume& vol, int thresh)
{
   // build a surface net using a TNetExtractor object
   assertStr( ( thresh >= 0 ) && ( thresh <= 255 ), "TSurfaceNet::TSurfaceNet : wrong thresh value");
   thresh_ = (unsigned char) thresh;
   va_ = 0;
   this->vol2net(vol);
#ifndef NO_AVS
   editlist_ = editlist;
#endif
}

void TSurfaceNet::read(istream& is)
{
  char ch;
  int wh[3];
  int count;
  int neighb[14];
  TVertexList vl;
  
  is >> ch;
  assertStr(ch == '{', "TSurfaceNet::read : '{' expected");
  for (;;) {
    is >> ch;
    if (ch == '}')
      break;
      
    is >> ch; is >> wh[0]; 
    is >> ch; is >> wh[1];
    is >> ch; is >> wh[2];
    is >> ch; is >> ch; is >> ch;
    for (count = 0; ; count++) {
      is >> neighb[count];
      is >> ch;
      if (ch == '}') {
        count++;
        break;
      }
    }
    TVertex v(wh,count,neighb);
    vl.append(v);
    assertStr(ch == '}', "TSurfaceNet::read : '}' expected");
    is >> ch; // '}'
    is >> ch; // ','
    if (ch == '}')
      break;
  }
  int i = 0;
  int L = vl.length();
  va_ = new TVertexArray(L);
  while (! vl.empty() ) {
     TVertex v = vl.pop();
     (*va_)[i] = v;
     ++i;
  }
}

void TSurfaceNet::print(ostream& os) const
{
  os << "{" << endl;
  (*va_).print(os);
  os << "}" << endl;
}
  
int TSurfaceNet::GetIndex(const TPoint3D& pt) const
{
   for (int i=0; i<va_->size(); ++i)
      if ((*va_)[i].GetCoords3D() == pt)
   return i;

   return -1;
}

int TSurfaceNet::nface() const
{
   // count faces
   int nfaces = 0;
   for (int i=0; i<va_->size(); ++i) {
      TVertex v = (*va_)[i];
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
  
void TSurfaceNet::CopyToAVS(int* outavs)
{ 
   int i, iavs;
   for (i=0,iavs=0; i<va_->size(); ++i) {
      TVertex v = (*va_)[i];
      outavs[iavs++] = v.wh_[0];
      outavs[iavs++] = v.wh_[1];
      outavs[iavs++] = v.wh_[2];
      outavs[iavs++] = v.count_;
      for (int k=0; k<14; k++)
   outavs[iavs++] = v.neighb_[k];
   } 
}

void TSurfaceNet::CopyFromAVS(const int* inavs)
{ 
   int  i, iavs;
   for (i=0,iavs=0; i<va_->size(); ++i) {
      TVertex v;
      v.wh_[0] = inavs[iavs++];
      v.wh_[1] = inavs[iavs++];
      v.wh_[2] = inavs[iavs++];
      v.count_ = inavs[iavs++];
      for (int k=0; k<14; k++)
   v.neighb_[k] = inavs[iavs++];
      (*va_)[i] = v;
   } 
}

void TSurfaceNet::vol2net(TVoxelVolume& vol)
{
   vol.SetBoundary(0);
   vol.Binarize(thresh_);

   TNetExtractor ne(&vol);
   TVertexList* vl;
   vl = ne.extractnet();
   int i = 0;
   int L = vl->length();
   va_ = new TVertexArray(L);
   while (! vl->empty() ) {
      TVertex v = vl->pop();
      (*va_)[i] = v;
      ++i; 
   }
   delete vl;
}

void TSurfaceNet::ShowLabels()
{
    for (int i=0;i<this->length();++i) {
      TVertex v = (*va_)[i];
      TPoint3D pt = v.GetCoords3D();
#ifndef NO_AVS
      TGeometryCreator gm(editlist_);
      gm.MakeLabel("lab", i, pt, gRed);
#endif
   }  
}
  
ostream& operator<< (ostream& os, const TSurfaceNet& sn)
{
  sn.print(os);
  return os;
}
