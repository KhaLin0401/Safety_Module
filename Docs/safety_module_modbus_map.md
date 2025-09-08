# 📘 Modbus Register Map - Safety Module

## 🟣 System Registers (0x0100 - 0x0109)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0100 | Device_ID | uint8 | R/W | Modbus slave address | 5 |
| 0x0101 | Config_Baudrate | uint8 | R/W | 1=9600, 2=19200, 3=38400, 4=57600, 5=115200 | 5 |
| 0x0102 | Config_Parity | uint8 | R/W | 0=None, 1=Even, 2=Odd | 0 |
| 0x0103 | Config_Stop_bit | uint8 | R/W | 1 or 2 | 1 |
| 0x0104 | Module_Type | uint8 | R | Type of module | 6 |
| 0x0105 | Firmware_Version | uint16 | R | Version of firmware | 0x0001 |
| 0x0106 | Hardware_Version | uint16 | R | Version of hardware | 0x0001 |
| 0x0107 | System_Status | uint16 | R | Bitfield: system status | 0x0000 |
| 0x0108 | System_Error | uint16 | R | Global error code | 0 |
| 0x0109 | Reset_Error_Command | uint16 | W | Write 1 to reset all error flags | 0 |

## 🟣 Safety Status Registers (0x0000 - 0x0005)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0000 | Safety_System_Status | uint16 | R | Overall safety system status | 0x0000 |
| 0x0001 | Emergency_Stop_Status | uint16 | R | E-Stop status (0=Normal, 1=Pressed) | 0 |
| 0x0002 | Safety_Zone_Status | uint16 | R | Safety zone status (bitfield) | 0 |
| 0x0003 | Proximity_Alert_Status | uint16 | R | Proximity alert status (bitfield) | 0 |
| 0x0004 | Relay_Output_Status | uint16 | R | Relay outputs status (bitfield) | 0 |
| 0x0005 | Safety_Error_Code | uint16 | R | Safety error code | 0 |

## 🟣 Analog Input Registers (0x0010 - 0x0021)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0010 | Analog_Input_1 | uint16 | R | Analog Input 1 - Processed value (mV) | 0 |
| 0x0011 | Analog_Input_2 | uint16 | R | Analog Input 2 - Processed value (mV) | 0 |
| 0x0012 | Analog_Input_3 | uint16 | R | Analog Input 3 - Processed value (mV) | 0 |
| 0x0013 | Analog_Input_4 | uint16 | R | Analog Input 4 - Processed value (mV) | 0 |
| 0x0014 | Analog_Coefficient | uint16 | R/W | Analog processing coefficient | 20 |
| 0x0015 | Analog_Calibration | uint16 | R/W | Analog calibration value | 15 |
| 0x0016 | Analog_Input_1_Raw | uint16 | R | Analog Input 1 - Raw ADC (0-4095) | 0 |
| 0x0017 | Analog_Input_2_Raw | uint16 | R | Analog Input 2 - Raw ADC (0-4095) | 0 |
| 0x0018 | Analog_Input_3_Raw | uint16 | R | Analog Input 3 - Raw ADC (0-4095) | 0 |
| 0x0019 | Analog_Input_4_Raw | uint16 | R | Analog Input 4 - Raw ADC (0-4095) | 0 |
| 0x001A | Analog_1_Enable | uint16 | R/W | Enable/disable Analog Input 1 | 0 |
| 0x001B | Analog_2_Enable | uint16 | R/W | Enable/disable Analog Input 2 | 0 |
| 0x001C | Analog_3_Enable | uint16 | R/W | Enable/disable Analog Input 3 | 0 |
| 0x001D | Analog_4_Enable | uint16 | R/W | Enable/disable Analog Input 4 | 0 |
| 0x001E | Analog_1_Offset | uint16 | R/W | Offset value for Analog Input 1 | 0 |
| 0x001F | Analog_2_Offset | uint16 | R/W | Offset value for Analog Input 2 | 0 |
| 0x0020 | Analog_3_Offset | uint16 | R/W | Offset value for Analog Input 3 | 0 |
| 0x0021 | Analog_4_Offset | uint16 | R/W | Offset value for Analog Input 4 | 0 |

## 🟣 Digital Input Registers (0x0022 - 0x0029)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0022 | DI1_Status | uint16 | R | Trạng thái Digital Input 1 | 0 |
| 0x0023 | DI2_Status | uint16 | R | Trạng thái Digital Input 2 | 0 |
| 0x0024 | DI3_Status | uint16 | R | Trạng thái Digital Input 3 | 0 |
| 0x0025 | DI4_Status | uint16 | R | Trạng thái Digital Input 4 | 0 |
| 0x0026 | DI1_Enable | uint16 | R/W | Bật/tắt Digital Input 1 | 0 |
| 0x0027 | DI2_Enable | uint16 | R/W | Bật/tắt Digital Input 2 | 0 |
| 0x0028 | DI3_Enable | uint16 | R/W | Bật/tắt Digital Input 3 | 0 |
| 0x0029 | DI4_Enable | uint16 | R/W | Bật/tắt Digital Input 4 | 0 |
| 0x002A | DI1_Active_Level | uint16 | R/W | Mức tích cực Digital Input 1 (0=Low, 1=High) | 0 |
| 0x002B | DI2_Active_Level | uint16 | R/W | Mức tích cực Digital Input 2 (0=Low, 1=High) | 0 |
| 0x002C | DI3_Active_Level | uint16 | R/W | Mức tích cực Digital Input 3 (0=Low, 1=High) | 0 |
| 0x002D | DI4_Active_Level | uint16 | R/W | Mức tích cực Digital Input 4 (0=Low, 1=High) | 0 |

## 🟣 Relay Output Control Registers (0x002A - 0x002D)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0040 | Relay1_Control | uint16 | R/W | Control Relay Output 1 | 0 |
| 0x0041 | Relay2_Control | uint16 | R/W | Control Relay Output 2 | 0 |
| 0x0042 | Relay3_Control | uint16 | R/W | Control Relay Output 3 | 0 |
| 0x0043 | Relay4_Control | uint16 | R/W | Control Relay Output 4 | 0 |

## 🟣 Safety Configuration Registers (0x0044 - 0x004B)

| **Address** | **Name** | **Type** | **R/W** | **Description** | **Default** |
|-------------|----------|----------|---------|-----------------|-------------|
| 0x0044 | Safety_Zone1_Threshold | uint16 | R/W | Safety Zone 1 threshold | 500 |
| 0x0045 | Safety_Zone2_Threshold | uint16 | R/W | Safety Zone 2 threshold | 1000 |
| 0x0046 | Safety_Zone3_Threshold | uint16 | R/W | Safety Zone 3 threshold | 1500 |
| 0x0047 | Safety_Zone4_Threshold | uint16 | R/W | Safety Zone 4 threshold | 2000 |
| 0x0048 | Proximity_Threshold | uint16 | R/W | Proximity sensor threshold | 100 |
| 0x0049 | Safety_Response_Time | uint16 | R/W | Safety response time (ms) | 50 |
| 0x004A | Auto_Reset_Enable | uint16 | R/W | Enable auto reset (0=Off, 1=On) | 0 |
| 0x004B | Safety_Mode | uint16 | R/W | Safety mode (1=Normal, 2=Warning, 3=Protective Stop, 4=Emergency Stop) | 1 |