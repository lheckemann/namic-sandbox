/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: cmnPrintf.h,v 1.6 2006/06/30 13:38:47 ofri Exp $

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



/*!
  \file
  \brief Declaration of cmnPrintf and cmnPrintfParser
  \ingroup cisstCommon
*/

#ifndef _cmnPrintf_h
#define _cmnPrintf_h

#include <iostream>
#include <cisstCommon/cmnPortability.h>

// always include last
#include <cisstCommon/cmnExport.h>

/* Forward declarations */
class cmnPrintfParser;

CISST_EXPORT bool cmnTypePrintf(cmnPrintfParser & parser, const int number);
CISST_EXPORT bool cmnTypePrintf(cmnPrintfParser & parser, const unsigned int number);
CISST_EXPORT bool cmnTypePrintf(cmnPrintfParser & parser, const double number);
CISST_EXPORT bool cmnTypePrintf(cmnPrintfParser & parser, const char * text);


/*!
  \brief printf-like formatted output for streams

  \ingroup cisstCommon
  
  This is the user interface for output formatting with C++ streams
  after the fashion of printf.

  A usage example:

  \code
  std::cout << cmnPrintf("Hello %.4f World\n") << 12.345;
  \endcode
  
  prints the text (followed by a \n newline)
  
  \verbatim
  Hello 12.3450 World
  \endverbatim
  
  cmnPrintf can be "streamed" into any object derived from the
  std::ostream base class.  The formatting rules are the same as in
  the standard C routines printf, sprintf, etc.  Note that if a
  cmnPrintf object is streamed out, the rest of the expression no
  longer behaves like an ordinary "ostream <<", but the cmnPrintf
  object takes precedence.

  In behavior, the format string is parsed and whenever a '%' sequence
  is encountered, a single object is fetched from the corresponding
  '<<'.  That object is formatted and printed.  If there are more
  inputs in the '<<' sequence than `%' sequences in the format string,
  then only the objects that correspond to '%' sequences are printed,
  and the rest are ignored.  For example,

  \code
  std::cout << cmnPrintf("One (%d), two and a half (%f), text (%s)")
            << 1 << 2.5 << "text-input" << 10;
  \endcode

  will print:
  
  \verbatim
  One (1), two and a half (2.5), text (text-input)
  \endverbatim
  
  and ignore the input 10.

  If there are fewer inputs in the '<<' sequence, then the formatting
  occurs until the '%' sequence that follows the last input object,
  and stops there.  For example,

  \code
  std::cout << cmnPrintf("One (%d), two and a half (%f), text (%s)")
            << 1 << 2.5;
  \endcode

  will print:

  \verbatim
  One (1), two and a half (2.5), text(
  \endverbatim
  
  Default formatting is provided for the types double ("%f"), int
  ("%d") and const char * ("%s").  Other basic types are automatically
  converted by the compiler to one of these and printed.  However, the
  model can be extended to include more output types.  See the
  documentation of cmnPrintfParser for more information.

  Note that special ios manipulators and functions, such as endl,
  flush, setw, setprecision, etc., have no '%' sequence, and therefore
  CANNOT be used with with cmnPrintf.  To print a newline (without
  flush), one may, for example, include a '\n' character in the format
  stream, as done with the C printf function.  To flush, a flush
  method must be called outside of the cmnPrintf expression.

  cmnPrintf is stateless, which means that it does not affect any of
  the state variables in the output stream, such as width, precision,
  etc.  All the output is done through conversion to character
  strings.

  The allocated space for each output element is defined in \c
  #cmnPrintfParser::BUFFER_SIZE.  If the length of the formatted
  element is larger than this number, then the output is truncated,
  and the text '(truncated)' is appended following the first
  BUFFER_SIZE characters.  Unfortunately, there is no standard way to
  compute the length of a formatted string (snprintf is not portable
  to Visual Studio), and hence to way for our code to know how much
  memory to allocate for the result of the formatting.

  Implementation-wise, this is a dummy class that just stores a copy
  of the format string.  The real work is done by a cmnPrintfParser
  object which is returned from operator <<.

  The expression 
  \code
  std::cout << cmnPrintf("something")
  \endcode

  creates a cmnPrintf object, which is streamed into std::cout.  The
  result of the expression is an invisible cmnPrintfParser object,
  into which all the following elements are streamed.  The
  cmnPrintfParser object parses the format string, formats the
  streamed objects, and forwards the formatted result to std::cout.
*/
class CISST_EXPORT cmnPrintf
{
public:
    /*! Create a cmnPrintf object and provide a format string
      following the printf specification.  The cmnPrintf object stores
      a copy of the format string. */
    cmnPrintf(const char * formatStr)
        : FormatStr(strdup(formatStr))
    {}

    /*! Destructor.  Delete any internally allocated memory */
    ~cmnPrintf()
    {
        delete[] FormatStr;
        FormatStr = 0;
    }

