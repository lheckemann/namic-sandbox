#ifndef convertBYU_h
#define convertBYU_h

/*
December 2003
Isabelle Corouge - Code adapted from Joshua Stough

Description: Transform a byu surface into a voxel volume.
*/

#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "RAWImageFile.h"
#include "Image3D.h"
#include "Vector3D.h"
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageDuplicator.h>
#include <itkImageRegionIterator.h>
#include <itkRawImageIO.h>

#define DBL_MAX  1.7976931348623158e+308
#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#define MAX(A,B) (((A) > (B)) ? (A) : (B))


struct hitstruct {
  double d;
  int sign;
  char boundary;  // 0=none 1=A 2=B
  Vector3D pt; //point of intersection.
};


int compareHitByDistance(const void *arg1, const void *arg2);

class convertBYU
{
public:
  convertBYU();
  ~convertBYU();
  
  typedef unsigned short PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<unsigned char,3> ImageTypeChar;
  typedef ImageType::Pointer ImagePointer;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  typedef itk::ImageFileReader<ImageType>   ReaderType;
  typedef itk::ImageFileWriter<ImageType>   WriterType;
  typedef itk::ImageFileWriter<ImageTypeChar>   WriterTypeChar;
  
  
  // PROTOTYPES
  void SetDimensions(int x, int y, int z,double pxi,double pyi,double pzi,
                                         double pxf,double pyf,double pzf,
                                         double addFrameSize);
  void ReadInput(const char * byuFilename);
  void FillP(double & volume);

  void TransformWorldToImage(double * point);
  // transforms a point from world (input) coordinates to image coordinates
  
  double FillHitsForDist();
  
  void addHit(int i, int j, double distance, int sign, int boundary, double e[]);
  double Intersect_tri(double e[], double v[], double tuv[], int vert);
  void WriteVolume(const char* imagename,int type =0);
  void WriteImage(const char* imagename,int type =0);
  void LoadRawImage(const char* imagename,int sizex,int sizey, int sizez, \
        float spacingx, float spacingy, float spacingz,int imagetype);
  void LoadOriginalImage(const char* imagename,float* pixdims);

  int pixes[3];      //the pixels in each dimension in output volume. 

  //Bounding box of the two surfaces;
  double minx, miny, minz;
  double maxx, maxy, maxz;

  Image3D *inImage;

private:
  
  double * L1;
  int numVertices, numTriangles, numEdges;
  int * T1;

  int pix;
  // STATIC hits
  hitstruct*** hits;  // [pix][pix][# of hits]

/*   int pixes[3];      //the pixels in each dimension in output volume.  */

/*   //Bounding box of the two surfaces; */
/*   double minx, miny, minz; */
/*   double maxx, maxy, maxz; */
  double deltaxi,deltayi,deltazi;
  double deltaxf,deltayf,deltazf;
  int imDimX, imDimY, imDimZ;
  //  float imPixDimX, imPixDimY, imPixDimZ;
  double addFrameSize;

  double addX, addY, addZ;
  // additional terms for dealing with surfaces that have negative coordinates
  
  //  Image3D *inImage;

};

#endif

