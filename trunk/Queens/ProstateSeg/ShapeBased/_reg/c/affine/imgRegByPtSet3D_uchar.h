#ifndef imgRegByPtSet3D_uchar_h_
#define imgRegByPtSet3D_uchar_h_

#include "imgRegByPtSet3D.h"


class CImgRegByPtSet3D_uchar : public CImgRegByPtSet3D
{
public:
  CImgRegByPtSet3D_uchar();
  CImgRegByPtSet3D_uchar(DoubleImage3DPointerType fixedImg, DoubleImage3DPointerType movingImg);

  UCHARImage3DPointerType getRegisteredMovingImage();  // just reload this
};

#endif
