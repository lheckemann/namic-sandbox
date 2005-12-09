// -*- C++ -*-
/****************************************************************************
 *
 *  class TGeometryCreator
 *
 ****************************************************************************
 *
 *  File         :  TGeometryCreator.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  9  Jan 95    Robert Schweikert
 *  last change  :  20 Jan 95
 *
 ****************************************************************************/
 

#include "TGeometryCreator.h"


float gRed[3]   = {1,0,0};
float gGreen[3] = {0,1,0};
float gBlue[3]  = {0,0,1};
float gWhite[3] = {1,1,1};
float gBlack[3] = {0,0,0};
float gAquamarine[3] = {0.498, 1, 0.831};
float gLightBlue[3]  = {0.678, 0.847, 0.902};
float gIvory[3]  = {1, 1, 0.941};
float gRoyalBlue[3] = {0.255, 0.412, 0.882};
float gDarkTurquoise[3] = {0, 0.808, 0.82};
float gChartreuse[3] = {0.498, 1, 0};
float gSeaGreen[3] = {0.18, 0.545, 0.341};

void TGeometryCreator::MakeSphere(char* name, const TPoint3D& pt, const float& radius, float* color)
{
   float ver[3];
   float localradius = radius;
   ver[0] = pt.x_; ver[1] = pt.y_; ver[2] = pt.z_;

   GEOMobj* object= GEOMcreate_sphere( NULL, ver  ,&localradius, NULL, NULL, 1, GEOM_COPY_DATA);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(object);
}

void TGeometryCreator::MakeSphere(char* name, const TPointFloat3D& pt, const float& radius, float* color)
{
   float ver[3];
   float localradius = radius;
   ver[0] = pt.x_; ver[1] = pt.y_; ver[2] = pt.z_; // o la la, copying things around (increible)

   GEOMobj* object= GEOMcreate_sphere( NULL, ver  ,&localradius, NULL, NULL, 1, GEOM_COPY_DATA);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(object);
}

void TGeometryCreator::MakeLabel(char* name, int num, const TPoint3D& pt, float* color)
{
   int i, x,y, z;
   float refpt[3];
   float offset[3] = {0,0,0};
  
   char snum[40];
  
   int lflags = GEOMcreate_label_flags(1, TFALSE, TFALSE, TFALSE, GEOM_LABEL_RIGHT, 0);
   GEOMobj* object = GEOMcreate_label(GEOM_NULL, lflags);
   refpt[0] = pt.x_; refpt[1] = pt.y_; refpt[2] = pt.z_;
   sprintf(snum, "%d",num);
   GEOMadd_label(object, snum, refpt, offset, 0.09, GEOM_NULL, -1);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(object);
}

void TGeometryCreator::SurfaceNet2Polyhedron(TSurfaceNet* sn, char* name, float color[3])
{
   int i, j, ivert, icol, ifaces;
   int L = sn->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   for (i=0, ivert=0; i<L; ++i) {
      // vertices
      TVertex v = sn->GetVertex(i);
      verts[ivert++] = v.wh_[0];
      verts[ivert++] = v.wh_[1];
      verts[ivert++] = v.wh_[2];
   }

   int nfaces = sn->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sn->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, GEOM_NULL, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(object);
} 

void TGeometryCreator::Patch2Polyhedron(TSurfacePatch* sp, char* name, float color[3])
{
   int i, j, ivert, icol, ifaces;
   int L = sp->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   for (i=0, ivert=0; i<L; ++i) {
      // vertices
      TVertex v = sp->GetVertex(i);
      verts[ivert++] = v.wh_[0];
      verts[ivert++] = v.wh_[1];
      verts[ivert++] = v.wh_[2];
   }

   int nfaces = sp->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, GEOM_NULL, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(object);
}  

