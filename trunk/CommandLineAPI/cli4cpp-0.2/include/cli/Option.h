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
#ifndef _Option_h_included
#define  _Option_h_included

#include <string>
//#include <memory>
#include <vector>

namespace CLI {

/**
 * Describes a single command-line option.
 *
 * @author John Poplett
 * @see Options, CommandLine
 */
class Option {
    public:
        Option();

        Option(const Option&);
        Option& operator=(const Option&);

        Option(const char* const option, const bool hasArg, const char* const description);

        Option(const char* const option, const char* const description);

        Option(const char* const option, const char* const longOption, const bool hasArg, const char* const description);

        ~Option();

        /**
         * Provide the display name used for the argument to this option.
         * This value is used in formatting display help.
         */
        std::string getArgName() const;

        std::string getOpt() const;
        std::string getLongOpt() const;
        bool hasLongOpt() const;

        std::string getDescription() const;

        /**
         * Returns an identifier for this option
         */
        int getId() const;

        std::string getValue() const;
        std::string getValue(const char* const defaultValue) const;
        std::string getValue(const unsigned index) const;

        std::vector<std::string> getValues() const;

        /**
         * Report whether the arg name value for this option was set.
         */
        bool hasArgName() const;

        bool hasArg() const;
        bool hasArgs() const;

        void setArgName(const std::string& argName);

        void addValue(const std::string& value);

    private:
        class Implementation;
        // std::auto_ptr<Implementation> impl_;
        Implementation* impl_;
};


} 

#endif // #ifndef _Option_h_included
