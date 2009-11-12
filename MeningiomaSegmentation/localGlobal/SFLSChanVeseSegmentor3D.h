#ifndef SFLSChanVeseSegmentor3D_h_
#define SFLSChanVeseSegmentor3D_h_

#include "SFLSSegmentor3D.h"

#include <list>


//douher
//#include "cArray3D.h"

template< typename TPixel >
class CSFLSChanVeseSegmentor3D : public CSFLSSegmentor3D< TPixel >
{
public:
  typedef CSFLSSegmentor3D< TPixel > SuperClassType;

  //    typedef boost::shared_ptr< CSFLSChanVeseSegmentor3D< TPixel > > Pointer;

  /*================================================================================
    ctor */
  CSFLSChanVeseSegmentor3D() : CSFLSSegmentor3D< TPixel >()
  {
    basicInit();
  }

  void basicInit();

  //     /* ============================================================
  //        New    */
  //     static Pointer New() 
  //     {
  //       return Pointer(new CSFLSChanVeseSegmentor3D< TPixel >);
  //     }

  void setCurvatureWeight(double a) 
  {
    m_curvatureWeight = a;
  }

  void setMaxVolxel(long v) 
  {
    if (v < 1)
      {
        std::cerr<<"Error: max volxel count < 1???\n";
        raise(SIGABRT);
      }

    m_maxVoxelNumber = v;
    m_maxVolume = m_maxVoxelNumber*(this->m_dx)*(this->m_dy)*(this->m_dz);
  }

  void setMaxVolume(double v)
  {
    if (v < 0)
      {
        std::cerr<<"Error: max volume < 0???\n";
        raise(SIGABRT);
      }

    m_maxVolume = v;
    m_maxVoxelNumber = static_cast<long>(round( (m_maxVolume/(this->m_dx)/(this->m_dy)/(this->m_dz)) );
  }


  // data
  double m_curvatureWeight;

  double m_areaIn;
  double m_areaOut;

  double m_meanIn;
  double m_meanOut;

  long m_maxVoxelNumber;
  double m_maxVolume;


  /* ============================================================
   * functions
   * ============================================================*/
  void computeMeans();
  void updateMeans();

  void doChanVeseSegmenation();


  /* ============================================================
     computeForce    */
  void computeForce();

};


#include "SFLSChanVeseSegmentor3D.hpp"

#endif
