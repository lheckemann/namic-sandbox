
#ifndef __MultiThreadMIMetricTestImplementation_h
#define __MultiThreadMIMetricTestImplementation_h

#include "itkFixedArray.h"

#include <string>

//template <class TPixel, unsigned int VImageDimension=2>

template <unsigned int Dimension = 3>
class MultiThreadMIMetricTestImplementation
{
public:
  typedef typename itk::FixedArray<unsigned int, Dimension> SizeType;

  MultiThreadMIMetricTestImplementation();

  bool RunTest();

  void SetNumberOfIterations( unsigned int iter );
  unsigned int GetNumberOfIterations();

  void SetImageSize( SizeType& size );
  void SetBSplineGridOnImageSize( SizeType& size );
  void SetNumberOfSamples( unsigned long samples );
  void SetUseThreading( bool threading );
  void CreateTimingOutputFileName();
  void CreateMemoryOutputFileName();
  void SetTimingOutputFileName( const char* );
  void SetMemoryOutputFileName( const char* );
  void SetDerivativeOutputFileName( const char* );

protected:
  std::string m_OutputFileName;
  std::string m_MemOutputFileName;
  std::string m_DerivativeOutputFileName;

  bool        m_UseThreading;

  SizeType m_ImageSize;
  SizeType m_BSplineSize;

  unsigned int m_NumberOfIterations;
  unsigned long m_NumberOfSamples;

};

#include "MultiThreadMIMetricTestImplementation.txx"

#endif // MultiThreadMIMetricTestImplementation
