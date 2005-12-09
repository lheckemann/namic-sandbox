#include "Image3D.h"

#include <iostream>

const GreyValue OUT_OF_BOUNDS_VALUE = 0;
const GreyValue MAX_GREY_VALUE = 65535;
const GreyValue MIN_GREY_VALUE = 0;

Image3D::Image3D()
{
    voxelArray = NULL;
    yIndexLUT = NULL;
    zIndexLUT = NULL;

    xDim = 0;
    yDim = 0;
    zDim = 0;

    xSpacing = 0.0;
    ySpacing = 0.0;
    zSpacing = 0.0;

    modelToWorldScale = 1.0;
    worldToModelScale = 1.0;

    intensityMin = 0.0f;
    intensityMax = 1.0f;

    boundaryType = EXTENDED_IMAGE_BOUNDARY;
    /*
    // TEST CODE
    double space = 1.0 / 128.0;
    xDim = yDim = zDim = 128;
    float center = 64.0f;

    float a, b, c;

    a = 2304.0f;
    b = 1024.0f;
    c = 256.0f;

    int index = 0;

    voxelArray = new GreyValue[xDim * yDim * zDim];
    setSpacing(space, space, space);

    for(float k = -center; k < center; k+=1.0f)
    {
        for(float j = -center; j < center; j+=1.0f)
        {
            for(float i = -center; i < center; i+=1.0f)
            {
                if(i*i / a + j*j / b + k*k / c <= 1.0f)
                    voxelArray[index] = MAX_GREY_VALUE * 0.75;
//                else if(i*i / a + j*j /b + k*k / c <= 1.5f)
//                    voxelArray[index] = MAX_GREY_VALUE * 0.7;
                else
                    voxelArray[index] = MIN_GREY_VALUE;

                index++;
            }
        }
    }
    */
}

Image3D::Image3D(int _xDim, int _yDim, int _zDim)
{
    int index,
        increment,
        i;

    xDim = _xDim;
    yDim = _yDim;
    zDim = _zDim;

    setSpacing(1.0, 1.0, 1.0);
    modelToWorldScale = 1.0;
    worldToModelScale = 1.0;

    intensityMin = 0.0f;
    intensityMax = 1.0f;

    yIndexLUT = new int[yDim];
    zIndexLUT = new int[zDim];

    index = 0;
    for(i = 0; i < yDim; i++)
    {
        yIndexLUT[i] = index;
        index += xDim;
    }

    index = 0;
    increment = xDim * yDim;
    for(i = 0; i < zDim; i++)
    {
        zIndexLUT[i] = index;
        index += increment;
    }

    voxelArray = new GreyValue[xDim * yDim * zDim];
    if (!voxelArray) {
      std::cout << "error allocating voxel array" << std::endl;
    }

    boundaryType = EXTENDED_IMAGE_BOUNDARY;
}

void Image3D::clear(GreyValue val) {
    if(voxelArray == NULL)
    return;

  for (int i = 0; i < xDim*yDim*zDim; i++)
    voxelArray[i] = val;
}

Image3D::~Image3D()
{
    if(voxelArray != NULL)
        delete voxelArray;
    if(yIndexLUT != NULL)
        delete yIndexLUT;
    if(zIndexLUT != NULL)
        delete zIndexLUT;
}

void Image3D::modelToWorldCoordinates(Vector3D & coord)
{
    coord *= modelToWorldScale;
    coord += originPixelPos;
}

void Image3D::worldToModelCoordinates(Vector3D & coord)
{
    coord -= originPixelPos;
    coord *= worldToModelScale;
}

void Image3D::worldToImageCoordinates(Vector3D & coord)
{
    coord.setX((coord.getX() - originPixelPos.getX()) * worldToImageScale.getX());
    coord.setY((coord.getY() - originPixelPos.getY()) * worldToImageScale.getY());
    coord.setZ((coord.getZ() - originPixelPos.getZ()) * worldToImageScale.getZ());
}

void Image3D::modelToImageCoordinates(Vector3D & coord)
{
    coord.setX(coord.getX() * modelToImageScale.getX() - 0.5);
    coord.setY(coord.getY() * modelToImageScale.getY() - 0.5);
    coord.setZ(coord.getZ() * modelToImageScale.getZ() - 0.5);
}

