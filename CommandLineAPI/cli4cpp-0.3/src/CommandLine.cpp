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
#include <cli/ParseException.h>
#include <cli/CommandLine.h>

namespace CLI {

class CommandLine::Implementation {
    private:
        Options::Vector options_;
        std::vector<std::string> remainingArgs_;

    public:
        std::vector<std::string> getArgs() const {
            return remainingArgs_;
        }
        std::string getOptionValue(const char* const option) const;
        std::string getOptionValue(const char* const option, const char* const defaultValue) const;
        bool hasOption(const char option) const;
        const Options::Vector& operator()() const {
            return options_;
        }
        void addOption(const Option& option) {
            options_.push_back(option);
        }
        void addOption(const Option& sourceOption, int& index, const int argc, const char* argv[]);
        void addArg(const char* const arg) {
            remainingArgs_.push_back(std::string(arg));
        }
        void addOption(const Option& sourceOption, Tokens::const_iterator& token, const Tokens::const_iterator& end);
        void addArg(const std::string& arg) {
            remainingArgs_.push_back(arg);
        }
};

bool CommandLine::Implementation::hasOption(const char option) const {
    bool rc = false;
    for(Options::Vector::const_iterator iterator = options_.begin();
        iterator != options_.end(); iterator++) {
        if((*iterator).getOpt()[0] == option) {
            rc = true;
            break;
        }
    }
    return rc;
}

  std::string CommandLine::Implementation::getOptionValue(const char* const option, const char* const defaultValue) const {
    if ( hasOption ( option[0] ) )
      {
      return getOptionValue ( option );
      }
    else
      {
      return std::string ( defaultValue );
      }
  }
std::string CommandLine::Implementation::getOptionValue(const char* const option) const {
    std::string rc;
    for(Options::Vector::const_iterator iterator = options_.begin();
        iterator != options_.end(); iterator++) {
        if((*iterator).getOpt()[0] == *option) {
            rc = (*iterator).getValue();
        }
    }
    return rc;
}

void CommandLine::Implementation::addOption(const Option& sourceOption, int& index, const int argc, const char* argv[]) {
    Option option( sourceOption );
    if(option.hasArg()) {
        if(index + 1 >= argc) {
            throw ParseException("missing option argument");
        }
        std::string value(argv[++index]);
        option.addValue(value);
    }
    addOption( option );
}

void CommandLine::Implementation::addOption(const Option& sourceOption, Tokens::const_iterator& token, const Tokens::const_iterator& end) {
    Option option( sourceOption );
    if(option.hasArg()) {
        if(token++ == end || token == end) {
            throw ParseException("missing option argument");
        }
        option.addValue(*token);
    }
    addOption( option );
}


CommandLine::CommandLine() : impl_( new Implementation ) {}

CommandLine::CommandLine(const CommandLine& other) : impl_( new Implementation( *other.impl_ ) ) {}

CommandLine& CommandLine::operator=(const CommandLine& other) {
    if(&other != this) {
        Implementation* pointer( new Implementation( *(other.impl_) ) );
        std::swap(impl_, pointer);
        delete pointer;
    }
    return *this;
}

CommandLine::~CommandLine() {
    delete impl_;
    impl_ = 0;
}

std::vector<std::string> CommandLine::getArgs() const {
    return impl_->getArgs();
}

bool CommandLine::hasOption(const char option) const {
    return impl_->hasOption(option);
}

std::string CommandLine::getOptionValue(const char* const option) const {
    return impl_->getOptionValue(option);
}

  std::string CommandLine::getOptionValue(const char* const option, const char* const defaultValue ) const {
    return impl_->getOptionValue(option, defaultValue);
}

const Options::Vector& CommandLine::operator()() const {
    return (*impl_)();
}

void CommandLine::addOption(const Option& sourceOption, int& index, const int argc, const char* argv[]) { 
     return impl_->addOption(sourceOption, index, argc, argv);
}

void CommandLine::addArg(const char* const arg) {
     impl_->addArg(arg);
}

void CommandLine::addOption(const Option& sourceOption, Tokens::const_iterator& token, const Tokens::const_iterator& end) {
     return impl_->addOption(sourceOption, token, end);
}

void CommandLine:: addArg(const std::string& arg) {
     impl_->addArg(arg);
}

}
