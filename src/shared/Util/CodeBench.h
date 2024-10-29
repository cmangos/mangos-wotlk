struct ChronoTimeTracker
{
    public:
        ChronoTimeTracker() : m_startTime(std::chrono::steady_clock::now()), m_name("Unnamed Timer") {}
        ChronoTimeTracker(std::string name) : m_startTime(std::chrono::steady_clock::now()), m_name(name) {}
        ~ChronoTimeTracker();
        std::chrono::seconds elapsedSeconds()
        {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_startTime);
        }
        std::chrono::milliseconds elapsedMillis()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_startTime);
        }
        std::chrono::microseconds elapsedMicros()
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startTime);
        }
        std::chrono::nanoseconds elapsedNanos()
        {
            return std::chrono::steady_clock::now() - m_startTime;
        }
    private:
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> m_startTime;
        std::string m_name;
        inline constexpr std::chrono::nanoseconds nanos(std::chrono::nanoseconds nanos) { return nanos; }
        inline constexpr std::chrono::microseconds micros(std::chrono::nanoseconds nanos) { return std::chrono::duration_cast<std::chrono::microseconds>(nanos); }
        inline constexpr std::chrono::milliseconds millis(std::chrono::nanoseconds nanos) { return std::chrono::duration_cast<std::chrono::milliseconds>(nanos); }
        inline constexpr std::chrono::seconds secs(std::chrono::nanoseconds nanos) { return std::chrono::duration_cast<std::chrono::seconds>(nanos); }
};