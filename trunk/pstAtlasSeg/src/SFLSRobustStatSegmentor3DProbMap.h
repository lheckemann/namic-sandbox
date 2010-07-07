#ifndef SFLSRobustStatSegmentor3DProbMap_h_
#define SFLSRobustStatSegmentor3DProbMap_h_

// douher
#include "SFLSRobustStatSegmentor3DLabelMap.h"


// std
#include <list>
#include <vector>

// boost
#include "boost/shared_ptr.hpp"


/**
   Perform the RSS segmentation based on the probability map.

   Given the probability map, usually generated by atals segmentation,
   we use its >0.9 region to learn the pdf of the object. Then using
   the pdf to compute the likelihood. Then in the curve evolution, use
   the probability map as the prior term to compute the posterior.

   This class is inherited from CSFLSSegmentor3DLabelMap

   @author Yi Gao yi.gao@gatech.edu
*/

namespace newProstate
{
  template< typename TPixel >
  class CSFLSRobustStatSegmentor3DProbMap : public CSFLSRobustStatSegmentor3DLabelMap< TPixel >
  {
  public:
    typedef CSFLSRobustStatSegmentor3DLabelMap< TPixel > SuperClassType;

    typedef CSFLSRobustStatSegmentor3DProbMap< TPixel > Self;
    typedef boost::shared_ptr< Self > Pointer;


    typedef typename SuperClassType::NodeType NodeType;
    typedef typename SuperClassType::CSFLSLayer CSFLSLayer;

    /*================================================================================
      ctor */
    CSFLSRobustStatSegmentor3DProbMap() : CSFLSRobustStatSegmentor3DLabelMap< TPixel >()  {}


    /* New */
    static Pointer New() { return Pointer(new Self); }


    typedef typename SuperClassType::TCharImage TLabelImage;
    //typedef typename TLabelImage::Pointer TLabelImagePointer;

    typedef typename SuperClassType::TFloatImage TFloatImage;

    typedef typename SuperClassType::MaskImageType TMaskImage;

    typedef typename SuperClassType::TIndex TIndex;
    typedef typename SuperClassType::TSize TSize;
    typedef typename SuperClassType::TRegion TRegion;

    /* ============================================================
     * functions
     * ============================================================*/

    virtual void doSegmenation();

    void getThingsReady();
    virtual void computeForce();

    void setProbabilityMap(typename TFloatImage::Pointer pb);

  protected:
    /* data */
    typename TFloatImage::Pointer m_probabilityMap;

    /* fn */
    void checkProbabilityImage();
    void computeLabelMap();

    void computeMask();


  };
} //namespace

#include "SFLSRobustStatSegmentor3DProbMap.txx"

#endif
