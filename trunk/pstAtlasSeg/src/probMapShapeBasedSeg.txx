#ifndef probMapShapeBasedSeg_txx_
#define probMapShapeBasedSeg_txx_

#include "probMapShapeBasedSeg.h"

//itk
#include "itkCastImageFilter.h"


//tst
#include "cArrayOp.h"

namespace newProstate
{
  template<typename short_t, typename float_t>
  void probMapShapeBasedSeg<short_t, float_t>::doShapeBasesSeg()
  {
    /**
     * 1. do RSS segmentation using probability map
     * 2. project shape
     */

    CSFLSRobustStatSegmentor3DProbMap<short_t>::doSegmenation();

    //   // tst
    //   // direct output the pdf seg result
    //   this->mp_phi->SetSpacing(this->mp_img->GetSpacing() );
    //   this->mp_phi->CopyInformation(this->mp_img );
    //   douher::writeImage3<typename SFLSPdfSegmentor3D<float_t>::LSImageType::PixelType>(this->mp_phi, "jjjj.nrrd");
    //   exit(0);
    //   // tst//



    /**
     * The mp_phi from rss segmentor is itk::Image<float, 3>, can be pluged directly into shape projector
     */
    shapeProjector<float_t>::setNewShape( this->mp_phi );

    shapeProjector<float_t>::gogogo();

    return;
  }

}

#endif
