#define _TASK_LTS_POINTER  // Enables LTS for v4.0.3

#include <TaskScheduler.h>
#include "./enviroment.h"
#include "./Classes/motorLineal.cpp"

Scheduler runner;            // Global – used by motorLineal
boolean showActive = false;  // Global - used to know if show is active

/**
*
*TEST REMOVE AFTER FINISHING
*
*/

int i = 0;

/**
*
*
*
*/

MotorLineal leftMouth(MOTOR_1_A_PIN, MOTOR_1_B_PIN, MOTOR_1_MAX_MILLIS_OPENED);
MotorLineal centralMouth(MOTOR_3_A_PIN, MOTOR_3_B_PIN, MOTOR_3_MAX_MILLIS_OPENED);
MotorLineal rigthMouth(MOTOR_4_A_PIN, MOTOR_4_B_PIN, MOTOR_4_MAX_MILLIS_OPENED);

using TimelineEvent = struct
{
  unsigned long timeMs;
  void (*action)();
};

TimelineEvent timeline[] = {
  { 0, [] {
     showActive = true;
   } },
  { 1000, [] {
     leftMouth.goUpTimed(5000);
     Serial.println("Go up (5s)");
   } },
  { 2300, [] {
     centralMouth.goUpMax();
   } },
  { 6010, [] {
     leftMouth.goDownTimed(1000);
     Serial.println("Go down (1s)");
   } },
  { 7020, [] {
     leftMouth.goDownMax();
     Serial.println("It should go down for (4s)");
   } },
  { 11030, [] {
     leftMouth.goUpMax();
     Serial.println("It should go up to Max (10s)");
   } },
  { 21040, [] {
     showActive = false;
   } }
};

const int numOfEventsInTimeline = sizeof(timeline) / sizeof(timeline[0]);

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
      Serial.println("Timeline Finished");
    }
  },
  &runner, false);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("\nTaskScheduler v4.0.3 starting...");
  runner.startNow();

  delay(1000);
  tTimeline.enable();
}

void loop() {
  runner.execute();

  if (!showActive) {
    if (!leftMouth.getActive()) {
      if (i % 2 == 0) {
        leftMouth.goUpMax();
      } else {
        leftMouth.goDownMax();
      }
      i++;
    }
  }
}