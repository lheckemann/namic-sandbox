/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: OrientImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/24 17:05:56 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
//  Software Guide : BeginCommandLineArgs
//    INPUTS: {BrainProtonDensitySlice.png}
//    OUTPUTS: {FlipImageFilterOutput.png}
//    0 1
//  Software Guide : EndCommandLineArgs

//  Software Guide : BeginLatex
//
//  The \doxygen{FlipImageFilter} is used for flipping the image content in any
//  of the coordinate axis. This filter must be used with \textbf{EXTREME}
//  caution. You probably don't want to appear in the newspapers as the
//  responsible of a surgery mistake in which a doctor extirpates the left
//  kidney when it should have extracted the right one\footnote{\emph{Wrong
//  side} surgey accounts for $2\%$ of the reported medical errors in the United
//  States. Trivial... but equally dangerous.} . If that prospect doesn't
//  scares you, maybe it is time for you to reconsider your career in medical
//  image processing. Flipping effects that may seem inocuous at first view may
//  still have dangerous consequences. For example flipping the cranio-caudal
//  axis of a CT scans forces an observer to flip the left-right axis in order
//  to make sense of the image.
//
//  \index{itk::FlipImageFilter}
//
//  Software Guide : EndLatex 


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"



//  Software Guide : BeginLatex
//
//  The header file corresponding to this filter should be included first.
//
//  \index{itk::OrientImageFilter!header}
//
//  Software Guide : EndLatex 


// Software Guide : BeginCodeSnippet
#include "itkOrientImageFilter.h"
// Software Guide : EndCodeSnippet


int main( int argc, char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile   outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  //rps to lia
  //  Software Guide : BeginLatex
  //
  //  Then the pixel types for input and output image must be defined and, with
  //  them, the image types can be instantiated.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef   signed short  PixelType;

  typedef itk::Image< PixelType,  3 >   ImageType;
  // Software Guide : EndCodeSnippet


  typedef itk::ImageFileReader< ImageType >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  //  Software Guide : BeginLatex
  //
  //  Using the image types it is now possible to instantiate the filter type
  //  and create the filter object. 
  //
  //  \index{itk::OrientImageFilter!instantiation}
  //  \index{itk::OrientImageFilter!New()}
  //  \index{itk::OrientImageFilter!Pointer}
  // 
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::OrientImageFilter< ImageType, ImageType >  FilterType;

  FilterType::Pointer filter = FilterType::New();
  // Software Guide : EndCodeSnippet

  //filter->UseImageDirectionOn();
  //filter->SetGivenCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS);
  reader->Update();
  itk::SpatialOrientation::ValidCoordinateOrientationFlags fileOrientation;
  itk::ExposeMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
    (reader->GetOutput()->GetMetaDataDictionary(),itk::ITK_CoordinateOrientation,fileOrientation);
  filter->SetGivenCoordinateOrientation(fileOrientation);
  filter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA);

  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  The input to the filter can be taken from any other filter, for example
  //  a reader. The output can be passed down the pipeline to other filters,
  //  for example, a writer. An update call on any downstream filter will
  //  trigger the execution of the mean filter.
  //
  //  \index{itk::OrientImageFilter!SetInput()}
  //  \index{itk::OrientImageFilter!GetOutput()}
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  // 
  // \begin{figure}
  // \center
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySlice.eps}
  // \includegraphics[width=0.44\textwidth]{OrientImageFilterOutput.eps}
  // \itkcaption[Effect of the MedianImageFilter]{Effect of the OrientImageFilter on a slice
  // from a MRI proton density brain image.}
  // \label{fig:OrientImageFilterOutput}
  // \end{figure}
  //
  //  Figure \ref{fig:OrientImageFilterOutput} illustrates the effect of this
  //  filter on a slice of MRI brain image using a flip array $[0,1]$ which
  //  means that the $Y$ axis was flipped while the $X$ axis was conserved.
  //
  //  Software Guide : EndLatex 


  return EXIT_SUCCESS;
}

