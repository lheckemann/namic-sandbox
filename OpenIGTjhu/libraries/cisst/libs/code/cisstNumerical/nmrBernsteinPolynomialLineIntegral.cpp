/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrBernsteinPolynomialLineIntegral.cpp,v 1.5 2006/06/12 20:23:54 ofri Exp $

  Author(s):  Ofri Sadowsky
  Created on:   2003-05-06

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


#include <cisstNumerical/nmrBernsteinPolynomialLineIntegral.h>

#include <cisstNumerical/nmrBernsteinPolynomial.h>
#include <cisstNumerical/nmrStandardPolynomial.h>

#include <cisstNumerical/nmrMultiIndexCounter.h>

#include <math.h>

const double nmrBernsteinPolynomialLineIntegral::DefaultRoundoffTolerance = 1.0e-4;

nmrBernsteinPolynomialLineIntegral::ValueType
nmrBernsteinPolynomialLineIntegral::
EvaluateForSegment(const VariableType p0[], const VariableType p1[],
                   const VariableType gradientNorm,
           const double roundoffTolerance,
                   CoefficientType const * coefficients)
#if !CACHE_POWER_BASIS
                   const
#endif
{
  VariableIndexType numVariables = Integrand.GetNumVariables();

  // pre-calculate the powers of all variables at the 'p0' point.
  // pre-calculate the powers of all variables at the 'p1' point.
#if CACHE_POWER_BASIS
    PowersAtP0.SetVariables(p0);
    PowersAtP1.SetVariables(p1);
#else
    nmrMultiVariablePowerBasis::StandardPowerBasis 
        powersAtP0(Integrand.GetNumVariables(), Integrand.GetMaxDegree());
    nmrMultiVariablePowerBasis::StandardPowerBasis
        powersAtP1(Integrand.GetNumVariables(), Integrand.GetMaxDegree());
  powersAtP0.SetVariables(p0);
  powersAtP1.SetVariables(p1);
#endif  // CACHE_POWER_BASIS

  ValueType result = 0;

#if USE_INTEGRAND_TABLEAU
    // Find which variables are zero and store them in arrays for being skipped
    // later.
    IndexContainerForZeroVariables zerosOfP0;
    IndexContainerForZeroVariables zerosOfP1;
    VariableIndexType variableIndex;
    for (variableIndex = 0; variableIndex < numVariables; ++variableIndex) {
        if ( fabs(p0[variableIndex]) <= roundoffTolerance )
            zerosOfP0.push_back(variableIndex);
        if ( fabs(p1[variableIndex]) <= roundoffTolerance )
            zerosOfP1.push_back(variableIndex);
    }

    PolynomialIntegrationTableau::const_iterator termTableauIterator = IntegrationTableau.begin();
    nmrBernsteinPolynomial::TermConstIteratorType integrandIterator = Integrand.FirstTermIterator();
    while (termTableauIterator != IntegrationTableau.end()) {

    // Evaluate the line integral for the current term, and add it to the total.
        const CoefficientType coefficient = (coefficients != NULL) 
            ? *coefficients 
            : Integrand.GetCoefficient(integrandIterator);

        PowerType termDegree = Integrand.GetTermPowerIndex(integrandIterator).GetDegree();

        ValueType termIntegral = IntegrateSingleTerm(
            coefficient, gradientNorm, termDegree,
            *termTableauIterator, 
#if CACHE_POWER_BASIS
            PowersAtP0, PowersAtP1, 
#else
            powersAtP0, powersAtP1,
#endif
            zerosOfP0, zerosOfP1);

        result += termIntegral;

        ++termTableauIterator;
        ++integrandIterator;
        if (coefficients != NULL)
            ++coefficients;
    }
#else
  // iterate for each term of the Bernstein polynomial...
  nmrBernsteinPolynomial::TermConstIteratorType termIterator = Integrand.FirstTermIterator();
  nmrBernsteinPolynomial::TermConstIteratorType termEndIterator = Integrand.EndTermIterator();
  while (termIterator != termEndIterator) {

    // Evaluate the line integral for the current term, and add it to the total.
        const CoefficientType coefficient = (coefficients != NULL) 
            ? *coefficients 
            : Integrand.GetCoefficient(termIterator);
    ValueType termIntegral = IntegrateSingleTerm( 
      coefficient,
      gradientNorm,
      Integrand.GetTermPowerIndex(termIterator), 
#if CACHE_POWER_BASIS
            PowersAtP0, PowersAtP1,
#else
      powersAtP0, powersAtP1,
#endif
      roundoffTolerance);

    // Accumulate the term integral for the result.
    result += termIntegral;

    // go to the next term
    termIterator++;

        if (coefficients != NULL)
            ++coefficients;
  }
#endif
  return result;
}


