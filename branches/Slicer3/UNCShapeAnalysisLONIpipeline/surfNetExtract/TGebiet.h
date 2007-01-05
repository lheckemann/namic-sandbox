// -*- C++ -*-
/****************************************************************************
 *
 *  class TGebiet
 *
 ****************************************************************************
 *
 *  File         :  TGebiet.h
 *
 *  Purpose      :  given a surface net, determine a border polygon on it
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  21 Feb 95
 *
 ****************************************************************************/
 
#ifndef TGEBIET_H
#define TGEBIET_H

#include "util_avs.h"
#include "TSurfaceNet.h"
#include "TIntset.h"
#include "TPoint3D.h"
#include "TIntList.h"
#include "TSegmentInfoList.h"

// states for avs module
enum { FIRST, EDITGEBIET, MAKEPATCH };

// states for a TGebiet
enum { GFIRSTCLICK = (MAKEPATCH+1) , GDETBORDER, GALTERBORDER };
enum { BUNDEF, BDEF, BADD, BMOVE, BDEL };

// error classes to be throwed
class AmbiguousPath {  };
class DifferentObjects {  };

class TGebiet {
public:
   TGebiet(TSurfaceNet* sn, GEOMedit_list* editlist, char* linename, float linecolor[3]);
   TGebiet(TSurfaceNet* sn, istream& is, GEOMedit_list* editlist, char* linename, float linecolor[3]);
   ~TGebiet() { delete [] borderID_; delete borderlist_;  }
  
   // define a TGebiet
   void addToBorder(const TPoint3D& pt);
   void SetPrefs(TSegmentInfo& seginfo)  { si_ = seginfo; }

   // graphics
   void drawBorder();
   void drawBorder(int id);
   void drawBorderID();
   void showClickPoints();
   void showBaseSegment();
  
   // edit a TGebiet
   void setBasePoint(const TPoint3D& pt);
   void addAfterBasePoint(const TPoint3D& pt);
   void deleteBasePoint();
   void moveBasePoint(const TPoint3D& newpt);
   void recalcBaseSegment();

   // determines a path from stop to start
   void extractBorder(const TSegmentInfo& startinfo, const TSegmentInfo& stopinfo);
   void extractBorder();

   void printBorder(ostream& os) const;

  
   int               gebietState_;         // provides debugging/consistency information only
   int               baseState_;           // basestate for AVS module to use

   int               start_, prev_, next_; // used during GDETBORDER
   TSegmentInfo      baseinfo_;            // setBasePoint sets this for subsequent edit operation      
   TSegmentInfoList  segments;             // information about all editable border points
             // (where the user clicked)

   TSurfaceNet*      sn_;                  // pointer to surface net

   TIntList*         borderlist_;          // list of the border indices
   int               nextID_;              // every segment gets an ID > 0
   int*              borderID_;            // border info ( borderID_[i] > 0 means i is in border )

   TSegmentInfo      si_;                  // the next edit will use this SegmentInfo

   GEOMedit_list*    editlist_;            // add geometries to this list
   char              linename_[80];        // name of geometries
   float             linecolor_[3];        // geometries-color
};

#endif
