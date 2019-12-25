#pragma once

#include <ch.h>

/*
 * Notes:
 *
 * 1) #define FAULT_NO_PRINT to remove chprintf, etc
 * 2) #define FAULT_INFO_HOOK(fault_info) to receive a struct fault_info when
 *    a fault is produced.
 */

struct decoded_fault_registers {
    struct general {
        bool bus_fault_on_ivt_read;
        bool escalation;
        uint32_t current_thread_address;
        const char *current_thread_name;
    } general;
    struct memfault {
        bool stacking_error;
        bool unstacking_error;
        bool data_access_violation;
        uint32_t data_access_violation_address;
        bool instruction_access_violation;
    } memfault;
    struct busfault {
        bool stacking_error;
        bool unstacking_error;
        bool precise_data_bus_error;
        uint32_t precise_data_bus_error_address;
        bool imprecise_data_bus_error;
        bool instruction_bus_error;
    } busfault;
    struct usagefault {
        bool division_by_zero;
        bool unaligned_memory_access;
        bool no_coprocessor_instructions;
        bool invalid_load_of_pc;
        bool invalid_state;
        bool undefined_instruction;
    } usagefault;
};

struct fault_info {
    struct decoded_fault_registers decoded_fault_registers;
#ifndef FAULT_NO_PRINT
    char decoded_info_string[300];
#endif
};