nmrBernsteinPolynomialLineIntegral::ValueType
nmrBernsteinPolynomialLineIntegral::IntegrateSingleTerm(const CoefficientType termCoefficient,
                            const CoefficientType gradientNorm,
                            const nmrPolynomialTermPowerIndex & termIndex,
                                                        const nmrMultiVariablePowerBasis & powersAtP0,
                                                        const nmrMultiVariablePowerBasis & powersAtP1,
                            const double roundoffTolerance)
{
  const VariableIndexType numVariables = termIndex.GetNumVariables();

  // These are the multi-index counters of the summation : the counter index a_k
  // goes from 0 to p_k==termIndex.GetPower(k)
  // may be cached later to save allocation/deallocation overhead
  nmrMultiIndexCounter p0Counter( numVariables );
  nmrMultiIndexCounter p1Counter( numVariables );

  // A vector of zeros used to initialize the mutli-index counters as the lower
  // bounds.
  // may be cached later to save allocation/deallocation overhead
  const std::vector<nmrMultiIndexCounter::IndexType> zeros( p0Counter.GetSize(), 0 );

  // We use explicit cast to convert 'unsigned int *' to 'int *'. Make sure
  // that IndexType and PowerType are compatible in size.
  p0Counter.SetBounds( &zeros[0], termIndex.GetPowersAsSigned() );
  p1Counter.SetBounds( &zeros[0], termIndex.GetPowersAsSigned() );

  // If any of the variables is zero, we want to factor its counterpart out of
  // the summation.  See Russ Taylor's simplification of the line integral
  // for the case of a ray intersecting a face of a tetrahedron, and a barycentric
  // coordinate at the intersection point is zero.
  //
  // After the factoring out, we reduce the number of iterations.  We could have
  // done better by completely factoring out the product of the degenerate variables,
  // and not evaluating it in the inner loop.  But I suspect that the required
  // tests are too complicated, and don't gain as much performance as the reduction
  // in interation count.
  nmrPolynomialBase::VariableIndexType variableIndex;
  for (variableIndex = 0; variableIndex < numVariables; variableIndex++) {
    // We test both endpoints of the segment in the same loop to avoid conflicts
    // related to degenerate cases, where more the same coordinate is zero in both
    // endpoints.
    if ( fabs(powersAtP0.GetVariable(variableIndex)) <= roundoffTolerance) {
      p0Counter.SetHighBound(variableIndex, 0);
      const nmrPolynomialTermPowerIndex::PowerType p1Power = termIndex.GetPower(variableIndex);
      p1Counter.SetLowBound(variableIndex, p1Power);
      continue;
    }

    if ( fabs(powersAtP1.GetVariable(variableIndex)) <= roundoffTolerance) {
      p1Counter.SetHighBound(variableIndex, 0);
      const nmrPolynomialTermPowerIndex::PowerType p0Power = termIndex.GetPower(variableIndex);
      p0Counter.SetLowBound(variableIndex, p0Power);
      continue;
    }
  }


  p0Counter.GoToLowBounds();
  p1Counter.GoToHighBounds();

  // We create a nmrPolynomialTermPowerIndex to store the same index as the
  // counter, in order to serve as an index into the polynomial and to evaluate
  // multinomial factors.  We create them before the loop to save memory
  // allocation overhead.
  nmrPolynomialTermPowerIndex p0PowerIndex( powersAtP0.GetNumVariables(), powersAtP0.GetMaxDegree() );
  nmrPolynomialTermPowerIndex p1PowerIndex( powersAtP1.GetNumVariables(), powersAtP1.GetMaxDegree() );

  // We mutliply each element of the sum by 1/(n+1) to avoid explosion of the
  // sum.
  const ValueType integrationDenominator = gradientNorm / static_cast<ValueType>(termIndex.GetDegree() + 1);

  ValueType result = 0;



  while (p0Counter.IsBelowHighBounds()) {

        p0PowerIndex.SetPowers( p0Counter.GetIndexCells() );
    p1PowerIndex.SetPowers( p1Counter.GetIndexCells() );

    const MultinomialType p0Multinomial = p0PowerIndex.GetMultinomialCoefficient();
    const MultinomialType p1Multinomial = p1PowerIndex.GetMultinomialCoefficient();

    const ValueType p0PowerProduct = powersAtP0.EvaluatePowerProduct(p0PowerIndex.GetPowers());
    const ValueType p1PowerProduct = powersAtP1.EvaluatePowerProduct(p1PowerIndex.GetPowers());

    const ValueType element = p0Multinomial * p0PowerProduct * p1Multinomial * p1PowerProduct;

    result += integrationDenominator * element;

    p0Counter.Increment();
    p1Counter.Decrement();
  }

  result *= termCoefficient;

  return result;
}




