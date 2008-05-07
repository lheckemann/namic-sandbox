/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNumericTraitsFixedArrayPixel.h,v $
  Language:  C++
  Date:      $Date: 2007/09/19 22:32:47 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNumericTraitsFixedArrayPixel_h
#define __itkNumericTraitsFixedArrayPixel_h

#include "itkNumericTraits.h"
#include "itkFixedArray.h"

// This file defines numeric traits for vector pixels types in itk
// TODO: Add doxygen tags..

namespace itk
{
template <> class NumericTraits<FixedArray<unsigned char, 2 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 2> PrintType;
  typedef FixedArray<unsigned char, 2> AbsType;
  typedef FixedArray<unsigned short, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<unsigned char, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 2> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 2> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 2> PrintType;
  typedef FixedArray<unsigned char, 2> AbsType;
  typedef FixedArray<short, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<signed char, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 2> ITKCommon_EXPORT One;
  static FixedArray<signed char,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 2> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 2> PrintType;
  typedef FixedArray<unsigned char, 2> AbsType;
  typedef FixedArray<short, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<char, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 2> ITKCommon_EXPORT One;
  static FixedArray<char,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 2> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 2> PrintType;
  typedef FixedArray<unsigned short, 2> AbsType;
  typedef FixedArray<int, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<short, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 2> ITKCommon_EXPORT One;
  static FixedArray<short,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 2> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 2> PrintType;
  typedef FixedArray<unsigned short, 2> AbsType;
  typedef FixedArray<unsigned int, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<unsigned short, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 2> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 2> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 2> PrintType;
  typedef FixedArray<unsigned int, 2> AbsType;
  typedef FixedArray<long, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<int, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 2> ITKCommon_EXPORT One;
  static FixedArray<int,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 2> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 2> PrintType;
  typedef FixedArray<unsigned int, 2> AbsType;
  typedef FixedArray<unsigned long, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<unsigned int, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 2> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 2> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 2> PrintType;
  typedef FixedArray<unsigned long, 2> AbsType;
  typedef FixedArray<long, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<long, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 2> ITKCommon_EXPORT One;
  static FixedArray<long,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 2> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 2> PrintType;
  typedef FixedArray<unsigned long, 2> AbsType;
  typedef FixedArray<unsigned long, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<unsigned long, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 2> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 2> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 2> PrintType;
  typedef FixedArray<float, 2> AbsType;
  typedef FixedArray<double, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<float, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 2> ITKCommon_EXPORT One;
  static FixedArray<float, 2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 2> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 2> PrintType;
  typedef FixedArray<double, 2> AbsType;
  typedef FixedArray<long double, 2> AccumulateType;
  typedef FixedArray<double, 2> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<double, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 2> ITKCommon_EXPORT One;
  static FixedArray<double, 2>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 2> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 2> PrintType;
  typedef FixedArray<long double, 2> AbsType;
  typedef FixedArray<long double, 2> AccumulateType;
  typedef FixedArray<long double, 2> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 2> FloatType;
  static const FixedArray<long double, 2> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 2> ITKCommon_EXPORT One;
  static FixedArray<long double, 2>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 3 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 3> PrintType;
  typedef FixedArray<unsigned char, 3> AbsType;
  typedef FixedArray<unsigned short, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<unsigned char, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 3> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 3> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 3> PrintType;
  typedef FixedArray<unsigned char, 3> AbsType;
  typedef FixedArray<short, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<signed char, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 3> ITKCommon_EXPORT One;
  static FixedArray<signed char,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 3> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 3> PrintType;
  typedef FixedArray<unsigned char, 3> AbsType;
  typedef FixedArray<short, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<char, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 3> ITKCommon_EXPORT One;
  static FixedArray<char,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 3> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 3> PrintType;
  typedef FixedArray<unsigned short, 3> AbsType;
  typedef FixedArray<int, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<short, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 3> ITKCommon_EXPORT One;
  static FixedArray<short,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 3> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 3> PrintType;
  typedef FixedArray<unsigned short, 3> AbsType;
  typedef FixedArray<unsigned int, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<unsigned short, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 3> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 3> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 3> PrintType;
  typedef FixedArray<unsigned int, 3> AbsType;
  typedef FixedArray<long, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<int, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 3> ITKCommon_EXPORT One;
  static FixedArray<int,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 3> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 3> PrintType;
  typedef FixedArray<unsigned int, 3> AbsType;
  typedef FixedArray<unsigned long, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<unsigned int, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 3> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 3> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 3> PrintType;
  typedef FixedArray<unsigned long, 3> AbsType;
  typedef FixedArray<long, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<long, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 3> ITKCommon_EXPORT One;
  static FixedArray<long,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 3> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 3> PrintType;
  typedef FixedArray<unsigned long, 3> AbsType;
  typedef FixedArray<unsigned long, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<unsigned long, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 3> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 3> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 3> PrintType;
  typedef FixedArray<float, 3> AbsType;
  typedef FixedArray<double, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<float, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 3> ITKCommon_EXPORT One;
  static FixedArray<float, 3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 3> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 3> PrintType;
  typedef FixedArray<double, 3> AbsType;
  typedef FixedArray<long double, 3> AccumulateType;
  typedef FixedArray<double, 3> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<double, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 3> ITKCommon_EXPORT One;
  static FixedArray<double, 3>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 3> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 3> PrintType;
  typedef FixedArray<long double, 3> AbsType;
  typedef FixedArray<long double, 3> AccumulateType;
  typedef FixedArray<long double, 3> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 3> FloatType;
  static const FixedArray<long double, 3> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 3> ITKCommon_EXPORT One;
  static FixedArray<long double, 3>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 4 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 4> PrintType;
  typedef FixedArray<unsigned char, 4> AbsType;
  typedef FixedArray<unsigned short, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<unsigned char, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 4> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 4> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 4> PrintType;
  typedef FixedArray<unsigned char, 4> AbsType;
  typedef FixedArray<short, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<signed char, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 4> ITKCommon_EXPORT One;
  static FixedArray<signed char,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 4> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 4> PrintType;
  typedef FixedArray<unsigned char, 4> AbsType;
  typedef FixedArray<short, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<char, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 4> ITKCommon_EXPORT One;
  static FixedArray<char,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 4> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 4> PrintType;
  typedef FixedArray<unsigned short, 4> AbsType;
  typedef FixedArray<int, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<short, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 4> ITKCommon_EXPORT One;
  static FixedArray<short,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 4> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 4> PrintType;
  typedef FixedArray<unsigned short, 4> AbsType;
  typedef FixedArray<unsigned int, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<unsigned short, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 4> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 4> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 4> PrintType;
  typedef FixedArray<unsigned int, 4> AbsType;
  typedef FixedArray<long, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<int, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 4> ITKCommon_EXPORT One;
  static FixedArray<int,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 4> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 4> PrintType;
  typedef FixedArray<unsigned int, 4> AbsType;
  typedef FixedArray<unsigned long, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<unsigned int, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 4> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 4> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 4> PrintType;
  typedef FixedArray<unsigned long, 4> AbsType;
  typedef FixedArray<long, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<long, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 4> ITKCommon_EXPORT One;
  static FixedArray<long,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 4> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 4> PrintType;
  typedef FixedArray<unsigned long, 4> AbsType;
  typedef FixedArray<unsigned long, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<unsigned long, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 4> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 4> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 4> PrintType;
  typedef FixedArray<float, 4> AbsType;
  typedef FixedArray<double, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<float, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 4> ITKCommon_EXPORT One;
  static FixedArray<float, 4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 4> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 4> PrintType;
  typedef FixedArray<double, 4> AbsType;
  typedef FixedArray<long double, 4> AccumulateType;
  typedef FixedArray<double, 4> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<double, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 4> ITKCommon_EXPORT One;
  static FixedArray<double, 4>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 4> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 4> PrintType;
  typedef FixedArray<long double, 4> AbsType;
  typedef FixedArray<long double, 4> AccumulateType;
  typedef FixedArray<long double, 4> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 4> FloatType;
  static const FixedArray<long double, 4> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 4> ITKCommon_EXPORT One;
  static FixedArray<long double, 4>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 5 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 5> PrintType;
  typedef FixedArray<unsigned char, 5> AbsType;
  typedef FixedArray<unsigned short, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<unsigned char, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 5> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 5> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 5> PrintType;
  typedef FixedArray<unsigned char, 5> AbsType;
  typedef FixedArray<short, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<signed char, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 5> ITKCommon_EXPORT One;
  static FixedArray<signed char,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 5> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 5> PrintType;
  typedef FixedArray<unsigned char, 5> AbsType;
  typedef FixedArray<short, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<char, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 5> ITKCommon_EXPORT One;
  static FixedArray<char,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 5> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 5> PrintType;
  typedef FixedArray<unsigned short, 5> AbsType;
  typedef FixedArray<int, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<short, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 5> ITKCommon_EXPORT One;
  static FixedArray<short,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 5> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 5> PrintType;
  typedef FixedArray<unsigned short, 5> AbsType;
  typedef FixedArray<unsigned int, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<unsigned short, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 5> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 5> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 5> PrintType;
  typedef FixedArray<unsigned int, 5> AbsType;
  typedef FixedArray<long, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<int, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 5> ITKCommon_EXPORT One;
  static FixedArray<int,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 5> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 5> PrintType;
  typedef FixedArray<unsigned int, 5> AbsType;
  typedef FixedArray<unsigned long, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<unsigned int, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 5> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 5> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 5> PrintType;
  typedef FixedArray<unsigned long, 5> AbsType;
  typedef FixedArray<long, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<long, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 5> ITKCommon_EXPORT One;
  static FixedArray<long,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 5> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 5> PrintType;
  typedef FixedArray<unsigned long, 5> AbsType;
  typedef FixedArray<unsigned long, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<unsigned long, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 5> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 5> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 5> PrintType;
  typedef FixedArray<float, 5> AbsType;
  typedef FixedArray<double, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<float, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 5> ITKCommon_EXPORT One;
  static FixedArray<float, 5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 5> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 5> PrintType;
  typedef FixedArray<double, 5> AbsType;
  typedef FixedArray<long double, 5> AccumulateType;
  typedef FixedArray<double, 5> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<double, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 5> ITKCommon_EXPORT One;
  static FixedArray<double, 5>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 5> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 5> PrintType;
  typedef FixedArray<long double, 5> AbsType;
  typedef FixedArray<long double, 5> AccumulateType;
  typedef FixedArray<long double, 5> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 5> FloatType;
  static const FixedArray<long double, 5> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 5> ITKCommon_EXPORT One;
  static FixedArray<long double, 5>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 6 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 6> PrintType;
  typedef FixedArray<unsigned char, 6> AbsType;
  typedef FixedArray<unsigned short, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<unsigned char, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 6> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 6> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 6> PrintType;
  typedef FixedArray<unsigned char, 6> AbsType;
  typedef FixedArray<short, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<signed char, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 6> ITKCommon_EXPORT One;
  static FixedArray<signed char,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 6> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 6> PrintType;
  typedef FixedArray<unsigned char, 6> AbsType;
  typedef FixedArray<short, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<char, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 6> ITKCommon_EXPORT One;
  static FixedArray<char,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 6> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 6> PrintType;
  typedef FixedArray<unsigned short, 6> AbsType;
  typedef FixedArray<int, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<short, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 6> ITKCommon_EXPORT One;
  static FixedArray<short,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 6> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 6> PrintType;
  typedef FixedArray<unsigned short, 6> AbsType;
  typedef FixedArray<unsigned int, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<unsigned short, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 6> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 6> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 6> PrintType;
  typedef FixedArray<unsigned int, 6> AbsType;
  typedef FixedArray<long, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<int, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 6> ITKCommon_EXPORT One;
  static FixedArray<int,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 6> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 6> PrintType;
  typedef FixedArray<unsigned int, 6> AbsType;
  typedef FixedArray<unsigned long, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<unsigned int, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 6> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 6> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 6> PrintType;
  typedef FixedArray<unsigned long, 6> AbsType;
  typedef FixedArray<long, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<long, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 6> ITKCommon_EXPORT One;
  static FixedArray<long,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 6> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 6> PrintType;
  typedef FixedArray<unsigned long, 6> AbsType;
  typedef FixedArray<unsigned long, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<unsigned long, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 6> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 6> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 6> PrintType;
  typedef FixedArray<float, 6> AbsType;
  typedef FixedArray<double, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<float, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 6> ITKCommon_EXPORT One;
  static FixedArray<float, 6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 6> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 6> PrintType;
  typedef FixedArray<double, 6> AbsType;
  typedef FixedArray<long double, 6> AccumulateType;
  typedef FixedArray<double, 6> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<double, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 6> ITKCommon_EXPORT One;
  static FixedArray<double, 6>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 6> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 6> PrintType;
  typedef FixedArray<long double, 6> AbsType;
  typedef FixedArray<long double, 6> AccumulateType;
  typedef FixedArray<long double, 6> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 6> FloatType;
  static const FixedArray<long double, 6> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 6> ITKCommon_EXPORT One;
  static FixedArray<long double, 6>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 7 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 7> PrintType;
  typedef FixedArray<unsigned char, 7> AbsType;
  typedef FixedArray<unsigned short, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<unsigned char, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 7> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 7> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 7> PrintType;
  typedef FixedArray<unsigned char, 7> AbsType;
  typedef FixedArray<short, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<signed char, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 7> ITKCommon_EXPORT One;
  static FixedArray<signed char,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 7> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 7> PrintType;
  typedef FixedArray<unsigned char, 7> AbsType;
  typedef FixedArray<short, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<char, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 7> ITKCommon_EXPORT One;
  static FixedArray<char,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 7> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 7> PrintType;
  typedef FixedArray<unsigned short, 7> AbsType;
  typedef FixedArray<int, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<short, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 7> ITKCommon_EXPORT One;
  static FixedArray<short,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 7> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 7> PrintType;
  typedef FixedArray<unsigned short, 7> AbsType;
  typedef FixedArray<unsigned int, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<unsigned short, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 7> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 7> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 7> PrintType;
  typedef FixedArray<unsigned int, 7> AbsType;
  typedef FixedArray<long, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<int, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 7> ITKCommon_EXPORT One;
  static FixedArray<int,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 7> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 7> PrintType;
  typedef FixedArray<unsigned int, 7> AbsType;
  typedef FixedArray<unsigned long, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<unsigned int, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 7> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 7> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 7> PrintType;
  typedef FixedArray<unsigned long, 7> AbsType;
  typedef FixedArray<long, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<long, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 7> ITKCommon_EXPORT One;
  static FixedArray<long,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 7> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 7> PrintType;
  typedef FixedArray<unsigned long, 7> AbsType;
  typedef FixedArray<unsigned long, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<unsigned long, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 7> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 7> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 7> PrintType;
  typedef FixedArray<float, 7> AbsType;
  typedef FixedArray<double, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<float, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 7> ITKCommon_EXPORT One;
  static FixedArray<float, 7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 7> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 7> PrintType;
  typedef FixedArray<double, 7> AbsType;
  typedef FixedArray<long double, 7> AccumulateType;
  typedef FixedArray<double, 7> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<double, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 7> ITKCommon_EXPORT One;
  static FixedArray<double, 7>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 7> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 7> PrintType;
  typedef FixedArray<long double, 7> AbsType;
  typedef FixedArray<long double, 7> AccumulateType;
  typedef FixedArray<long double, 7> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 7> FloatType;
  static const FixedArray<long double, 7> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 7> ITKCommon_EXPORT One;
  static FixedArray<long double, 7>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 8 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 8> PrintType;
  typedef FixedArray<unsigned char, 8> AbsType;
  typedef FixedArray<unsigned short, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<unsigned char, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 8> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 8> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 8> PrintType;
  typedef FixedArray<unsigned char, 8> AbsType;
  typedef FixedArray<short, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<signed char, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 8> ITKCommon_EXPORT One;
  static FixedArray<signed char,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 8> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 8> PrintType;
  typedef FixedArray<unsigned char, 8> AbsType;
  typedef FixedArray<short, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<char, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 8> ITKCommon_EXPORT One;
  static FixedArray<char,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 8> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 8> PrintType;
  typedef FixedArray<unsigned short, 8> AbsType;
  typedef FixedArray<int, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<short, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 8> ITKCommon_EXPORT One;
  static FixedArray<short,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 8> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 8> PrintType;
  typedef FixedArray<unsigned short, 8> AbsType;
  typedef FixedArray<unsigned int, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<unsigned short, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 8> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 8> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 8> PrintType;
  typedef FixedArray<unsigned int, 8> AbsType;
  typedef FixedArray<long, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<int, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 8> ITKCommon_EXPORT One;
  static FixedArray<int,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 8> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 8> PrintType;
  typedef FixedArray<unsigned int, 8> AbsType;
  typedef FixedArray<unsigned long, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<unsigned int, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 8> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 8> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 8> PrintType;
  typedef FixedArray<unsigned long, 8> AbsType;
  typedef FixedArray<long, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<long, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 8> ITKCommon_EXPORT One;
  static FixedArray<long,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 8> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 8> PrintType;
  typedef FixedArray<unsigned long, 8> AbsType;
  typedef FixedArray<unsigned long, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<unsigned long, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 8> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 8> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 8> PrintType;
  typedef FixedArray<float, 8> AbsType;
  typedef FixedArray<double, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<float, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 8> ITKCommon_EXPORT One;
  static FixedArray<float, 8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 8> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 8> PrintType;
  typedef FixedArray<double, 8> AbsType;
  typedef FixedArray<long double, 8> AccumulateType;
  typedef FixedArray<double, 8> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<double, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 8> ITKCommon_EXPORT One;
  static FixedArray<double, 8>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 8> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 8> PrintType;
  typedef FixedArray<long double, 8> AbsType;
  typedef FixedArray<long double, 8> AccumulateType;
  typedef FixedArray<long double, 8> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 8> FloatType;
  static const FixedArray<long double, 8> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 8> ITKCommon_EXPORT One;
  static FixedArray<long double, 8>  ZeroValue() { return Zero; }
};

