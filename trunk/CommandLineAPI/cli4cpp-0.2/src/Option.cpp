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
#include <cli/Option.h>

namespace CLI {

class Option::Implementation {
    private:
        std::string option_;
        std::string longOption_;
        std::string argName_;
        bool hasArgName_;
        unsigned takesArgs_;
        std::string description_;
        std::vector<std::string> values_;

    public:
        Implementation() : argName_("arg"), hasArgName_(false), takesArgs_(0u), values_(0) {}

        Implementation(const char* const option, const bool hasArg, const char* const description) :
            option_(option), longOption_(""), argName_("arg"), hasArgName_(false), takesArgs_(hasArg ? 1 : 0), description_(description) {
        }

        Implementation(const char* const option, const char* const longOption, const bool hasArg, const char* const description) :
            option_(option), longOption_(longOption), argName_("arg"), hasArgName_(false), takesArgs_(hasArg ? 1 : 0), description_(description) {
        }

        std::string getDescription() const {
            return description_;
        }

        // FIXME - short option not necessarily set.
        int getId() const {
            return option_[0];
        }

        std::string getArgName() const {
            return argName_;
        }

        std::string getOpt() const {
            return option_;
        }

        std::string getLongOpt() const {
            return longOption_;
        }

        bool hasLongOpt() const {
            return longOption_.length() > 0;
        }

        std::string getValue() const {
            return getValue("");
        }

        std::string getValue(const char* const defaultValue) const {
            return (values_.size() == 0 ) ? std::string(defaultValue) : values_[0];
        }

        std::string getValue(const unsigned index) const {
            return (index < values_.size()) ? values_[index] : std::string("");
        }

        std::vector<std::string> getValues() const {
            return values_;
        }

        bool hasArgName() const {
            return hasArgName_;
        }

        bool hasArg() const {
            return takesArgs_ > 0;
        }

        bool hasArgs() const {
            return takesArgs_ > 1;
        }

        void setArgName(const std::string& argName) {
            argName_ = argName;
            hasArgName_ = true;
        }

        void addValue(const std::string& value) {
            values_.push_back(value);
        }
};

Option::Option() : impl_( new Implementation ) {}

Option::Option(const Option& other) : impl_( new Implementation( *(other.impl_) ) ) {}

Option& Option::operator=(const Option& other) {
    if(&other != this) {
        Implementation* pointer( new Implementation( *(other.impl_) ) );
        std::swap(impl_, pointer);
        delete pointer;
    }
    return *this;
}

Option::Option(const char* const option, const bool hasArg, const char* const description) : impl_( new Implementation( option, hasArg, description ) ) {
    // do nothing
}

Option::Option(const char* const option, const char* const description) : impl_( new Implementation( option, false, description ) ) {
    // do nothing
}

Option::Option(const char* const option, const char* const longOption, const bool hasArg, const char* const description) : impl_( new Implementation(option, longOption, hasArg, description) ) {
    // do nothing
}

Option::~Option() {
    delete impl_;
    impl_ = 0;
}

std::string Option::getArgName() const {
    return impl_->getArgName();
}

std::string Option::getOpt() const {
    return impl_->getOpt();
}

std::string Option::getLongOpt() const  {
    return impl_->getLongOpt();
}

bool Option::hasLongOpt() const  {
    return impl_->hasLongOpt();
}

std::string Option::getDescription() const  {
    return impl_->getDescription();
}

int Option::getId() const  {
    return impl_->getId();
}

std::string Option::getValue() const  {
    return impl_->getValue();
}

std::string Option::getValue(const char* const defaultValue) const  {
    return impl_->getValue(defaultValue);
}

std::string Option::getValue(const unsigned index) const {
    return impl_->getValue(index);
}

std::vector<std::string> Option::getValues() const {
    return impl_->getValues();
}

bool Option::hasArgName() const {
    return impl_->hasArgName();
}

bool Option::hasArg() const {
    return impl_->hasArg();
}

bool Option::hasArgs() const {
    return impl_->hasArgs();
}

void Option::setArgName(const std::string& argName) {
    impl_->setArgName(argName);
}

void Option::addValue(const std::string& value) {
    return impl_->addValue(value);
}

} 

