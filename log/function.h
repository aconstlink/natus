#pragma once

#include <natus/std/string.hpp>

#define natus_log_fn( text ) natus::std::string_t( "[" + natus::std::string_t( __FUNCTION__ ) + " : ]" + natus::std::string_t(text) )