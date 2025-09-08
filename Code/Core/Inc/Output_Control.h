#ifndef OUTPUT_CONTROL_H
#define OUTPUT_CONTROL_H

#include <stdint.h>
#include "Safety_Monitor.h"
#include "ModbusMap.h"

#define OUTPUT_CONTROL_COUNT 4


typedef struct {
    uint8_t output_channel;
    uint8_t output_state;
} Output_Control_t;

extern Output_Control_t g_output_control[OUTPUT_CONTROL_COUNT];

void Output_Control_Init(void);
void Output_Control_Process(void);
void Set_Output_Control_State(uint8_t output_channel, uint8_t output_state);

#endif