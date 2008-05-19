/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnTokenizer.cpp,v 1.6 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on:  2003-06-09

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


#include <cisstCommon/cmnTokenizer.h>

cmnTokenizer::cmnTokenizer()
    : Delimiters(DefaultDelimiters)
    , QuoteMarkers(DefaultQuoteMarkers)
    , EscapeMarkers(DefaultEscapeMarkers)
{}

cmnTokenizer::~cmnTokenizer()
{}

const char * const cmnTokenizer::DefaultDelimiters = " \t\n\012";
const char * const cmnTokenizer::DefaultEscapeMarkers = "\\";
const char * const cmnTokenizer::DefaultQuoteMarkers = "\"'";

void cmnTokenizer::Parse(const char * string) throw(std::runtime_error)
{
    const size_t stringLength = strlen(string);
    StringBuffer.clear();
    StringBuffer.reserve(stringLength+1);
    StringBuffer.push_back(0);
    Tokens.clear();

    bool inToken = false;
    int lastQuote = 0;
    const char * currentCharPtr = string;
    const char * lastCharPtr = string + stringLength;

    for (; currentCharPtr < lastCharPtr; ++currentCharPtr) {
        int currentChar = *currentCharPtr;
        // skip all the delimiters outside of a token
        if (!inToken && (strchr(Delimiters, currentChar) != 0))
            continue;

        if (!inToken)
            inToken = true;

        // If encountered an escaped character, push the following character to the buffer,
        // and continue to the one after.  It follows that escaped characters are always
        // pushed.
        if (strchr(EscapeMarkers, currentChar) != 0) {
            ++currentCharPtr;
            currentChar = *currentCharPtr;
            StringBuffer.push_back(currentChar);
            continue;
        }

        // If encountered delimiter outside of quotes and not escaped, end the token and
        // switch the state of inToken.
        if ( (lastQuote == 0) && (strchr(Delimiters, currentChar) != 0) ) {
            inToken = false;
            StringBuffer.push_back(0);
            continue;
        }

        // If encountered quote mark outside of a quote, move to the following character
        // and store the quote mark.
        if ( (lastQuote == 0) && (strchr(QuoteMarkers, currentChar) != 0) ) {
            lastQuote = currentChar;
            continue;
        }

        // If encountered the opening quote mark, close the quote.  If the character immediately
        // following the quote is a delimiter, terminate the token.
        if (lastQuote == currentChar) {
            lastQuote = 0;
            if (strchr(Delimiters, *(currentCharPtr+1)) != 0)
                StringBuffer.push_back(0);
            inToken = false;
            continue;
        }

        // Otherwise, just push the next character to the buffer
        StringBuffer.push_back(currentChar);
    }
    StringBuffer.push_back(0);

    if (lastQuote != 0) {
        cmnThrow(std::runtime_error("mscTokenizer::Parse -- unclosed quotes"));
    }

    // Now after parsing the string, create a list of pointers to the tokens
    size_t bufferSize = StringBuffer.size();
    size_t index = 0;
    while (index < bufferSize) {
        if (StringBuffer[index] == 0) {
            ++index;
            continue;
        }

        Tokens.push_back(&(StringBuffer[index]));
        while (StringBuffer[index] != 0) {
            ++index;
        }
    }

    // Insert a null pointer at the end of the list.
    Tokens.push_back(NULL);
}


void cmnTokenizer::GetArgvTokens(std::vector<const char *> & argvTokens) const
{
    const unsigned int numTokens = GetNumTokens();
    argvTokens.resize(numTokens+1);
    argvTokens[0] = 0;
    std::copy( &(Tokens[0]), &(Tokens[0]) + numTokens, &(argvTokens[1]) );
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnTokenizer.cpp,v $
//  Revision 1.6  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.5  2005/09/23 23:59:49  anton
//  cmnTokenizer: Use cmnThrow for all exceptions.
//
//  Revision 1.4  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.3  2005/04/26 03:25:12  anton
//  cmnTokenizer: Added Parse(std::string) and used NULL instead of "0" for
//  pointers.
//
//  Revision 1.2  2005/04/25 15:40:26  ofri
//  cmnTokenizer: Added methods GetNumTokens() and GetArgvTokens()
//
//  Revision 1.1  2005/04/22 23:50:18  ofri
//  Added class cmnTokenizer to the repository.  It can be use to break strings
//  (commands, paths, etc.) into tokens.
//
//
// ****************************************************************************

