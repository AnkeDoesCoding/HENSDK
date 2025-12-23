#ifndef _HENLEVEL_PRIMITIVES_H_
#define _HENLEVEL_PRIMITIVES_H_

namespace hen::level::primitives
{
    namespace cube
    {
        extern float Vertices[144];
        extern unsigned int Indices[36];
    }

    namespace sphere
    {
        extern float Vertices[156];
        extern unsigned int Indices[144];
    }
}

#endif // !_HENLEVEL_PRIMITIVES_H_