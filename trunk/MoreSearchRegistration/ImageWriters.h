#ifndef __ImageWriters_h
#define __ImageWriters_h

#include <itkImageFileWriter.h>
#include <itkResampleImageFilter.h>

template< typename T >
void writeimage(const T* image,
                const std::string & filename)
{
    // Write out test image
  typename itk::ImageFileWriter<T>::Pointer writer = 
    itk::ImageFileWriter<T>::New();
  writer->SetFileName(filename);
  writer->SetInput(image);
  writer->Update();
}

template< typename T >
void writeimage(itk::SmartPointer<T> image,
                const std::string & filename)
{
  writeimage(image.GetPointer(), filename);
}

template< typename T, typename J>
void writeimage(const T* image,
                const J* transform, 
                const std::string & filename)
{
  typedef itk::ResampleImageFilter<T, T> ResampleImageFilter;
  typename ResampleImageFilter::Pointer resampler = 
    ResampleImageFilter::New();
  // Resample the moving image
  resampler->SetInput(image);
  resampler->SetOutputParametersFromImage(image);
  resampler->SetTransform(transform);
  resampler->Update();

  // Write out test image
  typename itk::ImageFileWriter<T>::Pointer writer = 
    itk::ImageFileWriter<T>::New();
  writer->SetFileName(filename);
  writer->SetInput(resampler->GetOutput());
  writer->Update();
}

template< typename T, typename J>
void writeimage(const T* image,
                itk::SmartPointer<J> transform, 
                const std::string & filename)
{
  writeimage(image, transform.GetPointer(), filename);
}

#endif
