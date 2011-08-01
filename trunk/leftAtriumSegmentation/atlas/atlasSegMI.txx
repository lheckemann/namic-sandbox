#ifndef atlasSegMI_txx_
#define atlasSegMI_txx_


#include <csignal>
#include <cmath>
#include <algorithm> // for max and min

// local
#include "atlasSegMI.h"
#include "../utilities.hxx"


// // douher
// #include "cArrayOp.h"
#include "reg_3d_affine_mi.hpp"
#include "reg_3d_bspline_mi.hpp"


// itk
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkCastImageFilter.h"

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"


namespace laSeg
{

  template<typename raw_image_t, typename label_image_t>
  typename itk::Image<float, 3>::Pointer
  atlasSegMI_ROI_weight(typename raw_image_t::Pointer rawImg,           \
                        const typename std::vector<typename raw_image_t::Pointer>& trainingImageList, \
                        const typename std::vector<typename label_image_t::Pointer>& labelImageList)
  {
    /**
     * Atlas based segmentation based on MI
     *     
     * The difference between this one and atlasSegMI_weight is that:
     * after the affine reg, the non-zero region of the transformed
     * label image is extracted. Using that region, we extract an
     * sub-image in the transformed training image (A), and a
     * sub-image in the transformed label image (B), a sum-image in
     * the target image (C). Then A is registered to C using
     * MI-bspline with 10 nodes, and B is transformed using the found
     * transformation. The transformation is Therefore the nodes are
     * denser and the computation is faster. After that, the
     * transformed B should be pad to the original size for further
     * processing.
     *
     * Refer to atlasSegMI_weight for other details.
     *
     * 20101101
     * Yi Gao
     */

    typedef float internal_pixel_t;
    typedef itk::Image<internal_pixel_t, 3> internal_image_t;


    unsigned long nTrainingImg = trainingImageList.size();
    if (nTrainingImg != labelImageList.size() )
      {
        std::cerr<<"Error: number of training images does not match number of lable images.\n";
        raise(SIGABRT);
      }


    // 2.
    typename internal_image_t::Pointer resultLabelImg = internal_image_t::New();
    resultLabelImg->SetRegions(rawImg->GetLargestPossibleRegion());
    resultLabelImg->Allocate();
    resultLabelImg->SetSpacing(rawImg->GetSpacing() );
    resultLabelImg->CopyInformation(rawImg );
    resultLabelImg->FillBuffer(0);

    typedef itk::ImageRegionConstIterator<internal_image_t> ImageRegionConstIterator_t;
    typedef itk::ImageRegionIterator<internal_image_t> ImageRegionIterator_t;


    // record transformed label images
    std::vector< typename internal_image_t::Pointer > transformedLabelImages(nTrainingImg);
    std::vector<double> registrationFinalCostFunctionalValues(nTrainingImg);



    // 1.
#ifdef USE_OMP
#pragma omp parallel for num_threads(nTrainingImg)
#endif
    for (long iTrainingImg = 0; iTrainingImg < (long)nTrainingImg; ++iTrainingImg)
      {
        //         //tst
        // #ifdef USE_OMP
        // #pragma omp critical
        // #endif
        //         {
        //           std::cout<<"   Now reg "<<trainingImageList[iTrainingImg]<<" to raw image.\n"<<std::flush;
        //         }
        //         //tst//


        typename internal_image_t::Pointer trainingImg                  \
          = laSeg::castItkImage<typename raw_image_t::PixelType, internal_pixel_t>( trainingImageList[iTrainingImg] ) ;

        // 1.1
        typename internal_image_t::Pointer labelImg                     \
          = laSeg::castItkImage<typename label_image_t::PixelType, internal_pixel_t>( labelImageList[iTrainingImg] ) ;

        // 1.2
        if (trainingImg->GetLargestPossibleRegion().GetSize() != labelImg->GetLargestPossibleRegion().GetSize())
          {
            std::cerr<<"Error: size of trainging image does not match size of label image.\n";

            raise(SIGABRT);
          }

        // 1.3
        // 1.3.1
        {
          //tst
          std::cout<<"\t registering to "<<iTrainingImg<<"-th training image.\n"<<std::flush;
          //tst//
        }


        double finalRegCost = -1;
        std::cout<<"\t\t Affine registering...\n"<<std::flush;
        std::pair<typename internal_image_t::Pointer, typename internal_image_t::Pointer> affineRegPair = \
          reg_3d_affine_mi<internal_image_t, internal_image_t, internal_image_t>(rawImg, trainingImg, labelImg, 0, finalRegCost);


        typename internal_image_t::RegionType nonzeroRegion             \
          = computeNonZeroRegion<internal_image_t>(affineRegPair.second);

        {
          //tst
          //std::cout<<"\t\t registering to "<<iTrainingImg<<"-th training image.\n"<<std::flush;
          //std::cout<<iTrainingImg<<"\t nonzeroRegion \t"<<nonzeroRegion<<std::endl<<std::flush;
          //tst//
        }

        typename internal_image_t::RegionType largerNonzeroRegion       \
          = enlargeNonZeroRegion<internal_image_t>(affineRegPair.second, nonzeroRegion);

//         typename internal_image_t::RegionType largerNonzeroRegion       \
//           = enlargeNonZeroRegionByOnePixel<internal_image_t>(affineRegPair.second, nonzeroRegion);



        // //tst
        // std::cout<<"largerNonzeroRegion \t"<<largerNonzeroRegion<<std::endl<<std::flush;
        // //tst//

        typename internal_image_t::Pointer subTrainingImage             \
          = extractROI<internal_image_t>(affineRegPair.first, largerNonzeroRegion);

        typename internal_image_t::Pointer subLabelImage                \
          = extractROI<internal_image_t>(affineRegPair.second, largerNonzeroRegion);

        typename internal_image_t::Pointer subRawImage                  \
          = extractROI<internal_image_t>(rawImg, largerNonzeroRegion);


        // //tst
        // std::cout<<"\t\t"<<largerNonzeroRegion<<std::endl;

        // char subLabelImageName[100];
        // sprintf(subLabelImageName, "subLabelImage_%ld.nrrd", iTrainingImg);

        // std::cout<<"GetLargestPossibleRegion = \n"<<subLabelImage->GetLargestPossibleRegion()<<std::endl;
        // writeImage<internal_image_t>(subLabelImage, subLabelImageName);
        // //tst//

//         char subTrainingImageName[100];
//         sprintf(subTrainingImageName, "subTrainingImage_%ld.nrrd", iTrainingImg);
//         writeImage<internal_image_t>(subTrainingImage, subTrainingImageName);
//         //tst//




// //         //tst
//         char subRawImageName[100];
//         sprintf(subRawImageName, "subRawImage_%ld.nrrd", iTrainingImg);
//         writeImage<internal_image_t>(subRawImage, subRawImageName);
// //         //tst//
// //         //tst
//         char affineRegLabelName[100];
//         sprintf(affineRegLabelName, "affineRegLabel_%ld.nrrd", iTrainingImg);
//         writeImage<internal_image_t>(affineRegPair.second, affineRegLabelName);
// //         //tst//


        std::cout<<"\t\t BSpline registering...\n"<<std::flush;
        std::pair<typename internal_image_t::Pointer, typename internal_image_t::Pointer> deformableRegPair = \
          reg_3d_bspline_mi<internal_image_t, internal_image_t, internal_image_t>(subRawImage, \
                                                                                  subTrainingImage, \
                                                                                  subLabelImage, \
                                                                                  0, \
                                                                                  10, \
                                                                                  finalRegCost);

        transformedLabelImages[iTrainingImg]                            \
          = antiExtractROI<internal_image_t>(static_cast<typename internal_image_t::ConstPointer>(deformableRegPair.second), \
                                             largerNonzeroRegion,       \
                                             static_cast<typename internal_image_t::ConstPointer>(rawImg)); // should be rawImg instead of labelImg coz the registered labelImg is in the Regin/Size of the rawImg

        registrationFinalCostFunctionalValues[iTrainingImg] = finalRegCost;

        // {
        //   //tst
        //   std::cout<<"finalRegCost = "<<finalRegCost<<std::endl<<std::flush;
        //   //tst//
        // }

        // {
        //   //tst
        //   //write intermediate result
        //   char tmp1[1000];
        //   sprintf(tmp1, "regTrainImg%ld.nrrd", iTrainingImg);
        //   writeImage<internal_image_t>(affineRegPair.first, tmp1);
        // }

        // {
        //   //tst
        //   std::cout<<"registrationFinalCostFunctionalValues["<<iTrainingImg<<"] = "<<registrationFinalCostFunctionalValues[iTrainingImg]<<std::endl;
        //   char tmp[1000];
        //   sprintf(tmp, "regLableImg%ld.nrrd", iTrainingImg);
        //   writeImage<internal_image_t>(transformedLabelImages[iTrainingImg], tmp);
        //   //tst//
        // }

      } //iTrainingImg


    /**
     * Fusion
     *
     * 1. from final cost functional values, compute the weights
     * 2. weighted sum of transformed label images
     */
    {
      // 1.
      double maxCostFnalValue = *max_element(registrationFinalCostFunctionalValues.begin(), \
                                             registrationFinalCostFunctionalValues.end()); // worst reg

      double minCostFnalValue = *min_element(registrationFinalCostFunctionalValues.begin(), \
                                             registrationFinalCostFunctionalValues.end());  // best reg

      long minCostFnalValueIndex = static_cast<long>(min_element(registrationFinalCostFunctionalValues.begin(), \
                                                                 registrationFinalCostFunctionalValues.end()) \
                                                     - registrationFinalCostFunctionalValues.begin());


      // cast to float and output
      return laSeg::castItkImage<internal_pixel_t, float>( transformedLabelImages[minCostFnalValueIndex] ) ;



      

      double costFnalRange = maxCostFnalValue - minCostFnalValue;

      //     // compute factor s.t. exp(-factor*costFnalRange) = 0.1
      //     double minus_ln01 = 2.302585093; // -ln(0.1)

      // compute factor s.t. exp(-factor*costFnalRange) = 0.2

      /**
       * for the HUVA data set, 0.2 is better than 0.1, 0.01, 0.001, and not-weight (same weight average)
       */
      double minus_ln01 = -log(0.2);

      double factor = minus_ln01/costFnalRange;


      assert(nTrainingImg == registrationFinalCostFunctionalValues.size());

      std::vector<double> weights(nTrainingImg);

      for (int i = 0; i < static_cast<int>(nTrainingImg); ++i)
        {
          weights[i] = exp(factor*(minCostFnalValue - registrationFinalCostFunctionalValues[i]));
        }

      // normalize weights s.t. sum to 1
      double s = 0.0;
      for (int i = 0; i < static_cast<int>(nTrainingImg); ++i)
        {
          s += weights[i];
        }

      for (int i = 0; i < static_cast<int>(nTrainingImg); ++i)
        {
          weights[i] /= s;
          std::cout<<weights[i]<<"  ";
        }
      std::cout<<std::endl;

    
      // 2.
      {
        ImageRegionIterator_t resultLabelImgIter(resultLabelImg, resultLabelImg->GetLargestPossibleRegion() );

        for (int iTrainingImg = 0; iTrainingImg < static_cast<int>(nTrainingImg); ++iTrainingImg)
          {
            resultLabelImgIter.GoToBegin();

            typename internal_image_t::Pointer regLabelImg = transformedLabelImages[iTrainingImg];
            ImageRegionConstIterator_t regLabelImgIter(regLabelImg, regLabelImg->GetLargestPossibleRegion() );
            regLabelImgIter.GoToBegin();

            for (; !regLabelImgIter.IsAtEnd(); ++regLabelImgIter, ++resultLabelImgIter)
              {
                resultLabelImgIter.Set(resultLabelImgIter.Get() + weights[iTrainingImg]*regLabelImgIter.Get() );
              }
          }
      } // 2.//

    } // Fusion



    // cast to float and output
    return laSeg::castItkImage<internal_pixel_t, float>( resultLabelImg ) ;
  }



