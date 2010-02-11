#include "imgRegByPtSet3D_uchar.h"

#include <csignal>
#include <iostream>

#include "itkImageRegionIterator.h"

#include "imageProcessing/imageProcessing.h"




/*============================================================*/
CImgRegByPtSet3D_uchar::CImgRegByPtSet3D_uchar() : CImgRegByPtSet3D() 
{
}

/*============================================================*/
CImgRegByPtSet3D_uchar::CImgRegByPtSet3D_uchar(DoubleImage3DPointerType fixedImg, DoubleImage3DPointerType movingImg) \
  : CImgRegByPtSet3D(fixedImg, movingImg) 
{
}


/*============================================================*/
UCHARImage3DPointerType CImgRegByPtSet3D_uchar::getRegisteredMovingImage()
{
  if (m_computationDone == false)
    {
      std::cerr<<"computation not done, abort.\n";
      raise(SIGABRT);
    }

  double fillValue = 0;
  unsigned char interpType = 1;

  DoubleImage3DPointerType resultImg = douher::affineTransform3< double >(m_movingImage, \
                                                                          m_finalAffineMatrix, m_finalTranslationVector, \
                                                                          fillValue, interpType);


  // constuct uchar image by thresholding double image
  UCHARImage3DPointerType newImage = UCHARImage3DType::New();
  newImage->SetRegions( resultImg->GetLargestPossibleRegion() );
  newImage->CopyInformation(resultImg);
  //  newImage->SetSpacing( m_movingImage->GetSpacing() );
  newImage->SetSpacing( m_fixedImage->GetSpacing() ); // tmp change
  newImage->Allocate();

  typedef itk::ImageRegionIterator< DoubleImage3DType > DoubleIteratorType;
  DoubleIteratorType resultImgIter(resultImg, resultImg->GetLargestPossibleRegion());

  typedef itk::ImageRegionIterator< UCHARImage3DType > IteratorType;
  IteratorType imIter(newImage, newImage->GetLargestPossibleRegion());

  for (imIter.GoToBegin(), resultImgIter.GoToBegin(); \
       !resultImgIter.IsAtEnd(); \
       ++imIter, ++resultImgIter)
    {
      double v = resultImgIter.Get();

      if (v > 0)
        {
          imIter.Set(1);
        }
      else
        {
          imIter.Set(0);
        }
    }

  return newImage;
}


