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

// Selected Show Timeline Event
struct TimelineEvent {
  unsigned long timeMs;
  void (*action)();
};

TimelineEvent* currentTimeline = nullptr; //Pointer to current timeline
int currentTimelineLength = 0;        //Length of current timeline

// Object declaration
MotorLineal leftMouth(MOTOR_1_A_PIN, MOTOR_1_B_PIN, MOTOR_1_MAX_MILLIS_OPENED);
MotorLineal centralMouth(MOTOR_3_A_PIN, MOTOR_3_B_PIN, MOTOR_3_MAX_MILLIS_OPENED);
MotorLineal rightMouth(MOTOR_5_A_PIN, MOTOR_5_B_PIN, MOTOR_5_MAX_MILLIS_OPENED);
MotorLineal leftHead(MOTOR_2_A_PIN,MOTOR_2_B_PIN,MOTOR_2_MAX_MILLIS_OPENED);
MotorLineal centralHead(MOTOR_4_A_PIN,MOTOR_4_B_PIN,MOTOR_4_MAX_MILLIS_OPENED);
MotorLineal rightHead(MOTOR_6_A_PIN,MOTOR_6_B_PIN,MOTOR_6_MAX_MILLIS_OPENED);

// Show Timeline
TimelineEvent mainShow[] = {
    {0, []{showActive = true;}},
    {145000, []
     {
       centralHead.goDownMax();
     }},
     {182000, [] 
      {
        centralHead.goUpMax();
        centralMouth.goUpMax();
      }},
      {187000, []
       {
         centralMouth.goDownMax();
         leftHead.goDownMax();
         leftMouth.goUpMax();
       }},
       {191000, [] {
          rightHead.goDownMax();
          rightMouth.goUpMax();
       }},
       {192000,[]{
          centralMouth.goUpMax();
          leftMouth.goDownMax();
          leftHead.goUpMax();
       }},
       {196000,[]{
          rightHead.goUpMax();
          rightMouth.goDownMax();

       }},
       {197000,[]{
          centralMouth.goDownMax();
          leftMouth.goUpMax();
          leftHead.goDownMax();
       }},
       {201000,[]{
        rightMouth.goUpMax();
        rightHead.goDownMax();
       }},
       {202000,[]{
          centralMouth.goUpMax();
          leftMouth.goDownMax();
          leftHead.goUpMax();
       }},
       {206000,[]{
        rightMouth.goDownTimed(2000);
        rightHead.goUpTimed(2000);
       }},
       {207000,[]{
          centralMouth.goDownMax();
          leftMouth.goUpMax();
          leftHead.goDownMax();
       }},
       {209000,[]{
        rightMouth.goUpMax();
        rightHead.goDownMax();
       }},
       {210000,[]{centralMouth.goUpMax();}},
       {212000,[]{rightMouth.goDownMax();}},
       {214000,[]{leftMouth.goDownMax();}},
       {216000,[]{centralMouth.goDownMax();}},
       {218000,[]{rightMouth.goUpMax();}},
       {220000,[]{leftMouth.goUpMax();}},
       {222000,[]{centralMouth.goUpMax();}},
       {224000,[]{rightMouth.goDownMax();}},
       {226000,[]{leftMouth.goDownMax();}},
       {228000,[]{centralMouth.goDownMax();}},
       {230000,[]{rightMouth.goUpMax();}},
       {232000,[]{leftMouth.goUpMax();}},
       {234000,[]{centralMouth.goUpMax();}},
       {236000,[]{rightMouth.goDownMax();}},
       {238000,[]{leftMouth.goDownMax();}},
       {240000,[]{centralMouth.goDownMax();}},
       {242000,[]{rightMouth.goUpMax();}},
       {244000,[]{leftMouth.goUpMax();}},
       {246000,[]{centralMouth.goUpMax();}},
       {248000,[]{rightMouth.goDownMax();}},
       {250000,[]{leftMouth.goDownMax();}},
       {252000,[]{centralMouth.goDownMax();}},
       {254000,[]{rightMouth.goUpMax();}},
       {256000,[]{leftMouth.goUpMax();}},
       {258000,[]{centralMouth.goUpMax();}},
       {260000,[]{rightMouth.goDownMax();}},
       {262000,[]{leftMouth.goDownMax();}},
       {264000,[]{centralMouth.goDownMax();}},
       {266000,[]{rightMouth.goUpMax();}},
       {275000,[]{
        rightHead.goUpMax();
        leftHead.goUpMax();
      }},
      {279000,[]{centralHead.goDownMax();}},
      {280000, []{showActive = false;}}
       
    };

