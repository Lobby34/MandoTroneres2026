#include <Conceptinetics.h>

void checkMovementCommand(DMX_Slave &dmx_slave, MotorLineal &motor, int manualChannel, int manualFixedChannel, int autoChannel)
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