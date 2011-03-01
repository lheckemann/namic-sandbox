#ifndef SFLSMultiSegmentor3D_h_
#define SFLSMultiSegmentor3D_h_

#include "SFLS.h"

#include <list>
#include <map>


//itk
#include "itkImage.h"

template< typename TPixel >
class SFLSMultiSegmentor3D_c
{
public:

  typedef SFLSMultiSegmentor3D_c< TPixel > Self;

  typedef CSFLS::NodeType NodeType;
  typedef CSFLS::CSFLSLayer CSFLSLayer;
  

  typedef itk::Image<TPixel, 3> TImage;
  typedef itk::Image<float, 3> TFloatImage;
  typedef itk::Image<double, 3> TDoubleImage;
  typedef itk::Image<char, 3> TCharImage;
  typedef itk::Image<unsigned char, 3> TUCharImage;
  typedef itk::Image<short, 3> shortImage_t;

  typedef TImage ImageType;
  typedef TFloatImage LSImageType;

  // This label is used internally for SFLS indicating -3~3. So char
  // is enough. This is not for the label map user provided for
  // multiple objects. There char may not be enough. So we call this
  // type as "LabelImage" and call the user input label as "Label Map"
  typedef TCharImage LabelImageType; 

  // This is used for user provided init label map. called "Label Map"
  // to differentiate with the "Label Image" which is used internally
  // in SFLS above.
  typedef shortImage_t labelMap_t;


  typedef typename TImage::IndexType TIndex;
  typedef typename TImage::SizeType TSize;
  typedef typename TImage::RegionType TRegion;

  SFLSMultiSegmentor3D_c();
  virtual ~SFLSMultiSegmentor3D_c() {}

  /* New */
  //static Pointer New() { return Pointer(new Self); }


  /* ============================================================
   * functions         */
  void basicInit();

  void setNumIter(unsigned long n);

  void setImage(typename ImageType::Pointer img);

  void setCurvatureWeight(double a) {m_curvatureWeight = a;} // all object use this curvature factor


  /* Only after this, we can decide how many objects are going to be
     segmented. So to decide the length of curvatureWeight, m_phiList,
     m_forceList etc.
  */
  void setLabelMap(typename labelMap_t::Pointer labelMap);

  virtual void computeForce() = 0;
  virtual void computeForce(short labelId) = 0;

  void normalizeForce();
  void normalizeForce(short labelId);

  bool getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(short labelId, \
                                                                      long ix, long iy, long iz, double& thePhi);

  void oneStepLevelSetEvolution();
  void oneStepLevelSetEvolution(short labelId);

  void initializeSFLS() { initializeSFLSFromLabelMap(); }
  void initializeSFLSFromLabelMap();
  void initializeIthSFLSFromLabelMap(short i);

  void initializeLabel();
  void initializePhi();

  virtual void doSegmenation() = 0;


  //LSImageType::Pointer getLevelSetFunction();

  /* ============================================================
   * data     */
  // CSFLS::Pointer mp_sfls;
  typename ImageType::Pointer mp_img;

  std::vector< typename LSImageType::Pointer> m_phiList;
  std::vector< typename LabelImageType::Pointer> m_labelList;
  std::vector< typename labelMap_t::Pointer> m_finalLabelList;

  std::vector< std::vector< double > > m_forceList;

  double m_timeStep;

  unsigned long m_numIter;

  short getNumberOfObjects() {return m_numOfObjects;}

  // aux
  void getFinalLabelList();
  typename labelMap_t::Pointer getFinalLabelMap();


protected:
  double m_curvatureWeight;

  typename labelMap_t::Pointer m_labelMap; // 0 for bkgd, different labels for different objects.

  std::vector<double> m_curvatureWeightList;

  bool m_done;

  long m_nx;
  long m_ny;
  long m_nz;


  double m_dx; // in mm
  double m_dy; // in mm
  double m_dz; // in mm

  short m_numOfObjects;
  std::vector<short> m_objectLabelList;
  std::map<short, short> m_labelValueToIndexMap; // first=labelValue, second=its idx in m_objectLabelList

  inline bool doubleEqual(double a, double b, double eps = 1e-10)
  {
    return (a-b < eps && b-a < eps);
  }

  // geometry
  double computeKappa(short labelId, long ix, long iy, long iz);

  void preprocessLableMap();

  void initVariablesAccordingToLabelMap();

  std::vector<CSFLS> m_sflsList;

  bool m_labelMapPreprocessed;

};




#include "SFLSMultiSegmentor3D.txx"


#endif
