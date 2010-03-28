/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: MeanSquaresImageMetric1.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/21 23:49:15 $
  Version:   $Revision: 1.9 $

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
// the apropriate selection of the optimizer to be use for driving the
// registration process, as well as for selecting the optimizer parameters.
// This process makes possible to identify how noisy a metric may be in a given
// range of parameters, and it will also give an idea of the number of local
// minima or maxima in which an optimizer may get trapped while exploring the
// parametric space.
//
// Software Guide : EndLatex 



#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVersorRigid3DTransform.h"



// Software Guide : BeginLatex
//
// We start by including the headers of the basic components: Metric, Transform
// and Interpolator.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMutualInformationImageToImageMetric.h"


#include "itkLinearInterpolateImageFunction.h"
#include "itkTranslationTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
// Software Guide : EndCodeSnippet


int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  fixedImage  movingImage NumberOfHistogramBins(20) NumberOfSpatialSamples(10000)" << std::endl;
    return 1;
    }

// Software Guide : BeginLatex
//
// We define the dimension and pixel type of the images to be used in the
// evaluation of the Metric.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  const     unsigned int   Dimension = 3;
  typedef   float  PixelType;

  typedef itk::Image< PixelType, Dimension >   ImageType;
// Software Guide : EndCodeSnippet




  typedef itk::ImageFileReader< ImageType >  ReaderType;

  ReaderType::Pointer fixedReader  = ReaderType::New();
  ReaderType::Pointer movingReader = ReaderType::New();

  fixedReader->SetFileName(  argv[ 1 ] );
  movingReader->SetFileName( argv[ 2 ] );

  try 
    {
    fixedReader->Update();
    movingReader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    }

// Software Guide : BeginLatex
//
// The type of the Metric is instantiated and one is constructed.  In this case
// we decided to use the same image type for both the fixed and the moving
// images.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  //typedef itk::MeanSquaresImageToImageMetric< 
  //                       ImageType, ImageType >  MetricType;
  
  //typedef itk::NormalizedCorrelationImageToImageMetric< 
  //                                ImageType, 
  //                                ImageType >    MetricType;

  //typedef itk::MutualInformationImageToImageMetric< 
  //                                       ImageType, 
  //                                        ImageType >    MetricType;

  typedef itk::MattesMutualInformationImageToImageMetric< 
                                          ImageType, 
                                          ImageType >    MetricType;
  MetricType::Pointer metric = MetricType::New();
// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// We also instantiate the transform and interpolator types, and create objects
// of each class.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  //typedef itk::TranslationTransform< double, Dimension >  TransformType;

  typedef itk::VersorRigid3DTransform< double > TransformType;

  TransformType::Pointer transform = TransformType::New();


  //typedef itk::NearestNeighborInterpolateImageFunction< 
  //                             ImageType, double >  InterpolatorType;

 typedef itk:: LinearInterpolateImageFunction< 
                                    ImageType,
                                    double          >    InterpolatorType;


  InterpolatorType::Pointer interpolator = InterpolatorType::New();
// Software Guide : EndCodeSnippet


  transform->SetIdentity();

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

  
metric->ReinitializeSeed(1);


metric->SetNumberOfHistogramBins( atoi(argv[3]) );
  metric->SetNumberOfSpatialSamples( atoi(argv[4]) );
// Software Guide : EndCodeSnippet

   metric->SetFixedImageRegion(  fixedImage->GetBufferedRegion()  );

  try 
    {
    metric->Initialize();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception catched !" << std::endl;
    std::cerr << excep << std::endl;
    return -1;
    }


// Software Guide : BeginLatex
//
// Finally we select a region of the parametric space to explore. In this case
// we are using a translation transform in 2D, so we simply select translations
// from a negative position to a positive position, in both $x$ and $y$. For
// each one of those positions we invoke the GetValue() method of the Metric.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

 
  MetricType::TransformParametersType displacement( 2*Dimension );

  
  //  const double  rax = 0.52; //~30 degree
  //  const double  ray = 0.52; //~30 degrre
  //  const double  raz = 0.52; //~30 degree
 

  const double  rax = 0.52; //~30 degree
  const double  ray = 0.52; //~30 degrre
  const double  raz = 0.52; //~30 degree


  const int rangex = 45;
  const int rangey = 45;
  const int rangez = 45;

  
   
 for( double ax = -rax; ax <= rax; ax+=0.03 )//~4 degree = 0.05
    {
    for( double ay = -ray; ay <= ray; ay+=0.03 )
      {
      for ( double az = -raz; az <= raz; az+=0.03 )
        {
           for( int dx = -rangex; dx <= rangex; dx+=5 )
                { 
                for( int dy = -rangey; dy <= rangey; dy+=5 )
                {
                   for ( int dz = -rangez; dz <= rangez; dz+=5 )
                     {
                    displacement[0] = ax ;
                    displacement[1] = ay;
                    displacement[2] = az;
                    displacement[3] = dx;
                    displacement[4] = dy;
                    displacement[5] = dz;
                    
                     double value;  
                     try 
                       {
                        value = metric->GetValue( displacement );
                       }
                        catch( itk::ExceptionObject & excep )
                       {         
                        // std::cerr << "Exception catched !" << std::endl;
                        //std::cerr << excep << std::endl;
                        //return -1;
                       }
                        // const double value = metric->GetValue( displacement );
                        //const double value = metric->GetValue( transform );
                        std::cout  << ax << "   "  << ay <<  "   " << az << "   " << dx << "   "  << dy <<  "   " << dz << "   " << value << std::endl;
 }
 }
 }
} }
  }

  

// Software Guide : EndCodeSnippet



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



  return 0;
}

