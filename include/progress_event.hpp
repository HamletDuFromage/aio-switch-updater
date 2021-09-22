#pragma once

class ProgressEvent
{
private:
    ProgressEvent() {}
    int _current = 0;
    int _max = 60;
    double _now = 0;
    double _total = 0;
    double _speed = 0;
    long _status_code = 0;
    bool _interupt = false;

public:
    ProgressEvent(const ProgressEvent&) = delete;
    ProgressEvent& operator=(const ProgressEvent&) = delete;
    ProgressEvent(ProgressEvent&&) = delete;
    ProgressEvent& operator=(ProgressEvent&&) = delete;

    static auto& instance()
    {
        static ProgressEvent event;
        return event;
    }

    void reset()
    {
        _current = 0;
        _max = 60;
        _now = 0;
        _total = 0;
        _speed = 0;
        _status_code = 0;
        _interupt = false;
    }

    inline void setTotalSteps(int steps) { _max = steps; }
    inline void setTotalCount(double total) { _total = total; }
    inline void setSpeed(double speed) { _speed = speed; }
    inline void setStep(int step) { _current = step; }
    inline void setStatusCode(long status_code) { _status_code = status_code; }
    inline void incrementStep(int increment) { _current += increment; }
    inline void setNow(double now) { _now = now; }
    inline int getStep() { return _current; }
    inline double getNow() { return _now; }
    inline bool finished() { return (_current == _max); }
    inline int getMax() { return _max; }
    inline double getTotal() { return _total; }
    inline double getSpeed() { return _speed; }
    inline double getStatusCode() { return _status_code; }
    inline void setInterupt(bool interupt) { _interupt = interupt; }
    inline bool getInterupt() { return _interupt; }
};
