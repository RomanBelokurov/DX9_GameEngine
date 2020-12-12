#pragma once

class FrameTimer
{
    public:
        long lasttick;
        long interval;
        long* lastticks;
        long* intervals;
    
    public:
        void Refresh();
        bool Timer(int);
        float Interval(float);
        void Init();
        void Item(int, long);
};

