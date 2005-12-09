/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: IntensityHistogram3D.cxx,v $
Language:  C++
Date:      $Date: 2002/08/07 16:56:42 $
Version:   $Revision: 1.12 $

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include <itkImage.h>
#include <itkImageFileReader.h> 
#include <itkCastImageFilter.h>
#include <itkHistogram.h>
#include <itkImageRegionIterator.h>
#include <itkThresholdImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h> 

#include <itkCurvatureFlowImageFilter.h> 
#include <itkDiscreteGaussianImageFilter.h> 
#include <itkBinaryBallStructuringElement.h> 
#include <itkBinaryCrossStructuringElement.h>
#include <itkBinaryDilateImageFilter.h> 
#include <itkBinaryErodeImageFilter.h> 
#include <itkBinaryThresholdImageFilter.h>

#include <itkMinimumMaximumImageCalculator.h> 
#include <itkBSplineInterpolateImageFunction.h> 
#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

#include <itkAntiAliasBinaryImageFilter.h>

#include <itkImageFileWriter.h>
#include <itkMetaImageIO.h>


#include "argio.hh"
#include "ccl.h"

using namespace itk;
using namespace std;

static int debug = 1;
static void draw_fill_inside_image(unsigned char *image, int *dim, int new_label);
static int NoDiagConnect (unsigned char *image, int *dim);
static void clear_edge(unsigned char *image, int *dims, int clear_label);

