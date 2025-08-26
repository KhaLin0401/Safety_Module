# 📘 Modbus Register Map – Dual DC Motor Driver (STM32F103C8T6)

## 🟣 System Registers

| **Address** | **Name**                    | **Type**     | **R/W** | **Description**                                  | **Default** |
|---------|-------------------------|----------|-----|----------------------------------------------|---------|
| 0x0100  | Device_ID               | uint8   | R/W | Modbus slave address                         | 6       |
| 0x0101  | Config_Baudrate        | uint8   | R/W   | 	1=9600, 2=19200, 3=38400, 4=57600, 5=115200       | 5  |
| 0x0102  | Config_Parity           | uint8   | R/W | 0=None, 1=Even, 2=Odd     |    0      |
| 0x0103  | Config_Stop_bit           | uint8   | R/W | 1 or 2  |   1         | 
| 0x0104  | Module Type           | uint8   | R | Type of module                         | 6 = `safety module`       |
| 0x0105  | Firmware Version           | uint16   | R | Version of firmware                         | 0x001=`v0.01`       |
| 0x0106  | Hardware Version            | uint16   | R | Version of hardware                         | 0x001=`v0.01`       |
| 0x0107  | System_Status           | uint16   | R   | Bitfield: system status                      | 0x0000  |
| 0x0108  | System_Error            | uint16   | R   | Global error code                            | 0       |
| 0x0109  | Reset_Error_Command     | uint16   | W   | Write 1 to reset all error flags             | 0   |

---

## 🟣 Safety Module Register
| **Register Address** | **Tên Register** | **Loại** | **Đơn vị** | **Mô tả** | **Access** |
|---------------------|------------------|----------|------------|-----------|------------|
| **0x0000** | Safety System Status | Input | - | Trạng thái hệ thống an toàn | R |
| **0x0001** | Emergency Stop Status | Input | - | Trạng thái E-Stop (0=Normal, 1=Pressed) | R |
| **0x0002** | Safety Zone Status | Input | - | Trạng thái vùng an toàn (bit field) | R |
| **0x0003** | Proximity Alert Status | Input | - | Trạng thái cảnh báo tiệm cận (bit field) | R |
| **0x0004** | Relay Output Status | Input | - | Trạng thái relay outputs (bit field) | R |
| **0x0005** | Safety Error Code | Input | - | Mã lỗi an toàn | R |
| **0x0006** | System Temperature | Input | °C × 10 | Nhiệt độ hệ thống | R |
| **0x0007** | System Voltage | Input | V × 10 | Điện áp hệ thống | R |
| **0x0010** | Analog Input 1 | Input | mm | Khoảng cách cảm biến 1 (mm) | R |
| **0x0011** | Analog Input 2 | Input | mm | Khoảng cách cảm biến 2 (mm) | R |
| **0x0012** | Analog Input 3 | Input | mm | Khoảng cách cảm biến 3 (mm) | R |
| **0x0013** | Analog Input 4 | Input | mm | Khoảng cách cảm biến 4 (mm) | R |
| **0x0014** | Analog Input 1 Raw | Input | ADC | Giá trị ADC thô cảm biến 1 | R |
| **0x0015** | Analog Input 2 Raw | Input | ADC | Giá trị ADC thô cảm biến 2 | R |
| **0x0016** | Analog Input 3 Raw | Input | ADC | Giá trị ADC thô cảm biến 3 | R |
| **0x0017** | Analog Input 4 Raw | Input | ADC | Giá trị ADC thô cảm biến 4 | R |
| **0x0020** | Digital Input Status | Input | - | Trạng thái 4 digital inputs (bit field) | R |
| **0x0021** | DI1 Status | Input | - | Trạng thái DI1 (0=Clear, 1=Detected) | R |
| **0x0022** | DI2 Status | Input | - | Trạng thái DI2 (0=Clear, 1=Detected) | R |
| **0x0023** | DI3 Status | Input | - | Trạng thái DI3 (0=Clear, 1=Detected) | R |
| **0x0024** | DI4 Status | Input | - | Trạng thái DI4 (0=Clear, 1=Detected) | R |
| **0x0030** | Relay Output Control | Holding | - | Điều khiển 4 relay outputs (bit field) | R/W |
| **0x0031** | Relay 1 Control | Holding | - | Điều khiển Relay 1 (0=Off, 1=On) | R/W |
| **0x0032** | Relay 2 Control | Holding | - | Điều khiển Relay 2 (0=Off, 1=On) | R/W |
| **0x0033** | Relay 3 Control | Holding | - | Điều khiển Relay 3 (0=Off, 1=On) | R/W |
| **0x0034** | Relay 4 Control | Holding | - | Điều khiển Relay 4 (0=Off, 1=On) | R/W |
| **0x0040** | Safety Zone 1 Threshold | Holding | mm | Ngưỡng vùng an toàn 1 (mm) | R/W |
| **0x0041** | Safety Zone 2 Threshold | Holding | mm | Ngưỡng vùng an toàn 2 (mm) | R/W |
| **0x0042** | Safety Zone 3 Threshold | Holding | mm | Ngưỡng vùng an toàn 3 (mm) | R/W |
| **0x0043** | Safety Zone 4 Threshold | Holding | mm | Ngưỡng vùng an toàn 4 (mm) | R/W |
| **0x0050** | Proximity Threshold | Holding | - | Ngưỡng cảm biến tiệm cận | R/W |
| **0x0051** | Safety Response Time | Holding | ms | Thời gian phản ứng an toàn (ms) | R/W |
| **0x0052** | Auto Reset Enable | Holding | - | Bật/tắt tự động reset (0=Disable, 1=Enable) | R/W |
| **0x0053** | Safety Mode | Holding | - | Chế độ an toàn (0=Normal, 1=High, 2=Critical) | R/W |