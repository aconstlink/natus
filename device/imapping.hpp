

#pragma once

namespace natus
{
    namespace device
    {
        class imapping
        {
        private:

            natus::std::string_t _name ;

        public:

            imapping( natus::std::string_cref_t name ) : _name( name ) {}
            imapping( imapping&& rhv ) : _name( ::std::move( rhv._name ) ) {}
            imapping( imapping const& rhv ) : _name( rhv._name ) {}
            virtual ~imapping( void_t ) {}

            natus::std::string_cref_t name( void_t ) const noexcept { return _name ; }

        protected:

            void_t set_name( natus::std::string_cref_t name ) noexcept { _name = name ; }
            natus::std::string_t move_name( void_t ) noexcept { return ::std::move( _name ) ; }

        public:

            virtual void_t update( void_t ) noexcept = 0;
        };
        natus_typedef( imapping ) ;
        using imapping_res_t = natus::soil::res< imapping_t > ;
    }
}