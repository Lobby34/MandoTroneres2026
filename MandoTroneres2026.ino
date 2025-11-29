#define _TASK_LTS_POINTER // Enables LTS for v4.0.3

// Library improts
#include <TaskScheduler.h>  //Task scheduler so we can call "asyinc" functions
#include <Conceptinetics.h> //For dmx comunication

// File imports
#include "./Classes/motorLineal.cpp" //Import Classes
#include "./enviroment_pins.h"       //Import Declaration of enviroment variables
#include "./enviroment_dmx.h"
#include "./Classes/motorPersiana.cpp"

// DMX Startup
DMX_Slave dmx_slave(DMX_SLAVE_CHANNELS);

// Variable Delaration
Scheduler runner;           // Global – used by motorLineal
boolean showActive = false; // Global - used to know if show is active

  bool hewo = true;

// Object declaration
MotorLineal leftMouth(MOTOR_1_A_PIN, MOTOR_1_B_PIN, MOTOR_1_MAX_MILLIS_OPENED);
MotorLineal centralMouth(MOTOR_3_A_PIN, MOTOR_3_B_PIN, MOTOR_3_MAX_MILLIS_OPENED);
MotorLineal rightMouth(MOTOR_4_A_PIN, MOTOR_4_B_PIN, MOTOR_4_MAX_MILLIS_OPENED);
MotorLineal leftHead(MOTOR_2_A_PIN,MOTOR_2_B_PIN,MOTOR_2_MAX_MILLIS_OPENED);
MotorLineal rightHead(MOTOR_5_A_PIN,MOTOR_5_B_PIN,MOTOR_5_MAX_MILLIS_OPENED);

MotorPersiana centralHead(BLIND_1_A_PIN, BLIND_1_B_PIN, BLIND_1_C_PIN);

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
    static int index = 0;
    unsigned long now = millis();

    while (index < numOfEventsInTimeline && timeline[index].timeMs <= now) {
      timeline[index].action();
      index++;
    }

    if (index >= numOfEventsInTimeline) {
      tTimeline.disable();
    } },
    &runner, false);

void checkMovementCommand(MotorLineal &motor, int manualChannel, int manualFixedChannel, int autoChannel)
{
  // MANUAL MOVEMENT
  switch (dmx_slave.getChannelValue(manualChannel))
  {
  // STAY STILL
  case 0 ... 127:
    motor.stopMovement();
    break;

  // GO DOWN
  case 128 ... 191:
    motor.startDownMovement();
    break;

  // GO UP
  case 192 ... 255:
    motor.startUpMovement();
    break;

  default:
    motor.stopMovement();
    break;
  }

  // MANUAL FIXED && AUTO MOVEMENT
  if (dmx_slave.getChannelValue(manualChannel) < 128)
  {
    switch (dmx_slave.getChannelValue(manualFixedChannel))
    {
    case 0 ... 127:
      break;
    case 128 ... 191:
      motor.goUpMax();
      break;
    case 192 ... 255:
      motor.goDownMax();
    default:
      break;
    }
  }
  else if (dmx_slave.getChannelValue(manualChannel) < 128 && dmx_slave.getChannelValue(manualFixedChannel) < 128 && dmx_slave.getChannelValue(autoChannel) > 128)
  {
    // HERE GOES THE AUTO TASK
  }
}

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

  if (!showActive)
  {
    // SHOW
    if (dmx_slave.getChannelValue(DMX_SHOW_CHANNEL) > 192)
    {
      tTimeline.enable();
    }
    // PRESHOW
    else if (dmx_slave.getChannelValue(DMX_PRESHOW_CHANNEL) > 192)
    {
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
        rightHead.setMillisOpened(0);
      }

      //CHECK ALL MOVEMENT COMMANDS OF ALL MOTORS
      checkMovementCommand(leftMouth, DMX_LEFTMOUTH_MANUAL_CHANNEL,DMX_LEFTMOUTH_MANUALFIXED_CHANNEL,DMX_LEFTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(centralMouth, DMX_CENTRALMOUTH_MANUAL_CHANNEL,DMX_CENTRALMOUTH_MANUALFIXED_CHANNEL,DMX_CENTRALMOUTH_AUTO_CHANNEL);
      checkMovementCommand(rightMouth, DMX_RIGHTMOUTH_MANUAL_CHANNEL,DMX_RIGHTMOUTH_MANUALFIXED_CHANNEL,DMX_RIGHTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(leftHead,DMX_LEFTHEAD_MANUAL_CHANNEL,DMX_LEFTHEAD_MANUALFIXED_CHANNEL,DMX_LEFTHEAD_AUTO_CHANNEL);
      checkMovementCommand(rightHead,DMX_RIGHTHEAD_MANUAL_CHANNEL,DMX_RIGHTHEAD_MANUALFIXED_CHANNEL,DMX_RIGHTHEAD_AUTO_CHANNEL);
    }
  }
}