#ifndef cArrayOp_txx_
#define cArrayOp_txx_

#include "cArrayOp.h"

#include "shuxue.h"

#include <iostream>

#include "itkVectorImage.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_filewrite.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkContinuousIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include <cassert>
#include <csignal>


namespace douher
{
  /* ============================================================
     itkImageToVnlVector  */
  template< typename T, unsigned int dim > vnl_vector< T > itkImageToVnlVector(typename itk::Image< T, dim >::Pointer img)
  {
    typedef itk::Image< T, dim > ImageType;

    typename ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();

    long totalLen = 1;
    for (unsigned long i = 0; i < dim; ++i)
      {
        totalLen *= size[i];
      }

    //  std::cout<<"totalLen = "<<totalLen<<std::endl;

    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    IteratorType imIter(img, img->GetLargestPossibleRegion());

    long longVectorIdx = 0;
    vnl_vector< T > vct(totalLen);

    for (imIter.GoToBegin(); !imIter.IsAtEnd(); ++imIter)
      {
        vct[longVectorIdx++] = imIter.Get();
      }

    return vct;
  }


  /*============================================================
     vnlVectorToItkImage  */
  template< typename T, unsigned int dim > typename itk::Image< T, dim >::Pointer vnlVectorToItkImage(const vnl_vector< T >& vct, \
                                                                                                      long imSize[])
  {
    typedef itk::Image< T, dim > ImageType;
    typename ImageType::Pointer image = ImageType::New();

    typename ImageType::IndexType start;
    start.Fill(0);

    typename ImageType::SizeType size;
    for (unsigned long i = 0; i < dim; ++i)
      {
        size[i] = imSize[i];
      }

    typename ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);

    image->SetRegions(region);
    image->Allocate();

    typedef itk::ImageRegionIterator<ImageType> ImageIteratorType;
    ImageIteratorType Iter(image, image->GetLargestPossibleRegion());

    long longVectorIdx = 0;
    for (Iter.GoToBegin(); !Iter.IsAtEnd(); ++Iter)
      {
        Iter.Set( vct[longVectorIdx++] );
      }

