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
#define OFFSET_SCALE_FACTOR     1000.0f

// Khai báo biến toàn cục
Safety_System_Data_t g_safety_system;
Analog_Sensor_t g_analog_sensors[ANALOG_SENSOR_COUNT];
Digital_Sensor_t g_digital_sensors[DIGITAL_SENSOR_COUNT];

uint16_t adc_buffer[4];

// Khởi tạo các giá trị mặc định cho các cảm biến
HAL_StatusTypeDef Safety_Monitor_Init(void){
    // Khởi tạo giá trị mặc định cho cảm biến analog
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 4);
    
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

    g_analog_sensors[0].raw_value = DEFAULT_ANALOG_INPUT_1;
    g_analog_sensors[1].raw_value = DEFAULT_ANALOG_INPUT_2; 
    g_analog_sensors[2].raw_value = DEFAULT_ANALOG_INPUT_3;
    g_analog_sensors[3].raw_value = DEFAULT_ANALOG_INPUT_4;
    
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

    // Xử lý cảm biến analog

    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        if(g_analog_sensors[i].sensor_active) {
            // Chuyển đổi giá trị ADC sang điện áp
            float voltage = (g_analog_sensors[i].raw_value * ADC_VREF) / ADC_RESOLUTION;
            
            // Áp dụng hệ số cân chỉnh
            voltage = voltage * g_analog_sensors[i].calibration_gain + 
                     g_analog_sensors[i].calibration_offset;

            // Cập nhật giá trị và thời gian
            g_analog_sensors[i].voltage = voltage;
        }
    }

    // Xử lý cảm biến digital
    for(uint8_t i = 0; i < DIGITAL_SENSOR_COUNT; i++) {
        if(g_digital_sensors[i].sensor_active) {
            uint8_t current_value = g_digital_sensors[i].sensor_value;
            
            // Phát hiện thay đổi trạng thái
            if(current_value != g_digital_sensors[i].previous_state) {
                // Kiểm tra chống dội
                if((current_time - g_digital_sensors[i].last_edge_time) > 
                   g_digital_sensors[i].debounce_time_ms) {
                    g_digital_sensors[i].state_change_count++;
                    g_digital_sensors[i].last_edge_time = current_time;
                }
            }
            
            g_digital_sensors[i].previous_state = current_value;

        }
    }
    
    return SAFETY_MONITOR_OK;
}

// Đọc cấu hình từ Modbus registers
HAL_StatusTypeDef Safety_Register_Load(void){
    // Đọc cấu hình cho cảm biến analog
    for(uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        g_analog_sensors[i].sensor_active = Modbus_Get_Register(REG_ANALOG_1_ENABLE + i);
        g_analog_sensors[i].calibration_gain = 
            (float)Modbus_Get_Register(REG_ANALOG_COEFFICIENT) / 1000.0f;
        g_analog_sensors[i].calibration_offset = 
            (float)Modbus_Get_Register(REG_ANALOG_CALIBRATION) / OFFSET_SCALE_FACTOR;
    }
    
    // Đọc cấu hình cho cảm biến digital
    for(uint8_t i = 0; i < MAX_DIGITAL_SENSORS; i++) {
        g_digital_sensors[i].sensor_active = Modbus_Get_Register(REG_DI1_ENABLE + i);
        g_digital_sensors[i].debounce_time_ms = Modbus_Get_Register(REG_DI1_DEBOUNCE + i);
    }
    
    return HAL_OK;
}

