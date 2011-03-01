#ifndef SFLSMultiSegmentor3D_hpp_
#define SFLSMultiSegmentor3D_hpp_

#include "SFLSMultiSegmentor3D.h"

#include <algorithm>
#include <cmath>

#include <csignal>

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"


template< typename TPixel >
SFLSMultiSegmentor3D_c< TPixel >
::SFLSMultiSegmentor3D_c()
{
  basicInit(); 
}


/* ============================================================
   basicInit    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::basicInit()
{
  m_numIter = 100;
  m_timeStep = 1.0;

  m_nx = 0;
  m_ny = 0;
  m_nz = 0;

  m_dx = 1.0;
  m_dy = 1.0;
  m_dz = 1.0;


  m_numOfObjects = 0;

  m_curvatureWeight = 0.0;

//   m_insideVoxelCount = 0;
//   m_insideVolume = 0;

//   m_maxVolume = 1e10; // in mm^3
//   m_maxRunningTime = 3600; // in sec

  m_labelMapPreprocessed = false;

  m_done = false;
}

/* ============================================================
   setNumIter    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::setNumIter(unsigned long n)
{
  m_numIter = n;
}


/* ============================================================
   setImage    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::setImage(typename ImageType::Pointer img)
{
  mp_img = img;

  TIndex start = mp_img->GetLargestPossibleRegion().GetIndex();
  TIndex origin = {{0, 0, 0}};
  if (start != origin)
    {
      std::cout<<"Warrning: Force image start to be (0, 0, 0)\n";

      TRegion region = mp_img->GetLargestPossibleRegion();
      region.SetIndex(origin);

      mp_img->SetRegions(region);
    }

  TSize size = img->GetLargestPossibleRegion().GetSize();

  if (m_nx + m_ny + m_nz == 0)
    {
      m_nx = size[0];
      m_ny = size[1];
      m_nz = size[2];

      typename ImageType::SpacingType spc = img->GetSpacing();      
      m_dx = spc[0];
      m_dy = spc[1];
      m_dz = spc[2];
    }
  else if ( m_nx != (long)size[0] || m_ny != (long)size[1] || m_nz != (long)size[2] )
    {
      std::cerr<<"image sizes do not match. abort\n";
      raise(SIGABRT);
    }

  return;
}



/* ============================================================
   setLabelMap    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::setLabelMap(typename labelMap_t::Pointer labelMap)
{
  /* This fn is important. Only after this, we can decide how many
     objects are going to be segmented. So to decide the length of
     curvatureWeight, m_phiList, m_forceList etc.
  */

  m_labelMap = labelMap;

  TSize size = m_labelMap->GetLargestPossibleRegion().GetSize();


  TIndex start = m_labelMap->GetLargestPossibleRegion().GetIndex();
  TIndex origin = {{0, 0, 0}};
  if (start != origin)
    {
      std::cout<<"Warrning: Force mask start to be (0, 0, 0)\n";

      TRegion region = m_labelMap->GetLargestPossibleRegion();
      region.SetIndex(origin);

      m_labelMap->SetRegions(region);
    }


  if (m_nx + m_ny + m_nz == 0)
    {
      m_nx = size[0];
      m_ny = size[1];
      m_nz = size[2];
    }
  else if ( m_nx != (long)size[0] || m_ny != (long)size[1] || m_nz != (long)size[2] )
    {
      std::cerr<<"image sizes do not match. abort\n";
      raise(SIGABRT);
    }

  return;
}

/* ============================================================
   setLabelMap    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::preprocessLableMap()
{
  if (!m_labelMap)
    {
      std::cerr<<"set m_labelMap first.\n";
      raise(SIGABRT);
    }

  /* 
     1. get the unique labels

     2. the # of unique labels

     3. get map from label value to its idx in m_objectLabelList
  */

  // 1.
  typedef itk::ImageRegionConstIterator<labelMap_t> ImageRegionConstIterator_t;
  ImageRegionConstIterator_t iter(m_labelMap, m_labelMap->GetLargestPossibleRegion() );
  iter.GoToBegin();

  m_objectLabelList.resize(m_nx*m_ny*m_nz);
  long i = 0;
  for (; !iter.IsAtEnd(); ++iter)
    {
      m_objectLabelList[i++] = iter.Get();
    }
  
  std::sort(m_objectLabelList.begin(), m_objectLabelList.end());
  std::vector<short>::iterator itl = unique(m_objectLabelList.begin(), m_objectLabelList.end());
  m_objectLabelList.resize( itl - m_objectLabelList.begin() );    

  if (m_objectLabelList[0] != 0)
    {
      std::cerr<<"Error: least label is not 0? no background?\n";
      raise(SIGABRT);
    }

  m_objectLabelList.erase(m_objectLabelList.begin());
 

  // 2.
  m_numOfObjects = m_objectLabelList.size();

  //dbg//
  std::cout<<"Totally "<<m_numOfObjects<<" labels.\n";
  //dbg//


  // 3.
  for (short i = 1; i < m_numOfObjects; ++i) // without 0
    {
      m_labelValueToIndexMap[m_objectLabelList[i]] = i;
    }


  initVariablesAccordingToLabelMap();



  m_labelMapPreprocessed = true;

  return;
}

template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::initVariablesAccordingToLabelMap()
{
  if (!m_labelMap)
    {
      std::cerr<<"set m_labelMap first.\n";
      raise(SIGABRT);
    }

  if (!m_numOfObjects)
    {
      std::cerr<<"Error: m_numOfObjects = 0\n";
      raise(SIGABRT);
    }

  m_curvatureWeightList.resize(m_numOfObjects);
  m_curvatureWeightList.assign(m_numOfObjects, m_curvatureWeight); // let's set to 0.0 first


  m_sflsList.resize(m_numOfObjects);

  m_forceList.resize(m_numOfObjects);


  return;
}

