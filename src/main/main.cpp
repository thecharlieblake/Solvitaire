#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>

#if defined NDEBUG
    #define LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg) BOOST_LOG_TRIVIAL(debug) << msg
#endif
#define LOG_INFO(msg) BOOST_LOG_TRIVIAL(info) << msg
#define LOG_WARNING(msg) BOOST_LOG_TRIVIAL(warning) << msg
#define LOG_ERROR(msg) BOOST_LOG_ERROR(error) << msg

int main()
{

    LOG_DEBUG("Log debug");
    LOG_INFO("Log info");
    LOG_WARNING("Log warning");

    std::string hello( "Hello, world!\n" );

    BOOST_FOREACH( char ch, hello )
    {
        std::cout << ch;
    }

    return 0;
}