/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: nmrMultiVariablePowerBasis.cpp,v 1.6 2006/06/20 14:04:56 ofri Exp $

Author(s):  Ofri Sadowsky
Created on:   2003-08-20

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


#include <cisstNumerical/nmrMultiVariablePowerBasis.h>

void nmrMultiVariablePowerBasis::Initialize(VariableIndexType numVariables, 
                                            PowerType maxPower)
{
    const PowerType numColumns = std::max(maxPower + 1, 2);
    BasisContainer.SetSize(numVariables, numColumns);
    BasisContainer.SetAll(0.0);
    BasisContainer.Column(0).SetAll(1.0);
}


void nmrMultiVariablePowerBasis::PrivateSetVariable(VariableIndexType varIndex,
                                                    VariableType value)
{
    const PowerType maxDegree = this->GetMaxDegree();
    VariableType * endPower = BasisContainer.Pointer(varIndex, maxDegree + 1);
    VariableType * currentPower = BasisContainer.Pointer(varIndex, 1);
    BasisContainer.Element(varIndex, 0) = 1.0;
    for (; currentPower != endPower; ++currentPower) {
        *currentPower = *(currentPower - 1) * value;
    }
}

void nmrMultiVariablePowerBasis::StandardPowerBasis::SetVariables(
    const VariableType vars[])
{
    VariableIndexType v;
    VariableIndexType numVars = GetNumVariables();
    for (v = 0; v < numVars; v++) {
        SetVariable(v, vars[v]);
    }
}

nmrMultiVariablePowerBasis::BarycentricBasis::BarycentricBasis(
    VariableIndexType numVariables, PowerType maxPower, VariableIndexType implicitVarIndex)
    : BaseType(numVariables, maxPower)
    , ImplicitVarIndex( ((implicitVarIndex < 0) || (implicitVarIndex >= numVariables)) 
    ? numVariables - 1 : implicitVarIndex)
{
    PrivateSetVariable(ImplicitVarIndex, 1);
}

void nmrMultiVariablePowerBasis::BarycentricBasis::SetVariable(
    VariableIndexType varIndex, VariableType value)
{
    CMN_ASSERT(CanSetVariable(varIndex));

    // newNu stores the new value of (1 - sum_of_all_free_variables).
    const VariableType oldValue = GetVariable(varIndex);
    const VariableType oldImplicit = GetImplicitVariable();
    const VariableType newImplicit = oldImplicit + oldValue - value;

    PrivateSetVariable(varIndex, value);

    PrivateSetVariable(ImplicitVarIndex, newImplicit);

}

void nmrMultiVariablePowerBasis::BarycentricBasis::SetVariables(
    const VariableType vars[])
{
    VariableType sumValues = 0.0;
    VariableIndexType v;
    const VariableIndexType numVars = this->GetNumVariables();
    for (v = 0; v < numVars; ++v) {
        if (v == ImplicitVarIndex)
            continue;

        PrivateSetVariable(v, vars[v]);
        sumValues += vars[v];
    }

    PrivateSetVariable(ImplicitVarIndex, 1.0 - sumValues);
}


nmrMultiVariablePowerBasis::ValueType 
nmrMultiVariablePowerBasis::EvaluatePowerProduct(const PowerType powers[]) const
{
    ValueType result = 1;
    VariableIndexType v;
    const VariableIndexType numVars = GetNumVariables();
    for (v = 0; v < numVars; v++) {
        result *= GetVariablePower(v, powers[v]);
    }

    return result;
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrMultiVariablePowerBasis.cpp,v $
//  Revision 1.6  2006/06/20 14:04:56  ofri
//  nmrMultiVariablePowerBasis.cpp : correction for signed/unsigned bug.
//
//  Revision 1.5  2006/06/12 19:48:06  ofri
//  nmrMultiVariablePowerBasis: Now using vctDynamicMatrix to cache variable
//  powers, eliminating the use of vctSingleVariablePowerBasis altogether.
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
//  Revision 1.4  2003/11/18 15:43:42  ofri
//  cisstNumerical: changes related mostly to signed/unsigned MSVC7 warning
//  fixes
//
//  Revision 1.3  2003/08/25 22:16:48  ofri
//  Factored out nmrPolynomialBase::EvaluatePowerProduct() to
//  nmrMultiVariablePowerBasis
//
//  Revision 1.2  2003/08/21 19:04:58  ofri
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
//  Revision 1.1  2003/08/21 18:00:43  ofri
//  Added classes nmrSingleVariablePowerBasis, nmrMultiVariablePowerBasis,
//  nmrMultiVariablePowerBasis::StandardPowerBasis,
//  nmrMultiVariablePowerBasis::BarycentricBasis
//
//
// ****************************************************************************
