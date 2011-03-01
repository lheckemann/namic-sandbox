#ifndef SFLSMultiRobustStatSegmentor3D_h_
#define SFLSMultiRobustStatSegmentor3D_h_

#include "SFLSMultiSegmentor3D.h"

#include <list>
#include <vector>

template< typename TPixel >
class SFLSMultiRobustStatSegmentor3D_c : public SFLSMultiSegmentor3D_c< TPixel >
{
  /*---------------------------------------------------------------------- 
    just copy, not logic change */

public:
  typedef SFLSMultiSegmentor3D_c< TPixel > SuperClassType;

  typedef SFLSMultiRobustStatSegmentor3D_c< TPixel > Self;

  typedef typename SuperClassType::NodeType NodeType;
  typedef typename SuperClassType::CSFLSLayer CSFLSLayer;



  /*================================================================================
    ctor */
  SFLSMultiRobustStatSegmentor3D_c() : SFLSMultiSegmentor3D_c< TPixel >()
  {
    basicInit();
  }


  void basicInit();

  /* just copy, not logic change
     ----------------------------------------------------------------------
     ----------------------------------------------------------------------
     ----------------------------------------------------------------------
     ---------------------------------------------------------------------- */

  typedef typename SuperClassType::TCharImage TLabelImage;
  typedef typename TLabelImage::Pointer TLabelImagePointer;

  typedef typename SuperClassType::TFloatImage TFloatImage;
  typedef typename TFloatImage::Pointer TFloatImagePointer;

  typedef typename SuperClassType::TDoubleImage TDoubleImage;
  typedef typename TDoubleImage::Pointer TDoubleImagePointer;

  typedef typename SuperClassType::labelMap_t labelMap_t;

  //typedef typename SuperClassType::MaskImageType TMaskImage;

  typedef typename SuperClassType::TIndex TIndex;
  typedef typename SuperClassType::TSize TSize;
  typedef typename SuperClassType::TRegion TRegion;

  /* ============================================================
   * functions
   * ============================================================*/

  void setSeeds(const std::vector< std::vector<std::vector<long> > >& multipleSeedLists) 
  {
    m_multipleSeedLists = multipleSeedLists;
  }

  void doSegmenation();

  void computeForce();
  void computeForce(short labelId);

  void setKernelWidthFactor(double f);
  void setIntensityHomogeneity(double h);

protected:
  /* data */

  //std::vector<std::vector<long> > m_seeds; // in IJK
  std::vector< std::vector<std::vector<int> > > m_multipleSeedLists;  // in IJK

  std::vector<std::vector< std::vector<double> > > m_FeatureAtMultipleSeeds;


  long m_statNeighborX;
  long m_statNeighborY;
  long m_statNeighborZ;

  const static short m_numberOfFeature = 3;
  /* Store the robust stat as the feature at each point 
     0: Meadian
     1: interquartile range (IRQ)
     2. median absolute deviation (MAD)
  */
  TLabelImagePointer m_featureComputed; // if feature at this point is computed, then is 1
  std::vector<TDoubleImagePointer> m_featureImageList;


  double m_kernelWidthFactor; // kernel_width = empirical_std/m_kernelWidthFactor, Eric has it at 10.0


  /* fn */
  void initFeatureComputedImage();
  void initFeatureImage();

  //void computeFeature();  
  void computeFeatureAt(TIndex idx, std::vector<double>& f);

  void getRobustStatistics(std::vector<double>& samples, std::vector<double>& robustStat);
  //void seedToMask();
  void labelMapToSeeds();

  //void dialteSeeds();
  void getFeatureOnSeeds();
  void estimateFeatureStdDevs();

  //void getFeatureAt(TDoubleImage::IndexType idx, std::vector<double>& f);

  TPixel m_inputImageIntensityMin;
  TPixel m_inputImageIntensityMax;
  void computeMinMax();

  std::vector< std::vector< std::vector<double> > > m_PDFlearnedFromSeeds; 
  // it's like this: pdfOfAllLabels[pdfOfAllFeaturesOfThisLabel[pdfOfAllPossibleRobustStatOfThisFeaturesOfThisLabel]] 
  // or m_PDFlearnedFromSeeds[labelId/objId][featureIdx][robustStatValue]
  void estimatePDFs();


  void getThingsReady();


  // kernel 
  //std::vector<double> m_kernelStddev;
  std::vector< std::vector<double> > m_kernelStddevOfEachObj;
  double kernelEvaluation(short labelId, const std::vector<double>& newFeature);
  double kernelEvaluationUsingPDF(short labelId, const std::vector<double>& newFeature);

};


#include "SFLSMultiRobustStatSegmentor3D.txx"

#endif
