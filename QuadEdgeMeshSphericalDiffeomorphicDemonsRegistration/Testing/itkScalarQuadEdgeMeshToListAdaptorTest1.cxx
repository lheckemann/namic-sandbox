/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarQuadEdgeMeshToListAdaptorTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-10 19:46:31 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkVector.h"
#include "itkListSample.h"
#include "itkQuadEdgeMesh.h"

#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkScalarQuadEdgeMeshToListAdaptor.h"
#include "itkListSampleToHistogramGenerator.h"

int main( int argc, char * argv [] )
{

  if( argc < 5 )
    {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage :  QuadEdgeMeshHistogram  inputMeshFileName " << std::endl;
    std::cerr << "NumberOfBins Min Max" << std::endl;
    return -1;
    }

  typedef float      MeshPixelType;
  const unsigned int Dimension = 3;

  const unsigned int MeasurementVectorLength = 1;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   MeshType;

  typedef itk::QuadEdgeMeshVTKPolyDataReader< MeshType >         InputReaderType;

  InputReaderType::Pointer inputMeshReader = InputReaderType::New();
  inputMeshReader->SetFileName( argv[1] );
  inputMeshReader->Update();


  typedef itk::ScalarQuadEdgeMeshToListAdaptor< MeshType >   AdaptorType;

  AdaptorType::Pointer adaptor = AdaptorType::New();

  adaptor->SetMesh(  inputMeshReader->GetOutput() );

  try
    {
  adaptor->Compute();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  typedef MeshPixelType        HistogramMeasurementType;

  typedef itk::Vector< MeshPixelType , MeasurementVectorLength > MeasurementVectorType ;
  typedef itk::Statistics::ListSample< MeasurementVectorType > ListSampleType ;

  typedef itk::Statistics::ListSampleToHistogramGenerator< 
                                                ListSampleType, 
                                                HistogramMeasurementType, 
                                                itk::Statistics::DenseFrequencyContainer, 
                                                MeasurementVectorLength  > GeneratorType; 

  GeneratorType::Pointer generator = GeneratorType::New();

  typedef GeneratorType::HistogramType  HistogramType;

  HistogramType::SizeType size;
  size.Fill( atoi(argv[2]) );

  generator->SetListSample( adaptor->GetSample() );
  generator->SetNumberOfBins( size );
  generator->SetMarginalScale( 10.0 );

  HistogramType::MeasurementVectorType min;
  HistogramType::MeasurementVectorType max;
  
  min.Fill(  atof(argv[3]) - 0.5 );
  max.Fill(  atof(argv[4]) + 0.5 );
  
  generator->SetHistogramMin( min );
  generator->SetHistogramMax( max );

  generator->Update();

  HistogramType::ConstPointer histogram = generator->GetOutput();

  const unsigned int histogramSize = histogram->Size();

  std::cout << "Histogram size " << histogramSize << std::endl;

  for( unsigned int bin=0; bin < histogramSize; bin++ )
    {
    std::cout << "bin = " << bin << " frequency = ";
    std::cout << histogram->GetFrequency( bin, 0 ) <<std::endl;
    }

  return 0;
  
}
