#ifndef _ITKIO_H_
#define _ITKIO_H_

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkOrientImageFilter.h"
#include "itkSpatialOrientation.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkAnalyzeImageIO.h"
#include "itkMetaDataObject.h"
#include "itkImageRegionIterator.h"
#include "itkThinPlateR2LogRSplineKernelTransform.h"
#include "itkResampleImageFilter.h"
#include "itkImageDuplicator.h"
#include "itksys/SystemTools.hxx"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"


namespace itkUtil
{

  /** read an image using ITK -- image-based template */
  template <typename TImage>
  typename TImage::Pointer ReadImage( const std::string fileName, const bool zeroOrigin = false )
  {
    typename TImage::Pointer image;

    std::string extension = itksys::SystemTools::GetFilenameLastExtension(fileName);
    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    if ( dicomIO->CanReadFile(fileName.c_str()) || (itksys::SystemTools::Strucmp(extension.c_str(), ".dcm") == 0))
    {
     std::string dicomDir = itksys::SystemTools::GetParentDirectory(fileName.c_str());

     itk::GDCMSeriesFileNames::Pointer FileNameGenerator = itk::GDCMSeriesFileNames::New();
     FileNameGenerator->SetUseSeriesDetails( true );
     FileNameGenerator->SetDirectory( dicomDir );
     typedef const std::vector<std::string>      ContainerType;
     const ContainerType & seriesUIDs = FileNameGenerator->GetSeriesUIDs();
     
     typedef typename itk::ImageSeriesReader< TImage >     ReaderType;
     typename ReaderType::Pointer reader = ReaderType::New();
     //itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
     reader->SetFileNames( FileNameGenerator->GetFileNames( seriesUIDs[0] ) );
     reader->SetImageIO( dicomIO );
     try
     {
      reader->Update();
     }
     catch( itk::ExceptionObject & err )
      {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw err;
      }
    catch(...)
      {
      std::cout << "Error while reading in image for patient " << fileName << std::endl;
      throw;
      }
     image = reader->GetOutput();
    }
    else
    {
    typedef itk::ImageFileReader<TImage> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( fileName.c_str() );
    try
      {
      reader->Update();
      }
    catch( itk::ExceptionObject & err )
      {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
      throw err;
      }
    catch(...)
      {
      std::cout << "Error while reading in image for patient " << fileName << std::endl;
      throw;
      }
     image = reader->GetOutput();
    }

    if(zeroOrigin)
      {
      double origin[TImage::ImageDimension];
      for(unsigned int i =0 ; i< TImage::ImageDimension ; i++)
        {
        origin[i]=0;
        }
      image->SetOrigin(origin);
      }
    return image;
  }

  template <class ImageType>
  typename ImageType::Pointer
  OrientImage(typename ImageType::ConstPointer &inputImage,
              itk::SpatialOrientation::ValidCoordinateOrientationFlags orient)
  {
    typename itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter =
      itk::OrientImageFilter<ImageType,ImageType>::New();

    orienter->SetDesiredCoordinateOrientation(orient);
    orienter->UseImageDirectionOn();
    orienter->SetInput(inputImage);
    orienter->Update();
    typename ImageType::Pointer returnval =
      orienter->GetOutput();
    return returnval;
  }

  template <class ImageType>
  typename ImageType::Pointer
  OrientImage(typename ImageType::ConstPointer &inputImage,
              const typename ImageType::DirectionType &dirCosines)
  {
    return OrientImage<ImageType>
      (inputImage,
       itk::SpatialOrientationAdapter().FromDirectionCosines(dirCosines));
  }

  template <class ImageType>
  typename ImageType::Pointer
  ReadImageAndOrient(const std::string &filename,
                     itk::SpatialOrientation::ValidCoordinateOrientationFlags orient,
                     const bool zeroOrigin = false )
  {
    typename ImageType::Pointer img =
      ReadImage<ImageType>(filename, zeroOrigin);
    typename ImageType::ConstPointer constImg(img);
    typename ImageType::Pointer  image = itkUtil::OrientImage<ImageType>(constImg,orient);
    if(zeroOrigin)
      {
      double origin[ImageType::ImageDimension];
      for(unsigned int i =0 ; i< ImageType::ImageDimension ; i++)
        {
        origin[i]=0;
        }
      image->SetOrigin(origin);
      }
    return image;
  }


