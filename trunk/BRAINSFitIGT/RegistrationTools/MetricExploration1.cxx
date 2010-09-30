/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MeanSquaresImageMetric1.cxx,v $
  Language:  C++
  Date:      $Date: 2006-11-12 22:08:29 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif




// Software Guide : BeginLatex
//
// This example illustrates how to explore the domain of an image metric.  This
// is a useful exercise to do before starting a registration process, since
// getting familiar with the characteristics of the metric is fundamental for
// the appropriate selection of the optimizer to be use for driving the
// registration process, as well as for selecting the optimizer parameters.
// This process makes possible to identify how noisy a metric may be in a given
// range of parameters, and it will also give an idea of the number of local
// minima or maxima in which an optimizer may get trapped while exploring the
// parametric space.
//
// Software Guide : EndLatex 



#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkMeanSquaresImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkTranslationTransform.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageMaskSpatialObject.h"

// Software Guide : EndCodeSnippet


int main( int argc, char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  fixedImage  movingImage transform [fixed_mask moving_mask]" << std::endl;
    return EXIT_FAILURE;
    }

// Software Guide : BeginLatex
//
// We define the dimension and pixel type of the images to be used in the
// evaluation of the Metric.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  typedef itk::OrientedImage< float, 3 >   ImageType;
  typedef itk::OrientedImage< unsigned char, 3 >   MaskImageType;
// Software Guide : EndCodeSnippet




  typedef itk::ImageFileReader< ImageType >  ReaderType;
  typedef itk::ImageFileReader< MaskImageType >  MaskReaderType;
  typedef itk::TransformFileReader TransformReaderType;
  typedef itk::TransformFileWriter TransformWriterType;

  ReaderType::Pointer fixedReader  = ReaderType::New();
  ReaderType::Pointer movingReader = ReaderType::New();
  TransformReaderType::Pointer tfmReader = TransformReaderType::New();

  fixedReader->SetFileName(  argv[ 1 ] );
  movingReader->SetFileName( argv[ 2 ] );
  tfmReader->SetFileName( argv[3] );

  try 
    {
    fixedReader->Update();
    movingReader->Update();
    tfmReader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }

  typedef itk::AffineTransform<double> TransformType;
  typedef itk::MattesMutualInformationImageToImageMetric< 
                            ImageType, ImageType >  MetricType;
  MetricType::FixedImageMaskType::Pointer fixedMask = NULL;
  MetricType::MovingImageMaskType::Pointer movingMask = NULL;

  if(argc>4){
    MaskReaderType::Pointer rm1 = MaskReaderType::New();
    rm1->SetFileName(argv[4]);
    rm1->Update();
    MaskReaderType::Pointer rm2 = MaskReaderType::New();
    rm2->SetFileName(argv[5]);
    rm2->Update();
    MaskImageType::Pointer fixedMaskImage = rm1->GetOutput();
    MaskImageType::Pointer movingMaskImage = rm2->GetOutput();

    typedef itk::ImageMaskSpatialObject<3> MaskObject;
    MaskObject::Pointer fixedMaskObject = MaskObject::New();
    MaskObject::Pointer movingMaskObject = MaskObject::New();
    fixedMaskObject->SetImage(fixedMaskImage);
    movingMaskObject->SetImage(movingMaskImage);
    fixedMaskObject->ComputeObjectToWorldTransform();
    movingMaskObject->ComputeObjectToWorldTransform();
    fixedMask = dynamic_cast<MetricType::FixedImageMaskType*>(fixedMaskObject.GetPointer());
    movingMask = dynamic_cast<MetricType::MovingImageMaskType*>(movingMaskObject.GetPointer());
    ImageType::PointType p;
    p[0] = 41;
    p[1] = 25;
    p[2] = -18;
    std::cout << fixedMask->IsInside(p) << " , " << movingMask->IsInside(p) << std::endl;
  }


  MetricType::Pointer metric = MetricType::New();


// Software Guide : EndCodeSnippet


  TransformType::Pointer transform = TransformType::New();
  if(!dynamic_cast<TransformType*>(tfmReader->GetTransformList()->front().GetPointer())){
    std::cerr << "Input transform is not Affine!" << std::endl;
    return -1;
  }
  transform = dynamic_cast<TransformType*>(tfmReader->GetTransformList()->front().GetPointer());
  /*
  TransformReaderType::TransformType::Pointer rtfm = 
    *(tfmReader->GetTransformList()->begin());
  typedef itk::MatrixOffsetTransformBase<double,3,3> OffsetType;
  OffsetType::Pointer d =
    dynamic_cast<OffsetType*>(rtfm.GetPointer());

  if(!d){
    std::cerr << "Cast failed!" << std::endl;
    return -1;
  }

  vnl_svd<double> svd(d->GetMatrix().GetVnlMatrix());
  transform->SetMatrix(svd.U()*vnl_transpose(svd.V()));
  transform->SetOffset(d->GetOffset());
  */
  TransformWriterType::Pointer tfmWriter = TransformWriterType::New();
  tfmWriter->SetFileName("transform.tfm");
  tfmWriter->SetInput(transform);
  tfmWriter->Update();
 
//  typedef itk::NearestNeighborInterpolateImageFunction< 
//                                 ImageType, double >  InterpolatorType;
  typedef itk::LinearInterpolateImageFunction< 
                                 ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();