  template<typename image_t>
  typename image_t::RegionType
  computeNonZeroRegion(typename image_t::Pointer img)
  {
    /**
     * Given the img, compute the region where outside this region,
     * the image is all zero.
     *
     * The minx, y, z are initialized as sizeX, y, z; then, whenever
     * encounter an non-zero voxel, the minx, y, z are updated
     * accordingly. Similar for maxX, y, z except that they are
     * intialized to 0, 0, 0
     */
    typedef typename image_t::RegionType imageRegion_t;
    typedef typename image_t::IndexType imageIndex_t;
    typedef typename image_t::SizeType imageSize_t;


    imageRegion_t entireRegion = img->GetLargestPossibleRegion();

    long minX = entireRegion.GetSize()[0];
    long minY = entireRegion.GetSize()[1];
    long minZ = entireRegion.GetSize()[2];

    long maxX = 0;
    long maxY = 0;
    long maxZ = 0;

    //    std::cout<<"hahaha = "<<minX<<'\t'<<minY<<'\t'<<minZ<<'\t'<<maxX<<'\t'<<maxX<<'\t'<<maxX<<'\n';

    typedef itk::ImageRegionConstIteratorWithIndex< image_t > itkImageRegionConstIteratorWithIndex_t;

    itkImageRegionConstIteratorWithIndex_t it(img, entireRegion);

    char foundNonZero = 0;

    {
      imageIndex_t idx;
      for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
          if (it.Get() != 0)
            {
              foundNonZero = 1;

              idx = it.GetIndex();

              minX = minX<idx[0]?minX:idx[0];
              minY = minY<idx[1]?minY:idx[1];
              minZ = minZ<idx[2]?minZ:idx[2];

              maxX = maxX>idx[0]?maxX:idx[0];
              maxY = maxY>idx[1]?maxY:idx[1];
              maxZ = maxZ>idx[2]?maxZ:idx[2];
            }
        }
    }

