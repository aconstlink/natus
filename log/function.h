#pragma once

#include <natus/ntd/string.hpp>

#define natus_log_fn( text ) natus::ntd::string_t( "[" + natus::ntd::string_t( __FUNCTION__ ) + "] : " + natus::ntd::string_t(text) )