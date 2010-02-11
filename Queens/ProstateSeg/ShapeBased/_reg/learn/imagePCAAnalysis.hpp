#ifndef imagePCAAnalysis3D_hpp_
#define imagePCAAnalysis3D_hpp_


#include <iostream>
#include <csignal>


#include "imagePCAAnalysis.h"

// douher
#include "cArrayOp.h"
#include "shuxue.h"
#include "imageProcessing.h"

namespace douher
{
  //////////////////////////////////////////////////////////// 
  template< typename TData >
  CImagePCAAnalysis3D<TData>::CImagePCAAnalysis3D()
  {
    m_N = 0;
    m_d = 0;

    m_pcaDone = false;
  }


  template< typename TData >
  void 
  CImagePCAAnalysis3D<TData>::setImageSeries(TImageSeries imageSeries)
  {
    if (imageSeries.empty() )
      {
        std::cerr<<"Error: imageSeries is empty.\n";
        raise(SIGABRT);
      }


    // get the image size from the first image.
    typename TItkImage::SizeType sizeOfFirstImage = imageSeries[0]->GetLargestPossibleRegion().GetSize();

    for (int id = 0; id < m_imageDimension; ++id)
      {
        m_imageSize[id] = sizeOfFirstImage[id];
      }
  

    // check if the images are of the same sizes
    m_N = imageSeries.size();

    for (long i = 1; i < m_N; ++i)
      {
        typename TItkImage::SizeType sizeOfNewImage = imageSeries[i]->GetLargestPossibleRegion().GetSize();
      
        if ((long)sizeOfNewImage[0] != m_imageSize[0] \
            || (long)sizeOfNewImage[1] != m_imageSize[1] \
            || (long)sizeOfNewImage[2] != m_imageSize[2])
          {
            std::cerr<<"Error: size of the new vector doesn't match those in the list.\n";
            raise(SIGABRT);
          }
      }

    // okay
    m_imageList = imageSeries;
  
    return;
  }


  ////////////////////////////////////////////////////////////
  template< typename TData >
  void 
  CImagePCAAnalysis3D<TData>::gogogo()
  {
    // 10. port images in the list to pca
    CpcaAnalysis<TData> pca;  

    for (int i = 0; i < m_N; ++i )
      {
        typename TItkImage::Pointer thisImage = m_imageList[i];

        TVnlVector vectorForThisImage = douher::itkImageToVnlVector<double, m_imageDimension>(thisImage);

//         //debug//
//         std::cout<<"The max and min value of "<<i<<"-th image is: "\
//                  <<vectorForThisImage.min_value()<<"\t"<<vectorForThisImage.min_value()<<std::endl;
//         //debug//

        pca.addData(vectorForThisImage);      
      }

    // 20. run pca
    pca.gogogo();


    // 30. port mean/eigen vectors to images
    // 33. port the eigen values
    m_eigenValues = pca.m_eigenValues;
  

    // 35. port the mean image
    TVnlVector vctMean = pca.getMean();
    m_meanImage = douher::vnlVectorToItkImage<TData, m_imageDimension>(vctMean, m_imageSize);
  
    // 37. port the eigen images
    for (int i = 0; i < m_N; ++i )
      {
        TVnlVector eigenImgVec = pca.getEigenMode(i);
        typename TItkImage::Pointer eigenImg = douher::vnlVectorToItkImage<TData, 3>(eigenImgVec, m_imageSize);
        m_eigenImageList.push_back(eigenImg);
      }

    m_pcaDone = true;

    return;
  }



  ////////////////////////////////////////////////////////////
  template< typename TData >
  typename CImagePCAAnalysis3D<TData>::TItkImage::Pointer
  CImagePCAAnalysis3D<TData>::getMeanImage()
  {
    if (!m_pcaDone)
      {
        std::cerr<<"pca not done. abort\n";
        raise(SIGABRT);
      }

    return m_meanImage;
  }

  ////////////////////////////////////////////////////////////
  template< typename TData >  
  typename CImagePCAAnalysis3D<TData>::TItkImage::Pointer
  CImagePCAAnalysis3D<TData>::getEigenImage( long i)
  {
    if (!m_pcaDone)
      {
        std::cerr<<"pca not done. abort\n";
        raise(SIGABRT);
      }
  
    if (i >= m_N)
      {
        std::cerr<<"at most "<<m_N<<" eigen modes. abort\n";
        raise(SIGABRT);      
      }

    return m_eigenImageList[i];
  }

  ////////////////////////////////////////////////////////////
  template< typename TData >
  double 
  CImagePCAAnalysis3D<TData>::getEigenValue(long i)
  {
    if (!m_pcaDone)
      {
        std::cerr<<"pca not done. abort\n";
        raise(SIGABRT);
      }
  
    if (i >= m_N)
      {
        std::cerr<<"at most "<<m_N<<" eigen values. abort\n";
        raise(SIGABRT);      
      }

    return m_eigenValues[i];
  }

} //doher

#endif
