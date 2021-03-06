
#pragma once

#define natus_typedefs_extend( from_, to_ )                                \
    typedef to_ ## _t * to_ ## _ptr_t ;                                 \
    typedef to_ ## _t * to_ ## _vtr_t ;                                 \
    typedef to_ ## _t & to_ ## _ref_t ;                                 \
    typedef to_ ## _t && to_ ## _rref_t ;                               \
    typedef to_ ## _t const * to_ ## _cptr_t ;                          \
    typedef to_ ## _t * const to_ ## _ptrc_t ;                          \
    typedef to_ ## _t const * const to_ ## _cptrc_t ;                   \
    typedef to_ ## _t const * to_ ## _cvtr_t ;                          \
    typedef to_ ## _t const & to_ ## _cref_t ;                          \
    typedef to_ ## _cref_t to_ ## _in_t ;                               \
    typedef to_ ## _ref_t to_ ## _out_t ;                               \
    typedef to_ ## _ref_t to_ ## _inout_t ;                             \
    typedef to_ ## _t * & to_ ## _ptr_ref_t


#define natus_typedef_extend( what_ ) natus_typedefs_extend( what_, what_ )

#define natus_typedefs( from_, to_ )                                       \
    typedef from_ to_ ## _t ;                                           \
    natus_typedefs_extend( from_, to_ )

#define natus_typedef( what_ ) natus_typedefs( what_, what_ )

#define natus_this_typedefs(x)                                             \
    typedef x this_t ;                                                  \
    typedef this_t & this_ref_t ;                                       \
    typedef this_t const & this_cref_t ;                                \
    typedef this_t * this_ptr_t ;                                       \
    typedef this_t const * this_cptr_t ;                                \
    typedef this_t && this_rref_t ;                                     \
    typedef this_t & this_inout_t

#define natus_class_proto_typedefs(x)                                      \
    class x ;                                                           \
    typedef x * x ## _ptr_t ;                                           \
    typedef x * x ## _vtr_t ;                                           \
    typedef x const * x ## _cptr_t ;                                    \
    typedef x * const x ## _ptrc_t ;                                    \
    typedef x const * const x ## _cptrc_t ;                             \
    typedef x && x ## _rref_t ;                                         \
    typedef x & x ## _inout_t ;                                         \
    typedef x & x ## _ref_t

#define natus_struct_proto_typedefs(x)                                     \
    struct x ;                                                          \
    typedef x * x ## _ptr_t ;                                           \
    typedef x * x ## _vtr_t ;                                           \
    typedef x const * x ## _cptr_t ;                                    \
    typedef x * const x ## _ptrc_t ;                                    \
    typedef x const * const x ## _cptrc_t ;                             \
    typedef x const * x ## _cvtr_t                                         
