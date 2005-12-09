// -*- C++ -*-
/****************************************************************************
 *
 *  class TGeometryCreator
 *
 ****************************************************************************
 *
 *  File         :  TGeometryCreator.h
 *  Type         :  
 *  Purpose      :  create geometries
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  29 Feb 95
 *
 ****************************************************************************/
 
#ifndef TGEOMETRYCREATOR_H
#define TGEOMETRYCREATOR_H

#ifndef NO_AVS

#include "util_avs.h"
#include "TPoint3D.h"
#include "TSurfaceNet.h"
#include "TSurfacePatch.h"
#include "TFlatPatch.h"
#include "Colormap.h"
#include "TVoxelVolume.h"

extern float gRed[3];
extern float gGreen[3];
extern float gBlue[3];
extern float gBlack[3];
extern float gWhite[3];
extern float gAquamarine[3];
extern float gLightBlue[3];
extern float gIvory[3];
extern float gRoyalBlue[3];
extern float gDarkTurquoise[3];
extern float gChartreuse[3];
extern float gSeaGreen[3];

#define MAXDISJOINT 1024

class TGeometryCreator {
public:
   TGeometryCreator(GEOMedit_list* editlist) : editlist_(editlist)  { }
   
   // utility routines
   void MakeSphere(char* name, const TPoint3D& pt, const float& radius, float* color);
   void MakeSphere(char* name, const TPointFloat3D& pt, const float& radius, float* color);
   void MakeLabel(char* name, int num, const TPoint3D& pt, float* color);

   // patches
   void SurfaceNet2Polyhedron(TSurfaceNet* sn, char* name, float color[3]);
   void Patch2Polyhedron(TSurfacePatch* sp, char* name, float color[3]);
   void Patch2Polyhedron(TSurfacePatch* sp, TVoxelVolume* cf, RGBcolormap* rgbcmap, char* name);
   void Patch2Polyhedron(TSurfacePatch* sp, AVSfield_char* frgb, char* name);
   void FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, TVoxelVolume* cf, 
           RGBcolormap* rgbcmap, char* name);
   void FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, AVSfield_char* frgb, char* name);
   void FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, char* name);

   // Disjoint Polygons
   void BeginDisjointPolygon();
   void EndDisjointPolygon(char* name, float color[3]);

   // Disjoint Lines
   void BeginDisjointLine();
   void EndDisjointLine(char* name, float color[3]);

   void AddDisjointPoint(TPointFloat3D& pt);

   // adds object with nothing in it (to delete it in the geometry viewer "downstream")
   void AddEmptyObject(char* name);

private:
   GEOMedit_list* editlist_;              // all geometries created get into this editlist
   
   TPointFloat3D* disjointPoints_;        // used by the disjoint lines and disjoint polygons routines
   int idisjoint_;                        // dito
};

#endif

#endif
