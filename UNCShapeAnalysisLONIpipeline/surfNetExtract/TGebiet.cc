// -*- C++ -*-
/****************************************************************************
 *
 *  class TGebiet
 *
 ****************************************************************************
 *
 *  File         :  TGebiet.cc
 *
 *  Purpose      :  given a surface net, determine a border polygon on it
 *
 ****************************************************************************
 *                 
 *  started      :  26 Jan 95     Robert Schweikert
 *  last change  :  21 Feb 95
 *
 ****************************************************************************/

#include <float.h>

#include "TGebiet.h"
#include "TGeometryCreator.h"

TGebiet::TGebiet(TSurfaceNet* sn, GEOMedit_list* editlist, char* linename, float linecolor[3]) : 
      sn_(sn), editlist_(editlist), gebietState_(GFIRSTCLICK), baseState_(BUNDEF), 
      start_(-1), prev_(-1), next_(-1),  nextID_(1)
{ 
   int L = sn_->size();
   borderID_ = new int[L]; 
   for (int i=0; i<L; ++i)
      borderID_[i] = 0;
  
   borderlist_ = new TIntList(); 

   linecolor_[0] = linecolor[0];
   linecolor_[1] = linecolor[1];
   linecolor_[2] = linecolor[2];

   strcpy(linename_, linename);

   // this removes existing selection
   TGeometryCreator gm(editlist_);
   gm.AddEmptyObject(linename_);
   gm.AddEmptyObject("clickPoints");
}

TGebiet::TGebiet(TSurfaceNet* sn, istream& is, GEOMedit_list* editlist, char* linename, float linecolor[3]) : 
      sn_(sn), editlist_(editlist), gebietState_(GALTERBORDER), baseState_(BUNDEF), 
      start_(-1), prev_(-1), next_(-1),  nextID_(1)
{
   int L = sn_->size();
   borderID_ = new int[L]; 
   for (int i=0; i<L; ++i)
      borderID_[i] = 0;
  
   borderlist_ = new TIntList(); 

   linecolor_[0] = linecolor[0];
   linecolor_[1] = linecolor[1];
   linecolor_[2] = linecolor[2];

   strcpy(linename_, linename);

   // this removes existing selection
   TGeometryCreator gm(editlist_);
   gm.AddEmptyObject(linename_);
   gm.AddEmptyObject("clickPoints");

   // read in segments and border
   char tmp[80];
   char ch;
   int val;

   // read click points
   is.get(tmp, 80, '\n');
   is >> ch;
   assertStrWarning( ch == '{', "TGebiet::TGebiet : '{' expected (click points)");
   for (;;) {
     is >> val;
     TSegmentInfo seginfo(si_);
     seginfo.inx_ = val;
     segments.append(seginfo);
     is >> ch; // ','
     assertStrWarning( (ch==',') || (ch == '}'), "TGebiet::TGebiet : comma or '}' expected (click points)");
     if (ch == '}')
  break;
   }

   // read border points
   is >> tmp;
   is.get(tmp, 80, '\n');
   is >> ch;
   assertStrWarning( ch == '{', "TGebiet::TGebiet : '{' expected (border)");
   for (;;) {
     is >> val;
     borderlist_->append(val);
     is >> ch; // ','
     assertStrWarning( (ch==',') || (ch == '}'), "TGebiet::TGebiet : comma or '}' expected (border)");
     if (ch == '}')
  break;
   }
}


void TGebiet::addToBorder(const TPoint3D& pt)
{
   try {
      next_ = sn_->GetIndex(pt);
      assertStr( next_ != -1, "This point is not in the surface net data structure");

      if ( gebietState_ == GFIRSTCLICK ) {
   report_progress("First Point", 50);

   gebietState_ = GDETBORDER;
   start_ = prev_ = next_;

   TSegmentInfo seginfo(si_);
   seginfo.inx_ = start_;

   segments.append(seginfo);

   this->showClickPoints();
      }

      else if ( next_ !=  start_ ) {
   report_progress("Adding Point", 50);

   // use preferences as in si_:
   TSegmentInfo nextinfo(si_);
   nextinfo.inx_ = next_;

   TSegmentInfo previnfo(prev_);

   segments.append(nextinfo);
   this->extractBorder(previnfo, nextinfo);

   this->drawBorder();
   this->showClickPoints();

   prev_ = next_;

      }

      else if ( next_ == start_ ) {
   report_progress("Last Point", 50);

   TSegmentInfo nextinfo(si_);
   nextinfo.inx_ = next_;

   TSegmentInfo previnfo(prev_);

   //segments.append(nextinfo);
   this->extractBorder(previnfo, nextinfo);

   this->drawBorder();
   this->showClickPoints();

   gebietState_ = GALTERBORDER;
      }
   }

   catch (DifferentObjects& dob) {
      TSegmentInfoItem* removeitem = segments.last();
      segments.remove(removeitem);

      throw(dob);
   }

   catch (AmbiguousPath& ap) {
      TSegmentInfoItem* removeitem = segments.last();
      segments.remove(removeitem);

      throw(ap);
   }
   
}
    

