#ifndef SFLSMultiRobustStatSegmentor3D_txx_
#define SFLSMultiRobustStatSegmentor3D_txx_

#include "SFLSMultiRobustStatSegmentor3D.h"

#include <algorithm>
#include <ctime>

//debug//
#include <fstream>
//DEBUG//

#include <limits>


#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"

/* ============================================================   */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::basicInit()
{
  SuperClassType::basicInit();

  m_statNeighborX = 1;
  m_statNeighborY = 1;
  m_statNeighborZ = 1;


  m_kernelWidthFactor = 10.0;

  m_inputImageIntensityMin = 0;
  m_inputImageIntensityMax = 0;


  return;
}  


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::computeForce(short labelId)
{
  CSFLSLayer& m_lz = this->m_sflsList[labelId].m_lz;
  std::vector< double >& forceForThisObj = this->m_forceList[labelId];

  
  double fmax = std::numeric_limits<double>::min();
  double kappaMax = std::numeric_limits<double>::min();

  long n = m_lz.size();
  double* kappaOnZeroLS = new double[ n ];
  double* cvForce = new double[ n ];


  std::vector<typename CSFLSLayer::iterator> m_lzIterVct( n );
  {
    long iiizzz = 0;
    for (typename CSFLSLayer::iterator itz = m_lz.begin(); itz != m_lz.end(); ++itz)
      m_lzIterVct[iiizzz++] = itz;    
  }


  {
    for (long i = 0; i < n; ++i)
      {
        typename CSFLSLayer::iterator itz = m_lzIterVct[i];

        long ix = (*itz)[0];
        long iy = (*itz)[1];
        long iz = (*itz)[2];
          
        TIndex idx = {{ix, iy, iz}};

        kappaOnZeroLS[i] = this->computeKappa(labelId, ix, iy, iz);

        std::vector<double> f(m_numberOfFeature);

        computeFeatureAt(idx, f);



        //       double a = -kernelEvaluation(labelId, f); // a is the force at
        //                                                 // this point. only
        //                                                 // has data-driven
        //                                                 // force, no curvature
        //                                                 // etc.
        double a = -kernelEvaluationUsingPDF(labelId, f);


        /* 
           Then go thru all the other labelId to see if this contour
           touches the others.

           If this point is touching some other contour, adjust the
           force here. The detail is:

           1. "touching": if the some other phi has value < 1 at this point.

           2. "adjust": so this point is shared by both (at least, may
           be more, but for now assume two) contours. The
           expending-force (negative value) of this contour will be the
           contracting-force (positive value) of that contour. So, the
           force of the other contour will affect the current force.

           If I have force -1 (this point wants to expend), at the same
           point, some other contour has force -2 (it wants to expend
           its contour too, but the force is larger). Then the force
           here is -1 - (-2) = 1. So this point will shrink. 

           But if the other force is, 2, contracting itself. Then my
           force -1 won't be changed. So, only pushing, no pulling.

           So myForce -= min(theirForce, 0)

           No change will be made on the other force, coz when the
           labelId is that contour, we'll handle it.
        */
        for (short ilabel = 0; ilabel < this->m_numOfObjects; ++ilabel)
          {
            if (labelId == ilabel)
              {
                continue;
              }

            float lsValueOnAnotherLevelSet = ((this->m_phiList)[ilabel])->GetPixel(idx);
            if (lsValueOnAnotherLevelSet >= 1)
              {
                continue;
              }
          
            //double thisForce = -kernelEvaluation(ilabel, f);
            double thisForce = -kernelEvaluationUsingPDF(ilabel, f);

            thisForce = thisForce<0.0?thisForce:0.0;

            a -= thisForce;
          }


        fmax = fmax>fabs(a)?fmax:fabs(a);
        kappaMax = kappaMax>fabs(kappaOnZeroLS[i])?kappaMax:fabs(kappaOnZeroLS[i]);


        cvForce[i] = a;
      }
  }



  forceForThisObj.resize(n);
  for (long i = 0; i < n; ++i)
    {
      forceForThisObj[i] = (1 - (this->m_curvatureWeightList[labelId]))*cvForce[i]/(fmax + 1e-10) \
        +  (this->m_curvatureWeightList[labelId])*kappaOnZeroLS[i]/(kappaMax + 1e-10);
    }

    
  delete[] kappaOnZeroLS;
  delete[] cvForce;

  
  return;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::computeForce()
{
  for (short i = 0; i < this->m_numOfObjects; ++i)
    {
      computeForce(i);
    }
  
  return;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::getThingsReady()
{
  /*
    0. preprocess label map
    1. Generate mp_mask from seeds
    2. Compute feature at each point
    3. Extract feature at/around the seeds
  */

  this->preprocessLableMap();

  labelMapToSeeds();

  //dialteSeeds();

  initFeatureComputedImage();
  initFeatureImage();


  //computeFeature();
  getFeatureOnSeeds();
  estimateFeatureStdDevs();


  estimatePDFs();

  return;
}


/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::initFeatureImage()
{
  for (long ifeature = 0; ifeature < m_numberOfFeature; ++ifeature)
    {
      TDoubleImagePointer fimg = TDoubleImage::New();
      fimg->SetRegions(this->mp_img->GetLargestPossibleRegion() );
      fimg->Allocate();
      fimg->CopyInformation(this->mp_img);

      m_featureImageList.push_back(fimg);
    }

  return;
}


/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::initFeatureComputedImage()
{
  m_featureComputed = TLabelImage::New();
  m_featureComputed->SetRegions(this->mp_img->GetLargestPossibleRegion());
  m_featureComputed->Allocate();
  m_featureComputed->CopyInformation(this->mp_img);
  m_featureComputed->FillBuffer(0);

  return;
}


/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::computeFeatureAt(TIndex idx, std::vector<double>& f)
{
  f.resize(m_numberOfFeature);

  if (m_featureComputed->GetPixel(idx))
    {
      // the feature at this pixel is computed, just retrive

      for (long i = 0; i < m_numberOfFeature; ++i)
        {
          f[i] = (m_featureImageList[i])->GetPixel(idx);
        }
    }
  else
    {
      // compute the feature
      std::vector< double > neighborIntensities;

      long ix = idx[0];
      long iy = idx[1];
      long iz = idx[2];

      for (long iiz = iz - m_statNeighborZ; iiz <= iz + m_statNeighborZ; ++iiz)
        {
          for (long iiy = iy - m_statNeighborY; iiy <= iy + m_statNeighborY; ++iiy)
            {
              for (long iix = ix - m_statNeighborX; iix <= ix + m_statNeighborX; ++iix)
                {
                  if (0 <= iix && iix < this->m_nx    \
                      && 0 <= iiy && iiy < this->m_ny    \
                      && 0 <= iiz && iiz < this->m_nz)
                    {
                      TIndex idxa = {{iix, iiy, iiz}};
                      neighborIntensities.push_back(this->mp_img->GetPixel(idxa));
                    }
                }
            }
        }

      getRobustStatistics(neighborIntensities, f);

      for (long ifeature = 0; ifeature < m_numberOfFeature; ++ifeature)
        {
          m_featureImageList[ifeature]->SetPixel(idx, f[ifeature]);
        }
      
      m_featureComputed->SetPixel(idx, 1); // mark as computed
    }
  
  return;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::doSegmenation()
{
  //double startingTime = clock();
      

  getThingsReady();

//   std::ofstream f("/tmp/d.txt", std::ios_base::app);
//   f<<"m_maxRunningTime = "<<this->m_maxRunningTime<<std::endl;
//   f.close();
  


  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */
  this->initializeSFLS();

  for (unsigned int it = 0; it < this->m_numIter; ++it)
    //for (unsigned int it = 0; ; ++it)
    {
      //dbg//
      std::cout<<"In iteration "<<it<<std::endl<<std::flush;
      //DBG//


      //double oldVoxelCount = this->m_insideVoxelCount;

      computeForce();

      this->normalizeForce();

      this->oneStepLevelSetEvolution();



      /*----------------------------------------------------------------------
        If the level set stops growing, stop */
//       this->updateInsideVoxelCount();
//       if (it > 2 && oldVoxelCount >= this->m_insideVoxelCount)
//         {
//           std::ofstream f("/tmp/o.txt");
//           f<<"stop grow\n";
//           f.close();

//           break;
//         }

      /* If the level set stops growing, stop
         ----------------------------------------------------------------------*/


//       /*----------------------------------------------------------------------
//         If the inside physical volume exceed expected volume, stop */
//       double volumeIn = (this->m_insideVoxelCount)*(this->m_dx)*(this->m_dy)*(this->m_dz);
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
//       /*If the inside physical volume exceed expected volume, stop 
//         ----------------------------------------------------------------------*/

      
//       double ellapsedTime = (clock() - startingTime)/static_cast<double>(CLOCKS_PER_SEC);
//       if (ellapsedTime > (this->m_maxRunningTime))
//         {
//           std::ofstream f("/tmp/o.txt");
//           f<<"running time = "<<ellapsedTime<<std::endl;
//           f<<"m_maxRunningTime = "<<this->m_maxRunningTime<<std::endl;
//           f.close();

//           break;
//         }


    }


  this->m_done = true;

  return;
}



/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::getRobustStatistics(std::vector<double>& samples, std::vector<double>& robustStat)
{
  /* note, sample is sorted, so the order is changed */
  robustStat.resize(m_numberOfFeature);

  std::sort(samples.begin(), samples.end() );

  double n = samples.size();

  double q1 = n/4.0;
  double q1_floor;
  double l1 = modf(q1, &q1_floor);

  double q2 = n/2.0;
  double q2_floor;
  double l2 = modf(q2, &q2_floor);

  double q3 = 3.0*n/4.0;
  double q3_floor;
  double l3 = modf(q3, &q3_floor);

  double median = (1 - l2)*samples[static_cast<long>(q2_floor)] + l2*samples[static_cast<long>(q2_floor) + 1];

  double iqr = ( (1 - l3)*samples[static_cast<long>(q3_floor)] + l3*samples[static_cast<long>(q3_floor) + 1] ) \
    - ( (1 - l1)*samples[static_cast<long>(q1_floor)] + l1*samples[static_cast<long>(q1_floor) + 1] );

  robustStat[0] = median;
  robustStat[1] = iqr;

  /* next compute MAD */
  long nn = samples.size();
  std::vector<double> samplesDeMedian(nn);
  for (long i = 0; i < nn; ++i)
    {
      samplesDeMedian[i] = fabs(samples[i] - median);
    }

  std::sort(samplesDeMedian.begin(), samplesDeMedian.end() );

  double mad = (1 - l2)*samplesDeMedian[static_cast<long>(q2_floor)] + l2*samplesDeMedian[static_cast<long>(q2_floor) + 1];  
  robustStat[2] = mad;


  return;
}


/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::labelMapToSeeds()
{
  m_multipleSeedLists.resize(this->m_numOfObjects);
  // m_numOfObjects different labels, including background, which is now just a label


  typedef itk::ImageRegionConstIteratorWithIndex<labelMap_t> ImageRegionConstIteratorWithIndex_t;
  ImageRegionConstIteratorWithIndex_t it(this->m_labelMap, this->m_labelMap->GetLargestPossibleRegion() );
  it.GoToBegin();


//   //debug//
//   char tmp[1000];
//   sprintf(tmp, "_seeds_%d");
//   std::ofstream sf("_seeds.txt");  
//   //DEBUG//


  {
    std::vector<int> thisSeed(3);
    for (; !it.IsAtEnd(); ++it)
      {
        typename labelMap_t::PixelType thisLabel = it.Get();

        if (thisLabel)
          {
            short thisLabelIdx = this->m_labelValueToIndexMap[thisLabel];

            TIndex idx = it.GetIndex();
            thisSeed[0] = idx[0];
            thisSeed[1] = idx[1];
            thisSeed[2] = idx[2];

            /*
            {//tst
              std::cout<<"thisLabel = "<<thisLabel<<"\t thisLabelIdx = "<<thisLabelIdx<<"\t" \
                       <<thisSeed[0]<<", "<<thisSeed[1]<<", "<<thisSeed[2]<<std::endl;
            }//tst//
            */

            this->m_multipleSeedLists[thisLabelIdx].push_back(thisSeed);
          }
      }
  }

  /*
  {//tst
    for (short idLabel = 0; idLabel < this->m_numOfObjects; ++idLabel)
      {
        long numOfSeedsForThisObj = this->m_multipleSeedLists[idLabel].size();
        for (long i = 0; i < numOfSeedsForThisObj; ++i)
          {
            std::cout<<"labelIdx = "<<idLabel<<", seeds = "              \
                     <<this->m_multipleSeedLists[idLabel][i][0]<<", "    \
                     <<this->m_multipleSeedLists[idLabel][i][1]<<", "    \
                     <<this->m_multipleSeedLists[idLabel][i][2]<<std::endl;
          }
      }
    exit(0);
  }//tst//
  */  


  return;
}




/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::getFeatureOnSeeds()
{
  if (!m_featureImageList[m_numberOfFeature-1])
    {
      // last feature image is not constructed
      std::cerr<<"Error: construct feature images first.\n";
      raise(SIGABRT);
    }

  m_FeatureAtMultipleSeeds.resize(this->m_numOfObjects);

  for (short iObj = 0; iObj < this->m_numOfObjects; ++iObj)
    {
      const std::vector<std::vector<int> >& seedsOfThisObj = m_multipleSeedLists[iObj];

      long n = seedsOfThisObj.size();


      if (n == 0)
        {
          std::cerr << "Error: No seeds specified." << std::endl;
          raise(SIGABRT);
        }


      for (long i = 0; i < n; ++i)
        {
          if (3 != seedsOfThisObj[i].size())
            {
              std::cerr<<"Error: 3 != seedsOfThisObj[i].size()\n";
              raise(SIGABRT);
            }

          long ix = seedsOfThisObj[i][0];
          long iy = seedsOfThisObj[i][1];
          long iz = seedsOfThisObj[i][2];

          TIndex idx = {{ix, iy, iz}};
                      
          std::vector<double> featureHere;
          computeFeatureAt(idx, featureHere);

          m_FeatureAtMultipleSeeds[iObj].push_back(featureHere);
        }

    } // iObj


  return;
}

/* ============================================================ */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::estimateFeatureStdDevs()
{
  m_kernelStddevOfEachObj.assign(this->m_numOfObjects, std::vector<double>(m_numberOfFeature, 0.0));


  for (short iObj = 0; iObj < this->m_numOfObjects; ++iObj)
    {
      const std::vector<std::vector<int> >& seedsOfThisObj = m_multipleSeedLists[iObj];

      long n = seedsOfThisObj.size();

      for (long i = 0; i < m_numberOfFeature; ++i)
        {
          double m = 0;
          for (long ii = 0; ii < n; ++ii)
            {
              m += m_FeatureAtMultipleSeeds[iObj][ii][i];
            }
          m /= n;

          for (long ii = 0; ii < n; ++ii)
            {
              m_kernelStddevOfEachObj[iObj][i] += (m_FeatureAtMultipleSeeds[iObj][ii][i] - m)*(m_FeatureAtMultipleSeeds[iObj][ii][i] - m);
            }

          m_kernelStddevOfEachObj[iObj][i] /= (n-1);
          m_kernelStddevOfEachObj[iObj][i] = sqrt(m_kernelStddevOfEachObj[iObj][i]);
        }
    } // iObj


//   //debug//
//   for (short iObj = 0; iObj < this->m_numOfObjects; ++iObj)
//     {
//       for (long i = 0; i < m_numberOfFeature; ++i)
//         {
//           std::cout<<"obj "<<iObj<<"\t, std = "<<m_kernelStddevOfEachObj[iObj][i]<<std::endl;
//         }
//     }

//   exit(0);
//   //debug//
  

  return;
}

template< typename TPixel >
double
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::kernelEvaluationUsingPDF(short labelId, const std::vector<double>& newFeature)
{
  double p = 1;

  for (long i = 0; i < m_numberOfFeature; ++i)
    {
      long idx = static_cast<long>(newFeature[i] - m_inputImageIntensityMin);

      double probOfThisFeature = m_PDFlearnedFromSeeds[labelId][i][idx];

      p *= probOfThisFeature;
    }

  return p;
}

/* ============================================================  */
template< typename TPixel >
double
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::kernelEvaluation(short labelId, const std::vector<double>& newFeature)
{
  const std::vector<std::vector<int> >& seedOfThisObject = m_multipleSeedLists[labelId];

  long n = seedOfThisObject.size(); // == m_featureAtTheSeeds.size()

  double p = 1;
  //double p = 0;

  for (long i = 0; i < m_numberOfFeature; ++i)
    {
      double pp = 0.0;

      double stdDev = m_kernelStddevOfEachObj[labelId][i]/m_kernelWidthFactor; // /10 as in Eric's appendix

      double var2 = -1.0/(2*stdDev*stdDev);
      double c = 1.0/sqrt(2*(vnl_math::pi))/stdDev;

      for (long ii = 0; ii < n; ++ii)
        {
          pp += exp(var2*(newFeature[i] - m_FeatureAtMultipleSeeds[labelId][ii][i]) \
                    *(newFeature[i] - m_FeatureAtMultipleSeeds[labelId][ii][i]));
        }
      
      pp *= c;
      pp /= n;

      p *= pp;
      //p = p>pp?p:pp;
    }

  return p;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::setKernelWidthFactor(double f)
{
  if (f < 0.3)
    {
      m_kernelWidthFactor = 0.3;
    }

  if (f > 10.0)
    {
      m_kernelWidthFactor = 10.0;
    }

  m_kernelWidthFactor = f;


  std::ofstream fil("/tmp/d.txt", std::ios_base::app);
  fil<<"m_kernelWidthFactor = "<<m_kernelWidthFactor<<std::endl;
  fil.close();


  return;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::setIntensityHomogeneity(double h)
{
  std::ofstream fil("/tmp/d.txt", std::ios_base::app);
  fil<<"intensity homogeneity = "<<h<<std::endl;
  fil.close();


  double f = h*(10.0 - 0.3) + 0.3;

  setKernelWidthFactor(f);

  return;
}

/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::estimatePDFs()
{
  m_PDFlearnedFromSeeds.clear();
  
  computeMinMax(); // so we have the range of all pdfs


  for (short labelId = 0; labelId < this->m_numOfObjects; ++labelId)
    {
      std::vector< std::vector<double> > PDFsOfAllFeaturesOfThisLable;

      for (long ifeature = 0; ifeature < m_numberOfFeature; ++ifeature)
        {
          std::vector<double> PDFOfThisFeaturesOfThisLable(m_inputImageIntensityMax - m_inputImageIntensityMin + 1); 
          // assumption: TPixel are of integer types.

          double stdDev = m_kernelStddevOfEachObj[labelId][ifeature]/m_kernelWidthFactor; // /10 as in Eric's appendix
          double var2 = -1.0/(2*stdDev*stdDev);
          double c = 1.0/sqrt(2*(vnl_math::pi))/stdDev;

          for (TPixel a = m_inputImageIntensityMin; a <= m_inputImageIntensityMax; ++a)
            {
              long ia = static_cast<long>(a - m_inputImageIntensityMin);


              const std::vector<std::vector<int> >& seedOfThisObject = m_multipleSeedLists[labelId];
              long n = seedOfThisObject.size(); // == m_featureAtTheSeeds.size()

              double pp = 0.0;
              for (long ii = 0; ii < n; ++ii)
                {
                  pp += exp(var2*(a - m_FeatureAtMultipleSeeds[labelId][ii][ifeature]) \
                            *(a - m_FeatureAtMultipleSeeds[labelId][ii][ifeature]));
                }
      
              pp *= c;
              pp /= n;

              PDFOfThisFeaturesOfThisLable[ia] = pp;
            }

          PDFsOfAllFeaturesOfThisLable.push_back(PDFOfThisFeaturesOfThisLable);
        }

      m_PDFlearnedFromSeeds.push_back(PDFsOfAllFeaturesOfThisLable);
    }

  return;
}


/* ============================================================  */
template< typename TPixel >
void
SFLSMultiRobustStatSegmentor3D_c< TPixel >
::computeMinMax()
{
  if (!(this->mp_img))
    {
      std::cerr<<"Error: set input image first.\n";
      raise(SIGABRT);
    }

  typedef itk::Image<TPixel, 3> itkImage_t;

  typedef itk::ImageRegionConstIterator<itkImage_t> itkImageRegionConstIterator_t;

  itkImageRegionConstIterator_t it((this->mp_img), (this->mp_img)->GetLargestPossibleRegion() );
  it.GoToBegin();

  m_inputImageIntensityMin = std::numeric_limits<unsigned>::max(); // yes, it's twisted so easity to compute.
  m_inputImageIntensityMax = std::numeric_limits<unsigned>::min();

  for (; !it.IsAtEnd(); ++it)
    {
      TPixel v = it.Get();
      
      m_inputImageIntensityMin = m_inputImageIntensityMin<v?m_inputImageIntensityMin:v;
      m_inputImageIntensityMax = m_inputImageIntensityMax>v?m_inputImageIntensityMax:v;
    }

  return;
}




#endif
