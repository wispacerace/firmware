#pragma once

#include <ff.h>

extern FATFS fs; // statically allocate working area for fatfs volume
void fs_init();