template <> class NumericTraits<FixedArray<unsigned char, 9 > > {
public:
  typedef unsigned char ValueType;
  typedef FixedArray<unsigned char, 9> PrintType;
  typedef FixedArray<unsigned char, 9> AbsType;
  typedef FixedArray<unsigned short, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<unsigned char, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned char, 9> ITKCommon_EXPORT One;
  static FixedArray<unsigned char,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<signed char, 9> > {
public:
  typedef signed char ValueType;
  typedef FixedArray<signed char, 9> PrintType;
  typedef FixedArray<unsigned char, 9> AbsType;
  typedef FixedArray<short, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<signed char, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<signed char, 9> ITKCommon_EXPORT One;
  static FixedArray<signed char,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<char, 9> > {
public:
  typedef char ValueType;
  typedef FixedArray<char, 9> PrintType;
  typedef FixedArray<unsigned char, 9> AbsType;
  typedef FixedArray<short, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<char, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<char, 9> ITKCommon_EXPORT One;
  static FixedArray<char,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<short, 9> > {
public:
  typedef short ValueType;
  typedef FixedArray<short, 9> PrintType;
  typedef FixedArray<unsigned short, 9> AbsType;
  typedef FixedArray<int, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<short, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<short, 9> ITKCommon_EXPORT One;
  static FixedArray<short,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned short, 9> > {
public:
  typedef unsigned short ValueType;
  typedef FixedArray<unsigned short, 9> PrintType;
  typedef FixedArray<unsigned short, 9> AbsType;
  typedef FixedArray<unsigned int, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<unsigned short, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned short, 9> ITKCommon_EXPORT One;
  static FixedArray<unsigned short,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<int, 9> > {
public:
  typedef int ValueType;
  typedef FixedArray<int, 9> PrintType;
  typedef FixedArray<unsigned int, 9> AbsType;
  typedef FixedArray<long, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<int, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<int, 9> ITKCommon_EXPORT One;
  static FixedArray<int,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned int, 9> > {
public:
  typedef unsigned int ValueType;
  typedef FixedArray<unsigned int, 9> PrintType;
  typedef FixedArray<unsigned int, 9> AbsType;
  typedef FixedArray<unsigned long, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<unsigned int, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned int, 9> ITKCommon_EXPORT One;
  static FixedArray<unsigned int,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long, 9> > {
public:
  typedef long ValueType;
  typedef FixedArray<long, 9> PrintType;
  typedef FixedArray<unsigned long, 9> AbsType;
  typedef FixedArray<long, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<long, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<long, 9> ITKCommon_EXPORT One;
  static FixedArray<long,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<unsigned long, 9> > {
public:
  typedef unsigned long ValueType;
  typedef FixedArray<unsigned long, 9> PrintType;
  typedef FixedArray<unsigned long, 9> AbsType;
  typedef FixedArray<unsigned long, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<unsigned long, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<unsigned long, 9> ITKCommon_EXPORT One;
  static FixedArray<unsigned long,9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<float, 9> > {
public:
  typedef float ValueType;
  typedef FixedArray<float, 9> PrintType;
  typedef FixedArray<float, 9> AbsType;
  typedef FixedArray<double, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<float, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<float, 9> ITKCommon_EXPORT One;
  static FixedArray<float, 9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<double, 9> > {
public:
  typedef double ValueType;
  typedef FixedArray<double, 9> PrintType;
  typedef FixedArray<double, 9> AbsType;
  typedef FixedArray<long double, 9> AccumulateType;
  typedef FixedArray<double, 9> RealType;
  typedef double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<double, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<double, 9> ITKCommon_EXPORT One;
  static FixedArray<double, 9>  ZeroValue() { return Zero; }
};
template <> class NumericTraits<FixedArray<long double, 9> > {
public:
  typedef long double ValueType;
  typedef FixedArray<long double, 9> PrintType;
  typedef FixedArray<long double, 9> AbsType;
  typedef FixedArray<long double, 9> AccumulateType;
  typedef FixedArray<long double, 9> RealType;
  typedef long double ScalarRealType;
  typedef FixedArray<float, 9> FloatType;
  static const FixedArray<long double, 9> ITKCommon_EXPORT Zero;
  static const FixedArray<long double, 9> ITKCommon_EXPORT One;
  static FixedArray<long double, 9>  ZeroValue() { return Zero; }
};

} // end namespace itk

#endif // __itkNumericTraitsFixedArrayPixel_h  

