//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_LOG_HELPER_H
#define SOLVITAIRE_LOG_HELPER_H

#include <iostream>

#if defined NDEBUG
    #define LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg) clog << "[debug]\n" << msg << "\n"
#endif
#define LOG_INFO(msg)  clog << "[info] " << msg << "\n"
#define LOG_WARNING(msg)  clog << "[warning] " << msg << "\n"
#define LOG_ERROR(msg)  cerr << "[error] " << msg << "\n"

#endif //SOLVITAIRE_LOG_HELPER_H
