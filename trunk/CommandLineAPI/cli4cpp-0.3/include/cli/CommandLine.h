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
#include "Types.h"  // FIXME

namespace CLI {

/**
 * A class to represent the parsed result of a command line,
 * included detected options and remaining arguments (non-options).
 *
 * @author John Poplett
 * @see BasicParser
 */
class CommandLine {
    public:
        CommandLine();
        /// Copy-constructor with deep-copy semantics
        CommandLine(const CommandLine& other);
        ~CommandLine();

        /// Assignment operator with deep-copy semantics
        CommandLine& operator=(const CommandLine& other);

        /**
         * Return the standard arguments of the parsed command
         * line.
         */
        std::vector<std::string> getArgs() const;

        /**
         * Report if a given option was detected during
         * parsing.
         *
         * @param option single-character value of the short option.
         */
        bool hasOption(const char option) const;

        /**
         * Report if a given option was detected during
         * parsing.
         *
         * @param option specifies either the short or long option format 
         * that we're searching for
         */
        std::string getOptionValue(const char* const option) const;
        std::string getOptionValue(const char* const option, const char* const defaultValue) const;

        /**
         * Return a vector of all detected options
         */
        const Options::Vector& operator()() const;

        void addOption(const Option& sourceOption, int& index, const int argc, const char* argv[]);
        void addArg(const char* const arg);


        void addOption(const Option& sourceOption, Tokens::const_iterator& token, const Tokens::const_iterator& end);
        void addArg(const std::string& arg);

    private:
        class Implementation;
        Implementation* impl_;
};

} 

#endif // #ifndef _CommandLine_h_included
