//
// Created by thecharlesblake on 11/13/17.
//

#include <fstream>
#include <sstream>

#include "util.h"

using namespace std;
using namespace boost;

const optional<string> util::read_file(const string& filename) {
    ifstream ifstr(filename);
    stringstream buf;
    buf << ifstr.rdbuf();

    if (ifstr.fail()) {
        return none;
    } else {
        return buf.str();
    }
}