template< typename TPixel >
bool
SFLSMultiSegmentor3D_c< TPixel >
::getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(short labelId, long ix, long iy, long iz, double& thePhi)
{
  /*--------------------------------------------------
   *
   * Look in all the neighbors, to find the phi value of the nbhd:
   * this nbhd should satisfy: 1. its layer is strictly closer to
   * the zero layer hence its value is thought to be updated. 2. If
   * there are several nbhd's belonging to the same layer, choose
   * the one whose phi value has the smallest abs value.  If (ix,
   * iy) is outside, go through all nbhd who is in the layer of
   * label = mylevel-1 pick the SMALLEST phi. If (ix, iy) is inside,
   * go through all nbhd who is in the layer of label = mylevel+1
   * pick the LARGEST phi.
   */
  
  typename LSImageType::Pointer mp_phi = m_phiList[labelId];
  typename LabelImageType::Pointer mp_label = m_labelList[labelId];


  TIndex idx = {{ix, iy, iz}};
  char mylevel = mp_label->GetPixel(idx);

  //  char mylevel = mp_label->get(ix, iy, iz);
  bool foundNbhd = false;

  if (mylevel > 0)
    {
      // find the SMALLEST phi
      thePhi = 10000;
      
      TIndex idx1 = {{ix+1, iy, iz}};
      if ( (ix+1 < m_nx) && (mp_label->GetPixel(idx1) == mylevel - 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx1);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if ( (ix-1 >= 0 ) && (mp_label->GetPixel(idx2) == mylevel - 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx2);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }
      
      TIndex idx3 = {{ix, iy+1, iz}};
      if ( (iy+1 < m_ny) && (mp_label->GetPixel(idx3) == mylevel - 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx3);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if ( ((iy-1) >= 0 ) && (mp_label->GetPixel(idx4) == mylevel - 1) ) 
        {
          double itsPhi = mp_phi->GetPixel(idx4);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if ( (iz+1 < m_nz) && (mp_label->GetPixel(idx5) == mylevel - 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx5);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx6 = {{ix, iy, iz-1}};
      if ( ((iz-1) >= 0 ) && (mp_label->GetPixel(idx6) == mylevel - 1) ) 
        {
          double itsPhi = mp_phi->GetPixel(idx6);
          thePhi = thePhi<itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }


    }
  else
    {
      // find the LARGEST phi
      thePhi = -10000;

      TIndex idx1 = {{ix+1, iy, iz}};
      if ( (ix+1 < m_nx) && (mp_label->GetPixel(idx1) == mylevel + 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx1);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if ( (ix-1 >= 0  ) && (mp_label->GetPixel(idx2) == mylevel + 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx2);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx3 = {{ix, iy+1, iz}};
      if ( (iy+1 < m_ny) && (mp_label->GetPixel(idx3) == mylevel + 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx3);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if ( ((iy-1) >= 0 ) && (mp_label->GetPixel(idx4) == mylevel + 1) ) 
        {
          double itsPhi = mp_phi->GetPixel(idx4);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if ( (iz+1 < m_nz) && (mp_label->GetPixel(idx5) == mylevel + 1) )
        {
          double itsPhi = mp_phi->GetPixel(idx5);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }

      TIndex idx6 = {{ix, iy, iz-1}};
      if ( ((iz-1) >= 0 ) && (mp_label->GetPixel(idx6) == mylevel + 1) ) 
        {
          double itsPhi = mp_phi->GetPixel(idx6);
          thePhi = thePhi>itsPhi?thePhi:itsPhi;

          foundNbhd = true;
        }
    }



  return foundNbhd;
}


/* ============================================================
   normalizeForce   
   Normalize m_force s.t. max(abs(m_force)) < 0.5 */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::normalizeForce(short labelId)
{
  std::vector< double >& forceOfThisObj = m_forceList[labelId];

  CSFLSLayer& m_lz = m_sflsList[labelId].m_lz;
  
  
  unsigned long nLz = m_lz.size();

  if (forceOfThisObj.size() != nLz )
    {
      std::cerr<<"forceOfThisObj.size() = "<<forceOfThisObj.size()<<std::endl;
      std::cerr<<"nLz = "<<nLz<<std::endl;

      std::cerr<<"forceOfThisObj.size() != nLz, abort.\n";
      raise(SIGABRT);
    }

  double fMax = fabs( forceOfThisObj.front() );

  {
    long nf = forceOfThisObj.size();

    for (long itf = 0; itf < nf; ++itf)
      {
        double v = fabs(forceOfThisObj[itf]);
        fMax = fMax>v?fMax:v;
      }
  }
  fMax /= 0.49;

  {
    long nf = forceOfThisObj.size();

    for (long itf = 0; itf < nf; ++itf)
      {
        forceOfThisObj[itf] /= (fMax + 1e-10);
      }
  }

  return;
}

/* ============================================================
   normalizeForce   
   Normalize m_force s.t. max(abs(m_force)) < 0.5 */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::normalizeForce()
{
  for (short i = 0; i < m_numOfObjects; ++i)
    {
      normalizeForce(i);
    }

  return;
}


// /* ============================================================
//    updateInsideVoxelCount    */
// template< typename TPixel >
// void
// SFLSMultiSegmentor3D_c< TPixel >
// ::updateInsideVoxelCount()
// {
//   m_insideVoxelCount -= m_lIn2out.size();
//   m_insideVoxelCount += m_lOut2in.size();

//   m_insideVolume = m_insideVoxelCount*m_dx*m_dy*m_dz;

//   return;
// }



/* ============================================================
   oneStepLevelSetEvolution    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::oneStepLevelSetEvolution(short labelId)
{
  // create 'changing status' lists
  CSFLSLayer Sz;
  CSFLSLayer Sn1;
  CSFLSLayer Sp1;
  CSFLSLayer Sn2;
  CSFLSLayer Sp2;


  typename LSImageType::Pointer mp_phi = m_phiList[labelId];
  typename LabelImageType::Pointer mp_label = m_labelList[labelId];

  std::vector< double >& m_force = m_forceList[labelId];

  CSFLSLayer& m_lz = m_sflsList[labelId].m_lz;
  CSFLSLayer& m_ln1 = m_sflsList[labelId].m_ln1;
  CSFLSLayer& m_lp1 = m_sflsList[labelId].m_lp1;
  CSFLSLayer& m_ln2 = m_sflsList[labelId].m_ln2;
  CSFLSLayer& m_lp2 = m_sflsList[labelId].m_lp2;

  /*--------------------------------------------------
    1. add F to phi(Lz), create Sn1 & Sp1 
    scan Lz values [-2.5 -1.5)[-1.5 -.5)[-.5 .5](.5 1.5](1.5 2.5]
    ========                */
  {
    //std::list<double>::const_iterator itf = m_force.begin();

    long nz = m_lz.size();
    std::vector<CSFLSLayer::iterator> m_lzIterVct( nz );
    {
      long iiizzz = 0;
      for (CSFLSLayer::iterator itz = m_lz.begin(); itz != m_lz.end(); ++itz)
        m_lzIterVct[iiizzz++] = itz;    
    }

    //    for (CSFLSLayer::iterator itz = m_lz.begin(); itz != m_lz.end(); ++itf)
    //#pragma omp parallel for
    for (long iiizzz = 0; iiizzz < nz; ++iiizzz)
      {
        long itf = iiizzz;

        CSFLSLayer::iterator itz = m_lzIterVct[iiizzz];

        long ix = (*itz)[0];
        long iy = (*itz)[1];
        long iz = (*itz)[2];

        TIndex idx = {{ix, iy, iz}};

        double phi_old = mp_phi->GetPixel(idx);
        double phi_new = phi_old + m_force[itf];


        mp_phi->SetPixel(idx, phi_new);

        if(phi_new > 0.5)
          {
            Sp1.push_back(*itz);
            itz = m_lz.erase(itz);
          }
        else if (phi_new < -0.5)
          {
            Sn1.push_back(*itz);
            itz = m_lz.erase(itz);
          }
        else
          {
            ++itz;              
          }
        /*--------------------------------------------------
          NOTE, mp_label are (should) NOT update here. They should
          be updated with Sz, Sn/p's
          --------------------------------------------------*/
      }
  }


  //     // debug
  //     labelsCoherentCheck1();


  /*--------------------------------------------------
    2. update Ln1,Lp1,Lp2,Lp2, ****in that order****

    2.1 scan Ln1 values [-2.5 -1.5)[-1.5 -.5)[-.5 .5](.5 1.5](1.5 2.5] 
    ==========                     */
  for (CSFLSLayer::iterator itn1 = m_ln1.begin(); itn1 != m_ln1.end(); )
    {
      long ix = (*itn1)[0];
      long iy = (*itn1)[1];
      long iz = (*itn1)[2];

      TIndex idx = {{ix, iy, iz}};

      double thePhi;
      bool found = getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(labelId, ix, iy, iz, thePhi);

      if (found)
        {
          double phi_new = thePhi-1;
          mp_phi->SetPixel(idx, phi_new);

          if (phi_new >= -0.5)
            {
              Sz.push_back(*itn1);
              itn1 = m_ln1.erase(itn1);
            }
          else if (phi_new < -1.5)
            {
              Sn2.push_back(*itn1);
              itn1 = m_ln1.erase(itn1);
            }
          else
            {
              ++itn1;
            }
        }
      else 
        {
          /*--------------------------------------------------
            No nbhd in inner (closer to zero contour) layer, so
            should go to Sn2. And the phi shold be further -1
          */
          Sn2.push_back(*itn1);
          itn1 = m_ln1.erase(itn1);

          mp_phi->SetPixel(idx, mp_phi->GetPixel(idx) - 1);  
        }
    }



  //     // debug
  //     labelsCoherentCheck1();


  /*--------------------------------------------------
    2.2 scan Lp1 values [-2.5 -1.5)[-1.5 -.5)[-.5 .5](.5 1.5](1.5 2.5] 
    ========          */
  for (CSFLSLayer::iterator itp1 = m_lp1.begin(); itp1 != m_lp1.end();)
    {
      long ix = (*itp1)[0];
      long iy = (*itp1)[1];
      long iz = (*itp1)[2];

      TIndex idx = {{ix, iy, iz}};

      double thePhi;
      bool found = getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(labelId, ix, iy, iz, thePhi);

      if (found)
        {
          double phi_new = thePhi+1;
          mp_phi->SetPixel(idx, phi_new);

          if (phi_new <= 0.5)
            {
              Sz.push_back(*itp1);
              itp1 = m_lp1.erase(itp1);
            }
          else if (phi_new > 1.5)
            {
              Sp2.push_back(*itp1);
              itp1 = m_lp1.erase(itp1);
            }
          else
            {
              ++itp1;
            }
        }
      else 
        {
          /*--------------------------------------------------
            No nbhd in inner (closer to zero contour) layer, so
            should go to Sp2. And the phi shold be further +1
          */

          Sp2.push_back(*itp1);
          itp1 = m_lp1.erase(itp1);

          mp_phi->SetPixel(idx, mp_phi->GetPixel(idx) + 1);  
        }
    }


  //     // debug
  //     labelsCoherentCheck1();



  /*--------------------------------------------------
    2.3 scan Ln2 values [-2.5 -1.5)[-1.5 -.5)[-.5 .5](.5 1.5](1.5 2.5] 
    ==========                                      */
  for (CSFLSLayer::iterator itn2 = m_ln2.begin(); itn2 != m_ln2.end(); )
    {
      long ix = (*itn2)[0];
      long iy = (*itn2)[1];
      long iz = (*itn2)[2];

      TIndex idx = {{ix, iy, iz}};

      double thePhi;
      bool found = getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(labelId, ix, iy, iz, thePhi);

      if (found)
        {
          double phi_new = thePhi-1;
          mp_phi->SetPixel(idx, phi_new);

          if (phi_new >= -1.5)
            {
              Sn1.push_back(*itn2);
              itn2 = m_ln2.erase(itn2);
            }
          else if (phi_new < -2.5)
            {
              itn2 = m_ln2.erase(itn2);
              mp_phi->SetPixel(idx, -3);
              mp_label->SetPixel(idx, -3);
            }
          else
            {
              ++itn2;
            }
        }
      else 
        {
          itn2 = m_ln2.erase(itn2);
          mp_phi->SetPixel(idx, -3);
          mp_label->SetPixel(idx, -3);
        }
    }


  //     // debug
  //     labelsCoherentCheck1();



  /*--------------------------------------------------
    2.4 scan Lp2 values [-2.5 -1.5)[-1.5 -.5)[-.5 .5](.5 1.5](1.5 2.5]
    ========= */
  for (CSFLSLayer::iterator itp2 = m_lp2.begin(); itp2 != m_lp2.end(); )
    {
      long ix = (*itp2)[0];
      long iy = (*itp2)[1];
      long iz = (*itp2)[2];
      TIndex idx = {{ix, iy, iz}};

      double thePhi;
      bool found = getPhiOfTheNbhdWhoIsClosestToZeroLevelInLayerCloserToZeroLevel(labelId, ix, iy, iz, thePhi);

      if (found)
        {
          double phi_new = thePhi+1;
          mp_phi->SetPixel(idx, phi_new);

          if (phi_new <= 1.5)
            {
              Sp1.push_back(*itp2);
              itp2 = m_lp2.erase(itp2);
            }
          else if (phi_new > 2.5)
            {
              itp2 = m_lp2.erase(itp2);
              mp_phi->SetPixel(idx, 3);
              mp_label->SetPixel(idx, 3);
            }
          else
            {
              ++itp2;
            }
        }
      else 
        {
          itp2 = m_lp2.erase(itp2);
          mp_phi->SetPixel(idx, 3);
          mp_label->SetPixel(idx, 3);
        }
    }


  //     // debug
  //     labelsCoherentCheck1();



  /*--------------------------------------------------
    3. Deal with S-lists Sz,Sn1,Sp1,Sn2,Sp2 
    3.1 Scan Sz */
  for (CSFLSLayer::iterator itSz = Sz.begin(); itSz != Sz.end(); ++itSz)
    {
      long ix = (*itSz)[0];
      long iy = (*itSz)[1];
      long iz = (*itSz)[2];
      TIndex idx = {{ix, iy, iz}};

        
      m_lz.push_back(*itSz);
      mp_label->SetPixel(idx, 0);
    }


  //     // debug
  //     labelsCoherentCheck1();



  /*--------------------------------------------------
    3.2 Scan Sn1     */
  for (CSFLSLayer::iterator itSn1 = Sn1.begin(); itSn1 != Sn1.end(); ++itSn1)
    {
      long ix = (*itSn1)[0];
      long iy = (*itSn1)[1];
      long iz = (*itSn1)[2];

      TIndex idx = {{ix, iy, iz}};
        
      m_ln1.push_back(*itSn1);
      // itSn1 = Sn1.erase(itSn1);

      mp_label->SetPixel(idx, -1);

      TIndex idx1 = {{ix+1, iy, iz}};
      if ( (ix+1 < m_nx) && doubleEqual(mp_phi->GetPixel(idx1), -3.0) )
        {
          Sn2.push_back(NodeType(ix+1, iy, iz));
          mp_phi->SetPixel(idx1, mp_phi->GetPixel(idx) - 1 ); 
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if ( (ix-1 >= 0) && doubleEqual(mp_phi->GetPixel(idx2), -3.0) )
        {
          Sn2.push_back(NodeType(ix-1, iy, iz));
          mp_phi->SetPixel(idx2, mp_phi->GetPixel(idx) - 1); 
        }

      TIndex idx3 = {{ix, iy+1, iz}};
      if ( (iy+1 < m_ny) && doubleEqual(mp_phi->GetPixel(idx3), -3.0) )
        {
          Sn2.push_back(NodeType(ix, iy+1, iz));
          mp_phi->SetPixel(idx3, mp_phi->GetPixel(idx) - 1 ); 
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if ( (iy-1>=0) && doubleEqual(mp_phi->GetPixel(idx4), -3.0) )
        {
          Sn2.push_back(NodeType(ix, iy-1, iz) );
          mp_phi->SetPixel(idx4, mp_phi->GetPixel(idx) - 1 );
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if ( (iz+1 < m_nz) && doubleEqual(mp_phi->GetPixel(idx5), -3.0) )
        {
          Sn2.push_back(NodeType(ix, iy, iz+1));
          mp_phi->SetPixel(idx5, mp_phi->GetPixel(idx) - 1 ); 
        }

      TIndex idx6 = {{ix, iy, iz-1}};
      if ( (iz-1>=0) && doubleEqual(mp_phi->GetPixel(idx6), -3.0) )
        {
          Sn2.push_back(NodeType(ix, iy, iz-1) );
          mp_phi->SetPixel(idx6, mp_phi->GetPixel(idx) - 1 );
        }
    }


  //     // debug
  //     labelsCoherentCheck1();


  /*--------------------------------------------------
    3.3 Scan Sp1     */
  for (CSFLSLayer::iterator itSp1 = Sp1.begin(); itSp1 != Sp1.end(); ++itSp1)
    {
      long ix = (*itSp1)[0];
      long iy = (*itSp1)[1];
      long iz = (*itSp1)[2];

      TIndex idx = {{ix, iy, iz}};

        
      m_lp1.push_back(*itSp1);
      mp_label->SetPixel(idx, 1);

      TIndex idx1 = {{ix+1, iy, iz}};
      if ( (ix+1 < m_nx) && doubleEqual(mp_phi->GetPixel(idx1), 3.0) )
        {
          Sp2.push_back(NodeType(ix+1, iy, iz));
          mp_phi->SetPixel(idx1, mp_phi->GetPixel(idx) + 1 ); 
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if ( (ix-1 >= 0) && doubleEqual(mp_phi->GetPixel(idx2), 3.0) )
        {
          Sp2.push_back(NodeType(ix-1, iy, iz));
          mp_phi->SetPixel(idx2, mp_phi->GetPixel(idx) + 1); 
        }

      TIndex idx3 = {{ix, iy+1, iz}};
      if ( (iy+1 < m_ny) && doubleEqual(mp_phi->GetPixel(idx3), 3.0) )
        {
          Sp2.push_back(NodeType(ix, iy+1, iz));
          mp_phi->SetPixel(idx3, mp_phi->GetPixel(idx) + 1 ); 
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if ( (iy-1>=0) && doubleEqual(mp_phi->GetPixel(idx4), 3.0) )
        {
          Sp2.push_back(NodeType(ix, iy-1, iz) );
          mp_phi->SetPixel(idx4, mp_phi->GetPixel(idx) + 1 );
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if ( (iz+1 < m_nz) && doubleEqual(mp_phi->GetPixel(idx5), 3.0) )
        {
          Sp2.push_back(NodeType(ix, iy, iz+1));
          mp_phi->SetPixel(idx5, mp_phi->GetPixel(idx) + 1 ); 
        }

      TIndex idx6 = {{ix, iy, iz-1}};
      if ( (iz-1>=0) && doubleEqual(mp_phi->GetPixel(idx6), 3.0) )
        {
          Sp2.push_back(NodeType(ix, iy, iz-1) );
          mp_phi->SetPixel(idx6, mp_phi->GetPixel(idx) + 1 );
        }
    } 


  //     // debug
  //     labelsCoherentCheck1();


  /*--------------------------------------------------
    3.4 Scan Sn2     */
  {
    //debug 
    int aaa = 0;
    for (CSFLSLayer::iterator itSn2 = Sn2.begin(); itSn2 != Sn2.end(); ++itSn2, ++aaa)
      {
        long ix = (*itSn2)[0];
        long iy = (*itSn2)[1];
        long iz = (*itSn2)[2];

        TIndex idx = {{ix, iy, iz}};
        
        m_ln2.push_back(*itSn2);

        mp_label->SetPixel(idx, -2);

        //           // debug
        //           labelsCoherentCheck1();
      }
  }



  /*--------------------------------------------------
    3.5 Scan Sp2     */
  for (CSFLSLayer::iterator itSp2 = Sp2.begin(); itSp2 != Sp2.end(); ++itSp2)
    {
      long ix = (*itSp2)[0];
      long iy = (*itSp2)[1];
      long iz = (*itSp2)[2];

      TIndex idx = {{ix, iy, iz}};
        
      m_lp2.push_back(*itSp2);

      mp_label->SetPixel(idx, 2);
    }


  //     // debug
  //     labelsCoherentCheck1();
  
  
  return;
}

/* ============================================================
   oneStepLevelSetEvolution    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::oneStepLevelSetEvolution()
{
  if (!m_labelMapPreprocessed)
    {
      std::cerr<<"Error: label map not pre-processed, that is prerequisite for all.\n";
      raise(SIGABRT);
    }


  for (short i = 0; i < m_numOfObjects; ++i)
    {
      oneStepLevelSetEvolution(i);
    }

  return;
}

/*================================================================================
  initializeLabel*/
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::initializeLabel()
{
  //find interface and mark as 0, create Lz
  char defaultLabel = 0;

  for (short i = 0; i < m_numOfObjects; ++i)
    {
      typename LabelImageType::Pointer thisLabel = LabelImageType::New();
      TRegion region = mp_img->GetLargestPossibleRegion();
  
      thisLabel->SetRegions( region );
      thisLabel->Allocate();

      thisLabel->CopyInformation(mp_img);

      thisLabel->FillBuffer(defaultLabel);


      m_labelList.push_back(thisLabel);
    }



  return;
}


/*================================================================================
  initializePhi*/
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::initializePhi()
{
  double arbitraryInitPhi = 1000;
  //  mp_phi.reset(new cArray3D< double >(m_nx, m_ny, m_nz, arbitraryInitPhi) );

  
  for (short i = 0; i < m_numOfObjects; ++i)
    {
      typename LSImageType::Pointer thisPhi = LSImageType::New();
      TRegion region = mp_img->GetLargestPossibleRegion();
  
      thisPhi->SetRegions( region );
      thisPhi->Allocate();

      thisPhi->CopyInformation(mp_img);

      thisPhi->FillBuffer(arbitraryInitPhi);


      m_phiList.push_back(thisPhi);
    }



  return;
}



/* ============================================================
   initializeIthSFLSFromLabelMap    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::initializeIthSFLSFromLabelMap(short i)
{
  short thisLabel = m_objectLabelList[i];

  for (long iz = 0; iz < m_nz; ++iz) 
    {
      for (long iy = 0; iy < m_ny; ++iy) 
        {
          for (long ix = 0; ix < m_nx; ++ix) 
            {
              TIndex idx = {{ix, iy, iz}};
              TIndex idx1 = {{ix-1, iy, iz}};
              TIndex idx2 = {{ix+1, iy, iz}};
              TIndex idx3 = {{ix, iy-1, iz}};
              TIndex idx4 = {{ix, iy+1, iz}};
              TIndex idx5 = {{ix, iy, iz-1}};
              TIndex idx6 = {{ix, iy, iz+1}};

              //mark the inside and outside of label and phi
              if( m_labelMap->GetPixel(idx) != thisLabel )
                { 
                  (m_labelList[i])->SetPixel(idx, 3); 
                  (m_phiList[i])->SetPixel(idx, 3); 
                }
              else
                { 
                  (m_labelList[i])->SetPixel(idx, -3); 
                  (m_phiList[i])->SetPixel(idx, -3); 


                  if ( (iy+1 < m_ny && m_labelMap->GetPixel(idx4) != thisLabel) \
                       || (iy-1 >= 0 && m_labelMap->GetPixel(idx3) != thisLabel) \
                       || (ix+1 < m_nx && m_labelMap->GetPixel(idx2) != thisLabel) \
                       || (ix-1 >= 0 && m_labelMap->GetPixel(idx1) != thisLabel) \
                       || (iz+1 < m_nz && m_labelMap->GetPixel(idx6) != thisLabel) \
                       || (iz-1 >= 0 && m_labelMap->GetPixel(idx5) != thisLabel) )
                    {
                      m_sflsList[i].m_lz.push_back(NodeType(ix, iy, iz));

                      (m_labelList[i])->SetPixel(idx, 0);
                      (m_phiList[i])->SetPixel(idx, 0.0);
                    }
                }
            }
        }
    }



  //scan Lz to create Ln1 and Lp1
  for (CSFLSLayer::const_iterator it = m_sflsList[i].m_lz.begin(); it != m_sflsList[i].m_lz.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];



      if(ix+1 < m_nx)
        {
          TIndex idx = {{ix+1, iy, iz}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back( NodeType(ix+1, iy, iz) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix+1, iy, iz) );
            }
        }
          
      if(ix-1 >= 0)
        {
          TIndex idx = {{ix-1, iy, iz}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back( NodeType(ix-1, iy, iz) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix-1, iy, iz) );
            }
        }

      if(iy+1 < m_ny)
        {
          TIndex idx = {{ix, iy+1, iz}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back(NodeType(ix, iy+1, iz) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix, iy+1, iz) );
            }
        }

      if(iy-1 >= 0)
        {
          TIndex idx = {{ix, iy-1, iz}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back( NodeType(ix, iy-1, iz) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix, iy-1, iz) );
            }
        }

      if(iz+1 < m_nz)
        {
          TIndex idx = {{ix, iy, iz+1}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back(NodeType(ix, iy, iz+1) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix, iy, iz+1) );
            }
        }

      if(iz-1 >= 0)
        {
          TIndex idx = {{ix, iy, iz-1}};

          if ( (m_labelList[i])->GetPixel(idx) == 3 )
            {
              (m_labelList[i])->SetPixel(idx, 1); 
              (m_phiList[i])->SetPixel(idx, 1); 
            
              m_sflsList[i].m_lp1.push_back( NodeType(ix, iy, iz-1) );
            }
          else if ( (m_labelList[i])->GetPixel(idx) == -3 )
            {
              (m_labelList[i])->SetPixel(idx, -1); 
              (m_phiList[i])->SetPixel(idx, -1); 
            
              m_sflsList[i].m_ln1.push_back( NodeType(ix, iy, iz-1) );
            }
        }
    }


  //scan Ln1 to create Ln2
  for (CSFLSLayer::const_iterator it = m_sflsList[i].m_ln1.begin(); it != m_sflsList[i].m_ln1.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];

      TIndex idx1 = {{ix+1, iy, iz}};
      if(ix+1 < m_nx && (m_labelList[i])->GetPixel(idx1) == -3 )
        {
          (m_labelList[i])->SetPixel(idx1, -2); 
          (m_phiList[i])->SetPixel(idx1, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix+1, iy, iz) );
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if(ix-1 >= 0 && (m_labelList[i])->GetPixel(idx2) == -3 )
        {
          (m_labelList[i])->SetPixel(idx2, -2); 
          (m_phiList[i])->SetPixel(idx2, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix-1, iy, iz) );
        }

      TIndex idx3 = {{ix, iy+1, iz}};
      if(iy+1 < m_ny && (m_labelList[i])->GetPixel(idx3) == -3 )
        {
          (m_labelList[i])->SetPixel(idx3, -2); 
          (m_phiList[i])->SetPixel(idx3, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix, iy+1, iz) );
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if(iy-1 >= 0 && (m_labelList[i])->GetPixel(idx4) == -3 )
        {
          (m_labelList[i])->SetPixel(idx4, -2); 
          (m_phiList[i])->SetPixel(idx4, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix, iy-1, iz) );
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if(iz+1 < m_nz && (m_labelList[i])->GetPixel(idx5) == -3 )
        {
          (m_labelList[i])->SetPixel(idx5, -2); 
          (m_phiList[i])->SetPixel(idx5, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix, iy, iz+1) );
        }


      TIndex idx6 = {{ix, iy, iz-1}};
      if(iz-1 >= 0 && (m_labelList[i])->GetPixel(idx6) == -3 )
        {
          (m_labelList[i])->SetPixel(idx6, -2); 
          (m_phiList[i])->SetPixel(idx6, -2); 
            
          m_sflsList[i].m_ln2.push_back( NodeType(ix, iy, iz-1) );
        }
    }

  //scan Lp1 to create Lp2
  for (CSFLSLayer::const_iterator it = m_sflsList[i].m_lp1.begin(); it != m_sflsList[i].m_lp1.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];

      TIndex idx1 = {{ix+1, iy, iz}};
      if(ix+1 < m_nx && (m_labelList[i])->GetPixel(idx1) == 3 )
        {
          (m_labelList[i])->SetPixel(idx1, 2); 
          (m_phiList[i])->SetPixel(idx1, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix+1, iy, iz) );
        }

      TIndex idx2 = {{ix-1, iy, iz}};
      if(ix-1 >= 0 && (m_labelList[i])->GetPixel(idx2) == 3 )
        {
          (m_labelList[i])->SetPixel(idx2, 2); 
          (m_phiList[i])->SetPixel(idx2, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix-1, iy, iz) );
        }

      TIndex idx3 = {{ix, iy+1, iz}};
      if(iy+1 < m_ny && (m_labelList[i])->GetPixel(idx3) == 3 )
        {
          (m_labelList[i])->SetPixel(idx3, 2); 
          (m_phiList[i])->SetPixel(idx3, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix, iy+1, iz) );
        }

      TIndex idx4 = {{ix, iy-1, iz}};
      if(iy-1 >= 0 && (m_labelList[i])->GetPixel(idx4) == 3 )
        {
          (m_labelList[i])->SetPixel(idx4, 2); 
          (m_phiList[i])->SetPixel(idx4, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix, iy-1, iz) );
        }

      TIndex idx5 = {{ix, iy, iz+1}};
      if(iz+1 < m_nz && (m_labelList[i])->GetPixel(idx5) == 3 )
        {
          (m_labelList[i])->SetPixel(idx5, 2); 
          (m_phiList[i])->SetPixel(idx5, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix, iy, iz+1) );
        }

      TIndex idx6 = {{ix, iy, iz-1}};
      if(iz-1 >= 0 && (m_labelList[i])->GetPixel(idx6) == 3 )
        {
          (m_labelList[i])->SetPixel(idx6, 2); 
          (m_phiList[i])->SetPixel(idx6, 2); 
            
          m_sflsList[i].m_lp2.push_back( NodeType(ix, iy, iz-1) );
        }
    }

  return;
}

/* ============================================================
   initializeSFLSFromLabelMap    */
template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::initializeSFLSFromLabelMap()
{
  if (!m_labelMap)
    {
      std::cerr<<"set m_labelMap first.\n";
      raise(SIGABRT);
    }

  if (0 == m_numOfObjects)
    {
      std::cerr<<"Error: 0 == m_numOfObjects.\n";
      raise(SIGABRT);
    }


  initializePhi();
  initializeLabel();


  for (short i = 0; i < m_numOfObjects; ++i)
    {
      initializeIthSFLSFromLabelMap(i);
    }

  return;
}


// /* ============================================================
//    doSegmenation    */
// template< typename TPixel >
// void
// SFLSMultiSegmentor3D_c< TPixel >
// ::doSegmenation()
// {
//   /*============================================================
//    * From the initial mask, generate: 1. SFLS, 2. mp_label and
//    * 3. mp_phi.      
//    */
//   initializeSFLS();

//   //douher::saveAsImage3< double >(mp_phi, "initPhi.nrrd");

//   for (unsigned int it = 0; it < m_numIter; ++it)
//     {
//       std::cout<<"iteration "<<it<<"\n"<<std::flush;

//       /*--------------------------------------------------
//         Compute the force on the zero level set, NOT on the whole domain.
//         This is NOT implemented in this base class.    

//         This function will compute the m_force. m_force has the same
//         size as the m_ln, indicating the change at each pixel on the
//         zero level set.
//       */
//       computeForce(); 


//       normalizeForce();

//       //         // debug
//       //         for (std::list< double >::const_iterator itf = this->m_force.begin(); itf != this->m_force.end(); ++itf)
//       //           {
//       //             std::cout<<(*itf)<<", ";
//       //           }
//       //         std::cout<<std::endl<<it<<std::endl<<std::endl;



//       //         //debug//
//       //         labelsCoherentCheck1();

//       oneStepLevelSetEvolution();


//       //         //debug//
//       //         std::cout<<"-----------------------"<<it<<"---------------------------"<<std::endl;
//       //         std::cout<<"lz \t ln1 \t ln2 \t lp1 \t lp2 \n";
//       //         std::cout<<m_lz.size()<<"\t"<<m_ln1.size()<<"\t"<<m_ln2.size()<<"\t"<<m_lp1.size()<<"\t"<<m_lp2.size()<<std::endl;
//       //         std::cout<<"--------------------------------------------------"<<std::endl;


//       //         // debug
//       //         labelsCoherentCheck1();


//       //        douher::saveAsImage3< double >(mp_phi, "temp.nrrd");

// updateInsideVoxelCount();
//     }
// }


// /* getLevelSetFunction */
// template< typename TPixel >
// itk::Image<double, 3>::Pointer
// SFLSMultiSegmentor3D_c< TPixel >
// ::getLevelSetFunction()
// {
//   if (!m_done)
//     {
//       std::cerr<<"Error: not done.\n";
//       raise(SIGABRT);
//     }

//   return mp_phi;
// }


template< typename TPixel >
typename itk::Image<short, 3>::Pointer
SFLSMultiSegmentor3D_c< TPixel >
::getFinalLabelMap()
{
  if (m_finalLabelList.empty())
    {
      getFinalLabelList();
    }

  typename labelMap_t::Pointer combinedLabelMap = labelMap_t::New();
  combinedLabelMap->SetRegions(mp_img->GetLargestPossibleRegion() );
  combinedLabelMap->CopyInformation(mp_img);
  combinedLabelMap->Allocate();
  combinedLabelMap->FillBuffer(0);

  typedef itk::ImageRegionIterator< labelMap_t > labelMapIter_t;

  labelMapIter_t it1( combinedLabelMap, combinedLabelMap->GetLargestPossibleRegion() );

  for (short i = 0; i < m_numOfObjects; ++i)
    {
      it1.GoToBegin();

      labelMap_t::Pointer thisLabelMap = m_finalLabelList[i];

      labelMapIter_t it2( thisLabelMap, thisLabelMap->GetLargestPossibleRegion() );
      it2.GoToBegin();
  
      for (; !it1.IsAtEnd(); ++it1, ++it2)
        {
          it1.Set(it1.Get() + it2.Get() );
        }
    }


  return combinedLabelMap;
}


template< typename TPixel >
void
SFLSMultiSegmentor3D_c< TPixel >
::getFinalLabelList()
{
  if (!m_done)
    {
      std::cerr<<"Error: work not done.\n";
      raise(SIGABRT);
    }

  if (!m_finalLabelList.empty())
    {
      if(m_finalLabelList.size() != (unsigned)m_numOfObjects)
        {
          std::cerr<<"Error: m_finalLabelList.size() != m_numOfObjects\n";
          raise(SIGABRT);
        }
    }

  typedef itk::ImageRegionIterator< LSImageType > itkImageRegionIterator_t;
  typedef itk::ImageRegionIterator< labelMap_t > maskImageRegionIterator_t;


  for (short i = 0; i < m_numOfObjects; ++i)
    {
      typename labelMap_t::Pointer bin = labelMap_t::New();
      bin->SetRegions(mp_img->GetLargestPossibleRegion() );
      bin->CopyInformation(mp_img);
      bin->Allocate();
      bin->FillBuffer(0);

      
      itkImageRegionIterator_t it1( m_phiList[i], m_phiList[i]->GetLargestPossibleRegion() );
      it1.GoToBegin();

      maskImageRegionIterator_t it2( bin, bin->GetLargestPossibleRegion() );
      it2.GoToBegin();
  
      for (; !it1.IsAtEnd(); ++it1, ++it2)
        {
          LSImageType::PixelType v = it1.Get();

          if (v <= 0)
            {
              it2.Set(i+1);
            }
        }
      
      m_finalLabelList.push_back(bin);
    }

  return;
}


/*============================================================
  computeKappa 

  Compute kappa at a point in the zero level set  */
template< typename TPixel >
double
SFLSMultiSegmentor3D_c< TPixel >
::computeKappa(short labelId, long ix, long iy, long iz)
{
  //double kappa = 0;

  double dx = 0;
  double dy = 0;
  double dz = 0;

  double dxx = 0;
  double dyy = 0;
  double dzz = 0;

  double dx2 = 0;
  double dy2 = 0;
  double dz2 = 0;

  double dxy = 0;
  double dxz = 0;
  double dyz = 0;

  char xok = 0;
  char yok = 0;
  char zok = 0;

  TIndex idx = {{ix, iy, iz}};
  TIndex idx1 = {{ix-1, iy, iz}};
  TIndex idx2 = {{ix+1, iy, iz}};
  TIndex idx3 = {{ix, iy-1, iz}};
  TIndex idx4 = {{ix, iy+1, iz}};
  TIndex idx5 = {{ix, iy, iz-1}};
  TIndex idx6 = {{ix, iy, iz+1}};


  typename LSImageType::Pointer mp_phi = m_phiList[labelId];
  typename LabelImageType::Pointer mp_label = m_labelList[labelId];


  if( ix+1 < m_nx && ix-1 >=0 )
    { 
      xok = 1;
    }

  if( iy+1 < m_ny && iy-1 >=0 ) 
    {
      yok = 1;
    }

  if( iz+1 < m_nz && iz-1 >=0 ) 
    {
      zok = 1;
    }

  if (xok)
    {
      dx  = (mp_phi->GetPixel(idx2) - mp_phi->GetPixel(idx1) )/(2.0*m_dx);
      dxx = (mp_phi->GetPixel(idx2) - 2.0*(mp_phi->GetPixel(idx)) + mp_phi->GetPixel(idx1))/(m_dx*m_dx);
      dx2 = dx*dx;
    }

  if (yok)
    {
      dy  = ((mp_phi->GetPixel(idx4) - mp_phi->GetPixel(idx3) ))/(2.0*m_dy);
      dyy = (mp_phi->GetPixel(idx4) - 2*(mp_phi->GetPixel(idx)) + mp_phi->GetPixel(idx3))/(m_dy*m_dy);
      dy2 = dy*dy;
    }

  if (zok)
    {
      dz  = ((mp_phi->GetPixel(idx6) - mp_phi->GetPixel(idx5) ))/(2.0*m_dz);
      dzz = (mp_phi->GetPixel(idx6) - 2.0*(mp_phi->GetPixel(idx)) + mp_phi->GetPixel(idx5))/(m_dz*m_dz);
      dz2 = dz*dz;
    }


  if(xok && yok)
    {
      TIndex idx_1 = {{ix+1, iy+1, iz}};
      TIndex idx_2 = {{ix-1, iy-1, iz}};
      TIndex idx_3 = {{ix+1, iy-1, iz}};
      TIndex idx_4 = {{ix-1, iy+1, iz}};

      dxy = 0.25*(mp_phi->GetPixel(idx_1) + mp_phi->GetPixel(idx_2) - mp_phi->GetPixel(idx_3) - mp_phi->GetPixel(idx_4)) \
        /(m_dx*m_dy);
    }

  if(xok && zok)
    {
      TIndex idx_1 = {{ix+1, iy, iz+1}};
      TIndex idx_2 = {{ix-1, iy, iz-1}};
      TIndex idx_3 = {{ix+1, iy, iz-1}};
      TIndex idx_4 = {{ix-1, iy, iz+1}};

      dxz = 0.25*(mp_phi->GetPixel(idx_1) + mp_phi->GetPixel(idx_2) - mp_phi->GetPixel(idx_3) - mp_phi->GetPixel(idx_4)) \
        /(m_dx*m_dz);
    }

  if(yok && zok)
    {
      TIndex idx_1 = {{ix, iy+1, iz+1}};
      TIndex idx_2 = {{ix, iy-1, iz-1}};
      TIndex idx_3 = {{ix, iy+1, iz-1}};
      TIndex idx_4 = {{ix, iy-1, iz+1}};

      dyz = 0.25*(mp_phi->GetPixel(idx_1) + mp_phi->GetPixel(idx_2) - mp_phi->GetPixel(idx_3) - mp_phi->GetPixel(idx_4)) \
        /(m_dy*m_dz);
    }

  return (dxx*(dy2 + dz2) + dyy*(dx2 + dz2) + dzz*(dx2 + dy2) - 2*dx*dy*dxy - 2*dx*dz*dxz - 2*dy*dz*dyz)\
    /(dx2 + dy2 + dz2 + vnl_math::eps);
}


#endif
