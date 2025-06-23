#ifndef SAFETY_ERROR_CODES_H
#define SAFETY_ERROR_CODES_H



// Enum for safety error codes
typedef enum {
    SAFETY_OK = 0,               // No error
    SAFETY_OVERHEAT,            // Overheating condition
    SAFETY_LOW_PRESSURE,        // Pressure below safety threshold
    SAFETY_HIGH_VOLTAGE,        // Voltage exceeds limit
    SAFETY_SENSOR_FAILURE,      // Sensor is not responding or gives bad data
    SAFETY_EMERGENCY_STOP,      // Emergency stop has been triggered
    SAFETY_COMMUNICATION_LOSS, // Communication with safety controller lost
    SAFETY_UNKNOWN_ERROR        // Undefined or unknown error
} SafetyErrorCode;

// Variable to store the current safety error status
extern SafetyErrorCode currentSafetyError;

#endif // SAFETY_ERRORS_H
