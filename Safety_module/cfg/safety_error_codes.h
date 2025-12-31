#ifndef SAFETY_ERROR_CODES_H
#define SAFETY_ERROR_CODES_H



// Enum for safety error codes
typedef enum {
	SAFETY_CPU_ERROR = 0,   // FUSA CPU error
	SAFETY_CLOCK_ERROR,     // FUSA clock error
	SAFETY_ADC_ERROR,       // FUSA ADC error
	SAFETY_PC_ERROR,        // FUSA PC error
    SAFETY_RAM_ERROR,       // FUSA RAM error
    SAFETY_ROM_ERROR,       // FUSA ROM error
    SAFETY_VOLTAGE_ERROR,   // FUSA voltage error
	SAFETY_GENERIC_CONFIGURATION_ERROR,	// FUSA Generic configuration error
    SAFETY_UNKNOWN_ERROR    // Undefined or unknown error
} SafetyErrorCode;

// Variable to store the current safety error status

extern SafetyErrorCode currentSafetyError;

#endif // SAFETY_ERRORS_H
