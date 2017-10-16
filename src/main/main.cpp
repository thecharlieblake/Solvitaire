#include <boost/program_options.hpp>
#include "command_line_helper.h"
#include <iostream>

using namespace std;

namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
    // Parses the command-line options
    command_line_helper clh;
    bool success = clh.parse(argc, argv);
    if (!success) {
        return 1;
    }

    cout << "Random deal: " << clh.get_random_deal() << "\n";
    cout << "Solitaire type: " << clh.get_solitaire_type() << "\n";
    cout << "Filenames: ";
    for (auto filename : clh.get_input_files()) {
        cout << filename << ", ";
    }
    cout << "\n";

    return 0;
}