void TGebiet::setBasePoint(const TPoint3D& pt)
{
   assertStr(gebietState_ == GALTERBORDER, "TGebiet::setBasePoint : Tried to edit unmature TGebiet");
   int base = sn_->GetIndex(pt);
   assertStr( base != -1, "TGebiet::setBasePoint : This point is not in the surface net data structure");
   TSegmentInfo baseinfo(base);
   TSegmentInfoItem* it;
   it = segments.search(baseinfo);
   assertStrWarning( it != 0, "TGebiet::setBasePoint : This point is not in the segments list");
   
   baseinfo_ = it->val_;
}


void TGebiet::addAfterBasePoint(const TPoint3D& pt)
{
   TSegmentInfoItem* baseitem;

   try {
      assertStr( gebietState_ == GALTERBORDER, "TGebiet::addAfterBasePoint : Tried to edit unmature TGebiet");
      int inx = sn_->GetIndex(pt);
      assertStr( inx != -1, "TGebiet::addArfterBasePoint : This point is not in the surface net data structure");
   
      // search list
      baseitem = segments.search(baseinfo_);

      // make new SegmentInfo with same prefs base had
      TSegmentInfo inxinfo(baseinfo_);
      inxinfo.inx_ = inx;

      segments.insert_after(inxinfo, baseitem);

      this->extractBorder();
      this->drawBorder();
      this->showClickPoints();

      baseinfo_ = inxinfo;
   }
 
   catch ( DifferentObjects& dob ) {
      // if an error occurred, restore segments list as before
      TSegmentInfoItem* rmitem = segments.succ(baseitem);
      segments.remove(rmitem);

      throw(dob);
   }

   catch ( AmbiguousPath& ap ) {
      TSegmentInfoItem* rmitem = segments.succ(baseitem);
      segments.remove(rmitem);

      throw(ap);
   }
  
}

void TGebiet::deleteBasePoint()
{  
   TSegmentInfo saveinfo;
   TSegmentInfoItem* beforeitem;

   try {
      assertStr( gebietState_ == GALTERBORDER , "TGebiet::deleteBasePoint : Tried to edit unmature TGebiet");
      TSegmentInfoItem* baseitem = segments.search(baseinfo_);

      // save for undo
      saveinfo = baseitem->val_;
      beforeitem = segments.pred(baseitem);

      // delete
      segments.remove(baseitem);

      // recalculate
      this->extractBorder();
      this->drawBorder();
      this->showClickPoints();
   }

   catch (DifferentObjects& dob) {
      segments.insert_after(saveinfo, beforeitem);

      throw(dob);
   }

   catch (AmbiguousPath& ap) {
      segments.insert_after(saveinfo, beforeitem);

      throw(ap);
   }

}

void TGebiet::moveBasePoint(const TPoint3D& newpt)
{
   TSegmentInfoItem* baseitem;
   TSegmentInfo saveinfo;

   try {
      assertStr(gebietState_ == GALTERBORDER , "TGebiet::moveBasePoint : Tried to edit unmature TGebiet");
      int newinx = sn_->GetIndex(newpt);
      assertStr( newinx != -1, "TGebiet::moveBasePoint : This point is not in the surface net data structure");

      // search list
      baseitem = segments.search(baseinfo_);

      // new SegmentInfo with same prefs
      TSegmentInfo newinxinfo(baseinfo_);
      saveinfo = baseinfo_;
      newinxinfo.inx_ = newinx;

      baseitem->val_ = newinxinfo;

      this->extractBorder();
      this->drawBorder();
      this->showClickPoints();

      baseinfo_ = newinxinfo;
   }
 
   catch (DifferentObjects& dob) {
      baseitem->val_ = saveinfo;

      throw(dob);
   }

   catch (AmbiguousPath& ap) {
      baseitem->val_ = saveinfo;

      throw(ap);
   }
  
}

