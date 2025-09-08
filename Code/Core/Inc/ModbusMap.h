#ifndef MODBUSMAP_H
#define MODBUSMAP_H

#include <stdint.h>

// System Registers (Base Address: 0x0000)
#define REG_DEVICE_ID              0x0100
#define REG_CONFIG_BAUDRATE        0x0101
#define REG_CONFIG_PARITY          0x0102
#define REG_CONFIG_STOP_BIT        0x0103
#define REG_MODULE_TYPE            0x0104
#define REG_FIRMWARE_VERSION       0x0105
#define REG_HARDWARE_VERSION       0x0106
#define REG_SYSTEM_STATUS          0x0107
#define REG_SYSTEM_ERROR           0x0108
#define REG_RESET_ERROR_COMMAND    0x0109


// Motor 1 Registers (Base Address: 0x0010)
// Safety System Status Registers
#define REG_SAFETY_SYSTEM_STATUS   0x0000
#define REG_EMERGENCY_STOP_STATUS  0x0001  
#define REG_SAFETY_ZONE_STATUS     0x0002
#define REG_PROXIMITY_ALERT_STATUS 0x0003
#define REG_RELAY_OUTPUT_STATUS    0x0004
#define REG_SAFETY_ERROR_CODE      0x0005

// Analog Input Registers (Processed Values - Giá trị đã chuyển đổi)
// CÁC THANH GHI NÀY CHỨA GIÁ TRỊ CUỐI CÙNG SAU KHI ĐÃ XỬ LÝ VÀ HIỆU CHUẨN
#define REG_ANALOG_INPUT_1         0x0010  // Analog Input 1 - Final processed value (millivolts)
#define REG_ANALOG_INPUT_2         0x0011  // Analog Input 2 - Final processed value (millivolts)  
#define REG_ANALOG_INPUT_3         0x0012  // Analog Input 3 - Final processed value (millivolts)
#define REG_ANALOG_INPUT_4         0x0013  // Analog Input 4 - Final processed value (millivolts)
#define REG_ANALOG_COEFFICIENT     0x0014
#define REG_ANALOG_CALIBRATION     0x0015
// #define REG_ANALOG_INPUT_1_RAW     0x0016  // Analog Input 1 - Raw ADC value (0-4095)
// #define REG_ANALOG_INPUT_2_RAW     0x0017  // Analog Input 2 - Raw ADC value (0-4095)
// #define REG_ANALOG_INPUT_3_RAW     0x0018  // Analog Input 3 - Raw ADC value (0-4095)
// #define REG_ANALOG_INPUT_4_RAW     0x0019  // Analog Input 4 - Raw ADC value (0-4095)
#define REG_ANALOG_1_ENABLE        0x001A
#define REG_ANALOG_2_ENABLE        0x001B
#define REG_ANALOG_3_ENABLE        0x001C
#define REG_ANALOG_4_ENABLE        0x001D
#define REG_ANALOG_1_OFFSET        0x001E
#define REG_ANALOG_2_OFFSET        0x001F
#define REG_ANALOG_3_OFFSET        0x0020
#define REG_ANALOG_4_OFFSET        0x0021


// Digital Input Registers
#define REG_DI1_STATUS             0x0022
#define REG_DI2_STATUS             0x0023
#define REG_DI3_STATUS             0x0024
#define REG_DI4_STATUS             0x0025
#define REG_DI1_ENABLE             0x0026
#define REG_DI2_ENABLE             0x0027
#define REG_DI3_ENABLE             0x0028
#define REG_DI4_ENABLE             0x0029
#define REG_DI1_ACTIVE_LEVEL       0x002A
#define REG_DI2_ACTIVE_LEVEL       0x002B
#define REG_DI3_ACTIVE_LEVEL       0x002C
#define REG_DI4_ACTIVE_LEVEL       0x002D

// Relay Output Control Registers  
#define REG_RELAY1_CONTROL         0x0040  // Control Relay Output 1
#define REG_RELAY2_CONTROL         0x0041  // Control Relay Output 2
#define REG_RELAY3_CONTROL         0x0042  // Control Relay Output 3
#define REG_RELAY4_CONTROL         0x0043  // Control Relay Output 4

// Safety Configuration Registers
#define REG_SAFETY_ZONE1_THRESHOLD 0x0044  // Safety Zone 1 threshold
#define REG_SAFETY_ZONE2_THRESHOLD 0x0045  // Safety Zone 2 threshold
#define REG_SAFETY_ZONE3_THRESHOLD 0x0046  // Safety Zone 3 threshold
#define REG_SAFETY_ZONE4_THRESHOLD 0x0047  // Safety Zone 4 threshold
#define REG_PROXIMITY_THRESHOLD    0x0048  // Proximity sensor threshold
#define REG_SAFETY_RESPONSE_TIME   0x0049  // Safety response time (ms)
#define REG_AUTO_RESET_ENABLE      0x004A  // Enable auto reset (0=Off, 1=On)
#define REG_SAFETY_MODE            0x004B  // Safety mode (1=Normal, 2=Warning, 3=Protective Stop, 4=Emergency Stop)

// Total register count  
#define TOTAL_HOLDING_REG_COUNT    0x0036  // Total number of registers (0x0000-0x0035)

