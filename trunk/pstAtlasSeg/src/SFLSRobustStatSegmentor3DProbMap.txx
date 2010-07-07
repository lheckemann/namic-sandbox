#ifndef SFLSRobustStatSegmentor3DProbMap_txx_
#define SFLSRobustStatSegmentor3DProbMap_txx_

#include "SFLSRobustStatSegmentor3DProbMap.h"

#include <algorithm>
#include <ctime>

#include <limits>

// //debug//
#include "cArrayOp.h"
//#include <fstream>
// //DEBUG//


#include <omp.h>


#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"

#include "itkMinimumMaximumImageCalculator.h"
#include "itkBinaryThresholdImageFilter.h"

/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::setProbabilityMap(typename TFloatImage::Pointer pb)
{
  m_probabilityMap = pb;

  TSize size = m_probabilityMap->GetLargestPossibleRegion().GetSize();


  TIndex start = m_probabilityMap->GetLargestPossibleRegion().GetIndex();
  TIndex origin = {{0, 0, 0}};
  if (start != origin)
    {
      std::cout<<"Warrning: Force mask start to be (0, 0, 0)\n";

      TRegion region = m_probabilityMap->GetLargestPossibleRegion();
      region.SetIndex(origin);

      m_probabilityMap->SetRegions(region);
    }


  if (this->m_nx + this->m_ny + this->m_nz == 0)
    {
      this->m_nx = size[0];
      this->m_ny = size[1];
      this->m_nz = size[2];
    }
  else if ( this->m_nx != (long)size[0] || this->m_ny != (long)size[1] || this->m_nz != (long)size[2] )
    {
      std::cerr<<"Error: image sizes do not match with label image size.\n";
      raise(SIGABRT);
    }


  // check it
  checkProbabilityImage();

  return;
}


/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::checkProbabilityImage()
{
  /* This is a protected function, i.e. it's called inside the
     class. So to save typing, I may assume I call this function
     before all its pre-requisites are satisfied.
  */

  if (!m_probabilityMap)
    {
      std::cerr<<"Error: m_probabilityMap is not ready yet.\n";
      raise(SIGABRT);
    }


  /**
     Check if probability map fullfil some value requirements
  */
  typedef itk::MinimumMaximumImageCalculator<TFloatImage> MinimumMaximumImageCalculator_t;

  typename MinimumMaximumImageCalculator_t::Pointer minmaxCalculator = MinimumMaximumImageCalculator_t::New();
  
  minmaxCalculator->SetImage(m_probabilityMap);
  minmaxCalculator->Compute();

  typename TFloatImage::PixelType maxPb = minmaxCalculator->GetMaximum();
  typename TFloatImage::PixelType minPb = minmaxCalculator->GetMinimum();

  if (minPb < 0)
    {
      std::cerr<<"Error: min prob < 0, there must be something wrong...\n";
      raise(SIGABRT);
    }

  if (maxPb > 1.0)
    {
      std::cout<<"Error: max prob > 1.0, there must be something wrong...\n";
      raise(SIGABRT);
    }


  if (minPb > 0.1)
    {
      std::cout<<"Warning: min prob > 0.1, there might be something wrong...\n";
    }

  if (maxPb < 0.8)
    {
      std::cout<<"Warning: max prob < 0.8, there might be something wrong...\n";
    }


  return;
}



/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::computeLabelMap()
{
  /**
     Use the probabiliy image to compute the label map, which is used
     as the label map in RSS to compute the pdf of the seed.

     Input: probability image, m_probabilityMap, must be ready
  */
  typedef itk::BinaryThresholdImageFilter<TFloatImage, TLabelImage> binaryThresholdImageFilter_t;

  typename binaryThresholdImageFilter_t::Pointer thlder = binaryThresholdImageFilter_t::New();
  thlder->SetInput(m_probabilityMap);
  thlder->SetInsideValue(0);
  thlder->SetOutsideValue(1);
  thlder->SetUpperThreshold(0.8);
  thlder->SetLowerThreshold(0);
  thlder->Update();


  this->setInputLabelImage(thlder->GetOutput());


  //tst
  douher::writeImage3<typename TLabelImage::PixelType>(thlder->GetOutput(), "highProbLabel.nrrd");
  //tst//
  

  return;
}


