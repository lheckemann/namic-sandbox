/*
 * Copyright (c) 2005 John H. Poplett. All rights reserved.
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *     1. The origin of this software must not be misrepresented; you must
 *     not claim that you wrote the original software. If you use this
 *     software in a product, an acknowledgment in the product documentation
 *     would be appreciated but is not required.
 * 
 *     2. Altered source versions must be plainly marked as such, and must
 *     not be misrepresented as being the original software.
 * 
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 */
#ifndef _parse_exception_h_included
#define _parse_exception_h_included
#include <stdexcept>
namespace CLI {

/**
 * Define an exception class to signal exceptions encountered
 * during command-line parsing.
 *
 * @author John Poplett
 * @version 1.0
 */
class ParseException : public std::runtime_error {
    public:
        ParseException() : std::runtime_error("parse error") {}
        ParseException(const char* const message) : std::runtime_error(message) {}
};

}

#endif // #ifndef _parse_exception_h_included
