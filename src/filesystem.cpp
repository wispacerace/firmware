#include "ff.h"

FATFS fs; // statically allocate working area for fatfs volume
void init_fs() {
    f_mount(&fs, "", 0);
}