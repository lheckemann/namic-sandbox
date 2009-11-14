#ifndef SFLSLocalChanVeseSegmentor3D_hpp_
#define SFLSLocalChanVeseSegmentor3D_hpp_

#include "SFLSLocalChanVeseSegmentor3D.h"

#include <algorithm>



/* ============================================================
   basicInit    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::basicInit()
{
  SuperClassType::basicInit();
    
  m_curvatureWeight = 0.1;

  m_expectedPhysicalVolume = 1e10;

  // neighbor size
  m_nbx = 20;
  m_nby = 20;
  m_nbz = 3;
}  


/* ============================================================
   computeForce    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::setExpectedVolume(double v)
{
  if (v <= 0)
    {
      std::cerr<<"Error: v <= 0\n";
      raise(SIGABRT);
    }

  m_expectedPhysicalVolume = v;

  return;
}


/* ============================================================
   computeForce    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::computeForce()
{
  this->m_force.clear();

  double fmax = -1e10;
  double kappaMax = -1e10;

  long n = this->m_lz.size();
  double* kappaOnZeroLS = new double[ n ];
  double* cvForce = new double[ n ];

  {
    long i = 0;
    for (CSFLSLayer::iterator itz = this->m_lz.begin(); itz != this->m_lz.end(); ++itz, ++i)
      {
        long ix = (*itz)[0];
        long iy = (*itz)[1];
        long iz = (*itz)[2];

        kappaOnZeroLS[i] = this->computeKappa(ix, iy, iz);

        
        computeMeansAt(ix, iy, iz);

        typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};


        double I = this->mp_img->GetPixel(idx);
        double a = (I - m_meanIn)*(I - m_meanIn) - (I - m_meanOut)*(I - m_meanOut);

        fmax = fabs(a)>fmax?fabs(a):fmax;
        kappaMax = fabs(kappaOnZeroLS[i])>kappaMax?fabs(kappaOnZeroLS[i]):kappaMax;

        cvForce[i] = a;
      }
  }


  for (long i = 0; i < n; ++i)
    {
      this->m_force.push_back((1 - m_curvatureWeight)*cvForce[i]/(fmax + 1e-10) +  m_curvatureWeight*kappaOnZeroLS[i]/(kappaMax + 1e-10));
    }
    
  delete[] kappaOnZeroLS;
  delete[] cvForce;
}



// /* ============================================================
//    adjustCurvatureWeight    */
// template< typename TPixel >
// void
// CSFLSLocalChanVeseSegmentor3D< TPixel >
// ::adjustCurvatureWeight()
// {
//   /* 
//      If the curvature weight is too large s.t. the mask shrinks to
//      nothing, then half it until it grows.

//      To do so, run the algorithm for once, if the area decrease, half
//      the curvature weight and adjust again.
//   */

//   while(true)
//     {  
//       double oldAreaIn = m_voxelCountIn;

//       countInsideVoxel();

//       computeForce();
//       this->normalizeForce();
//       this->oneStepLevelSetEvolution();
  
//       updateCountInsideVoxel();

//       if  (oldAreaIn > m_voxelCountIn)
//         {
//           m_curvatureWeight -= 0.1;

//           if (m_curvatureWeight < 0)
//             {
//               m_curvatureWeight = 0;
//               return;
//             }
//         }
//       else
//         {
//           return;
//         }
//     }
// }

/* ============================================================
   countInsideVoxel    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::countInsideVoxel()
{
  m_voxelCountIn = 0;

  for (long ix = 0; ix < this->m_nx; ++ix)
    {
      for (long iy = 0; iy < this->m_ny; ++iy)
        {
          for (long iz = 0; iz < this->m_nz; ++iz)
            {
              typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};
                
              double phi = this->mp_phi->GetPixel(idx);

              if (phi <= 0)
                {
                  ++m_voxelCountIn;
                }
            }
        }
    }
  
  return;
}


/* ============================================================
   updateCountInsideVoxel    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::updateCountInsideVoxel()
{
  m_voxelCountIn -= (this->m_lIn2out).size();
  m_voxelCountIn += (this->m_lOut2in).size();

  return;
}



/* ============================================================
   doChanVeseSegmenation    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::doLocalChanVeseSegmenation()
{
  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */
  this->initializeSFLS();

  //adjustCurvatureWeight();


  //douher::saveAsImage3< double >(mp_phi, "initPhi.nrrd");
  for (unsigned int it = 0; it < this->m_numIter; ++it)
    {
      //         //debug//
      //      std::cout<<"In "<<it<<"-th iteration, check nan:\n"<<std::flush;
      //         checkNan();
      //         //DEBUG//

      computeForce();

      this->normalizeForce();

      this->oneStepLevelSetEvolution();


      updateCountInsideVoxel();

      double volumeIn = m_voxelCountIn*(this->m_dx)*(this->m_dy)*(this->m_dz);

      if (volumeIn > 1.5*m_expectedPhysicalVolume)
        {
          break;
        }
    }

  return;
}


/* ============================================================
   computeMeansAt    */
template< typename TPixel >
void
CSFLSLocalChanVeseSegmentor3D< TPixel >
::computeMeansAt(long ix, long iy, long iz)
{
  /*----------------------------------------------------------------------
    Compute the local meanIn/Out areaIn/Out at this pixel. */

  m_areaIn = 0;
  m_areaOut = 0;

  m_meanIn = 0;
  m_meanOut = 0;

    
  for (long iix = ix-m_nbx; iix <= ix+m_nbx; ++iix)
    {
      for (long iiy = iy-m_nby; iiy <= iy+m_nby; ++iiy)
        {
          for (long iiz = iz-m_nbz; iiz <= iz+m_nbz; ++iiz)
            {
              if (iix >= 0 && iix < this->m_nx && iiy >= 0 && iiy < this->m_ny && iiz >= 0 && iiz < this->m_nz)
                {
                  typename itk::Image<TPixel, 3>::IndexType idx = {{iix, iiy, iiz}};

                  TPixel imgVal = (this->mp_img)->GetPixel(idx);
                  double phi = (this->mp_phi)->GetPixel(idx);

                  if (phi <= 0)
                    {
                      // in
                      ++m_areaIn;
                      m_meanIn += imgVal;
                    }
                  else 
                    {
                      ++m_areaOut;
                      m_meanOut += imgVal;;
                    }
                }
            }
        }
    }

  m_meanIn /= (m_areaIn + vnl_math::eps);
  m_meanOut /= (m_areaOut + vnl_math::eps);

  return;
}



#endif
