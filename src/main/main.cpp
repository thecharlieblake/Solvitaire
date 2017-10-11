#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>

namespace logging = boost::log;

int main()
{

#ifndef NDEBUG
    BOOST_LOG_TRIVIAL(debug) << "Log debug";
#endif
    BOOST_LOG_TRIVIAL(info) << "Log info";
    BOOST_LOG_TRIVIAL(warning) << "Log warning";

    std::string hello( "Hello, world!\n" );

    BOOST_FOREACH( char ch, hello )
    {
        std::cout << ch;
    }

    return 0;
}