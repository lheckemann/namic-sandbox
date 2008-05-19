/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: cmnPrintf.cpp,v 1.3 2006/06/30 13:38:47 ofri Exp $

Author(s):  Ofri Sadowsky
Created on: 2006-02-15

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

#include <cisstCommon/cmnPrintf.h>
#include <stdio.h>
#include <string.h>

const char * cmnPrintfParser::TypeIdCharset = "cCdiouxXeDfgGnpsS%";
const char * cmnPrintfParser::IntegerTypeIds = "cCdiouxX";
const char * cmnPrintfParser::FloatTypeIds = "eEfgG";
const char * cmnPrintfParser::StringTypeIds = "sS";

#ifdef CISST_COMPILER_IS_MSVC
#define snprintf _snprintf
#endif

cmnPrintfParser::cmnPrintfParser(std::ostream & output, const cmnPrintf & outputFormatter)
: OutputStream(output)
, OutputFormatter(outputFormatter)
{
    NextFormatTextPosition = OutputFormatter.GetFormat();
    NextFormatTextCharacter = *NextFormatTextPosition;
    FormatSequence = 0;
    NextTypeIdCharacter = 0;

    DumpUntilPercent();
    ProcessPercent();
}

void cmnPrintfParser::DumpUntilPercent()
{
    if (NextFormatTextPosition == 0)
        return;
    char * firstPosition = NextFormatTextPosition;
    *firstPosition = NextFormatTextCharacter;
    NextFormatTextPosition = strchr(firstPosition, '%');
    if (NextFormatTextPosition != 0) {
        *NextFormatTextPosition = 0;
        NextFormatTextCharacter = '%';
    }
    OutputStream << firstPosition;
}

void cmnPrintfParser::ProcessPercent()
{
    if (NextFormatTextPosition == 0)
        return;
    *NextFormatTextPosition = NextFormatTextCharacter;
    char * firstPosition = NextFormatTextPosition;
    NextFormatTextPosition = strpbrk(NextFormatTextPosition+1, TypeIdCharset);
    if (NextFormatTextPosition == 0) {
        return;
    }

    if (*NextFormatTextPosition == '%') {
        OutputStream << "%";
        ++NextFormatTextPosition;
        NextFormatTextCharacter = *NextFormatTextPosition;
        DumpUntilPercent();
        ProcessPercent();
        return;
    }

    NextTypeIdCharacter = *NextFormatTextPosition;
    ++NextFormatTextPosition;
    NextFormatTextCharacter = *NextFormatTextPosition;
    *NextFormatTextPosition = 0;
    FormatSequence = firstPosition;
    return;
}

bool cmnPrintfParser::NextTypeIdCharIsOneOf(const char * typeIdCharset) const
{
    if (strchr(typeIdCharset, NextTypeIdCharacter)) {
        return true;
    }
    return false;
}


bool cmnTypePrintf(cmnPrintfParser & parser, const int number)
{
    if (parser.NextTypeIdCharIsOneOf(cmnPrintfParser::FloatTypeIds)) {
        return cmnTypePrintf(parser, static_cast<double>(number));
    }

    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::IntegerTypeIds)) {
        std::cerr << "cmnTypePrintf : Expected type identified by <" 
            << parser.GetNextTypeIdCharacter()
            << ">, received int instead.  Suspending output"
            << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
        parser.GetNextFormatSequence(), number);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}


bool cmnTypePrintf(cmnPrintfParser & parser, const unsigned int number)
{
    return cmnTypePrintf(parser, static_cast<int>(number));
}


bool cmnTypePrintf(cmnPrintfParser & parser, const double number)
{
    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::FloatTypeIds)) {
        std::cerr << "FormatterStream::Format : Expected type identified by <"
            << parser.GetNextTypeIdCharacter()
            << ">, received double instead.  Suspending output"
            << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
        parser.GetNextFormatSequence(), number);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}


bool cmnTypePrintf(cmnPrintfParser & parser, const char * text)
{
    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::StringTypeIds)) {
        std::cerr << "FormatterStream::Format : Expected type identified by <"
            << parser.GetNextTypeIdCharacter()
            << ">, received char * instead.  Suspending output"
            << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
        parser.GetNextFormatSequence(), text);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnPrintf.cpp,v $
//  Revision 1.3  2006/06/30 13:38:47  ofri
//  cmnPrintf : Added explicit specialization for unsigned int.
//
//  Revision 1.2  2006/02/16 16:37:42  anton
//  cmnPrintf: Modified conditional compilation for _snprintf to support all
//  versions of Microsoft compilers (so far, .net 2000 and 2003).
//
//  Revision 1.1  2006/02/16 00:21:00  ofri
//  Added formatted output files cmnPrintf and vctPrintf in the respective
//  directories (code, include)
//
//
// ****************************************************************************
