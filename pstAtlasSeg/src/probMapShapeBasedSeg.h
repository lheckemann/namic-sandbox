#ifndef probMapShapeBasedSeg_h_
#define probMapShapeBasedSeg_h_

/*
  Given:

  20100424 create
  20100424 last modified

  Yi Gao
*/



// itk
#include "itkImage.h"

// boost
#include "boost/shared_ptr.hpp"

// local
#include "shapeProjector.h"
#include "SFLSRobustStatSegmentor3DProbMap.h"

namespace newProstate
{
  template<typename short_t, typename float_t>
  class probMapShapeBasedSeg : public shapeProjector<float_t>, public CSFLSRobustStatSegmentor3DProbMap<short_t>
  {
  public:
    typedef probMapShapeBasedSeg< short_t, float_t > Self;
    typedef boost::shared_ptr< Self > Pointer;


    ~probMapShapeBasedSeg() {}

    /* New */
    static Pointer New() { return Pointer(new Self); }


    void doShapeBasesSeg();


  protected:

    /**
     * init this class
     */
    //void init();

    probMapShapeBasedSeg() {}
  };


} //namespace

#include "probMapShapeBasedSeg.txx"


#endif
