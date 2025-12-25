#ifndef _HENTIMER_H_
#define _HENTIMER_H_

#include <chrono>

// yes this is shamelessly copied from https://github.com/turanszkij/WickedEngine/blob/master/WickedEngine/wiTimer.h

namespace hen
{
	struct Timer
	{
		std::chrono::high_resolution_clock::time_point TimeStamp = std::chrono::high_resolution_clock::now();

		inline void Record()
		{
			TimeStamp = std::chrono::high_resolution_clock::now();
		}

		inline double ElapsedSecondsSince(std::chrono::high_resolution_clock::time_point timeStamp2)
		{
			std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(timeStamp2 - TimeStamp);
			return timeSpan.count();
		}

		inline double ElapsedSeconds()
		{
			return ElapsedSecondsSince(std::chrono::high_resolution_clock::now());
		}

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