// Lưu dữ liệu vào Modbus registers
HAL_StatusTypeDef Safety_Register_Save(void){
    // MODIFICATION LOG
    // Date: 2025-01-14
    // Changed by: AI Agent  
    // Description: Modified to save only processed analog sensor values
    // Reason: User requirement - only save converted sensor values, not raw voltages
    // Impact: Modbus registers contain calibrated and processed sensor data
    // Testing: Verify processed values are correctly scaled and stored
    
    // Lưu dữ liệu cảm biến analog (chỉ giá trị đã chuyển đổi và hiệu chuẩn)
    for(uint8_t i = 0; i < MAX_ANALOG_SENSORS; i++) {
        // Lưu giá trị cảm biến đã được xử lý, áp dụng hiệu chuẩn và chuyển đổi đơn vị
        // Giá trị này đã bao gồm: ADC -> voltage -> calibration (gain + offset)
        // Chuyển đổi từ float (volts) sang uint16_t (millivolts) cho Modbus
        uint16_t processed_value = (uint16_t)(g_analog_sensors[i].voltage * VOLTAGE_SCALE_FACTOR);
        
        Modbus_Set_Register(REG_ANALOG_INPUT_1 + i, processed_value);
        Modbus_Set_Register(REG_ANALOG_1_ENABLE + i, g_analog_sensors[i].sensor_active);
        
        // Lưu giá trị ADC thô để debug (tùy chọn)
        Modbus_Set_Register(REG_ANALOG_INPUT_1_RAW + i, g_analog_sensors[i].raw_value);
        
        // Lưu trạng thái sensor và error count
        Modbus_Set_Register(REG_ANALOG_1_ERROR + i, g_analog_sensors[i].error_count);
    }
    
    // Lưu dữ liệu cảm biến digital
    for(uint8_t i = 0; i < MAX_DIGITAL_SENSORS; i++) {
        Modbus_Set_Register(REG_DI1_STATUS + i, g_digital_sensors[i].sensor_value);
        Modbus_Set_Register(REG_DI1_ENABLE + i, g_digital_sensors[i].sensor_active);
        Modbus_Set_Register(REG_DI1_CHANGES + i, g_digital_sensors[i].state_change_count);
        Modbus_Set_Register(REG_DI1_ERROR + i, g_digital_sensors[i].error_count);
    }
    
    return HAL_OK;
}

float Safety_Get_Analog_Voltage(uint8_t sensor_id){
    if(sensor_id >= ANALOG_SENSOR_COUNT) {
        return 0;
    }
    return g_analog_sensors[sensor_id].voltage;
}

/**
 * @brief Get processed analog sensor value (after calibration)
 * @param sensor_id: Sensor ID (0-3)
 * @return uint16_t Processed sensor value (scaled for Modbus)
 * @note Trả về giá trị cảm biến đã được xử lý và hiệu chuẩn
 */
uint16_t Safety_Get_Analog_Value(uint8_t sensor_id){
    if(sensor_id >= ANALOG_SENSOR_COUNT) {
        return 0;
    }
    
    // Trả về giá trị đã được xử lý (voltage * scale factor)
    // Đây là giá trị cuối cùng được lưu vào Modbus register
    return (uint16_t)(g_analog_sensors[sensor_id].voltage * VOLTAGE_SCALE_FACTOR);
}

