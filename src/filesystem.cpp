#include "ch.h"
#include "hal.h"

#include "filesystem.h"

int sd_read_block(const struct lfs_config* c, lfs_block_t block,
	lfs_off_t off, void *buffer, lfs_size_t size) {
}
int sd_write_block(const struct lfs_config* c, lfs_block_t block,
	lfs_off_t off, const void *buffer, lfs_size_t size) {
}
int sd_erase_block(const struct lfs_config* c, lfs_block_t block) {
	sdcErase(&SDCD1, block, block);

	return 0;
}
int sd_device_sync(const struct lfs_config* c) {
	return 0;
}
