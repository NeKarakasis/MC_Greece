/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_MOTOR_INTERFACES
 * @defgroup MOTOR_TUNER_API Motor tuner Interface
 * @brief Interface for motor tuner functions.
 *
 * @section MOTOR_TUNER_API_Summary Summary
 * The Motor Tuner interface provides motor parameter estimation and tuning functionality.
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_TUNER_API_H
#define RM_MOTOR_TUNER_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdbool.h>

/* Register definitions, common services and error codes. */
#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define TUNER_VOLTAGE_ERROR_LUT_SIZE    (5) ///< Size of voltage error lookup table

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**
 * Indicates the status of Tuner.
 * Main transitions:
 * - READY      -> RUNNING   : START or RESUME command
 * - RUNNING    -> READY     : STOP command
 * - RUNNING    -> ERROR     : An error occurs
 * - RUNNING    -> COMPLETED : Identification (ID) completed
 * - ERROR      -> RESET     : RESET command
 * - ERROR      -> RUNNING   : RESUME command
 * - RESET      -> READY     : Always (returns to READY)
 * - COMPLETED  -> RUNNING   : START or RESUME command
 */
typedef enum e_motor_tuner_status
{
    MOTOR_TUNER_STATUS_READY     = 0,  ///< Ready to run (idle)
    MOTOR_TUNER_STATUS_RUNNING   = 1,  ///< Running (identification/processing in progress)
    MOTOR_TUNER_STATUS_ERROR     = 2,  ///< Error occurred
    MOTOR_TUNER_STATUS_RESET     = 3,  ///< Reset state
    MOTOR_TUNER_STATUS_COMPLETED = 4,  ///< Operation completed
} motor_tuner_status_t;

/**
 * Motor tuner parameter types.
 */
typedef enum e_motor_tuner_param_type
{
    MOTOR_TUNER_PARAM_TYPE_POLE_PAIRS       = 0, ///< Number of pole pairs
    MOTOR_TUNER_PARAM_TYPE_INERTIA          = 1, ///< Rotor inertia moment
    MOTOR_TUNER_PARAM_TYPE_VISCOUS_FRICTION = 2, ///< Viscous friction coefficient
    MOTOR_TUNER_PARAM_TYPE_RATED_CURRENT    = 3, ///< Nominal/Rated current

    MOTOR_TUNER_PARAM_TYPE_PMSM_R  = 100,        ///< Stator resistance
    MOTOR_TUNER_PARAM_TYPE_PMSM_LD = 101,        ///< d-axis inductance
    MOTOR_TUNER_PARAM_TYPE_PMSM_LQ = 102,        ///< q-axis inductance
    MOTOR_TUNER_PARAM_TYPE_PMSM_KE = 103,        ///< Back-EMF constant
} motor_tuner_param_type_t;

/**
 * Motor Tuner block.  Allocate an instance specific control block to pass into the tuner API calls.
 */
typedef void motor_tuner_ctrl_t;

/** Tuner configuration structure */
typedef struct st_motor_tuner_cfg
{
    void const * p_context;            ///< Placeholder for user data
    void const * p_extend;             ///< Placeholder for user extension
} motor_tuner_cfg_t;

/**
 * Motor tuner request types.
 */
typedef enum e_motor_tuner_request
{
    MOTOR_TUNER_REQUEST_NONE = 0,      ///< No request
    MOTOR_TUNER_REQUEST_START,         ///< Start tuner
    MOTOR_TUNER_REQUEST_STOP,          ///< Stop tuner
    MOTOR_TUNER_REQUEST_RESET,         ///< Reset tuner
} motor_tuner_request_t;

/**
 * Input signals for the motor tuner inner loop.
 */
typedef struct st_motor_tuner_in
{
    float iu;                          ///< U-phase current in SI unit
    float iv;                          ///< V-phase current in SI unit
    float iw;                          ///< W-phase current in SI unit
    float vdc;                         ///< DC bus voltage in SI unit
    float va_max;                      ///< Maximum voltage vector magnitude in the power invariant clarke transform type dq frame
} motor_tuner_in_t;

/**
 * Output signals from the motor tuner inner loop.
 */
