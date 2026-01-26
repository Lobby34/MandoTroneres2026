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
Scheduler runner;                // Global – used by motorLineal
boolean showActive = false;      // Global - used to know if show is active
int timelineIndex = 0;           // Global - to know the current position in the timeline
unsigned long showStartTime = 0; // Global - to know when the show started

// Selected Show Timeline Event
struct TimelineEvent
{
  unsigned long timeMs;
  void (*action)();
};

TimelineEvent *currentTimeline = nullptr; // Pointer to current timeline
int currentTimelineLength = 0;            // Length of current timeline

// Object declaration
MotorLineal leftMouth(MOTOR_1_A_PIN, MOTOR_1_B_PIN, MOTOR_1_MAX_MILLIS_OPENED);
MotorLineal centralMouth(MOTOR_3_A_PIN, MOTOR_3_B_PIN, MOTOR_3_MAX_MILLIS_OPENED);
MotorLineal rightMouth(MOTOR_5_A_PIN, MOTOR_5_B_PIN, MOTOR_5_MAX_MILLIS_OPENED);
MotorLineal leftHead(MOTOR_2_A_PIN, MOTOR_2_B_PIN, MOTOR_2_MAX_MILLIS_OPENED);
MotorLineal centralHead(MOTOR_4_A_PIN, MOTOR_4_B_PIN, MOTOR_4_MAX_MILLIS_OPENED);
MotorLineal rightHead(MOTOR_6_A_PIN, MOTOR_6_B_PIN, MOTOR_6_MAX_MILLIS_OPENED);