void Image3D::setVoxels(GreyValue * voxels, int numX, int numY, int numZ)
{
    int index,
        increment,
        i;

    if(voxelArray != NULL)
        delete voxelArray;

    if(yIndexLUT != NULL)
        delete yIndexLUT;

    if(zIndexLUT != NULL)
        delete zIndexLUT;

    voxelArray = voxels;

    if(voxelArray != NULL)
    {
        xDim = numX;
        yDim = numY;
        zDim = numZ;

        yIndexLUT = new int[yDim];
        zIndexLUT = new int[zDim];

        index = 0;
        for(i = 0; i < yDim; i++)
        {
            yIndexLUT[i] = index;
            index += xDim;
        }

        index = 0;
        increment = xDim * yDim;
        for(i = 0; i < zDim; i++)
        {
            zIndexLUT[i] = index;
            index += increment;
        }
    }
    else
    {
        xDim = 0;
        yDim = 0;
        zDim = 0;

        yIndexLUT = NULL;
        zIndexLUT = NULL;
    }
}

void Image3D::setVoxel(int x, int y, int z, GreyValue val)
{
    if(x < 0 || x >= xDim || y < 0 || y >= yDim || z < 0 || z >= zDim)
    {
      std::cout << "Out of image range:" << x << "," << xDim << ";" << y << "," << yDim << ";" <<z << "," << zDim << std::endl ;
        return;
    }

    voxelArray[x + yIndexLUT[y] + zIndexLUT[z]] = val;
}

void Image3D::setVoxel(int p, GreyValue val)
{
  voxelArray[p] = val;
}

void Image3D::setSpacing(double xSpace, double ySpace, double zSpace)
{
    xSpacing = xSpace;
    xExtent = xSpacing * (double) xDim;

    ySpacing = ySpace;
    yExtent = ySpacing * (double) yDim;

    zSpacing = zSpace;
    zExtent = zSpacing * (double) zDim;

    // The scaling from model to world coordinates is the maximum extent
    modelToWorldScale = xExtent;
    if(yExtent > modelToWorldScale)
        modelToWorldScale = yExtent;
    if(zExtent > modelToWorldScale)
        modelToWorldScale = zExtent;

    // Scaling from world to model is just the inverse
    worldToModelScale = 1.0 / modelToWorldScale;

    // Scaling from world to image coordinates
    worldToImageScale.setX(1.0 / xSpacing);
    worldToImageScale.setY(1.0 / ySpacing);
    worldToImageScale.setZ(1.0 / zSpacing);

    // Scaling from model to image coordinates
    modelToImageScale.setX(modelToWorldScale / xSpacing);
    modelToImageScale.setY(modelToWorldScale / ySpacing);
    modelToImageScale.setZ(modelToWorldScale / zSpacing);
}

GreyValue Image3D::getVoxelValue(int x, int y, int z)
{
    if(voxelArray == NULL || yIndexLUT == NULL || zIndexLUT == NULL)
        return OUT_OF_BOUNDS_VALUE;

    if((boundaryType == CONSTANT_IMAGE_BOUNDARY) && 
       (x < 0 || x >= xDim || y < 0 || y >= yDim || z < 0 || z >= zDim))
    {
    std::cout << "CONSTANT!!!!!!" << std::endl;
        return OUT_OF_BOUNDS_VALUE;
    }
    else if(boundaryType == EXTENDED_IMAGE_BOUNDARY)
    {
        if(x < 0)
            x = 0;
        else if(x >= xDim)
            x = xDim - 1;
        if(y < 0)
            y = 0;
        else if(y >= yDim)
            y = yDim - 1;
        if(z < 0)
            z = 0;
        else if(z >= zDim)
            z = zDim - 1;
    }

    return voxelArray[x + yIndexLUT[y] + zIndexLUT[z]];
}

GreyValue* Image3D::getVoxelAddr(int x, int y, int z)
//Like the above except it returns the address of a voxel value.
{
    if(voxelArray == NULL || yIndexLUT == NULL || zIndexLUT == NULL)
        return (GreyValue*)&OUT_OF_BOUNDS_VALUE;

    if((boundaryType == CONSTANT_IMAGE_BOUNDARY) && 
       (x < 0 || x >= xDim || y < 0 || y >= yDim || z < 0 || z >= zDim))
    {
    std::cout << "CONSTANT!!!!!!" << std::endl;
        return (GreyValue*)&OUT_OF_BOUNDS_VALUE;
    }
    else if(boundaryType == EXTENDED_IMAGE_BOUNDARY)
    {
        if(x < 0)
            x = 0;
        else if(x >= xDim)
            x = xDim - 1;
        if(y < 0)
            y = 0;
        else if(y >= yDim)
            y = yDim - 1;
        if(z < 0)
            z = 0;
        else if(z >= zDim)
            z = zDim - 1;
    }

    return &voxelArray[x + yIndexLUT[y] + zIndexLUT[z]];
}

