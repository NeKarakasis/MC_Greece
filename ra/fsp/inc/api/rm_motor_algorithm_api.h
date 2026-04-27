/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_MOTOR_INTERFACES
 * @defgroup MOTOR_ALGORITHM_API Motor Interface
 * @brief Interface for Motor Algorithm functions.
 *
 * @section MOTOR_ALGORITHM_API_Summary Summary
 * The Motor Algorithm provides Motor functionality.
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_ALGORITHM_API_H
#define RM_MOTOR_ALGORITHM_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Register definitions, common services and error codes. */
#include "rm_motor_hal_driver_api.h"
#include "rm_motor_sensor_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Control block for motor algorithm */
typedef void motor_algorithm_ctrl_t;

/** Events that can trigger a callback function */
typedef enum e_motor_algorithm_event
{
    MOTOR_ALGORITHM_EVENT_INNER = 1,   ///< Event forward current control
    MOTOR_ALGORITHM_EVENT_OUTER,       ///< Event set speed control output data
    MOTOR_ALGORITHM_EVENT_TRAJECTORY,  ///< Event set speed control trajectory data
    MOTOR_ALGORITHM_EVENT_ERROR,       ///< Event error detected
} motor_algorithm_event_t;

/** Control mode for pm foc */
typedef enum e_motor_algorithm_control_mode
{
    MOTOR_ALGORITHM_CONTROL_MODE_NONE  = 0, ///< Not configured control mode
    MOTOR_ALGORITHM_CONTROL_MODE_SPEED,     ///< Speed control mode
} motor_algorithm_control_mode_t;

typedef struct st_motor_algorithm_callback_args
{
    void const            * p_context;            ///< Pointer to the user-provided context
    motor_algorithm_event_t event;                ///< Event that triggered the callback
    uint32_t                error_info;           ///< Error code when event is MOTOR_ALGORITHM_EVENT_ERROR
    float                 * p_speed_mech_ref_rpm; ///< Speed parameter for trajectory control
    float                 * p_speed_mech_rpm;     ///< Actual speed [rpm]
} motor_algorithm_callback_args_t;

/** Motor algorithm configuration structure */
typedef struct st_motor_algorithm_cfg
{
    motor_hal_driver_instance_t const * p_motor_hal_driver_instance; ///< Motor driver access module instance
    motor_sensor_instance_t const     * p_motor_sensor_instance;     ///< Motor angle sensor module instance (Slot 1)
    motor_sensor_instance_t const     * p_motor_sensor_instance2;    ///< Motor angle sensor module instance (Slot 2)
    timer_instance_t const            * p_timer_instance;            ///< Timer instance for outer loop control

    void const * p_extend;                                           ///< Pointer to extended configuration
} motor_algorithm_cfg_t;

/** Motor algorithm API structure */
typedef struct st_motor_algorithm_api
{
    /** Initialize the motor algorithm module.
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in]  p_cfg    Pointer to configuration structure.
     */
    fsp_err_t (* open)(motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_cfg_t const * const p_cfg);

    /** Close (Finish) the motor algorithm module.
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     */
    fsp_err_t (* close)(motor_algorithm_ctrl_t * const p_ctrl);

    /** Reset(Stop) the motor algorithm module.
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     */
    fsp_err_t (* reset)(motor_algorithm_ctrl_t * const p_ctrl);

    /** Activate the motor algorithm.
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     */
    fsp_err_t (* run)(motor_algorithm_ctrl_t * const p_ctrl);

    /** Inactivate the motor algorithm.
     *
     * @param[in]  p_ctrl   Pointer to instance structure.
     */
    fsp_err_t (* stop)(motor_algorithm_ctrl_t * const p_ctrl);

    /** Get control mode
     *
     * @param[in]   p_ctrl       Pointer to instance structure.
     * @param[out]  p_control_mode   Pointer to control mode data across multiple modules.
     */
    fsp_err_t (* controlModeGet)(motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_control_mode_t * p_control_mode);

    /** Set control mode
     *
     * @param[in]  p_ctrl       Pointer to instance structure.
     * @param[in]  control_mode  Control mode value across multiple modules.
     */
    fsp_err_t (* controlModeSet)(motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_control_mode_t control_mode);

    /** Get status
     *
     * @param[in]  p_ctrl       Pointer to instance structure.
     */
    fsp_err_t (* statusGet)(motor_algorithm_ctrl_t * const p_ctrl, uint8_t * const p_status);

    /** Set configuration.
     *
     * @param[in]  p_ctrl       Pointer to instance structure.
     * @param[in]  p_cfg            Pointer to configuration structure.
     */
    fsp_err_t (* parameterUpdate)(motor_algorithm_ctrl_t * const p_ctrl, motor_algorithm_cfg_t const * const p_cfg);

    /** Get error info.
     *
     * @param[in]   p_ctrl       Pointer to instance structure.
     * @param[out]  p_error_flags    Pointer to error info.
     */
    fsp_err_t (* errorGet)(motor_algorithm_ctrl_t * const p_ctrl, uint32_t * p_error_flags);
} motor_algorithm_api_t;

/** This structure encompasses everything that is needed to use an instance of motor algorithm interface. */
typedef struct st_motor_algorithm_instance
{
    motor_algorithm_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    motor_algorithm_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    motor_algorithm_api_t const * p_api;  ///< Pointer to the API structure for this instance
} motor_algorithm_instance_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_ALGORITHM_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_ALGORITHM_API_H
