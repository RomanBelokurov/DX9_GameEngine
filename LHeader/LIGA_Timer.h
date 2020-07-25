
//****************************
// In-Game TIMING code here!!!
//****************************

class heTime
{
public:
     long lasttick;
     long interval;
     long *lastticks;
     long *intervals;
public:
     void Refresh();
     bool Timer( int );
     float Interval( float );
     void Init();
     void Item( int, long );
};

void heTime::Refresh()
{
     lasttick = GetTickCount();
     interval = GetTickCount() - lasttick;
}

bool heTime::Timer( int item )
{
     if( lasttick - lastticks[item] >= intervals[item] )
     {
          lastticks[item] = lasttick;
          return true;
     }
     return false;
}

float heTime::Interval( float actionpersecond )
{
     return float( actionpersecond ) * float( interval ) / 1000.0f;
}

void heTime::Init()
{
     lastticks = new long[256];
     intervals = new long[256];
}

void heTime::Item( int item, long interval )
{
     intervals[item] = interval;
}

heTime heTimer;