    imageRegion_t nonZeroRegion;

    if (1 == foundNonZero)
      {
        imageIndex_t startIdx;
        startIdx[0] = minX;
        startIdx[1] = minY;
        startIdx[2] = minZ;

        imageSize_t size;
        size[0] = maxX - minX;
        size[1] = maxY - minY;
        size[2] = maxZ - minZ;

        nonZeroRegion.SetSize( size );
        nonZeroRegion.SetIndex( startIdx );
      }
    else
      {
        imageIndex_t startIdx;
        startIdx[0] = 0;
        startIdx[1] = 0;
        startIdx[2] = 0;

        imageSize_t size;
        size[0] = entireRegion.GetSize()[0];
        size[1] = entireRegion.GetSize()[1];
        size[2] = entireRegion.GetSize()[2];

        nonZeroRegion.SetSize( size );
        nonZeroRegion.SetIndex( startIdx );
      }

    
    return nonZeroRegion;
  }


  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegion(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion)
  {
    typedef typename image_t::RegionType imageRegion_t;
    typedef typename image_t::IndexType imageIndex_t;
    typedef typename image_t::SizeType imageSize_t;

    imageRegion_t entireRegion = img->GetLargestPossibleRegion();
    imageSize_t entireSize = entireRegion.GetSize();

    imageIndex_t start = nonZeroRegion.GetIndex();
    imageSize_t sz = nonZeroRegion.GetSize();


    start[0] = std::max(0l, static_cast<long>(start[0] - sz[0]/5));
    start[1] = std::max(0l, static_cast<long>(start[1] - sz[1]/5));
    start[2] = std::max(0l, static_cast<long>(start[2] - sz[2]/5));

    sz[0] = std::min(entireSize[0] - start[0], 7*sz[0]/5);
    sz[1] = std::min(entireSize[1] - start[1], 7*sz[1]/5);
    sz[2] = std::min(entireSize[2] - start[2], 7*sz[2]/5);

    
    
    // {
    //   //tst
    //   std::cout<<"\t\t start =    "<<start<<std::endl<<std::flush;
    //   std::cout<<"\t\t entireSize =    "<<entireSize<<std::endl<<std::flush;
    //   std::cout<<"\t\t entireSize[1] - start[1], 7*sz[1]/5   "<<entireSize[1] - start[1]<<'\t'<<7*sz[1]/5<<'\t'<<sz[1]<<std::endl<<std::flush;
    //   //tst//
    // }

    imageRegion_t largerRegion;
    largerRegion.SetSize( sz );
    largerRegion.SetIndex( start );

    return largerRegion;
  }



  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegionByOnePixel(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion)
  {
    typedef typename image_t::RegionType imageRegion_t;
    typedef typename image_t::IndexType imageIndex_t;
    typedef typename image_t::SizeType imageSize_t;

    imageRegion_t entireRegion = img->GetLargestPossibleRegion();
    imageSize_t entireSize = entireRegion.GetSize();

    imageIndex_t start = nonZeroRegion.GetIndex();
    imageSize_t sz = nonZeroRegion.GetSize();


    start[0] = std::max(0l, static_cast<long>(start[0] - 1));
    start[1] = std::max(0l, static_cast<long>(start[1] - 1));
    start[2] = std::max(0l, static_cast<long>(start[2] - 1));

    sz[0] = std::min(entireSize[0] - start[0], sz[0] + 2);
    sz[1] = std::min(entireSize[1] - start[1], sz[1] + 2);
    sz[2] = std::min(entireSize[2] - start[2], sz[2] + 2);

    
    // {
    //   //tst
    //   std::cout<<"\t\t start =    "<<start<<std::endl<<std::flush;
    //   std::cout<<"\t\t entireSize =    "<<entireSize<<std::endl<<std::flush;
    //   std::cout<<"\t\t entireSize[1] - start[1], 7*sz[1]/5   "<<entireSize[1] - start[1]<<'\t'<<7*sz[1]/5<<'\t'<<sz[1]<<std::endl<<std::flush;
    //   //tst//
    // }

    imageRegion_t largerRegion;
    largerRegion.SetSize( sz );
    largerRegion.SetIndex( start );

    return largerRegion;
  }


  template<typename image_t>
  typename image_t::Pointer
  extractROI(typename image_t::Pointer img, typename image_t::RegionType region)
  {
    typedef itk::RegionOfInterestImageFilter<image_t, image_t> itkRegionOfInterestImageFilter_t;

    typename itkRegionOfInterestImageFilter_t::Pointer ROIfilter = itkRegionOfInterestImageFilter_t::New();
    ROIfilter->SetInput( img );
    ROIfilter->SetRegionOfInterest( region );
    ROIfilter->Update();

    return ROIfilter->GetOutput();
  }


  /** 
   * Generate an all-zero image the same size/origin/spacing/etc. as
   * referenceImg, inside of whick, the roiRegion is the roiImg
   */
  template<typename image_t>
  typename image_t::Pointer
  antiExtractROI(typename image_t::ConstPointer roiImg, const typename image_t::RegionType roiRegion, \
                 typename image_t::ConstPointer referenceImg)
  {

    // {
    //   //tst
    //   std::cout<<"\t in antiExtractROI\n"<<std::flush;
    //   std::cout<<"\t roiImg.GetLargestPossibleRegion() = "<<roiImg->GetLargestPossibleRegion()<<std::endl<<std::flush;
    //   std::cout<<"\t roiRegion = "<<roiRegion<<std::endl<<std::flush;
    //   std::cout<<"\t referenceImg.GetLargestPossibleRegion() = "<<referenceImg->GetLargestPossibleRegion()<<std::endl<<std::flush;
    //   //tst//
    // }


    typedef typename image_t::Pointer imagePointer_t;

    imagePointer_t largeImage = image_t::New();
    largeImage->SetRegions( referenceImg->GetLargestPossibleRegion() );
    largeImage->Allocate();

    largeImage->FillBuffer(0);
    largeImage->CopyInformation(referenceImg);


    typedef itk::ImageRegionIterator< image_t > itkImageRegionIterator_t;
    typedef itk::ImageRegionConstIterator< image_t > itkImageRegionConstIterator_t;

    {
      itkImageRegionConstIterator_t itROI(roiImg, roiImg->GetLargestPossibleRegion());
      itkImageRegionIterator_t itNew(largeImage, roiRegion);

      itROI.GoToBegin();
      itNew.GoToBegin();
      for (; !itROI.IsAtEnd(); ++itROI, ++itNew)
        {
          itNew.Set(itROI.Get());
        }
    }

    return largeImage;
  }





}// namespace
#endif
