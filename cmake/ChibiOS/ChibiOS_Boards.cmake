add_library(ChibiOS::Board::NucleoF413ZH INTERFACE IMPORTED)
target_include_directories(ChibiOS::Board::NucleoF413ZH INTERFACE
        ${ChibiOS_ROOT}/os/hal/boards/ST_NUCLEO144_F413ZH)
target_sources(ChibiOS::Board::NucleoF413ZH INTERFACE
        ${ChibiOS_ROOT}/os/hal/boards/ST_NUCLEO144_F413ZH/board.c)