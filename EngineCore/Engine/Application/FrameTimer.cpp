#include "FrameTimer.h"

#include <windows.h>

void FrameTimer::Refresh()
{
    lasttick =  GetTickCount();
    interval = GetTickCount() - lasttick;
}

bool FrameTimer::Timer(int item)
{
    if (lasttick - lastticks[item] >= intervals[item])
    {
        lastticks[item] = lasttick;
        return true;
    }
    return false;
}

float FrameTimer::Interval(float actionpersecond)
{
    return float(actionpersecond) * float(interval) / 1000.0f;
}

void FrameTimer::Init()
{
    lastticks = new long[256];
    intervals = new long[256];
}

void FrameTimer::Item(int item, long interval)
{
    intervals[item] = interval;
}

