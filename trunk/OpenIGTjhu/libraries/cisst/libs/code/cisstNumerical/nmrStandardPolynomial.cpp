/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrStandardPolynomial.cpp,v 1.5 2006/07/10 19:16:39 ofri Exp $

  Author(s):  Ofri Sadowsky
  Created on:   2003

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


#include <cisstNumerical/nmrStandardPolynomial.h>


#include <assert.h>
#include <iostream>

#if INCLUDE_DEPRECATED_POLYNOMIAL_CODE
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
void nmrStandardPolynomial::SetVariable(VariableIndexType varIndex, VariableType value)
{
    VariablePowers->SetVariable(varIndex, value);
}

void nmrStandardPolynomial::SetVariables(const VariableType vars[])
{
  VariableIndexType v;
  VariableIndexType numVars = GetNumVariables();
  for (v = 0; v < numVars; v++) {
    SetVariable(v, vars[v]);
  }
}
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(pop)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#endif  // INCLUDE_DEPRECATED_POLYNOMIAL_CODE

nmrStandardPolynomial::InsertStatus nmrStandardPolynomial::SetCoefficient(const nmrPolynomialTermPowerIndex & where, CoefficientType coefficient)
{
  if (!this->CanIncludeIndex(where))
    return INSERT_FAIL;

#if (CONTAINER_TYPE == MAP_CONTAINER)

  TermIteratorType foundTerm = FindTerm(where);
  if (foundTerm != EndTermIterator()) {
    DereferenceIterator(foundTerm) = coefficient;
    return INSERT_REPLACE;
  }

  TermType term(where, new CoefficientType(coefficient) );
  std::pair<TermIteratorType, bool> result = Terms.insert(term);
  return INSERT_NEW;

#elif (CONTAINER_TYPE == LIST_CONTAINER)

    TermIteratorType foundTerm = std::find_if(FirstTermIterator(), EndTermIterator(), LessOrEqualityTester(where));
    if (foundTerm == EndTermIterator()) {
        Terms.insert( EndTermIterator(), ContainerElementType(where, new CoefficientType(coefficient)) );
        return INSERT_NEW;
    }

    if (where.Compare( (*foundTerm).first ) == 0) {
        DereferenceIterator(foundTerm) = coefficient;
        return INSERT_REPLACE;
    }

    Terms.insert( foundTerm, ContainerElementType(where, new CoefficientType(coefficient)) );
    return INSERT_NEW;
#endif
}


void nmrStandardPolynomial::RemoveTerm(TermIteratorType & where)
{
  delete &(DereferenceIterator(where));
  BaseType::RemoveTerm(where);
}


void nmrStandardPolynomial::Clear()
{
  TermIteratorType termIt = FirstTermIterator();
  while (termIt != EndTermIterator()) {
    CoefficientType * pCoeff = &(DereferenceIterator(termIt));
    delete pCoeff;
    termIt++;
  }

  BaseType::Clear();
}


void nmrStandardPolynomial::SerializeTermInfo(std::ostream & output, const TermConstIteratorType & termIterator) const
{
    const CoefficientType pCoeff = DereferenceConstIterator(termIterator);
    output.write( (const char *)&pCoeff, sizeof(CoefficientType) );
}

void nmrStandardPolynomial::DeserializeTermInfo(std::istream & input, TermIteratorType & termIterator)
{
    termIterator->second = new CoefficientType();
    input.read( (char *)termIterator->second, sizeof(CoefficientType) );
}


void nmrStandardPolynomial::AddConstant(CoefficientType shiftAmount)
{
    nmrPolynomialTermPowerIndex termIndex(*this);
    termIndex.SetDegree(0);
    CoefficientType currentConstant = GetCoefficient(termIndex);
    SetCoefficient(termIndex, currentConstant + shiftAmount);
}

void nmrStandardPolynomial::AddConstantToCoefficients(CoefficientType coefficients[],
    CoefficientType shiftAmount) const
{
    TermConstIteratorType termIt = FirstTermIterator();
    TermConstIteratorType endIt = EndTermIterator();
    TermCounterType termCounter = 0;

    for(; termIt != endIt; ++termIt, ++termCounter) {
        if (GetTermPowerIndex(termIt).GetDegree() == 0) {
            coefficients[termCounter] += shiftAmount;
            return;
        }
    }

    assert(false);
}

#define STANDARD_POLYNOMIAL_TEST_CONSTRUCTOR 0

#if STANDARD_POLYNOMIAL_TEST_CONSTRUCTOR
static nmrStandardPolynomial g_TestStandardPolynomial(4, 0, 3);
#endif

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrStandardPolynomial.cpp,v $
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
//  Revision 1.7  2003/11/18 15:43:42  ofri
//  cisstNumerical: changes related mostly to signed/unsigned MSVC7 warning
//  fixes
//
//  Revision 1.6  2003/09/22 18:29:22  ofri
//  Added abstract methods Scale(), ScaleCoefficients(), AddConstant(),
//  AddConstantToCoefficients() to the base class nmrPolynomialBase,
//  and actual implementations in nmrStandardPolynomial and
//  nmrBernsteinPolynomial.
//
//  Submitted after successful build but no testing yet.
//
//  Revision 1.5  2003/08/21 19:04:58  ofri
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
//  Revision 1.4  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
