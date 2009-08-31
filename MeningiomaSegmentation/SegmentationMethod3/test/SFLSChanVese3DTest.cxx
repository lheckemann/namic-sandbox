#include <iostream>

//douher
#include "cArrayOp.h"
#include "cArray3D.h"

#include "../SFLSChanVeseSegmentor3D.h"

template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getMask(typename douher::cArray3D< TPixel >::Pointer img );

int main(int argc, char** argv)
{
  if (argc != 5)
    {
      std::cerr<<"Parameters: inputImage outputImage numIter lambda\n";
      exit(-1);
    }

  const char* inputImage = argv[1];
  const char* outputImage = argv[2];
  unsigned long numiter = atol(argv[3]);
  double lambda = atof(argv[4]);

  typedef double PixelType;
  typedef douher::cArray3D< PixelType > ImageType;
  typedef douher::cArray3D< unsigned char > MaskImageType;

  ImageType::Pointer img = douher::readImageToArray3< PixelType >(inputImage);
  MaskImageType::Pointer mask = getMask< PixelType >(img);

  douher::saveAsImage3< unsigned char >(mask, "initMask.nrrd");
  
  douher::CSFLSChanVeseSegmentor3D< PixelType >::Pointer cv = douher::CSFLSChanVeseSegmentor3D< PixelType >::New();
  cv->setImage(img);
  cv->setMask(mask);

  cv->setNumIter(numiter);

  cv->setCurvatureWeight(lambda);

  //cv->doSegmenation();
  cv->doChanVeseSegmenation();


  douher::saveAsImage3< double >(cv->mp_phi, outputImage);

  
  return 0;
}


template< typename TPixel >
douher::cArray3D< unsigned char >::Pointer 
getMask(typename douher::cArray3D< TPixel >::Pointer img )
{
  long nx = img->getSizeX();
  long ny = img->getSizeY();
  long nz = img->getSizeZ();

  douher::cArray3D< unsigned char >::Pointer mask(new douher::cArray3D< unsigned char >(nx, ny, nz, 0));

  for (long ix = nx/4; ix < 3*nx/4; ++ix)
    {
      for (long iy = ny/4; iy < 3*ny/4; ++iy)
        {
          for (long iz = nz/4; iz < 3*nz/4; ++iz)
            {
              mask->set(ix, iy, iz, 1);
            }
        }
    }

  return mask;  
}