TimelineEvent exampleShow[] = {
  {0, [] {showActive = true; leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax(); leftHead.goUpMax(); rightHead.goUpMax();}},
  {5000, [] {leftMouth.goDownMax(); centralMouth.goDownMax(); rightMouth.goDownMax();}},
  {10000, [] {leftMouth.goUpMax(); rightMouth.goUpMax();}},
  {13000, [] {centralMouth.goUpMax();}},
  {15000, [] {leftMouth.goDownMax();}},
  {18000, [] {rightMouth.goDownMax();}},
  {19000, [] {centralMouth.goDownMax();}},
  {25000, [] {leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax();}},
  {30000, [] {leftMouth.goDownMax(); centralMouth.goDownMax(); rightMouth.goDownMax();}},
  {35000, [] {centralMouth.goUpMax(); rightMouth.goUpMax();}},
  {38000, [] {leftMouth.goUpMax();}},
  {40000, [] {centralMouth.goDownMax();}},
  {48000, [] {rightMouth.goDownMax();}},
  {49000, [] {leftMouth.goDownMax();}},
  {25000, [] {leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax();}},
  {55000, [] {rightMouth.goDownMax(); leftMouth.goDownMax(); centralMouth.goDownMax();}},
  {60000, [] {leftMouth.goUpMax(); centralMouth.goUpMax();}},
  {63000, [] {rightMouth.goUpMax();}},
  {65000, [] {leftMouth.goDownMax();}},
  {72000, [] {centralMouth.goDownMax();}},
  {73000, [] {rightMouth.goDownMax();}},
  {80000, [] {leftMouth.startDownMovement(); centralMouth.startDownMovement(); rightMouth.startDownMovement();}},
  {85000, [] {leftMouth.stopMovement(); centralMouth.stopMovement(); rightMouth.stopMovement();}},
  {160000, [] {showActive = false;}}
  };

// Task declaration to run the showTimeline
Task tTimeline(
    10, -1, []
    {
    unsigned long elapsed = millis() - showStartTime;

    while (timelineIndex < currentTimelineLength && currentTimeline[timelineIndex].timeMs <= elapsed) {
      currentTimeline[timelineIndex].action();
      timelineIndex++;
    }

    if (timelineIndex >= currentTimelineLength) {
      tTimeline.disable();
      showActive = false;
    } },
    &runner, false);

void startTimeline(TimelineEvent* timelineArray, int length) {
  currentTimeline = timelineArray;
  currentTimelineLength = length;
  timelineIndex = 0;
  showStartTime = millis();
  showActive = true;
  tTimeline.enable();
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

  if (!tTimeline.isEnabled())
  {
    // SHOW
    if (dmx_slave.getChannelValue(DMX_SHOW_CHANNEL) > 192)
    {
      startTimeline(mainShow, sizeof(mainShow) / sizeof(mainShow[0]));
    }
    // PRESHOW
    else if (dmx_slave.getChannelValue(DMX_PRESHOW_CHANNEL) > 192)
    {
      leftMouth.goDownMax();
      leftHead.goUpMax();
      centralMouth.goDownMax();
      centralHead.goDownMax();
      rightMouth.goDownMax();
      rightHead.goUpMax();

    }
    // EXAMPLE SHOW
    else if (dmx_slave.getChannelValue(DMX_EXAMPLESHOW_CHANNEL) > 192)
    {
      startTimeline(exampleShow, sizeof(exampleShow) / sizeof(exampleShow[0]));
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