void TGebiet::recalcBaseSegment()
{
   TSegmentInfo saveinfo;
   TSegmentInfoItem* baseitem;

   try {

      assertStr(gebietState_ == GALTERBORDER , "TGebiet::recalcBaseSegment : Tried to edit unmature TGebiet");
      baseitem = segments.search(baseinfo_);

      saveinfo = baseitem->val_;

      TSegmentInfo seginfo(si_);
      seginfo.inx_ = saveinfo.inx_;

      baseitem->val_ = seginfo;

      this->extractBorder();
      this->drawBorder();
      this->showClickPoints();

      baseinfo_ = seginfo;
   }

   catch (DifferentObjects& dob) {
      baseitem->val_ = saveinfo;

      throw(dob);
   }

   catch (AmbiguousPath& ap) {
      baseitem->val_ = saveinfo;

      throw(ap);
   }
}

void TGebiet::drawBorder()
{
   TVertex v;
   TPointFloat3D pt;

   TIntItem* it;
   it = borderlist_->first();
   int first = it->val_;
   it = borderlist_->last();
   int last = it->val_;

   TGeometryCreator gm(editlist_);

   gm.BeginDisjointLine();
   forall_intlistitems(it, (*borderlist_)) {
      int binx = it->val_;
      // display border points
      v = sn_->GetVertex(binx);
      pt = v.GetCoordsFloat3D();
      gm.MakeSphere(linename_, pt, 0.05, linecolor_);
      gm.AddDisjointPoint(pt);
      if ( ( binx != first ) && ( binx != last ) )
   gm.AddDisjointPoint(pt);
   }

   if ( gebietState_ == GALTERBORDER ) {
      v = sn_->GetVertex(last);
      pt = v.GetCoordsFloat3D();   
      gm.AddDisjointPoint(pt);

      v = sn_->GetVertex(first);
      pt = v.GetCoordsFloat3D();   
      gm.AddDisjointPoint(pt);
   }

   gm.EndDisjointLine(linename_, linecolor_);
}
         
void TGebiet::drawBorder(int id)
{
   // just for debugging
   TGeometryCreator gm(editlist_);
   int L = sn_->size();
   for (int i=0; i<L; ++i) {
      if ( borderID_[i] == id ) {
   TVertex v = sn_->GetVertex(i);
   TPointFloat3D pt = v.GetCoordsFloat3D();
   gm.MakeSphere("BorderByID1", pt, 0.05, gBlue);
      }
   }
}

void TGebiet::drawBorderID()
{
   for (int i=1; i<nextID_; ++i)
      this->drawBorder(i);
}

void TGebiet::showClickPoints()
{
   TSegmentInfoItem* p = 0;
   TGeometryCreator gm(editlist_);

   forall_segmentinfolistitems(p, segments) {
      TVertex v = sn_->GetVertex(p->val_.inx_);
      TPointFloat3D pt = v.GetCoordsFloat3D();
      gm.MakeSphere("clickPoints", pt, 0.11, gRed);
   }     
}

void TGebiet::extractBorder()
{
   int* savedID;
   TIntList* savedList;

   try {
      savedID = borderID_;
      savedList = borderlist_;

      int progressCount = 0;
      int progressLength = segments.size();
      char progressMessage[80];

      // reset borderID_
      nextID_ = 1;
      int L = sn_->size();
      borderID_ = new int[L];
      for (int i=0; i<L; ++i)
      borderID_[i] = 0;

      // regenerate  borderlist
      borderlist_ = new TIntList();

      TSegmentInfoItem *from, *to;
  
      forall_segmentinfolistitems(from, segments) {
   to = segments.cyclic_succ(from);
   this->extractBorder(from->val_, to->val_);

   // show progress
   sprintf(progressMessage, "%s%i", "Segment:", progressCount );
   report_progress(progressMessage,progressLength?((progressCount*100)/progressLength):50);
   progressCount++;

   // if ( to == segments.last() )
   //  break;
      }

      // we survived, delte old stuff
      delete [] savedID;
      delete savedList;
   }

  catch (DifferentObjects& dob) {
     delete [] borderID_;
     delete borderlist_;
     borderID_ = savedID;
     borderlist_ = savedList;

     throw(dob); 
  }

  catch (AmbiguousPath& ap) {
     delete [] borderID_;
     delete borderlist_;
     borderID_ = savedID;
     borderlist_ = savedList;

     throw(ap); 
  }

  catch (AssertWarning& aw) {
     delete [] borderID_;
     delete borderlist_;
     borderID_ = savedID;
     borderlist_ = savedList;

     throw(aw);
  }

  catch (AssertFatal& af) {
     delete [] borderID_;
     delete borderlist_;
     borderID_ = savedID;
     borderlist_ = savedList;

     throw(af); 
  }

}

