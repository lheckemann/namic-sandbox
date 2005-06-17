#ifndef RAW_IMAGE_FILE_H
#define RAW_IMAGE_FILE_H

#include "Image3D.h"
#include "Vector3D.h"

extern const int UNKNOWN_GREY_VALUE;

class RAWImageFile
{
public:
    RAWImageFile() {}
    ~RAWImageFile() {}

    Image3D * readRAW3(const char * filename);
    Image3D * ReadmyRaw(const char * filename,    
      int xDim, int yDim, int zDim,
      float xSpacing, float ySpacing, float zSpacing,
      float xOrig, float yOrig, float zOrig);
    void write(const char * filename, Image3D & image);
    void writeRAW3(const char * filename, Image3D & image);

private:
    void skipComments(FILE * fp);
};

#endif

