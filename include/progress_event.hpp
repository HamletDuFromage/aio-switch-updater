#pragma once

class ProgressEvent{
private:
    ProgressEvent() {}
    int _current = 0;
    int _max = 60;

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
    }

    inline void setTotalSteps(int steps) { _max = steps; }
    inline void setStep(int step) { _current = step; }
    inline int getStep() { return _current; }
    inline bool finished() { return (_current == _max) ; }
    inline int getMax() { return _max; }
};
