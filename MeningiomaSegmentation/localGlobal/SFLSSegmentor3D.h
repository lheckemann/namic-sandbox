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

  typedef CSFLSSegmentor3D< TPixel > Self;
  //typedef boost::shared_ptr< Self > Pointer;

  typedef itk::Image<TPixel, 3> TImage;
  typedef itk::Image<double, 3> TDoubleImage;
  typedef itk::Image<char, 3> TCharImage;
  typedef itk::Image<unsigned char, 3> TUCharImage;

  typedef TImage ImageType;
  typedef TDoubleImage LSImageType;
  typedef TCharImage LabelImageType;
  typedef TUCharImage MaskImageType;


  typedef typename TImage::IndexType TIndex;
  typedef typename TImage::SizeType TSize;
  typedef typename TImage::RegionType TRegion;

  CSFLSSegmentor3D();
  virtual ~CSFLSSegmentor3D() {}

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

  //void getSFLSFromPhi();

  void initializeSFLS() { initializeSFLSFromMask(); }
  void initializeSFLSFromMask(); // m_insideVoxelCount is first computed here

  void initializeLabel();
  void initializePhi();

  virtual void doSegmenation() = 0;

  // geometry
  double computeKappa(long ix, long iy, long iz);


  void setMaxVolume(double v); // v is in mL
  void setMaxRunningTime(double t); // t in min

    
  void setCurvatureWeight(double a);

  LSImageType::Pointer getLevelSetFunction();

  /* ============================================================
   * data     */
  // CSFLS::Pointer mp_sfls;

  typename ImageType::Pointer mp_img;
  typename LabelImageType::Pointer mp_label;
  typename MaskImageType::Pointer mp_mask; // 0, non-0 mask for object
  typename LSImageType::Pointer mp_phi;

  std::list< double > m_force;

  double m_timeStep;

  unsigned long m_numIter;

protected:
  double m_curvatureWeight;

  bool m_done;

  long m_nx;
  long m_ny;
  long m_nz;


  double m_dx; // in mm
  double m_dy; // in mm
  double m_dz; // in mm

  long m_insideVoxelCount;
  double m_insideVolume;

  double m_maxVolume; // max physical volume, in mm^3
  double m_maxRunningTime; // in sec


  /*----------------------------------------------------------------------
    These two record the pts which change status 

    Because they are created and visited sequentially, and when not
    needed, are clear-ed as a whole. No random insertion or removal is
    needed. So use vector is faster than list.  */ 
  CSFLSLayer m_lIn2out;
  CSFLSLayer m_lOut2in;


  void updateInsideVoxelCount();


  inline bool doubleEqual(double a, double b, double eps = 1e-10)
  {
    return (a-b < eps && b-a < eps);
  }


};




#include "SFLSSegmentor3D.txx"


#endif