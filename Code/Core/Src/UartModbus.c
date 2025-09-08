#include "UartModbus.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "main.h"
#include "ModbusMap.h"



// Global register arrays definition
uint16_t g_holdingRegisters[HOLDING_REG_COUNT];
uint16_t g_inputRegisters[INPUT_REG_COUNT];
uint8_t g_coils[COIL_COUNT];
uint8_t g_discreteInputs[DISCRETE_COUNT];

// Task counters
uint32_t g_taskCounter = 0;
uint32_t g_modbusCounter = 0;

// UART buffer variables
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;
uint8_t frameReceived = 0;
uint32_t g_lastUARTActivity = 0;

// Diagnostic variables
uint32_t g_totalReceived = 0;
uint32_t g_corruptionCount = 0;
uint8_t g_receivedIndex = 0;



void initializeModbusRegisters(void) {
    // Initialize all registers to default values
    
    // System Registers (0x00F0-0x00F6)
    g_holdingRegisters[REG_DEVICE_ID] = DEFAULT_DEVICE_ID;  
    g_holdingRegisters[REG_CONFIG_BAUDRATE] = DEFAULT_CONFIG_BAUDRATE;
    g_holdingRegisters[REG_CONFIG_PARITY] = DEFAULT_CONFIG_PARITY;
    g_holdingRegisters[REG_CONFIG_STOP_BIT] = DEFAULT_CONFIG_STOP_BIT;
    g_holdingRegisters[REG_MODULE_TYPE] = DEFAULT_MODULE_TYPE;
    g_holdingRegisters[REG_FIRMWARE_VERSION] = DEFAULT_FIRMWARE_VERSION;
    g_holdingRegisters[REG_HARDWARE_VERSION] = DEFAULT_HARDWARE_VERSION;
    g_holdingRegisters[REG_SYSTEM_STATUS] = DEFAULT_SYSTEM_STATUS;
    g_holdingRegisters[REG_SYSTEM_ERROR] = DEFAULT_SYSTEM_ERROR;
    g_holdingRegisters[REG_RESET_ERROR_COMMAND] = DEFAULT_RESET_ERROR_COMMAND;
    
    // Safety Module Registers (0x0000-0x000C)
    g_holdingRegisters[REG_ANALOG_1_ENABLE] = DEFAULT_ANALOG_1_ENABLE;
    g_holdingRegisters[REG_ANALOG_2_ENABLE] = DEFAULT_ANALOG_2_ENABLE;
    g_holdingRegisters[REG_ANALOG_3_ENABLE] = DEFAULT_ANALOG_3_ENABLE;
    g_holdingRegisters[REG_ANALOG_4_ENABLE] = DEFAULT_ANALOG_4_ENABLE;
    g_holdingRegisters[REG_ANALOG_COEFFICIENT] = DEFAULT_ANALOG_COEFFICIENT;
    g_holdingRegisters[REG_ANALOG_CALIBRATION] = DEFAULT_ANALOG_CALIBRATION;
    g_holdingRegisters[REG_DI1_ENABLE] = DEFAULT_DI1_ENABLE;
    g_holdingRegisters[REG_DI2_ENABLE] = DEFAULT_DI2_ENABLE;
    g_holdingRegisters[REG_DI3_ENABLE] = DEFAULT_DI3_ENABLE;
    g_holdingRegisters[REG_DI4_ENABLE] = DEFAULT_DI4_ENABLE;
    g_holdingRegisters[REG_RELAY1_CONTROL] = DEFAULT_RELAY1_CONTROL;
    g_holdingRegisters[REG_RELAY2_CONTROL] = DEFAULT_RELAY2_CONTROL;
    g_holdingRegisters[REG_RELAY3_CONTROL] = DEFAULT_RELAY3_CONTROL;
    g_holdingRegisters[REG_RELAY4_CONTROL] = DEFAULT_RELAY4_CONTROL;
    g_holdingRegisters[REG_SAFETY_ZONE1_THRESHOLD] = DEFAULT_SAFETY_ZONE1_THRESHOLD;
    g_holdingRegisters[REG_SAFETY_ZONE2_THRESHOLD] = DEFAULT_SAFETY_ZONE2_THRESHOLD;
    g_holdingRegisters[REG_SAFETY_ZONE3_THRESHOLD] = DEFAULT_SAFETY_ZONE3_THRESHOLD;
    g_holdingRegisters[REG_SAFETY_ZONE4_THRESHOLD] = DEFAULT_SAFETY_ZONE4_THRESHOLD;
    g_holdingRegisters[REG_PROXIMITY_THRESHOLD] = DEFAULT_PROXIMITY_THRESHOLD;
    g_holdingRegisters[REG_SAFETY_RESPONSE_TIME] = DEFAULT_SAFETY_RESPONSE_TIME;
    g_holdingRegisters[REG_AUTO_RESET_ENABLE] = DEFAULT_AUTO_RESET_ENABLE;
    g_holdingRegisters[REG_SAFETY_MODE] = DEFAULT_SAFETY_MODE;
    

    // Initialize other arrays
    for (int i = 0; i < INPUT_REG_COUNT; i++) {
        g_inputRegisters[i] = 0;
    }
    
    for (int i = 0; i < COIL_COUNT; i++) {
        g_coils[i] = 0;
    }
    
    for (int i = 0; i < DISCRETE_COUNT; i++) {
        g_discreteInputs[i] = 0;
    }
}


