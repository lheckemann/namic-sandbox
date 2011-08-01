#ifndef utilities_hxx_
#define utilities_hxx_

// itk
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkCastImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkMultiplyByConstantImageFilter.h"


#include <csignal>

namespace laSeg
{
  /**
   * readImage
   */
  template< typename itkImage_t >
  typename itkImage_t::Pointer readImage(const char *fileName)
  {
    typedef itk::ImageFileReader< itkImage_t > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename itkImage_t::Pointer image;
    
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


  /**
   * writeImage
   */
  template< typename itkImage_t > void writeImage(typename itkImage_t::Pointer img, const char *fileName)
  {
    typedef itk::ImageFileWriter< itkImage_t > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(img);
    writer->UseCompressionOn();

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


  /**
   * Read a series of images.
   */
  template< typename inputPixel_t, typename outputPixel_t > 
  typename itk::Image<outputPixel_t, 3>::Pointer
  castItkImage( typename itk::Image<inputPixel_t, 3>::Pointer inputImage )
  {
    typedef itk::Image<inputPixel_t, 3> inputImage_t;
    typedef itk::Image<outputPixel_t, 3> outputImage_t;

    typedef itk::CastImageFilter< inputImage_t, outputImage_t > itkCastFilter_t;

    typename itkCastFilter_t::Pointer caster = itkCastFilter_t::New();
    caster->SetInput( inputImage );
    caster->Update();


    return caster->GetOutput();
  }


  /**
   * Read a series of images.
   */
  template< typename itkImage_t > 
  std::vector< typename itkImage_t::Pointer >
  readImageSeries( const std::vector< std::string >& imageNameList )
  {
    typedef typename itkImage_t::Pointer itkImagePointer_t;
    typedef std::vector< itkImagePointer_t > itkImageList_t;
    typedef itk::ImageFileReader< itkImage_t > itkImageReader_t;


    itkImageList_t imageSeries;
    
    int n = imageNameList.size();
    for (int i = 0; i < n; ++i)
      {
        std::string thisName = imageNameList[i];

        typename itkImageReader_t::Pointer reader = itkImageReader_t::New();
        reader->SetFileName(thisName);

        itkImagePointer_t img;

        try
          {
            reader->Update();
            img = reader->GetOutput();
          }
        catch ( itk::ExceptionObject &err)
          {
            std::cerr<< "ExceptionObject caught !" << std::endl; 
            std::cerr<< err << std::endl; 
            raise(SIGABRT);
          }


        imageSeries.push_back(img);
      }

    return imageSeries;
  }

//   /**
//    * From a multi-label map, extra a hippocampus
//    */
//   template<typename itkImage_t>
//   void extractHippocampusLabel(typename itkImage_t::Pointer im)
//   {
//     typedef itk::ImageRegionIterator<itkImage_t> itkImageRegionIterator_t;
//     typedef typename itkImage_t::PixelType itkImagePixel_t;

//     itkImageRegionIterator_t it(im, im->GetLargestPossibleRegion());
//     for (it.GoToBegin(); !it.IsAtEnd(); ++it)
//       {
//         itkImagePixel_t v = it.Get();

//         if (v == 7 || v == 8)
//           {
//             it.Set(1);
//           }
//         else
//           {
//             it.Set(0);
//           }
//       }

//     return;
//   }


//   /**
//    * In the list of label maps, extract hippocampus
//    */
//   template<typename itkImage_t>
//   void extractHippocampusLabelAll(std::vector< typename itkImage_t::Pointer > imList)
//   {
//     int n = imList.size();
//     for (int i = 0; i < n; ++i)
//       {
//         extractHippocampusLabel<itkImage_t>(imList[i]);
//       }

//     return;
//   }



  /*============================================================
   * readTextLineToListOfString   
   */
  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName)
  {
    /* The file MUST end with an empty line, then each line will be
       stored as an element in the returned vector object. */


    // Here speed and space is not a big issue, so just let it copy and return stuff.
    std::vector< std::string > listOfStrings;

    std::ifstream f(textFileName);
    std::string thisLine;

    if (f.good())
      {
        while( std::getline(f, thisLine) )
          {
            listOfStrings.push_back(thisLine);
          }
      }
    else
      {
        std::cerr<<"Error: can not open file:"<<textFileName<<std::endl;
        raise(SIGABRT);
      }

    f.close();

    return listOfStrings;
  }


//   template< typename pixel_t, unsigned int dim > 
//   typename itk::Image< pixel_t, dim >::Pointer 
//   readImage(const char *fileName)
//   {
//     typedef itk::Image< pixel_t, dim > ImageType;
//     typedef itk::ImageFileReader< ImageType > ImageReaderType;
//     typename ImageReaderType::Pointer reader = ImageReaderType::New();
//     reader->SetFileName(fileName);

//     typename ImageType::Pointer image;
    
//     try
//       {
//         reader->Update();
//         image = reader->GetOutput();
//       }
//     catch ( itk::ExceptionObject &err)
//       {
//         std::cerr<< "ExceptionObject caught !" << std::endl; 
//         std::cerr<< err << std::endl; 
//         raise(SIGABRT);
//       }

//     return image;
//   }



//   template< typename pixel_t, unsigned int dim > 
//   void 
//   writeImage(typename itk::Image< pixel_t, dim >::Pointer img, const char *fileName)
//   {
//     typedef itk::Image< pixel_t, dim > ImageType;
//     typedef itk::ImageFileWriter< ImageType > WriterType;

//     typename WriterType::Pointer writer = WriterType::New();
//     writer->SetFileName( fileName );
//     writer->SetInput(img);
//     writer->UseCompressionOn();

//     try
//       {
//         writer->Update();
//       }
//     catch ( itk::ExceptionObject &err)
//       {
//         std::cout << "ExceptionObject caught !" << std::endl; 
//         std::cout << err << std::endl; 
//         raise(SIGABRT);   
//       }
//   }


  template<typename image_t>
  double getVol(typename image_t::Pointer img, typename image_t::PixelType thld = 0)
  {
    typedef itk::ImageRegionConstIterator<image_t> ImageRegionConstIterator_t;
    ImageRegionConstIterator_t it(img, img->GetLargestPossibleRegion() );

    double cell = (img->GetSpacing()[0])*(img->GetSpacing()[1])*(img->GetSpacing()[2]);

    double v = 0.0;

    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
        typename image_t::PixelType f = it.Get();

        v += (f>thld?cell:0.0);
      }

    return v;
  }



  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  thld3(typename input_image_t::Pointer input,                            \
        typename input_image_t::PixelType lowerT,                         \
        typename input_image_t::PixelType upperT, \
        typename output_image_t::PixelType insideValue = 1,               \
        typename output_image_t::PixelType outsideValue = 0)
  {
    /**
     * O(x) :=    I(x) \in [lowerT, upperT] ? insideValue : outsideValue
     */

    //tst
    //   std::cout<<lowerT<<std::endl;
    //   std::cout<<upperT<<std::endl;
    //tst//

    typedef itk::BinaryThresholdImageFilter<input_image_t, output_image_t> binaryThresholdImageFilter_t;

    typename binaryThresholdImageFilter_t::Pointer thlder = binaryThresholdImageFilter_t::New();
    thlder->SetInput(input);
    thlder->SetInsideValue(insideValue);
    thlder->SetOutsideValue(outsideValue);
    thlder->SetUpperThreshold(upperT);
    thlder->SetLowerThreshold(lowerT);
    thlder->Update();
  
    return thlder->GetOutput();
  }



  /**
   * Post process probabiliry map: Multiply by 200, then convert to
   * uchar image. This will make the final result to be easier
   * thresholded by Slicer.
   */
  template<typename image_t>
  typename itk::Image<unsigned char, 3>::Pointer
  postProcessProbabilityMap(typename image_t::Pointer probMap, typename image_t::PixelType c)
  {
    typedef itk::MultiplyByConstantImageFilter< image_t, typename image_t::PixelType, itk::Image<unsigned char, 3> > \
      itkMultiplyByConstantImageFilter_t;

    typename itkMultiplyByConstantImageFilter_t::Pointer mul = itkMultiplyByConstantImageFilter_t::New();
    mul->SetInput(probMap);
    mul->SetConstant(c);
    mul->Update();
    
    return mul->GetOutput();
  }


}// douher

#endif
