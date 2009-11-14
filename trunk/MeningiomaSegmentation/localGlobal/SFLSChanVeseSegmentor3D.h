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

  void setExpectedVolume(double v);


  // data
  double m_curvatureWeight;

  double m_areaIn;
  double m_areaOut;

  double m_meanIn;
  double m_meanOut;



  /* ============================================================
   * functions
   * ============================================================*/
  void computeMeans();
  void updateMeans();

  void doChanVeseSegmenation();


  /* ============================================================
     computeForce    */
  void computeForce();

private:
  //void adjustCurvatureWeight();

  double m_expectedPhysicalVolume;

};


#include "SFLSChanVeseSegmentor3D.hpp"

#endif
