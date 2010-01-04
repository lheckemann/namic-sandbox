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

  typedef typename SuperClassType::NodeType NodeType;
  typedef typename SuperClassType::CSFLSLayer CSFLSLayer;


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

  double m_areaIn;
  double m_areaOut;

  double m_meanIn;
  double m_meanOut;


  /* ============================================================
   * functions
   * ============================================================*/
  void computeMeans();
  void updateMeans();

  //void doChanVeseSegmenation();
  void doSegmenation();


  /* ============================================================
     computeForce    */
  void computeForce();

};


#include "SFLSChanVeseSegmentor3D.txx"

#endif
