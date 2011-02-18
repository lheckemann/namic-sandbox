#ifndef wallSegmenter3D_h_
#define wallSegmenter3D_h_

#include "SFLSSegmentor3D.h"

#include <list>


template< typename TPixel >
class CWallSegmenter3D : public CSFLSSegmentor3D< TPixel >
{
public:
  typedef CSFLSSegmentor3D< TPixel > SuperClassType;

  typedef CWallSegmenter3D< TPixel > Self;
  //    typedef boost::shared_ptr< Self > Pointer;
  //    typedef Pointer PointerType;

  typedef typename SuperClassType::NodeType NodeType;
  typedef typename SuperClassType::CSFLSLayer CSFLSLayer;
  typedef typename SuperClassType::MaskImageType MaskImageType;
  typedef typename SuperClassType::TFloatImage floatImage_t;


  /*================================================================================
    ctor */
  CWallSegmenter3D() : CSFLSSegmentor3D< TPixel >()
  {
    basicInit();
  }

  void basicInit();


  /**
   * Reload the function. In this one, besides the basic function
   * performed in that of the base class, also we build the distance
   * function for the interior. However, as I recall the ITK distance
   * map filter does NOT consider the spacing. This could be a
   * problem, but let's see.
   */
  void setMask(typename MaskImageType::Pointer mask);


  /** Output final mask corresponding to the wall */
  //typename MaskImageType::Pointer getWallMask();
  itk::Image<unsigned char, 3>::Pointer getWallMask();


  //protected:
  typename MaskImageType::Pointer mp_maskOfInterior; // 0, non-0 mask for object
  typename floatImage_t::Pointer mp_distanceMapOfInterior;
  typename floatImage_t::Pointer mp_metricFromDistanceMapOfInterior;


protected:
  void computeMetricFromDistanceMap();


  typename MaskImageType::Pointer mp_maskOfWall; // 1 for mask of wall, 0 for background
  
  double m_sigmoidAlpha;
  double m_sigmoidBeta;

  double m_meanWallThickness;

};



#include "wallSegmenter3D.txx"

#endif //wallSegmenter3D_h_
