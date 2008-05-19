/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnTypeTraits.cpp,v 1.13 2006/06/23 18:01:53 ofri Exp $
  
  Author(s):  Anton Deguet
  Created on: 2004-01-23

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


#include <cisstCommon/cmnTypeTraits.h>


// ---------- float ----------
static float zeroFloat = 0.0f;
template<>
float & cmnTypeTraits<float>::ToleranceValue()
{
    static float tolerance = 1.0e-5f;
    return tolerance;
}

template<>
const float cmnTypeTraits<float>::DefaultTolerance = cmnTypeTraits<float>::ToleranceValue();

template<>
std::string cmnTypeTraits<float>::TypeName()
{
    return "float";
}

template<>
float cmnTypeTraits<float>::PlusInfinity()
{
    static const float inf = 1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::MinusInfinity()
{
    static const float inf = -1.0f / zeroFloat;
    return inf;
}

template<>
float cmnTypeTraits<float>::NaN()
{
    static const float nan = zeroFloat / zeroFloat;
    return nan;
}



// ---------- double ----------
static double zeroDouble = 0.0;
template<>
double & cmnTypeTraits<double>::ToleranceValue()
{
    static double tolerance = 1.0e-9;
    return tolerance;
}

template<>
const double cmnTypeTraits<double>::DefaultTolerance = cmnTypeTraits<double>::ToleranceValue();

template<>
std::string cmnTypeTraits<double>::TypeName()
{
    return "double";
}

template<>
double cmnTypeTraits<double>::PlusInfinity()
{
    static const double inf = 1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::MinusInfinity()
{
    static const double inf = -1.0 / zeroDouble;
    return inf;
}

template<>
double cmnTypeTraits<double>::NaN()
{
    static const double nan = zeroDouble / zeroDouble;
    return nan;
}


// ---------- int ----------
template<>
int & cmnTypeTraits<int>::ToleranceValue()
{
    static int tolerance = 0;
    return tolerance;
}

template<>
const int cmnTypeTraits<int>::DefaultTolerance = cmnTypeTraits<int>::ToleranceValue();

template<>
std::string cmnTypeTraits<int>::TypeName()
{
    return "int";
}

template<>
int cmnTypeTraits<int>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
int cmnTypeTraits<int>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
int cmnTypeTraits<int>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- char ----------
template<>
char & cmnTypeTraits<char>::ToleranceValue()
{
    static char tolerance = 0;
    return tolerance;
}

template<>
const char cmnTypeTraits<char>::DefaultTolerance = cmnTypeTraits<char>::ToleranceValue();

template<>
std::string cmnTypeTraits<char>::TypeName()
{
    return "char";
}

template<>
char cmnTypeTraits<char>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
char cmnTypeTraits<char>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
char cmnTypeTraits<char>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- short ----------
template<>
short & cmnTypeTraits<short>::ToleranceValue()
{
    static short tolerance = 0;
    return tolerance;
}

template<>
const short cmnTypeTraits<short>::DefaultTolerance = cmnTypeTraits<short>::ToleranceValue();

template<>
std::string cmnTypeTraits<short>::TypeName()
{
    return "short";
}

template<>
short cmnTypeTraits<short>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
short cmnTypeTraits<short>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
short cmnTypeTraits<short>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- long ----------
template<>
long & cmnTypeTraits<long>::ToleranceValue()
{
    static long tolerance = 0;
    return tolerance;
}

template<>
const long cmnTypeTraits<long>::DefaultTolerance = cmnTypeTraits<long>::ToleranceValue();

template<>
std::string cmnTypeTraits<long>::TypeName()
{
    return "long";
}

template<>
long cmnTypeTraits<long>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
long cmnTypeTraits<long>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
long cmnTypeTraits<long>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned long ----------
template<>
unsigned long & cmnTypeTraits<unsigned long>::ToleranceValue()
{
    static unsigned long tolerance = 0;
    return tolerance;
}

template<>
const unsigned long cmnTypeTraits<unsigned long>::DefaultTolerance = cmnTypeTraits<unsigned long>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned long>::TypeName()
{
    return "unsigned long";
}

template<>
unsigned long cmnTypeTraits<unsigned long>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned long cmnTypeTraits<unsigned long>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned long cmnTypeTraits<unsigned long>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned int ----------
template<>
unsigned int & cmnTypeTraits<unsigned int>::ToleranceValue()
{
    static unsigned int tolerance = 0;
    return tolerance;
}

template<>
const unsigned int cmnTypeTraits<unsigned int>::DefaultTolerance = cmnTypeTraits<unsigned int>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned int>::TypeName()
{
    return "unsigned int";
}

template<>
unsigned int cmnTypeTraits<unsigned int>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned int cmnTypeTraits<unsigned int>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned short ----------
template<>
unsigned short & cmnTypeTraits<unsigned short>::ToleranceValue()
{
    static unsigned short tolerance = 0;
    return tolerance;
}

template<>
const unsigned short cmnTypeTraits<unsigned short>::DefaultTolerance = cmnTypeTraits<unsigned short>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned short>::TypeName()
{
    return "unsigned short";
}

template<>
unsigned short cmnTypeTraits<unsigned short>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned short cmnTypeTraits<unsigned short>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}

// ---------- unsigned char ----------
template<>
unsigned char & cmnTypeTraits<unsigned char>::ToleranceValue()
{
    static unsigned char tolerance = 0;
    return tolerance;
}

template<>
const unsigned char cmnTypeTraits<unsigned char>::DefaultTolerance = cmnTypeTraits<unsigned char>::ToleranceValue();

template<>
std::string cmnTypeTraits<unsigned char>::TypeName()
{
    return "unsigned char";
}

template<>
unsigned char cmnTypeTraits<unsigned char>::PlusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MaxPositiveValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::MinusInfinity() {
    // Not meaningful!  Here to avoid link errors
    return MinNegativeValue();
}

template<>
unsigned char cmnTypeTraits<unsigned char>::NaN() {
    // Not meaningful!  Here to avoid link errors
    return 0;
}


// ---------- bool ----------
template<>
bool & cmnTypeTraits<bool>::ToleranceValue()
{
    static bool tolerance = 0;
    return tolerance;
}

template<>
const bool cmnTypeTraits<bool>::DefaultTolerance = cmnTypeTraits<bool>::ToleranceValue();

template<>
std::string cmnTypeTraits<bool>::TypeName()
{
    return "bool";
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnTypeTraits.cpp,v $
//  Revision 1.13  2006/06/23 18:01:53  ofri
//  cmnTypeTraits : added specializations for unsigned int, unsigned short,
//  unsigned char
//
//  Revision 1.12  2005/12/14 20:21:31  ofri
//  cmnTypeTraits: Added type traits for unsigned long
//
//  Revision 1.11  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.10  2005/08/14 00:51:26  anton
//  cisstCommon: Port to gcc-4.0 (use template<> for template specialization).
//
//  Revision 1.9  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.8  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.7  2005/02/03 22:26:38  anton
//  cmnTypeTraits: Added support for native types short and long.
//
//  Revision 1.6  2004/10/25 15:33:51  ofri
//  cmnTypeTraits: Moved definitions of limit functions from cpp files to inline functions
//  in h file.
//
//  Revision 1.5  2004/07/13 13:46:30  ofri
//  cmnTypeTraits: Added methods PlusInfinityOrMax() MinusInfinityOrMin() to
//  facilitate ``idempotent'' min and max operations.  Moved the definitions of
//  HasInfinity() methods to inline functions in the header files, so that they can
//  be optimized to skip the function call.
//
//  Revision 1.4  2004/04/02 21:23:23  anton
//  - Added CISST_EXPORT for method TypeName()
//  - Instantiated Infinity and NaN methods for int and char
//  - Added methods HasInfinity and HasNaN to make sure one can test that these
//    methods are meaningful
//
//  Revision 1.3  2004/04/02 16:23:18  anton
//  Removed old code commented out by #if 0
//
//  Revision 1.2  2004/03/16 21:01:15  ofri
//  All the type dependent constants are now accessed through static methods
//  rather than static members, to prevent initialization problems.  I added a
//  number of such constants.  cmnTypeTraits is no longer specialized for
//  char, float, etc., but uses externalized type-promotion class.  This simplifies
//  adding new members to cmnTypeTraits, which do not have to be replicated
//  for the specialized versions.  cmnTypeTraits.cpp was completely rewritten to
//  use standard header files for constant-value definitions.
//
//  Revision 1.1  2004/02/06 15:38:04  anton
//  Renamed cmnVaTypes to cmnTypeTraits
//  Added Tolerance
//
//
// ****************************************************************************