void TGeometryCreator::Patch2Polyhedron(TSurfacePatch* sp, TVoxelVolume* cf, RGBcolormap* rgbcmap, char* name)
{
   int i, j, ivert, icol, ifaces;
   int L = sp->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   unsigned long* colors = new unsigned long[L];
   for (i=0, ivert=0, icol=0; i<L; ++i) {
      // vertices
      TVertex v = sp->GetVertex(i);
      verts[ivert++] = v.wh_[0];
      verts[ivert++] = v.wh_[1];
      verts[ivert++] = v.wh_[2];

      // colors
      int cindex = cf->GetVox(v.wh_);
      unsigned long col = 0;
      unsigned long red   = rgbcmap->r[cindex]*255;
      unsigned long green = rgbcmap->g[cindex]*255;
      unsigned long blue  = rgbcmap->b[cindex]*255;
      col |= (red<<AVS_RED_SHIFT);
      col |= (green<<AVS_GREEN_SHIFT);
      col |= (blue<<AVS_BLUE_SHIFT);
      colors[icol++] = col;
   }

   int nfaces = sp->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, colors, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] colors;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMdestroy_obj(object);
}

void TGeometryCreator::Patch2Polyhedron(TSurfacePatch* sp, AVSfield_char* frgb,  char* name)
{
   int i, j, ivert, icol, ifaces;
   int L = sp->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   unsigned long* colors = new unsigned long[L];
   for (i=0, ivert=0, icol=0; i<L; ++i) {
      // vertices
      TVertex v = sp->GetVertex(i);
      verts[ivert++] = v.wh_[0];
      verts[ivert++] = v.wh_[1];
      verts[ivert++] = v.wh_[2];

      // colors
      unsigned long col = 0;
      unsigned long red   = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[0];
      unsigned long green = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[1];
      unsigned long blue  = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[2];
      col |= (red<<AVS_RED_SHIFT);
      col |= (green<<AVS_GREEN_SHIFT);
      col |= (blue<<AVS_BLUE_SHIFT);
      colors[icol++] = col;
   }

   int nfaces = sp->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, colors, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] colors;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMdestroy_obj(object);
}   

void TGeometryCreator::FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, 
              TVoxelVolume* cf, RGBcolormap* rgbcmap, char* name)
{
   int i, j, ivert, icol, ifaces;
   int L = sp->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   unsigned long* colors = new unsigned long[L];
   for (i=0, ivert=0, icol=0; i<L; ++i) {
      // vertices
      TPointFloat3D pt = fp->GetPointFloat3D(i);
      verts[ivert++] = pt.x_;
      verts[ivert++] = pt.y_;
      verts[ivert++] = 0;

      // colors
      TVertex v = sp->GetVertex(i);
      int cindex = cf->GetVox(v.wh_);
      unsigned long col = 0;
      unsigned long red   = rgbcmap->r[cindex]*255;
      unsigned long green = rgbcmap->g[cindex]*255;
      unsigned long blue  = rgbcmap->b[cindex]*255;
      col |= (red<<AVS_RED_SHIFT);
      col |= (green<<AVS_GREEN_SHIFT);
      col |= (blue<<AVS_BLUE_SHIFT);
      colors[icol++] = col;
   }

   int nfaces = sp->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, colors, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] colors;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS); 
   GEOMedit_geometry(*editlist_, name, object);
   GEOMdestroy_obj(object);
}


void TGeometryCreator::FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, AVSfield_char* frgb, char* name)
{
   int i, j, ivert, icol, ifaces;
   int L = sp->length();
   GEOMobj* object;

   float* verts = new float[L*3];
   unsigned long* colors = new unsigned long[L];
   for (i=0, ivert=0, icol=0; i<L; ++i) {
      // vertices
      TPointFloat3D pt = fp->GetPointFloat3D(i);
      verts[ivert++] = pt.x_;
      verts[ivert++] = pt.y_;
      verts[ivert++] = 0;

      // colors
      TVertex v = sp->GetVertex(i);
      unsigned long col = 0;
      unsigned long red   = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[0];
      unsigned long green = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[1];
      unsigned long blue  = (I3DV(frgb, v.wh_[0], v.wh_[1], v.wh_[2]))[2];
      col |= (red<<AVS_RED_SHIFT);
      col |= (green<<AVS_GREEN_SHIFT);
      col |= (blue<<AVS_BLUE_SHIFT);
      colors[icol++] = col;
   }

   int nfaces = sp->nface();
   int* faces = new int[nfaces*5 + 1];
   for (i=0, ifaces=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      faces[ifaces++] = 4;
      faces[ifaces++] = i + 1; // +1 for GEOM polyhedron object
      faces[ifaces++] = v.neighb_[j] + 1;
      faces[ifaces++] = v.neighb_[j+1] + 1;
      faces[ifaces++] = v.neighb_[second] + 1;
   }
      }
   }
   faces[ifaces] = 0; 
   object = GEOMcreate_polyh_with_data(GEOM_NULL, verts, GEOM_NULL, colors, L, faces, GEOM_CONCAVE, GEOM_COPY_DATA);

   delete [] verts;
   delete [] colors;
   delete [] faces;

   GEOMgen_normals(object,GEOM_FACET_NORMALS); 
   GEOMedit_geometry(*editlist_, name, object);
   GEOMdestroy_obj(object);
}



