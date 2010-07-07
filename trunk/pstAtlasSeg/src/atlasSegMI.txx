#ifndef atlasSegMI_txx_
#define atlasSegMI_txx_


#include <csignal>

// local
#include "atlasSegMI.h"


// newProstate
#include "cArrayOp.h"
#include "reg_3d_affine_mi.hpp"
#include "reg_3d_bspline_mi.hpp"


// itk
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

#include "itkHistogramMatchingImageFilter.h"

#include "itkCastImageFilter.h"

namespace newProstate
{
  template<typename raw_image_t>
  typename itk::Image<float, 3>::Pointer
  atlasSegMI_outputPr(typename raw_image_t::Pointer rawImg,                       \
                      const std::vector< std::string >& trainingImageList, \
                      const std::vector< std::string >& labelImageList)
  {
    /**
     * Atlas based segmentation based on MI

     - input:
     -- txt file containing the names of the trainging images
     -- txt file containing the names of the corresponding label images
     -- raw image
     - output:
     -- label map of the raw image

     - process
     1. for each training images
     1.1 read label images
     1.2 check image sizes match with the training images
     1.3 reg training image to raw image
     1.3.1 affine
     1.3.2 deformable
     1.4 use transformation to transform label image
     1.5 save the transformed label image in float
     2 majority rules to decide the final label


     20100521
     Yi Gao
    */


    typedef float internal_pixel_t;
    typedef itk::Image<internal_pixel_t, 3> internal_image_t;


    unsigned long nTrainingImg = trainingImageList.size();
    if (nTrainingImg != labelImageList.size() )
      {
        std::cerr<<"Error: number of training images does not match number of label images.\n";
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



    // 1.
    //#pragma omp parallel for num_threads(nTrainingImg) 
    // use itk-optimized-reg compilation, do not do omp here.
    for (long iTrainingImg = 0; iTrainingImg < (long)nTrainingImg; ++iTrainingImg)
      {
        //tst
        //#pragma omp critical
        {
          std::cout<<"   Now reg "<<trainingImageList[iTrainingImg]<<" to raw image.\n"<<std::flush;
        }
        //tst//


        typename internal_image_t::Pointer trainingImg =                       \
          readImage3<internal_pixel_t>(trainingImageList[iTrainingImg].c_str());

        // smooth the training image
        typedef itk::CurvatureAnisotropicDiffusionImageFilter<internal_image_t, internal_image_t >  SmoothingFilterType;
        typename SmoothingFilterType::Pointer smoother = SmoothingFilterType::New();
        smoother->SetInput( trainingImg );
        const unsigned int numberOfIterations = 5;
        const double       timeStep = 0.02;
        const double       conductance = 1;
      
        smoother->SetNumberOfIterations( numberOfIterations );
        smoother->SetTimeStep( timeStep );
        smoother->SetConductanceParameter( conductance );
        smoother->UseImageSpacingOn();
        smoother->Update();

        trainingImg = smoother->GetOutput();


        // 1.1
        typename internal_image_t::Pointer labelImg =                        \
          readImage3<internal_pixel_t>(labelImageList[iTrainingImg].c_str());


        // 1.2
        if (trainingImg->GetLargestPossibleRegion().GetSize() != labelImg->GetLargestPossibleRegion().GetSize())
          {
            std::cerr<<"Error: size of trainging image does not match size of label image.\n" \
                     <<"Training image name: "<<trainingImageList[iTrainingImg]<<"\n" \
                     <<"Label image name: "<<labelImageList[iTrainingImg]<<"\n";
            raise(SIGABRT);
          }

        // 1.3
        // 1.3.1
        std::cout<<"       Affine registering...\n"<<std::flush;
        double finalCostFn = 0.0;
        std::pair<typename internal_image_t::Pointer, typename internal_image_t::Pointer> affineRegPair = \
          reg_3d_affine_mi<internal_image_t, internal_image_t, internal_image_t>(rawImg, trainingImg, labelImg, 0, finalCostFn);
        std::cout<<"           final cost = "<<finalCostFn<<std::endl;


        std::cout<<"       BSpline registering...\n"<<std::flush;
        std::pair<typename internal_image_t::Pointer, typename internal_image_t::Pointer> demonsRegPair = \
          reg_3d_bspline_mi<internal_image_t, internal_image_t, internal_image_t>(rawImg, \
                                                                                  affineRegPair.first, \
                                                                                  affineRegPair.second, \
                                                                                  0, \
                                                                                  10);

        //tst
        //write intermediate result
        char tmp1[1000];
        sprintf(tmp1, "regTrainImg%ld.nrrd", iTrainingImg);
        writeImage3<internal_pixel_t>(affineRegPair.first, tmp1);



        char tmp[1000];
        sprintf(tmp, "regLabelImg%ld.nrrd", iTrainingImg);
        writeImage3<internal_pixel_t>(affineRegPair.second, tmp);
        //tst//


        // sum
        //#pragma omp critical
        {
          ImageRegionIterator_t resultLabelImgIter(resultLabelImg, resultLabelImg->GetLargestPossibleRegion() );
          resultLabelImgIter.GoToBegin();

          typename internal_image_t::Pointer regLabelImg = demonsRegPair.second;
          ImageRegionConstIterator_t regLabelImgIter(regLabelImg, regLabelImg->GetLargestPossibleRegion() );
          regLabelImgIter.GoToBegin();

          for (; !regLabelImgIter.IsAtEnd(); ++regLabelImgIter, ++resultLabelImgIter)
            {
              resultLabelImgIter.Set(resultLabelImgIter.Get() + regLabelImgIter.Get() );
            }
        }
      }




    // average
    ImageRegionIterator_t resultLabelImgIter(resultLabelImg, resultLabelImg->GetLargestPossibleRegion() );
    for (resultLabelImgIter.GoToBegin(); !resultLabelImgIter.IsAtEnd(); ++resultLabelImgIter)
      {
        resultLabelImgIter.Set(resultLabelImgIter.Get()/static_cast<float>(nTrainingImg) );
      }


    // cast
    typedef itk::CastImageFilter< internal_image_t, itk::Image<float, 3> > CastFilterType;
    typename CastFilterType::Pointer  caster =  CastFilterType::New();
    caster->SetInput( resultLabelImg );
    caster->Update();
  
    return caster->GetOutput();
  }

} //namespace

#endif
