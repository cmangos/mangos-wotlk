#include "CodeBench.h"
#include "Log/Log.h"

ChronoTimeTracker::~ChronoTimeTracker()
{
    std::chrono::nanoseconds elapsed = elapsedNanos();
    sLog.outError("%s: time elapsed: %ldns, %ldµs, %ldms, %lds",
        m_name.c_str(),
        nanos(elapsed).count(),
        micros(elapsed).count(),
        millis(elapsed).count(),
        secs(elapsed).count());
}
