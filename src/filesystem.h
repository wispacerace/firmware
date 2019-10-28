#pragma once
#include "ch.h"

#define SD_BLOCK_SIZE_HC 512

#include "lfs.h"

typedef struct lfs_bd_context {
	SDCDriver *sdc;
} lfs_context;
void init_fs(SDCDriver *sdc);
