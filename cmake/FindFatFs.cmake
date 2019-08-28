add_library(FatFs INTERFACE IMPORTED)
target_sources(FatFs INTERFACE
        ${FatFs_ROOT}/source/ff.c
        ${FatFs_ROOT}/source/ffunicode.c)
target_include_directories(FatFs INTERFACE
        ${FatFs_ROOT}/source/)