int main(int argc, const char* argv[])
{


  if (argc <= 1 || ipExistsArgument(argv,"-h") || ipExistsArgument(argv,"-help") || 
      ipExistsArgument(argv,"-usage"))
    {
      cout << "usage: " << argv[0] << " --- v 2.0" << endl
     << "       infile [options]" << endl << endl 
     << " -o        Outputfile" << endl
     << " -Gauss    Do Gaussian filtering " << endl
     << " -var        variance of Gauss filter in all 3 dimensions" << endl
     << " -NoLS     Don't do LevelSet based smoothing (guaranteed to be within a single voxel)" << endl
     << " -RMS <float>    target RMS error for LS smoothing" << endl
     << " -iter <float>   number of Iterations for LS smoothing" << endl
     << " -label val  First extract this label before processing" << endl
     << " -isotropic  Scale first to isotropic pixel dimensions" << endl
     << " -space sx,sy,sz  Enforced spacing in x,y and z direction before any processing" << endl
     << " -asymClose  perform an asymmetric closing operations (dilation with block, erosion with star structuring element)" << endl
     << " -noCCL      do not perform a connected component labeling and threshold for the largest part" << endl
     << " -v          verbose mode" << endl;
      exit(0) ;
    }

  string fileName(argv[1]);

  string outfileName(ipGetStringArgument(argv, "-o", ""));

  if (outfileName.empty()) {
    outfileName = "output.mha";
    cout << "no outputname specified using " << outfileName << endl;
  }

  const int Dimension = 3;
  const int NO_LABEL = -1;

  bool gaussianOn  = ipExistsArgument(argv,"-Gauss"); 
  float variance   = ipGetFloatArgument(argv, "-var", 1.5);

  bool LSSmoothOn  = ! ipExistsArgument(argv,"-NoLS"); 
  double maximumRMSError = ipGetDoubleArgument(argv,"-rms",0.01);
  unsigned int numberOfIterations = ipGetIntArgument(argv,"-iter",50);

  bool labelOn     = ipExistsArgument(argv,"-label"); 
  int label        = ipGetIntArgument(argv,"-label",NO_LABEL); 

  debug            = ipExistsArgument(argv,"-v"); 
  bool isotropicOn = ipExistsArgument(argv,"-isotropic"); 
  bool asymCloseOn = ipExistsArgument(argv,"-asymClose"); 
  bool CCLOn = !ipExistsArgument(argv,"-noCCL"); 
  bool scaleOn = false;
  double  spacing[Dimension];
  if (ipExistsArgument(argv,"-space")) {  
    char *tmp_str    = ipGetStringArgument(argv, "-space", NULL);
    int numDim       = ipExtractDoubleTokens(spacing, tmp_str, Dimension);
    if (numDim != Dimension) {              // odd number of input tokens
      cerr << argv[0] << ": numbers of spacing dimensions is not "<< Dimension << ".\n";
      exit(-1);
    }
    free(tmp_str);
    if (isotropicOn) {
      isotropicOn = false;
      cerr << "-space option overrides -isotropic option" << endl;
    }
    scaleOn = true;
  } 

  // types used in this routine
  typedef double  CoordRepType;
  typedef unsigned char ImagePixelType;
  typedef Image<ImagePixelType,Dimension>  ImageType;
  typedef float SmoothImagePixelType;
  typedef ImageType::SizeType ImageSizeType;
  typedef Image<SmoothImagePixelType,Dimension> SmoothImageType;
  typedef ImageFileReader<ImageType> VolumeReaderType;
  typedef ImageFileWriter<ImageType> VolumeWriterType;
  typedef ResampleImageFilter<ImageType , ImageType > ResamplerType;
  typedef BSplineInterpolateImageFunction<ImageType, CoordRepType> InterpolFunctionType;
  typedef AffineTransform<CoordRepType, Dimension> TransformType;
  typedef LinearInterpolateImageFunction<ImageType, CoordRepType> LinearInterpolFunctionType;
  typedef NearestNeighborInterpolateImageFunction<ImageType, CoordRepType> NNInterpolFunctionType;


  typedef ThresholdImageFilter< ImageType > threshFilterType;
  typedef BinaryThresholdImageFilter< ImageType, ImageType > binThreshFilterType;
  typedef BinaryBallStructuringElement<ImagePixelType,Dimension> StructuringElementType;
  typedef BinaryCrossStructuringElement<ImagePixelType,Dimension> AsymStructuringElementType;

  typedef BinaryDilateImageFilter<ImageType, ImageType, StructuringElementType> dilateFilterType;
  typedef BinaryErodeImageFilter<ImageType, ImageType, StructuringElementType> erodeFilterType;
  typedef BinaryErodeImageFilter<ImageType, ImageType, AsymStructuringElementType> erodeAsymFilterType;

  typedef CastImageFilter<ImageType,SmoothImageType> castInputFilterType;
  typedef CastImageFilter<SmoothImageType,ImageType> castOutputFilterType;
  typedef DiscreteGaussianImageFilter<SmoothImageType, SmoothImageType> gaussFilterType;

  typedef CastImageFilter<ImageType ,SmoothImageType > CastToRealFilterType;
  typedef AntiAliasBinaryImageFilter<SmoothImageType, SmoothImageType> AntiAliasFilterType;
  typedef RescaleIntensityImageFilter<SmoothImageType, ImageType > RescaleFilter;

  ImageType::Pointer image ;

  if (debug) cout << "Loading file " << endl;
  try
    {
      VolumeReaderType::Pointer imageReader = VolumeReaderType::New();
      imageReader->SetFileName(fileName.c_str()) ;
      imageReader->Update() ;
      image = imageReader->GetOutput() ;
    }
  catch (ExceptionObject e)
    {
      e.Print(cout) ;
      exit(0) ;
    }
  
  int dim[Dimension];

  // HERE the processing starts
  
  // prepare for my own routines
  ImageType::RegionType imageRegion = image->GetBufferedRegion();
  dim[0] = imageRegion.GetSize(0);
  dim[1] = imageRegion.GetSize(1);
  dim[2] = imageRegion.GetSize(2);
  ImageType::IndexType nullIndex;
  nullIndex[0] = 0;
  nullIndex[1] = 0;
  nullIndex[2] = 0;
  if (debug) cout << "size of the image " << dim[0] << "," << dim[1] << "," << dim[2]<< endl; 

  const int elemSize = 1;
  const int LABEL_VAL = 255;
  ImageType::Pointer procImage ;

  if (labelOn) {
    // Thresholding at label -> set to 0 otherwise
    if (debug) cout << "extracting object " << label << endl; 
    
    threshFilterType::Pointer threshFilter = threshFilterType::New();
    threshFilter->SetInput(image);
    threshFilter->ThresholdOutside(label, label);
    threshFilter->SetOutsideValue (0);
    threshFilter->Update();
    image = threshFilter->GetOutput();
    
  }
  {
    // Set object to 255 -> Tresholding all but label
    if (debug) cout << "set object to " << LABEL_VAL  << endl; 
    threshFilterType::Pointer threshFilter = threshFilterType::New();
    threshFilter->SetInput(image);
    threshFilter->ThresholdAbove(0);
    threshFilter->SetOutsideValue (LABEL_VAL);
    threshFilter->Update();
    procImage = threshFilter->GetOutput();
  }

  // clear edge and fill inside 
  if (debug) cout << "fill inside " << endl; 

  imageRegion = procImage->GetBufferedRegion();
  dim[0] = imageRegion.GetSize(0);
  dim[1] = imageRegion.GetSize(1);
  dim[2] = imageRegion.GetSize(2);
  ImagePixelType *data = &((*procImage)[nullIndex]);
  clear_edge(data, dim, 0);
  draw_fill_inside_image(data, dim, LABEL_VAL);


  // Rescaling the data ?
  if (isotropicOn) {
    const itk::Vector<double,3> inSpacing = procImage->GetSpacing();
    double minSpacing = inSpacing[0];
    for (int i = 1; i < Dimension; i++) if (minSpacing > inSpacing[i]) minSpacing = inSpacing[i];
    for (int i = 0; i < Dimension; i++) spacing[i] = minSpacing;
    scaleOn = true;
  }

  if (scaleOn) {
    if (debug) {
      cout << " resampling (NN interpol) the data to voxelsize ";
      for (int i = 0; i < Dimension; i++) cout << spacing[i] << ", ";
      cout << endl;
    }
    ResamplerType::Pointer resampleFilter = ResamplerType::New();
    TransformType::Pointer transform = TransformType::New();
    //InterpolFunctionType::Pointer interpolFunction = InterpolFunctionType::New();
    //LinearInterpolFunctionType::Pointer interpolFunction = LinearInterpolFunctionType::New();
    NNInterpolFunctionType::Pointer interpolFunction = NNInterpolFunctionType::New();

    transform->SetIdentity();
    //interpolFunction->SetSplineOrder(3);
    interpolFunction->SetInputImage(procImage);
    
    ResamplerType::SizeType size;
    imageRegion = procImage->GetBufferedRegion();
    dim[0] = imageRegion.GetSize(0);
    dim[1] = imageRegion.GetSize(1);
    dim[2] = imageRegion.GetSize(2);
    const itk::Vector<double,3> inSpacing = image->GetSpacing();
    for (int i = 0; i < Dimension; i++) {
      size[i] = (unsigned int) ((double) inSpacing[i] * dim[i] / spacing[i] );
    }
    resampleFilter->SetInput(procImage); 
    resampleFilter->SetSize(size);
    resampleFilter->SetOutputSpacing(spacing);
    resampleFilter->SetOutputOrigin(image->GetOrigin());
    resampleFilter->SetTransform(transform.GetPointer());
    resampleFilter->SetInterpolator(interpolFunction.GetPointer());
    resampleFilter->Update();

    //     if (debug) cout << "Tresholding at 50% " << endl;
    //     binThreshFilterType::Pointer binTreshFilter = binThreshFilterType::New();
    //     binTreshFilter->SetInput(resampleFilter->GetOutput());
    //     binTreshFilter->SetUpperThreshold(LABEL_VAL);
    //     binTreshFilter->SetLowerThreshold(LABEL_VAL/2); 
    //     binTreshFilter->SetOutsideValue (0);
    //     binTreshFilter->SetInsideValue (LABEL_VAL);
    //     binTreshFilter->Update();

    //procImage = binTreshFilter->GetOutput();
    procImage = resampleFilter->GetOutput();

    imageRegion = procImage->GetBufferedRegion();
    dim[0] = imageRegion.GetSize(0);
    dim[1] = imageRegion.GetSize(1);
    dim[2] = imageRegion.GetSize(2);
    if (debug) cout << "size of the resized image " << dim[0] << "," << dim[1] << "," << dim[2]<< endl; 
  }

  // Closing 1
  {
    if (asymCloseOn) {
      if (debug) cout << "Closing 1" << endl;
      StructuringElementType structuringElement;
      AsymStructuringElementType asymStructuringElement;
      
      dilateFilterType::Pointer dilateFilter = dilateFilterType::New();  
      erodeAsymFilterType::Pointer erodeFilter = erodeAsymFilterType::New();  
      
      dilateFilter->SetInput(procImage);
      erodeFilter->SetInput(dilateFilter->GetOutput());
      dilateFilter->SetDilateValue (LABEL_VAL);
      erodeFilter->SetErodeValue (LABEL_VAL);
      
      structuringElement.SetRadius( elemSize );  // 3x3x3 structuring element
      asymStructuringElement.SetRadius( elemSize );
      structuringElement.CreateStructuringElement( );
      asymStructuringElement.CreateStructuringElement( );
      
      dilateFilter->SetKernel( structuringElement );
      erodeFilter->SetKernel( asymStructuringElement );
    
      erodeFilter->Update();
      
      procImage = erodeFilter->GetOutput();
    } else {
      if (debug) cout << "Closing 1" << endl;
      StructuringElementType structuringElement;
      
      dilateFilterType::Pointer dilateFilter = dilateFilterType::New();  
      erodeFilterType::Pointer erodeFilter = erodeFilterType::New();  
      
      dilateFilter->SetInput(procImage);
      erodeFilter->SetInput(dilateFilter->GetOutput());
      dilateFilter->SetDilateValue (LABEL_VAL);
      erodeFilter->SetErodeValue (LABEL_VAL);
      
      structuringElement.SetRadius( elemSize );  // 3x3x3 structuring element
      structuringElement.CreateStructuringElement( );
      
      dilateFilter->SetKernel( structuringElement );
      erodeFilter->SetKernel( structuringElement );
    
      erodeFilter->Update();
      
      procImage = erodeFilter->GetOutput();
    }
  }

  // CC 1 & threshold
  if (CCLOn) {
    if (debug) cout << "Connected Component Labeling 1"  << endl;
    imageRegion = procImage->GetBufferedRegion();
    dim[0] = imageRegion.GetSize(0);
    dim[1] = imageRegion.GetSize(1);
    dim[2] = imageRegion.GetSize(2);
    data = &((*procImage)[nullIndex]);
    DoConnectedComponentLabeling(data, dim, 6); // 6 connectedness Connected component labeling

    binThreshFilterType::Pointer binTreshFilter = binThreshFilterType::New();
    binTreshFilter->SetInput(procImage);
    binTreshFilter->SetUpperThreshold(1);
    binTreshFilter->SetLowerThreshold(1);// only largest object (labels are sorted largest to smallest)
    binTreshFilter->SetOutsideValue (0);
    binTreshFilter->SetInsideValue (LABEL_VAL);
    binTreshFilter->Update();
    
    procImage = binTreshFilter->GetOutput();
  }

  // Conversion to float & gauss filter & Conversion to unsigned char & Tresholding at LABEL_VAL/2

  if (gaussianOn) {

    if (debug) cout << "conversions, smoothing" << endl;
    {
      castInputFilterType::Pointer convInFilter = castInputFilterType::New();
      convInFilter->SetInput(procImage);
      
      gaussFilterType::Pointer smoothFilter = gaussFilterType::New();  
      
      smoothFilter->SetInput(convInFilter->GetOutput());
      smoothFilter->SetVariance(variance);
      
      castOutputFilterType::Pointer convOutFilter = castOutputFilterType::New();
      
      convOutFilter->SetInput(smoothFilter->GetOutput());
      convOutFilter->Update();
      
      
      if (debug) cout << "Tresholding at 50% " << endl;
      binThreshFilterType::Pointer binTreshFilter = binThreshFilterType::New();
      binTreshFilter->SetInput(convOutFilter->GetOutput());
      binTreshFilter->SetUpperThreshold(LABEL_VAL);
      binTreshFilter->SetLowerThreshold(LABEL_VAL/2); 
      binTreshFilter->SetOutsideValue (0);
      binTreshFilter->SetInsideValue (LABEL_VAL);
      binTreshFilter->Update();
      
      if (debug) cout << "Closing 2 " << endl;
      if (asymCloseOn) {
  StructuringElementType structuringElement;
  AsymStructuringElementType asymStructuringElement;
  dilateFilterType::Pointer dilateFilter = dilateFilterType::New();  
  erodeAsymFilterType::Pointer erodeFilter = erodeAsymFilterType::New();  
  
  dilateFilter->SetInput(binTreshFilter->GetOutput());
  erodeFilter->SetInput(dilateFilter->GetOutput());
  dilateFilter->SetDilateValue (LABEL_VAL);
  erodeFilter->SetErodeValue (LABEL_VAL);
  structuringElement.SetRadius( elemSize );  // 3x3x3 structuring element
  asymStructuringElement.SetRadius( elemSize );
  asymStructuringElement.CreateStructuringElement( );
  structuringElement.CreateStructuringElement( );
  dilateFilter->SetKernel( structuringElement );
  erodeFilter->SetKernel( asymStructuringElement );
  
  erodeFilter->Update();
  
  procImage = erodeFilter->GetOutput();
      } else {
  StructuringElementType structuringElement;
  dilateFilterType::Pointer dilateFilter = dilateFilterType::New();  
  erodeFilterType::Pointer erodeFilter = erodeFilterType::New();  
  
  dilateFilter->SetInput(binTreshFilter->GetOutput());
  erodeFilter->SetInput(dilateFilter->GetOutput());
  dilateFilter->SetDilateValue (LABEL_VAL);
  erodeFilter->SetErodeValue (LABEL_VAL);
  structuringElement.SetRadius( elemSize );  // 3x3x3 structuring element
  structuringElement.CreateStructuringElement( );
  dilateFilter->SetKernel( structuringElement );
  erodeFilter->SetKernel( structuringElement );
  
  erodeFilter->Update();
  
  procImage = erodeFilter->GetOutput();
      }
    }

    // CC 2 & treshold
    if (CCLOn) {
      if (debug) cout << "Connected Component Labeling 2"  << endl;
      imageRegion = procImage->GetBufferedRegion();
      dim[0] = imageRegion.GetSize(0);
      dim[1] = imageRegion.GetSize(1);
      dim[2] = imageRegion.GetSize(2);
      data = &((*procImage)[nullIndex]);
      DoConnectedComponentLabeling(data, dim, 6); // 6 connectedness Connected component labeling
      
      binThreshFilterType::Pointer binTreshFilter = binThreshFilterType::New();
      binTreshFilter->SetInput(procImage);
      binTreshFilter->SetUpperThreshold(1);
      binTreshFilter->SetLowerThreshold(1);// only largest object (labels are sorted largest to smallest)
      binTreshFilter->SetOutsideValue (0);
      binTreshFilter->SetInsideValue (LABEL_VAL);
      binTreshFilter->Update();
      
      procImage = binTreshFilter->GetOutput();
    } 
  }

  if (LSSmoothOn) { // do level set smoothing!
    if (debug) cout << "Level Set Smoothing"  << endl;
    CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();
    toReal->SetInput(procImage );
    
    AntiAliasFilterType::Pointer antiAliasFilter = AntiAliasFilterType::New();
    antiAliasFilter->SetInput( toReal->GetOutput() );
    antiAliasFilter->SetMaximumRMSError( maximumRMSError );
    antiAliasFilter->SetNumberOfIterations( numberOfIterations );
    antiAliasFilter->SetNumberOfLayers( 2 );
    antiAliasFilter->Update();
  
    RescaleFilter::Pointer rescale = RescaleFilter::New();
    rescale->SetInput( antiAliasFilter->GetOutput() );
    rescale->SetOutputMinimum(   0 );
    rescale->SetOutputMaximum( 255 );

    binThreshFilterType::Pointer binTreshFilter = binThreshFilterType::New();
    binTreshFilter->SetInput(rescale->GetOutput());
    binTreshFilter->SetUpperThreshold(255);
    binTreshFilter->SetLowerThreshold(127); 
    binTreshFilter->SetOutsideValue (0);
    binTreshFilter->SetInsideValue (1);
    binTreshFilter->Update();
    procImage = binTreshFilter->GetOutput();
  }

  // connect everything by 6 connectedness, so that no diagonals in data
  if (debug) cout << "enforce strict 6 connectedness" << endl; 
  data = &((*procImage)[nullIndex]);
  imageRegion = procImage->GetBufferedRegion();
  dim[0] = imageRegion.GetSize(0);
  dim[1] = imageRegion.GetSize(1);
  dim[2] = imageRegion.GetSize(2);
  clear_edge(data, dim, 0);
  NoDiagConnect(data,dim);

  try
    {
      if (debug) cout << "writing processed data" << endl;
      VolumeWriterType::Pointer writer = VolumeWriterType::New();
      MetaImageIO::Pointer metaWriter = MetaImageIO::New();
      writer->SetImageIO(metaWriter);
      metaWriter->SetDataFileName("LOCAL");
      
      writer->SetFileName(outfileName.c_str());
      
      writer->SetInput(procImage);
      writer->Write();
    }
  catch (ExceptionObject e)
    {
      e.Print(cout) ;
      exit(0) ;
    }
  
    
  return 0 ;
}


