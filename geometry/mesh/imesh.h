
#pragma  once

#include "../typedefs.h"
#include "../api.h"
#include "../result.h"
#include "../protos.h"

#include "../enums/component_formats.h"

namespace natus
{
    namespace geometry
    {
        class NATUS_GEOMETRY_API imesh
        {
        public:

            virtual ~imesh( void_t ) {}
        };
        natus_typedef( imesh ) ;
    }
}