    return image;
  }


  /* --------------------------------------------------
     readImage3
  */
  template< typename T > typename itk::Image< T, 3 >::Pointer readImage3(const char *fileName)
  {
    return readImage<T, 3>(fileName);
  }


  /* --------------------------------------------------
     readImage
  */
  template< typename T, unsigned int dim > typename itk::Image< T, dim >::Pointer readImage(const char *fileName)
  {
    typedef itk::Image< T, dim > ImageType;
    typedef itk::ImageFileReader< ImageType > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename ImageType::Pointer image;
    
    try
      {
        reader->Update();
        image = reader->GetOutput();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cerr<< "ExceptionObject caught !" << std::endl; 
        std::cerr<< err << std::endl; 
        raise(SIGABRT);
      }

    return image;
  }


  /* IO: write series of images */
  template< typename TData > 
  void
  writeImageSeries3( const std::vector< typename itk::Image< TData, 3 >::Pointer >& imgs, \
                     const std::vector< std::string >& imageNameList )
  {
    writeImageSeries<TData, 3>(imgs, imageNameList);
  }


  template< typename TData, unsigned int dim > 
  void
  writeImageSeries( const std::vector< typename itk::Image< TData, dim >::Pointer >& imgs, \
                    const std::vector< std::string >& imageNameList )
  {
    if (imgs.size() != imageNameList.size())
      {
        std::cerr<<"Error: imgs.size() != imageNameList.size()\n";
        raise(SIGABRT);
      }
    
    long n = imageNameList.size();
    for (long i = 0; i < n; ++i)
      {
        writeImage<TData, dim>(imgs[i], imageNameList[i].c_str());
      }

    return;
  }

  template< typename TData, unsigned int dim > 
  void
  writeImageSeries( const std::vector< typename itk::Image< TData, dim >::Pointer >& imgs)
  {
    long N = imgs.size();
    std::vector< std::string > imageNameList(N);

    char tmp[255];

    for (long i = 0; i < N; ++i)
      {
        sprintf(tmp, "_imageSeries%ld.mha", i);
        //sprintf(tmp, "_imageSeries%ld.nrrd", i);
        std::string thisName(tmp);

        imageNameList[i] = thisName;
      }

    writeImageSeries<TData, dim>( imgs, imageNameList);
  }



  /* IO: read series of images */
  template< typename TData > 
  std::vector< typename itk::Image< TData, 3 >::Pointer >
  readImageSeries3( const std::vector< std::string >& imageNameList )
  {
    return readImageSeries<TData, 3>(imageNameList);
  }

  template< typename TData, unsigned int dim > 
  std::vector< typename itk::Image< TData, dim >::Pointer >
  readImageSeries( const std::vector< std::string >& imageNameList )
  {
    typedef itk::Image< TData, dim > TImage;
    typedef typename TImage::Pointer TImagePointer;
    typedef std::vector< TImagePointer > TImageSeries;

    TImageSeries imageSeries;
    
    long n = imageNameList.size();
    for (long i = 0; i < n; ++i)
      {
        std::string thisName = imageNameList[i];
        
        TImagePointer img = readImage< TData, dim >( thisName.c_str() );

        imageSeries.push_back(img);
      }

    return imageSeries;
  }



  /*--------------------------------------------------
    writeImage3
  */
  template< typename T > void writeImage3(typename itk::Image< T, 3 >::Pointer img, const char *fileName)
  {
    writeImage<T, 3>(img, fileName);
  }


  /*--------------------------------------------------
    writeImage
  */
  template< typename T, unsigned int dim > void writeImage(typename itk::Image< T, dim >::Pointer img, const char *fileName)
  {
    typedef itk::Image< T, dim > ImageType;
    typedef itk::ImageFileWriter< ImageType > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(img);

    try
      {
        writer->Update();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cout << "ExceptionObject caught !" << std::endl; 
        std::cout << err << std::endl; 
        raise(SIGABRT);   
      }
  }




  /*============================================================
    readImageToArray3  */
  template< typename T > 
  typename cArray3D< T >::PointerType readImageToArray3(const char *fileName)
  {
    typedef itk::Image< T, 3 > ImageType;
    typedef itk::ImageFileReader< ImageType > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename ImageType::Pointer image;
    
    try
      {
        reader->Update();
        image = reader->GetOutput();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cerr<< "ExceptionObject caught !" << std::endl; 
        std::cerr<< err << std::endl; 
        raise(SIGABRT);
      }

  
    return itkImageToArray3< T >(image);
  }



  /*============================================================
    saveAsImage3  */
  template< typename T > 
  void saveAsImage3(typename cArray3D< T >::PointerType array3, const char *fileName)
  {
    typedef itk::Image< T, 3 > ImageType; // 3 means 3D  

    typename ImageType::Pointer image = cArray3ToItkImage< T >(array3);

    typedef itk::ImageFileWriter< ImageType > WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(image);

    try
      {
        writer->Update();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cout << "ExceptionObject caught !" << std::endl; 
        std::cout << err << std::endl; 
        raise(SIGABRT);   
      }

    return;
  }



  /*============================================================
    itkImageToArray3 */
  template< typename T > 
  typename cArray3D< T >::PointerType itkImageToArray3(typename itk::Image< T, 3 >::Pointer image)
  {
    typedef itk::Image< T, 3 > ImageType; // 3 means 3D  
    typename ImageType::RegionType::SizeType dataSz = (image->GetLargestPossibleRegion()).GetSize();

    typename cArray3D<T>::PointerType array3( new cArray3D< T >(dataSz[0], dataSz[1], dataSz[2]) );

    typedef itk::ImageRegionIterator< ImageType > ImageIteratorType;
    ImageIteratorType imIter(image, image->GetLargestPossibleRegion() );

    imIter.GoToBegin();

    for (unsigned long iz = 0; iz < dataSz[2]; ++iz)
      {
        for (unsigned long iy = 0; iy < dataSz[1]; ++iy)
          {
            for (unsigned long ix = 0; ix < dataSz[0]; ++ix)
              {
                /*
                  NOTE!!! the order: iz changes slowest while ix changes
                  fastest MATTERS! 

                  Since this is using together with iterator of
                  itkImage. There the iterator first goes thru x-dim,
                  then y-dim, then z-dim. So the fastest changing
                  dimension should be consistant.
                */
                (*array3)(ix, iy, iz) = imIter.Get();

                ++imIter;
              }
          }
      }

    typename ImageType::SpacingType sp = image->GetSpacing();
    array3->setSpacingX(sp[0]);
    array3->setSpacingY(sp[1]);
    array3->setSpacingZ(sp[2]);


    return array3;
  }

  /*============================================================
    cArray3ToItkImage  */
  template< typename T > 
  typename itk::Image< T, 3 >::Pointer cArray3ToItkImage(typename cArray3D< T >::PointerType array3)
  {
    typedef itk::Image< T, 3 > ImageType; // 3 means 3D  

    typename ImageType::Pointer image = ImageType::New();
  
    typename ImageType::IndexType imStart;
    imStart[0] = 0;  // first index on X
    imStart[1] = 0;  // first index on Y
    imStart[2] = 0;  // first index on Y

    typename ImageType::SizeType  imSize;
    imSize[0] = array3->getSizeX();  // size along X
    imSize[1] = array3->getSizeY();  // size along Y
    imSize[2] = array3->getSizeZ();  // size along Y

    typename ImageType::RegionType imRegion;
    imRegion.SetSize( imSize );
    imRegion.SetIndex( imStart );

    typename ImageType::SpacingType sp;
    sp[0] = array3->getSpacingX();
    sp[1] = array3->getSpacingY();
    sp[2] = array3->getSpacingZ();

    image->SetSpacing(sp);
    image->SetRegions( imRegion );
    image->Allocate();

    typedef itk::ImageRegionIterator< ImageType > IteratorType;
    IteratorType imIt(image, image->GetLargestPossibleRegion() );
    imIt.GoToBegin();

    for (long i = 0; !imIt.IsAtEnd(); ++imIt)
      {
        imIt.Set((*array3)[i]);
        ++i;
      }


    return image;
  }


  
  /* ============================================================
     cropArray 3D */
  template< typename T > 
  typename cArray3D< T >::PointerType cropArray(typename cArray3D< T >::PointerType array3, \
                                                long xMin, long xMax, long yMin, long yMax, long zMin, long zMax)
  {
    /*
      xMin is the min index, in the original array, to be *included* in
      the cropped array. While xMax is the max, in the original array,
      to be *included* in the cropped array.

      So the limit of xMin is 0, indicating: no crop at head. And the
      limit of xMax is nx-1, indicating: no crop at end.

      Same for y-dim.
    */
    long nx = array3->getSizeX();
    long ny = array3->getSizeY();
    long nz = array3->getSizeZ();

    if (xMin < 0 || xMax > nx-1 || yMin < 0 || yMax > ny-1 || zMin < 0 || zMax > nz-1)
      {
        std::cerr<<"cropping out of bounds, exit.\n";
        raise(SIGABRT);
      }

    if (xMin > xMax || yMin > yMax || zMin > zMax)
      {
        std::cerr<<"xMin > xMax || yMin > yMax || zMin > zMax, exit.\n";
        raise(SIGABRT);
      }

    long szX = xMax - xMin + 1;
    long szY = yMax - yMin + 1;
    long szZ = zMax - zMin + 1;

    cArray3D< T >* cropped = new cArray3D< T >(szX, szY, szZ);
  
    for (long ix = xMin; ix <= xMax; ++ix)
      {
        for (long iy = yMin; iy <= yMax; ++iy)
          {
            for (long iz = zMin; iz <= zMax; ++iz)
              {
                // Notice the "=" in the upper limit, since we want to
                // include the xMax and yMax and zMax
                long iix = ix - xMin;
                long iiy = iy - yMin;
                long iiz = iz - zMin;

                (*cropped)(iix, iiy, iiz) = (*array3)(ix, iy, iz);
              }
          }
      }


    return cropped;
  }



  /*============================================================
    const pad array 3D  */
  template< typename T > 
  typename cArray3D< T >::PointerType constPadArray(typename cArray3D< T >::PointerType array3, T padValue, \
                                                    long xHeadNum, long xTailNum, \
                                                    long yHeadNum, long yTailNum, \
                                                    long zHeadNum, long zTailNum)
  {
    long nx = array3->getSizeX();
    long ny = array3->getSizeY();
    long nz = array3->getSizeZ();

    long newNx = nx + xHeadNum + xTailNum;
    long newNy = ny + yHeadNum + yTailNum;
    long newNz = nz + zHeadNum + zTailNum;

    typename cArray3D< T >::PointerType padded( new cArray3D< T >(newNx, newNy, newNz, padValue) );

    for (long ix = 0; ix < nx; ++ix)
      {
        for (long iy = 0; iy < ny; ++iy)
          {
            for (long iz = 0; iz < nz; ++iz)
              {
                long newIx = ix + xHeadNum;
                long newIy = iy + yHeadNum;
                long newIz = iz + zHeadNum;

                (*padded)(newIx, newIy, newIz) = (*array3)(ix, iy, iz);
              }
          }
      }

    return padded;
  }


} // douher

////////////////////////////////////////////////////////////
#endif


