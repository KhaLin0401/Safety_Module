#include "Safety_Monitor.h"

// MODIFICATION LOG
// Date: 2025-01-14 
// Changed by: AI Agent
// Description: Added external ADC handle declaration and missing constants
// Reason: Need access to ADC peripheral for analog sensor reading
// Impact: Enables proper analog sensor data acquisition
// Testing: Verify ADC reading functionality with all 4 channels

// External ADC handle from main.c
extern ADC_HandleTypeDef hadc1;

// Missing constants for proper compilation
#define MAX_ANALOG_SENSORS      4
#define MAX_DIGITAL_SENSORS     4
#define VOLTAGE_SCALE_FACTOR    1000.0f
#define OFFSET_SCALE_FACTOR     100.0f

// Khai báo biến toàn cục
Safety_System_Data_t g_safety_system;
Analog_Sensor_t g_analog_sensors[ANALOG_SENSOR_COUNT];
Digital_Sensor_t g_digital_sensors[DIGITAL_SENSOR_COUNT];

volatile uint16_t adc_buffer[4];

// Khởi tạo các giá trị mặc định cho các cảm biến
HAL_StatusTypeDef Safety_Monitor_Init(void){
    // Hiệu chuẩn ADC trước khi bắt đầu DMA để đảm bảo độ chính xác
    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        return HAL_ERROR;
    }
    // Bắt đầu chuyển đổi ADC ở chế độ quét 4 kênh với DMA vòng
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 4) != HAL_OK) {
        return HAL_ERROR;
    }
    // Khởi tạo trạng thái hoạt động cho cảm biến analog
    g_analog_sensors[0].sensor_active = DEFAULT_ANALOG_1_ENABLE;
    g_analog_sensors[1].sensor_active = DEFAULT_ANALOG_2_ENABLE;
    g_analog_sensors[2].sensor_active = DEFAULT_ANALOG_3_ENABLE;
    g_analog_sensors[3].sensor_active = DEFAULT_ANALOG_4_ENABLE;

    // Khởi tạo các thông số cân chỉnh cho cảm biến analog
    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        g_analog_sensors[i].calibration_gain = DEFAULT_ANALOG_COEFFICIENT;
        g_analog_sensors[i].calibration_offset = DEFAULT_ANALOG_CALIBRATION;
        g_analog_sensors[i].error_count = 0;
    }

    // Khởi tạo giá trị mặc định cho cảm biến digital  
    g_digital_sensors[0].sensor_value = DEFAULT_DI1_STATUS;
    g_digital_sensors[1].sensor_value = DEFAULT_DI2_STATUS;
    g_digital_sensors[2].sensor_value = DEFAULT_DI3_STATUS;
    g_digital_sensors[3].sensor_value = DEFAULT_DI4_STATUS;

    // Khởi tạo trạng thái hoạt động cho cảm biến digital
    g_digital_sensors[0].sensor_active = DEFAULT_DI1_ENABLE;
    g_digital_sensors[1].sensor_active = DEFAULT_DI2_ENABLE;
    g_digital_sensors[2].sensor_active = DEFAULT_DI3_ENABLE;
    g_digital_sensors[3].sensor_active = DEFAULT_DI4_ENABLE;

    // Khởi tạo các thông số bổ sung cho cảm biến digital
    for(uint8_t i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        g_digital_sensors[i].error_count = 0;
        g_digital_sensors[i].state_change_count = 0;
        g_digital_sensors[i].last_edge_time = 0;
        g_digital_sensors[i].previous_state = 0;
        g_digital_sensors[i].debounced_state = 0;
        g_digital_sensors[i].debounce_time_ms = DEFAULT_SAFETY_RESPONSE_TIME;
    }
    
    return HAL_OK;
}