nmrBernsteinPolynomialLineIntegral::ValueType
nmrBernsteinPolynomialLineIntegral::IntegrateSingleTerm(
    const CoefficientType termCoefficient,
    const CoefficientType gradientNorm,
    const PowerType termDegree,
    const SingleTermIntegrationTableau & termIntegrationTableau,
    const nmrMultiVariablePowerBasis & powersAtP0,
    const nmrMultiVariablePowerBasis & powersAtP1,
    const IndexContainerForZeroVariables & zerosOfP0,
    const IndexContainerForZeroVariables & zerosOfP1
    )
{
  // We mutliply each element of the sum by 1/(n+1) to avoid explosion of the
  // sum.
  const ValueType integrationDenominator = gradientNorm / static_cast<ValueType>(termDegree + 1);

  ValueType result = 0;

    SingleTermIntegrationTableau::const_iterator summationElementIterator =
        termIntegrationTableau.begin();

    SingleTermIntegrationTableau::const_iterator lastElement = termIntegrationTableau.end();

    for (; summationElementIterator != lastElement; ++summationElementIterator) {

        // Do what we can to skip this summation element if it contains a non-zero
        // power of one of the zero input variables.
        // Check for P0 first
        IndexContainerForZeroVariables::const_iterator testForP0Zeros = 
            zerosOfP0.begin();
        for (; testForP0Zeros != zerosOfP0.end(); ++testForP0Zeros) {
            if ( (*summationElementIterator).P0PowerIndex[*testForP0Zeros] != 0 )
                break;
        }
        if (testForP0Zeros != zerosOfP0.end())
            continue;


        // Check for zero variables in P1
        IndexContainerForZeroVariables::const_iterator testForP1Zeros = 
            zerosOfP1.begin();
        for (; testForP1Zeros != zerosOfP1.end(); ++testForP1Zeros) {
            if ( (*summationElementIterator).P1PowerIndex[*testForP1Zeros] != 0 )
                break;
        }
        if (testForP1Zeros != zerosOfP1.end())
            continue;


        // Evaluate the product of multinomial factors and powers that constitutes
        // the summation element
    const MultinomialType p0Multinomial = (*summationElementIterator).P0MultinomialFactor;
    const MultinomialType p1Multinomial = (*summationElementIterator).P1MultinomialFactor;

    const ValueType p0PowerProduct = powersAtP0.EvaluatePowerProduct( &( (*summationElementIterator).P0PowerIndex.front() ) );
    const ValueType p1PowerProduct = powersAtP1.EvaluatePowerProduct( &( (*summationElementIterator).P1PowerIndex.front() ) );

    const ValueType element = p0Multinomial * p0PowerProduct * p1Multinomial * p1PowerProduct;

    result += integrationDenominator * element;

  }

  result *= termCoefficient;

  return result;
}



void nmrBernsteinPolynomialLineIntegral::InitializePolynomialIntegrationTableau(
    const IntegrandType & integrand,
    PolynomialIntegrationTableau & tableau)
{
    IntegrandType::TermConstIteratorType termIterator = integrand.FirstTermIterator();

    tableau.clear();
    for (; termIterator != integrand.EndTermIterator(); ++termIterator) {
        SingleTermIntegrationTableau initTermTableau;
        tableau.push_back(initTermTableau);
        SingleTermIntegrationTableau & termTableau = tableau.back();
        InitializeSingleTermIntegrationTableau( 
            integrand.GetTermPowerIndex(termIterator),
            termTableau);
    }
}


