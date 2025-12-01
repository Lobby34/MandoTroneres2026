#include "./taskController.h"

// TimelineEvent definition
using TimelineEvent = struct
{
    unsigned long timeMs;
    void (*action)();
};

// Timeline array
TimelineEvent timeline[] = {
    {0, [] { showActive = true; }},
    {1000, [] { leftMouth.goUpTimed(5000); }},
    {2300, [] { centralMouth.goUpMax(); }},
    {6010, [] { leftMouth.goDownTimed(1000); }},
    {7020, [] { leftMouth.goDownMax(); }},
    {11030, [] { leftMouth.goUpMax(); }},
    {21040, [] { showActive = false; }}
};

// Calculate the number of events
const int numOfEventsInTimeline = sizeof(timeline) / sizeof(timeline[0]);

// Timeline task declaration
Task tTimeline(
    10, -1, [] {
        static int index = 0;
        unsigned long now = millis();

        while (index < numOfEventsInTimeline && timeline[index].timeMs <= now) {
            timeline[index].action();
            index++;
        }

        if (index >= numOfEventsInTimeline) {
            tTimeline.disable();
            index = 0; // Reset for next run
        }
    },
    &runner, false
);

// Optional initialization function
void initTimelineTask() {
    tTimeline.disable();
}
