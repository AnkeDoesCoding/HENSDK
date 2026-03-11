#include "core/henJobSystem.h"

#include "tools/henConsole.h"
#include "core/henTimer.h"

#if PLATFORM_WINDOWS
    #define NOMINMAX
    #include <windows.h>
#endif // !PLATFORM_WINDOWS

#include <algorithm>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <cmath>

template <typename Item, size_t Capacity>
class ThreadSafeRingBuffer
{
public:
    inline bool PushBack(const Item& item)
    {
        bool result = false;
        m_Lock.lock();

        size_t next = (m_Head + 1) % Capacity;
        if (next != m_Tail)
        {
            m_Data[m_Head] = item;
            m_Head = next;
            result = true;
        }

        m_Lock.unlock();
        return result;
    }

    inline bool PopFront(Item& item)
    {
        bool result = false;
        m_Lock.lock();

        if (m_Tail != m_Head)
        {
            item = m_Data[m_Tail];
            m_Tail = (m_Tail + 1 ) % Capacity;
            result = true;
        }

        m_Lock.unlock();
        return result;
    }

private:
    Item m_Data[Capacity];
    size_t m_Tail = 0;
    size_t m_Head = 0;
    std::mutex m_Lock;
};

namespace hen::jobsystem
{
    bool Initialised = false;

    static uint32_t NumberOfThreads = 0;
    std::atomic<uint64_t> CurrentLabel;
    std::atomic<uint64_t> FinishedLabel;

    static ThreadSafeRingBuffer<std::function<void()>, 256> JobPool;

    static std::condition_variable WakeCondition;
    static std::mutex WakeMutex;

    inline void Poll()
    {
        WakeCondition.notify_one();
        std::this_thread::yield();
    }

    void Initialise()
    {
        Timer timer;

        FinishedLabel.store(0);
        CurrentLabel.store(0);

        auto numberOfCores = std::thread::hardware_concurrency();
        NumberOfThreads = std::max(1u, numberOfCores);

        for (uint32_t i = 0; i < NumberOfThreads; ++i)
        {
            std::thread worker([] 
            {
                std::function<void()> job;

                while (true)
                {
                    if (JobPool.PopFront(job))
                    {
                        job();
                        FinishedLabel.fetch_add(1);
                    }
                    else
                    {
                        std::unique_lock<std::mutex> lock(WakeMutex);
                        WakeCondition.wait(lock);
                    }
                }
            });

            // wierd windows shit
            #if PLATFORM_WINDOWS
                HANDLE handle = static_cast<HANDLE>(worker.native_handle());

                DWORD_PTR affinityMask = 1ull << i;
                DWORD_PTR affinityResult = SetThreadAffinityMask(handle, affinityMask);
                HEN_ASSERT(affinityResult > 0, "affinityResult Couldn't be created");

                std::wstringstream descString;
                descString << "hen::jobsystem_" << i;
                HRESULT result = SetThreadDescription(handle, descString.str().c_str());
                HEN_ASSERT(SUCCEEDED(result), "Couldn't set thread description");
            #endif // !PLATFORM_WINDOWS

            worker.detach(); // tell that mf to firm life 
        }

        Initialised = true;

        HEN_LOG("[hen::jobsystem] Initialised in " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms");
    }

    void Execute(const std::function<void()>& job)
    {
        CurrentLabel.fetch_add(1);

        while (!JobPool.PushBack(job))
        {
            Poll();
        }

        WakeCondition.notify_one();
    }

    void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(DispatchArgs)>& job)
    {
        if (jobCount == 0 || groupSize == 0)
		{
            HEN_WARN("[hen::jobsystem] Couldn't dispatch jobs due to job count or group size being 0");
			return;
		}

        // an overestimate of job groups
		const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

		CurrentLabel.fetch_add(groupCount);

		for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
		{
            // for each group, make one job
			const auto& jobGroup = [jobCount, groupSize, job, groupIndex]() 
            {
				const uint32_t groupJobOffset = groupIndex * groupSize;
				const uint32_t groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

				DispatchArgs args;
				args.GroupIndex = groupIndex;

				for (uint32_t i = groupJobOffset; i < groupJobEnd; ++i)
				{
					args.JobIndex = i;
					job(args);
				}
			};

			while (!JobPool.PushBack(jobGroup)) 
            { 
                Poll(); 
            }

			WakeCondition.notify_one();
		}
    }
    
    void Wait()
    {
        while (IsBusy())
        {
            Poll();
        }
    }

    bool IsBusy()
    {
        return FinishedLabel.load() < CurrentLabel.load();
    }
}