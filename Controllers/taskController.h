#pragma once
#include <TaskScheduler.h>

// Forward declarations of motors and global flag
extern bool showActive;
class MotorLineal;
extern MotorLineal leftMouth;
extern MotorLineal centralMouth;
extern MotorLineal rightMouth;
extern MotorLineal leftHead;
extern MotorLineal rightHead;
class MotorPersiana;
extern MotorPersiana centralHead;

// Task Scheduler object
extern Scheduler runner;

// Timeline task
extern Task tTimeline;

// Function to initialize the timeline task
void initTimelineTask();
