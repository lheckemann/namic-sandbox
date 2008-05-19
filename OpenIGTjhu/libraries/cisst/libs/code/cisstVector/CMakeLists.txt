#
# $Id: CMakeLists.txt,v 1.24 2006/02/16 00:25:16 ofri Exp $
#
# CMakeLists for cisstVector
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
SET(LIBRARY cisstVector)

# set dependencies
SET(DEPENDENCIES
    cisstCommon)

# all source files
SET(SOURCE_FILES
    vctAngleRotation2.cpp
    vctAxisAngleRotation3.cpp
    vctFrameBase.cpp
    vctMatrixRotation2.cpp
    vctMatrixRotation2Base.cpp
    vctMatrixRotation3.cpp
    vctMatrixRotation3Base.cpp
    vctPrintf.cpp
    vctQuaternionBase.cpp
    vctQuaternionRotation3.cpp
    vctQuaternionRotation3Base.cpp
    vctRandom.cpp
    vctRodriguezRotation3.cpp
    vctRodriguezRotation3Base.cpp
    vctTypes.cpp
    )

# all header files
SET(HEADER_FILES
    vctAngleRotation2.h
    vctAxisAngleRotation3.h
    vctBarycentricVector.h
    vctBinaryOperations.h
    vctContainerTraits.h
    vctDeterminant.h
    vctDynamicConstMatrixBase.h
    vctDynamicConstMatrixRef.h
    vctDynamicConstVectorBase.h
    vctDynamicConstVectorRef.h
    vctDynamicMatrixBase.h
    vctDynamicMatrixLoopEngines.h
    vctDynamicMatrix.h
    vctDynamicMatrixOwner.h
    vctDynamicMatrixRef.h
    vctDynamicMatrixRefOwner.h
    vctDynamicMatrixTypes.h
    vctDynamicVectorBase.h
    vctDynamicVectorLoopEngines.h
    vctDynamicVectorRef.h
    vctDynamicVectorRefOwner.h
    vctDynamicVector.h
    vctDynamicVectorOwner.h
    vctDynamicVectorTypes.h
    vctExport.h
    vctFixedSizeConstVectorBase.h
    vctFixedSizeConstVectorRef.h
    vctFixedSizeConstMatrixBase.h
    vctFixedSizeConstMatrixRef.h
    vctFixedSizeMatrixBase.h
    vctFixedSizeMatrix.h
    vctFixedSizeMatrixRef.h
    vctFixedSizeMatrixTraits.h
    vctFixedSizeMatrixTypes.h
    vctFixedSizeVectorBase.h
    vctFixedSizeVectorRef.h
    vctFixedSizeVectorRecursiveEngines.h
    vctFixedSizeVectorTraits.h
    vctFixedSizeVector.h
    vctFixedSizeVectorTypes.h
    vctFixedStrideMatrixIterator.h
    vctFixedStrideVectorIterator.h
    vctFixedSizeMatrixLoopEngines.h
    vctFrameBase.h
    vctForwardDeclarations.h
    vctMatrixRotation2.h
    vctMatrixRotation2Base.h
    vctMatrixRotation3.h
    vctMatrixRotation3Base.h
    vctPrintf.h
    vctQuaternion.h
    vctQuaternionBase.h
    vctQuaternionRotation3.h
    vctQuaternionRotation3Base.h
    vctRandom.h
    vctRodriguezRotation3.h
    vctRodriguezRotation3Base.h
    vctStoreBackBinaryOperations.h
    vctStoreBackUnaryOperations.h
    vctTransformationTypes.h
    vctTypes.h
    vctUnaryOperations.h
    vctVarStrideVectorIterator.h
    vctVarStrideMatrixIterator.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

#
# $Log: CMakeLists.txt,v $
# Revision 1.24  2006/02/16 00:25:16  ofri
# Modified CMakeLists files to include cmnPrintf and vctPrintf
#
# Revision 1.23  2005/12/01 22:11:57  anton
# cisstVector: 2D transformations, added a base class for rotation matrix 2.
#
# Revision 1.22  2005/10/08 20:16:24  anton
# vctQuaternionBase: Sepcialized default ctor to perform required memory
# allocation if derived from dynamic vector.
#
# Revision 1.21  2005/10/07 09:26:28  anton
# cisstVector: Split vctTypes in multiples files (see #60 and #160).
#
# Revision 1.20  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.19  2005/09/01 06:18:08  anton
# cisstVector transformations: Added a level of abstraction for all classes
# derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
# vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
# from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
#
# Revision 1.18  2005/05/19 19:29:01  anton
# cisst libs: Added the license to cisstCommon and cisstVector
#
# Revision 1.17  2005/02/24 01:51:50  anton
# cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
# code has not been tested.   The normalization and conversion methods have
# to be reviewed.
#
# Revision 1.16  2005/02/04 16:56:54  anton
# cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
# as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
# classes.  Other updates include vctRandom(rotation), new vctTypes and
# forward declarations.
#
# Revision 1.15  2004/11/12 22:09:48  anton
# cisstVector: Added 4 functions to fill different containers with random
# values.  This is based on cmnRandomSequence.
#
# Revision 1.14  2004/11/11 20:35:46  anton
# cisstVector: *: Added a vctContainerTraits to centralize the declarations
# of size_type, difference_type, reference, const_reference, etc. *: All
# iterators are now derived from std::iterator. *: Added some missing typedef
# for iterators.
#
# Revision 1.13  2004/10/25 19:19:54  anton
# cisstVector:  Created vctForwardDeclarations.h and removed forward
# declarations of classes in all other files.  Added some constructors
# for vector references to reference fixed size from dynamic and vice versa.
#
# Revision 1.12  2004/10/21 20:12:41  anton
# cisstVector: Renaming of vctFrame and related typedefs as specified in
# ticket #72.
#
# Revision 1.11  2004/08/13 17:47:40  anton
# cisstVector: Massive renaming to replace the word "sequence" by "vector"
# (see ticket #50) as well as another more systematic naming for the engines.
# The changes for the API are as follow:
# *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
# *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
# *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
# *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
# *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
# *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
# *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
# *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
# *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
# *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
# *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
# Also updated and corrected the documentation (latex, doxygen, figures) as
# well as the tests and examples.
#
# Revision 1.10  2004/08/04 21:11:10  anton
# cisstVector: Added preliminary version of dynamic matrices.  Lots of work
# is still required, this code is not ready to be used.
#
# Revision 1.9  2004/07/02 20:14:07  anton
# Code for VarStrideSequenceIterator.  Added the class as well as some code to
# try it.  An example is provided in examples/vectorTutorial/main.cpp.
#
# Revision 1.8  2004/07/02 16:17:29  anton
# Massive renaming in cisstVector for the addition of dynamic size vectors
# and matrices.  The dynamic vectors are far from ready.
#
# Revision 1.7  2004/04/20 21:18:09  ofri
# class vctDeterminant added to the repository after being tested on .NET
#
# Revision 1.6  2004/04/02 21:20:28  anton
# Removed vctConstants, replaced by cmnTypeTraits
#
# Revision 1.5  2004/02/18 15:59:40  anton
# vctFixedSizeVector.cpp moved to vctTypes.cpp
# Added vctFrame.{h,cpp}
# vctBarycentricVector.h was missing
#
# Revision 1.4  2004/02/06 15:37:07  anton
# code/cisstVector/CMakeLists.txt
#
# Revision 1.3  2004/01/30 18:57:47  ofri
# Use macro CISST_ADD_LIBRARY_TO_PROJECT instead of CISST_ADD_LIBRARY
#
# Revision 1.2  2004/01/23 18:13:55  anton
# Removed vctVectorAndMatrixOperations.h
# Replaced some tabs by spaces
#
# Revision 1.1  2004/01/22 21:55:42  anton
# We are now using recursive CMakeLists.txt instead of a single root CMakeLists which
# used a macro to create all the libraries.  This should avoid useless recompilations
# with .Net and allow more flexibility for each library.
#
#
# Revision 1.16  2004/01/16 22:54:37  anton
# Added vctRotationQuaternion3
#
# Revision 1.15  2004/01/15 15:20:29  anton
# Added vctRotationMatrix3
#
# Revision 1.14  2004/01/12 18:55:03  anton
# Added vctQuaternion.h
#
# Revision 1.13  2003/12/18 16:24:15  anton
# Added vctMatrixLoopEngines and vctTypes.h
# Removed vctDouble3.h
#
# Revision 1.12  2003/12/09 20:37:44  ofri
# Added vctConstants  .h and .cpp to the cisstVector project.
#
# Revision 1.11  2003/11/20 19:49:55  anton
# ArrayIterator is now SequenceIterator.  Split const and non const classes.  Removed vctVector2 and vctVector3
#
# Revision 1.10  2003/11/05 21:04:24  ofri
# Removed duplicate inclusion of vctSequenceRecursiveEngines.h from cisstVector.cmake
#
# Revision 1.9  2003/11/04 21:55:36  ofri
# Added the fixed size matrix files to the project
#
# Revision 1.8  2003/10/20 19:41:57  anton
# files moved from FizedSizeVector to FixedLengthSequence
#
# Revision 1.7  2003/10/03 19:16:39  anton
# added vctVector{2,3}.h
#
# Revision 1.6  2003/10/02 18:42:34  anton
# removed vctDouble3.cpp
#
# Revision 1.5  2003/09/30 14:03:26  anton
# added code for vctFixedLengthSequence
#
# Revision 1.4  2003/09/24 13:49:01  anton
# added dependencies
#
# Revision 1.3  2003/09/17 14:11:27  anton
# Added vctExport.h and vctFixedSizeVector.cpp
#
# Revision 1.2  2003/09/12 14:28:47  anton
# Added vctDouble
#
# Revision 1.1  2003/09/09 18:54:51  anton
# creation
#
#
