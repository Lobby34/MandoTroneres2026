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
    bool latched = false;
    bool doingAutoCycle = false;

    Task tStop;

    // Static callback – retrieves 'this' via LTS
    static void stopCallback()
    {
        MotorLineal *self = static_cast<MotorLineal *>(runner.currentTask().getLtsPointer());
        if (self)
        {
            self->stopMovement();
        }
    }

public:
    // CONSTRUCTOR
    MotorLineal(uint8_t up, uint8_t down, int maxTime)
        : pinUp(up), pinDown(down), maxMillisOpened(maxTime),
          tStop(0, TASK_ONCE, &MotorLineal::stopCallback, &runner, false)
    {
        pinMode(pinUp, OUTPUT);
        pinMode(pinDown, OUTPUT);
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinDown, HIGH);

        // Store 'this' pointer via LTS (enabled by the #define)
        tStop.setLtsPointer(static_cast<void *>(this));

        runner.addTask(tStop);
    }

    // MOTOR FUNCTIONS AND LOGIC
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
        digitalWrite(pinDown, HIGH);
        digitalWrite(pinUp, LOW);
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
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinDown, LOW);
        active = true;
        millisOpened -= ms;
        tStop.setInterval(ms);
        tStop.restartDelayed();
    }

    void goUpMax()
    {
        if (active)
        {
            return;
        };
        latched = true;
        int remaining = maxMillisOpened - millisOpened;
        // Serial.println(String("Going up max distance. The current positon is: ") + millisOpened + " the max is: " + maxMillisOpened + " And we will move: " + remaining);
        if (remaining > 0)
            goUpTimed(static_cast<unsigned long>(remaining));
        else
            handleMovementFinished();
    }

    void goDownMax()
    {
        if (active)
        {
            return;
        };
        latched = true;
        int toClose = millisOpened;
        // Serial.println(String("Going down max distance. The current positon is: ") + millisOpened + " the min is: " + 0 + " And we will move: " + toClose);
        if (toClose > 0)
            goDownTimed(static_cast<unsigned long>(toClose));
        else
            handleMovementFinished();
    }

    void startAutoMovement()
    {
        if (active)
        {
            return;
        }
        latched = true;
        doingAutoCycle = true;

        goUpMax();
    }

    void startUpMovement()
    {
        if (!active)
        {
            digitalWrite(pinDown, HIGH);
            digitalWrite(pinUp, LOW);
            active = true;
            latched = false;
        }
    }

    void startDownMovement()
    {
        if (!active)
        {
            digitalWrite(pinUp, HIGH);
            digitalWrite(pinDown, LOW);
            active = true;
            latched = false;
        }
    }

    void stopMovement()
    {
        digitalWrite(pinUp, HIGH);
        digitalWrite(pinDown, HIGH);
        active = false;
    }
    
    void stopManualMovement() {
        tStop.disable();
        stopMovement();
        latched = false;
        doingAutoCycle = false;
    }

    void handleMovementFinished()
    {
        stopMovement();

        if (doingAutoCycle)
        {
            if (millisOpened >= maxMillisOpened)
            {
                goDownMax();
            }
            else if (millisOpened <= 0)
            {
                doingAutoCycle = false;
                latched = false;
            }
        }
        else
        {
            latched = false;
        }
    }

    /** SETTERS AND GETTERS **/

    bool getActive()
    {
        return active;
    }

    bool getLatched()
    {
        return latched;
    }

    void setMillisOpened(int millis)
    {
        millisOpened = millis;
    }

    int getMaxMillisOpened()
    {
        return maxMillisOpened;
    }
};