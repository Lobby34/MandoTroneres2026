// Motor.cpp
#include <TaskScheduler.h>

// Global scheduler (extern from main.ino)
extern Scheduler runner;

class MotorLineal
{
private:
    uint8_t pinUp;
    uint8_t pinDown;
    int maxMillisOpened;
    int millisOpened = 0;
    bool active = false;

    Task tStop;
    Task tAuto;

    // Static callback – retrieves 'this' via LTS
    static void stopCallback()
    {
        MotorLineal *self = static_cast<MotorLineal *>(runner.currentTask().getLtsPointer());
        if (self)
        {
            self->stopMovement();
        }
    }

    static void autoCallback()
    {
        MotorLineal *self = static_cast<MotorLineal *>(runner.currentTask().getLtsPointer());
        if (self)
        {
            self->goDownMax();
        }
    }

public:
    // CONSTRUCTOR
    MotorLineal(uint8_t up, uint8_t down, int maxTime)
        : pinUp(up), pinDown(down), maxMillisOpened(maxTime),
          tStop(0, TASK_ONCE, &MotorLineal::stopCallback, &runner, false),
          tAuto(0, TASK_ONCE, &MotorLineal::autoCallback, &runner, false)
    {
        pinMode(pinUp, OUTPUT);
        pinMode(pinDown, OUTPUT);
        digitalWrite(pinUp, LOW);
        digitalWrite(pinDown, LOW);

        // Store 'this' pointer via LTS (enabled by the #define)
        tStop.setLtsPointer(static_cast<void *>(this));
        tAuto.setLtsPointer(static_cast<void *>(this));

        runner.addTask(tStop);
        runner.addTask(tAuto);
    }

    void goUpTimed(int ms)
    {
        if (active)
        {
            // Serial.println("Cant go down. Movement active.");
            return;
        };
        if ((millisOpened + ms) > maxMillisOpened)
        {
            // Serial.println("Cant go up specified time. Lower Ceiling Reached. Going down the maximum available.");
            ms = maxMillisOpened - millisOpened;
        };
        digitalWrite(pinDown, LOW);
        digitalWrite(pinUp, HIGH);
        active = true;
        millisOpened += ms;

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
        if ((millisOpened - ms) < 0)
        {
            // Serial.println("Cant go down specified time. Lower Ceiling Reached. Going down the maximum available.");
            ms = millisOpened;
        };
        digitalWrite(pinUp, LOW);
        digitalWrite(pinDown, HIGH);
        active = true;
        millisOpened -= ms;
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
        int remaining = maxMillisOpened - millisOpened;
        // Serial.println(String("Going up max distance. The current positon is: ") + millisOpened + " the max is: " + maxMillisOpened + " And we will move: " + remaining);
        if (remaining > 0)
            goUpTimed(static_cast<unsigned long>(remaining));
    }

    void goDownMax()
    {
        if (active)
        {
            // Serial.println("Cant go max down. Movement active.");
            return;
        };
        int toClose = millisOpened;
        // Serial.println(String("Going down max distance. The current positon is: ") + millisOpened + " the min is: " + 0 + " And we will move: " + toClose);
        if (toClose > 0)
            goDownTimed(static_cast<unsigned long>(toClose));
    }

    void stopMovement()
    {
        if (active)
        {
            digitalWrite(pinUp, LOW);
            digitalWrite(pinDown, LOW);
            active = false;
            // Serial.println("Movement stopped");
        }
    }

    void startAutoMovement()
    {
        if(active)
        {
           return; 
        }
        goUpMax();
        tAuto.setInterval(maxMillisOpened - millisOpened);
        tAuto.restartDelayed();
    }

    void startUpMovement()
    {
        if (!active)
        {
            digitalWrite(pinDown, LOW);
            digitalWrite(pinUp, HIGH);
            active = true;
        }
    }

    void startDownMovement()
    {
        if (!active)
        {
            digitalWrite(pinUp, LOW);
            digitalWrite(pinDown, HIGH);
            active = true;
        }
    }

    boolean getActive()
    {
        return active;
    }

    void setMillisOpened(int millis) {
        millisOpened = millis;
    }

    int getMaxMillisOpened() {
        return maxMillisOpened;
    }
};