uint8_t Safety_Get_Digital_State(uint8_t sensor_id){
    if(sensor_id >= DIGITAL_SENSOR_COUNT) {
        return 0;
    }
    return g_digital_sensors[sensor_id].sensor_value;
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
uint16_t Safety_Read_Analog_Sensor(uint8_t sensor_id)
{
    uint16_t adc_value = 0;
    uint32_t channel = 0;
    HAL_StatusTypeDef status = HAL_ERROR;
    uint32_t timeout_start = 0;
    
    // Kiểm tra tính hợp lệ của sensor_id
    if (sensor_id >= ANALOG_SENSOR_COUNT) {
        g_analog_sensors[0].error_count++;  // Log error to first sensor if invalid ID
        return 0;
    }
    
    // Kiểm tra trạng thái hoạt động của sensor
    if (!g_analog_sensors[sensor_id].sensor_active) {
        return g_analog_sensors[sensor_id].raw_value;  // Return last known value
    }
    
    // Map sensor_id to ADC channel
    switch (sensor_id) {
        case 0: channel = ADC_CHANNEL_0; break;  // PA0 - Analog Input 1
        case 1: channel = ADC_CHANNEL_1; break;  // PA1 - Analog Input 2  
        case 2: channel = ADC_CHANNEL_4; break;  // PA4 - Analog Input 3
        case 3: channel = ADC_CHANNEL_8; break;  // PB0 - Analog Input 4
        default:
            g_analog_sensors[sensor_id].error_count++;
            return 0;
    }
    
    // Configure ADC channel
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;  // Longer sampling for better accuracy
    
    status = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    if (status != HAL_OK) {
        g_analog_sensors[sensor_id].error_count++;
        return g_analog_sensors[sensor_id].raw_value;  // Return last known value
    }
    
    // Start ADC conversion
    timeout_start = HAL_GetTick();
    status = HAL_ADC_Start(&hadc1);
    if (status != HAL_OK) {
        g_analog_sensors[sensor_id].error_count++;
        return g_analog_sensors[sensor_id].raw_value;
    }
    
    // Wait for conversion complete with timeout
    status = HAL_ADC_PollForConversion(&hadc1, 100);  // 100ms timeout
    if (status == HAL_OK) {
        // Get ADC value
        adc_value = HAL_ADC_GetValue(&hadc1);
        
        // Apply simple digital filter (moving average)
        if (g_analog_sensors[sensor_id].raw_value == 0) {
            // First reading - no filtering
            g_analog_sensors[sensor_id].filtered_value = (float)adc_value;
        } else {
            // Apply low-pass filter: filtered = 0.8 * old + 0.2 * new
            g_analog_sensors[sensor_id].filtered_value = 
                0.8f * g_analog_sensors[sensor_id].filtered_value + 0.2f * (float)adc_value;
        }
        
        // Update raw value
        g_analog_sensors[sensor_id].raw_value = adc_value;
        
        // Update min/max statistics
        float voltage = (adc_value * ADC_VREF) / ADC_RESOLUTION;
        if (g_analog_sensors[sensor_id].min_recorded == 0.0f || voltage < g_analog_sensors[sensor_id].min_recorded) {
            g_analog_sensors[sensor_id].min_recorded = voltage;
        }
        if (voltage > g_analog_sensors[sensor_id].max_recorded) {
            g_analog_sensors[sensor_id].max_recorded = voltage;
        }
        
    } else {
        // Timeout or error occurred
        g_analog_sensors[sensor_id].error_count++;
        adc_value = g_analog_sensors[sensor_id].raw_value;  // Return last known value
    }
    
    // Stop ADC
    HAL_ADC_Stop(&hadc1);
    
    return adc_value;
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
    
    // Process each analog sensor
    for (uint8_t i = 0; i < ANALOG_SENSOR_COUNT; i++) {
        if (g_analog_sensors[i].sensor_active) {
            // Read sensor value
            uint16_t raw_value = Safety_Read_Analog_Sensor(i);
            
            // Convert to voltage with calibration
            float voltage = (raw_value * ADC_VREF) / ADC_RESOLUTION;
            voltage = voltage * g_analog_sensors[i].calibration_gain + 
                     g_analog_sensors[i].calibration_offset;
            
            // Update sensor data
            g_analog_sensors[i].voltage = voltage;
            
            // Check thresholds and set status flags
            g_analog_sensors[i].sensor_status = SENSOR_STATUS_OK;
            g_analog_sensors[i].alarm_flags = 0;
            
            // Check critical thresholds first
            if (voltage <= g_analog_sensors[i].critical_low || 
                voltage >= g_analog_sensors[i].critical_high) {
                g_analog_sensors[i].sensor_status |= SENSOR_STATUS_CRITICAL;
                g_analog_sensors[i].alarm_flags |= 0x04;  // Critical alarm
                overall_status = HAL_ERROR;
            }
            // Check warning thresholds  
            else if (voltage <= g_analog_sensors[i].warning_low || 
                     voltage >= g_analog_sensors[i].warning_high) {
                g_analog_sensors[i].sensor_status |= SENSOR_STATUS_WARNING;
                g_analog_sensors[i].alarm_flags |= 0x02;  // Warning alarm
            }
            
            // Check for sensor errors (invalid readings)
            if (raw_value == 0 || raw_value >= 4095) {
                g_analog_sensors[i].sensor_status |= SENSOR_STATUS_ERROR;
                g_analog_sensors[i].alarm_flags |= 0x08;  // Error alarm
                overall_status = HAL_ERROR;
            }
        }
    }
    
    return overall_status;
}

HAL_StatusTypeDef Safety_Set_Analog_Calibration(uint8_t sensor_id, float offset, float gain){

}

HAL_StatusTypeDef Safety_Set_Analog_Enable(uint8_t sensor_id, uint8_t enable){

}

HAL_StatusTypeDef Safety_Set_Digital_Enable(uint8_t sensor_id, uint8_t enable){

}