  template <class ImageType>
  typename ImageType::Pointer
  ReadImageAndOrient(const std::string &filename,
                     const typename itk::SpatialOrientationAdapter::DirectionType &dir,
                     const bool zeroOrigin = false)
  {
    return ReadImageAndOrient<ImageType>
      (filename,itk::SpatialOrientationAdapter().FromDirectionCosines(dir),zeroOrigin);
  }


  template <typename  TReadImageType>
  typename TReadImageType::Pointer ReadImageCoronal(const std::string &fileName,
                                                    const bool zeroOrigin = false)
  {
    typename itk::SpatialOrientationAdapter::DirectionType CORdir=
      itk::SpatialOrientationAdapter().ToDirectionCosines(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    return ReadImageAndOrient<TReadImageType>(fileName,CORdir,zeroOrigin);
  }

  template <class ImageType>
  void
  WriteImage(typename ImageType::Pointer &image ,
             const std::string &filename)
  {

    typedef itk::ImageFileWriter< ImageType > WriterType;
    typename  WriterType::Pointer writer = WriterType::New();

    writer->SetFileName(filename.c_str());

    writer->SetInput(image);

    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &err) {
    std::cout << "Exception Object caught: " << std::endl;
    std::cout << err << std::endl;
    throw;
    }
  }
  template <class ImageType>
  void
  WriteImageOriented(typename ImageType::Pointer &image ,
                     const std::string &filename,
                     itk::SpatialOrientation::ValidCoordinateOrientationFlags orient)
  {
    
    typename ImageType::ConstPointer constImg(image);
    typename ImageType::Pointer orientedImage =
      itkUtil::OrientImage<ImageType>(constImg,orient);
   itkUtil::WriteImage<ImageType>(orientedImage,filename);
  }

  template <class InputImageType,class OutputImageType>
  typename OutputImageType::Pointer
  TypeCast(const typename InputImageType::Pointer &input)
  {

    typedef itk::CastImageFilter< InputImageType, OutputImageType> CastToRealFilterType;
    typename CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();

    toReal->SetInput(input);

    toReal->Update();

    return toReal->GetOutput();

  }

  template <class OutputImageType,class InputImageType>
  void
  WriteCastImage(const typename InputImageType::Pointer &input,
                 const std::string &filename )
  {
    typename OutputImageType::Pointer out =
      TypeCast<InputImageType,OutputImageType>(input);
    WriteImage<OutputImageType>(out,filename);
  }

  template <class ImageType>
  typename ImageType::Pointer
  CopyImage(const typename ImageType::Pointer &input )
  {
    typedef itk::ImageDuplicator<ImageType> ImageDupeType;
    typename ImageDupeType::Pointer MyDuplicator = ImageDupeType::New();
    MyDuplicator->SetInputImage(input);
    MyDuplicator->Update();
    return MyDuplicator->GetOutput();
  }

  /** Common code for allocating an image, allowing the region and spacing to be
   * explicitly set.
   */
  template <class TemplateImageType, class OutputImageType>
  typename OutputImageType::Pointer
  AllocateImageFromRegionAndSpacing(const typename TemplateImageType::RegionType &region,
                                    const typename TemplateImageType::SpacingType &spacing)
  {
    typename OutputImageType::Pointer rval;
    rval = OutputImageType::New();
    rval->SetSpacing(spacing);
    //    rval->SetLargestPossibleRegion(region);
    //    rval->SetBufferedRegion(region);
    rval->SetRegions(region);
    rval->Allocate();
    return rval;
  }

  template <class ImageType>
  typename ImageType::Pointer
  AllocateImageFromRegionAndSpacing(const typename ImageType::RegionType &region,
                                    const typename ImageType::SpacingType &spacing)
  {
    return AllocateImageFromRegionAndSpacing<ImageType,ImageType>
      (region,spacing);
  }
}

#endif


