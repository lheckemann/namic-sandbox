#ifndef _CVOLOP_H_
#define _CVOLOP_H_


#include "cVolume.h"

#include <string>


class cVolOp
{
public:

  cVolOp();  


  ////////////////////////////////////////////////////////////////////////////////
  // IO

  // read
  cVolume *volreadUchar( const char* filename );
  cVolume *volreadDouble( const char* filename );
  cVolume *volreadUshort( const char* filename );
  cVolume *volreadShort( const char* filename );

  // write
  void volwriteUchar( const char* filename, cVolume *volume );
  void volwriteDouble( const char* filename, cVolume *volume );
  void volwriteUshort( const char* filename, cVolume *volume );
  void volwriteShort( const char* filename, cVolume *volume );

  void writeRawData( cVolume *volume );


  ////////////////////////////////////////////////////////////////////////////////
  // convertion cVolume <==> ITKvolume
  //  void cVolume2itkVolume( cVolume *volume );
  itkInternalVolumeType::Pointer cVolume2itkVolume( cVolume *volume );

  cVolume* itkVolume2cVolume( itkInternalVolumeType::Pointer itkVol );


  ////////////////////////////////////////////////////////////////////////////////
  // point-wise operation
  cVolume* normalizeVol( cVolume *volume, pixelType theMin, pixelType theMax );
  cVolume* expTrans( cVolume *volume );
  cVolume* logTrans( cVolume *volume );
  cVolume* sqrtTrans( cVolume *volume );

  cVolume *addVolumes( cVolume *volume1, cVolume *volume2 );
  cVolume *substractVolumes( cVolume *volume1, cVolume *volume2 );

  cVolume *removeNeg( cVolume *volume );


  ////////////////////////////////////////////////////////////////////////////////
  // smooth
  cVolume* meanSmooth( cVolume *volume, int kernelSize=3 );
  cVolume* GaussSmth( cVolume *volume, int kernelWidth = 5, double varOfGauss = 2.0);
  cVolume* medianFilter( cVolume *volume, int radiusX, int radiusY, int radiusZ);
  cVolume* gheSmooth( cVolume *volume, int iterNum=10 );



  // threshold
  cVolume *hardThreshold(cVolume *vol, voxelType thld, voxelType outputLow = 0, voxelType outputHigh = 1);


  // crop
  cVolume* volcrop( cVolume *volume, int minX, int minY, int minZ, int xSize, int ySize, int zSize);

  // morphology
  cVolume *dilate(cVolume *vol, int radius = 1);
  


};

#endif

