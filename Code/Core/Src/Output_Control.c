#include "Output_Control.h"

Output_Control_t g_output_control[OUTPUT_CONTROL_COUNT];

void Output_Control_Init(void){
    for(uint8_t i = 0; i < OUTPUT_CONTROL_COUNT; i++){
        g_output_control[i].output_channel = i;
        g_output_control[i].output_state = 0;
    }
}

void Output_Control_Process(void){
}

void Set_Output_Control_State(uint8_t output_channel, uint8_t output_state)
{
    for (uint8_t i = 0; i < OUTPUT_CONTROL_COUNT; i++)
    {
        g_holdingRegisters[REG_RELAY1_CONTROL + i] = g_output_control[i].output_state;
    }
}