#ifndef atlasSegMI_h_
#define atlasSegMI_h_

#include "itkImage.h"

template<typename raw_image_t, typename label_image_t>
typename label_image_t::Pointer
atlasSegMI(typename raw_image_t::Pointer rawImg,                       \
           const std::vector< std::string >& trainingImageList,        \
           const std::vector< std::string >& labelImageList);


template<typename raw_image_t>
typename itk::Image<float, 3>::Pointer
atlasSegMI_outputPr(typename raw_image_t::Pointer rawImg,                       \
                    const std::vector< std::string >& trainingImageList, \
                    const std::vector< std::string >& labelImageList);

#include "atlasSegMI.txx"


#endif
