// -*- C++ -*-
/****************************************************************************
 *
 *  class TVoxelVolume
 *
 ****************************************************************************
 *
 *  File         :  TVoxelVolume.h
 *  Type         :  
 *  Purpose      :  class for field 3D byte
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/


#ifndef TVOXELVOLUME_H
#define  TVOXELVOLUME_H

#include "util.h"
#include "TPoint3D.h"

#define VOXEL(x,y,z) data_[(x) + dim_[0] *((y) + dim_[1] *(z))]

class TVoxelVolume {
public:
   TVoxelVolume();
   TVoxelVolume(int xs,int ys, int zs, const unsigned char *data);
   TVoxelVolume(const int dim[3], const unsigned char *data);
   TVoxelVolume(const TVoxelVolume& vox);
   ~TVoxelVolume();

   TVoxelVolume& operator=(const TVoxelVolume& src);
   TVoxelVolume  operator+ (const TVoxelVolume& src);
   TVoxelVolume& operator+=(const TVoxelVolume& src);
   TVoxelVolume  operator- (const TVoxelVolume& src);
   TVoxelVolume& operator-=(const TVoxelVolume& src);

   TVoxelVolume  operator* (const TVoxelVolume& src);
   TVoxelVolume  operator* (const unsigned char& val);
   TVoxelVolume& operator*=(const TVoxelVolume& src);
   TVoxelVolume& operator*=(const unsigned char& val);
   unsigned char operator()(int x,int y, int z) const           { return VOXEL(x,y,z); }

   // basic operations
   void     GetSize(int& xs, int& ys, int& zs) const { xs = dim_[0]; ys = dim_[1]; zs = dim_[2]; }
   void     GetSize(int dim[3]) const    { dim[0] = dim_[0]; dim[1] = dim_[1]; dim[2] = dim_[2]; }
   int      Nx() const                                          { return dim_[0]; }
   int      Ny() const                                          { return dim_[1]; }
   int      Nz() const                                          { return dim_[2]; }
   void     Fill(unsigned char val);
   void     SetBoundary(unsigned char val);
   void     Binarize(unsigned char thresh);
   void     SetVox(unsigned char val,int x,int y, int z)        { VOXEL(x,y,z) = val;  }
   void     SetVox(unsigned char val, TPoint3D pt)              { VOXEL(pt.x_, pt.y_, pt.z_) = val; }
   unsigned char    GetVox(int x,int y, int z) const            { return VOXEL(x,y,z); }
   unsigned char    GetVox(int wh[3]) const                     { return VOXEL(wh[0], wh[1], wh[2]); }
   
   // change size of field, retain as much info as possible
   void      DimensionsChanged(int newxs, int newys, int newzs);
   void      DimensionsChanged(int newdim[3]) { DimensionsChanged(newdim[0], newdim[1], newdim[2]); }

   // AVS
   void CopyToAVS(unsigned char* outavs);
   void CopyFromAVS(const unsigned char* inavs);

private:
   unsigned char* data_;
   int            dim_[3];
   long           size_; // same as dim_[0]*dim_[1]*dim_[2]
};

#define forall_voxels(vox) for (int z=0;z<(vox).Nz();z++) for (int y=0;y<(vox).Ny();y++) for (int x=0;x<(vox).Nx();x++)

#endif
