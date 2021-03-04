
#include "particle.h"
#include "emitters.hpp"
#include "force_fields.hpp"

namespace natus
{
    namespace physics
    {
        class NATUS_PHYSICS_API particle_system
        {
            natus_this_typedefs( particle_system ) ;

        public:
        
            particle_system( void_t ) noexcept ;
            particle_system( this_cref_t rhv ) noexcept = delete ;
            particle_system( this_rref_t rhv ) noexcept ;

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t rhv ) noexcept  ;

        private:

            // particle array
            natus::ntd::vector< particle_t > _particles ;

            struct emitter_data
            {
                emitter_res_t emt ;

                // time since attached
                float_t seconds ;

                size_t emitted ;

                size_t emit ;
            };
            natus_typedef( emitter_data ) ;

            // emitter array
            natus::ntd::vector< emitter_data_t > _emitter ;

            struct force_field_data
            {
                force_field_res_t ff ;
            };
            natus_typedef( force_field_data ) ;

            // force fields
            natus::ntd::vector< force_field_data_t > _forces ;

        public:

            void_t attach_emitter( emitter_res_t emt ) noexcept ;

            void_t detach_emitter( emitter_res_t emt ) noexcept ;

            void_t attach_force_field( force_field_res_t ff ) noexcept ;

            void_t detach_force_field( force_field_res_t ff ) noexcept ;

            void_t clear( void_t ) noexcept ;

        public: 

            void_t update( float_t const dt ) noexcept ;

            typedef std::function< void_t ( natus::ntd::vector< particle_t > const &  ) > on_particles_funk_t ;
            void_t on_particles( on_particles_funk_t funk ) noexcept ;
        };
        natus_res_typedef( particle_system ) ;
    }
}