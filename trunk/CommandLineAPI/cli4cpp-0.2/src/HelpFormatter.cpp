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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cli/HelpFormatter.h>

namespace CLI {

class HelpFormatter::Implementation {
    public:
        void printHelp(const char* const banner, const Options& options);
};

HelpFormatter::HelpFormatter() : impl_( new Implementation ) {}

HelpFormatter::HelpFormatter(const HelpFormatter& other) : impl_( new Implementation( *other.impl_ ) ) {}

HelpFormatter& HelpFormatter::operator=(const HelpFormatter& other) {
    if(&other != this) {
        Implementation* pointer( new Implementation( *(other.impl_) ) );
        std::swap(impl_, pointer);
        delete pointer;
    }
    return *this;
}

void HelpFormatter::printHelp(const char* const banner, const Options& options) {
    impl_->printHelp(banner, options);
}

HelpFormatter::~HelpFormatter() {
    delete impl_;
    impl_ = 0;
}

void HelpFormatter::Implementation::printHelp(const char* const banner, const Options& options) {
    std::cout << "usage: " << banner << std::endl;
    std::vector<std::string> invocationStrings;
    for(Options::Vector::const_iterator iterator = options().begin();
       iterator != options().end(); iterator++) {
        std::ostringstream invocation;
        if((*iterator).hasLongOpt()) {
            invocation << std::string("--") << (*iterator).getLongOpt();
        } else {
            invocation << std::string("-") << (*iterator).getOpt()[0];
        }
        if((*iterator).hasArg()) {
            invocation << " <" << (*iterator).getArgName() << ">";
        }
        invocationStrings.push_back( invocation.str() );
    }
    unsigned int width(20);
    for(std::vector<std::string>::const_iterator iterator =  invocationStrings.begin();
        iterator != invocationStrings.end(); iterator++) {
        if(iterator->length() > width) {
            width = iterator->length();
        }
    }
    std::vector<std::string>::const_iterator invocationIterator =  invocationStrings.begin();
    for(Options::Vector::const_iterator optionIterator = options().begin();
        optionIterator != options().end() && invocationIterator != invocationStrings.end();
            optionIterator++, invocationIterator++) {
        std::cout << std::setw(static_cast<int>(width)) << std::left << *invocationIterator << "  " 
            << (*optionIterator).getDescription() << std::endl;
    }
}

} 

