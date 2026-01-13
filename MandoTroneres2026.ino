#define _TASK_LTS_POINTER // Enables LTS for v4.0.3

// Library improts
#include <TaskScheduler.h>  //Task scheduler so we can call "asyinc" functions
#include <Conceptinetics.h> //For dmx comunication

// File imports
#include "./Classes/motorLineal.cpp" //Import Classes
#include "./enviroment_pins.h"       //Import Declaration of enviroment variables
#include "./enviroment_dmx.h"
#include "./Classes/motorPersiana.cpp"
#include "./Controllers/movementController.cpp"
// #include "./Controllers/taskController.h"
// #include "./Controllers/taskController.cpp"

// DMX Startup
DMX_Slave dmx_slave(DMX_SLAVE_CHANNELS);

// Variable Delaration
Scheduler runner;           // Global – used by motorLineal
boolean showActive = false; // Global - used to know if show is active
int timelineIndex = 0;    // Global - to know the current position in the timeline
unsigned long showStartTime = 0; // Global - to know when the show started

// Object declaration
MotorLineal leftMouth(MOTOR_1_A_PIN, MOTOR_1_B_PIN, MOTOR_1_MAX_MILLIS_OPENED);
MotorLineal centralMouth(MOTOR_3_A_PIN, MOTOR_3_B_PIN, MOTOR_3_MAX_MILLIS_OPENED);
MotorLineal rightMouth(MOTOR_4_A_PIN, MOTOR_4_B_PIN, MOTOR_4_MAX_MILLIS_OPENED);
MotorLineal leftHead(MOTOR_2_A_PIN,MOTOR_2_B_PIN,MOTOR_2_MAX_MILLIS_OPENED);
MotorLineal centralHead(MOTOR_6_A_PIN,MOTOR_6_B_PIN,MOTOR_6_MAX_MILLIS_OPENED);
MotorLineal rightHead(MOTOR_5_A_PIN,MOTOR_5_B_PIN,MOTOR_5_MAX_MILLIS_OPENED);

// Show "Class"
using TimelineEvent = struct
{
  unsigned long timeMs;
  void (*action)();
};

// Show Timeline
TimelineEvent timeline[] = {
    {0, []
     {
       showActive = true;
     }},
    {1000, []
     {
       leftMouth.goUpTimed(5000);
     }},
    {2300, []
     {
       centralMouth.goUpMax();
     }},
    {6010, []
     {
       leftMouth.goDownTimed(1000);
     }},
    {7020, []
     {
       leftMouth.goDownMax();
     }},
    {11030, []
     {
       leftMouth.goUpMax();
     }},
    {21040, []
     {
       showActive = false;
     }}};

// Calculate the space each position of the array takes
const int numOfEventsInTimeline = sizeof(timeline) / sizeof(timeline[0]);

// Task declaration to run the showTimeline
Task tTimeline(
    10, -1, []
    {
    unsigned long elapsed = millis() - showStartTime;

    while (timelineIndex < numOfEventsInTimeline && timeline[timelineIndex].timeMs <= elapsed) {
      timeline[timelineIndex].action();
      timelineIndex++;
    }

    if (timelineIndex >= numOfEventsInTimeline) {
      tTimeline.disable();
    } },
    &runner, false);

void setup()
{
  // DMX SETUP
  dmx_slave.enable();
  dmx_slave.setStartAddress(DMX_SLAVE_ADDRESS);

  // Inicialize task Scheduler
  runner.startNow();


}

void loop()
{
  // Task scheduler "move one step"
  runner.execute();

  if (!tTimeline.isEnabled())
  {
    // SHOW
    if (dmx_slave.getChannelValue(DMX_SHOW_CHANNEL) > 192)
    {
      //Reset timeline to start again
      timelineIndex = 0;

      //Mark current time as start time
      showStartTime = millis();

      //Start show by enabling timeline task
      showActive = true;
      tTimeline.enable();
    }
    // PRESHOW
    else if (dmx_slave.getChannelValue(DMX_PRESHOW_CHANNEL) > 192)
    {
      leftMouth.goDownMax();
      leftHead.goDownMax();
      centralMouth.goDownMax();
      centralHead.goDownMax();
      rightMouth.goDownMax();
      rightHead.goDownMax();

    }
    // EXAMPLE SHOW
    else if (dmx_slave.getChannelValue(DMX_EXAMPLESHOW_CHANNEL) > 192)
    {
      
    }
    else
    {
      // SET CURRENT POSITION AS HOME
      if (dmx_slave.getChannelValue(DMX_SETHOMEPOSITIONS_CHANNEL) > 192)
      {
        leftMouth.setMillisOpened(0);
        centralMouth.setMillisOpened(0);
        rightMouth.setMillisOpened(0);
        leftHead.setMillisOpened(0);
        centralHead.setMillisOpened(0);
        rightHead.setMillisOpened(0);
      }

      //CHECK ALL MOVEMENT COMMANDS OF ALL MOTORS
      checkMovementCommand(dmx_slave, leftMouth, DMX_LEFTMOUTH_MANUAL_CHANNEL,DMX_LEFTMOUTH_MANUALFIXED_CHANNEL,DMX_LEFTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, centralMouth, DMX_CENTRALMOUTH_MANUAL_CHANNEL,DMX_CENTRALMOUTH_MANUALFIXED_CHANNEL,DMX_CENTRALMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, rightMouth, DMX_RIGHTMOUTH_MANUAL_CHANNEL,DMX_RIGHTMOUTH_MANUALFIXED_CHANNEL,DMX_RIGHTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, leftHead,DMX_LEFTHEAD_MANUAL_CHANNEL,DMX_LEFTHEAD_MANUALFIXED_CHANNEL,DMX_LEFTHEAD_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, centralHead,DMX_CENTRALHEAD_MANUAL_CHANNEL,DMX_CENTRALHEAD_MANUALFIXED_CHANNEL,DMX_CENTRALHEAD_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, rightHead,DMX_RIGHTHEAD_MANUAL_CHANNEL,DMX_RIGHTHEAD_MANUALFIXED_CHANNEL,DMX_RIGHTHEAD_AUTO_CHANNEL);
    }
  }
}