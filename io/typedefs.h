#pragma once

#include "result.h"

#include <natus/memory/global.h>
#include <natus/memory/res.hpp>
#include <natus/memory/macros.h>

#include <natus/std/string.hpp>
#include <natus/std/filesystem.hpp>
#include <natus/std/string/utf8.hpp>

#include <natus/core/types.hpp>
#include <natus/core/macros/typedef.h>
#include <natus/core/assert.h>

#include <functional>

namespace natus
{
    namespace io
    {
        using namespace natus::core::types ;

        using memory = natus::memory::global ;
        natus_typedefs( natus::std::filesystem::path, path ) ;

        typedef ::std::function< void_t( char_cptr_t, size_t const, natus::io::result const ) > load_completion_funk_t ;
        typedef ::std::function< void_t( natus::io::result const ) > store_completion_funk_t ;

        typedef natus::core::void_ptr_t internal_item_ptr_t ;
    }
}