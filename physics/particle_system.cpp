
#include "particle_system.h"

using namespace natus::physics ;

particle_system::particle_system( void_t ) noexcept {}
particle_system::particle_system( this_rref_t rhv ) noexcept 
{
    _particles = std::move( rhv._particles ) ;
    _forces = std::move( rhv._forces ) ;
    _emitter = std::move( rhv._emitter ) ;
}

            
particle_system::this_ref_t particle_system::operator = ( this_rref_t rhv ) noexcept 
{
    _particles = std::move( rhv._particles ) ;
    _forces = std::move( rhv._forces ) ;
    _emitter = std::move( rhv._emitter ) ;
    return *this ;
}

void_t particle_system::attach_emitter( emitter_res_t emt ) noexcept
{
    auto iter = std::find_if( _emitter.begin(), _emitter.end(), [&]( emitter_data_cref_t d )
    {
        return d.emt == emt ;
    } ) ;
    if( iter != _emitter.end() ) return ;

    _emitter.push_back( { emt, 0, 0, 0 } ) ;
}

void_t particle_system::detach_emitter( emitter_res_t emt ) noexcept
{
    auto iter = std::find_if( _emitter.begin(), _emitter.end(), [&]( emitter_data_cref_t d )
    {
        return d.emt == emt ;
    } ) ;
    if( iter == _emitter.end() ) return ;

    _emitter.erase( iter ) ;
}

void_t particle_system::attach_force_field( force_field_res_t ff ) noexcept
{
    auto iter = std::find_if( _forces.begin(), _forces.end(), [&]( force_field_data_cref_t d )
    {
        return d.ff == ff ;
    } ) ;
    if( iter != _forces.end() ) return ;
    _forces.push_back( { ff } ) ;
}

void_t particle_system::detach_force_field( force_field_res_t ff ) noexcept
{
    auto iter = std::find_if( _forces.begin(), _forces.end(), [&]( force_field_data_cref_t d )
    {
        return d.ff == ff ;
    } ) ;
    if( iter == _forces.end() ) return ;

    _forces.erase( iter ) ;
}

void_t particle_system::clear( void_t ) noexcept
{
    _particles.clear() ;
    for( auto & e : _emitter )
    {
        e.emitted = 0 ;
        e.emit = 0 ;
        e.seconds = 0.0f ;
    }
}

void_t particle_system::update( float_t const dt ) noexcept 
{
    // update particle
    {
        #if !NATUS_PHYSICS_USE_PARALLEL_FOR
        for( auto & p : _particles )
        {
            p.age -= dt ;
        }
        #else
        natus::concurrent::parallel_for<size_t>( natus::concurrent::range_1d<size_t>( 0, _particles.size() ),
            [&]( natus::concurrent::range_1d<size_t> const & r )
            {
                for( size_t i=r.begin(); i<r.end(); ++i )
                {
                    _particles[i].age -= dt ;
                }
            } ) ;
        #endif
    }

    // reorder particles
    {
        size_t cur = 0 ;
        size_t last = _particles.size() ;
        for( size_t i=0; i<last; ++i )
        {
            auto const & p = _particles[i] ;
            if( p.age <= 0.0f )
            {
                _particles[i--] = _particles[ --last ] ;
            }
        }
        _particles.resize( last ) ;
    }

    // emit new particles
    {
        size_t new_emit = 0 ;
        for( auto & e : _emitter )
        {
            auto const emit = e.emt->calc_emits( e.emitted, e.seconds ) ;
            e.seconds += dt ;
            e.emitted += emit ;
            e.emit = emit ;
            new_emit += emit ;
        }

        size_t begin = _particles.size() ;
        _particles.resize( _particles.size() + new_emit ) ;

        for( auto & e : _emitter )
        {
            if( e.emit == 0 ) continue ;
            e.emt->emit( begin, e.emit, _particles ) ; begin += e.emit ;
            e.emit = 0 ;
        }
    }
            
    // reset particle physics here so 
    // value can be read elsewhere during a 
    // cycle of physics
    {
        for( auto & p : _particles )
        {
            //p.force.negate();//natus::math::vec2f_t() ;
            //p.acl = natus::math::vec2f_t() ;
            //p.vel = natus::math::vec2f_t() ;
        }
    }

    // do force fields
    {
        for( auto & f : _forces )
        {
            f.ff->apply( 0, _particles.size(), _particles ) ;
        }
    }
            
    // do physics
    {
        #if !NATUS_PHYSICS_USE_PARALLEL_FOR
        for( auto & p : _particles )
        {
            p.acl = p.force / p.mass ;
            p.vel += natus::math::vec2f_t( dt ) * p.acl ;
            p.pos += natus::math::vec2f_t( dt ) * p.vel ;
        }
        #else
        natus::concurrent::parallel_for<size_t>( natus::concurrent::range_1d<size_t>( 0, _particles.size() ),
            [&]( natus::concurrent::range_1d<size_t> const & r )
            {
                for( size_t i=r.begin(); i<r.end(); ++i )
                {
                    auto & p = _particles[i] ;
                    p.acl = p.force / p.mass ;
                    p.vel += natus::math::vec2f_t( dt ) * p.acl ;
                    p.pos += natus::math::vec2f_t( dt ) * p.vel ;
                }
            } ) ;
        #endif
    }

    // compute extend
    {
        natus::math::vec2f_t min_xy( std::numeric_limits<float_t>::max(), std::numeric_limits<float_t>::max() ) ;
        natus::math::vec2f_t max_xy( std::numeric_limits<float_t>::min(), std::numeric_limits<float_t>::min() ) ;

        for( auto & p : _particles )
        {
            min_xy = min_xy.greater_than( p.pos ).select( p.pos, min_xy ) ;
            max_xy = max_xy.less_than( p.pos ).select( p.pos, max_xy ) ;
        }

        _extend = natus::math::vec4f_t( min_xy, max_xy ) ;
    }
}

void_t particle_system::on_particles( on_particles_funk_t funk ) noexcept
{
    funk( _particles ) ;
}

std::array< natus::math::vec2f_t, 4 > particle_system::get_extend_rect( void_t ) const noexcept 
{
    return 
    {
        _extend.xy() ,
        _extend.xw() ,
        _extend.zw() ,
        _extend.zy() 
    } ;
}