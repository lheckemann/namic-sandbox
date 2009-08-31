/*
  Given the center point of the meningioma, the segmentor performs
  global/local statistics based active contour segmentation on the
  meningioma image.
*/

#include <iostream>

//douher
#include "cArrayOp.h"
#include "cArray3D.h"

#include "SFLSLocalChanVeseSegmentor3D.h"
#include "SFLSChanVeseSegmentor3D.h"


template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getInitMask(typename douher::cArray3D< TPixel >::Pointer img, long cx, long cy, long cz );


template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getFinalMask(typename douher::cArray3D< TPixel >::Pointer img, TPixel thod = 0);


int main(int argc, char** argv)
{
  if (argc != 9)
    {
      std::cerr<<"Parameters: inputImage outputImage numIter lambda cx cy cz globalSwitch\n";
      exit(-1);
    }

  const char* inputImage = argv[1];
  const char* outputImage = argv[2];
  unsigned long numiter = atol(argv[3]);
  double lambda = atof(argv[4]);
  long cx = atol(argv[5]);
  long cy = atol(argv[6]);
  long cz = atol(argv[7]);
  int globalSwitch = atoi(argv[8]);

  typedef double PixelType;
  typedef douher::cArray3D< PixelType > ImageType;
  typedef douher::cArray3D< unsigned char > MaskImageType;

  ImageType::Pointer img = douher::readImageToArray3< PixelType >(inputImage);
  MaskImageType::Pointer mask = getInitMask< PixelType >(img, cx, cy, cz);

  //  douher::saveAsImage3< unsigned char >(mask, "initMask.nrrd");
  
  if (1 == globalSwitch)
    {
      // perform global statistics based segmentation
      douher::CSFLSChanVeseSegmentor3D< PixelType >::Pointer cv = douher::CSFLSChanVeseSegmentor3D< PixelType >::New();
      cv->setImage(img);
      cv->setMask(mask);

      cv->setNumIter(numiter);

      cv->setCurvatureWeight(lambda);

      //cv->doSegmenation();
      cv->doChanVeseSegmenation();

      douher::saveAsImage3< unsigned char >(getFinalMask<double>(cv->mp_phi), outputImage);
    }
  else
    {
      // perform local statistics based segmentation
      douher::CSFLSLocalChanVeseSegmentor3D< PixelType >::Pointer cv = douher::CSFLSLocalChanVeseSegmentor3D< PixelType >::New();
      cv->setImage(img);
      cv->setMask(mask);

      cv->setNumIter(numiter);

      cv->setCurvatureWeight(lambda);
      cv->setNBHDSize(30, 30, 5);


      // do segmentation
      cv->doLocalChanVeseSegmenation();

      douher::saveAsImage3< unsigned char >(getFinalMask<double>(cv->mp_phi), outputImage);
    }



  
  return 0;
}


template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getInitMask(typename douher::cArray3D< TPixel >::Pointer img, long cx, long cy, long cz )
{
  long nx = img->getSizeX();
  long ny = img->getSizeY();
  long nz = img->getSizeZ();

  if (cx < 0 || cx > nx-1 || cy < 0 || cy > ny-1 || cz < 0 || cz > nz-1 )
    {
      std::cerr<<"(cx, cy, cz) not in the img domain. abort.\n";
      raise(SIGABRT);
    }

  douher::cArray3D< unsigned char >::Pointer mask(new douher::cArray3D< unsigned char >(nx, ny, nz, 0));


  for (long ix = cx-5; ix < cx+6; ++ix)
    {
      for (long iy = cy-5; iy < cy+6; ++iy)
        {
          for (long iz = cz-5; iz < cz+6; ++iz)
            {
              mask->set(ix, iy, iz, 1);
            }
        }
    }

  return mask;  
}



template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getFinalMask(typename douher::cArray3D< TPixel >::Pointer img, TPixel thod)
{
  long nx = img->getSizeX();
  long ny = img->getSizeY();
  long nz = img->getSizeZ();

  douher::cArray3D< unsigned char >::Pointer mask(new douher::cArray3D< unsigned char >(nx, ny, nz, 0));


  for (long ix = 0; ix < nx; ++ix)
    {
      for (long iy = 0; iy < ny; ++iy)
        {
          for (long iz = 0; iz < nz; ++iz)
            {
              TPixel v = img->get(ix, iy, iz);
              
              mask->set(ix, iy, iz, v<=thod?1:0);
            }
        }
    }

  return mask;
}
