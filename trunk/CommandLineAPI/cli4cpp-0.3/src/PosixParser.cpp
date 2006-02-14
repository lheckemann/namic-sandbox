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

#include <cctype>
#include <algorithm>
#include <cli/PosixParser.h>

namespace CLI {

Tokens PosixParser::flatten(const Options& options, const int argc, const char* argv[]) {
    Tokens tokens;
    for(int index = 1; index < argc; index++) {
        const char* arg = argv[index];
        if(*arg== '-' && isalpha(*(arg + 1))) {
            ++arg;
            while(isalpha(*arg)) {
                char newArg [3];
                newArg[0] = '-';
                newArg[1] = *arg++;
                newArg[2] = '\0';
                tokens.push_back( newArg );
            }
        } else {
            tokens.push_back( argv[index] );
        }
    }
    return tokens;
}

} 

