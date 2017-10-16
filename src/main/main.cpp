#include <boost/program_options.hpp>
#include <iostream>
#include "command_line_helper.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
    // Creates the command line options
    po::options_description desc = get_options_desc();

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("compression")) {
        cout << "Compression level was set to "
             << vm["compression"].as<int>() << ".\n";
    } else {
        cout << "Compression level was not set.\n";
    }
}

