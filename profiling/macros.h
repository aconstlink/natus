#pragma once

#include <natus/log/global.h>
#include <chrono>

// counts and prints when a second passed
#define NATUS_PROFILING_COUNTER_HERE( name ) \
{\
    typedef std::chrono::high_resolution_clock ___clk_t ;\
    static size_t ___count = 0 ;\
    ___count++ ;\
    static ___clk_t::time_point ___tp = ___clk_t::now() ;\
\
    if( std::chrono::duration_cast< std::chrono::milliseconds >( ___clk_t::now() - ___tp ).count() > 1000 )\
    {\
        ___tp = ___clk_t::now() ;\
        natus::log::global_t::status( natus::ntd::string_t( name ) + " : " + std::to_string( ___count ) ) ;\
        ___count = 0 ;\
    }\
}\
( void ) nullptr