    /*! Copy constructor. We would like to have this constructor
      private but gcc-4.0.2 complains if it is private even thought is
      doesn't use it. */
    cmnPrintf(const cmnPrintf & other):
        FormatStr(strdup(other.FormatStr))
    {}

private:
    /*! A copy of the format string */
    char * FormatStr;

    friend class cmnPrintfParser;

    /*! Return a pointer to the internal copy of the format string, so
      that the parser can process it */
    char * GetFormat() const
    {
        return FormatStr;
    }
};


/*!
  \brief Parser for cmnPrintf

  \ingroup cisstCommon

  This class performs the actual formatting of the input.  It
  communicates with a cmnPrintf object, parses the format string, and
  sends formatted inputs to the output.  Normally, the user does not
  deal with this class.

  The user may use a cmnPrintfParser object to provide formatting for
  new types, such as user-defined classes.  Note that the collection
  of `%' sequences is specified through the C sprintf set of control
  characters, and cannot be changed.  However, the user may override
  an existing `%' sequence to format new types with that control.  For
  example, the sequence "%d%" can be used to format a single integer
  or a vector of integers.

  To format new classes, the user should provide an overload of the
  global function cmnTypePrintf, that takes the user's class as an
  input.  For example,

  \code
  bool cmnTypePrintf(cmnPrintfParser & outputParser, const MyClass data);
  \endcode

  The function should return true if the formatting is successful, and
  false otherwise, although in the current implementation we ignore
  the return value.
*/
class CISST_EXPORT cmnPrintfParser
{

public:
    /*! Initialize a cmnPrintf object with the destination output stream
    and a format string.
    \param output the actual output stream to which text will be printed
    \param outputFormat a cmnPrintf object that holds a copy of the format
    string
    */
    cmnPrintfParser(std::ostream & output, const cmnPrintf & outputFormat);

    /*! Dump raw text to the output stream.  The user may call this
      function to output any text to the stream, without internal
      formatting.
    */
    void RawOutput(const char * text)
    {
        OutputStream << text;
    }

    /*! This is the overloaded operator << that formates the currenct
      input element and advances to the next one.  It calls
      cmnTypePrintf for the given input type. */
    template<class _argType>
    inline cmnPrintfParser &
    operator << (const _argType & dataOut)
    {
        if (NextFormatTextPosition == 0)
            return *this;
        cmnTypePrintf(*this, dataOut);
        AdvanceToNextFormat();
        return *this;
    }

    /*! Overloaded operator for integers.  This operator is mostly a
      helper for compilers which won't convert an anonymous enum as an
      integer to infer the type used in the templated operator.

      This solution works with gcc 4.0.2 but not with gcc 4.0.1.  If
      you get a compilation error like:
      \code
      error: 'myClass::<anonymous enum>' is/uses anonymous type
      \endcode
      You should cast your enum value to int.
    */
    inline cmnPrintfParser &
    operator << (const int dataOut)
    {
        if (NextFormatTextPosition == 0)
            return *this;
        cmnTypePrintf(*this, dataOut);
        AdvanceToNextFormat();
        return *this;
    }

    /*! Overloaded operator for std::string.  This operators is
      required since there is no implicit cast from std::string to
      const char pointer. */
    inline cmnPrintfParser &
    operator << (const std::string & dataOut)
    {
        if (NextFormatTextPosition == 0)
            return *this;
        cmnTypePrintf(*this, dataOut.c_str());
        AdvanceToNextFormat();
        return *this;
    }
    
    /*! Overload operator void * to allow similar evaluations of
      "success" as are available for ostream.  E.g.,
      \code
      if (!std::cout) {
      // do something
      }
      \endcode

      A similar test can be performed for a cmnPrintfParser object,
      and tests the status of the output channel.
    */
    inline operator void *()
    {
        return (void *)(OutputStream);
    }

    /*! The size of buffer allocated for formatting each output
      element.  Exceeding this size will truncate the output for that
      element. */
    enum {BUFFER_SIZE = 256};


private:
    /*! Dump all the text in the format string to the output stream,
      until a percent sign is encountered, and stop there */
    void DumpUntilPercent();

    /*! Process the character following a percent sign in the format
      string */
    void ProcessPercent();

    /*! The actual output stream */
    std::ostream & OutputStream;

    /*! An object storing a copy of the format string */
    const cmnPrintf & OutputFormatter;

    /*! Processing the format string always resumes at this point */
    char * NextFormatTextPosition;

    /*! Temporary storage for the character in NextFormatTextPosition,
     *when it it substituted by 0 in the format string during
     *processing */
    char NextFormatTextCharacter;

    /*! The format sequence that will be used when sprintf is
      invoked */
    char * FormatSequence;

    /*! A type identifying character (as in printf) which is used to
      determine which processing should be performed */
    char NextTypeIdCharacter;

    /*! Advance the formatting to the next format sequence in the
      string (begins with %).  This means dumping out all the text
      between the current sequence and the next one, then stopping
      there and waiting for input.
    */
    void AdvanceToNextFormat()
    {
        FormatSequence = 0;
        DumpUntilPercent();
        ProcessPercent();
    }

public:

