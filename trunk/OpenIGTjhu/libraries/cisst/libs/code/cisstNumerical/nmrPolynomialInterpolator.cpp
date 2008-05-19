/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPolynomialInterpolator.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $

  Author(s):  Ofri Sadowsky
  Created on:   2003-07-09

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


#include <cisstNumerical/nmrPolynomialInterpolator.h>
#include <vnl/algo/vnl_svd.h>

std::vector<nmrPolynomialInterpolator::CoefficientType>
nmrPolynomialInterpolator::FitCoefficients(
        const nmrDynAllocPolynomialContainer & polynomial,
        unsigned int numPoints, InterpolationPointType interpolationPoints[],
        const ValueType functionValues[])
{
    const nmrPolynomialBase::TermCounterType numTerms = polynomial.GetNumberOfTerms();
    assert( numPoints >= numTerms );
    const unsigned int numVariables = polynomial.GetNumVariables();

    InterpolationMatrixType equationMatrix;

    nmrMultiVariablePowerBasis::StandardPowerBasis 
        powerBasis(polynomial.GetNumVariables(), polynomial.GetMaxDegree());

    GenerateTermMatrixForPoints(polynomial, numPoints, interpolationPoints, equationMatrix);

    std::vector<CoefficientType> result(numTerms, CoefficientType(0));

    vnl_svd<CoefficientType> vnlSvd(equationMatrix);

    int rc = vnlSvd.rank();

    vnlSvd.solve(functionValues, &(result.front()));

    return result;
}

void nmrPolynomialInterpolator::GenerateTermMatrixForPoints(
    const nmrDynAllocPolynomialContainer & polynomial,
    unsigned int numPoints,
    InterpolationPointType interpolationPoints[],
    InterpolationMatrixType & result)
{
    nmrPolynomialBase::TermCounterType numTerms = polynomial.GetNumberOfTerms();
    result.set_size(numPoints, numTerms);

    nmrMultiVariablePowerBasis::StandardPowerBasis 
        powerBasis(polynomial.GetNumVariables() , polynomial.GetMaxDegree());

    nmrDynAllocPolynomialContainer::TermConstIteratorType termIterator;


    unsigned int pointIndex = 0;
    for (pointIndex = 0; pointIndex < numPoints; ++pointIndex)
    {
        powerBasis.SetVariables( interpolationPoints[pointIndex] );
        polynomial.EvaluateBasisVector( powerBasis, result[pointIndex] );
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrPolynomialInterpolator.cpp,v $
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
//  Revision 1.5  2003/12/24 17:57:46  ofri
//  Updates to cisNumerical:
//  1) Using #include <...> instead of #include "..." for cisst header files
//  2) Getting rid of cisMatrixVector in most places (if not all), replacing it with
//  vnl
//
//  Revision 1.4  2003/09/29 23:29:23  ofri
//  changed the definition of nmrPolynomialInterpolator::InterpolationPointType
//  and added some subsequent chages.  This due to const compilation issues.
//
//  Revision 1.3  2003/09/26 01:14:36  ofri
//  Changed interface of nmrPolynomialInterpolator
//
//  Revision 1.2  2003/07/16 22:05:45  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.1  2003/07/10 15:45:04  ofri
//  Adding class nmrPolynomialInterpolator to the repository
//
//
// ****************************************************************************
