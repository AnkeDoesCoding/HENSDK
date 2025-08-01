#ifndef _HENTIMER_H_
#define _HENTIMER_H_

#include <chrono>

namespace hen
{
	struct Timer
	{
		std::chrono::high_resolution_clock::time_point TimeStamp = std::chrono::high_resolution_clock::now();

		// Record a reference timestamp
		inline void record()
		{
			TimeStamp = std::chrono::high_resolution_clock::now();
		}

		// Elapsed time in seconds between the wi::Timer creation or last recording and "timeStamp2"
		inline double ElapsedSecondsSince(std::chrono::high_resolution_clock::time_point timeStamp2)
		{
			std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(timeStamp2 - TimeStamp);
			return timeSpan.count();
		}

		// Elapsed time in seconds since the wi::Timer creation or last recording
		inline double ElapsedSeconds()
		{
			return ElapsedSecondsSince(std::chrono::high_resolution_clock::now());
		}

		// Elapsed time in milliseconds since the wi::Timer creation or last recording
		inline double ElapsedMilliseconds()
		{
			return ElapsedSeconds() * 1000.0;
		}
		
		inline double RecordElapsedSeconds()
		{
			auto timeStamp2 = std::chrono::high_resolution_clock::now();
			auto elapsed = ElapsedSecondsSince(timeStamp2);
			TimeStamp = timeStamp2;
			return elapsed;
		}
	};
}


#endif // !_HENTIMER_H_