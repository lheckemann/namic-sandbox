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
#ifndef _help_formatter_h_included
#define  _help_formatter_h_included

#include "Options.h"

namespace CLI {

/**
 * Helper class to format and print help for the given
 * set of options.
 *
 * @author John Poplett
 */
class HelpFormatter {
    public:
        HelpFormatter();
        HelpFormatter(const HelpFormatter&);
        HelpFormatter& operator=(const HelpFormatter&);
        ~HelpFormatter();
        void printHelp(const char* const banner, const Options& opt);
    private:
        class Implementation;
        Implementation* impl_;
};

} 

#endif // #ifndef _help_formatter_h_included
