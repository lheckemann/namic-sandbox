#ifndef SFLSLocalChanVeseSegmentor3D_h_
#define SFLSLocalChanVeseSegmentor3D_h_

#include "SFLSSegmentor3D.h"

#include <list>


template< typename TPixel >
class CSFLSLocalChanVeseSegmentor3D : public CSFLSSegmentor3D< TPixel >
{
public:
  typedef CSFLSLocalChanVeseSegmentor3D< TPixel > Self;
  //    typedef boost::shared_ptr< Self > Pointer;
  //    typedef Pointer PointerType;

  typedef CSFLSSegmentor3D< TPixel > SuperClassType;


  /*================================================================================
    ctor */
  CSFLSLocalChanVeseSegmentor3D() : CSFLSSegmentor3D< TPixel >()
  {
    basicInit();
  }

  void basicInit();


  void setCurvatureWeight(double a) 
  {
    m_curvatureWeight = a;
  }

  void setNBHDSize(long nbx, long nby, long nbz)
  {
    m_nbx = nbx;
    m_nby = nby;
    m_nbz = nbz;
  }

  void setExpectedVolume(double v);


  // data
  double m_curvatureWeight;

  long m_nbx, m_nby, m_nbz;

  long m_areaIn;
  long m_areaOut;

  double m_meanIn;
  double m_meanOut;

  long m_voxelCountIn;

  /* ============================================================
   * functions
   * ============================================================*/
  void computeMeans();
  void computeMeansAt(long ix, long iy, long iz);

  void doLocalChanVeseSegmenation();
    

  /* ============================================================
     computeForce    */
  void computeForce();


private:
  //  void adjustCurvatureWeight();

  void countInsideVoxel();
  void updateCountInsideVoxel();

  double m_expectedPhysicalVolume;

};



#include "SFLSLocalChanVeseSegmentor3D.hpp"

#endif
