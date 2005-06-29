/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNumericTraitsSymmetricSecondRankTensorPixel.cxx,v $
  Language:  C++
  Date:      $Date: 2005/05/13 20:12:19 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkNumericTraitsTensorPixel.h"

namespace itk
{

const SymmetricSecondRankTensor<double , 2>  NumericTraits<SymmetricSecondRankTensor<double, 2> >::Zero = SymmetricSecondRankTensor<double , 2>( NumericTraits<double >::Zero );
const SymmetricSecondRankTensor<double , 2>  NumericTraits<SymmetricSecondRankTensor<double , 2> >::One = SymmetricSecondRankTensor<double , 2>( NumericTraits<double >::One );
 const SymmetricSecondRankTensor<double , 3>  NumericTraits<SymmetricSecondRankTensor<double, 3> >::Zero = SymmetricSecondRankTensor<double , 3>( NumericTraits<double>::Zero );
const SymmetricSecondRankTensor<double , 3>  NumericTraits<SymmetricSecondRankTensor<double , 3> >::One = SymmetricSecondRankTensor<double , 3>( NumericTraits<double>::One );
 const SymmetricSecondRankTensor<double , 4>  NumericTraits<SymmetricSecondRankTensor<double, 4> >::Zero = SymmetricSecondRankTensor<double , 4>( NumericTraits<double>::Zero );
const SymmetricSecondRankTensor<double , 4>  NumericTraits<SymmetricSecondRankTensor<double , 4> >::One = SymmetricSecondRankTensor<double , 4>( NumericTraits<double>::One );
const SymmetricSecondRankTensor<double , 5>  NumericTraits<SymmetricSecondRankTensor<double, 5> >::Zero = SymmetricSecondRankTensor<double , 5>( NumericTraits<double>::Zero );
const SymmetricSecondRankTensor<double , 5>  NumericTraits<SymmetricSecondRankTensor<double , 5> >::One = SymmetricSecondRankTensor<double , 5>( NumericTraits<double>::One );
const SymmetricSecondRankTensor<double , 6>  NumericTraits<SymmetricSecondRankTensor<double, 6> >::Zero = SymmetricSecondRankTensor<double , 6>( NumericTraits<double>::Zero );
const SymmetricSecondRankTensor<double , 6>  NumericTraits<SymmetricSecondRankTensor<double , 6> >::One = SymmetricSecondRankTensor<double , 6>( NumericTraits<double>::One );
/*const SymmetricSecondRankTensor<double , 7>  NumericTraits<SymmetricSecondRankTensor<double, 7> >::Zero = SymmetricSecondRankTensor<double , 7>( NumericTraits<double>::Zero );
const SymmetricSecondRankTensor<double , 7>  NumericTraits<SymmetricSecondRankTensor<double , 7> >::One = SymmetricSecondRankTensor<double , 7>( NumericTraits<double>::One );
*/
const SymmetricSecondRankTensor<float , 2>  NumericTraits<SymmetricSecondRankTensor<float, 2> >::Zero = SymmetricSecondRankTensor<float , 2>( NumericTraits<float >::Zero );
const SymmetricSecondRankTensor<float , 2>  NumericTraits<SymmetricSecondRankTensor<float , 2> >::One = SymmetricSecondRankTensor<float , 2>( NumericTraits<float >::One );
 const SymmetricSecondRankTensor<float , 3>  NumericTraits<SymmetricSecondRankTensor<float, 3> >::Zero = SymmetricSecondRankTensor<float , 3>( NumericTraits<float>::Zero );
const SymmetricSecondRankTensor<float , 3>  NumericTraits<SymmetricSecondRankTensor<float , 3> >::One = SymmetricSecondRankTensor<float , 3>( NumericTraits<float>::One );
 const SymmetricSecondRankTensor<float , 4>  NumericTraits<SymmetricSecondRankTensor<float, 4> >::Zero = SymmetricSecondRankTensor<float , 4>( NumericTraits<float>::Zero );
const SymmetricSecondRankTensor<float , 4>  NumericTraits<SymmetricSecondRankTensor<float , 4> >::One = SymmetricSecondRankTensor<float , 4>( NumericTraits<float>::One );
const SymmetricSecondRankTensor<float , 5>  NumericTraits<SymmetricSecondRankTensor<float, 5> >::Zero = SymmetricSecondRankTensor<float , 5>( NumericTraits<float>::Zero );
const SymmetricSecondRankTensor<float , 5>  NumericTraits<SymmetricSecondRankTensor<float , 5> >::One = SymmetricSecondRankTensor<float , 5>( NumericTraits<float>::One );
const SymmetricSecondRankTensor<float , 6>  NumericTraits<SymmetricSecondRankTensor<float, 6> >::Zero = SymmetricSecondRankTensor<float , 6>( NumericTraits<float>::Zero );
const SymmetricSecondRankTensor<float , 6>  NumericTraits<SymmetricSecondRankTensor<float , 6> >::One = SymmetricSecondRankTensor<float , 6>( NumericTraits<float>::One );


} // end namespace itk
