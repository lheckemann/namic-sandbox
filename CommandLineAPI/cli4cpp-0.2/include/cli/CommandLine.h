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
#ifndef _CommandLine_h_included
#define  _CommandLine_h_included

#include <string>
#include <vector>

#include "Options.h"

namespace CLI {

class CommandLine {
    public:
        CommandLine();
        CommandLine(const CommandLine& other);
        ~CommandLine();
        CommandLine& operator=(const CommandLine& other);
        std::vector<std::string> getArgs() const;
        bool hasOption(const char option) const;
        std::string getOptionValue(const char* const option, const char* const defaultValue) const;
        std::string getOptionValue(const char* const option) const;
        const Options::Vector& operator()() const;
        void addOption(const Option& sourceOption, int& index, const int argc, const char* argv[]);
        void addArg(const char* const arg);

    private:
        class Implementation;
        Implementation* impl_;
};

} 

#endif // #ifndef _CommandLine_h_included