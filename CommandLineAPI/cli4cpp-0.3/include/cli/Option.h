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
        /**
         * Enumeration to specify how many arguments this option can take. An option
         * can have either no arguments, exactly one argument, one or more arguments,
         * or zero or more arguments.
         */
        enum Args {
            NO_ARGS,
            ONE_ARG,
            AT_LEAST_ONE_ARG,
            MANY_ARGS
        };

        Option();

        Option(const Option&);
        Option& operator=(const Option&);

        /**
         * Construct an option from the given parameters.
         *
         * @param option short option
         * @param hasArg true if option takes a single argument
         * @param description textual description for user help
         */
        Option(const char* const option, const bool hasArg, const char* const description);

        /**
         * Construct an option from the given parameters.
         *
         * @param option short option
         * @param takesArgs specify number of allowable arguments for this option
         * @param description textual description for user help
         */
        Option(const char* const option, const Args takesArgs, const char* const description);

        /**
         * Construct an option from the given parameters. This constructor
         * assumes that the option does not take an argument.
         *
         * @param option short option
         * @param description textual description for user help
         */
        Option(const char* const option, const char* const description);

        /**
         * Construct an option from the given parameters. 
         *
         * @param option short option (e.g. "-c")
         * @param longOption long option (e.g. of the form "--something")
         * @param hasArg true if option takes a single argument
         * @param description textual description for user help
         */
        Option(const char* const option, const char* const longOption, const bool hasArg, const char* const description);
        /**
         * Construct an option from the given parameters. 
         *
         * @param option short option (e.g. "-c")
         * @param longOption long option (e.g. of the form "--something")
         * @param takesArgs specify number of allowable arguments for this option
         * @param description textual description for user help
         */
        Option(const char* const option, const char* const longOption, const Args takesArgs, const char* const description);


        ~Option();

        /**
         * Provide the display name used for the argument to this option.
         * This value is used in formatting display help.
         */
        std::string getArgName() const;

        /**
         * Return a copy of the short option.
         */
        std::string getOpt() const;
        /**
         * Return a copy of the long option.
         */
        std::string getLongOpt() const;
        /**
         * Report whether the option has a long option.
         */
        bool hasLongOpt() const;

        /**
         * Return the description for this option
         * as would be presented in a help screen.
         *
         * @return the description of this option
         * @see HelpFormatter
         */
        std::string getDescription() const;

        /**
         * Returns an identifier for this option. The
         * value returned must differ from all other
         * active Option instances.
         *
         * @return this option's identifier
         */
        int getId() const;

        /**
         * Get the value for this option. Typically, assumes hasArg() == true.
         */
        std::string getValue() const;

        /**
         * Get the value for this option. If none was specified,
         * return the value specified.
         *
         * @param defaultValue return this value if an argument is not given.
         */
        std::string getValue(const char* const defaultValue) const;


        /**
         * Get an argument value for this option at the given index location.
         *
         * @param index address of the given argument value
         */
        std::string getValue(const unsigned index) const;

        std::vector<std::string> getValues() const;

        /**
         * Report whether the arg name value for this option was set.
         */
        bool hasArgName() const;

        bool hasArg() const;
        bool hasArgs() const;

        /**
         * Set the argument name used by this option as
         * used in help screens. The default value otherwise
         * is "arg".
         *
         * @see HelpFormatter
         */
        void setArgName(const std::string& argName);

        /**
         * Specify how many arguments this option can 
         * take. 
         *
         * @param takesArgs specify the number of valid arguments for this option
         * @see Args
         */
        void setArgs(const Args takesArgs);

        /**
         * Add a value to this option. This method is
         * used during parsing or for test purposes.
         */
        void addValue(const std::string& value);

    private:
        class Implementation;
        Implementation* impl_;
};


} 

#endif // #ifndef _Option_h_included
