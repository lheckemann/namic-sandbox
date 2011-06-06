/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: Resample.cxx,v $
  Language:  C++
  Date:      $Date: 2010-03-31 10:56:05 -0400 (Wed, 31 Mar 2010) $
  Version:   $Revision: 12562 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// Resample a series
//   Usage: ResampleVolume --spacing xSpacing,ySpacing,zSpacing
//                         InputDirectory OutputDirectory
//                        
//
//   Example: ResampleVolume --spacing 0,0,1.5 CT.mhd CTResample.mhd
//            will read a volume CT.mhd and create a
//            new volume called CTResample.mhd directory. The new series
//            will have the same x,y spacing as the input series, but
//            will have a z-spacing of 1.5.
//
// Description:
// ResampleVolume resamples a volume with user-specified
// spacing. The number of slices in
// the output series may be larger or smaller due to changes in the
// z-spacing. To retain the spacing for a given dimension, specify 0.
//
// The program progresses as follows:
// 1) Read the input series
// 2) Resample the series according to the user specified x-y-z
//    spacing.
// 3) Write the new series
//

#include "itkVersion.h"

#include "itkPluginUtilities.h"
#include "itkOrientedImage.h"
#include "itkMinimumMaximumImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkPasteImageFilter.h"
#include "itkImageDuplicator.h"

#include "itkImageRegionConstIterator.h"
#include "itkAbsImageFilter.h"

#include <string>
#include <sstream>
#include "ShapeBasedInterpolationCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace {

