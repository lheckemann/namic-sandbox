#include "cVolume.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <float.h>  // for DBL_MIN

///////////// Constructor
cVolume::cVolume()
{
  _voxel = 0; 
}

cVolume::~cVolume()
{ 
  clear();
}

cVolume::cVolume(int N)
{
  _voxel = 0; 
  initiateVol(N, N, N, 1.0, 1.0, 1.0);
}

cVolume::cVolume(int x, int y, int z)
{
  _voxel = 0; 
  initiateVol(x, y, z, 1.0, 1.0, 1.0);
}

cVolume::cVolume(int x, int y, int z, double spX, double spY, double spZ)
{
  _voxel = 0; 
  initiateVol(x, y, z, spX, spY, spZ);
}

cVolume::cVolume(const cVolume *oriVol)
{
  _voxel = 0; 
  initiateVol(oriVol);
}


//////////////////////////////////////////
// functions
int cVolume::getDimension()
{
  if ( _voxel )
    {
      return _volDimension;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}

int cVolume::getSizeX()
{
  if ( _voxel )
    {
      return _sizeX;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}

int cVolume::getSizeY()
{
  if ( _voxel )
    {
      return _sizeY;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}

int cVolume::getSizeZ()
{
  if ( _voxel )
    {
      return _sizeZ;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}

// void cVolume::setVoxel(int x, int y, int z, voxelType val)
// {
//   // for efficiency, do not check range. So be careful.
//   _voxel[z*_sizeX*_sizeY + y*_sizeX + x] = val;
// }

// voxelType cVolume::getVoxel(int x, int y, int z)
// {
//   // for efficiency, do not check range. So be careful.  
//   return _voxel[z*_sizeX*_sizeY + y*_sizeX + x];
// }

// void cVolume::setVoxel(int idx, voxelType val)
// {
//   // for efficiency, do not check range. So be careful.
//   // index of each dimension starts from 0, NOT 1!!!
//   _voxel[idx] = val;
// }

// voxelType cVolume::getVoxel(int idx)
// {
//   // for efficiency, do not check range. So be careful.  
//   return _voxel[idx];
// }


void cVolume::initiateVol(int x, int y, int z, double spX, double spY, double spZ)
{
  _volDimension = 3;

  _sizeX = x;
  _sizeY = y;
  _sizeZ = z;

  _spaceX = spX;
  _spaceY = spY;
  _spaceZ = spZ;

  _voxel = new voxelType[x*y*z];
  //  memset(_voxel, 0, sizeof(voxelType)*x*y);

  return;
}

void cVolume::initiateVol(const cVolume *oriVol)
{
  _volDimension = oriVol->_volDimension;

  _voxel = new voxelType[ (oriVol->_sizeX)*(oriVol->_sizeY)*(oriVol->_sizeZ) ];
  //  memset(_voxel,0,_sizeX*_sizeY*_sizeZ*sizeof(voxelType));

  //  _voxel.resize( (oriVol->_sizeX)*(oriVol->_sizeY)*(oriVol->_sizeZ) );
  _sizeX = oriVol->_sizeX;
  _sizeY = oriVol->_sizeY;
  _sizeZ = oriVol->_sizeZ;

  _spaceX = oriVol->_spaceX;
  _spaceY = oriVol->_spaceY;
  _spaceZ = oriVol->_spaceZ;

  int n = _sizeX*_sizeY*_sizeZ;
  for (int i = 0; i <= n-1 ; ++i)
    {
      _voxel[i] = oriVol->_voxel[i];
    }

  //  _voxel.assign( oriVol->_voxel.begin(), oriVol->_voxel.end() );

  return;
}

voxelType cVolume::getMaxVoxel()
{
  voxelType m = 0;
  
  if (!_voxel)
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
  else
    {
      m = _voxel[0];
      int n = _sizeX*_sizeY*_sizeZ;

      for (int i = 0; i <= n-1; ++i)
        {
          m = (m>=_voxel[i])?m:_voxel[i];
        }      
    }

  return m;

  //  return *(max_element(_voxel.begin(), _voxel.end()));
}

voxelType cVolume::getMinVoxel()
{
  voxelType m = 0;
  
  if (!_voxel)
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
  else
    {
      m = _voxel[0];
      int n = _sizeX*_sizeY*_sizeZ;

      for (int i = 0; i <= n-1; ++i)
        {
          m = (m<=_voxel[i])?m:_voxel[i];
        }      
    }

  return m;

  //  return *(min_element(_voxel.begin(), _voxel.end()));
        
//   m = _voxel[0];
//   int s = _sizeX*_sizeY*_sizeZ;

//   for (int it = 0; it < s; ++it)
//     {
//       m = (m > _voxel[it])? _voxel[it]:m ;
//     }

//   return m;
}

// std::vector<voxelType>::iterator cVolume::getVoxelIterator()
// {
//   if (!_initiated)
//     {
//       //      std::cerr<<"image not initialized. abort"<<std::endl;
//       exit(1);
//     }
//   return _voxel.begin();
// }

// std::vector<voxelType>::const_iterator cVolume::getVoxelConstIterator()
// {
//   if (!_initiated)
//     {
//       //      std::cerr<<"image not initialized. abort"<<std::endl;
//       exit(1);
//     }
//   return static_cast<std::vector<voxelType>::const_iterator>( _voxel.begin() );
// }


// const std::vector<voxelType> cVolume::getVoxelVector()
// {
//   if (!_initiated)
//     {
//       //      std::cerr<<"image not initialized. abort"<<std::endl;
//       exit(1);
//     }
//   return _voxel;
// }


void cVolume::setAllVoxels( voxelType val )
{
  if (_voxel)
    {
      int n = _sizeX*_sizeY*_sizeZ;
      for (int i = 0; i <= n-1; ++i)
        {
          _voxel[i] = val;
        }
    }

  return;

  //  _voxel.assign(_voxel.size(), val);
}

void cVolume::clear()
{
  if ( _voxel ) 
    {
      delete[] _voxel;
      _voxel = 0;
    }

  return;  
}


double cVolume::getSpacingX()
{
  if ( _voxel )
    {
      return _spaceX;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }  
}

double cVolume::getSpacingY()
{
  if ( _voxel )
    {
      return _spaceY;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }  
}

double cVolume::getSpacingZ()
{
  if ( _voxel )
    {
      return _spaceZ;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }  
}


void cVolume::getSpacing(double &spX, double &spY, double &spZ)
{
  if ( _voxel )
    {
      spX = _spaceX;
      spY = _spaceY;
      spZ = _spaceZ;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}


void cVolume::setSpacing(double spX, double spY, double spZ)
{
  if ( _voxel )
    {
      _spaceX = spX;
      _spaceY = spY;
      _spaceZ = spZ;
    }
  else
    {
      std::cerr<<"not initiated, exiting..."<<std::endl;
      exit(-1);
    }
}


voxelType cVolume::getVoxelTrilinear(double x, double y, double z)
{
  // need to check whether it's boundary. thus do border checking
  if ( !_voxel || x < 0 || x > (_sizeX - 1) || y < 0 || y > (_sizeY - 1) || z < 0 || z > (_sizeZ - 1))
    {
      std::cerr<<"not initiated or out of bound. exiting...\n";
      exit(-1);
    }
    
  // may use modf() to separate integer and decimal parts also.

  int a = (int)floor(x);
  int b = (int)floor(y);
  int c = (int)floor(z);

  double ax = x - a; // ax should >= 0
  double by = y - b; // by should >= 0
  double cz = z - c; // cz should >= 0

//   voxelType i1 = (_voxel[c*_sizeX*_sizeY + b*_sizeX + a])*(1 - cz) + (_voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a])*cz;
//   voxelType i2 = (_voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a])*(1 - cz) + (_voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a])*cz;

//   voxelType j1 = (_voxel[c*_sizeX*_sizeY + b*_sizeX + a+1])*(1 - cz) + (_voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a+1])*cz;
//   voxelType j2 = (_voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a+1])*(1 - cz) + (_voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a+1])*cz;

//   voxelType w1 = i1*(1 - by) + i2*by;
//   voxelType w2 = j1*(1 - by) + j2*by;

//   return w1*(1 - ax) + w2*ax;

  voxelType v000 = _voxel[c*_sizeX*_sizeY + b*_sizeX + a];
  voxelType v001 = 0; 
  voxelType v010 = 0; 
  voxelType v011 = 0; 
  voxelType v100 = 0; 
  voxelType v101 = 0; 
  voxelType v110 = 0; 
  voxelType v111 = 0; 
  // v*&^ means x=*, y=&, z=^


//    v000 = _voxel[c*_sizeX*_sizeY + b*_sizeX + a];
//    v001 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
//    v010 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a];
//    v011 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a+1];
//    v100 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
//    v101 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a+1];
//    v110 = _voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a];
//    v111 = _voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a+1];
//   // v*&^ means x=*, y=&, z=^

  
  voxelType v = 0;

  if (a != _sizeX - 1 && b != _sizeY - 1 && c != _sizeZ - 1)
    {
      // 3-D Trilinear interp
      v001 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v010 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a];
      v011 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a+1];
      v100 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v101 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a+1];
      v110 = _voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a];
      v111 = _voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a+1];

      v = v000*(1-ax)*(1-by)*(1-cz)                 \
        + v100*ax*(1-by)*(1-cz)                     \
        + v010*(1-ax)*y*(1-cz)                      \
        + v001*(1-ax)*(1-by)*cz                     \
        + v101*ax*(1-by)*cz                         \
        + v011*(1-ax)*by*cz                         \
        + v110*ax*by*(1-cz)                         \
        + v111*ax*by*cz;
    }
  else if (a != _sizeX - 1 && b != _sizeY - 1 && c == _sizeZ - 1)
    {
      // 2-D Bilinear interp
      v010 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a];
      v100 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v110 = _voxel[(c+1)*_sizeX*_sizeY + (b+1)*_sizeX + a];

      v = v000*(1 - ax)*(1 - by)               \
        + v100*ax*(1 - by)                     \
        + v010*(1 - ax)*by                     \
        + v110*ax*by;
    }
  else if (a != _sizeX - 1 && b == _sizeY - 1 && c != _sizeZ - 1)
    {
      // 2-D Bilinear interp
      v001 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v100 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v101 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a+1];

      v = v000*(1 - ax)*(1 - cz)               \
        + v001*(1 - ax)*cz                     \
        + v100*ax*(1 - cz)               \
        + v101*ax*cz;
    }
  else if (a == _sizeX - 1 && b != _sizeY - 1 && c != _sizeZ - 1)
    {
      v001 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];
      v010 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a];
      v011 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a+1];

      v = v000*(1 - by)*(1 - cz)               \
        + v001*(1 - by)*cz                     \
        + v010*by*(1 - cz)               \
        + v011*by*cz;
    }
  else if (a == _sizeX - 1 && b == _sizeY - 1 && c != _sizeZ - 1)
    {
      // 1-D linear interp
      v001 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];

      v = v000*(1 - cz) + v001*cz;
    }
  else if (a == _sizeX - 1 && b != _sizeY - 1 && c == _sizeZ - 1)
    {
      v010 = _voxel[c*_sizeX*_sizeY + (b+1)*_sizeX + a];
      
      v = v000*(1 - by) + v010*by;
    }
  else if (a != _sizeX - 1 && b == _sizeY - 1 && c == _sizeZ - 1)
    {
      v100 = _voxel[(c+1)*_sizeX*_sizeY + b*_sizeX + a];

      v = v000*(1 - ax) + v100*ax;
    }
  else // (a == _sizeX - 1 && b == _sizeY - 1 && c == _sizeZ - 1)
    {
      v = v000;
    }

  return v;
}




voxelType* cVolume::getVoxelPointer()
{
  if (!_voxel)
    {
      std::cerr<<"volume not initialized. abort"<<std::endl;
      exit(1);
    }

  return _voxel;

}
