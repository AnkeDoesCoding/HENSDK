#ifndef _HENLEVEL_PRIMITIVES_H_
#define _HENLEVEL_PRIMITIVES_H_

#include <cstdint>

namespace hen::level::primitives
{
    namespace cube
    {
        extern float Vertices[144];
        extern uint32_t Indices[36];
    }

    namespace sphere
    {
        extern float Vertices[156];
        extern uint32_t Indices[144];
    }
}

#endif // !_HENLEVEL_PRIMITIVES_H_