#
# $Id: CMakeLists.txt,v 1.30 2006/06/27 13:57:44 ofri Exp $
#
# CMakeLists for cisstNumerical
#

# --- begin cisst license - do not edit ---
# 
# CISST Software License Agreement(c)
# 
# Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.
# 
# This software ("CISST Software") is provided by The Johns Hopkins
# University on behalf of the copyright holders and
# contributors. Permission is hereby granted, without payment, to copy,
# modify, display and distribute this software and its documentation, if
# any, for research purposes only, provided that (1) the above copyright
# notice and the following four paragraphs appear on all copies of this
# software, and (2) that source code to any modifications to this
# software be made publicly available under terms no more restrictive
# than those in this License Agreement. Use of this software constitutes
# acceptance of these terms and conditions.
# 
# The CISST Software has not been reviewed or approved by the Food and
# Drug Administration, and is for non-clinical, IRB-approved Research
# Use Only.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
# EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, AND NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
# --- end cisst license ---


# set the library name
SET(LIBRARY cisstNumerical)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector)

# all source files
SET(SOURCE_FILES
    nmrBernsteinPolynomial.cpp
    nmrBernsteinPolynomialLineIntegral.cpp
    nmrGaussJordanInverse.cpp
    nmrMultiIndexCounter.cpp
    nmrMultiVariablePowerBasis.cpp
    nmrPolynomialBase.cpp
    nmrPolynomialTermPowerIndex.cpp
    nmrSingleVariablePowerBasis.cpp
    nmrStandardPolynomial.cpp
    )


# all header files
SET(HEADER_FILES
    nmrBernsteinPolynomial.h
    nmrBernsteinPolynomialLineIntegral.h
    nmrDynAllocPolynomialContainer.h
    nmrExport.h
    nmrGaussJordanInverse.h
    nmrIsOrthonormal.h
    nmrMultiIndexCounter.h
    nmrMultiVariablePowerBasis.h
    nmrPolynomialBase.h
    nmrPolynomialContainer.h
    nmrPolynomialTermPowerIndex.h
    nmrSingleVariablePowerBasis.h
    nmrStandardPolynomial.h
    )

IF(CISST_HAS_NETLIB)
  SET(SOURCE_FILES
      ${SOURCE_FILES}
      nmrLDPSolver.cpp
      nmrLSISolver.cpp
      nmrLSqLin.cpp
      nmrPInverse.cpp
      nmrPInverseEconomy.cpp
      nmrSVD.cpp
      nmrSVDEconomy.cpp
      )
  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrInverse.h
      nmrLDPSolver.h
      nmrLSSolver.h
      nmrLSISolver.h
      nmrLSqLin.h
      nmrLU.h
      nmrLUSolver.h
      nmrHFTISolver.h
      nmrNNLSSolver.h
      nmrCallBack.h
      nmrPInverse.h
      nmrPInverseEconomy.h
      nmrSVD.h
      nmrSVDEconomy.h
      nmrSVDSolver.h
      nmrSVDRSSolver.h
      )
ENDIF(CISST_HAS_NETLIB)

# add files if cnetlib is used
IF(CISST_HAS_CNETLIB)

  SET(HEADER_FILES
      ${HEADER_FILES}
      nmrFnSolver.h
      nmrFminSolver.h
      nmrFnJacobianSolver.h
      nmrLSEISolver.h
      nmrLSNonLinSolver.h
      nmrLSNonLinJacobianSolver.h
      nmrPInverseSolver.h
      )

ENDIF(CISST_HAS_CNETLIB)


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Windows Dll require to link      
IF(CISST_HAS_CNETLIB)
  TARGET_LINK_LIBRARIES(cisstNumerical ${CNETLIB_LIBRARIES})
ENDIF(CISST_HAS_CNETLIB)

IF(CISST_HAS_CISSTNETLIB)
  TARGET_LINK_LIBRARIES(cisstNumerical
                        ${CISSTNETLIB_LIBRARIES}
                        ${CISSTNETLIB_F95_LIBRARIES}
                        ${CISSTNETLIB_GCC_LIBRARIES})
ENDIF(CISST_HAS_CISSTNETLIB)


