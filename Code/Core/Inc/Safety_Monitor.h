#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include "main.h"
#include "UartModbus.h"
#include "ModbusMap.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* ========================== CONSTANTS & DEFINITIONS ========================== */
#define ANALOG_SENSOR_COUNT         4
#define DIGITAL_SENSOR_COUNT        4
#define SENSOR_NAME_LENGTH          16
#define ADC_RESOLUTION              4096.0f
#define ADC_VREF                    3.3f

/* Sensor status flags */
#define SENSOR_STATUS_OK            0x00
#define SENSOR_STATUS_WARNING       0x01
#define SENSOR_STATUS_CRITICAL      0x02
#define SENSOR_STATUS_ERROR         0x04

/* Safety system status */
typedef enum
{
    SAFETY_MONITOR_OK = 0,          // All systems normal
    SAFETY_MONITOR_WARNING = 1,     // Warning condition detected
    SAFETY_MONITOR_CRITICAL = 2,    // Critical condition - action required
    SAFETY_MONITOR_EMERGENCY = 3,   // Emergency stop condition
    SAFETY_MONITOR_ERROR = 4        // System error
} Safety_Monitor_Status_t;

/* Analog sensor configuration and data */
typedef struct
{
    uint8_t sensor_id;              // Sensor identifier (0-3)
    uint8_t sensor_active;          // Sensor enable/disable flag
    
    
    float voltage;
    float filtered_value;           // Filtered value
    
    /* Configuration */
    float min_range;
    float max_range;
    float warning_low;              // Warning low threshold
    float warning_high;
    float critical_low;
    float critical_high;
    
    /* Calibration */
    float calibration_offset;       // Calibration offset
    float calibration_gain;         // Calibration gain
    
    /* Status and alarms */
    uint8_t sensor_status;          // Current sensor status
    uint8_t alarm_flags;            // Alarm condition flags    
    /* Statistics */
    float min_recorded;             // Minimum recorded value
    float max_recorded;             // Maximum recorded value
    uint32_t error_count;           // Error counter
} Analog_Sensor_t;

/* Digital sensor configuration and data */
typedef struct
{
    uint8_t sensor_id;              // Sensor identifier (0-3)
    uint8_t sensor_active;   
    uint8_t sensor_value;  
            // Sensor enable/disable flag
    /* Current state */
    uint8_t previous_state;         // Previous state for edge detection
    uint8_t debounced_state;        // Debounced state
    
    /* Configuration */
    uint8_t active_level;   
    uint8_t sensor_state;        // Active level (0=active low, 1=active high)
    uint16_t debounce_time_ms;      // Debounce time in milliseconds
    
    /* Edge detection */
    uint8_t rising_edge_detected;   // Rising edge flag
    uint8_t falling_edge_detected;  // Falling edge flag
    uint32_t last_edge_time;        // Last edge detection timestamp
    
    /* Status and counters */
    uint8_t sensor_status;          // Current sensor status
    uint32_t state_change_count;    // State change counter
    uint32_t error_count;           // Error counter
    uint8_t alarm_flags;            // Alarm condition flags
} Digital_Sensor_t;

/* Safety system global data structure */
typedef struct
{    
    /* System status */
    Safety_Monitor_Status_t system_status;
    uint8_t emergency_stop_active;
    uint32_t system_uptime;
    uint32_t last_safety_check;
    
    /* Statistics */
    uint32_t warning_count;
    uint32_t critical_count;
    uint32_t emergency_count;
    
    /* Synchronization */
    SemaphoreHandle_t data_mutex;
    
} Safety_System_Data_t;

/* ========================== GLOBAL VARIABLES ========================== */
extern Safety_System_Data_t g_safety_system;
extern Analog_Sensor_t g_analog_sensors[ANALOG_SENSOR_COUNT];
extern Digital_Sensor_t g_digital_sensors[DIGITAL_SENSOR_COUNT];

/* ========================== FUNCTION DECLARATIONS ========================== */

/* ========================== CÁC HÀM CHÍNH - TỐI ƯU HÓA ========================== */
/**
 * @brief Khởi tạo hệ thống giám sát an toàn
 * @param Không có
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Monitor_Init(void);

/**
 * @brief Hàm xử lý giám sát an toàn chính (được gọi từ Safety Task)
 * @param Không có
 * @return Safety_Monitor_Status_t Trạng thái hệ thống hiện tại
 */
Safety_Monitor_Status_t Safety_Monitor_Process(void);

/**
 * @brief Xử lý tất cả các cảm biến tương tự (đọc, lọc, kiểm tra ngưỡng)
 * @param Không có
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Process_Analog_Sensors(void);

/**
 * @brief Xử lý tất cả các cảm biến số (đọc, lọc, kiểm tra ngưỡng)
 * @param Không có
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Process_Digital_Sensors(void);

/* ========================== CÁC HÀM TRUY CẬP CẢM BIẾN ========================== */
/**
 * @brief Lấy giá trị đã xử lý của cảm biến tương tự (giá trị cuối cùng đã hiệu chuẩn)
 * @param sensor_id: ID cảm biến (0-3)
 * @return uint16_t Giá trị cảm biến đã xử lý (millivolt)
 */
uint16_t Safety_Get_Analog_Value(uint8_t sensor_id);

/**
 * @brief Lấy điện áp cảm biến tương tự (đơn vị Volt)
 * @param sensor_id: ID cảm biến (0-3)
 * @return float Giá trị điện áp tính bằng Volt
 */
float Safety_Get_Analog_Voltage(uint8_t sensor_id);

/**
 * @brief Lấy trạng thái cảm biến số
 * @param sensor_id: ID cảm biến (0-3)
 * @return uint8_t Trạng thái hiện tại (0 hoặc 1)
 */
uint8_t Safety_Get_Digital_State(uint8_t sensor_id);

/* ========================== CÁC HÀM CẤU HÌNH ========================== */
/**
 * @brief Thiết lập hiệu chuẩn cảm biến tương tự
 * @param sensor_id: ID cảm biến (0-3)
 * @param offset: Độ lệch hiệu chuẩn
 * @param gain: Hệ số khuếch đại hiệu chuẩn
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Analog_Calibration(uint8_t sensor_id, float offset, float gain);

/**
 * @brief Bật/tắt cảm biến tương tự
 * @param sensor_id: ID cảm biến (0-3)
 * @param enable: Cờ bật/tắt (0=tắt, 1=bật)
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Analog_Enable(uint8_t sensor_id, uint8_t enable);

/**
 * @brief Bật/tắt cảm biến số
 * @param sensor_id: ID cảm biến (0-3)
 * @param enable: Cờ bật/tắt (0=tắt, 1=bật)
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Digital_Enable(uint8_t sensor_id, uint8_t enable);

/**
 * @brief Lấy trạng thái an toàn tổng thể của hệ thống
 * @param Không có
 * @return Safety_Monitor_Status_t Trạng thái hệ thống hiện tại
 */
Safety_Monitor_Status_t Safety_Get_System_Status(void);

/* ========================== CÁC HÀM GIAO TIẾP MODBUS ========================== */
/**
 * @brief Tải tham số an toàn từ thanh ghi Modbus
 * @param Không có
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Register_Load(void);

/**
 * @brief Lưu dữ liệu an toàn vào thanh ghi Modbus
 * @param Không có
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Register_Save(void);

/**
 * @brief Chuyển đổi giá trị cảm biến sang khoảng cách
 * @param sensor_id: ID cảm biến (0-3)
 * @return float Giá trị khoảng cách
 */
float Safety_Convert_To_Distance(uint8_t sensor_id);

#endif /* SAFETY_MONITOR_H */