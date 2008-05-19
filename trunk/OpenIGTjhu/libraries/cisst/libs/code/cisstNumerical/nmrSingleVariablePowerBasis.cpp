/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSingleVariablePowerBasis.cpp,v 1.5 2006/06/12 19:43:28 ofri Exp $

  Author(s):  Ofri Sadowsky
  Created on: 2003-08-20

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


#include <cisstNumerical/nmrSingleVariablePowerBasis.h>

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(push)
#pragma warning(disable:4996)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)

void nmrSingleVariablePowerBasis::SetVariable(VariableType variable)
{
    PowersContainerType::const_iterator formerPowerIterator =
        PowersContainer.begin();
    PowersContainerType::iterator currentPowerIterator =
        PowersContainer.begin() + 1;
    PowersContainerType::iterator endPowerIterator =
        PowersContainer.end();
    nmrPolynomialTermPowerIndex::PowerType power = 1;
    for (; currentPowerIterator != endPowerIterator; 
    ++currentPowerIterator, ++power) {
        *currentPowerIterator = (*formerPowerIterator) * (*formerPowerIterator);
        if ( (power % 2) != 0 ) {
            *currentPowerIterator *= variable;
            ++formerPowerIterator;
        }
    }
}

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(pop)
#endif  // if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSingleVariablePowerBasis.cpp,v $
//  Revision 1.5  2006/06/12 19:43:28  ofri
//  nmrSingleVariablePowerBasis: class is declared deprecated and will be
//  removed from future releases.
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
//  Revision 1.2  2003/09/29 23:28:19  ofri
//  nmrSingleVariablePowerBasis now evaluates powers of the variable
//  using p(x,n) = p(x, n/2)^2 -- nore stable numerically.
//  (not tested)
//
//  Revision 1.1  2003/08/21 18:00:43  ofri
//  Added classes nmrSingleVariablePowerBasis, nmrMultiVariablePowerBasis,
//  nmrMultiVariablePowerBasis::StandardPowerBasis,
//  nmrMultiVariablePowerBasis::BarycentricBasis
//
//
// ****************************************************************************
