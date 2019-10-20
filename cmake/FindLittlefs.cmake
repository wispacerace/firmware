add_library(littlefs INTERFACE IMPORTED)
target_sources(littlefs INTERFACE
	"${Littlefs_ROOT}/lfs.c"
	"${Littlefs_ROOT}/lfs_util.c")
target_include_directories(littlefs INTERFACE "${Littlefs_ROOT}/")