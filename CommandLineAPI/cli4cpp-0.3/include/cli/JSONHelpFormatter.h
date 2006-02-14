/*
 * Copyright Insight Software Consortium
 */
#ifndef _json_help_formatter_h_included
#define  _json_help_formatter_h_included

#include "Options.h"

namespace CLI {

/**
 */
class JSONHelpFormatter {
    public:
        JSONHelpFormatter();
        ~JSONHelpFormatter();
        void printJSONHelp ( const char* algclass,
                             const char* name,
                             const char* description,
                             const char* executable,
                             const Options& options );
    private:
};

} 

#endif // #ifndef _json_help_formatter_h_included
