#ifndef _HENJOBSYSTEM_H_
#define _HENJOBSYSTEM_H_

#include <functional>

// again shamelessly copied from Wicked Engine, thanks Turan!!!

namespace hen
{
    namespace jobsystem
    {
        struct DispatchArgs
        {
        	uint32_t JobIndex;
        	uint32_t GroupIndex;
        };

        void Initialise();
        
        void Execute(const std::function<void()>& job);
        void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(DispatchArgs)>& job);
        void Wait();

        bool IsBusy();

        extern bool Initialised;
    }
}

#endif // !_HENJOBSYSTEM_H_