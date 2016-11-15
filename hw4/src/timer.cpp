#include "timer.hpp"

ScheduledTask::ScheduledTask(
    bool active,
    int period,
    int last_execute,
    void (*task)(int elapsed_time)) :
    active(active),
    period(period),
    last_execute(last_execute),
    task(task)
{
    //
}

Timer::Timer() :
    time_increment(default_time_increment),
    start(boost::chrono::system_clock::now()),
    tasks()
{

}
Timer::~Timer() {

}

void Timer::resetClock() {
    start = boost::chrono::system_clock::now();
}

int Timer::checkClock(TimeIncrement inc) {
    switch (inc) {
        case TI_NANO:
            return boost::chrono::duration_cast<boost::chrono::nanoseconds>(
                boost::chrono::system_clock::now() - this->start).count();
        case TI_MICRO:
            return boost::chrono::duration_cast<boost::chrono::microseconds>(
                boost::chrono::system_clock::now() - this->start).count();
        case TI_MILLI:
            return boost::chrono::duration_cast<boost::chrono::milliseconds>(
                boost::chrono::system_clock::now() - this->start).count();
        default:
            fprintf(stderr, "Timer::checkClock ERROR invalid time increment %d\n",
                inc);
            exit(1);
    }
}

TaskID Timer::registerTask(int period, void (*task)(int elapsed_time)) {
    TaskID new_id = this->tasks.size();
    this->tasks.emplace_back(true, period, -1, task);
    return new_id;
}
void Timer::activateTask(TaskID id) {
    assert(id < this->tasks.size());
    this->tasks[id].active = true;
}
void Timer::deactivateTask(TaskID id) {
    assert(id < this->tasks.size());
    this->tasks[id].active = false;
}
void Timer::trigger() {
    int cur_time = this->checkClock(this->time_increment);
    for (ScheduledTask &task : this->tasks) {
        if (task.active) {
            // if last_execute of task is -1, the task hasn't been triggered yet
            // skip the first trigger to set the correct last_execute
            if (task.last_execute == -1) {
                task.last_execute = cur_time;
            } else {
                int elapsed_time = cur_time - task.last_execute;
                if (elapsed_time > task.period) {
                    task.task(elapsed_time);
                    task.last_execute = cur_time;
                }
            }
        }
    }
}
