#ifndef IMAGE3D_H
#define IMAGE3D_H

//#include <cstdio>
#include "Vector3D.h"


/*  Class Image3D stores the voxels and handles coordinate transformations
    between three spaces.  World (physical coordinates are specified in
  the image files.  Model space maps the world coordinates uniformly so
  the longest axis (largest extent) has length 1.0.  The other two axes
  must then be in the range (0.0, 1.0].  Image space non-uniformly maps
  world space so all axes have length 1.0.  World coordinate space may
  have an origin other than (0, 0, 0), but the other two spaces do not.
*/


typedef unsigned short GreyValue;

extern const GreyValue MAX_GREY_VALUE;
extern const GreyValue MIN_GREY_VALUE;

// This tells how to handle out of bounds image values
enum ImageBoundaryType
{
    CONSTANT_IMAGE_BOUNDARY,
    EXTENDED_IMAGE_BOUNDARY
};

class Image3D
{
public:

    Image3D();
    Image3D(int _xDim, int _yDim, int _zDim);
    ~Image3D();
  void clear(GreyValue val = 0);

    GreyValue * getVoxels() const { return voxelArray; }

  // Number of slices per Cartesian direction
    int getXDim() const { return xDim; }
    int getYDim() const { return yDim; }
    int getZDim() const { return zDim; }

  // Physical (world) distance between slices
    double getXSpacing() const { return xSpacing; }
    double getYSpacing() const { return ySpacing; }
    double getZSpacing() const { return zSpacing; }

  // Physical (world) lengths of the axes
    double getXExtent() const { return xExtent; }
    double getYExtent() const { return yExtent; }
    double getZExtent() const { return zExtent; }

  // World coordinate origin
    Vector3D getOriginPixelPos() const { return originPixelPos; }

    // Coordinate conversion routines
    void modelToWorldCoordinates(Vector3D & coord);
    void worldToModelCoordinates(Vector3D & coord);
    void worldToImageCoordinates(Vector3D & coord);
    void modelToImageCoordinates(Vector3D & coord);

    double getModelToWorldScale() const { return modelToWorldScale; }
    const Vector3D & getWorldToImageScale() const { return worldToImageScale; }

    void setVoxels(GreyValue * voxels, int numX, int numY, int numZ);
    void setVoxel(int x, int y, int z, GreyValue val);
  void setVoxel(int p, GreyValue val);  //1D set.

    void setSpacing(double xSpace, double ySpace, double zSpace);

    void setOriginPixelPos(const Vector3D & pos) { originPixelPos = pos; }

  GreyValue * getVoxelAddr(int x, int y, int z);
    GreyValue getVoxelValue(int x, int y, int z);
    double getInterpolatedVoxelValue(double x, double y, double z);

    GreyValue getWindowedVoxelValue(int x, int y, int z);
    double getWindowedInterpolatedVoxelValue(double x, double y, double z);

    void intensityWindow(double min, double max)
    {
        intensityMin = min;
        intensityMax = max;
    }

    double getMinIntensityWindow() { return intensityMin; }
    double getMaxIntensityWindow() { return intensityMax; }


protected:

    GreyValue * voxelArray;

    // Number of voxels in x, y, and z
    int xDim,
        yDim,
        zDim;

    // Spacing between voxels in world coordinates
    double xSpacing,
           ySpacing,
           zSpacing;

    // Extent of each dimension in world coordinates
    double xExtent,
           yExtent,
           zExtent;

    // Position of pixel (0,0,0) in world coordinates
    Vector3D originPixelPos;

    double modelToWorldScale;
    double worldToModelScale;
    Vector3D worldToImageScale;
    Vector3D modelToImageScale;

    // Look-up tables for y and z indices
    int * yIndexLUT;
    int * zIndexLUT;

    double intensityMin,
          intensityMax;

    ImageBoundaryType boundaryType;
};

#endif