void nmrBernsteinPolynomialLineIntegral::InitializeSingleTermIntegrationTableau(
        const nmrPolynomialTermPowerIndex & termIndex,
        SingleTermIntegrationTableau & termTableau )
{
  const VariableIndexType numVariables = termIndex.GetNumVariables();

  // These are the multi-index counters of the summation : the counter index a_k
  // goes from 0 to p_k==termIndex.GetPower(k)
  // may be cached later to save allocation/deallocation overhead
  nmrMultiIndexCounter p0Counter( numVariables );
  nmrMultiIndexCounter p1Counter( numVariables );

  // A vector of zeros used to initialize the mutli-index counters as the lower
  // bounds.
  // may be cached later to save allocation/deallocation overhead
  const std::vector<nmrMultiIndexCounter::IndexType> zeros( p0Counter.GetSize(), 0 );

  // We use explicit cast to convert 'unsigned int *' to 'int *'. Make sure
  // that IndexType and PowerType are compatible in size.
  p0Counter.SetBounds( &zeros.front(), termIndex.GetPowersAsSigned() );
  p1Counter.SetBounds( &zeros.front(), termIndex.GetPowersAsSigned() );

  // We create a nmrPolynomialTermPowerIndex to store the same index as the
  // counter, in order to serve as an index into the polynomial and to evaluate
  // multinomial factors.  We create them before the loop to save memory
  // allocation overhead.
  nmrPolynomialTermPowerIndex powersOfP0Counter( termIndex );
  nmrPolynomialTermPowerIndex powersOfP1Counter( termIndex );

  p0Counter.GoToLowBounds();
  p1Counter.GoToHighBounds();
    termTableau.clear();

    while (p0Counter.IsBelowHighBounds()) {
    powersOfP0Counter.SetPowers( p0Counter.GetIndexCells() );
    powersOfP1Counter.SetPowers( p1Counter.GetIndexCells() );

        TermSummationElement initSummationElement;
        termTableau.push_back( initSummationElement );
        TermSummationElement & newElement = termTableau.back();

        newElement.P0PowerIndex.insert( 
            newElement.P0PowerIndex.end(), 
            p0Counter.GetIndexCellsAsUnsigned(), 
            p0Counter.GetIndexCellsAsUnsigned() + numVariables );

        newElement.P0MultinomialFactor = powersOfP0Counter.GetMultinomialCoefficient();

        newElement.P1PowerIndex.insert( 
            newElement.P1PowerIndex.end(), 
            p1Counter.GetIndexCellsAsUnsigned(), 
            p1Counter.GetIndexCellsAsUnsigned() + numVariables );

        newElement.P1MultinomialFactor = powersOfP1Counter.GetMultinomialCoefficient();


        p0Counter.Increment();
        p1Counter.Decrement();
    }

}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrBernsteinPolynomialLineIntegral.cpp,v $
//  Revision 1.5  2006/06/12 20:23:54  ofri
//  nmrBernsteinPolynomialLineIntegral: eliminated all references to
//  FACTOR_OUT_POLYNOMIAL_VARIABLES
//
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.2  2004/10/22 01:24:51  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:53:07  ofri
//  Adding polynomial-related cpp files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.9  2003/12/11 23:03:38  ofri
//  Declareing VariableIndexType as a signed integer instead of unsigned
//  for better robustness.
//
//  Revision 1.8  2003/12/01 16:11:01  ofri
//  Cross compiler compatibility with gcc/.net
//
//  Revision 1.7  2003/11/18 15:43:42  ofri
//  cisstNumerical: changes related mostly to signed/unsigned MSVC7 warning
//  fixes
//
//  Revision 1.6  2003/10/15 19:19:42  ofri
//  Changes to nmrBernsteinPolynomialLineIntegral:
//  1 Re-formatted documentation to doxygen
//  2 Externalized the segment length argument, and added documentation
//  3 Deprecated the profiling info and removed it from function declarations
//
//  Revision 1.5  2003/08/25 22:21:45  ofri
//  Factored out the functionality of nmrStandardPolynomial into
//  nmrMultiVariablePowerBasis in cisstBernsteinPolynomialLineIntegral.
//  The current version contains conditional compilation options for using
//  the factored variables, tableau of powers, and caching the basis variables.
//  They should be tested for correctness and efficiency, and eventually
//  cleared so that only one of the eight options is active.
//
//  Revision 1.4  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************
