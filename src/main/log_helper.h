//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_LOG_HELPER_H
#define SOLVITAIRE_LOG_HELPER_H

#if defined NDEBUG
    #define LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg) BOOST_LOG_TRIVIAL(debug) << msg
#endif
#define LOG_INFO(msg) BOOST_LOG_TRIVIAL(info) << msg
#define LOG_WARNING(msg) BOOST_LOG_TRIVIAL(warning) << msg
#define LOG_ERROR(msg) BOOST_LOG_ERROR(error) << msg

#endif //SOLVITAIRE_LOG_HELPER_H
