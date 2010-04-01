/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: PointSetToListAdaptor.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:56:49 $
  Version:   $Revision: 1.10 $

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
// We will decribe how to use \doxygen{PointSet} as a
// \subdoxygen{Statistics}{Sample} using an adaptor in this example.
//
// \index{itk::Statistics::PointSetToListAdaptor}
//
// The \subdoxygen{Statistics}{PointSetToListAdaptor} class requires a
// PointSet as input. The PointSet class is an associative data
// container. Each point in a PointSet object can have an associated
// optional data value. For the statistics subsystem, the current
// implementation of PointSetToListAdaptor takes only the point part
// into consideration. In other words, the measurement vectors from a
// PointSetToListAdaptor object are points from the PointSet
// object that is plugged into the adaptor object.
//
// To use an PointSetToListAdaptor class, we include the header file for the
// class.
//
// Software Guide : EndLatex 


// Software Guide : BeginCodeSnippet
#include "itkPointSetToListAdaptor.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Since we are using an adaptor, we also include the header file for
// the PointSet class.
//
// Software Guide :EndLatex

// Software Guide : BeginCodeSnippet
#include "itkPointSet.h"
#include "itkVector.h"
// Software Guide : EndCodeSnippet

int main()
{
  // Software Guide : BeginLatex
  //
  // Next we create a PointSet object (see
  // Section~\ref{sec:CreatingAPointSet} otherwise). The following code
  // snippet will create a PointSet object that stores points (its coordinate
  // value type is float) in 3D space.
  // 
  // Software Guide :EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::PointSet< short > PointSetType;
  PointSetType::Pointer pointSet = PointSetType::New();
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // Note that the \code{short} type used in the declaration of
  // \code{PointSetType} pertains to the pixel type associated with every
  // point, not to the type used to represent point coordinates.  If we want
  // to change the type of point in terms of the coordinate value and/or
  // dimension, we have to modify the \code{TMeshTraits} (one of the optional
  // template arguments for the \code{PointSet} class). The easiest way of
  // create a custom mesh traits instance is to specialize the existing
  // \doxygen{DefaultStaticMeshTraits}. By specifying the \code{TCoordRep}
  // template argument, we can change the coordinate value type of a point.
  // By specifying the \code{VPointDimension} template argument, we can
  // change the dimension of the point. As mentioned earlier, a
  // \code{PointSetToListAdaptor} object cares only about the points, and the
  // type of measurement vectors is the type of points. Therefore, we can
  // define the measurment vector type as in the following code snippet.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef PointSetType::PointType MeasurementVectorType;
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // To make the example a little bit realistic, we add two point 
  // into the \code{pointSet}.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  PointSetType::PointType point;
  point[0] = 1.0;
  point[1] = 2.0;
  point[2] = 3.0;

  pointSet->SetPoint( 0UL, point);

  point[0] = 2.0;
  point[1] = 4.0;
  point[2] = 6.0;

  pointSet->SetPoint( 1UL, point );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // Now we have a PointSet object that has two points in it. And the 
  // pointSet is ready to be plugged into the adaptor.
  // First, we create an instance of the PointSetToListAdaptor class
  // with the type of the input PointSet object.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::Statistics::PointSetToListAdaptor< PointSetType > SampleType;
  SampleType::Pointer sample = SampleType::New();
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // Second, just as we did with the ImageToListAdaptor example in
  // Section~\ref{sec:ImageToListAdaptor}, all we have to do is to
  // plug in the PointSet object to the adaptor.  After that,
  // we can use the common methods and iterator interfaces shown in
  // Section~\ref{sec:SampleInterface}.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  sample->SetPointSet( pointSet );
  // Software Guide : EndCodeSnippet

  return 0;
}