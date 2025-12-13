// Motor.cpp
#include <TaskScheduler.h>

// Global scheduler (extern from main.ino)
extern Scheduler runner;

class MotorPersiana
{
private:
    uint8_t pinUp;
    uint8_t pinDown;
    uint8_t pinStop;
    bool isUp = false;
    bool active = false;

    Task tStop;
    Task tStopPress;

    // Static callback – retrieves 'this' via LTS
    static void stopCallback()
    {
        MotorPersiana *self = static_cast<MotorPersiana *>(runner.currentTask().getLtsPointer());
        if (self)
        {
            self->stopMovement();
        }
    }

    static void stopPressCallBack()
    {
        MotorPersiana *self = static_cast<MotorPersiana *>(runner.currentTask().getLtsPointer());
        if (self)
        {
            self->stopPress();
        }
    }

public:
    // CONSTRUCTOR
    MotorPersiana(uint8_t up, uint8_t down, uint8_t stop)
        : pinUp(up), pinDown(down), pinStop(stop),
          tStop(0, TASK_ONCE, &MotorPersiana::stopCallback, &runner, false),
          tStopPress(0, TASK_ONCE, &MotorPersiana::stopPressCallBack, &runner, false)
    {
        pinMode(pinUp, OUTPUT);
        pinMode(pinDown, OUTPUT);
        pinMode(pinStop, OUTPUT);
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinDown, HIGH);
        digitalWrite(pinStop, HIGH);

        // Store 'this' pointer via LTS (enabled by the #define)
        tStop.setLtsPointer(static_cast<void *>(this));
        tStopPress.setLtsPointer(static_cast<void *>(this));

        runner.addTask(tStop);
        runner.addTask(tStopPress);
    }

    void goUpTimed(int ms)
    {
        if (active)
        {
            // Serial.println("Cant go down. Movement active.");
            return;
        };
        digitalWrite(pinDown, HIGH);
        digitalWrite(pinUp, LOW);
        active = true;
        tStopPress.setInterval(100);
        tStopPress.restartDelayed();

        tStop.setInterval(ms);
        tStop.restartDelayed();
    }

    void goDownTimed(int ms)
    {
        if (active)
        {
            // Serial.println("Cant go down. Movement active.");
            return;
        };
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinStop, HIGH);
        digitalWrite(pinDown, LOW);
        active = true;
        tStopPress.setInterval(100);
        tStopPress.restartDelayed();

        tStop.setInterval(ms);
        tStop.restartDelayed();
    }

    void goUpMax()
    {
        if (active)
        {
            // Serial.println("Cant go max up. Movement active.");
            return;
        };
        digitalWrite(pinDown, HIGH);
        digitalWrite(pinStop, HIGH);
        digitalWrite(pinUp, LOW);
        active = true;

        tStopPress.setInterval(100);
        tStopPress.restartDelayed();
    }

    void goDownMax()
    {
        if (active)
        {
            // Serial.println("Cant go max down. Movement active.");
            return;
        };
        tStopPress.setInterval(100);
        tStopPress.restartDelayed();
        active = true;
    }

    void stopMovement()
    {
        digitalWrite(pinStop, LOW);

        tStopPress.setInterval(100);
        tStopPress.restartDelayed();
    }

    void stopPress()
    {
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinDown, HIGH);
        digitalWrite(pinStop, HIGH);
        active = false;
    }

    void startUpMovement()
    {
        if (!active)
        {
            digitalWrite(pinDown, HIGH);
            digitalWrite(pinUp, LOW);
            active = true;
            tStopPress.setInterval(100);
            tStopPress.restartDelayed();
        }
    }

    void startDownMovement()
    {
        if (!active)
        {
            digitalWrite(pinUp, HIGH);
            digitalWrite(pinDown, LOW);
            active = true;
            tStopPress.setInterval(100);
            tStopPress.restartDelayed();
        }
    }

    boolean getActive()
    {
        return active;
    }
};