//
// Created by thecharlesblake on 11/13/17.
//

#ifndef SOLVITAIRE_UTIL_H
#define SOLVITAIRE_UTIL_H

#include <string>

#include <boost/optional.hpp>

class util {
public:
    static const boost::optional<std::string> read_file(const std::string&);
};


#endif //SOLVITAIRE_UTIL_H
