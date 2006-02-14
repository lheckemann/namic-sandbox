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
#include <cli/BasicParser.h>

namespace CLI {

#if 0
class BasicParser::Implementation {

    public:
        CommandLine parse(Options& options, const int argc, const char* argv[]);
};

BasicParser::BasicParser() : impl_( new Implementation ) {}

BasicParser::BasicParser(const BasicParser& other) : impl_( new Implementation ( *other.impl_ ) ) {}

BasicParser& BasicParser::operator=(const BasicParser& other) {
    if(&other != this) {
        Implementation* pointer( new Implementation( *(other.impl_) ) );
        std::swap(impl_, pointer);
        delete pointer;
    }
    return *this;
}

CommandLine BasicParser::parse(Options& options, const int argc, const char* argv[]) {
    return impl_->parse(options, argc, argv);
}

BasicParser::~BasicParser() {
    delete impl_;
    impl_ = 0;
}

CommandLine BasicParser::Implementation::parse(Options& options, const int argc, const char* argv[]) {
    CommandLine commandLine;
    for(int index = 1; index < argc; index++) {
        const char* candidateOption = argv[index]; 
        if(*candidateOption == '-') {
            ++candidateOption;
            if(isalpha(*candidateOption)) {
                for(Options::Vector::const_iterator iterator = options().begin();
                    iterator != options().end(); iterator++) {
                    if(*candidateOption == (*iterator).getOpt()[0]) {
                        commandLine.addOption( *iterator, index, argc, argv );
                        break;
                    }
                }
            } else if (*candidateOption == '-') {
                ++candidateOption;
                for(Options::Vector::const_iterator iterator = options().begin();
                    iterator != options().end(); iterator++) {
                    if(candidateOption == (*iterator).getLongOpt()) {
                        commandLine.addOption( *iterator, index, argc, argv );
                        break;
                    } 
                }
            } else {
                commandLine.addArg(argv[index]);
            }
        } else {
            commandLine.addArg(argv[index]);
        }
    }
    return commandLine;
}
#endif

Tokens BasicParser::flatten(const Options& options, const int argc, const char* argv[]) {
    Tokens tokens;
    for(int index = 1; index < argc; index++) {
        tokens.push_back( argv[index] );
    }
    return tokens;
}

} 

