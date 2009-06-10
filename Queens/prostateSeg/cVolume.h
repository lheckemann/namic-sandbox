#ifndef _CVOLUME_H_
#define _CVOLUME_H_

typedef double elementType;

typedef elementType pixelType;

typedef pixelType voxelType;

const double PI = 3.14159265353232979;

#include "itkImage.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkDiscreteGaussianImageFilter.h"
#include "itkMedianImageFilter.h"

// #include "itkRescaleIntensityImageFilter.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 


////////////////////////////////////////////////////////////
// typedef for uchar 3D Volume
const unsigned int             itkVolDimension = 3;

// typedef itk::Image< unsigned char, itkVolDimension > itkVolumeType;
// typedef itk::ImageRegionConstIterator< itkVolumeType > itkVolConstIteratorType;
// typedef itk::ImageRegionIterator< itkVolumeType> itkVolIteratorType;

////////////////////////////////////////////////////////////
// typedef for the internal 3D double ITK volume
typedef itk::Image< voxelType, itkVolDimension > itkInternalVolumeType;
typedef itk::Image< double, itkVolDimension > DoubleImageType;
typedef itk::Image< unsigned char, itkVolDimension > UcharImageType;


typedef itk::ImageRegionConstIterator< itkInternalVolumeType > itkVolumeInternalConstIteratorType;
typedef itk::ImageRegionIterator< itkInternalVolumeType> itkVolumeInternalIteratorType;



class cVolume
{
protected:
  voxelType* _voxel;
  //  std::vector< voxelType > _voxel;

  int _sizeX, _sizeY, _sizeZ;
  double _spaceX, _spaceY, _spaceZ;

  int _volDimension;

  //  void initiateIm(int x, int y);
  void initiateVol(int x, int y, int z, double spX, double spY, double spZ);
  void initiateVol(const cVolume* oriVol);  

public:
  cVolume();
  cVolume(int N);
  cVolume(int x, int y, int z);
  cVolume(int x, int y, int z, double spX, double spY, double spZ);
  cVolume(const cVolume* oriVol);
  ~cVolume();

//   std::vector<voxelType>::iterator getVoxelIterator();
//   std::vector<voxelType>::const_iterator getVoxelConstIterator();

//   const std::vector<voxelType> getVoxelVector();

  voxelType* getVoxelPointer();

  inline bool initiated() { return ( _voxel != 0 ); }

  int getDimension();
  int getSizeX();
  int getSizeY();
  int getSizeZ();

  double getSpacingX();
  double getSpacingY();
  double getSpacingZ();
  //  double *getSpacing(); allocate the double *sp = new double[3] inside fn or not?
  void getSpacing(double &spX, double &spY, double &spZ);
  void setSpacing(double spX, double spY, double spZ);

  
  inline void setVoxel(int x, int y, int z, voxelType val)
  {
    _voxel[z*_sizeX*_sizeY + y*_sizeX + x] = val;
  }

  inline voxelType getVoxel(int x, int y, int z)
  {
    return _voxel[z*_sizeX*_sizeY + y*_sizeX + x];
  }

  inline voxelType& operator()(int x, int y, int z)
  {
    return _voxel[z*_sizeX*_sizeY + y*_sizeX + x];
  }

  voxelType getVoxelTrilinear(double x, double y, double z);
  
  inline void setVoxel(int idx, voxelType val)
  {
    _voxel[idx] = val;
  }

  inline voxelType getVoxel(int idx)
  {
    return _voxel[idx];
  }

  inline voxelType& operator[](int idx)
  {
    return _voxel[idx];
  }


  void setAllVoxels( voxelType val );


  voxelType getMaxVoxel();
  voxelType getMinVoxel();


  void clear();

};


#endif //_CVOLUME_H_