typedef struct st_motor_tuner_out
{
    float                 vu;          ///< U-phase voltage in SI unit
    float                 vv;          ///< V-phase voltage in SI unit
    float                 vw;          ///< W-phase voltage in SI unit
    motor_tuner_request_t request;     ///< Tuner request, see @ref motor_tuner_request_t
    motor_tuner_status_t  status;      ///< Tuner status, see @ref motor_tuner_status_t
} motor_tuner_out_t;

/**
 * Voltage error lookup table structure.
 */
typedef struct st_motor_tuner_volterr_lut
{
    float current_breakpoints[TUNER_VOLTAGE_ERROR_LUT_SIZE]; ///< Current breakpoints table
    float volterr_table[TUNER_VOLTAGE_ERROR_LUT_SIZE];       ///< Voltage error table
    float ref_voltage;                                       ///< Reference voltage, DC voltage that used in the measurement [V]
} motor_tuner_volterr_lut_t;

/** Functions implemented at the HAL layer will follow this API. */
typedef struct st_motor_tuner_api
{
    /** Open tuner.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure.
     */
    fsp_err_t (* open)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_cfg_t const * const p_cfg);

    /** Process inner loop.
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in]  p_in     Pointer to input structure.
     * @param[out] p_out    Pointer to output structure.
     */
    fsp_err_t (* innerLoopProcess)(motor_tuner_ctrl_t * p_ctrl, motor_tuner_in_t * p_in, motor_tuner_out_t * p_out);

    /** Process outer loop.
     *
     * @param[in]  p_ctrl   Pointer to tuner instance.
     */
    fsp_err_t (* outerLoopProcess)(motor_tuner_ctrl_t * p_ctrl);

    /** Check if tuner is active.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_is_active  Pointer to active status flag.
     */
    fsp_err_t (* isActive)(motor_tuner_ctrl_t * const p_ctrl, bool * p_is_active);

    /**
     * Get tuner status.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_status     Pointer to status structure.
     */
    fsp_err_t (* statusGet)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_status_t * p_status);

    /**
     * Get tuner error code.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_error      Pointer to error code.
     */
    fsp_err_t (* errorGet)(motor_tuner_ctrl_t * const p_ctrl, uint16_t * p_error);

    /**
     * Get progress of parameter identification.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_progress   Pointer to progress value.
     */
    fsp_err_t (* progressGet)(motor_tuner_ctrl_t * const p_ctrl, float * p_progress);

    /**
     * Start motor tuner.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* start)(motor_tuner_ctrl_t * const p_ctrl);

    /**
     * Stop motor tuner.
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* stop)(motor_tuner_ctrl_t * const p_ctrl);

    /**
     * Reset motor tuner.
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* reset)(motor_tuner_ctrl_t * const p_ctrl);

    /**
     * Abort motor tuner operation.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  error_code   Error code to be thrown.
     */
    fsp_err_t (* abort)(motor_tuner_ctrl_t * const p_ctrl, uint8_t error_code);

    /**
     * Get identified motor parameter.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  param_type   Type of parameter to get.
     * @param[out] p_value      Pointer to store identified parameter value.
     */
    fsp_err_t (* identifiedParamGet)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_param_type_t param_type,
                                     float * p_value);

    /**
     * Get identified voltage error lookup table.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_lut        Pointer to lookup table structure.
     * @param[out] p_rdc        Pointer to store identified DC bus resistance.
     */
    fsp_err_t (* identifiedVoltageErrorGet)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_volterr_lut_t * p_lut,
                                            float * p_rdc);

    /**
     * Set known motor parameter.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  param_type   Type of parameter to set.
     * @param[in]  value        Value of the known parameter.
     */
    fsp_err_t (* knownParamSet)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_param_type_t param_type, float value);

    /**
     * Set voltage error lookup table.
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_lut        Pointer to lookup table structure.
     */
    fsp_err_t (* voltageErrorLutSet)(motor_tuner_ctrl_t * const p_ctrl, motor_tuner_volterr_lut_t * p_lut);
} motor_tuner_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_motor_tuner_instance
{
    motor_tuner_ctrl_t      * p_ctrl;  ///< Pointer to the control structure for this instance
    motor_tuner_cfg_t const * p_cfg;   ///< Pointer to the configuration structure for this instance
    motor_tuner_api_t const * p_api;   ///< Pointer to the API structure for this instance
} motor_tuner_instance_t;

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_TUNER_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RM_MOTOR_TUNER_API_H */