// Show Timeline
TimelineEvent mainShow[] = {
    {0, []
     { showActive = true; }},
    {145000, []
     {
       centralHead.goDownMax();
     }},
    {182000, []
     {
       centralHead.goUpMax();
       centralMouth.goUpMax();
     }},
    {186500, []
     {
       leftHead.goDownMax();
       leftMouth.goUpMax();
     }},
    {191000, []
     {
       rightHead.goDownMax();
       rightMouth.goUpMax();
     }},
    {191510, []
     { leftMouth.goDownMax(); }},
    {195000, []
     {leftHead.goUpMax(); rightHead.goUpMax(); }},
    {196010, []
     { rightMouth.goDownMax(); }},
    {196520, []
     { leftMouth.goUpMax(); }},
    {200010, []
     {leftHead.goDownMax(); rightHead.goDownMax(); }},
    {201020, []
     { rightMouth.goUpMax(); }},
    {201530, []
     { leftMouth.goDownMax(); }},
    {205020, []
     {leftHead.goUpTimed(2000); rightHead.goUpTimed(2000); }},
    {206030, []
     { rightMouth.goDownTimed(970); }},
    {207000, [] { /*CHECKPOINT: STOP ALL MOVEMENT*/ }},
    {207500, []{leftMouth.startDownMovement(); centralMouth.startDownMovement(); rightMouth.startDownMovement(); }},
    {210000, []{leftMouth.stopMovement(); centralMouth.stopMovement(); rightMouth.stopMovement(); }},
    {211000, []{leftMouth.goUpMax();}},
    {212000, []{rightMouth.goUpMax();}},
    {215000, []{centralMouth.goUpMax();}},
    {216010, []{leftMouth.goDownMax();}},
    {217010, []{rightMouth.goDownMax();}},
    {220010, []{centralMouth.goDownMax();}},
    {221020, []{leftMouth.goUpMax();}},
    {222020, []{rightMouth.goUpMax();}},
    {225020, []{centralMouth.goUpMax();}},
    {226030, []{leftMouth.goDownMax();}},
    {227030, []{rightMouth.goDownMax();}},
    {230030, []{centralMouth.goDownMax();}},
    {231040, []{leftMouth.goUpMax();}},
    {232040, []{rightMouth.goUpMax();}},
    {235040, []{centralMouth.goUpMax();}},
    {236050, []{leftMouth.goDownMax();}},
    {237050, []{rightMouth.goDownMax();}},
    {240050, []{centralMouth.goDownMax();}},
    {241060, []{leftMouth.goUpMax();}},
    {242060, []{rightMouth.goUpMax();}},
    {245060, []{centralMouth.goUpMax();}},
    {246070, []{leftMouth.goDownMax();}},
    {247070, []{rightMouth.goDownMax();}},
    {250070, []{centralMouth.goDownMax();}},
    {251080, []{leftMouth.goUpMax();}},
    {252080, []{rightMouth.goUpMax();}},
    {255080, []{centralMouth.goUpMax();}},
    {256090, []{leftMouth.goDownMax();}},
    {257090, []{rightMouth.goDownMax();}},
    {260090, []{centralMouth.goDownMax();}},
    {261100, []{leftMouth.goUpMax();}},
    {262100, []{rightMouth.goUpMax();}},
    {265100, []{centralMouth.goUpMax();}},
    {266110, []{leftMouth.goDownMax();}},
    {267110, []{rightMouth.goDownMax();}},
    {272500, []{
      leftMouth.startDownMovement(); rightMouth.startDownMovement(); 
    }},
    {274900, []{
      leftMouth.stopMovement(); rightMouth.stopMovement(); 
    }},
    {275000, []{
      leftHead.goUpMax(); rightMouth.goUpTimed(3000);
    }},
    {278000, []{
      leftHead.goDownTimed(3000); rightMouth.goDownMax();
    }},
    {282000, []{
      rightHead.goUpMax(); leftMouth.goUpTimed(3000);
    }},
    {285000, []{
      rightHead.goDownTimed(3000); leftMouth.goDownMax();
    }},
    {289000, []{
      leftMouth.goUpMax(); rightMouth.goUpMax(); centralMouth.goDownMax();
    }},
    {303500, []{
      leftHead.goUpTimed(1500), rightHead.goUpTimed(1500); centralHead.goDownTimed(1500);
    }},
    {305000, []{
      leftHead.goUpTimed(1500), rightHead.goUpTimed(1500); centralHead.goUpTimed(1500);
    }},
    {307000, []{
      leftMouth.goDownMax(); rightMouth.goDownMax(); centralMouth.goUpMax();
    }},
    {311000, []{
      leftHead.goDownTimed(3500); rightHead.goDownTimed(3500); centralHead.goDownMax(); leftMouth.goUpMax(); rightMouth.goUpMax(); centralMouth.goDownMax();
    }},
    {314510, []{
      leftHead.goUpMax(); rightHead.goUpMax();
    }},
    {316010, []{
      leftMouth.goDownMax(); rightMouth.goDownMax(); centralMouth.goUpMax();
    }},
    {318010, []{
      centralHead.goUpMax();
    }},
    {318020, []{
      leftHead.goDownTimed(3500); rightHead.goDownTimed(3500);
    }},
    {321020, []{
      leftMouth.goUpMax(); rightMouth.goUpMax(); centralMouth.goDownMax();
    }},
    {321530, []{
      leftHead.goUpMax(); rightHead.goUpMax();
    }},
    {325040, []{
      leftHead.goDownTimed(3500); rightHead.goDownTimed(3500);
    }},
    {326030, []{
      leftMouth.goDownMax(); rightMouth.goDownMax(); centralMouth.goUpMax();
    }},
    {328020, []{
      centralHead.goDownMax();
    }},
    {328550, []{
      leftHead.goUpMax(); rightHead.goUpMax();
    }},
    {331050, []{
      leftMouth.goUpMax(); rightMouth.goUpMax(); centralMouth.goDownMax();
    }},
    {332060, []{
      leftHead.goDownTimed(3500); rightHead.goDownTimed(3500);
    }},
    {335070, []{
      leftHead.goUpMax(); rightHead.goUpMax();
    }},
    {336060, []{
      leftMouth.goDownTimed(2940); rightMouth.goDownTimed(2940); centralMouth.startDownMovement();
    }},
    {338030, []{
      centralHead.goUpMax();
    }},
    {338580, []{
      leftHead.goDownTimed(1000); rightHead.goDownTimed(1000);
    }},
    {339000, []{
      centralMouth.stopMovement(); /*STOOOOP*/
    }},
    {339590, []{leftHead.goUpMax(); rightHead.goUpMax();}},
    {341000, []{leftHead.goDownTimed(3500); leftMouth.goUpMax();}},
    {344510, []{leftHead.goUpMax();}},
    {346010, []{leftMouth.goDownMax();}},
    {347000, []{rightHead.goDownTimed(3500); rightMouth.goUpMax();}},
    {348020, []{leftHead.goDownTimed(3500);}},
    {348040, []{centralHead.goDownMax(); centralMouth.goUpMax();}},
    {350510, []{rightHead.goUpMax();}},
    {351020, []{leftMouth.goUpMax();}},
    {351530, []{leftHead.goUpMax();}},
    {352010, []{rightMouth.goDownMax();}},
    {353010, []{centralMouth.goDownMax();}},
    {354020, []{rightHead.goDownTimed(3500);}},
    {355040, []{leftHead.goDownTimed(3500);}},
    {356030, []{leftMouth.goDownMax();}},
    {357020, []{rightMouth.goUpMax();}},
    {357530, []{rightHead.goUpMax();}},
    {358020, []{centralMouth.goUpMax();}},
    {358050, []{centralHead.goUpMax();}},
    {358550, []{leftHead.goUpMax();}},
    {361040, []{rightHead.goDownTimed(3500); leftMouth.goUpMax();}},
    {362030, []{rightMouth.goDownMax();}},
    {362060, []{leftHead.goDownTimed(3500);}},
    {363030, []{centralMouth.goDownMax();}},
    {364550, []{rightHead.goUpMax();}},
    {365570, []{leftHead.goUpMax();}},
    {366050, []{leftMouth.goDownMax();}},
    {367040, []{rightMouth.goUpMax();}},
    {368040, []{centralMouth.goUpMax();}},
    {368060, []{centralHead.startUpMovement();}},
    {369080, []{leftHead.goDownMax(); rightHead.goDownMax();}},
    {370000, []{centralHead.stopMovement();}},
    {371050, []{rightMouth.goDownMax();}},
    {371060, []{leftMouth.startDownMovement();}},
    {373050, []{centralMouth.goDownMax();}}
    {375000, []{leftHead.startDownMovement(); rightHead.startDownMovement(); leftMouth.stopMovement();}},
    {377500, []{rightMouth.startDownMovement();}},
    {378000, []{leftHead.stopMovement(); rightHead.stopMovement();}},
    {378500, []{centralMouth.startDownMovement();}},
    {379000, []{rightMouth.stopMovement();}},
    {380000, []{centralMouth.stopMovement();}},
    {382000, []
     { showActive = false; }}
};

