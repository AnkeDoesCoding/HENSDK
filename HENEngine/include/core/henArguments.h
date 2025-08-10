#ifndef _HENARGUMENTS_H_
#define _HENARGUMENTS_H_

#include <string>

namespace hen::arguments
{
    void Parse(int argumentCount, char* argumentVector[]);
    bool HasArgument(const std::string& argument);
}

#endif _HENARGUMENTS_H_