#ifndef imagePCAAnalysis3D_h_
#define imagePCAAnalysis3D_h_


#include <vector>

#include "itkImage.h"

#include "pcaAnalysis.h"

namespace douher
{
  template< typename TData >
  class CImagePCAAnalysis3D
  {
    /* TData must be double, coz the eigen decomposition in vnl only
       takes double. Well one could load say int image and convert
       them to double when converting image to vnl vector, but I will
       save the trouble for now.
    */

  public:
    CImagePCAAnalysis3D();
    //  ~CImagePCAAnalysis();

    static const int m_imageDimension = 3;

    typedef vnl_vector<double> TVnlVector;
    typedef vnl_matrix<double> TVnlMatrix;

    typedef itk::Image< TData, m_imageDimension > TItkImage;

    typedef std::vector< typename TItkImage::Pointer > TImageSeries;

    //  void addImage(typename TItkImage::Pointer image);
    void setImageSeries(TImageSeries imageSeries);

    void gogogo();

    double getEigenValue(long i);
    typename TItkImage::Pointer getMeanImage();
    typename TItkImage::Pointer getEigenImage( long i);
  
    // long getNumberOfEigenModes() { return m_N>m_d?m_d:m_N; }


    // data

    TImageSeries m_imageList;
    //  TVnlMatrix m_matrixOfAllData; // each column is a long vector of data



    // result images
    TVnlVector m_eigenValues;

    std::vector< typename TItkImage::Pointer > m_eigenImageList;
    typename TItkImage::Pointer m_meanImage;
  
    //  bool m_analysisStarted;
    bool m_pcaDone;


    long m_N; // total number of data
    long m_d; // dimension of every datum
    long m_imageSize[m_imageDimension];
  };

}//douher


#include "imagePCAAnalysis.hpp"


#endif
