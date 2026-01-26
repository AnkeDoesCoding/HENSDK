#include "core/henArguments.h"

#include <unordered_set>

namespace hen::arguments
{
    static std::unordered_set<std::string> Arguments;

    void Parse(int argumentCount, char* argumentVector[])
    {
        for (int i = 1; i < argumentCount; i++)
		{
			Arguments.insert(std::string(argumentVector[i]));
		}
    }

    bool HasArgument(const std::string& argument)
    {
        return Arguments.find(argument) != Arguments.end();
    }
}