struct growentry {
   int    onfront;
   int    last;
   double dist;
};

const double DIRECTDIST = 1.0;

void TGebiet::extractBorder(const TSegmentInfo& startinfo, const TSegmentInfo& stopinfo)
{
  int start = startinfo.inx_;
  int stop  = stopinfo.inx_;

  int i, k, count, neighb[14];
  TVertex v, vstop;
  TPoint3D p, pstop;
  double qdist;
  growentry* zelle;
  TIntList tmplist;
  int currentFront = 1;

  try {
     if (start == stop)
  return;
  
     int L = sn_->length();
     zelle = new growentry[L];
     for (i = 0; i < L; i++) {
         zelle[i].onfront = 0;
   zelle[i].dist = FLT_MAX;
   zelle[i].last = -1;
     }
     
     // currentFront is an id to number the active front
     // because we loop over the whole net
     zelle[start].onfront = currentFront;
     zelle[start].dist = 0;

     vstop = sn_->GetVertex(stop);
     pstop = vstop.GetCoords3D();

     while ( zelle[stop].last == -1 ) {
  int gotAnUpdate = 0;
  for (i = 0; i < L; i++) {
     if ( zelle[i].onfront == currentFront ) {
        v = sn_->GetVertex(i);
        p = v.GetCoords3D();
        for (k = 0; k < v.count_; k+=2) {
     qdist = zelle[i].dist +  DIRECTDIST;
     int update = TFALSE;
     
     // we have a smaller distance: update zelle entry
     if ( qdist < zelle[v.neighb_[k]].dist ) 
        update = TTRUE;

     // same distance
     else if ( qdist == zelle[v.neighb_[k]].dist ) {
        TVertex vlast = sn_->GetVertex(zelle[v.neighb_[k]].last);
        TPoint3D plast = vlast.GetCoords3D();

        float distp_pstop = pdist(p,pstop);
        float distplast_pstop = pdist(plast,pstop);
        if (distp_pstop < distplast_pstop)
           update = TTRUE;
     }

     if ( update ) {
        zelle[v.neighb_[k]].dist = qdist;
        zelle[v.neighb_[k]].onfront = currentFront+1;
        zelle[v.neighb_[k]].last = i;
        gotAnUpdate = 1;
     }
        }
     }
  }

  currentFront++;

  // if we try to spread over the net, but do not actually add some new neighbors to zelle
  // the two points are not 18-connected
  if ( gotAnUpdate == 0 )
     throw DifferentObjects();

     }

     for (i = stop; ; i = zelle[i].last) {
  
  if (i == start)
     break;
     
  if ( borderID_[i] != 0 )
     throw AmbiguousPath();

  tmplist.push(i);
     }
     
     while (! tmplist.empty() ) {
  int tmpval = tmplist.pop();
  borderlist_->insert(tmpval);
  borderID_[tmpval] = nextID_;
     }
     nextID_++;

     delete [] zelle;
  }

  catch (DifferentObjects& dob) {
     delete [] zelle;
     throw(dob); 
  }

  catch (AmbiguousPath& ap) {
     delete [] zelle;
     throw(ap); 
  }

  catch (AssertWarning& aw) {
     delete [] zelle;
     throw(aw); 
  }

  catch (AssertFatal& af) {
     delete [] zelle;
     throw(af); 
  }
}

void TGebiet::printBorder(ostream& os) const
{
   os << "ClickPoints = " << endl;
   os << "{ " << segments << " }" << endl;

   os << "BorderPoints = " << endl;
   os << "{ " << (*borderlist_) << " }" << endl;
}
