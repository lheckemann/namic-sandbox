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
#ifndef _Options_h_included
#define  _Options_h_included

#include <string>
#include <vector>

#include "Option.h"

namespace CLI {

/**
 * A container for Option instances that defines the 
 * command-line options of a given program. 
 *
 * Create an instance of this container to define
 * the options for your program.
 *
 * @author John Poplett
 * @version 1.0
 *
 * @see CLI::BasicParser
 */
class Options {
    public:
        typedef std::vector<Option> Vector;

        Options();

        Options(const Options&);
        Options& operator=(const Options&);

        ~Options(); 

        /**
         * Add the specified option to the container.
         */
        void addOption(const Option& option);

        /**
         * Create and add option a new option to the container.
         * Use this method to create an option with a short
         * option switch only.
         *
         * @param option name of the short option
         * @param hasArg specify if this option takes an argument
         * @param usage description of argument
         */
        void addOption(const char* const option, const bool hasArg, const char* const usage);

        void addOption(const char* const option, const char* const longOption, const bool hasArg, const char* const usage);

        bool hasOption(const char* const which) const;
        const Option& getOption(const char* const which) const;
        std::string toString() const;
        const Vector& operator()() const;

    private:
        class Implementation;
        Implementation* impl_;
};

} 

#endif // #ifndef _Options_h_included
