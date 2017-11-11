//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_LOG_HELPER_H
#define SOLVITAIRE_LOG_HELPER_H

#include <iostream>

#if defined NDEBUG
    #define LOG_DEBUG
#else
    #define LOG_DEBUG clog << "[debug]\n"
#endif
#define LOG_INFO clog << "[info] "
#define LOG_WARNING clog << "[warning] "
#define LOG_ERROR cerr << "[ERROR] "

#endif //SOLVITAIRE_LOG_HELPER_H
