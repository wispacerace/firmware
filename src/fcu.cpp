#include "ch.h"
#include "hal.h"

int main() {
    halInit();
    chSysInit();

    for (;;) {}
}