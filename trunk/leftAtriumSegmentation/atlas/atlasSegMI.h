#ifndef atlasSegMI_h_
#define atlasSegMI_h_

#include "itkImage.h"


namespace laSeg
{
  template<typename raw_image_t, typename label_image_t>
  typename itk::Image<float, 3>::Pointer
  atlasSegMI_ROI_weight(typename raw_image_t::Pointer rawImg,               \
                        const typename std::vector<typename raw_image_t::Pointer>& trainingImageList, \
                        const typename std::vector<typename label_image_t::Pointer>& labelImageList);



  /**
   * Aux functions
   */
  template<typename image_t>
  typename image_t::Pointer
  extractROI(typename image_t::Pointer img, typename image_t::RegionType region);

  /** 
   * Generate an all-zero image the same size/origin/spacing/etc. as
   * referenceImg, inside of whick, the roiRegion is the roiImg
   */
  template<typename image_t>
  typename image_t::Pointer
  antiExtractROI(typename image_t::ConstPointer roiImg,         \
                 const typename image_t::RegionType roiRegion,  \
                 typename image_t::ConstPointer referenceImg);



  template<typename image_t>
  typename image_t::RegionType
  computeNonZeroRegion(typename image_t::Pointer img);

  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegion(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion);


  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegionByOnePixel(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion);


}


#include "atlasSegMI.txx"


#endif
