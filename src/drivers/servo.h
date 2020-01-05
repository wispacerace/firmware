#include "ch.h"
#include "hal.h"

class Servo {
  public:
    Servo(PWMDriver *drv, pwmchannel_t chnl)
        : channel(chnl), drv(drv), angle(90) {}
    int read();
    void write(int new_angle);

  private:
    int angle;
    pwmchannel_t channel;
    PWMDriver *drv;
};
