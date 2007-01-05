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
#include <cli/Parser.h>

namespace CLI {

Parser::~Parser() {
}

CommandLine Parser::parse(Options& options, const int argc, const char* argv[]) {
    CommandLine commandLine;
    Tokens tokens = flatten(options, argc, argv);
    for(Tokens::const_iterator token = tokens.begin(); token != tokens.end(); token++) {
        std::string::const_iterator candidateOption = token->begin();
        if(candidateOption != token->end() && *candidateOption == '-') {
            ++candidateOption;
            if(candidateOption != token->end() && isalpha(*candidateOption)) {
                for(Options::Vector::const_iterator iterator = options().begin();
                    iterator != options().end(); iterator++) {
                    if(*candidateOption == (*iterator).getOpt()[0]) {
                        commandLine.addOption( *iterator, token, tokens.end() );
                        break;
                    }
                }
            } else if (candidateOption != token->end() && *candidateOption == '-') {
                ++candidateOption;
                std::string candidateLongOption( candidateOption, token->end() );
                for(Options::Vector::const_iterator iterator = options().begin();
                    iterator != options().end(); iterator++) {
                    if(candidateLongOption == (*iterator).getLongOpt()) {
                        commandLine.addOption( *iterator, token, tokens.end() );
                        break;
                    } 
                }
            } else {
                commandLine.addArg( *token ); 
            }
        } else {
            commandLine.addArg( *token );
        }
    }
    return commandLine;
}

} 

