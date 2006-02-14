#include <iostream>

#include <cli/CLI.h>

int main(int argc, const char* argv[]) {
    int rc(0);
    try {
        CLI::Option option("w", "write-protect region");

        CLI::Options options;

        options.addOption("c", "calls-per-second", true, "Generate the specified number of calls per second");
        options.addOption("h", "help", false, "Print help for the call generator");
        options.addOption("i", "initial-endpoints", true, "Specify the initial number of endpoint to create");
        options.addOption("s", "statistics", false, "Print statistics at the end of the test");
        options.addOption("v", "version", false, "Print the version number of the call generator");

        CLI::BasicParser parser;
        CLI::CommandLine cl( parser.parse(options, argc, argv) );

        if ( cl.hasOption('h') ) {
            CLI::HelpFormatter formatter;
            formatter.printHelp("demo", options);
        } else if( cl.hasOption('v') ) {
            std::cout << "demo version 1.0" << std::endl;
        } else {
            std::cout << cl.getOptionValue("c") << std::endl;
            std::cout << cl.getOptionValue("i") << std::endl;
        }
    }
    catch (CLI::ParseException& e) {
        std::cerr << "error: " << e.what() << std::endl;
        ++rc;
    }
    exit( rc );
}