TimelineEvent exampleShow[] = {
    {0, []
     {showActive = true; leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax(); leftHead.goUpMax(); rightHead.goUpMax(); }},
    {5000, []
     {leftMouth.goDownMax(); centralMouth.goDownMax(); rightMouth.goDownMax(); }},
    {10000, []
     {leftMouth.goUpMax(); rightMouth.goUpMax(); }},
    {13000, []
     { centralMouth.goUpMax(); }},
    {15000, []
     { leftMouth.goDownMax(); }},
    {18000, []
     { rightMouth.goDownMax(); }},
    {19000, []
     { centralMouth.goDownMax(); }},
    {25000, []
     {leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax(); }},
    {30000, []
     {leftMouth.goDownMax(); centralMouth.goDownMax(); rightMouth.goDownMax(); }},
    {35000, []
     {centralMouth.goUpMax(); rightMouth.goUpMax(); }},
    {38000, []
     { leftMouth.goUpMax(); }},
    {40000, []
     { centralMouth.goDownMax(); }},
    {48000, []
     { rightMouth.goDownMax(); }},
    {49000, []
     { leftMouth.goDownMax(); }},
    {25000, []
     {leftMouth.goUpMax(); centralMouth.goUpMax(); rightMouth.goUpMax(); }},
    {55000, []
     {rightMouth.goDownMax(); leftMouth.goDownMax(); centralMouth.goDownMax(); }},
    {60000, []
     {leftMouth.goUpMax(); centralMouth.goUpMax(); }},
    {63000, []
     { rightMouth.goUpMax(); }},
    {65000, []
     { leftMouth.goDownMax(); }},
    {72000, []
     { centralMouth.goDownMax(); }},
    {73000, []
     { rightMouth.goDownMax(); }},
    {80000, []
     {leftMouth.startDownMovement(); centralMouth.startDownMovement(); rightMouth.startDownMovement(); }},
    {85000, []
     {leftMouth.stopMovement(); centralMouth.stopMovement(); rightMouth.stopMovement(); }},
    {160000, []
     { showActive = false; }}};

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

void startTimeline(TimelineEvent *timelineArray, int length)
{
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

      // CHECK ALL MOVEMENT COMMANDS OF ALL MOTORS
      checkMovementCommand(dmx_slave, leftMouth, DMX_LEFTMOUTH_MANUAL_CHANNEL, DMX_LEFTMOUTH_MANUALFIXED_CHANNEL, DMX_LEFTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, centralMouth, DMX_CENTRALMOUTH_MANUAL_CHANNEL, DMX_CENTRALMOUTH_MANUALFIXED_CHANNEL, DMX_CENTRALMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, rightMouth, DMX_RIGHTMOUTH_MANUAL_CHANNEL, DMX_RIGHTMOUTH_MANUALFIXED_CHANNEL, DMX_RIGHTMOUTH_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, leftHead, DMX_LEFTHEAD_MANUAL_CHANNEL, DMX_LEFTHEAD_MANUALFIXED_CHANNEL, DMX_LEFTHEAD_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, centralHead, DMX_CENTRALHEAD_MANUAL_CHANNEL, DMX_CENTRALHEAD_MANUALFIXED_CHANNEL, DMX_CENTRALHEAD_AUTO_CHANNEL);
      checkMovementCommand(dmx_slave, rightHead, DMX_RIGHTHEAD_MANUAL_CHANNEL, DMX_RIGHTHEAD_MANUALFIXED_CHANNEL, DMX_RIGHTHEAD_AUTO_CHANNEL);
    }
  }
}