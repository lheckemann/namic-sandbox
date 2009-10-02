#ifndef SFLS_h_
#define SFLS_h_




//douher
#include "SFLSLayer.h"


  class CSFLS
  {
  public:
    typedef CSFLS Self;
    //typedef boost::shared_ptr< Self > Pointer;

    // ctor
    CSFLS()
    {
//       m_lz.reset(new CSFLSLayer);
//       m_ln1.reset(new CSFLSLayer);
//       m_ln2.reset(new CSFLSLayer);
//       m_lp1.reset(new CSFLSLayer);
//       m_lp2.reset(new CSFLSLayer);
    }
  
    //   static Pointer New() 
    //   {
    //     return Pointer(new CSFLS);
    //   }

    CSFLSLayer m_lz;
    CSFLSLayer m_ln1;
    CSFLSLayer m_ln2;
    CSFLSLayer m_lp1;
    CSFLSLayer m_lp2;
  };

#endif