// Software Guide : EndCodeSnippet

  ImageType::ConstPointer fixedImage  = fixedReader->GetOutput();
  ImageType::ConstPointer movingImage = movingReader->GetOutput();


// Software Guide : BeginLatex
//
// The classes required by the metric are connected to it. This includes the
// fixed and moving images, the interpolator and the  transform.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  metric->SetTransform( transform );
  metric->SetInterpolator( interpolator );

  metric->SetFixedImage(  fixedImage  );
  metric->SetMovingImage( movingImage );
// Software Guide : EndCodeSnippet

  metric->SetFixedImageRegion(  fixedImage->GetBufferedRegion()  );
  std::cout << "Original number of spatial samples: " << metric->GetNumberOfSpatialSamples() 
    << std::endl;
  metric->SetNumberOfSpatialSamples(100000);
  
  // MI-specific parameters
  metric->SetNumberOfHistogramBins(50);
  metric->SetNumberOfSpatialSamples(1e+5);

  std::cout << "argc = " << argc << std::endl;
  if(argc>4){    
    metric->SetFixedImageMask(fixedMask);
    metric->SetMovingImageMask(movingMask);
    std::cout << "Image masks have been initialized" << std::endl;
  }

//  metric->SetUseFixedImageSamplesIntensityThreshold(true);
//  metric->SetFixedImageSamplesIntensityThreshold(10);

  std::cout << "Before initialize metric" << std::endl;
  try 
    {
    metric->Initialize();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Done" << std::endl;


// Software Guide : BeginLatex
//
// Finally we select a region of the parametric space to explore. In this case
// we are using a translation transform in 2D, so we simply select translations
// from a negative position to a positive position, in both $x$ and $y$. For
// each one of those positions we invoke the GetValue() method of the Metric.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  MetricType::TransformParametersType parameters;
  parameters = transform->GetParameters();
  std::cout << "Translation: " << parameters[9] << ", " << parameters[10] <<
    ", " << parameters[11] << std::endl;
  double initTranslation[3];
  initTranslation[0] = parameters[9];
  initTranslation[1] = parameters[10];
  initTranslation[2] = parameters[11];

//  std::cout << parameters << std::endl;
//  std::cout << "Metric value: " << metric->GetValue(parameters) << std::endl;
//  fprintf(stderr, "%.10f\n", metric->GetValue(parameters));

/*
  const float rangex = 5;
  const float rangey = 5;

  for( float dx = -rangex; dx <= rangex; dx+=0.2 )
    {
    for( float dy = -rangey; dy <= rangey; dy+=0.2 )
      {
      parameters[9] = initTranslation[0]+dx;
      parameters[10] = initTranslation[1]+dy;
      const double value = metric->GetValue( parameters );
//      std::cout << parameters << std::endl;
      std::cout << dx << "   "  << dy << "   " << value << std::endl;
      }
    }
*/
  
  const float anglerangex = 3;
  const float anglerangey = 3;
  
  for( float dx = -anglerangex; dx <= anglerangex; dx+=0.3 )
    {
    for( float dy = -anglerangey; dy <= anglerangey; dy+=0.3 )
      {
      TransformType::Pointer tmpTransform = TransformType::New();
      tmpTransform->SetParameters(parameters);
      tmpTransform->Rotate3D(dx, 0);
      tmpTransform->Rotate3D(dy, 1);
      parameters = tmpTransform->GetParameters();
//      parameters[9] = initTranslation[0]+dx;
//      parameters[10] = initTranslation[1]+dy;
      const double value = fabs(metric->GetValue( parameters ));
      std::cout << dx << "   "  << dy << "   " << value << std::endl;
      }
    }
    

// Software Guide : EndCodeSnippet

#if 0

// Software Guide : BeginLatex
//
// \begin{figure}
// \center
// \includegraphics[height=0.33\textwidth]{MeanSquaresMetricPlot1.eps}
// \includegraphics[height=0.33\textwidth]{MeanSquaresMetricPlot2.eps}
// \itkcaption[Mean Squares Metric Plots]{Plots of the Mean Squares Metric for
// an image compared to itself under multiple translations.}
// \label{fig:MeanSquaresMetricPlot}
// \end{figure}
//
// Running this code using the image BrainProtonDensitySlice.png as both the
// fixed and the moving images results in the plot shown in
// Figure~\ref{fig:MeanSquaresMetricPlot}. From this Figure, it can be seen
// that a gradient based optimizer will be appropriate for finding the extrema
// of the Metric. It is also possible to estimate a good value for the step
// length of a gradient-descent optimizer.
//
// This exercise of plotting the Metric is probably the best thing to do when a
// registration process is not converging and when it is unclear how to fine
// tune the different parameters involved in the registration. This includes
// the optimizer parameters, the metric parameters and even options such as
// preprocessing the image data with smoothing filters.
//
// The shell and Gnuplot\footnote{http://www.gnuplot.info} scripts used for
// generating the graphics in Figure~\ref{fig:MeanSquaresMetricPlot} are
// available in the directory
//
//             \code{InsightDocuments/SoftwareGuide/Art}
// 
// Of course, this plotting exercise becomes more challenging when the
// transform has more than three parameters, and when those parameters have
// very different range of values. In those cases is necessary to select only a
// key subset of parameters from the transform and to study the behavior of the
// metric when those parameters are varied.
//
//
// Software Guide : EndLatex 

#endif

  return EXIT_SUCCESS;
}


