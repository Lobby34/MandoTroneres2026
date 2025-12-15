#include <Conceptinetics.h>

void checkMovementCommand(DMX_Slave &dmx_slave, MotorLineal &motor, int manualChannel, int manualFixedChannel, int autoChannel)
{
  int m = dmx_slave.getChannelValue(manualChannel);
  int mf = dmx_slave.getChannelValue(manualFixedChannel);
  int a = dmx_slave.getChannelValue(autoChannel);

  // ----- 1. MANUAL MOVEMENT -----
  const int DEAD_LOW = 181;  // dead zone lower bound
  const int DEAD_HIGH = 199; // dead zone upper bound

  if (m >= 128 && m <= 180)
  {
    motor.startDownMovement();
    return; // manual overrides everything else
  }
  else if (m >= 200 && m <= 255)
  {
    motor.startUpMovement();
    return; // manual overrides everything else
  }
  else if (m >= DEAD_LOW && m <= DEAD_HIGH)
  {
    motor.stopMovement(); // dead zone, do nothing
    return;
  }
  // else m < 128 → neutral, continue to next step

  // ----- 2. MANUAL FIXED -----
  if (mf >= 128 && mf <= 180)
  {
    motor.goDownMax();
    return; // manual-fixed down
  }
  else if (mf >= 200 && mf <= 255)
  {
    motor.goUpMax();
    return; // manual-fixed up
  }
  else if (mf >= DEAD_LOW && mf <= DEAD_HIGH)
  {
    return;
  }

  // ----- 3. AUTO MODE -----
  if (a > 128)
  {
    motor.startAutoMovement();
    return;
  }

  // ----- 4. DEFAULT -----
  motor.stopMovement();
}