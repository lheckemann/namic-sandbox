/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialBase.cpp,v 1.5 2006/07/10 19:16:39 ofri Exp $

  Author(s):  Ofri Sadowsky
  Created on:   2001-10-16

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---
*/


#include <cisstNumerical/nmrPolynomialBase.h>
#include <assert.h>
#include <iostream>

nmrPolynomialBase::nmrPolynomialBase(VariableIndexType numVariables, 
                                     PowerType minDegree, PowerType maxDegree)
    : NumVariables(numVariables)
    , MinDegree(minDegree)
    , MaxDegree(maxDegree)
{}


bool nmrPolynomialBase::CanIncludeIndex(const nmrPolynomialTermPowerIndex & term) const
{
  // We test for each clause in a separate line for easier debugging
  if (term.GetNumVariables() != GetNumVariables())
    return false;
  if (GetMinDegree() > term.GetMinDegree())
    return false;
  if (term.GetMaxDegree() > GetMaxDegree())
    return false;
  if (!term.IsValid())
    return false;
  return true;
}


void nmrPolynomialBase::ScaleCoefficients(CoefficientType coefficients[], 
                                          CoefficientType scaleFactor) const
{
    TermCounterType numTerms = GetNumberOfTerms();
    TermCounterType termCounter = 0;
    for (; termCounter < numTerms; ++termCounter) {
        coefficients[termCounter] *= scaleFactor;
    }
}



void nmrPolynomialBase::SerializeRaw(std::ostream & output) const
{
    output.write( (const char *)&NumVariables, sizeof(NumVariables) );
    output.write( (const char *)&MinDegree, sizeof(MinDegree) );
    output.write( (const char *)&MaxDegree, sizeof(MaxDegree) );
}


void nmrPolynomialBase::DeserializeRaw(std::istream & input)
{
    Clear();
    input.read( (char *)&NumVariables, sizeof(NumVariables) );
    input.read( (char *)&MinDegree, sizeof(MinDegree) );
    input.read( (char *)&MaxDegree, sizeof(MaxDegree) );

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
    VariablePowers->Initialize(NumVariables, MaxDegree);
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(pop)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialBase.cpp,v $
//  Revision 1.5  2006/07/10 19:16:39  ofri
//  cisstNumerical polynomial classes: The code that had been declared
//  deprecated is now marked-out through #if brackets.  It will be removed
//  from the next release.
//
//  Revision 1.4  2006/06/12 20:22:50  ofri
//  cisstNumerical polynomial classes.  The internal cache of variable powers
//  (lookup table) is now declared deprecated.  The functionality should be
//  factored out to nmrMultiVariablePowerBasis objects.  Pragmas set for MSVC
//  to reduce the number of compilation warnings.  The inner
//  #define FACTOR_OUT_POLYNOMIAL_VARIABLES has been removed, as well as all
//  references to it.
//
//  Revision 1.3  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.2  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.10  2003/12/24 17:57:46  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.9  2003/09/22 18:29:22  ofri
//  Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.8  2003/08/26 16:28:44  ofri
//  Fixed bug related to initialization of 0-degree polynomial when variables are
//  not factored out.
//
//  Revision 1.7  2003/08/25 22:15:12  ofri
//  Factored out nmrPolynomialBase::EvaluatePowerProduct() to
//  nmrMultiVariablePowerBasis
//
//  Revision 1.6  2003/08/21 19:04:58  ofri
//  Completed factoring the variable container out of the polynomial classes.
//  In this submission, the user can control conditional compilation of the library
//  with or without the use of the refactored classes, by setting the flag
//  FACTOR_OUT_POLYNOMIAL_VARIABLES .
//  The next step is to run tests and then remove the conditional compilation
//  option.
//  After that, I may completely remove the reference to the variable container
//  from the polynomial class, and have only evaulation parametrized by
//  a variable container.
//
//  Revision 1.5  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.4  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
