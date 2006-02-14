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
#include <cli/Options.h>

namespace CLI {

class Options::Implementation {
    private:
        Options::Vector options_;

        const Option* const  findOption(const char* const optionName) const {
            for(Options::Vector::const_iterator iterator = options_.begin(); iterator != options_.end(); iterator++) {
                const std::string currentOption( (*iterator).getOpt() );
                if(*optionName == currentOption[0]) {
                    return &(*iterator);
                    break;
                } else if((*iterator).hasLongOpt()) {
                    const std::string currentLongOption( (*iterator).getLongOpt() );
                    if(optionName == currentLongOption) {
                        return &(*iterator);
                        break;
                    }
                }
            }
            return 0;
        }

    public:
        void addOption(const Option& opt) {
            options_.push_back(opt);
        }

        void addOption(const char* const option, const bool hasArg, const char* const description) {
            Option opt( option, hasArg, description );
            options_.push_back(opt);
        }

        void addOption(const char* const option, const char* const longOption, const bool hasArg, const char* const description) {
            Option opt( option, longOption, hasArg, description );
            options_.push_back(opt);
        }

        bool hasOption(const char* const option) const {
            return findOption(option) != 0;
        }

        const Option& getOption(const char* const which) const {
            const Option* const option =  findOption(which);
            if(option == 0) {
                throw ParseException("no such option");
            }
            return *option;
        }

        std::string toString() const {
            return std::string("");
        }

        const Options::Vector& operator()() const {
            return options_;
        }
};

Options::Options() : impl_(new Implementation) {
}

Options::Options(const Options& other) : impl_( new Implementation( *other.impl_ ) ) {
}

Options& Options::operator=(const Options& other) {
    if(&other != this) {
        Implementation* pointer( new Implementation( *(other.impl_) ) );
        std::swap(impl_, pointer);
        delete pointer;
    }
    return *this;
}

Options::~Options() {
    delete impl_;
    impl_ = 0;
}

void Options::addOption(const Option& opt) {
    impl_->addOption(opt);
}

void Options::addOption(const char* const option, const bool hasArg, const char* const usage) {
    impl_->addOption(option, hasArg, usage);
}

void Options::addOption(const char* const option, const char* const longOption, const bool hasArg, const char* const usage) {
    impl_->addOption(option, longOption, hasArg, usage);
}

bool Options::hasOption(const char* const option) const {
    return impl_->hasOption(option);
}

const Option& Options::getOption(const char* const option) const {
    return impl_->getOption(option);
}

std::string Options::toString() const {
    return impl_->toString();
}

const Options::Vector& Options::operator()() const {
    return (*impl_)();
}

} 

