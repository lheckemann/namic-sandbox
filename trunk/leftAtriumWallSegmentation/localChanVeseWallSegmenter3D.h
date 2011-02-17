#ifndef localChanVeseWallSegmenter3D_h_
#define localChanVeseWallSegmenter3D_h_

#include "wallSegmenter3D.h"

#include <list>


template< typename TPixel >
class CLocalChanVeseWallSegmenter3D : public CWallSegmenter3D< TPixel >
{
public:
  typedef CWallSegmenter3D< TPixel > SuperClassType;

  typedef CLocalChanVeseWallSegmenter3D< TPixel > Self;
  //    typedef boost::shared_ptr< Self > Pointer;
  //    typedef Pointer PointerType;

  typedef typename SuperClassType::NodeType NodeType;
  typedef typename SuperClassType::CSFLSLayer CSFLSLayer;
  typedef typename SuperClassType::MaskImageType MaskImageType;
  typedef typename SuperClassType::TFloatImage floatImage_t;

  /*================================================================================
    ctor */
  CLocalChanVeseWallSegmenter3D() : CWallSegmenter3D< TPixel >()
  {
    basicInit();
  }

  void basicInit();


  void setNBHDSize(long nbx, long nby, long nbz)
  {
    m_nbx = nbx;
    m_nby = nby;
    m_nbz = nbz;
  }

  // data
  long m_nbx, m_nby, m_nbz;

  long m_areaIn;
  long m_areaOut;

  double m_meanIn;
  double m_meanOut;


  /* ============================================================
   * functions
   * ============================================================*/
  void computeMeans();
  void computeMeansAt(long ix, long iy, long iz);

  void doSegmenation();
  //void doLocalChanVeseSegmenation();
    
  void initializeSFLS();

  /* ============================================================
     computeForce    */
  void computeForce();



  void setInflation(float f) {m_globalInflation = f;}

protected:

  float m_globalInflation;

  /**
   * The weight between 
   */
  double m_GACWeight;

  double computeTwoTermsInGAC(long ix, long iy, long iz);

  

};



#include "localChanVeseWallSegmenter3D.txx"

#endif //localChanVeseWallSegmenter3D