uint16_t calcCRC(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        g_lastUARTActivity = HAL_GetTick();
        
        if (rxIndex < RX_BUFFER_SIZE - 1) {
            rxBuffer[rxIndex++] = huart->Instance->DR;
            frameReceived = 1;
            
            if (rxIndex >= 6) {
                uint8_t expectedLength = 0;
                if (rxBuffer[1] == 3 || rxBuffer[1] == 6) {
                    expectedLength = 8;
                } else if (rxBuffer[1] == 4) {
                    expectedLength = 8;
                } else if (rxBuffer[1] == 16) {
                    if (rxIndex >= 7) {
                        expectedLength = 9 + rxBuffer[6];
                    }
                }
                
                if (rxIndex >= expectedLength) {
                    processModbusFrame();
                }
            }
        } else {
            rxIndex = 0;
            frameReceived = 0;
        }
        HAL_UART_Receive_IT(&huart2, &rxBuffer[rxIndex], 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        rxIndex = 0;
        frameReceived = 0;
        HAL_UART_Abort(&huart2);
        HAL_UART_Receive_IT(&huart2, &rxBuffer[rxIndex], 1);
    }
}

void resetUARTCommunication(void) {
    HAL_UART_Abort(&huart2);
    rxIndex = 0;
    frameReceived = 0;
    HAL_UART_Receive_IT(&huart2, &rxBuffer[0], 1);
}

void processModbusFrame(void) {
    if (rxIndex < 6) return;
    if (rxBuffer[0] != MODBUS_SLAVE_ADDRESS) return;

    uint16_t crc = calcCRC(rxBuffer, rxIndex - 2);
    if (rxBuffer[rxIndex - 2] != (crc & 0xFF) || rxBuffer[rxIndex - 1] != (crc >> 8)) {
        return;
    }

    uint8_t funcCode = rxBuffer[1];
    uint8_t txBuffer[256];
    uint8_t txIndex = 0;
    txBuffer[0] = MODBUS_SLAVE_ADDRESS;
    txBuffer[1] = funcCode;

    if (funcCode == 3) {
        uint16_t addr = (rxBuffer[2] << 8) | rxBuffer[3];
        uint16_t qty = (rxBuffer[4] << 8) | rxBuffer[5];
        if (addr + qty <= HOLDING_REG_COUNT) {
            txBuffer[2] = qty * 2;
            txIndex = 3;
            for (int i = 0; i < qty; i++) {
                txBuffer[txIndex++] = g_holdingRegisters[addr + i] >> 8;
                txBuffer[txIndex++] = g_holdingRegisters[addr + i] & 0xFF;
            }
        } else {
            txBuffer[1] |= 0x80;
            txBuffer[2] = 0x02;
            txIndex = 3;
        }
    } else if (funcCode == 4) {
        uint16_t addr = (rxBuffer[2] << 8) | rxBuffer[3];
        uint16_t qty = (rxBuffer[4] << 8) | rxBuffer[5];
        if (addr + qty <= INPUT_REG_COUNT) {
            txBuffer[2] = qty * 2;
            txIndex = 3;
            for (int i = 0; i < qty; i++) {
                txBuffer[txIndex++] = g_inputRegisters[addr + i] >> 8;
                txBuffer[txIndex++] = g_inputRegisters[addr + i] & 0xFF;
            }
        } else {
            txBuffer[1] |= 0x80;
            txBuffer[2] = 0x02;
            txIndex = 3;
        }
    } else if (funcCode == 6) {
        uint16_t addr = (rxBuffer[2] << 8) | rxBuffer[3];
        uint16_t value = (rxBuffer[4] << 8) | rxBuffer[5];
        if (addr < HOLDING_REG_COUNT) {
            g_holdingRegisters[addr] = value;
            
            // Handle special register writes
            if (addr == REG_RESET_ERROR_COMMAND && value == 1) {
                g_holdingRegisters[REG_SYSTEM_ERROR] = 0;
            }
            
            txBuffer[2] = rxBuffer[2];
            txBuffer[3] = rxBuffer[3];
            txBuffer[4] = rxBuffer[4];
            txBuffer[5] = rxBuffer[5];
            txIndex = 6;
        } else {
            txBuffer[1] |= 0x80;
            txBuffer[2] = 0x02;
            txIndex = 3;
        }
    } else if (funcCode == 16) {
        uint16_t addr = (rxBuffer[2] << 8) | rxBuffer[3];
        uint16_t qty = (rxBuffer[4] << 8) | rxBuffer[5];
        uint8_t byteCount = rxBuffer[6];
        if (addr + qty <= HOLDING_REG_COUNT && byteCount == qty * 2) {
            for (int i = 0; i < qty; i++) {
                g_holdingRegisters[addr + i] = (rxBuffer[7 + i*2] << 8) | rxBuffer[8 + i*2];
            }
            txBuffer[2] = rxBuffer[2];
            txBuffer[3] = rxBuffer[3];
            txBuffer[4] = rxBuffer[4];
            txBuffer[5] = rxBuffer[5];
            txIndex = 6;
        } else {
            txBuffer[1] |= 0x80;
            txBuffer[2] = 0x02;
            txIndex = 3;
        }
    } else {
        txBuffer[1] |= 0x80;
        txBuffer[2] = 0x01;
        txIndex = 3;
    }

    crc = calcCRC(txBuffer, txIndex);
    txBuffer[txIndex++] = crc & 0xFF;
    txBuffer[txIndex++] = crc >> 8;
    
    if (HAL_UART_Transmit(&huart2, txBuffer, txIndex, 100) != HAL_OK) {
        HAL_UART_Abort(&huart2);

    } else {
    }
    
    rxIndex = 0;
    frameReceived = 0;
}