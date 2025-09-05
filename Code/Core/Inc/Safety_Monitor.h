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
    
    /* Raw data */
    uint16_t raw_value;             // Raw ADC value (0-4095)
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
    uint8_t sensor_value;          // Sensor enable/disable flag
    /* Current state */
    uint8_t previous_state;         // Previous state for edge detection
    uint8_t debounced_state;        // Debounced state
    
    /* Configuration */
    uint8_t active_level;           // Active level (0=active low, 1=active high)
    uint16_t debounce_time_ms;      // Debounce time in milliseconds
    
    /* Edge detection */
    uint8_t rising_edge_detected;   // Rising edge flag
    uint8_t falling_edge_detected;  // Falling edge flag
    uint32_t last_edge_time;        // Last edge detection timestamp
    
    /* Status and counters */
    uint8_t sensor_status;          // Current sensor status
    uint32_t state_change_count;    // State change counter
    uint32_t error_count;           // Error counter
} Digital_Sensor_t;

/* Safety system global data structure */
typedef struct
{
    /* Sensor arrays */
    Analog_Sensor_t analog_sensors[ANALOG_SENSOR_COUNT];
    Digital_Sensor_t digital_sensors[DIGITAL_SENSOR_COUNT];
    
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

/* ========================== FUNCTION DECLARATIONS ========================== */

/* ========================== CORE FUNCTIONS - STREAMLINED ========================== */
/**
 * @brief Initialize Safety Monitor system
 * @param None
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Monitor_Init(void);

/**
 * @brief Main safety monitor processing function (called from Safety Task)
 * @param None
 * @return Safety_Monitor_Status_t Current system status
 */
Safety_Monitor_Status_t Safety_Monitor_Process(void);

/**
 * @brief Process all analog sensors (reading, filtering, threshold checking)
 * @param None
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Process_Analog_Sensors(void);

/* ========================== SENSOR ACCESS FUNCTIONS ========================== */
/**
 * @brief Get analog sensor processed value (final calibrated value)
 * @param sensor_id: Sensor ID (0-3)
 * @return uint16_t Processed sensor value (millivolts)
 */
uint16_t Safety_Get_Analog_Value(uint8_t sensor_id);

/**
 * @brief Get analog sensor voltage (in Volts)
 * @param sensor_id: Sensor ID (0-3)
 * @return float Voltage value in Volts
 */
float Safety_Get_Analog_Voltage(uint8_t sensor_id);

/**
 * @brief Get digital sensor state
 * @param sensor_id: Sensor ID (0-3)
 * @return uint8_t Current state (0 or 1)
 */
uint8_t Safety_Get_Digital_State(uint8_t sensor_id);

/* ========================== CONFIGURATION FUNCTIONS ========================== */
/**
 * @brief Set analog sensor calibration
 * @param sensor_id: Sensor ID (0-3)
 * @param offset: Calibration offset
 * @param gain: Calibration gain
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Analog_Calibration(uint8_t sensor_id, float offset, float gain);

/**
 * @brief Enable/disable analog sensor
 * @param sensor_id: Sensor ID (0-3)
 * @param enable: Enable flag (0=disable, 1=enable)
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Analog_Enable(uint8_t sensor_id, uint8_t enable);

/**
 * @brief Enable/disable digital sensor
 * @param sensor_id: Sensor ID (0-3)
 * @param enable: Enable flag (0=disable, 1=enable)
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Set_Digital_Enable(uint8_t sensor_id, uint8_t enable);

/**
 * @brief Get overall system safety status
 * @param None
 * @return Safety_Monitor_Status_t Current system status
 */
Safety_Monitor_Status_t Safety_Get_System_Status(void);

/* ========================== MODBUS INTERFACE FUNCTIONS ========================== */
/**
 * @brief Load safety parameters from Modbus registers
 * @param None
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Register_Load(void);

/**
 * @brief Save safety data to Modbus registers
 * @param None
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef Safety_Register_Save(void);

/**
 * @brief Read analog sensor value from ADC
 * @param sensor_id: Sensor ID (0-3)
 * @return uint16_t Raw ADC value (0-4095)
 */
uint16_t Safety_Read_Analog_Sensor(uint8_t sensor_id);

#endif /* SAFETY_MONITOR_H */