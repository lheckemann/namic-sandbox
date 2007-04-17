/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:13 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkHistogram.h"

#include "itkDenseFrequencyContainer.h"

int itkHistogramTest(int, char* [] ) 
{
  std::cout << "Histogram Test \n \n"; 
  bool pass = true;
  std::string whereFail = "";
  
  typedef float MeasurementType;
  const unsigned int numberOfComponents = 3;

  // creats a histogram with 3 components measurement vectors
  typedef itk::Statistics::Histogram< MeasurementType, 
          numberOfComponents, 
          itk::Statistics::DenseFrequencyContainer > HistogramType;
  HistogramType::Pointer histogram = HistogramType::New();

  typedef HistogramType::MeasurementVectorType MeasurementVectorType;
  typedef HistogramType::InstanceIdentifier    InstanceIdentifier; 
  typedef HistogramType::IndexType             IndexType; 

  // initializes a 64 x 64 x 64 histogram with equal size interval
  HistogramType::SizeType size;
  size.Fill(64);
  unsigned long totalSize = size[0] * size[1] * size[2];
  MeasurementVectorType lowerBound;
  MeasurementVectorType upperBound;
  lowerBound.Fill(0);
  upperBound.Fill(1024);
  histogram->Initialize(size, lowerBound, upperBound );
  histogram->SetToZero();
  double interval = 
    (upperBound[0] - lowerBound[0]) / 
    static_cast< HistogramType::MeasurementType >(size[0]);

  // tests begin
  MeasurementVectorType measurements;
  measurements.Fill(512);
  IndexType index;
  IndexType ind;
  index.Fill(32);
  if(histogram->GetIndex(measurements,ind))
    {
    if(index != ind)
      {
      pass = false;
      whereFail = "GetIndex(MeasurementVectorType&)";
      }
    }
  else
    {
    pass = false;
    whereFail = "GetIndex(MeasurementVectorType&)";
    }
  
  InstanceIdentifier id = histogram->GetInstanceIdentifier(index);
  if (index != histogram->GetIndex(id))
    {
    pass = false;
    whereFail = "GetIndex(InstanceIdentifier&)";
    }

  index.Fill( -5 ); // test for outside below
  
  if( !histogram->IsIndexOutOfBounds(index) )
    {
    std::cerr << "IsIndexOutOfBounds() for " << index << std::endl;
    pass = false;
    whereFail = "IsIndexOutOfBounds(IndexType)";
    }


  index.Fill(32); // test for inside
  
  if( histogram->IsIndexOutOfBounds(index) )
    {
    std::cerr << "IsIndexOutOfBounds() for " << index << std::endl;
    pass = false;
    whereFail = "IsIndexOutOfBounds(IndexType)";
    }

  index.Fill(100); // test for outside
  
  if( !histogram->IsIndexOutOfBounds(index) )
    {
    std::cerr << "IsIndexOutOfBounds() for " << index << std::endl;
    pass = false;
    whereFail = "IsIndexOutOfBounds(IndexType)";
    }

  if (totalSize != histogram->Size())
    {
    pass = false;
    whereFail = "Size()";
    }

  if (size != histogram->GetSize())
    {
    pass = false;
    whereFail = "GetSize()";
    }

  if ((lowerBound[0] + interval * 31) != histogram->GetBinMin(0,31))
    {
    pass = false;
    whereFail = "GetBinMin(Dimension, nthBin)";
    }

  if ((lowerBound[0] + interval * 32) != histogram->GetBinMax(0,31))
    {
    pass = false;
    whereFail = "GetBinMax(Dimension, nthBin)";
    }

  for (id = 0; 
       id < static_cast< InstanceIdentifier >(totalSize);
       id++)
    {
    histogram->SetFrequency(id, 1);
    histogram->IncreaseFrequency(id, 1);
    if (histogram->GetFrequency(id) != 2)
      {
      pass = false;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) + IncreaseFrequency(InstanceIdentifier, 1) + GetFrequency(InstanceIdentifier)";
      }
    }

  double quantile1 = histogram->Quantile(0, 0.3);
  if( quantile1 != 307.2)
    {
    std::cerr << "quantile1 = " << quantile1 << std::endl;
    pass = false;
    whereFail = "Quantile(Dimension, percent)";
    }

  double quantile2 = histogram->Quantile(0, 0.5);
  if( quantile2 != 512.0)
    {
    std::cerr << "quantile2 = " << quantile2 << std::endl;
    pass = false;
    whereFail = "Quantile(Dimension, percent)";
    }

  double quantile3 = histogram->Quantile(0, 0.7);
  if( quantile3 != 716.8)
    {
    std::cerr << "quantile3 = " << quantile3 << std::endl;
    pass = false;
    whereFail = "Quantile(Dimension, percent)";
    }


  if( !pass )
    {
    std::cerr << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }


  // Histogram with SparseFrequencyContainer
  typedef itk::Statistics::Histogram< MeasurementType, 3, 
    itk::Statistics::SparseFrequencyContainer > SparseHistogramType;
  SparseHistogramType::Pointer sparseHistogram = SparseHistogramType::New();

  // initializes a 64 x 64 x 64 histogram with equal size interval
  sparseHistogram->Initialize(size, lowerBound, upperBound );
  sparseHistogram->SetToZero();
  interval = (upperBound[0] - lowerBound[0]) / 
    static_cast< SparseHistogramType::MeasurementType >(size[0]);

  measurements.Fill(512);
  index.Fill(32);
  sparseHistogram->GetIndex(measurements,ind);

  if (index != ind)
    {
    pass = false;
    whereFail = "Sparse Histogram: GetIndex(MeasurementVectorType&)";
    }
  
  id = sparseHistogram->GetInstanceIdentifier(index);
  if (index != sparseHistogram->GetIndex(id))
    {
    pass = false;
    whereFail = "Sparse Histogram: GetIndex(InstanceIdentifier&)";
    }

  index.Fill(100);
  
  if (!sparseHistogram->IsIndexOutOfBounds(index))
    {
    pass = false;
    whereFail = "Sparse Histogram: IsIndexOutOfBounds(IndexType)";
    }

  if (totalSize != sparseHistogram->Size())
    {
    pass = false;
    whereFail = "Sparse Histogram: Size()";
    }

  if (size != sparseHistogram->GetSize())
    {
    pass = false;
    whereFail = "Sparse Histogram: GetSize()";
    }

  if ((lowerBound[0] + interval * 31) != sparseHistogram->GetBinMin(0,31))
    {
    pass = false;
    whereFail = "Sparse Histogram: GetBinMin(Dimension, nthBin)";
    }

  if ((lowerBound[0] + interval * 32) != sparseHistogram->GetBinMax(0,31))
    {
    pass = false;
    whereFail = "Sparse Histogram: GetBinMax(Dimension, nthBin)";
    }


  for (id = 0; 
       id < static_cast< SparseHistogramType::InstanceIdentifier >(totalSize);
       id++)
    {
    bool result = sparseHistogram->SetFrequency(id, 1);
    if( !result )
      {
      pass = false;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) ";
      break;

      }

    result = sparseHistogram->IncreaseFrequency(id, 1);
    if( !result )
      {
      pass = false;
      whereFail = 
        "IncreaseFrequency(InstanceIdentifier, 1) ";
      break;
      }

    if (sparseHistogram->GetFrequency(id) != 2)
      {
      pass = false;
      whereFail = 
        "SetFrequency(InstanceIdentifier, 1) + IncreaseFrequency(InstanceIdentifier, 1) + GetFrequency(InstanceIdentifier)";
      break;
      }
    }

  if (pass && (sparseHistogram->Quantile(0, 0.5) != 512.0))
    {
    pass = false;
    whereFail = "Sparse Histogram: Quantile(Dimension, percent)";
    }

 
  histogram->SetClipBinsAtEnds( true );
  if( !histogram->GetClipBinsAtEnds() )
    {
    pass = false;
    whereFail = "Set/GetClipBinsAtEnds()";
    }

  histogram->SetClipBinsAtEnds( false );
  if( histogram->GetClipBinsAtEnds() )
    {
    pass = false;
    whereFail = "Set/GetClipBinsAtEnds()";
    }

  if( histogram->GetMeasurementVectorSize() != numberOfComponents )
    {
    pass = false;
    whereFail = "Set/GetMeasurementVectorSize()";
    }

  const unsigned int measurementVectorSize = 17;
  try
    {
    histogram->SetMeasurementVectorSize( measurementVectorSize );
    pass = false;
    whereFail = "SetMeasurementVectorSize() didn't throw expected exception";
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << "Expected exception ";
    std::cout << excp << std::endl;
    }

  index.Fill(0);
  MeasurementVectorType measurement = histogram->GetMeasurementVector( index );
  for( unsigned kid0 = 0; kid0 < numberOfComponents; kid0++ )
    {
    if( measurement[kid0] != 8 )
      {
      std::cerr << "GetMeasurementVector() for index = ";
      std::cerr << index << std::endl;
      pass = false;
      whereFail = "GetMeasurementVector() failed for index";
      break;
      }
    }
  
  histogram->SetClipBinsAtEnds( true );

  measurement = histogram->GetMeasurementVector( index );
  for( unsigned kid1 = 0; kid1 < numberOfComponents; kid1++ )
    {
    if( measurement[kid1] != 8 )
      {
      std::cerr << "GetMeasurementVector() for index = ";
      std::cerr << index << std::endl;
      pass = false;
      whereFail = "GetMeasurementVector() failed for index";
      break;
      }
    }
  
  const InstanceIdentifier instanceId = 0;
  measurement = histogram->GetMeasurementVector( instanceId );
  for( unsigned kid2 = 0; kid2 < numberOfComponents; kid2++ )
    {
    if( measurement[kid2] != 8 )
      {
      std::cerr << "GetMeasurementVector() for instanceId = ";
      std::cerr << instanceId << std::endl;
      pass = false;
      whereFail = "GetMeasurementVector() failed for instanceId";
      break;
      }
    }
 
  // Test GetIndex with different settings of SetClipBinsAtEnds
  MeasurementVectorType outOfLowerRange;
  MeasurementVectorType outOfUpperRange;

  for(unsigned int k = 0; k < numberOfComponents; k++)
    {
    outOfLowerRange[k] = lowerBound[k] - 13;
    outOfUpperRange[k] = upperBound[k] + 23;
    }

  histogram->SetClipBinsAtEnds( false );

  IndexType index1;
  bool getindex1 = histogram->GetIndex( outOfLowerRange, index1 );

  std::cout << "GetIndex() with SetClipBinsAtEnds() = false " << std::endl;
  std::cout << "Boolean " << getindex1 << " Index " << index1 << std::endl;
  
  if( !getindex1 )
    {
    pass = false;
    whereFail = "GetIndex() returned boolean failed for outOfLowerRange";
    }

  for(unsigned k1=0; k1 < numberOfComponents; k1++ )
    {
    if( index1[ k1 ] != 0 )
      {
      pass = false;
      whereFail = "GetIndex() index value failed for outOfLowerRange";
      }
    }

  
  histogram->SetClipBinsAtEnds( true );
  
  getindex1 = histogram->GetIndex( outOfLowerRange, index1 );
  
  std::cout << "GetIndex() with SetClipBinsAtEnds() = true " << std::endl;
  std::cout << "Boolean " << getindex1 << " Index " << index1 << std::endl;

  if( getindex1 )
    {
    pass = false;
    whereFail = "GetIndex() failed for outOfLowerRange";
    }

  histogram->SetClipBinsAtEnds( false );

  IndexType index2;
  bool getindex2 = histogram->GetIndex( outOfUpperRange, index2 );

  std::cout << "GetIndex() with SetClipBinsAtEnds() = false " << std::endl;
  std::cout << "Boolean " << getindex2 << " Index " << index2 << std::endl;
  
  if( !getindex2 )
    {
    pass = false;
    whereFail = "GetIndex() returned boolean failed for outOfUpperRange";
    }

  for(unsigned k2=0; k2 < numberOfComponents; k2++ )
    {
    if( index2[ k2 ] != (long)size[k2] - 1 )
      {
      pass = false;
      whereFail = "GetIndex() index value failed for outOfUpperRange";
      }
    }

  
  histogram->SetClipBinsAtEnds( true );
  
  getindex2 = histogram->GetIndex( outOfUpperRange, index2 );
  
  std::cout << "GetIndex() with SetClipBinsAtEnds() = true " << std::endl;
  std::cout << "Boolean " << getindex2 << " Index " << index2 << std::endl;

  if( getindex2 )
    {
    pass = false;
    whereFail = "GetIndex() failed for outOfUpperRange";
    }


  // Testing GetIndex() for values that are above the median value of the Bin.
  IndexType pindex;
  pindex.Fill( 32 );
  MeasurementVectorType measurementVector =
    histogram->GetMeasurementVector( pindex );

  for( unsigned int gik1=0; gik1<numberOfComponents; gik1++)
    {
    measurementVector[gik1] += 0.3;
    }
  
  IndexType gindex = histogram->GetIndex( measurementVector );

  for( unsigned int gik2=0; gik2<numberOfComponents; gik2++)
    {
    if( gindex[gik2] != 32 )
      {
      std::cerr << "GetIndex() / GetMeasurementVector() failed " << std::endl;
      std::cerr << "MeasurementVector = " << measurementVector << std::endl;
      std::cerr << "Index returned = " << gindex << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Testing GetIndex() for values that are below the median value of the Bin.
  for( unsigned int gik3=0; gik3<numberOfComponents; gik3++)
    {
    measurementVector[gik3] -= 0.6;
    }

  gindex = histogram->GetIndex( measurementVector );

  for( unsigned int gik4=0; gik4<numberOfComponents; gik4++)
    {
    if( gindex[gik4] != 32 )
      {
      std::cerr << "GetIndex() / GetMeasurementVector() failed " << std::endl;
      std::cerr << "MeasurementVector = " << measurementVector << std::endl;
      std::cerr << "Index returned = " << gindex << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Testing GetIndex on the upper and lower bounds
  IndexType upperIndex;
  bool upperIndexBool = histogram->GetIndex( upperBound, upperIndex );
  if( upperIndexBool )
    {
    std::cout << "Upper bound index = " << upperIndex << std::endl;
    }

  IndexType lowerIndex;
  bool lowerIndexBool = histogram->GetIndex( lowerBound, lowerIndex );
  if( lowerIndexBool )
    {
    std::cout << "Upper bound index = " << lowerIndex << std::endl;
    }

  // Testing GetIndex above the upper bound of a bin
  histogram->SetClipBinsAtEnds( false );
  MeasurementVectorType measurementVectorAbove;
  for( unsigned int gupk1 = 0; gupk1<numberOfComponents; gupk1++)
    {
    measurementVectorAbove[gupk1] = 129.9;
    }

  IndexType aboveUpperIndex;
  bool aboveUpperIndexBool =
    histogram->GetIndex( measurementVectorAbove, aboveUpperIndex );
  if( !aboveUpperIndexBool )
    {
    std::cerr << "Upper bound index = " << aboveUpperIndex << std::endl;
    }

  HistogramType::Iterator itr = histogram->Begin();
  HistogramType::Iterator end = histogram->End();

  HistogramType::TotalFrequencyType totalFrequency = 
    histogram->GetTotalFrequency();

  InstanceIdentifier histogramSize = histogram->Size();
  
  while( itr != end )
    {
    itr.SetFrequency( itr.GetFrequency() + 1 );
    ++itr;
    }

  HistogramType::TotalFrequencyType newTotalFrequency =
    histogram->GetTotalFrequency();

  if( newTotalFrequency != histogramSize + totalFrequency )
    {
    std::cerr << "Get/SetFrequency error in the Iterator" << std::endl;
    return EXIT_FAILURE;
    }


  // Testing methods specific to Iterators
  {
  typedef HistogramType::Iterator IteratorType;
  IteratorType iter = histogram->Begin();
  IteratorType iter2;
  iter2 = iter;
  if( iter2 != iter )
    {
    std::cerr << "Iterator operator=() failed" << std::endl;
    return EXIT_FAILURE;
    }

  IteratorType iter3( histogram );
  if( iter3 != histogram->Begin() )
    {
    std::cerr << "Iterator constructor from histogram failed" << std::endl;
    return EXIT_FAILURE;
    }

  unsigned int counter = 0;
  while( iter3 != histogram->End() )
    {
    ++iter3;
    counter++;
    }

  if( counter != histogram->Size() )
    {
    std::cerr << "Iterator walk failed" << std::endl;
    return EXIT_FAILURE;
    }
  }

  // Testing methods specific to ConstIterators
  {
  typedef HistogramType::ConstIterator ConstIteratorType;
  ConstIteratorType iter = histogram->Begin();
  ConstIteratorType iter2;

  iter2 = iter;

  if( iter2 != iter )
    {
    std::cerr << "ConstIterator operator!=() or operator=() failed" << std::endl;
    return EXIT_FAILURE;
    }

  if( !( iter2 == iter ) )
    {
    std::cerr << "ConstIterator operator==() failed" << std::endl;
    return EXIT_FAILURE;
    }

  ConstIteratorType iter3( iter2 ); 
  if( iter3 != iter2 )
    {
    std::cerr << "ConstIterator copy constructor failed" << std::endl;
    return EXIT_FAILURE;
    }

  const HistogramType * constHistogram = histogram.GetPointer();

  ConstIteratorType iter4( constHistogram->Begin() ); 
  ConstIteratorType iter5( histogram->Begin() );
  if( iter4 != iter5 )
    {
    std::cerr << "Constructor from const container Begin() differs from non-const Begin() " << std::endl;
    return EXIT_FAILURE;
    }

  ConstIteratorType iter6( constHistogram ); 
  ConstIteratorType iter7( histogram );
  if( iter6 != iter7 )
    {
    std::cerr << "Constructor from const container differs from non-const container" << std::endl;
    return EXIT_FAILURE;
    }

  ConstIteratorType iter8( 0, histogram );
  if( iter8.GetInstanceIdentifier() != 0 )
    {
    std::cerr << "Constructor with instance identifier 0 failed" << std::endl;
    return EXIT_FAILURE;
    }

  ConstIteratorType iter9( 7, histogram );
  if( iter9.GetInstanceIdentifier() != 7 )
    {
    std::cerr << "Constructor with instance identifier 7 failed" << std::endl;
    return EXIT_FAILURE;
    }

  MeasurementVectorType vector9a = iter9.GetMeasurementVector();
  MeasurementVectorType vector9b = histogram->GetMeasurementVector( 7 );
  for( unsigned int kitr =0; kitr < numberOfComponents; kitr++ )
    {
    if( vnl_math_abs( vector9b[kitr] - vector9a[kitr] ) )
      {
      std::cerr << "Constructor with instance identifier 8 failed" << std::endl;
      return EXIT_FAILURE;
      }
    }

  unsigned int counter = 0;
  ConstIteratorType iter10( constHistogram );
  if( iter10 != constHistogram->Begin() )
    {
    std::cerr << "ConstIterator constructor from histogram failed" << std::endl;
    return EXIT_FAILURE;
    }


  while( iter10 != constHistogram->End() )
    {
    ++iter10;
    counter++;
    }

  if( counter != constHistogram->Size() )
    {
    std::cerr << "Iterator walk failed" << std::endl;
    return EXIT_FAILURE;
    }

  }
  

  if( !pass )
    {
    std::cout << "Test failed in " << whereFail << "." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