static int
NoDiagConnect (unsigned char *image, int *dim) 
  // does not allow connection via diagonals only, enforces strict 6 connectedness
  // image has to be of type unsigned char
{

  //z axis
  int dimx = dim[0];
  int dimy = dim[1];
  int dimz = dim[2];
  bool correctionNeeded = true;
  int cnt = 0;

  while (correctionNeeded) {
    cnt++;
    if (debug) cout << "NoDiag scan " << cnt << endl; 
    correctionNeeded = false;
    int dy = dimx*dimy;
    int dx = dimx;

    for (int i = 1; i < dimz - 1; i++) {
      for (int j = 1; j < dimy - 1; j++) {
  for (int k = 1; k < dimz - 1; k++) {
    unsigned char val = image[i + j * dimx + k * dy];
    if (val != 0) {
      // x,y 
      if ((image[i-1+j*dx+k*dy] == 0) && (image[i+(j-1)*dx+k*dy] == 0) && (image[i-1+(j-1)*dx+k*dy] != 0)) {
        correctionNeeded = true;
        image[i-1+j*dx+k*dy] = val;
      }
      if ((image[i+1+j*dx+k*dy] == 0) && (image[i+(j+1)*dx+k*dy] == 0) && (image[i+1+(j+1)*dx+k*dy] != 0)) {
        correctionNeeded = true;
        image[i+1+j*dx+k*dy] = val;
      }
      if ((image[i+1+j*dx+k*dy] == 0) && (image[i+(j-1)*dx+k*dy] == 0) && (image[i+1+(j-1)*dx+k*dy] != 0)) {
        correctionNeeded = true;
        image[i+1+j*dx+k*dy] = val;
      }
      if ((image[i-1+j*dx+k*dy] == 0) && (image[i+(j+1)*dx+k*dy] == 0) && (image[i-1+(j+1)*dx+k*dy] != 0)) {
        correctionNeeded = true;
        image[i-1+j*dx+k*dy] = val;
      }
      
      // xz
      if ((image[i-1+j*dx+k*dy] == 0) && (image[i+j*dx+(k-1)*dy] == 0) && (image[i-1+j*dx+(k-1)*dy] != 0)) {
        correctionNeeded = true;
        image[i-1+j*dx+k*dy] = val;
      }
      if ((image[i+1+j*dx+k*dy] == 0) && (image[i+j*dx+(k+1)*dy] == 0) && (image[i+1+j*dx+(k+1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+1+j*dx+k*dy] = val;
      }
      if ((image[i+1+j*dx+k*dy] == 0) && (image[i+j*dx+(k-1)*dy] == 0) && (image[i+1+j*dx+(k-1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+1+j*dx+k*dy] = val;
      }
      if ((image[i-1+j*dx+k*dy] == 0) && (image[i+j*dx+(k+1)*dy] == 0) && (image[i-1+j*dx+(k+1)*dy] != 0)) {
        correctionNeeded = true;
        image[i-1+j*dx+k*dy] = val;
      }
      
      // yz
      if ((image[i+(j-1)*dx+k*dy] == 0) && (image[i+j*dx+(k-1)*dy] == 0) && (image[i+(j-1)*dx+(k-1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+(j-1)*dx+k*dy] = val;
      }
      if ((image[i+(j+1)*dx+k*dy] == 0) && (image[i+j*dx+(k+1)*dy] == 0) && (image[i+(j+1)*dx+(k+1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+(j+1)*dx+k*dy] = val;
      }
      if ((image[i+(j+1)*dx+k*dy] == 0) && (image[i+j*dx+(k-1)*dy] == 0) && (image[i+(j+1)*dx+(k-1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+(j+1)*dx+k*dy] = val;
      }
      if ((image[i+(j-1)*dx+k*dy] == 0) && (image[i+j*dx+(k+1)*dy] == 0) && (image[i+(j-1)*dx+(k+1)*dy] != 0)) {
        correctionNeeded = true;
        image[i+(j-1)*dx+k*dy] = val;
      }
    }
  }
      }
    }
  }

}

static void clear_edge(unsigned char *image, int *dims, int clear_label)
  // clears the edge of the image
{
  int size_plane = dims[0]*dims[1];
  int size_line = dims[0];

  for (int z = 0; z < dims[2]; z++) {
    for (int y = 0; y < dims[1]; y++) {
      if ( (y == 0) || (y == dims[1]-1) ||
     (z == 0) || (z == dims[2]-1) ) { // draw whole plane
  for (int x = 0; x < dims[0] ; x++) 
    image[x +  size_line * y + size_plane * z] = clear_label;
      } else { // draw edges of x
  image[0 +  size_line * y + size_plane * z] = clear_label;
  image[size_line - 1 +  size_line * y + size_plane * z] = clear_label;
      }
    }
  }

}


static void 
draw_fill_inside_image(unsigned char *image, int *dims, int new_label)
  // Fill the 'inside' part of an image (closed objects that do not touch the
  // image edges)
{
  int size_plane = dims[0]*dims[1];
  int size_line = dims[0];

  unsigned char label;
  if (new_label > 1) label = new_label-1;
  else label = new_label+1;
  
  unsigned char background = 0;

  // fill image edges -> won't work if object touches the edge !!
  for (int z = 0; z < dims[2]; z++) {
    for (int y = 0; y < dims[1]; y++) {
      if ( (y == 0) || (y == dims[1]-1) ||
     (z == 0) || (z == dims[2]-1) ) { // draw whole plane
  for (int x = 0; x < dims[0] ; x++) 
    image[x +  size_line * y + size_plane * z] = label;
      } else { // draw edges of x
  image[0 +  size_line * y + size_plane * z] = label;
  image[size_line - 1 +  size_line * y + size_plane * z] = label;
      }
    }
  }
  
  // forward propagation
  for (int z = 1; z < dims[2]-1; z++) {
    for (int y = 1; y < dims[1]-1; y++) {
      for (int x = 1; x < dims[0]-1; x++) {
  int index = x +  size_line * y + size_plane * z;
  if (image[index] == background &&    // check past neighborhood
      (image[index - 1] == label || 
       image[index + size_line] == label || 
       image[index - size_plane] == label 
       )) {
    image[index] = label;
  }
      }
    }
  }

  // backward propagation
  for (int z = dims[2]-2; z > 0; z--) {
    for (int y = dims[1]-2; y > 0; y--) {
      for (int x = dims[0]-2; x > 0; x--) {
  int index = x +  size_line * y + size_plane * z;
  if (image[index] == background &&    // check past neighborhood
      (image[index + 1] == label || 
       image[index + size_line] == label || 
       image[index + size_plane] == label 
       )) {
    image[index] = label;
  }
      }
    }
  }

  // reassign labels
  for (int i = 0; i < dims[2]*dims[1]*dims[0]; i++) {
    if (image[i] == label) image[i] = background;
    else if (image[i] == background) image[i] = new_label;
  }
}
