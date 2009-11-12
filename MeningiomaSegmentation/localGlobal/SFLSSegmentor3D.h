#ifndef SFLSSegmentor3D_h_
#define SFLSSegmentor3D_h_

#include "SFLS.h"

#include <list>

//douher
//#include "cArray3D.h"

//itk
#include "itkImage.h"

template< typename TPixel >
class CSFLSSegmentor3D : public CSFLS
{
public:
  virtual ~CSFLSSegmentor3D() {}

  typedef CSFLSSegmentor3D< TPixel > Self;
  //typedef boost::shared_ptr< Self > Pointer;

  typedef itk::Image<TPixel, 3> ImageType;
  typedef itk::Image<double, 3> LSImageType;
  typedef itk::Image<char, 3> LabelImageType;
  typedef itk::Image<unsigned char, 3> MaskImageType;

  CSFLSSegmentor3D();

  /* New */
  //static Pointer New() { return Pointer(new Self); }


  /* ============================================================
   * functions         */
  void basicInit();

  void setNumIter(unsigned long n);

  void setImage(typename ImageType::Pointer img);
  void setMask(typename MaskImageType::Pointer mask);

  virtual void computeForce() = 0;

  void normalizeForce();

  //     double maxPhi(long ix, long iy, long iz, double level);
  //     double minPhi(long ix, long iy, long iz, double level);
  bool getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(long ix, long iy, long iz, double& thePhi);

  void oneStepLevelSetEvolution();

  void getSFLSFromPhi();

  void initializeSFLS() { initializeSFLSFromMask(); }
  void initializeSFLSFromMask();

  void initializeLabel();
  void initializePhi();

  void doSegmenation();


  // geometry
  double computeKappa(long ix, long iy, long iz);
    



  /* ============================================================
   * data     */
  // CSFLS::Pointer mp_sfls;

  typename ImageType::Pointer mp_img;
  typename LabelImageType::Pointer mp_label;
  typename MaskImageType::Pointer mp_mask;
  typename LSImageType::Pointer mp_phi;

  long m_nx;
  long m_ny;
  long m_nz;

  double m_dx;
  double m_dy;
  double m_dz;

  std::list< double > m_force;

  double m_timeStep;

  unsigned long m_numIter;

protected:
  inline bool doubleEqual(double a, double b, double eps = 1e-10)
  {
    return (a-b < eps && b-a < eps);
  }

  /*----------------------------------------------------------------------
    These two record the pts which change status 

    Because they are created and visited sequentially, and when not
    needed, are clear-ed as a whole. No random insertion or removal is
    needed. So use vector is faster than list.  */ 
  CSFLSLayer m_lIn2out;
  CSFLSLayer m_lOut2in;
};




#include "SFLSSegmentor3D.hpp"


#endif
