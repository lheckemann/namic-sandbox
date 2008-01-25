/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrowCutImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006/01/11 19:43:31 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#ifndef _itkGrowCutImageFilter_txx
#define _itkGrowCutImageFilter_txx
#include "itkGrowCutImageFilter.h"

#include "itkNeighborhoodAlgorithm.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"

#include "itkMinimumMaximumImageCalculator.h"


namespace itk
{

  template< class TInputImage, class TLabelMapImage >
  void
  GrowCutImageFilter< TInputImage, TLabelMapImage >
  ::PrintSelf(std::ostream& os, Indent indent) const  
  {
    Superclass::PrintSelf(os,indent);
  }


  template< class TInputImage, class TLabelMapImage >
  void
  GrowCutImageFilter< TInputImage, TLabelMapImage >
  ::GenerateData()
  {
    // get pointers to the input and output
    //        InputImagePointer inputPtr = const_cast< TInputImage * >( this->GetInput() );
    typename InputImageType::ConstPointer  input  = this->GetInput();
    //        if ( !inputPtr )
    if ( !input )
      {
        return;
      }

    
    if ( !m_InitLabelMap )
      //  typename TLabelMapImage::Pointer initLabelMapPtr = m_labelMap;
      {
        return;
      }
    


    ///////////////////////////////////////////////////////////
    // need to check the dimensionality of two images to be the same.
    ///////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////
    // make some typedef's
    typedef itk::Image< float, LabelMapDimension >  strengthMapType;

    typedef itk::ImageRegionIterator< InputImageType > inputImageIteratorType;
    typedef itk::ImageRegionIterator< LabelMapImageType > labelMapIteratorType;
    typedef itk::ImageRegionIterator< strengthMapType > strengthMapIteratorType;

    typedef itk::MinimumMaximumImageCalculator< InputImageType > maxMinCalculatorType;

    //     typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType > inputImageFaceCalculatorType;
    //     typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< LabelMapImageType > labelMapFaceCalculatorType;

    typedef itk::ConstNeighborhoodIterator< InputImageType > InputImageNeighborhoodIteratorType;
    typedef itk::NeighborhoodIterator< LabelMapImageType, ConstantBoundaryCondition< LabelMapImageType > > LabelMapNeighborhoodIteratorType;


    // done. typedef's
    ////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////
    // initialization
    // allocate the output image
    LabelMapImagePointer output = this->GetOutput();
    output->SetRegions(m_InitLabelMap->GetLargestPossibleRegion());
    output->Allocate();
    output->CopyInformation( m_InitLabelMap );



    ////////////////////////////////////////////////////////////////////////////////
    // build the strength map
    typename strengthMapType::Pointer strengthMap = strengthMapType::New();
    strengthMap->SetRegions(m_InitLabelMap->GetLargestPossibleRegion());
    strengthMap->Allocate();
    strengthMap->CopyInformation( m_InitLabelMap );
    
    
    //    printf("label map dimension is: %d\n", LabelMapDimension);

    // copy the initial label map to be the output label map.
    // and build the strengh map according to the initial label map
    labelMapIteratorType initLabelMapIt( m_InitLabelMap, m_InitLabelMap->GetLargestPossibleRegion() );
    labelMapIteratorType outputLabelMapIt( output, m_InitLabelMap->GetLargestPossibleRegion() );
    strengthMapIteratorType strengthMapIt( strengthMap, m_InitLabelMap->GetLargestPossibleRegion() );



    for ( initLabelMapIt.GoToBegin(), outputLabelMapIt.GoToBegin(), strengthMapIt.GoToBegin(); 
          !initLabelMapIt.IsAtEnd();
          ++initLabelMapIt, ++outputLabelMapIt, ++strengthMapIt )
      {
        outputLabelMapIt.Set( initLabelMapIt.Get() );
        strengthMapIt.Set( static_cast< float >( initLabelMapIt.Get() != 0 ) );
      }
    // done. initialization
    //////////////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////
    // now we are ready to do grow cut


    /////////////////////////////////////////////////////////////////////
    // first we get the largest value of the image, in maxPixel
    typename maxMinCalculatorType::Pointer maxMinCal = maxMinCalculatorType::New();
    maxMinCal->SetImage( input );
    maxMinCal->ComputeMaximum();
    InputPixelType maxPixel = maxMinCal->GetMaximum();

    //    printf("max image pixel value is: %d\n", maxPixel);
    // done. get max value of the image
    /////////////////////////////////////////////////////////////////////



    /////////////////////////////////////////////////////////////////////
    // define the neighborhood size. Here we use the 3*3*...*3 pixel neighborhood
    // i.e. the neighboor radii is 1 at all directions.
    typename InputImageType::SizeType neighborRadii;
    neighborRadii.Fill( 1 );
    // done. define the neighborhood
    /////////////////////////////////////////////////////////////////////


    //     ////////////////////////////////////////////////////////////
    //     // Find the data-set boundary "faces"
    //     // faces of the input image

    //     inputImageFaceCalculatorType inputImageFaceCalculator;
    //     typename inputImageFaceCalculatorType::FaceListType inputImageFaceList;
    //     inputImageFaceList = inputImageFaceCalculator( input, input->GetLargestPossibleRegion(),neighborRadii );
    //     typename inputImageFaceCalculatorType::FaceListType::iterator inputImageFaceIt;

    //     // faces of the labelmap image(output)

    //     labelMapFaceCalculatorType labelMapFaceCalculator;
    //     typename labelMapFaceCalculatorType::FaceListType labelMapFaceList;
    //     labelMapFaceList = labelMapFaceCalculator( output, output->GetLargestPossibleRegion(),neighborRadii );
    //     typename labelMapFaceCalculatorType::FaceListType::iterator labelMapFaceIt;

    //     // faces of the temp labelmap image, internal use, will delete at the end.
    //     typename labelMapFaceCalculatorType::FaceListType tempLabelMapFaceList;
    //     tempLabelMapFaceList = labelMapFaceCalculator( tempLabelMap, tempLabelMap->GetLargestPossibleRegion(),neighborRadii );
    //     typename labelMapFaceCalculatorType::FaceListType::iterator tempLabelMapFaceIt;

    //     LabelMapPixelType a = 2;
    //     double c = 0;

    //    int numOfFaces = inputImageFaceList.size();


    ////////////////////////////////////////////////////////////
    // point inputImageFaceIt to the first face, which is the inner
    // region of the data. Where boundary checking is NOT necessary.
    //     inputImageFaceIt = inputImageFaceList.begin();
    //     labelMapFaceIt = labelMapFaceList.begin();
    //     tempLabelMapFaceIt = tempLabelMapFaceList.begin();


    InputImageNeighborhoodIteratorType inputImageNbhdIt = InputImageNeighborhoodIteratorType( neighborRadii, input, m_InitLabelMap->GetLargestPossibleRegion() );
    LabelMapNeighborhoodIteratorType labelMapNbhdIt = LabelMapNeighborhoodIteratorType( neighborRadii, output, m_InitLabelMap->GetLargestPossibleRegion() );

    //    printf("size of current iterator is: %d\n", inputImageNbhdIt.Size() ); // 3^N where N is the dimension

    //    outputLabelMapIt = labelMapIteratorType( output, *inputImageFaceIt );
    typename LabelMapImageType::IndexType idx;
    typename LabelMapImageType::IndexType centerIdx;

    // this will iterate through all the inner pixels
    bool changed = false;
    int nbhdSize = labelMapNbhdIt.Size();


    for (int in = 0; in < 300; ++in)
      {
        changed = false;

        //        printf("in the %d-th iteration.\n",in);

        for (labelMapNbhdIt.GoToBegin();
             !labelMapNbhdIt.IsAtEnd(); 
             ++labelMapNbhdIt )
          {
            centerIdx = labelMapNbhdIt.GetIndex(labelMapNbhdIt.Size() / 2);

            if ( strengthMap->GetPixel( centerIdx ) != 1 ) {

              for (int inbhd = 0; inbhd < nbhdSize; inbhd++)
                {
                  idx = labelMapNbhdIt.GetIndex( inbhd );

                  if ( (labelMapNbhdIt.GetPixel( inbhd ) != 0) && (idx != centerIdx) )
                    {
                      double gFn = 1 - fabs(input->GetPixel(idx) - input->GetPixel(centerIdx))/maxPixel;
                      double attackStr = gFn*(strengthMap->GetPixel(idx));
                      if (attackStr > (strengthMap->GetPixel(centerIdx)) )
                        {
                          output->SetPixel(centerIdx, output->GetPixel(idx));
                          strengthMap->SetPixel(centerIdx, attackStr);
                          changed = true;
                        }
                    }
                }
            }
          }

        if (changed == false)
          {
            //            printf("exiting, n= %d \n",itn);
            return;
          }



//         for ( outputLabelMapIt.GoToBegin(), strengthMapIt1.GoToBegin(), 
//                 strengthMapIt2.GoToBegin(), tempLabelMapIt.GoToBegin(); 
//               !outputLabelMapIt.IsAtEnd();
//               ++outputLabelMapIt, ++strengthMapIt1, ++strengthMapIt2, ++tempLabelMapIt )
//           {
//             outputLabelMapIt.Set( tempLabelMapIt.Get() );
//             strengthMapIt1.Set( strengthMapIt2.Get() );
//           }


      }


  }
  // template< class TInputImage, class TLabelMapImage >
  // void GrowCutImageFilter< TInputImage, TLabelMapImage >
  // ::duplicateFromInitialLabelMap(TLabelMapImage outputLabelMap)
  // {
  //   LabelMapImagePointer initPtr = const_cast< TInputImage * >( this->GetInput() );  
  // }



} // end namespace itk

#endif