/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::computeForce()
{
  double fmax = std::numeric_limits<double>::min();
  double kappaMax = std::numeric_limits<double>::min();

  long n = this->m_lz.size();
  double* kappaOnZeroLS = new double[ n ];
  double* cvForce = new double[ n ];


  std::vector<typename CSFLSLayer::iterator> m_lzIterVct( n );
  {
    long iiizzz = 0;
    for (typename CSFLSLayer::iterator itz = this->m_lz.begin(); itz != this->m_lz.end(); ++itz)
      m_lzIterVct[iiizzz++] = itz;    
  }


#pragma omp parallel
  {
    double fmaxOfThisThread = std::numeric_limits<double>::min();
    double kappaMaxOfThisThread = std::numeric_limits<double>::min(); 

#pragma omp for
    for (long i = 0; i < n; ++i)
      {
        typename CSFLSLayer::iterator itz = m_lzIterVct[i];

        long ix = (*itz)[0];
        long iy = (*itz)[1];
        long iz = (*itz)[2];
          
        TIndex idx = {{ix, iy, iz}};

        kappaOnZeroLS[i] = this->computeKappa(ix, iy, iz);

        std::vector<double> f(this->m_numberOfFeature);

        computeFeatureAt(idx, f);

        // compute the likelihood from the estimated pdf
        double a = -(this->kernelEvaluationUsingPDF(f));

        // use the probabiity map as the prior, compute the posterior
        double prior = m_probabilityMap->GetPixel(idx);

        a *= prior;        


        fmaxOfThisThread = fmaxOfThisThread>fabs(a)?fmaxOfThisThread:fabs(a);
        kappaMaxOfThisThread = kappaMaxOfThisThread>fabs(kappaOnZeroLS[i])\
          ?kappaMaxOfThisThread:fabs(kappaOnZeroLS[i]);

        cvForce[i] = a;
      }

#pragma omp critical
    {
      fmax = fmax>fmaxOfThisThread?fmax:fmaxOfThisThread;
      kappaMax = kappaMax>kappaMaxOfThisThread?kappaMax:kappaMaxOfThisThread;
    }
  }



//     for (int ithread = 0; ithread < nt; ++ithread)
//       {
//         fmax = fmax>fmaxOfThisThread[ithread]?fmax:fmaxOfThisThread[ithread];
//         kappaMax = kappaMax>kappaMaxOfThisThread[ithread]?kappaMax:kappaMaxOfThisThread[ithread];
//       }


    this->m_force.resize(n);
    for (long i = 0; i < n; ++i)
      {
        //this->m_force.push_back(cvForce[i]/(fmax + 1e-10) +  (this->m_curvatureWeight)*kappaOnZeroLS[i]);
        this->m_force[i] = (1 - (this->m_curvatureWeight))*cvForce[i]/(fmax + 1e-10) \
          +  (this->m_curvatureWeight)*kappaOnZeroLS[i]/(kappaMax + 1e-10);
      }

    
    delete[] kappaOnZeroLS;
    delete[] cvForce;
}


/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::getThingsReady()
{
  /*
   1. Generate mp_mask from seeds
   2. Compute feature at each point
   3. Extract feature at/around the seeds
  */

  computeLabelMap();

  computeMask();


  SuperClassType::getThingsReady();

  return;
}


/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::computeMask()
{
  /**
     Use the probabiliy image to compute the mask, which is used
     as the mask in RSS.

     Input: probability image, m_probabilityMap, must be ready
  */
  typedef typename SuperClassType::MaskImageType TMaskImage;

  typedef itk::BinaryThresholdImageFilter<TFloatImage, TMaskImage> binaryThresholdImageFilter_t;

  typename binaryThresholdImageFilter_t::Pointer thlder = binaryThresholdImageFilter_t::New();
  thlder->SetInput(m_probabilityMap);
  thlder->SetInsideValue(0);
  thlder->SetOutsideValue(1);
  thlder->SetUpperThreshold(0.6);
  thlder->SetLowerThreshold(0);
  thlder->Update();


  this->setMask(thlder->GetOutput());


  //tst
  douher::writeImage3<typename TMaskImage::PixelType>(thlder->GetOutput(), "mask.nrrd");
  //tst//
  

  return;
}




/* ============================================================  */
template< typename TPixel >
void
CSFLSRobustStatSegmentor3DProbMap< TPixel >
::doSegmenation()
{
  double startingTime = clock();
      

  getThingsReady();

//   std::ofstream f("/tmp/d.txt", std::ios_base::app);
//   f<<"m_maxRunningTime = "<<this->m_maxRunningTime<<std::endl;
//   f.close();


  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */
  this->initializeSFLS();

  //douher::saveAsImage2< double >(mp_phi, "initPhi.nrrd");
  for (unsigned int it = 0; it < this->m_numIter; ++it)
    //for (unsigned int it = 0; ; ++it)
    {
      //dbg//
      std::cout<<"In iteration "<<it<<std::endl<<std::flush;
      //DBG//



      // keep current zero contour as history is required
      if (this->m_keepZeroLayerHistory)
        {
          (this->m_zeroLayerHistory).push_back(this->m_lz);
        }



      //double oldVoxelCount = this->m_insideVoxelCount;

      computeForce();

      this->normalizeForce();

      this->oneStepLevelSetEvolution();



      /*----------------------------------------------------------------------
        If the level set stops growing, stop */
      this->updateInsideVoxelCount();
//       if (it > 2 && oldVoxelCount >= this->m_insideVoxelCount)
//         {
//           std::ofstream f("/tmp/o.txt");
//           f<<"stop grow\n";
//           f.close();

//           break;
//         }

      /* If the level set stops growing, stop
         ----------------------------------------------------------------------*/


      /*----------------------------------------------------------------------
        If the inside physical volume exceed expected volume, stop */
      //double volumeIn = (this->m_insideVoxelCount)*(this->m_dx)*(this->m_dy)*(this->m_dz);
//       if (volumeIn > (this->m_maxVolume))
//         {
//           //          std::fstream f("/tmp/o.txt", std::ios_base::app);
//           std::ofstream f("/tmp/o.txt");
//           f<<"m_maxVolume = "<<this->m_maxVolume<<std::endl;
//           f<<"volumeIn = "<<volumeIn<<std::endl;

//           f<<"reach max volume\n";
//           f.close();


//           break;
//         }
      /*If the inside physical volume exceed expected volume, stop 
        ----------------------------------------------------------------------*/

      
      double ellapsedTime = (clock() - startingTime)/static_cast<double>(CLOCKS_PER_SEC);
      if (ellapsedTime > (this->m_maxRunningTime))
        {
          std::ofstream f("/tmp/o.txt");
          f<<"running time = "<<ellapsedTime<<std::endl;
          f<<"m_maxRunningTime = "<<this->m_maxRunningTime<<std::endl;
          f.close();

          break;
        }


    }

  return;
}



#endif