// Default Values for System Registers
#define DEFAULT_DEVICE_ID          5
#define DEFAULT_CONFIG_BAUDRATE    5
#define DEFAULT_CONFIG_PARITY      0
#define DEFAULT_CONFIG_STOP_BIT    1
#define DEFAULT_MODULE_TYPE        6
#define DEFAULT_FIRMWARE_VERSION   0x0001
#define DEFAULT_HARDWARE_VERSION   0x0001
#define DEFAULT_SYSTEM_STATUS      0x0000
#define DEFAULT_SYSTEM_ERROR       0
#define DEFAULT_RESET_ERROR_COMMAND 0


// Giá trị mặc định cho các thanh ghi Analog Input
#define DEFAULT_ANALOG_INPUT_1      0       // Giá trị mặc định cho AI1 
#define DEFAULT_ANALOG_INPUT_2      0       // Giá trị mặc định cho AI2
#define DEFAULT_ANALOG_INPUT_3      0       // Giá trị mặc định cho AI3  
#define DEFAULT_ANALOG_INPUT_4      0       // Giá trị mặc định cho AI4
#define DEFAULT_ANALOG_INPUT_1_RAW  0       // Giá trị raw mặc định cho AI1
#define DEFAULT_ANALOG_INPUT_2_RAW  0       // Giá trị raw mặc định cho AI2
#define DEFAULT_ANALOG_INPUT_3_RAW  0       // Giá trị raw mặc định cho AI3
#define DEFAULT_ANALOG_INPUT_4_RAW  0  
#define DEFAULT_ANALOG_COEFFICIENT  2786      // Giá trị hệ số mặc định cho AI
#define DEFAULT_ANALOG_CALIBRATION  115       // Giá trị điều chỉnh mặc định cho AI
#define DEFAULT_ANALOG_1_ENABLE     0
#define DEFAULT_ANALOG_2_ENABLE     0
#define DEFAULT_ANALOG_3_ENABLE     0
#define DEFAULT_ANALOG_4_ENABLE     0

#define DEFAULT_DI1_ENABLE     0
#define DEFAULT_DI2_ENABLE     0
#define DEFAULT_DI3_ENABLE     0
#define DEFAULT_DI4_ENABLE     0
#define DEFAULT_RELAY_OUTPUT_CONTROL 0x0000
#define DEFAULT_RELAY1_CONTROL       0
#define DEFAULT_RELAY2_CONTROL       0
#define DEFAULT_RELAY3_CONTROL       0
#define DEFAULT_RELAY4_CONTROL       0

#define DEFAULT_SAFETY_ZONE1_THRESHOLD  500
#define DEFAULT_SAFETY_ZONE2_THRESHOLD  1000
#define DEFAULT_SAFETY_ZONE3_THRESHOLD  1500
#define DEFAULT_SAFETY_ZONE4_THRESHOLD  2000
#define DEFAULT_PROXIMITY_THRESHOLD     100
#define DEFAULT_SAFETY_RESPONSE_TIME    50
#define DEFAULT_AUTO_RESET_ENABLE       0
#define DEFAULT_SAFETY_MODE             1

// Giá trị mặc định cho các thanh ghi Digital Input
#define DEFAULT_DI1_STATUS          0        // Trạng thái mặc định DI1
#define DEFAULT_DI2_STATUS          0        // Trạng thái mặc định DI2
#define DEFAULT_DI3_STATUS          0        // Trạng thái mặc định DI3
#define DEFAULT_DI4_STATUS          0        // Trạng thái mặc định DI4

// Giá trị mặc định cho các thanh ghi Relay Output
#define DEFAULT_RELAY1_CONTROL       0       // Điều khiển mặc định relay 1
#define DEFAULT_RELAY2_CONTROL       0       // Điều khiển mặc định relay 2
#define DEFAULT_RELAY3_CONTROL       0       // Điều khiển mặc định relay 3
#define DEFAULT_RELAY4_CONTROL       0       // Điều khiển mặc định relay 4

// Giá trị mặc định cho các thanh ghi cấu hình an toàn
#define DEFAULT_SAFETY_ZONE1_THRESHOLD  500    // Ngưỡng mặc định vùng an toàn 1
#define DEFAULT_SAFETY_ZONE2_THRESHOLD  1000   // Ngưỡng mặc định vùng an toàn 2
#define DEFAULT_SAFETY_ZONE3_THRESHOLD  1500   // Ngưỡng mặc định vùng an toàn 3
#define DEFAULT_SAFETY_ZONE4_THRESHOLD  2000   // Ngưỡng mặc định vùng an toàn 4
#define DEFAULT_PROXIMITY_THRESHOLD     100    // Ngưỡng mặc định cảm biến tiệm cận
#define DEFAULT_SAFETY_RESPONSE_TIME    50     // Thời gian phản hồi mặc định (ms)
#define DEFAULT_AUTO_RESET_ENABLE       0      // Tắt tự động reset
#define DEFAULT_SAFETY_MODE             1      // Chế độ an toàn mặc định

// Các giá trị trạng thái an toàn
#define SAFETY_MODE_NORMAL          1    // Chế độ hoạt động bình thường
#define SAFETY_MODE_WARNING         2    // Chế độ cảnh báo
#define SAFETY_MODE_PROTECTIVE_STOP 3    // Chế độ dừng bảo vệ
#define SAFETY_MODE_EMERGENCY_STOP  4    // Chế độ dừng khẩn cấp

#endif // 