double Image3D::getInterpolatedVoxelValue(double x, double y, double z)
{
    int xIndex,
        yIndex,
        zIndex;

    double xt, yt, zt;

    double v[8];
    double val;

    if(voxelArray == NULL)
       return OUT_OF_BOUNDS_VALUE;

    xIndex = (int) x;
    yIndex = (int) y;
    zIndex = (int) z;

    xt = (double) x - (double) xIndex;
    yt = (double) y - (double) yIndex;
    zt = (double) z - (double) zIndex;

    v[0] = getVoxelValue(xIndex, yIndex, zIndex);
    v[1] = getVoxelValue(xIndex + 1, yIndex, zIndex);
    v[2] = getVoxelValue(xIndex, yIndex + 1, zIndex);
    v[3] = getVoxelValue(xIndex + 1, yIndex + 1, zIndex);
    v[4] = getVoxelValue(xIndex, yIndex, zIndex + 1);
    v[5] = getVoxelValue(xIndex + 1, yIndex, zIndex + 1);
    v[6] = getVoxelValue(xIndex, yIndex + 1, zIndex + 1);
    v[7] = getVoxelValue(xIndex + 1, yIndex + 1, zIndex + 1);

    val = (1 - zt) * ((1 - yt) * (v[0] * (1 - xt) + v[1] * xt)
        + yt * (v[2] * (1 - xt) + v[3] * xt))
        + zt * ((1 - yt) * (v[4] * (1 - xt) + v[5] * xt)
        + yt * (v[6] * (1 - xt) + v[7] * xt));

    return val;
}

GreyValue Image3D::getWindowedVoxelValue(int x, int y, int z)
{
    if(voxelArray == NULL || yIndexLUT == NULL || zIndexLUT == NULL)
        return OUT_OF_BOUNDS_VALUE;

    double initVal = (double) getVoxelValue(x, y, z) / (double) MAX_GREY_VALUE;
    double finalVal;

    if(initVal <= intensityMin)
        finalVal = 0.0;
    else if(initVal >= intensityMax)
        finalVal = 1.0;
    else
    {
        double range = intensityMax - intensityMin;

        finalVal = (initVal - intensityMin) / range;
    }

    return GreyValue(finalVal * (double)MAX_GREY_VALUE);
}

double Image3D::getWindowedInterpolatedVoxelValue(double x, double y, double z)
{
    int xIndex,
        yIndex,
        zIndex;

    double xt, yt, zt;

    double v[8];
    double val;

    if(voxelArray == NULL)
       return OUT_OF_BOUNDS_VALUE;

    xIndex = (int) x;
    yIndex = (int) y;
    zIndex = (int) z;

    xt = (double) x - (double) xIndex;
    yt = (double) y - (double) yIndex;
    zt = (double) z - (double) zIndex;

    v[0] = getVoxelValue(xIndex, yIndex, zIndex);
    v[1] = getVoxelValue(xIndex + 1, yIndex, zIndex);
    v[2] = getVoxelValue(xIndex, yIndex + 1, zIndex);
    v[3] = getVoxelValue(xIndex + 1, yIndex + 1, zIndex);
    v[4] = getVoxelValue(xIndex, yIndex, zIndex + 1);
    v[5] = getVoxelValue(xIndex + 1, yIndex, zIndex + 1);
    v[6] = getVoxelValue(xIndex, yIndex + 1, zIndex + 1);
    v[7] = getVoxelValue(xIndex + 1, yIndex + 1, zIndex + 1);

    val = (1 - zt) * ((1 - yt) * (v[0] * (1 - xt) + v[1] * xt) + yt * (v[2] * (1 - xt) + v[3] * xt)) +
          zt       * ((1 - yt) * (v[4] * (1 - xt) + v[5] * xt) + yt * (v[6] * (1 - xt) + v[7] * xt));

    double initVal = (double) val / (double) MAX_GREY_VALUE;
    double finalVal;

    if(initVal <= intensityMin)
        finalVal = 0.0;
    else if(initVal >= intensityMax)
        finalVal = 1.0;
    else
    {
        double range = intensityMax - intensityMin;

        finalVal = (initVal - intensityMin) / range;
    }

    return finalVal * (double) MAX_GREY_VALUE;
}