#
# $Log: CMakeLists.txt,v $
# Revision 1.30  2006/06/27 13:57:44  ofri
# cisstNumerical/CMakeLists.txt : Following check-in [2016] (and ticket #228)
# I restored the polynomial classes to the build.
#
# Revision 1.29  2006/06/19 01:55:46  kapoor
# cisstNumerical: Memory efficient methods for SVD and pseudo-inverse.
# Currently tested and works for VCT_COL_MAJOR format only and for dynamic
# matrices.
#
# Revision 1.28  2006/01/28 06:16:25  anton
# cisstNumerical: Added nmrInverse (see #207).
#
# Revision 1.27  2006/01/11 20:49:18  anton
# cisstNumerical CMake: Added a variable to indicate that either netlib is
# available and factor most of the CMake code (see CISST_HAS_NETLIB).
#
# Revision 1.26  2006/01/10 23:25:33  anton
# cisstNumerical: Incomplete but somewhat working version of nmrLU.
#
# Revision 1.25  2006/01/04 22:53:50  ofri
# cisstNumerical/CMakeLists.txt updated for gauss-jordan
#
# Revision 1.24  2005/12/28 21:59:01  anton
# cisstNumerical: Removed nmrCommon.h since it has become obsolete.
#
# Revision 1.23  2005/12/07 23:19:44  anton
# cisstNumerical CMakeLists.txt: Added missing libraries to link against if
# cisstNetlib is used (libgcc and libf95).  For DLLs only.
#
# Revision 1.22  2005/11/30 06:02:27  kapoor
# cisstNumerical: Added LSqLin to be compiled with cisstNetlib
#
# Revision 1.21  2005/11/29 03:01:42  anton
# cisstNumerical CMakeLists.txt: Added more files compiled with cisstNetlib.
#
# Revision 1.20  2005/11/28 22:04:44  kapoor
# cisstNumerical: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
#
# Revision 1.19  2005/11/20 21:35:18  kapoor
# LAPACK: Added CISST_HAS_CISSTCNETLIB flag to cmake, which can be used to
# compile code using the newer LAPACK3E. CLAPACK is still supported. See
# ticket #193.
#
# Revision 1.18  2005/10/21 23:13:27  anton
# nmrSVD: Working version for gcc 3.3, gcc 4.0 and VCPP7.1 (still some work
# to do, see #186).  Corrected CMakeLists.txt (see #185).
#
# Revision 1.17  2005/10/19 01:42:26  kapoor
# New version of nmrSVD, replaces existing API. See ticket #184 for details.
#
# Revision 1.16  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.15  2005/09/01 06:37:22  anton
# cisstNumerical CMakeLists: Always link against cnetlib, this seems to always
# work.
#
# Revision 1.14  2005/08/17 20:03:59  kapoor
# cisstNumerical: Added LSEI (a more recent version of Hansons' Code)
#
# Revision 1.13  2005/08/15 06:08:56  anton
# cisstNumerical CMakeLists.txt: If the libraries are compiled as shared,
# MacOSX requires to link with cnetlib (As does Windows).
#
# Revision 1.12  2005/08/13 23:34:57  kapoor
# Added NLP solver.
#
# Revision 1.11  2005/08/04 18:51:07  anton
# cisstNumerical: Added nmrLUSolver.
#
# Revision 1.10  2005/07/30 09:46:28  kapoor
# Numerical: Added wrappers for MINPACK for Powells method and
# Levenberg-Marquardt method (using analytic and approximate - BFGS jacobians)
#
# Revision 1.9  2005/07/29 00:09:53  kapoor
# cisstNumerical: Added wrappers for Least-Squares (LS) and pseudo-inverse.
#
# Revision 1.8  2005/07/27 22:38:29  anton
# cisstNumerical: Added nmrIsOrthonormal function.  This needs to be peer
# reviewed.
#
# Revision 1.7  2005/06/17 22:28:46  anton
# cisstNumerical CMakeLists.txt: Typo in CNETLIB_LIBRARIES
#
# Revision 1.6  2005/06/03 18:20:58  anton
# cisstNumerical: Added license.
#
# Revision 1.5  2005/01/10 19:19:55  anton
# cisstNumerical: Updates to allow compilation as a Dll.
#
# Revision 1.4  2004/12/15 05:46:06  kapoor
# Added HFTI code from L&H
#
# Revision 1.3  2004/11/01 17:44:48  anton
# cisstNumerical: Added some Solvers and the files to use our own cnetlib
# code added to the cisst CVS repository (module "cnetlib").
#
# Revision 1.2  2004/10/27 14:42:23  anton
# cisstNumerical: Added nmrSVDSolver based on netlib/CLAPACK function.
#
# Revision 1.1  2004/10/21 19:53:07  ofri
# Adding polynomial-related cpp files to the repository -- imported from Ofri.
# See ticket #78.
#
#