void TGeometryCreator::FlatPatch2Polyhedron(TSurfacePatch* sp, TFlatPatch* fp, char* name)
{
   int i, j, ivert;
   int L = sp->length();

   GEOMobj* object = GEOMcreate_obj(GEOM_POLYHEDRON, GEOM_NULL);

   float* verts = new float[L*3];
   for (i=0, ivert=0; i<L; ++i) {
      TPointFloat3D pt = fp->GetPointFloat3D(i);
      verts[ivert++] = pt.x_ * 100;
      verts[ivert++] = pt.y_ * 100;
      verts[ivert++] = 0;
   }
   GEOMadd_vertices(object, verts, L, GEOM_COPY_DATA);
   delete [] verts;

   int face[4];
   for (i=0; i<L; ++i) {
      TVertex v = sp->GetVertex(i);
      for (j=0; j+1 <v.count_; j+=2) {
   int second = (j+2)%v.count_;
   if (v.neighb_[j     ] > i &&
       v.neighb_[j+1   ] > i &&
       v.neighb_[second] > i) {
      face[0] = i + 1; // +1 for GEOM polyhedron object
      face[1] = v.neighb_[j] + 1;
      face[2] = v.neighb_[j+1] + 1;
      face[3] = v.neighb_[second] + 1;
      GEOMadd_polygon(object, 4, face, 0, GEOM_COPY_DATA);
   }
      }
   }
   GEOMgen_normals(object,GEOM_FACET_NORMALS);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMedit_color(*editlist_, name, gWhite);
   GEOMdestroy_obj(object);
}


void TGeometryCreator::BeginDisjointPolygon()
{
   idisjoint_ = 0;
   disjointPoints_ = new TPointFloat3D [MAXDISJOINT];
}

void TGeometryCreator::EndDisjointPolygon(char* name, float color[3])
{
   GEOMobj* disjointObj = GEOMcreate_obj(GEOM_POLYHEDRON, GEOM_NULL);
   GEOMadd_disjoint_polygon(disjointObj, (float*) disjointPoints_, GEOM_NULL, GEOM_NULL, idisjoint_, 
           GEOM_NOT_SHARED | GEOM_COMPLEX, GEOM_COPY_DATA);
   delete [] disjointPoints_;
   GEOMgen_normals(disjointObj,GEOM_FACET_NORMALS); 
   GEOMedit_geometry(*editlist_, name, disjointObj);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(disjointObj);
}

void TGeometryCreator::BeginDisjointLine()
{
   idisjoint_ = 0;
   disjointPoints_ = new TPointFloat3D [MAXDISJOINT];
}

void TGeometryCreator::EndDisjointLine(char* name, float color[3])
{
   GEOMobj* disjointObj = GEOMcreate_obj(GEOM_POLYTRI, GEOM_NULL);
   GEOMadd_disjoint_line(disjointObj, (float*) disjointPoints_, GEOM_NULL, idisjoint_, GEOM_COPY_DATA);
   delete [] disjointPoints_;
   GEOMedit_geometry(*editlist_, name, disjointObj);
   GEOMedit_color(*editlist_, name, color);
   GEOMdestroy_obj(disjointObj);
}

void TGeometryCreator::AddDisjointPoint(TPointFloat3D& pt)
{
   assertStr((idisjoint_) < MAXDISJOINT, "Capacity of disjoint point array is exhausted");
   disjointPoints_[idisjoint_++] = pt;
}

void TGeometryCreator::AddEmptyObject(char* name)
{  
   GEOMobj* object = GEOMcreate_obj(GEOM_POLYHEDRON, GEOM_NULL);
   GEOMedit_geometry(*editlist_, name, object);
   GEOMdestroy_obj(object);
}
