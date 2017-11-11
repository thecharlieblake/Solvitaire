//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_LOG_HELPER_H
#define SOLVITAIRE_LOG_HELPER_H

#include <iostream>

#if defined NDEBUG
    #define LOG_DEBUG
#else
    #define LOG_DEBUG(msg) clog << "[debug]\n" << msg
#endif
#define LOG_INFO(msg) clog << "[info]\n" << msg
#define LOG_WARNING(msg) clog << "[warning]\n" << msg
#define LOG_ERROR(msg) cerr << "[error]\n" << msg

#endif //SOLVITAIRE_LOG_HELPER_H
