#pragma once

#define natus_mem_handle_typedef( class_name ) \
    typedef natus::memory::handle< class_name > class_name ## _handle_t ; \
    typedef class_name ## _handle_t const & class_name ## _handle_cref_t ; \
    typedef class_name ## _handle_t && class_name ## _handle_rref_t 

