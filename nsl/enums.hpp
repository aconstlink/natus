#pragma once

namespace natus
{
    namespace nsl
    {
        //@obsolete nsl will be the only possibility
        enum class language_class
        {
            unknown,
            nsl,
            glsl, //@obsolete
            hlsl //@obsolete
        };

        static language_class to_language_class( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "nsl" ) return language_class::nsl ;
            else if( s == "glsl" ) return language_class::glsl ;
            else if( s == "hlsl" ) return language_class::hlsl ;
            return language_class::unknown ;
        }

        enum class api_type
        {
            gl3,
            es3,
            d3d11
        };

        enum class shader_type
        {
            unknown,
            vertex_shader,
            pixel_shader
        };
        static shader_type to_shader_type( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "vertex_shader" ) return shader_type::vertex_shader ;
            else if( s == "pixel_shader" ) return shader_type::pixel_shader ;
            return shader_type::unknown ;
        }

        enum class flow_qualifier
        {
            unknown,
            in,
            out,
            global
        };
        static flow_qualifier to_flow_qualifier( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "in" ) return flow_qualifier::in ;
            else if( s == "out" ) return flow_qualifier::out ;
            else if( s == "" || s == "global" ) return flow_qualifier::global ;
            return flow_qualifier::unknown ;
        }

        static natus::ntd::string_cref_t to_string( natus::nsl::flow_qualifier const fq ) noexcept
        {
            static natus::ntd::string_t const __strings[] = { 
                "unknown", "in", "out", "global" } ;
            return __strings[ size_t( fq ) ] ;
        }
    }

    namespace nsl
    {
        enum class binding
        {
            unknown, position, normal, tangent,
            texcoord0,texcoord1,texcoord2,texcoord3,texcoord4,texcoord5,texcoord6,texcoord7,
            color0, color1, color2, color3, color4, color5, color6, color7,
            projection, view, world, object, camera, camera_position, viewport
        };

        static binding to_binding( natus::ntd::string_cref_t b ) noexcept
        {
            typedef std::pair< natus::ntd::string_t, natus::nsl::binding > __mapping_t ;
            static __mapping_t const __mappings[] =
            {
                __mapping_t( "unknown", natus::nsl::binding::unknown ),
                __mapping_t( "position", natus::nsl::binding::position ),
                __mapping_t( "normal", natus::nsl::binding::normal ),
                __mapping_t( "tangent", natus::nsl::binding::tangent ),
                __mapping_t( "texcoord", natus::nsl::binding::texcoord0 ),
                __mapping_t( "texcoord0", natus::nsl::binding::texcoord0 ),
                __mapping_t( "texcoord1", natus::nsl::binding::texcoord1 ),
                __mapping_t( "texcoord2", natus::nsl::binding::texcoord2 ),
                __mapping_t( "texcoord3", natus::nsl::binding::texcoord3 ),
                __mapping_t( "texcoord4", natus::nsl::binding::texcoord4 ),
                __mapping_t( "texcoord5", natus::nsl::binding::texcoord5 ),
                __mapping_t( "texcoord6", natus::nsl::binding::texcoord6 ),
                __mapping_t( "texcoord7", natus::nsl::binding::texcoord7 ),
                __mapping_t( "color", natus::nsl::binding::color0 ),
                __mapping_t( "color0", natus::nsl::binding::color0 ),
                __mapping_t( "color1", natus::nsl::binding::color1 ),
                __mapping_t( "color2", natus::nsl::binding::color2 ),
                __mapping_t( "color3", natus::nsl::binding::color3 ),
                __mapping_t( "color4", natus::nsl::binding::color4 ),
                __mapping_t( "color5", natus::nsl::binding::color5 ),
                __mapping_t( "color6", natus::nsl::binding::color6 ),
                __mapping_t( "color7", natus::nsl::binding::color7 ),
                __mapping_t( "projection", natus::nsl::binding::projection ),
                __mapping_t( "view", natus::nsl::binding::view ),
                __mapping_t( "world", natus::nsl::binding::world ),
                __mapping_t( "object", natus::nsl::binding::object ),
                __mapping_t( "camera", natus::nsl::binding::camera ),
                __mapping_t( "camera_position", natus::nsl::binding::camera_position ),
                __mapping_t( "viewport", natus::nsl::binding::viewport )
            } ;

            for( auto const& m : __mappings ) if( b == m.first ) return m.second ;

            return natus::nsl::binding::unknown ;
        }

        static natus::ntd::string_cref_t to_string( natus::nsl::binding const b ) noexcept
        {
            static natus::ntd::string_t const __values[] = { 
                "unknown", "position", "normal", "tangent",
                "texcoord0", "texcoord1", "texcoord2", "texcoord3", "texcoord4", "texcoord5", "texcoord6", "texcoord7",
                "color0", "color1", "color2", "color3", "color4", "color5", "color6", "color7",
                "projection", "view", "world", "object", "camera", "camera_position", "viewport"
            } ;
            return __values[ size_t( b ) ] ;
        }

        static bool_t is_texcoord( natus::nsl::binding const b ) noexcept
        {
            return
                size_t( natus::nsl::binding::texcoord0 ) <= size_t( b ) &&
                size_t( natus::nsl::binding::texcoord7 ) >= size_t( b ) ;
        }

        static bool_t is_color( natus::nsl::binding const b ) noexcept
        {
            return
                size_t( natus::nsl::binding::color0 ) <= size_t( b ) &&
                size_t( natus::nsl::binding::color7 ) >= size_t( b ) ;
        }
    }

    namespace nsl
    {
        enum class type_base
        {
            unknown,
            tvoid,
            tfloat,
            tint,
            tuint,
        } ;

        enum class type_struct
        {
            unknown,
            vec1,
            vec2,
            vec3,
            vec4,
            mat2,
            mat3,
            mat4
        } ;

        enum class type_ext
        {
            unknown,
            singular,
            array,
            texture_1d,
            texture_2d,
            texture_2d_array
        } ;

        struct type
        {
            natus_this_typedefs( type ) ;

            type_base base = type_base::unknown ;
            type_struct struc = type_struct::unknown ;
            type_ext ext = type_ext::unknown ;

            type( void_t ) noexcept {}

            type( type_base const tb, type_struct const ts, type_ext const te ) noexcept : 
                base( tb), struc( ts), ext( te ){}
            type( this_cref_t rhv ) noexcept
            {
                *this = rhv ;
            }
            this_ref_t operator = ( this_cref_t t ) noexcept
            {
                base = t.base ;
                struc = t.struc ;
                ext = t.ext ;
                return *this ;
            }

            bool_t operator == ( this_cref_t t ) const noexcept
            {
                return base == t.base && struc == t.struc && ext == t.ext ;
            }

            bool_t operator != ( this_cref_t t ) const noexcept
            {
                return !this_t::operator == ( t ) ;
            }

            bool_t is_texture( void_t ) const noexcept 
            { 
                return 
                    ext == natus::nsl::type_ext::texture_1d || 
                    ext == natus::nsl::type_ext::texture_2d ||
                    ext == natus::nsl::type_ext::texture_2d_array ;
            }

            static this_t as_void( void_t ) noexcept
            {
                return { type_base::tvoid, type_struct::unknown, type_ext::singular } ;
            }

            static this_t as_float( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec1, type_ext::singular } ;
            }

            static this_t as_int( void_t ) noexcept
            {
                return { type_base::tint, type_struct::vec1, type_ext::singular } ;
            }

            static this_t as_uint( void_t ) noexcept
            {
                return { type_base::tuint, type_struct::vec1, type_ext::singular } ;
            }

            static this_t as_vec1( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec1, type_ext::singular } ;
            }

            static this_t as_vec2( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec2, type_ext::singular } ;
            }

            static this_t as_vec3( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec3, type_ext::singular } ;
            }

            static this_t as_vec4( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec4, type_ext::singular } ;
            }

            static this_t as_mat2( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::mat2, type_ext::singular } ;
            }

            static this_t as_mat3( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::mat3, type_ext::singular } ;
            }

            static this_t as_mat4( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::mat4, type_ext::singular } ;
            }

            static this_t as_tex1d( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec4, type_ext::texture_1d } ;
            }

            static this_t as_tex2d( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec4, type_ext::texture_2d } ;
            }

            static this_t as_tex2d_array( void_t ) noexcept
            {
                return { type_base::tfloat, type_struct::vec4, type_ext::texture_2d_array } ;
            }


        };
        natus_typedef( type ) ;

        

        static natus::nsl::type_t to_type( natus::ntd::string_cref_t t ) noexcept
        {
            typedef std::pair< natus::ntd::string_t, natus::nsl::type_t > __mapping_t ;
            static __mapping_t const __mappings[] = {
                __mapping_t( "void_t", type_t::as_void() ),
                __mapping_t( "float_t", type_t::as_float() ),
                __mapping_t( "int_t", type_t::as_int() ),
                __mapping_t( "uint_t", type_t::as_uint() ),
                __mapping_t( "vec1_t", type_t::as_vec1() ),
                __mapping_t( "vec2_t", type_t::as_vec2() ),
                __mapping_t( "vec3_t", type_t::as_vec3() ),
                __mapping_t( "vec4_t", type_t::as_vec4() ),
                __mapping_t( "mat2_t", type_t::as_mat2() ),
                __mapping_t( "mat3_t", type_t::as_mat3() ),
                __mapping_t( "mat4_t", type_t::as_mat4() ),
                __mapping_t( "tex1d_t", type_t::as_tex1d() ),
                __mapping_t( "tex2d_t", type_t::as_tex2d() ),
                __mapping_t( "tex2d_array_t", type_t::as_tex2d_array() )
            } ;

            for( auto const & m : __mappings ) if( m.first == t ) return m.second ;

            return natus::nsl::type_t() ;
        }

        enum class build_in
        {
            unknown,
            add,
            sub,
            div,
            mul,
            dot,
            cross,
            pulse,
            step,
            mix,
            pow,
            texture,
            rt_texture,
            num_build_ins
        };

        static build_in to_build_in( natus::ntd::string_cref_t s ) noexcept
        {
            if( s == "add" ) return natus::nsl::build_in::add ;
            else if( s == "sub" ) return natus::nsl::build_in::sub ;
            else if( s == "div" ) return natus::nsl::build_in::div ;
            else if( s == "mul" ) return natus::nsl::build_in::mul ;
            else if( s == "dot" ) return natus::nsl::build_in::dot ;
            else if( s == "cross" ) return natus::nsl::build_in::cross ;
            else if( s == "pulse" ) return natus::nsl::build_in::pulse ;
            else if( s == "step" ) return natus::nsl::build_in::step ;
            else if( s == "mix" ) return natus::nsl::build_in::mix ;
            else if( s == "pow" ) return natus::nsl::build_in::pow ;
            else if( s == "texture" ) return natus::nsl::build_in::texture ;
            else if( s == "rt_texture" ) return natus::nsl::build_in::rt_texture ;

            return natus::nsl::build_in::unknown ;
        }

        // the fragments function signature
        struct signature
        {
            struct arg
            {
                natus::nsl::type_t type ;
                natus::ntd::string_t name ;
            };
            natus_typedef( arg ) ;

            natus::nsl::type_t return_type ;
            natus::ntd::string_t name ;
            natus::ntd::vector< arg_t > args ;

            bool_t operator == ( signature const& rhv ) const
            {
                return !( *this != rhv ) ;
            }

            bool_t operator != ( signature const& rhv ) const
            {
                if( return_type != rhv.return_type ) return true ;
                if( name != rhv.name ) return true ;
                if( args.size() != rhv.args.size() ) return true ;

                for( size_t i = 0; i < args.size(); ++i )
                {
                    if( args[ i ].type != rhv.args[ i ].type ) return true ;
                }
                return false ;
            }
        };
        natus_typedef( signature ) ;

        static natus::nsl::type_t deduce_return_type( 
            natus::nsl::build_in const bi, natus::ntd::vector< natus::nsl::type_t > const & args ) noexcept
        {
            if( bi == natus::nsl::build_in::unknown ) return natus::nsl::type_t() ;

            using signatures_t = natus::ntd::vector< signature_t > ;

            static const signatures_t adds =
            {
                { type_t::as_vec1(), "add", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec2(), "add", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "add", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec4(), "add", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" } } },

                { type_t::as_mat2(), "add", { { type_t::as_mat2(), "a" }, { type_t::as_mat2(), "b" } } },
                { type_t::as_mat3(), "add", { { type_t::as_mat3(), "a" }, { type_t::as_mat3(), "b" } } },
                { type_t::as_mat4(), "add", { { type_t::as_mat4(), "a" }, { type_t::as_mat4(), "b" } } },
            } ;

            static const signatures_t subs =
            {
                { type_t::as_vec1(), "sub", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec2(), "sub", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "sub", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec4(), "sub", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" } } },

                { type_t::as_mat2(), "sub", { { type_t::as_mat2(), "a" }, { type_t::as_mat2(), "b" } } },
                { type_t::as_mat3(), "sub", { { type_t::as_mat3(), "a" }, { type_t::as_mat3(), "b" } } },
                { type_t::as_mat4(), "sub", { { type_t::as_mat4(), "a" }, { type_t::as_mat4(), "b" } } },
            } ;

            static const signatures_t divs =
            {
                { type_t::as_vec1(), "div", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec2(), "div", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "div", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec4(), "div", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" } } },
                { type_t::as_mat2(), "div", { { type_t::as_mat2(), "a" }, { type_t::as_mat2(), "b" } } },
                { type_t::as_mat3(), "div", { { type_t::as_mat3(), "a" }, { type_t::as_mat3(), "b" } } },
                { type_t::as_mat4(), "div", { { type_t::as_mat4(), "a" }, { type_t::as_mat4(), "b" } } },
            } ;

            static const signatures_t muls = 
            {  
                { type_t::as_vec1(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec2(), "mul", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "mul", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },

                { type_t::as_vec2(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec2(), "mul", { { type_t::as_vec2(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec3(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec3(), "mul", { { type_t::as_vec3(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec4(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_vec4(), "b" } } },
                { type_t::as_vec4(), "mul", { { type_t::as_vec4(), "a" }, { type_t::as_vec1(), "b" } } },

                { type_t::as_mat2(), "mul", { { type_t::as_mat2(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_mat2(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_mat2(), "b" } } },
                { type_t::as_mat3(), "mul", { { type_t::as_mat3(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_mat3(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_mat3(), "b" } } },
                { type_t::as_mat4(), "mul", { { type_t::as_mat4(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_mat4(), "mul", { { type_t::as_vec1(), "a" }, { type_t::as_mat4(), "b" } } },

                { type_t::as_vec2(), "mul", { { type_t::as_mat2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "mul", { { type_t::as_mat3(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec4(), "mul", { { type_t::as_mat4(), "a" }, { type_t::as_vec4(), "b" } } },


                { type_t::as_mat2(), "mul", { { type_t::as_vec2(), "a" }, { type_t::as_mat2(), "b" } } },
                { type_t::as_mat3(), "mul", { { type_t::as_vec3(), "a" }, { type_t::as_mat3(), "b" } } },
                { type_t::as_mat4(), "mul", { { type_t::as_vec4(), "a" }, { type_t::as_mat4(), "b" } } },
            };

            static const signatures_t dots =
            {
                { type_t::as_vec1(), "dot", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" } } },
                { type_t::as_vec1(), "dot", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec1(), "dot", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },
                { type_t::as_vec1(), "dot", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" } } },
            } ;

            static const signatures_t crosss =
            { 
                { type_t::as_vec3(), "cross", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" } } },
                { type_t::as_vec3(), "cross", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" } } },
            } ;

            static const signatures_t pulses =
            {
                { type_t::as_vec1(), "pulse", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" }, { type_t::as_vec1(), "x" } } },
                { type_t::as_vec2(), "pulse", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" }, { type_t::as_vec2(), "x" } } },
                { type_t::as_vec3(), "pulse", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" }, { type_t::as_vec3(), "x" } } },
                { type_t::as_vec4(), "pulse", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" }, { type_t::as_vec4(), "x" } } },
            } ;

            static const signatures_t steps =
            {
                { type_t::as_vec1(), "step", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "x" } } },
                { type_t::as_vec2(), "step", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "x" } } },
                { type_t::as_vec3(), "step", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "x" } } },
                { type_t::as_vec4(), "step", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "x" } } },
            } ;

            static const signatures_t mixes =
            {
                { type_t::as_vec1(), "mix", { { type_t::as_vec1(), "a" }, { type_t::as_vec1(), "b" }, { type_t::as_vec1(), "x" } } },
                { type_t::as_vec2(), "mix", { { type_t::as_vec2(), "a" }, { type_t::as_vec2(), "b" }, { type_t::as_vec2(), "x" } } },
                { type_t::as_vec3(), "mix", { { type_t::as_vec3(), "a" }, { type_t::as_vec3(), "b" }, { type_t::as_vec3(), "x" } } },
                { type_t::as_vec4(), "mix", { { type_t::as_vec4(), "a" }, { type_t::as_vec4(), "b" }, { type_t::as_vec4(), "x" } } },
            } ;

            static const signatures_t pows =
            {
                { type_t::as_vec1(), "pow", { { type_t::as_vec1(), "x" }, { type_t::as_vec1(), "y" } } },
                { type_t::as_vec2(), "pow", { { type_t::as_vec2(), "x" }, { type_t::as_vec2(), "y" } } },
                { type_t::as_vec3(), "pow", { { type_t::as_vec3(), "x" }, { type_t::as_vec3(), "y" } } },
                { type_t::as_vec4(), "pow", { { type_t::as_vec4(), "x" }, { type_t::as_vec4(), "y" } } },
            } ;

            static const signatures_t textures =
            {
                { type_t::as_vec4(), "texture", { { type_t::as_tex1d(), "t" }, { type_t::as_vec1(), "uv" } } },
                { type_t::as_vec4(), "texture", { { type_t::as_tex2d(), "t" }, { type_t::as_vec2(), "uv" } } },
                { type_t::as_vec4(), "texture", { { type_t::as_tex2d_array(), "t" }, { type_t::as_vec3(), "uv" } } },
            } ;

            static const signatures_t sigs[] =
            {
                adds, subs, divs, muls, dots, crosss, pulses, steps, mixes, pows, textures
            } ;

            signatures_t const & chosen = sigs[ size_t(bi) ] ;

            for( auto const & sig : chosen )
            {
                if( sig.args.size() != args.size() ) continue ;

                size_t match = 0 ;
                for( size_t i = 0; i < args.size(); ++i )
                {
                    if( sig.args[ i ].type != args[ i ] ) break ;
                    ++match ;
                }
                
                if( match == sig.args.size() )
                    return sig.return_type ;
            }
            
            return natus::nsl::type_t() ;
        }
    }
}