// Xử lý dữ liệu từ các cảm biến
Safety_Monitor_Status_t Safety_Monitor_Process(void){
    uint32_t current_time = HAL_GetTick();
    Safety_Monitor_Status_t system_status = SAFETY_MONITOR_OK;

    // Xử lý tất cả các cảm biến
    Safety_Process_Analog_Sensors();
    Safety_Process_Digital_Sensors();

    // Kiểm tra trạng thái của các cảm biến analog
    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        if(g_analog_sensors[i].sensor_active) {
            // Kiểm tra theo thứ tự ưu tiên từ cao đến thấp
            if(g_analog_sensors[i].sensor_status == SENSOR_STATUS_CRITICAL) {
                system_status = SAFETY_MONITOR_CRITICAL;
                g_safety_system.system_status = SAFETY_MONITOR_CRITICAL;
                break; // Thoát ngay khi phát hiện lỗi nghiêm trọng
            }
            else if(g_analog_sensors[i].sensor_status == SENSOR_STATUS_WARNING && 
                    g_safety_system.system_status != SAFETY_MONITOR_CRITICAL) {
                system_status = SAFETY_MONITOR_WARNING;
                g_safety_system.system_status = SAFETY_MONITOR_WARNING;
            }
            else if(g_analog_sensors[i].sensor_status == SENSOR_STATUS_ERROR && 
                    g_safety_system.system_status < SAFETY_MONITOR_CRITICAL) {
                system_status = SAFETY_MONITOR_ERROR;
                g_safety_system.system_status = SAFETY_MONITOR_ERROR;
            }
        }
    }

    // Kiểm tra trạng thái của các cảm biến digital 
    for(uint8_t i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        if(g_digital_sensors[i].sensor_active) {
            if(g_digital_sensors[i].sensor_status == SENSOR_STATUS_CRITICAL) {
                system_status = SAFETY_MONITOR_CRITICAL;
                g_safety_system.system_status = SAFETY_MONITOR_CRITICAL;
                break;
            }
            else if(g_digital_sensors[i].sensor_status == SENSOR_STATUS_WARNING && 
                    g_safety_system.system_status != SAFETY_MONITOR_CRITICAL) {
                system_status = SAFETY_MONITOR_WARNING;  
                g_safety_system.system_status = SAFETY_MONITOR_WARNING;  
            }
            else if(g_digital_sensors[i].sensor_status == SENSOR_STATUS_ERROR && 
                    g_safety_system.system_status < SAFETY_MONITOR_CRITICAL) {
                system_status = SAFETY_MONITOR_ERROR;
                g_safety_system.system_status = SAFETY_MONITOR_ERROR;
            }
        }
    }

    // Cập nhật trạng thái hệ thống
    g_safety_system.last_safety_check = current_time;

    // Cập nhật bộ đếm cảnh báo
    if(system_status == SAFETY_MONITOR_WARNING) {
        g_safety_system.warning_count++;
    }
    else if(system_status == SAFETY_MONITOR_CRITICAL) {
        g_safety_system.critical_count++;
    }
    else if(system_status == SAFETY_MONITOR_EMERGENCY) {
        g_safety_system.emergency_count++;
    }
    
    if(system_status == SAFETY_MONITOR_CRITICAL) { 
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        g_holdingRegisters[REG_RESET_FLAG] = 1;
    }
    else if(system_status == SAFETY_MONITOR_OK 
        && g_holdingRegisters[REG_RESET_FLAG] == 0) {
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    }
    g_safety_system.system_status = system_status;

    return system_status;
}

// Đọc cấu hình từ Modbus registers
HAL_StatusTypeDef Safety_Register_Load(void){
    // Đọc cấu hình cho cảm biến analog
    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        g_analog_sensors[i].sensor_active = g_holdingRegisters[REG_ANALOG_1_ENABLE + i];
        g_analog_sensors[i].calibration_gain = 
            (float)g_holdingRegisters[REG_ANALOG_COEFFICIENT];
        g_analog_sensors[i].calibration_offset = 
            (float)g_holdingRegisters[REG_ANALOG_CALIBRATION];
    }
    
    // Đọc cấu hình cho cảm biến digital
    for(uint8_t i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        g_digital_sensors[i].sensor_active = g_holdingRegisters[REG_DI1_ENABLE + i];
        g_digital_sensors[i].active_level = g_holdingRegisters[REG_DI1_ACTIVE_LEVEL + i];
        g_digital_sensors[i].debounce_time_ms = DEFAULT_SAFETY_RESPONSE_TIME;
    }
    return HAL_OK;

}

// Lưu dữ liệu vào Modbus registers
HAL_StatusTypeDef Safety_Register_Save(void) {

    // Lưu dữ liệu cảm biến analog
    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        // Lưu giá trị điện áp đã được xử lý (mV)
        g_holdingRegisters[REG_ANALOG_INPUT_1 + i] = 
            (uint16_t)(g_analog_sensors[i].filtered_value);
        
        // Lưu trạng thái kích hoạt của cảm biến
        g_holdingRegisters[REG_ANALOG_1_ENABLE + i] = 
            g_analog_sensors[i].sensor_active;
    }
    
    // Lưu dữ liệu cảm biến digital 
    for(uint8_t i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        // Lưu trạng thái và cấu hình của cảm biến digital
        g_holdingRegisters[REG_DI1_STATUS + i] = 
            g_digital_sensors[i].sensor_state;
        g_holdingRegisters[REG_DI1_ENABLE + i] = 
            g_digital_sensors[i].sensor_active;
    }
    if(g_holdingRegisters[REG_RESET_FLAG] == 1) {
        g_holdingRegisters[REG_SAFETY_SYSTEM_STATUS] = SAFETY_MONITOR_CRITICAL;
    }
    else {
        g_holdingRegisters[REG_SAFETY_SYSTEM_STATUS] = g_safety_system.system_status;
    }
    g_holdingRegisters[REG_CONFIG_BAUDRATE] = current_baudrate;
    return HAL_OK;
}


