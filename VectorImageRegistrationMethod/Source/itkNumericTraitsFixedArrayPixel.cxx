/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNumericTraitsFixedArrayPixel.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/05 20:31:29 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkNumericTraitsFixedArrayPixel.h"

namespace itk
{

const FixedArray<unsigned char, 2>  NumericTraits<FixedArray<unsigned char, 2> >::Zero = FixedArray<unsigned char, 2>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 2>  NumericTraits<FixedArray<unsigned char, 2> >::One = FixedArray<unsigned char, 2>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 2>  NumericTraits<FixedArray<signed char, 2> >::Zero = FixedArray<signed char, 2>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 2>  NumericTraits<FixedArray<signed char, 2> >::One = FixedArray<signed char, 2>( NumericTraits<signed char>::One );
const FixedArray<char, 2>  NumericTraits<FixedArray<char, 2> >::Zero = FixedArray<char, 2>( NumericTraits<char>::Zero );
const FixedArray<char, 2>  NumericTraits<FixedArray<char, 2> >::One = FixedArray<char, 2>( NumericTraits<char>::One );
const FixedArray<short, 2>  NumericTraits<FixedArray<short, 2> >::Zero = FixedArray<short, 2>( NumericTraits<short>::Zero );
const FixedArray<short, 2>  NumericTraits<FixedArray<short, 2> >::One = FixedArray<short, 2>( NumericTraits<short>::One );
const FixedArray<unsigned short, 2>  NumericTraits<FixedArray<unsigned short, 2> >::Zero = FixedArray<unsigned short, 2>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 2>  NumericTraits<FixedArray<unsigned short, 2> >::One = FixedArray<unsigned short, 2>( NumericTraits<unsigned short>::One );
const FixedArray<int, 2>  NumericTraits<FixedArray<int, 2> >::Zero = FixedArray<int, 2>( NumericTraits<int>::Zero );
const FixedArray<int, 2>  NumericTraits<FixedArray<int, 2> >::One = FixedArray<int, 2>( NumericTraits<int>::One );
const FixedArray<unsigned int, 2>  NumericTraits<FixedArray<unsigned int, 2> >::Zero = FixedArray<unsigned int, 2>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 2>  NumericTraits<FixedArray<unsigned int, 2> >::One = FixedArray<unsigned int, 2>( NumericTraits<unsigned int>::One );
const FixedArray<long, 2>  NumericTraits<FixedArray<long, 2> >::Zero = FixedArray<long, 2>( NumericTraits<long>::Zero );
const FixedArray<long, 2>  NumericTraits<FixedArray<long, 2> >::One = FixedArray<long, 2>( NumericTraits<long>::One );
const FixedArray<unsigned long, 2>  NumericTraits<FixedArray<unsigned long, 2> >::Zero = FixedArray<unsigned long, 2>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 2>  NumericTraits<FixedArray<unsigned long, 2> >::One = FixedArray<unsigned long, 2>( NumericTraits<unsigned long>::One );
const FixedArray<float, 2>  NumericTraits<FixedArray<float, 2> >::Zero = FixedArray<float, 2>( NumericTraits<float >::Zero );
const FixedArray<float, 2>  NumericTraits<FixedArray<float, 2> >::One = FixedArray<float, 2>( NumericTraits<float>::One );
const FixedArray<double, 2>  NumericTraits<FixedArray<double, 2> >::Zero = FixedArray<double , 2>( NumericTraits<double>::Zero );
const FixedArray<double, 2>  NumericTraits<FixedArray<double, 2> >::One = FixedArray<double, 2>( NumericTraits<double>::One );
const FixedArray<long double, 2>  NumericTraits<FixedArray<long double, 2> >::Zero = FixedArray<long double, 2>( NumericTraits<long double>::Zero );
const FixedArray<long double, 2>  NumericTraits<FixedArray<long double, 2> >::One = FixedArray<long double, 2>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 3>  NumericTraits<FixedArray<unsigned char, 3> >::Zero = FixedArray<unsigned char, 3>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 3>  NumericTraits<FixedArray<unsigned char, 3> >::One = FixedArray<unsigned char, 3>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 3>  NumericTraits<FixedArray<signed char, 3> >::Zero = FixedArray<signed char, 3>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 3>  NumericTraits<FixedArray<signed char, 3> >::One = FixedArray<signed char, 3>( NumericTraits<signed char>::One );
const FixedArray<char, 3>  NumericTraits<FixedArray<char, 3> >::Zero = FixedArray<char, 3>( NumericTraits<char>::Zero );
const FixedArray<char, 3>  NumericTraits<FixedArray<char, 3> >::One = FixedArray<char, 3>( NumericTraits<char>::One );
const FixedArray<short, 3>  NumericTraits<FixedArray<short, 3> >::Zero = FixedArray<short, 3>( NumericTraits<short>::Zero );
const FixedArray<short, 3>  NumericTraits<FixedArray<short, 3> >::One = FixedArray<short, 3>( NumericTraits<short>::One );
const FixedArray<unsigned short, 3>  NumericTraits<FixedArray<unsigned short, 3> >::Zero = FixedArray<unsigned short, 3>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 3>  NumericTraits<FixedArray<unsigned short, 3> >::One = FixedArray<unsigned short, 3>( NumericTraits<unsigned short>::One );
const FixedArray<int, 3>  NumericTraits<FixedArray<int, 3> >::Zero = FixedArray<int, 3>( NumericTraits<int>::Zero );
const FixedArray<int, 3>  NumericTraits<FixedArray<int, 3> >::One = FixedArray<int, 3>( NumericTraits<int>::One );
const FixedArray<unsigned int, 3>  NumericTraits<FixedArray<unsigned int, 3> >::Zero = FixedArray<unsigned int, 3>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 3>  NumericTraits<FixedArray<unsigned int, 3> >::One = FixedArray<unsigned int, 3>( NumericTraits<unsigned int>::One );
const FixedArray<long, 3>  NumericTraits<FixedArray<long, 3> >::Zero = FixedArray<long, 3>( NumericTraits<long>::Zero );
const FixedArray<long, 3>  NumericTraits<FixedArray<long, 3> >::One = FixedArray<long, 3>( NumericTraits<long>::One );
const FixedArray<unsigned long, 3>  NumericTraits<FixedArray<unsigned long, 3> >::Zero = FixedArray<unsigned long, 3>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 3>  NumericTraits<FixedArray<unsigned long, 3> >::One = FixedArray<unsigned long, 3>( NumericTraits<unsigned long>::One );
const FixedArray<float, 3>  NumericTraits<FixedArray<float, 3> >::Zero = FixedArray<float, 3>( NumericTraits<float >::Zero );
const FixedArray<float, 3>  NumericTraits<FixedArray<float, 3> >::One = FixedArray<float, 3>( NumericTraits<float>::One );
const FixedArray<double, 3>  NumericTraits<FixedArray<double, 3> >::Zero = FixedArray<double , 3>( NumericTraits<double>::Zero );
const FixedArray<double, 3>  NumericTraits<FixedArray<double, 3> >::One = FixedArray<double, 3>( NumericTraits<double>::One );
const FixedArray<long double, 3>  NumericTraits<FixedArray<long double, 3> >::Zero = FixedArray<long double, 3>( NumericTraits<long double>::Zero );
const FixedArray<long double, 3>  NumericTraits<FixedArray<long double, 3> >::One = FixedArray<long double, 3>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 4>  NumericTraits<FixedArray<unsigned char, 4> >::Zero = FixedArray<unsigned char, 4>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 4>  NumericTraits<FixedArray<unsigned char, 4> >::One = FixedArray<unsigned char, 4>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 4>  NumericTraits<FixedArray<signed char, 4> >::Zero = FixedArray<signed char, 4>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 4>  NumericTraits<FixedArray<signed char, 4> >::One = FixedArray<signed char, 4>( NumericTraits<signed char>::One );
const FixedArray<char, 4>  NumericTraits<FixedArray<char, 4> >::Zero = FixedArray<char, 4>( NumericTraits<char>::Zero );
const FixedArray<char, 4>  NumericTraits<FixedArray<char, 4> >::One = FixedArray<char, 4>( NumericTraits<char>::One );
const FixedArray<short, 4>  NumericTraits<FixedArray<short, 4> >::Zero = FixedArray<short, 4>( NumericTraits<short>::Zero );
const FixedArray<short, 4>  NumericTraits<FixedArray<short, 4> >::One = FixedArray<short, 4>( NumericTraits<short>::One );
const FixedArray<unsigned short, 4>  NumericTraits<FixedArray<unsigned short, 4> >::Zero = FixedArray<unsigned short, 4>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 4>  NumericTraits<FixedArray<unsigned short, 4> >::One = FixedArray<unsigned short, 4>( NumericTraits<unsigned short>::One );
const FixedArray<int, 4>  NumericTraits<FixedArray<int, 4> >::Zero = FixedArray<int, 4>( NumericTraits<int>::Zero );
const FixedArray<int, 4>  NumericTraits<FixedArray<int, 4> >::One = FixedArray<int, 4>( NumericTraits<int>::One );
const FixedArray<unsigned int, 4>  NumericTraits<FixedArray<unsigned int, 4> >::Zero = FixedArray<unsigned int, 4>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 4>  NumericTraits<FixedArray<unsigned int, 4> >::One = FixedArray<unsigned int, 4>( NumericTraits<unsigned int>::One );
const FixedArray<long, 4>  NumericTraits<FixedArray<long, 4> >::Zero = FixedArray<long, 4>( NumericTraits<long>::Zero );
const FixedArray<long, 4>  NumericTraits<FixedArray<long, 4> >::One = FixedArray<long, 4>( NumericTraits<long>::One );
const FixedArray<unsigned long, 4>  NumericTraits<FixedArray<unsigned long, 4> >::Zero = FixedArray<unsigned long, 4>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 4>  NumericTraits<FixedArray<unsigned long, 4> >::One = FixedArray<unsigned long, 4>( NumericTraits<unsigned long>::One );
const FixedArray<float, 4>  NumericTraits<FixedArray<float, 4> >::Zero = FixedArray<float, 4>( NumericTraits<float >::Zero );
const FixedArray<float, 4>  NumericTraits<FixedArray<float, 4> >::One = FixedArray<float, 4>( NumericTraits<float>::One );
const FixedArray<double, 4>  NumericTraits<FixedArray<double, 4> >::Zero = FixedArray<double , 4>( NumericTraits<double>::Zero );
const FixedArray<double, 4>  NumericTraits<FixedArray<double, 4> >::One = FixedArray<double, 4>( NumericTraits<double>::One );
const FixedArray<long double, 4>  NumericTraits<FixedArray<long double, 4> >::Zero = FixedArray<long double, 4>( NumericTraits<long double>::Zero );
const FixedArray<long double, 4>  NumericTraits<FixedArray<long double, 4> >::One = FixedArray<long double, 4>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 5>  NumericTraits<FixedArray<unsigned char, 5> >::Zero = FixedArray<unsigned char, 5>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 5>  NumericTraits<FixedArray<unsigned char, 5> >::One = FixedArray<unsigned char, 5>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 5>  NumericTraits<FixedArray<signed char, 5> >::Zero = FixedArray<signed char, 5>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 5>  NumericTraits<FixedArray<signed char, 5> >::One = FixedArray<signed char, 5>( NumericTraits<signed char>::One );
const FixedArray<char, 5>  NumericTraits<FixedArray<char, 5> >::Zero = FixedArray<char, 5>( NumericTraits<char>::Zero );
const FixedArray<char, 5>  NumericTraits<FixedArray<char, 5> >::One = FixedArray<char, 5>( NumericTraits<char>::One );
const FixedArray<short, 5>  NumericTraits<FixedArray<short, 5> >::Zero = FixedArray<short, 5>( NumericTraits<short>::Zero );
const FixedArray<short, 5>  NumericTraits<FixedArray<short, 5> >::One = FixedArray<short, 5>( NumericTraits<short>::One );
const FixedArray<unsigned short, 5>  NumericTraits<FixedArray<unsigned short, 5> >::Zero = FixedArray<unsigned short, 5>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 5>  NumericTraits<FixedArray<unsigned short, 5> >::One = FixedArray<unsigned short, 5>( NumericTraits<unsigned short>::One );
const FixedArray<int, 5>  NumericTraits<FixedArray<int, 5> >::Zero = FixedArray<int, 5>( NumericTraits<int>::Zero );
const FixedArray<int, 5>  NumericTraits<FixedArray<int, 5> >::One = FixedArray<int, 5>( NumericTraits<int>::One );
const FixedArray<unsigned int, 5>  NumericTraits<FixedArray<unsigned int, 5> >::Zero = FixedArray<unsigned int, 5>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 5>  NumericTraits<FixedArray<unsigned int, 5> >::One = FixedArray<unsigned int, 5>( NumericTraits<unsigned int>::One );
const FixedArray<long, 5>  NumericTraits<FixedArray<long, 5> >::Zero = FixedArray<long, 5>( NumericTraits<long>::Zero );
const FixedArray<long, 5>  NumericTraits<FixedArray<long, 5> >::One = FixedArray<long, 5>( NumericTraits<long>::One );
const FixedArray<unsigned long, 5>  NumericTraits<FixedArray<unsigned long, 5> >::Zero = FixedArray<unsigned long, 5>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 5>  NumericTraits<FixedArray<unsigned long, 5> >::One = FixedArray<unsigned long, 5>( NumericTraits<unsigned long>::One );
const FixedArray<float, 5>  NumericTraits<FixedArray<float, 5> >::Zero = FixedArray<float, 5>( NumericTraits<float >::Zero );
const FixedArray<float, 5>  NumericTraits<FixedArray<float, 5> >::One = FixedArray<float, 5>( NumericTraits<float>::One );
const FixedArray<double, 5>  NumericTraits<FixedArray<double, 5> >::Zero = FixedArray<double , 5>( NumericTraits<double>::Zero );
const FixedArray<double, 5>  NumericTraits<FixedArray<double, 5> >::One = FixedArray<double, 5>( NumericTraits<double>::One );
const FixedArray<long double, 5>  NumericTraits<FixedArray<long double, 5> >::Zero = FixedArray<long double, 5>( NumericTraits<long double>::Zero );
const FixedArray<long double, 5>  NumericTraits<FixedArray<long double, 5> >::One = FixedArray<long double, 5>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 6>  NumericTraits<FixedArray<unsigned char, 6> >::Zero = FixedArray<unsigned char, 6>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 6>  NumericTraits<FixedArray<unsigned char, 6> >::One = FixedArray<unsigned char, 6>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 6>  NumericTraits<FixedArray<signed char, 6> >::Zero = FixedArray<signed char, 6>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 6>  NumericTraits<FixedArray<signed char, 6> >::One = FixedArray<signed char, 6>( NumericTraits<signed char>::One );
const FixedArray<char, 6>  NumericTraits<FixedArray<char, 6> >::Zero = FixedArray<char, 6>( NumericTraits<char>::Zero );
const FixedArray<char, 6>  NumericTraits<FixedArray<char, 6> >::One = FixedArray<char, 6>( NumericTraits<char>::One );
const FixedArray<short, 6>  NumericTraits<FixedArray<short, 6> >::Zero = FixedArray<short, 6>( NumericTraits<short>::Zero );
const FixedArray<short, 6>  NumericTraits<FixedArray<short, 6> >::One = FixedArray<short, 6>( NumericTraits<short>::One );
const FixedArray<unsigned short, 6>  NumericTraits<FixedArray<unsigned short, 6> >::Zero = FixedArray<unsigned short, 6>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 6>  NumericTraits<FixedArray<unsigned short, 6> >::One = FixedArray<unsigned short, 6>( NumericTraits<unsigned short>::One );
const FixedArray<int, 6>  NumericTraits<FixedArray<int, 6> >::Zero = FixedArray<int, 6>( NumericTraits<int>::Zero );
const FixedArray<int, 6>  NumericTraits<FixedArray<int, 6> >::One = FixedArray<int, 6>( NumericTraits<int>::One );
const FixedArray<unsigned int, 6>  NumericTraits<FixedArray<unsigned int, 6> >::Zero = FixedArray<unsigned int, 6>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 6>  NumericTraits<FixedArray<unsigned int, 6> >::One = FixedArray<unsigned int, 6>( NumericTraits<unsigned int>::One );
const FixedArray<long, 6>  NumericTraits<FixedArray<long, 6> >::Zero = FixedArray<long, 6>( NumericTraits<long>::Zero );
const FixedArray<long, 6>  NumericTraits<FixedArray<long, 6> >::One = FixedArray<long, 6>( NumericTraits<long>::One );
const FixedArray<unsigned long, 6>  NumericTraits<FixedArray<unsigned long, 6> >::Zero = FixedArray<unsigned long, 6>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 6>  NumericTraits<FixedArray<unsigned long, 6> >::One = FixedArray<unsigned long, 6>( NumericTraits<unsigned long>::One );
const FixedArray<float, 6>  NumericTraits<FixedArray<float, 6> >::Zero = FixedArray<float, 6>( NumericTraits<float >::Zero );
const FixedArray<float, 6>  NumericTraits<FixedArray<float, 6> >::One = FixedArray<float, 6>( NumericTraits<float>::One );
const FixedArray<double, 6>  NumericTraits<FixedArray<double, 6> >::Zero = FixedArray<double , 6>( NumericTraits<double>::Zero );
const FixedArray<double, 6>  NumericTraits<FixedArray<double, 6> >::One = FixedArray<double, 6>( NumericTraits<double>::One );
const FixedArray<long double, 6>  NumericTraits<FixedArray<long double, 6> >::Zero = FixedArray<long double, 6>( NumericTraits<long double>::Zero );
const FixedArray<long double, 6>  NumericTraits<FixedArray<long double, 6> >::One = FixedArray<long double, 6>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 7>  NumericTraits<FixedArray<unsigned char, 7> >::Zero = FixedArray<unsigned char, 7>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 7>  NumericTraits<FixedArray<unsigned char, 7> >::One = FixedArray<unsigned char, 7>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 7>  NumericTraits<FixedArray<signed char, 7> >::Zero = FixedArray<signed char, 7>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 7>  NumericTraits<FixedArray<signed char, 7> >::One = FixedArray<signed char, 7>( NumericTraits<signed char>::One );
const FixedArray<char, 7>  NumericTraits<FixedArray<char, 7> >::Zero = FixedArray<char, 7>( NumericTraits<char>::Zero );
const FixedArray<char, 7>  NumericTraits<FixedArray<char, 7> >::One = FixedArray<char, 7>( NumericTraits<char>::One );
const FixedArray<short, 7>  NumericTraits<FixedArray<short, 7> >::Zero = FixedArray<short, 7>( NumericTraits<short>::Zero );
const FixedArray<short, 7>  NumericTraits<FixedArray<short, 7> >::One = FixedArray<short, 7>( NumericTraits<short>::One );
const FixedArray<unsigned short, 7>  NumericTraits<FixedArray<unsigned short, 7> >::Zero = FixedArray<unsigned short, 7>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 7>  NumericTraits<FixedArray<unsigned short, 7> >::One = FixedArray<unsigned short, 7>( NumericTraits<unsigned short>::One );
const FixedArray<int, 7>  NumericTraits<FixedArray<int, 7> >::Zero = FixedArray<int, 7>( NumericTraits<int>::Zero );
const FixedArray<int, 7>  NumericTraits<FixedArray<int, 7> >::One = FixedArray<int, 7>( NumericTraits<int>::One );
const FixedArray<unsigned int, 7>  NumericTraits<FixedArray<unsigned int, 7> >::Zero = FixedArray<unsigned int, 7>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 7>  NumericTraits<FixedArray<unsigned int, 7> >::One = FixedArray<unsigned int, 7>( NumericTraits<unsigned int>::One );
const FixedArray<long, 7>  NumericTraits<FixedArray<long, 7> >::Zero = FixedArray<long, 7>( NumericTraits<long>::Zero );
const FixedArray<long, 7>  NumericTraits<FixedArray<long, 7> >::One = FixedArray<long, 7>( NumericTraits<long>::One );
const FixedArray<unsigned long, 7>  NumericTraits<FixedArray<unsigned long, 7> >::Zero = FixedArray<unsigned long, 7>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 7>  NumericTraits<FixedArray<unsigned long, 7> >::One = FixedArray<unsigned long, 7>( NumericTraits<unsigned long>::One );
const FixedArray<float, 7>  NumericTraits<FixedArray<float, 7> >::Zero = FixedArray<float, 7>( NumericTraits<float >::Zero );
const FixedArray<float, 7>  NumericTraits<FixedArray<float, 7> >::One = FixedArray<float, 7>( NumericTraits<float>::One );
const FixedArray<double, 7>  NumericTraits<FixedArray<double, 7> >::Zero = FixedArray<double , 7>( NumericTraits<double>::Zero );
const FixedArray<double, 7>  NumericTraits<FixedArray<double, 7> >::One = FixedArray<double, 7>( NumericTraits<double>::One );
const FixedArray<long double, 7>  NumericTraits<FixedArray<long double, 7> >::Zero = FixedArray<long double, 7>( NumericTraits<long double>::Zero );
const FixedArray<long double, 7>  NumericTraits<FixedArray<long double, 7> >::One = FixedArray<long double, 7>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 8>  NumericTraits<FixedArray<unsigned char, 8> >::Zero = FixedArray<unsigned char, 8>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 8>  NumericTraits<FixedArray<unsigned char, 8> >::One = FixedArray<unsigned char, 8>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 8>  NumericTraits<FixedArray<signed char, 8> >::Zero = FixedArray<signed char, 8>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 8>  NumericTraits<FixedArray<signed char, 8> >::One = FixedArray<signed char, 8>( NumericTraits<signed char>::One );
const FixedArray<char, 8>  NumericTraits<FixedArray<char, 8> >::Zero = FixedArray<char, 8>( NumericTraits<char>::Zero );
const FixedArray<char, 8>  NumericTraits<FixedArray<char, 8> >::One = FixedArray<char, 8>( NumericTraits<char>::One );
const FixedArray<short, 8>  NumericTraits<FixedArray<short, 8> >::Zero = FixedArray<short, 8>( NumericTraits<short>::Zero );
const FixedArray<short, 8>  NumericTraits<FixedArray<short, 8> >::One = FixedArray<short, 8>( NumericTraits<short>::One );
const FixedArray<unsigned short, 8>  NumericTraits<FixedArray<unsigned short, 8> >::Zero = FixedArray<unsigned short, 8>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 8>  NumericTraits<FixedArray<unsigned short, 8> >::One = FixedArray<unsigned short, 8>( NumericTraits<unsigned short>::One );
const FixedArray<int, 8>  NumericTraits<FixedArray<int, 8> >::Zero = FixedArray<int, 8>( NumericTraits<int>::Zero );
const FixedArray<int, 8>  NumericTraits<FixedArray<int, 8> >::One = FixedArray<int, 8>( NumericTraits<int>::One );
const FixedArray<unsigned int, 8>  NumericTraits<FixedArray<unsigned int, 8> >::Zero = FixedArray<unsigned int, 8>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 8>  NumericTraits<FixedArray<unsigned int, 8> >::One = FixedArray<unsigned int, 8>( NumericTraits<unsigned int>::One );
const FixedArray<long, 8>  NumericTraits<FixedArray<long, 8> >::Zero = FixedArray<long, 8>( NumericTraits<long>::Zero );
const FixedArray<long, 8>  NumericTraits<FixedArray<long, 8> >::One = FixedArray<long, 8>( NumericTraits<long>::One );
const FixedArray<unsigned long, 8>  NumericTraits<FixedArray<unsigned long, 8> >::Zero = FixedArray<unsigned long, 8>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 8>  NumericTraits<FixedArray<unsigned long, 8> >::One = FixedArray<unsigned long, 8>( NumericTraits<unsigned long>::One );
const FixedArray<float, 8>  NumericTraits<FixedArray<float, 8> >::Zero = FixedArray<float, 8>( NumericTraits<float >::Zero );
const FixedArray<float, 8>  NumericTraits<FixedArray<float, 8> >::One = FixedArray<float, 8>( NumericTraits<float>::One );
const FixedArray<double, 8>  NumericTraits<FixedArray<double, 8> >::Zero = FixedArray<double , 8>( NumericTraits<double>::Zero );
const FixedArray<double, 8>  NumericTraits<FixedArray<double, 8> >::One = FixedArray<double, 8>( NumericTraits<double>::One );
const FixedArray<long double, 8>  NumericTraits<FixedArray<long double, 8> >::Zero = FixedArray<long double, 8>( NumericTraits<long double>::Zero );
const FixedArray<long double, 8>  NumericTraits<FixedArray<long double, 8> >::One = FixedArray<long double, 8>( NumericTraits<long double>::One );

const FixedArray<unsigned char, 9>  NumericTraits<FixedArray<unsigned char, 9> >::Zero = FixedArray<unsigned char, 9>( NumericTraits<unsigned char>::Zero );
const FixedArray<unsigned char, 9>  NumericTraits<FixedArray<unsigned char, 9> >::One = FixedArray<unsigned char, 9>( NumericTraits<unsigned char>::One );
const FixedArray<signed char, 9>  NumericTraits<FixedArray<signed char, 9> >::Zero = FixedArray<signed char, 9>( NumericTraits<signed char>::Zero );
const FixedArray<signed char, 9>  NumericTraits<FixedArray<signed char, 9> >::One = FixedArray<signed char, 9>( NumericTraits<signed char>::One );
const FixedArray<char, 9>  NumericTraits<FixedArray<char, 9> >::Zero = FixedArray<char, 9>( NumericTraits<char>::Zero );
const FixedArray<char, 9>  NumericTraits<FixedArray<char, 9> >::One = FixedArray<char, 9>( NumericTraits<char>::One );
const FixedArray<short, 9>  NumericTraits<FixedArray<short, 9> >::Zero = FixedArray<short, 9>( NumericTraits<short>::Zero );
const FixedArray<short, 9>  NumericTraits<FixedArray<short, 9> >::One = FixedArray<short, 9>( NumericTraits<short>::One );
const FixedArray<unsigned short, 9>  NumericTraits<FixedArray<unsigned short, 9> >::Zero = FixedArray<unsigned short, 9>( NumericTraits<unsigned short>::Zero );
const FixedArray<unsigned short, 9>  NumericTraits<FixedArray<unsigned short, 9> >::One = FixedArray<unsigned short, 9>( NumericTraits<unsigned short>::One );
const FixedArray<int, 9>  NumericTraits<FixedArray<int, 9> >::Zero = FixedArray<int, 9>( NumericTraits<int>::Zero );
const FixedArray<int, 9>  NumericTraits<FixedArray<int, 9> >::One = FixedArray<int, 9>( NumericTraits<int>::One );
const FixedArray<unsigned int, 9>  NumericTraits<FixedArray<unsigned int, 9> >::Zero = FixedArray<unsigned int, 9>( NumericTraits<unsigned int>::Zero );
const FixedArray<unsigned int, 9>  NumericTraits<FixedArray<unsigned int, 9> >::One = FixedArray<unsigned int, 9>( NumericTraits<unsigned int>::One );
const FixedArray<long, 9>  NumericTraits<FixedArray<long, 9> >::Zero = FixedArray<long, 9>( NumericTraits<long>::Zero );
const FixedArray<long, 9>  NumericTraits<FixedArray<long, 9> >::One = FixedArray<long, 9>( NumericTraits<long>::One );
const FixedArray<unsigned long, 9>  NumericTraits<FixedArray<unsigned long, 9> >::Zero = FixedArray<unsigned long, 9>( NumericTraits<unsigned long>::Zero );
const FixedArray<unsigned long, 9>  NumericTraits<FixedArray<unsigned long, 9> >::One = FixedArray<unsigned long, 9>( NumericTraits<unsigned long>::One );
const FixedArray<float, 9>  NumericTraits<FixedArray<float, 9> >::Zero = FixedArray<float, 9>( NumericTraits<float >::Zero );
const FixedArray<float, 9>  NumericTraits<FixedArray<float, 9> >::One = FixedArray<float, 9>( NumericTraits<float>::One );
const FixedArray<double, 9>  NumericTraits<FixedArray<double, 9> >::Zero = FixedArray<double , 9>( NumericTraits<double>::Zero );
const FixedArray<double, 9>  NumericTraits<FixedArray<double, 9> >::One = FixedArray<double, 9>( NumericTraits<double>::One );
const FixedArray<long double, 9>  NumericTraits<FixedArray<long double, 9> >::Zero = FixedArray<long double, 9>( NumericTraits<long double>::Zero );
const FixedArray<long double, 9>  NumericTraits<FixedArray<long double, 9> >::One = FixedArray<long double, 9>( NumericTraits<long double>::One );

} // end namespace itk
