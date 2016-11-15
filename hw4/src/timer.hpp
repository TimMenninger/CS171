#ifndef TIMER_HPP
#define TIMER_HPP

#include <vector>
#include <boost/chrono.hpp>

enum TimeIncrement {TI_MILLI, TI_MICRO, TI_NANO};
static const TimeIncrement default_time_increment = TI_MICRO;

static const int millisec_to_sec = 1e3;
static const int microsec_to_sec = 1e6;
static const int nanosec_to_sec = 1e9;

typedef unsigned int TaskID;
struct ScheduledTask {
    bool active;

    int period; // use 0 for executing on every trigger
    int last_execute;
    void (*task)(int elapsed_time);

    ScheduledTask(
        bool active,
        int period,
        int last_execute,
        void (*task)(int elapsed_time));
};

class Timer {
private:
    TimeIncrement time_increment;
    boost::chrono::system_clock::time_point start;
    std::vector<ScheduledTask> tasks;

public:
    explicit Timer();
    ~Timer();

    void resetClock();
    int checkClock(TimeIncrement inc);

    TaskID registerTask(int period, void (*task)(int elapsed_time));
    void activateTask(TaskID id);
    void deactivateTask(TaskID id);
    void trigger();
};

#endif
