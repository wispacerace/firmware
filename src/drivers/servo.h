#include "ch.h"
#include "hal.h"

class Servo {
  public:
    Servo(PWMDriver *drv, pwmchannel_t chnl);
    int read();
    void write(int angle);

  private:
    int angle;
    pwmchannel_t channel;
    PWMDriver *drv;
};