uint8_t Safety_Get_Digital_State(uint8_t sensor_id){
    
    switch (sensor_id)
    {
    case 0:
        g_digital_sensors[0].sensor_value = HAL_GPIO_ReadPin(DI1_GPIO_Port, DI1_Pin);
        return g_digital_sensors[0].sensor_value;
        break;
    case 1:
        g_digital_sensors[1].sensor_value = HAL_GPIO_ReadPin(DI2_GPIO_Port, DI2_Pin);
        return g_digital_sensors[1].sensor_value;
        break;
    case 2:
        g_digital_sensors[2].sensor_value = HAL_GPIO_ReadPin(DI3_GPIO_Port, DI3_Pin);
        return g_digital_sensors[2].sensor_value;
        break;
    case 3:
        g_digital_sensors[3].sensor_value = HAL_GPIO_ReadPin(DI4_GPIO_Port, DI4_Pin);
        return g_digital_sensors[3].sensor_value;
        break;
    default:
        return 0;
    }
}

Safety_Monitor_Status_t Safety_Get_System_Status(void){
    return g_safety_system.system_status;
}

/**
 * @brief Read analog sensor value from ADC
 * @param sensor_id: Sensor ID (0-3) 
 * @return uint16_t Raw ADC value (0-4095)
 * @note Đọc giá trị cảm biến analog từ ADC với xử lý lỗi toàn diện
 */

float Safety_Convert_To_Distance(uint8_t sensor_id){
    float distance, voltage;
    voltage = adc_buffer[sensor_id] * 3.3f / 4095.0f;
    if(voltage < 0.1f) return 0;

    distance = g_analog_sensors[sensor_id].calibration_gain/100.0f * powf(voltage, (g_analog_sensors[sensor_id].calibration_offset)/(-100.0f));
    g_analog_sensors[sensor_id].filtered_value = distance;
    return distance;
}

/**
 * @brief Process all analog sensors with comprehensive error handling
 * @param None
 * @return HAL_StatusTypeDef Overall processing status
 */
HAL_StatusTypeDef Safety_Process_Analog_Sensors(void)
{
    HAL_StatusTypeDef overall_status = HAL_OK;
    uint32_t current_time = HAL_GetTick();
    uint16_t distance;
    uint8_t i;
    
    // Process each analog sensor
    for (i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        if (g_analog_sensors[i].sensor_active) {
            // Read sensor value
            distance = Safety_Convert_To_Distance(i);
            
            // Kiểm tra các ngưỡng khoảng cách cho từng cảm biến
            if(distance == 0) {
                // Cảm biến không hoạt động hoặc lỗi
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_ERROR;
                g_analog_sensors[i].alarm_flags = 0x01;
            }
            else if(distance <= g_holdingRegisters[REG_SAFETY_ZONE1_THRESHOLD]) {
                // Vùng nguy hiểm 1 - Nguy hiểm cao nhất
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_CRITICAL;
                g_analog_sensors[i].alarm_flags = 0x08;
            }
            else if(distance <= g_holdingRegisters[REG_SAFETY_ZONE2_THRESHOLD]) {
                // Vùng nguy hiểm 2 - Cảnh báo cao
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_WARNING;
                g_analog_sensors[i].alarm_flags = 0x04;
            }
            else if(distance <= g_holdingRegisters[REG_SAFETY_ZONE3_THRESHOLD]) {
                // Vùng nguy hiểm 3 - Cảnh báo trung bình
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_WARNING;
                g_analog_sensors[i].alarm_flags = 0x02;
            }
            else if(distance <= g_holdingRegisters[REG_SAFETY_ZONE4_THRESHOLD]) {
                // Vùng nguy hiểm 4 - Cảnh báo thấp
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_OK;
                g_analog_sensors[i].alarm_flags = 0x01;
            }
            else {
                // Khoảng cách an toàn
                g_analog_sensors[i].sensor_status = SENSOR_STATUS_OK;
                g_analog_sensors[i].alarm_flags = 0;
            }
        }
    }
    
    return overall_status;
}

HAL_StatusTypeDef Safety_Process_Digital_Sensors(void){
    HAL_StatusTypeDef overall_status = HAL_OK;
    uint32_t current_time = HAL_GetTick();
    uint8_t i;
    
    for (i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        if (g_digital_sensors[i].sensor_active) {
            // Read sensor value
            g_digital_sensors[i].sensor_value = Safety_Get_Digital_State(i);
            if(g_digital_sensors[i].sensor_value == g_digital_sensors[i].active_level) {
                g_digital_sensors[i].sensor_status = SENSOR_STATUS_CRITICAL;
                g_digital_sensors[i].sensor_state = 1;
                g_digital_sensors[i].alarm_flags = 0x08;
            }
            else {
                g_digital_sensors[i].sensor_status = SENSOR_STATUS_OK;
                g_digital_sensors[i].sensor_state = 0;
                g_digital_sensors[i].alarm_flags = 0;
            }
        }
    }
    
    return overall_status;
}


