/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_MOTOR_INTERFACES
 * @defgroup MOTOR_TRAJECTORY_API Motor Trajectory Interface
 * @brief Interface for motor trajectory functions.
 *
 * @section MOTOR_TRAJECTORY_API_Summary Summary
 * The Motor trajectory interface provides Motor functionality.
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_TRAJECTORY_API_H
#define RM_MOTOR_TRAJECTORY_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Register definitions, common services and error codes. */
#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_TRAJECTORY_COMMAND_STATUS_FLAG_NONE         (0U)
#define MOTOR_TRAJECTORY_COMMAND_STATUS_FLAG_IMMEDIATE    (1U << 0)

/* Trajectory params Version Definitions */
#define MOTOR_TRAJECTORY_PARAMS_VERSION_1      (1U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Trajectory execution state */
typedef enum e_motor_trajectory_state
{
    MOTOR_TRAJECTORY_STATE_IDLE = 0,         ///< Trajectory inactive
    MOTOR_TRAJECTORY_STATE_IN_PROGRESS,      ///< Trajectory in execution
    MOTOR_TRAJECTORY_STATE_QUEUED            ///< New command is queued
} motor_trajectory_state_t;

/**  Type Identifier Definition */
typedef enum e_motor_trajectory_type_id
{
    MOTOR_TRAJECTORY_TYPE_ID_LINEAR    = 0x4C494E52U, ///< "LINR" in ASCII
    MOTOR_TRAJECTORY_TYPE_ID_TRAPEZOID = 0x54524150U, ///< "TRAP" in ASCII
    MOTOR_TRAJECTORY_TYPE_ID_POSITION  = 0x504F5349U, ///< "POSI" in ASCII
    MOTOR_TRAJECTORY_TYPE_ID_SINUSOID  = 0x53494E55U, ///< "SINU" in ASCII
    MOTOR_TRAJECTORY_TYPE_ID_SCURVE    = 0x53435552U, ///< "SCUR" in ASCII
    MOTOR_TRAJECTORY_TYPE_ID_HOMING    = 0x484F4D49U, ///< "HOMI" in ASCII
} motor_trajectory_type_id_t;

/** Bypass mode */
typedef enum e_motor_trajectory_bypass_mode
{
    MOTOR_TRAJECTORY_BYPASS_MODE_DISABLE = 0U, ///< Disable bypass mode.
    MOTOR_TRAJECTORY_BYPASS_MODE_ENABLE  = 1U, ///< Enable bypass mode.
} motor_trajectory_bypass_mode_t;

/** Command flags */
typedef uint32_t motor_trajectory_command_flags_t;

/** Command status */
typedef enum e_motor_trajectory_command_status
{
    MOTOR_TRAJECTORY_COMMAND_STATUS_ACCEPTED_IMMEDIATE = 0, ///< New command applied immediately
    MOTOR_TRAJECTORY_COMMAND_STATUS_QUEUED,                 ///< Command queued
    MOTOR_TRAJECTORY_COMMAND_STATUS_REJECTED                ///< Command rejected
} motor_trajectory_command_status_t;

/** Error information */
typedef enum e_motor_trajectory_error
{
    MOTOR_TRAJECTORY_ERROR_NONE    = 0x0000,
    MOTOR_TRAJECTORY_ERROR_UNKNOWN = 0xFFFF,
} motor_trajectory_error_t;

/** Acceleration / Deceleration operation status */
typedef enum e_motor_trajectory_accel_state
{
    MOTOR_TRAJECTORY_ACCEL_STATE_ACCELERATING = 0, ///< Accelerating
    MOTOR_TRAJECTORY_ACCEL_STATE_DECELERATING      ///< Decelerating
} motor_trajectory_accel_state_t;

/** CW/CCW select */
typedef enum e_motor_trajectory_direction
{
    MOTOR_TRAJECTORY_DIRECTION_CW = 0, ///< Clock Wize
    MOTOR_TRAJECTORY_DIRECTION_CCW     ///< Counter Clock Wize
} motor_trajectory_direction_t;

/** Trajectory callback arguments */
typedef struct st_motor_trajectory_callback_args
{
    float * p_reference;               ///< Pointer to new reference value
    bool    force_align;               ///< Force alignment flag
    float   align_value;               ///< Alignment value
} motor_trajectory_callback_args_t;

/** trajectory Control block.  Allocate an instance specific control block to pass into the API calls. */
typedef void motor_trajectory_ctrl_t;

/** Configuration parameters. */
typedef struct st_motor_trajectory_cfg
{
    void (* p_user_callback)(void * p_args); ///< Callback function
    void       * p_user_callback_args;       ///< Callback function arguments
    void const * p_context;                  ///< User context data
    void const * p_extend;                   ///< Extended configuration
} motor_trajectory_cfg_t;

/** Basis structure of orbital parameters */
typedef struct st_motor_trajectory_params_base
{
    uint32_t type_id;                               ///< Magic number for type identification
    uint16_t version;                               ///< Parameter structure version
    uint16_t size;                                  ///< Structure size for validation
    motor_trajectory_command_flags_t command_flags; ///< Command flags
} motor_trajectory_params_base_t;

/** Common Structure for Target Settings */
typedef struct st_motor_trajectory_target
{
    motor_trajectory_params_base_t const * p_params; ///< Pointer to parameter structure
} motor_trajectory_target_t;

/** Functions implemented at the HAL layer will follow this API. */
typedef struct st_motor_trajectory_api
{
    /** Open driver.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_cfg        Pointer to configuration structure.
     */
    fsp_err_t (* open)(motor_trajectory_ctrl_t * const p_ctrl, motor_trajectory_cfg_t const * const p_cfg);

    /** Close driver.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(motor_trajectory_ctrl_t * const p_ctrl);

    /** Reset the motor control. (Recover from the error status.)
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* reset)(motor_trajectory_ctrl_t * const p_ctrl);

    /** Set target information.
     *
     * @param[in]  p_ctrl        Pointer to control structure.
     * @param[in]  p_target      Pointer to target parameters.
     * @param[out] p_cmd_status  Pointer to command status output.
     */
    fsp_err_t (* targetSet)(motor_trajectory_ctrl_t * const p_ctrl, const motor_trajectory_target_t * p_target,
                            motor_trajectory_command_status_t * const p_cmd_status);

    /** Get the trajectory control status.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] p_status     Pointer to get the motor control status.
     */
    fsp_err_t (* statusGet)(motor_trajectory_ctrl_t * const p_ctrl, motor_trajectory_state_t * const p_status);

    /** Enable bypass mode for the trajectory module.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[out] bypass_mode  bypass control status
     */
    fsp_err_t (* bypassModeSet)(motor_trajectory_ctrl_t * const p_ctrl, motor_trajectory_bypass_mode_t bypass_mode);

    /** callback for the trajectory module.
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     * @param[in]  p_args       Pointer to callback arguments.
     */
    fsp_err_t (* callback)(motor_trajectory_ctrl_t * const p_ctrl, motor_trajectory_callback_args_t * const p_args);
} motor_trajectory_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_motor_trajectory_instance
{
    motor_trajectory_ctrl_t      * p_ctrl; ///< Pointer to the control structure for this instance
    motor_trajectory_cfg_t const * p_cfg;  ///< Pointer to the configuration structure for this instance
    motor_trajectory_api_t const * p_api;  ///< Pointer to the API structure for this instance
} motor_trajectory_instance_t;

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_TRAJECTORY_API)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RM_MOTOR_TRAJECTORY_API_H */
