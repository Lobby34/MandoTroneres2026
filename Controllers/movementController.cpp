#include <Conceptinetics.h>

void checkMovementCommand(DMX_Slave &dmx_slave, MotorLineal &motor, int manualChannel, int manualFixedChannel, int autoChannel)
{
  int m = dmx_slave.getChannelValue(manualChannel);
  int mf = dmx_slave.getChannelValue(manualFixedChannel);
  int a = dmx_slave.getChannelValue(autoChannel);

  // VARIABLES FOR MOVEMENT ZONES
  const int DEAD_LOW = 181;  // dead zone lower bound
  const int DEAD_HIGH = 199; // dead zone upper bound
  const int LOWER_BOUNDRY = 128;
  const int HIGHER_BOUNDRY = 255;

  // PRIORITY CHECKING
  if (m > LOWER_BOUNDRY)
  {

    // ----- 1. MANUAL MOVEMENT -----
    if (m >= LOWER_BOUNDRY && m < DEAD_LOW)
    {
      motor.startDownMovement();
      return; // manual overrides everything else
    }
    else if (m > DEAD_HIGH && m <= HIGHER_BOUNDRY)
    {
      motor.startUpMovement();
      return; // manual overrides everything else
    }
    else if (m >= DEAD_LOW && m <= DEAD_HIGH)
    {
      motor.stopMovement(); // dead zone, do nothing
      return;
    }
  }
  else if (mf > LOWER_BOUNDRY)
  {

    // ----- 2. MANUAL FIXED -----
    if (mf >= LOWER_BOUNDRY && mf < DEAD_LOW)
    {
      motor.goDownMax();
      return; // manual-fixed down
    }
    else if (mf > DEAD_HIGH && mf <= HIGHER_BOUNDRY)
    {
      motor.goUpMax();
      return; // manual-fixed up
    }
    else if (mf >= DEAD_LOW && mf <= DEAD_HIGH)
    {
      return;
    }
  }
  else
  {

    // ----- 3. AUTO MODE -----
    if (a > LOWER_BOUNDRY)
    {
      motor.startAutoMovement();
      return;
    }
  }

  // ----- 4. DEFAULT -----
  if (!motor.getLatched()) {
    motor.stopManualMovement();
  }
}