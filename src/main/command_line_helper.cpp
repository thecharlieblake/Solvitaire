//
// Created by thecharlesblake on 10/16/17.
//

#include "command_line_helper.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::options_description get_options_desc() {
    // Creates the command line options
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("compression", po::value<int>(), "set compression level");

    return desc;
}