#pragma once
#include "lfs.h"

constexpr auto cache_size = 16;
constexpr auto lookahead_size = 8;
static uint8_t read_buffer[cache_size];
static uint8_t prog_buffer[cache_size];
static uint8_t lookahead_buffer[lookahead_size];

int sd_read_block(const struct lfs_config* c, lfs_block_t block,
	lfs_off_t off, void *buffer, lfs_size_t size);
int sd_write_block(const struct lfs_config* c, lfs_block_t block,
	lfs_off_t off, const void *buffer, lfs_size_t size);
int sd_erase_block(const struct lfs_config* c, lfs_block_t block);
int sd_device_sync(const struct lfs_config* c);

const struct lfs_config cfg = {
	// block device operations
	.read = sd_read_block,
	.prog = sd_write_block,
	.erase = sd_erase_block,
	.sync = sd_device_sync,

	// block device configuration
	.read_size = 16,
	.prog_size = 16,
	.block_size = 512,
	.block_count = 256,
	.cache_size = cache_size,
	.lookahead_size = lookahead_size,


	.read_buffer = read_buffer,
	.prog_buffer = prog_buffer,
	.lookahead_buffer = lookahead_buffer,
};