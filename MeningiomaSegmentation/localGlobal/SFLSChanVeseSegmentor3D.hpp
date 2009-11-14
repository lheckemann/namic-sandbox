#ifndef SFLSChanVeseSegmentor3D_hpp_
#define SFLSChanVeseSegmentor3D_hpp_

#include "SFLSChanVeseSegmentor3D.h"

#include <algorithm>


#include <fstream>

#include "itkImageDuplicator.h"

/* ============================================================
   basicInit    */
template< typename TPixel >
void
CSFLSChanVeseSegmentor3D< TPixel >
::basicInit()
{
  SuperClassType::basicInit();
    
  m_curvatureWeight = 0.1;

  m_expectedPhysicalVolume = 1e10;
}  


/* ============================================================
   computeForce    */
template< typename TPixel >
void
CSFLSChanVeseSegmentor3D< TPixel >
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
CSFLSChanVeseSegmentor3D< TPixel >
::computeForce()
{
  this->m_force.clear();

  updateMeans();

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
          
        typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};

        kappaOnZeroLS[i] = this->computeKappa(ix, iy, iz);

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
// CSFLSChanVeseSegmentor3D< TPixel >
// ::adjustCurvatureWeight()
// {
//   /* 
//      If the curvature weight is too large s.t. the mask shrinks to
//      nothing, then half it until it grows.

//      To do so, run the algorithm for once, if the area decrease, half
//      the curvature weight and adjust again.
//   */

//   // make copies
//   typedef itk::ImageDuplicator< itk::Image<unsigned char, 3 > > MaskDuplicatorType; 
//   MaskDuplicatorType::Pointer duplicator = MaskDuplicatorType::New(); 
//   duplicator->SetInputImage(this->mp_mask); 
//   duplicator->Update(); 
//   itk::Image<unsigned char, 3 >::Pointer clonedMask = duplicator->GetOutput();



//   std::ofstream f("/tmp/c.txt", std::ios_base::app);

//   while(true)
//     {  
//       MaskDuplicatorType::Pointer duplicator1 = MaskDuplicatorType::New(); 
//       duplicator1->SetInputImage(clonedMask); 
//       duplicator1->Update(); 
//       this->mp_mask = duplicator1->GetOutput();


//       this->initializeSFLS();

//       computeMeans();    

//       double oldAreaIn = m_areaIn;

//       computeForce();
//       this->normalizeForce();
//       this->oneStepLevelSetEvolution();
  
//       updateMeans();


//       if  (oldAreaIn > m_areaIn)
//         {
//           f<<"m_curvatureWeight too big "<<m_curvatureWeight<<std::endl;

//           m_curvatureWeight -= 0.1;

//           f<<"m_curvatureWeight adjust to  "<<m_curvatureWeight<<std::endl;

//           if (m_curvatureWeight < 0)
//             {
//               m_curvatureWeight = 0;
//               return;
//             }
//         }
//       else
//         {
//           f<<"m_curvatureWeight fine "<<m_curvatureWeight<<std::endl;
//           f<<"no need to adjust.\n";
//           return;
//         }
//     }

//   f.close();
// }


/* ============================================================
   doChanVeseSegmenation    */
template< typename TPixel >
void
CSFLSChanVeseSegmentor3D< TPixel >
::doChanVeseSegmenation()
{
  /*============================================================
   * From the initial mask, generate: 1. SFLS, 2. mp_label and
   * 3. mp_phi.      
   */

  this->initializeSFLS();

  computeMeans();    


  //douher::saveAsImage2< double >(mp_phi, "initPhi.nrrd");
  for (unsigned int it = 0; it < this->m_numIter; ++it)
    {
      double oldVolume = m_areaIn;

      computeForce();

      if (it > 10 && oldVolume >= m_areaIn)
        {
          // stops growing
          std::ofstream f("/tmp/g.txt");
          f<<"oldVolume >= m_areaIn "<<oldVolume<<"\t"<<m_areaIn<<std::endl;
          f<<"stops growing\n";
          f.close();
          return;
        }

      this->normalizeForce();

      this->oneStepLevelSetEvolution();

      double volumeIn = m_areaIn*(this->m_dx)*(this->m_dy)*(this->m_dz);

      if (volumeIn > m_expectedPhysicalVolume)
        {
          //          std::fstream f("/tmp/o.txt", std::ios_base::app);
          std::ofstream f("/tmp/o.txt");
          f<<"m_expectedPhysicalVolume = "<<m_expectedPhysicalVolume<<std::endl;
          f<<"volumeIn = "<<volumeIn<<std::endl;

          f<<"out from volume\n";
          f.close();
          return;
        }
    }


  //  std::fstream f1("/tmp/o1.txt", std::ios_base::app);
  std::ofstream f1("/tmp/o1.txt");
  f1<<"m_numIter = "<<this->m_numIter<<std::endl;
  f1<<"out from iteration\n";
  f1.close();

  return;
}


/* ============================================================
   computeMeans    */
template< typename TPixel >
void
CSFLSChanVeseSegmentor3D< TPixel >
::computeMeans()
{
  m_areaIn = 0;
  m_areaOut = 0;

  m_meanIn = 0;
  m_meanOut = 0;

  for (long ix = 0; ix < this->m_nx; ++ix)
    {
      for (long iy = 0; iy < this->m_ny; ++iy)
        {
          for (long iz = 0; iz < this->m_nz; ++iz)
            {
              typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};
                
              double phi = this->mp_phi->GetPixel(idx);
              double v = this->mp_img->GetPixel(idx);

              if (phi <= 0)
                {
                  ++m_areaIn;
                  m_meanIn += v;
                }
              else
                {
                  ++m_areaOut;
                  m_meanOut += v;
                }
            }
        }
    }

  m_meanIn /= (m_areaIn + vnl_math::eps);
  m_meanOut /= (m_areaOut + vnl_math::eps);
}


/* ============================================================
   updateMeans    */
template< typename TPixel >
void
CSFLSChanVeseSegmentor3D< TPixel >
::updateMeans()
{
  double sumIn = m_meanIn*m_areaIn;
  double sumOut = m_meanOut*m_areaOut;

  for (CSFLSLayer::const_iterator it = this->m_lIn2out.begin(); it != this->m_lIn2out.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];

      typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};
            
      sumIn  -= this->mp_img->GetPixel(idx); 
      --m_areaIn;

      sumOut += this->mp_img->GetPixel(idx); 
      ++m_areaOut;
    }

  for (CSFLSLayer::const_iterator it = this->m_lOut2in.begin(); it != this->m_lOut2in.end(); ++it)
    {
      long ix = (*it)[0];
      long iy = (*it)[1];
      long iz = (*it)[2];

      typename itk::Image<TPixel, 3>::IndexType idx = {{ix, iy, iz}};
            
      sumIn  += this->mp_img->GetPixel(idx);
      ++m_areaIn;

      sumOut -= this->mp_img->GetPixel(idx);
      --m_areaOut;
    }


  m_meanIn = (sumIn/m_areaIn + vnl_math::eps);
  m_meanOut = (sumOut/m_areaOut + vnl_math::eps);
}




#endif
