#pragma once

class ProgressEvent{
private:
    ProgressEvent() {}
    int _current = 0;
    int _max = 60;
    double _now = 0;
    double _total = 0;
    double _speed = 0;

public:
    ProgressEvent(const ProgressEvent&) = delete;
    ProgressEvent& operator=(const ProgressEvent &) = delete;
    ProgressEvent(ProgressEvent &&) = delete;
    ProgressEvent & operator=(ProgressEvent &&) = delete;

    static auto& instance(){
        static ProgressEvent event;
        return event;
    }

    void reset() {
        _current = 0;
        _max = 60;
        _now = 0;
        _total = 0;
        _speed = 0;
    }

    inline void setTotalSteps(int steps) { _max = steps; }
    inline void setTotalCount(double total) { _total = total; }
    inline void setSpeed(double speed) { _speed = speed; }
    inline void setStep(int step) { _current = step; }
    inline void incrementStep(int increment) {_current += increment; }
    inline void setNow(double now) { _now = now; }
    inline int getStep() { return _current; }
    inline double getNow() { return _now; }
    inline bool finished() { return (_current == _max) ; }
    inline int getMax() { return _max; }
    inline double getTotal() { return _total; }
    inline double getSpeed() { return _speed; }
};