template<class T> int DoIt( int argc, char * argv[], T )
{
  PARSE_ARGS;
  const unsigned int InputDimension = 3;
  const unsigned int OutputDimension = 3;

  typedef T PixelType;

  typedef itk::OrientedImage< PixelType, InputDimension >
    InputImageType;
  typedef itk::OrientedImage< PixelType, OutputDimension >
    OutputImageType;
  typedef itk::OrientedImage< float, OutputDimension >
    InternalImageType;
  typedef itk::ImageFileReader< InputImageType >
    ReaderType;
  typedef itk::CastImageFilter<InputImageType,InternalImageType> InputCastType;
  typedef itk::CastImageFilter<InternalImageType,OutputImageType> OutputCastType;

  typedef itk::ExtractImageFilter<InternalImageType,InternalImageType> ExtractorType;
  typedef itk::PasteImageFilter<InternalImageType,InternalImageType> PasterType;

  typedef itk::ImageDuplicator<InternalImageType> DupType;

  typedef itk::SignedMaurerDistanceMapImageFilter<InternalImageType,InternalImageType>    
    MaurerType;
  typedef itk::DanielssonDistanceMapImageFilter<InternalImageType,InternalImageType>
    DanielssonType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType,InternalImageType>
    ThresholdType;
  typedef itk::BinaryThresholdImageFilter<InputImageType,InputImageType>
    InputThresholdType;

  typedef itk::IdentityTransform< double, InputDimension >
    TransformType;
  typedef itk::LinearInterpolateImageFunction< InternalImageType, double >
    LinearInterpolatorType;
  typedef itk::NearestNeighborInterpolateImageFunction< InternalImageType, double >
    NearestNeighborInterpolatorType;
  typedef itk::BSplineInterpolateImageFunction< InternalImageType, double >
    BSplineInterpolatorType;

  typedef itk::AbsImageFilter<InternalImageType,InternalImageType>
    AbsType;

  typedef itk::ImageRegionConstIterator<InternalImageType> ConstIterType;
  typedef itk::ImageRegionIterator<InternalImageType> IterType;

#define RADIUS 3

  typedef itk::WindowedSincInterpolateImageFunction<InternalImageType, RADIUS, itk::Function::HammingWindowFunction<RADIUS> > HammingInterpolatorType;
  typedef itk::WindowedSincInterpolateImageFunction<InternalImageType, RADIUS, itk::Function::CosineWindowFunction<RADIUS> > CosineInterpolatorType;
  typedef itk::WindowedSincInterpolateImageFunction<InternalImageType, RADIUS, itk::Function::WelchWindowFunction<RADIUS> > WelchInterpolatorType;
  typedef itk::WindowedSincInterpolateImageFunction<InternalImageType, RADIUS, itk::Function::LanczosWindowFunction<RADIUS> > LanczosInterpolatorType;
  typedef itk::WindowedSincInterpolateImageFunction<InternalImageType, RADIUS, itk::Function::BlackmanWindowFunction<RADIUS> > BlackmanInterpolatorType;

  typedef itk::ResampleImageFilter< InternalImageType, InternalImageType >
    ResampleFilterType;
  typedef itk::ImageFileWriter< OutputImageType >
    FileWriterType;
  typedef itk::ImageFileWriter< InternalImageType > InternalWriterType;

////////////////////////////////////////////////  
// 1) Read the input series

  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( InputVolume.c_str() );

  typename InputThresholdType::Pointer inputThresh = InputThresholdType::New();
  inputThresh->SetInput(reader->GetOutput());
  if(inputLabelID == -1){
    inputThresh->SetUpperThreshold(255);
    inputThresh->SetLowerThreshold(1);
    inputThresh->SetInsideValue(1);
  } else {
    inputThresh->SetUpperThreshold(inputLabelID);
    inputThresh->SetLowerThreshold(inputLabelID);
    inputThresh->SetInsideValue(inputLabelID);
  }

  typename InputCastType::Pointer inputCaster = InputCastType::New();
  inputCaster->SetInput(inputThresh->GetOutput());
  inputCaster->Update();

  const typename InputImageType::SpacingType& inputSpacing =
    inputCaster->GetOutput()->GetSpacing();
  const typename InputImageType::RegionType& inputRegion =
    inputCaster->GetOutput()->GetBufferedRegion();
  const typename InputImageType::SizeType& inputSize =
    inputRegion.GetSize();

  typename InputImageType::SpacingType outputSpacing;
  outputSpacing[0] = inputSpacing[0];
  outputSpacing[1] = inputSpacing[1];
  outputSpacing[2] = inputSpacing[2];

  unsigned int maxSpacingIdx = 0;


  for (unsigned int i = 0; i < 3; i++)
    if(inputSpacing[i] > inputSpacing[maxSpacingIdx])
      maxSpacingIdx = i;
  outputSpacing[maxSpacingIdx] *= sliceThicknessReductionConst;

  typename InputImageType::SizeType   outputSize;
  typedef typename InputImageType::SizeType::SizeValueType SizeValueType;
  outputSize[0] = static_cast<SizeValueType>(inputSize[0] * inputSpacing[0] / outputSpacing[0] + .5);
  outputSize[1] = static_cast<SizeValueType>(inputSize[1] * inputSpacing[1] / outputSpacing[1] + .5);
  outputSize[2] = static_cast<SizeValueType>(inputSize[2] * inputSpacing[2] / outputSpacing[2] + .5);



  typename InternalImageType::Pointer fInputImage = inputCaster->GetOutput();
  fInputImage->DisconnectPipeline();
  typename InternalImageType::RegionType extrRegion;
  typename InternalImageType::IndexType extrIdx;
  typename InternalImageType::SizeType extrSize = fInputImage->GetBufferedRegion().GetSize();
  typename DupType::Pointer dup2 = DupType::New();
  dup2->SetInputImage(fInputImage);
  dup2->Update();

  typename InternalImageType::Pointer pasteImage = dup2->GetOutput();
  pasteImage->FillBuffer(0);

  extrIdx.Fill(0);
  extrSize[maxSpacingIdx] = 1;
  std::vector<bool> blankSlice;
  int firstSlice = -1, lastSlice = -1;
  for(unsigned int i=0;i<inputSize[maxSpacingIdx];i++){
    typename ExtractorType::Pointer extractor = ExtractorType::New();
    extrIdx[maxSpacingIdx] = i;
    extrRegion.SetIndex(extrIdx);
    extrRegion.SetSize(extrSize);
    extractor->SetExtractionRegion(extrRegion);
    
    extractor->SetInput(fInputImage);
    extractor->Update();

    typename InternalImageType::Pointer currentSlice = extractor->GetOutput();
    typename InternalImageType::Pointer processedSlice = NULL;
    currentSlice->DisconnectPipeline();

    ConstIterType it(currentSlice, currentSlice->GetBufferedRegion());
    for(it.GoToBegin();!it.IsAtEnd();++it)
      if(it.Get())
        break;

    if(!it.IsAtEnd() && firstSlice == -1)
      firstSlice = i;
    if(it.IsAtEnd() && firstSlice != -1 && lastSlice == -1){
      lastSlice = i-1;
      break;
    }
  }

  InternalImageType::Pointer tmpImage = InternalImageType::New();
  InternalImageType::SizeType tmpSize = fInputImage->GetBufferedRegion().GetSize();
  InternalImageType::RegionType tmpRegion;
  InternalImageType::IndexType tmpIndex;
  InternalImageType::PointType tmpOrigin;
  tmpSize[maxSpacingIdx] = lastSlice-firstSlice+1;
  tmpIndex.Fill(0);
  tmpRegion.SetIndex(tmpIndex);
  tmpRegion.SetSize(tmpSize);
  tmpImage->SetRegions(tmpRegion);
  tmpImage->Allocate();
  tmpImage->FillBuffer(0);
  tmpImage->SetSpacing(inputSpacing);
  tmpImage->SetOrigin(fInputImage->GetOrigin());
  tmpImage->SetDirection(fInputImage->GetDirection());
  
  outputSize[0] = static_cast<SizeValueType>(tmpSize[0] * inputSpacing[0] / outputSpacing[0] + .5);
  outputSize[1] = static_cast<SizeValueType>(tmpSize[1] * inputSpacing[1] / outputSpacing[1] + .5);
  outputSize[2] = static_cast<SizeValueType>(tmpSize[2] * inputSpacing[2] / outputSpacing[2] + .5);

  tmpIndex[maxSpacingIdx] = firstSlice;
  fInputImage->TransformIndexToPhysicalPoint(tmpIndex, tmpOrigin);
  tmpImage->SetOrigin(tmpOrigin);


  unsigned int j = 0;
  for(unsigned int i=(unsigned int)firstSlice;i<=(unsigned int)lastSlice;i++,j++){
    typename ExtractorType::Pointer extractor = ExtractorType::New();
    extrIdx[maxSpacingIdx] = i;
    extrRegion.SetIndex(extrIdx);
    extrRegion.SetSize(extrSize);
    extractor->SetExtractionRegion(extrRegion);
    
    extractor->SetInput(fInputImage);
    extractor->Update();

    typename InternalImageType::Pointer currentSlice = extractor->GetOutput();
    typename InternalImageType::Pointer processedSlice = NULL;
    currentSlice->DisconnectPipeline();

    typename MaurerType::Pointer dt = MaurerType::New();
    dt->SetInput(extractor->GetOutput());
    dt->SetSquaredDistance(0);
    dt->SetUseImageSpacing(1);
    dt->Update();
    processedSlice = dt->GetOutput();

    typename PasterType::Pointer paster = PasterType::New();
    paster->SetInput(tmpImage);
    extrIdx[maxSpacingIdx] = j;
    paster->SetDestinationIndex(extrIdx);
    paster->SetDestinationImage(tmpImage);
    paster->SetSourceRegion(extrRegion);
    paster->SetSourceImage(processedSlice);
    paster->Update();
    tmpImage = paster->GetOutput();
    tmpImage->DisconnectPipeline();
  }

  pasteImage = tmpImage;

//   {
//    typename InternalWriterType::Pointer fw = InternalWriterType::New();
//    fw->SetInput(pasteImage);
//    fw->SetFileName("paste_image.nrrd");
//    fw->Update();
//    }

  

////////////////////////////////////////////////  
// 2) Resample the series
  typename LinearInterpolatorType::Pointer linearInterpolator = LinearInterpolatorType::New();
  typename NearestNeighborInterpolatorType::Pointer nearestNeighborInterpolator = NearestNeighborInterpolatorType::New();
  typename BSplineInterpolatorType::Pointer bsplineInterpolator = BSplineInterpolatorType::New();
  typename HammingInterpolatorType::Pointer hammingInterpolator = HammingInterpolatorType::New();
  typename CosineInterpolatorType::Pointer cosineInterpolator = CosineInterpolatorType::New();
  typename WelchInterpolatorType::Pointer welchInterpolator = WelchInterpolatorType::New();
  typename LanczosInterpolatorType::Pointer lanczosInterpolator = LanczosInterpolatorType::New();
  typename BlackmanInterpolatorType::Pointer blackmanInterpolator = BlackmanInterpolatorType::New();

  typename TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

//    typename InternalWriterType::Pointer sw1 = InternalWriterType::New();
//    sw1->SetInput(pasteImage);
//    sw1->SetFileName("resampler_input.nrrd");
//    sw1->Update();


  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  itk::PluginFilterWatcher watcher(resampler, "Resample Volume",
    CLPProcessInformation);

    resampler->SetInput( pasteImage );
    resampler->SetTransform( transform );
    resampler->SetDefaultPixelValue(inputSpacing[maxSpacingIdx]*100.);
    if (interpolationType == "linear")
      {
      resampler->SetInterpolator( linearInterpolator );
      }
    else if (interpolationType == "nearestNeighbor")
      {
      resampler->SetInterpolator( nearestNeighborInterpolator );
      }
    else if (interpolationType == "bspline")
      {
      resampler->SetInterpolator( bsplineInterpolator );
      }
    else if (interpolationType == "hamming")
      {
      resampler->SetInterpolator( hammingInterpolator );
      }
    else if (interpolationType == "cosine")
      {
      resampler->SetInterpolator( cosineInterpolator );
      }
    else if (interpolationType == "welch")
      {
      resampler->SetInterpolator( welchInterpolator );
      }
    else if (interpolationType == "lanczos")
      {
      resampler->SetInterpolator( lanczosInterpolator );
      }
    else if (interpolationType == "blackman")
      {
      resampler->SetInterpolator( blackmanInterpolator );
      }
    else
      {
      resampler->SetInterpolator( linearInterpolator );
      }

    resampler->SetOutputOrigin ( pasteImage->GetOrigin());
    resampler->SetOutputSpacing ( outputSpacing );
    resampler->SetOutputDirection ( pasteImage->GetDirection());
    
    resampler->SetSize ( outputSize );
    try{
      resampler->Update ();
    } catch (itk::ExceptionObject &e){
      std::cout << "Exception in resampler: " << e << std::endl;
      abort();
    }

//    typename InternalWriterType::Pointer sw = InternalWriterType::New();
//    sw->SetInput(resampler->GetOutput());
//    sw->SetFileName("dt.nrrd");
//    sw->Update();

    typename ThresholdType::Pointer thresh = ThresholdType::New();
    thresh->SetInput(resampler->GetOutput());
    thresh->SetUpperThreshold(0);
    thresh->SetLowerThreshold(-1e+3);
    if(inputLabelID)
      thresh->SetInsideValue(inputLabelID);
    else
      thresh->SetInsideValue(1);
    try{
      thresh->Update();
    } catch (itk::ExceptionObject &e){
      std::cout << "Threshold failed: " << e << std::endl;
      abort();
    }
    
   
    typename OutputCastType::Pointer outputCaster = OutputCastType::New();
    outputCaster->SetInput(thresh->GetOutput());
   

  typename FileWriterType::Pointer seriesWriter = FileWriterType::New();
    seriesWriter->SetInput( outputCaster->GetOutput() );
    seriesWriter->SetFileName( OutputVolume.c_str() );
    seriesWriter->SetUseCompression(1);
  try
    {
    seriesWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while writing the series " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << std::endl;

  return EXIT_SUCCESS;
}

} // end of anonymous namespace


int main( int argc, char * argv[] )
{
  
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType (InputVolume, pixelType, componentType);

    // This filter handles all types
    
    switch (componentType)
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt( argc, argv, static_cast<unsigned char>(0));
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<char>(0));
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt( argc, argv, static_cast<unsigned short>(0));
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0));
        break;
      case itk::ImageIOBase::UINT:
        return DoIt( argc, argv, static_cast<unsigned int>(0));
        break;
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0));
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt( argc, argv, static_cast<unsigned long>(0));
        break;
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0));
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0));
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0));
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
