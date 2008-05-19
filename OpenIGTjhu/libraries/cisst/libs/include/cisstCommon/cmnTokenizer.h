/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnTokenizer.h,v 1.7 2005/09/26 15:41:46 anton Exp $

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


/*!
  \file cmnTokenizer.h
  \brief Break strings into tokens.
*/

#ifndef _cmnTokenizer_h
#define _cmnTokenizer_h


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

#include <vector>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>

/*!  cmnTokenizer provides a convenient interface for parsing a string
  into a set of tokens.  The parsing uses several sets of control
  characters:

  Delimiters: Separate tokens.  A sequence of delimiters which is not
  quoted or escaped is ignored, and a new token begins after it.

  Quote markers: Enclose parts of, or complete, tokens.  Anything
  between a pair of identical quote markers is included in a token.

  Escape markers: Quote the character immediately following them.

  The default values for delimiters is whitespace (space, tab, CR);
  for quote markers is the set of double and single quotation marks
  (",'); for escape markers is backslash (\).  But the user can
  override them by calling the appropriate method.

  A cmnTokenizer object maintains an internal copy of the tokenized
  text, and can return a pointer to an array of pointer, after the
  fashion of argv.

  \note It is important to note that the stored tokens have the life
  span of the tokenizer.  If the tokenizer is destroyed, the user
  cannot access any of the tokens. */
class  CISST_EXPORT  cmnTokenizer { public:
  cmnTokenizer();

    ~cmnTokenizer();

    void SetDelimiters(const char * delimiters)
    {
        Delimiters = delimiters;
    }

    void SetQuoteMarkers(const char * markers)
    {
        QuoteMarkers = markers;
    }

    void SetEscapeMarkers(const char * markers)
    {
        EscapeMarkers = markers;
    }

    const char * GetDelimiters() const
    {
        return Delimiters;
    }

    const char * GetQuoteMarkers() const
    {
        return QuoteMarkers;
    }

    const char * GetEscapeMarkers() const
    {
        return EscapeMarkers;
    }

    static const char * GetDefaultDelimiters()
    {
        return DefaultDelimiters;
    }

    static const char * GetDefaultQuoteMarkers()
    {
        return DefaultQuoteMarkers;
    }

    static const char * GetDefaultEscapeMarkers()
    {
        return DefaultEscapeMarkers;
    }

    /*! Parse the input string and store the tokens internally.
      If there is a syntax error (e.g., unclosed quotes) throw
      an exception.
      \param string the text to be parsed.
    */
    void Parse(const char * string) throw(std::runtime_error);


    void Parse(const std::string& string) throw(std::runtime_error) {
        Parse(string.c_str());
    }


    /*! Return the number of tokens stored. */
    int GetNumTokens() const
    {
        return Tokens.size();
    }


    /*!
      Return the array of tokens in an argv fashion.
      
      \note This parsing returns exactly the tokens in the input
      string.  For an argv-style set of argument, one needs to have
      the "name of the program" argument in index 0, and the arguments
      starting at index 1.  Use the method GetArgvTokens() for
      that.
    */
    const char * const * GetTokensArray() const
    {
        if (Tokens.empty())
            return NULL;
        return &(Tokens[0]);
    }

    /*! This method will fill the input vector with the tokens, but
      first set the 0-index element to NULL, to follow the argv
      convention, where argv[0] contains the "name of the program". */
    void GetArgvTokens(std::vector<const char *> & argvTokens) const;

private:
    const char * Delimiters;
    const char * QuoteMarkers;
    const char * EscapeMarkers;

    static const char * const DefaultDelimiters;
    static const char * const DefaultQuoteMarkers;
    static const char * const DefaultEscapeMarkers;


    std::vector<char> StringBuffer;
    std::vector<const char *> Tokens;
};

#endif  // _cmnTokenizer_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnTokenizer.h,v $
//  Revision 1.7  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.6  2005/09/23 23:59:49  anton
//  cmnTokenizer: Use cmnThrow for all exceptions.
//
//  Revision 1.5  2005/07/21 01:58:37  alamora
//  added CISST_EXPORT for shared libraries
//
//  Revision 1.4  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.3  2005/04/26 03:25:12  anton
//  cmnTokenizer: Added Parse(std::string) and used NULL instead of "0" for
//  pointers.
//
//  Revision 1.2  2005/04/25 15:40:34  ofri
//  cmnTokenizer: Added methods GetNumTokens() and GetArgvTokens()
//
//  Revision 1.1  2005/04/22 23:50:24  ofri
//  Added class cmnTokenizer to the repository.  It can be use to break strings
//  (commands, paths, etc.) into tokens.
//
//
// ****************************************************************************