    /*! The complete collection of type identifying characters which
      can be used in printf */
    static const char * TypeIdCharset;

    /*! The collection of characters that identify integer output */
    static const char * IntegerTypeIds;

    /*! The collection of characters that identify floating-point output */
    static const char * FloatTypeIds;

    /*! The collection of characters that identify strings, e.g., 's' */
    static const char * StringTypeIds;

    /*!  This auxiliary function returns the set of characters that
      identify a double variable in the '%' sequence, such as 'e', 'f',
      'g', etc.
    */
    static inline const char * cmnPrintfParser::TypeIdString(double value)
    {
        return FloatTypeIds;
    }

    /*!  This auxiliary function returns the set of characters that
      identify a float variable in the '%' sequence, such as 'e', 'f',
      'g', etc.
    */
    static inline const char * cmnPrintfParser::TypeIdString(float value)
    {
        return FloatTypeIds;
    }

    /*!  This auxiliary function returns the set of characters that
      identify an int variable in the '%' sequence, such as 'd', 'x',
      'o', etc.
    */
    static inline const char * cmnPrintfParser::TypeIdString(int value)
    {
        return IntegerTypeIds;
    }

    /*! This function matches the "type" of the given data with the
      set of format characters for that type, by calling TypeIdString
      for the type of the data.  The function returns true of the next
      format character in the format string matches the type of the
      data, and false otherwise.  For example, if the format text is
      "%f" and the data is of type double, or float, the function
      returns true; but if the data is of type in the function returns
      false. */
    template<typename _outputType>
        bool MatchOutputWithFormatChar(const _outputType & data)
    {
        const char * formatCharsForData = TypeIdString(data);
        const bool result = this->NextTypeIdCharIsOneOf(formatCharsForData);
        return result;
    }

    /*! This function matches the next type character with the string
      of id characters given as argument.  For example, if the
      argument is "dxci" and the next type id in the format string was
      taken from "%10d" the function returns true; but if the next
      type id is taken from "%7.2f" the function returns false.
    */
    bool NextTypeIdCharIsOneOf(const char * typeIdCharset) const;

    void SuspendOutput()
    {
        NextFormatTextPosition = 0;
    }

    /*! Returns the next format sequence, i.e., '%' sequence, which is
      to be processed next.
    */
    const char * GetNextFormatSequence() const
    {
        return FormatSequence;
    }

    /*! Returns the type identifying character to be processed next */
    char GetNextTypeIdCharacter() const
    {
        return NextTypeIdCharacter;
    }
};


/*! This is the overloaded global operator << that gets called from
  the expression:
  \code
  std::cout << cmnPrintf("Format")
  \endcode
  The operator returns a cmnPrintfParser object, which is invisible to the
  user.
*/
inline cmnPrintfParser
operator << (std::ostream & outputStream, const cmnPrintf & formatStr)
{
    return cmnPrintfParser(outputStream, formatStr);
}


#endif  // _cmnPrintf_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnPrintf.h,v $
//  Revision 1.6  2006/06/30 13:38:47  ofri
//  cmnPrintf : Added explicit specialization for unsigned int.
//
//  Revision 1.5  2006/03/13 21:50:09  anton
//  cmnPrintf: Added documentation re. anonymous enum and updated example
//  to compile with gcc-4.0.1
//
//  Revision 1.4  2006/02/16 23:37:25  ofri
//  cmnPrintf: If a copy constructor is defined, it should be like this!
//
//  Revision 1.3  2006/02/16 21:59:24  anton
//  cmnPrintf.h: Copy ctor is now public to satisfy gcc-4.0.2.  Also, overloaded
//  operator << to explicitely support integers (otherwise gcc-4.0 can't infer
//  from an anonymous enum) and std::string (since there is no implicit cast
//  to const char *).
//
//  Revision 1.2  2006/02/16 16:35:10  anton
//  cmnPrintf: Corrected Doxygen comments and added CISST_EXPORT for DLLs.
//
//  Revision 1.1  2006/02/16 00:21:00  ofri
//  Added formatted output files cmnPrintf and vctPrintf in the respective
//  directories (code, include)
//
//  Revision 1.4  2005/04/22 17:26:21  ofri
//  mscOutputFormatter: changes related to formatting vectors.  1) Taking argument
//  of operator << as a reference instead of value (to prevent vector copy).
//  2) Explicit fetch of vector element as a local variable during formatting.
//
//  Revision 1.3  2005/04/05 19:16:22  ofri
//  mscOutputFormatter: Moved specialized definitions of
//  mscFormatterStream::TypeIdString to the header file following compilation
//  problems in .net2003
//
//  Revision 1.2  2005/03/04 16:10:24  ofri
//  no message
//
//  Revision 1.1  2005/02/24 16:21:27  ofri
//  Added classes mscDtopwatch and mscOutputFormatter
//
//
// ****************************************************************************
