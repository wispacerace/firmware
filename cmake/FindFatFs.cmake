add_library(FatFs INTERFACE IMPORTED)
target_include_directories(FatFs INTERFACE "${FatFs_ROOT}/src" "${FatFs_CONFIGDIR}")
target_sources(FatFs INTERFACE "${FatFs_ROOT}/src/ff.c"
                               "${FatFs_ROOT}/src/ffunicode.c")