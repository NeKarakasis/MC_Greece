/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_TRAJECTORY_LINEAR
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_TRAJECTORY_LINEAR_H
#define RM_MOTOR_TRAJECTORY_LINEAR_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/* Register definitions, common services and error codes. */
#include "bsp_api.h"
#include "rm_motor_trajectory_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/** Initialization Helper Macros */
#define MOTOR_TRAJECTORY_LINEAR_PARAMS_INIT(in_target, in_slew_rate)   \
    {                                                                  \
        .base = {                                                      \
            .type_id       = MOTOR_TRAJECTORY_TYPE_ID_LINEAR,          \
            .version       = MOTOR_TRAJECTORY_PARAMS_VERSION_1,        \
            .size          = sizeof(motor_trajectory_linear_params_t), \
            .command_flags = MOTOR_TRAJECTORY_COMMAND_STATUS_FLAG_NONE \
        },                                                             \
        .target    = (in_target),                                      \
        .slew_rate = (in_slew_rate)                                    \
    };

/*********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Extended configuration for linear trajectory */
typedef struct st_motor_trajectory_linear_extended_cfg
{
    float cycle_time_sec;              ///< Cycle time in seconds
    bool  bypass_mode;                 ///< Bypass mode flag
    bool  force_align;                 ///< Force alignment flag
} motor_trajectory_linear_extended_cfg_t;

/** Parameters for linear trajectories (inherits base structure) */
typedef struct st_motor_trajectory_linear_params
{
    motor_trajectory_params_base_t base; ///< Base structure (placed first)
    float target;                        ///< Target value
    float slew_rate;                     ///< Acceleration rate
} motor_trajectory_linear_params_t;

/** Trajectory Control Structure */
typedef struct st_motor_trajectory_linear_ctrl
{
    uint32_t                         open;           ///< Used to determine if the channel is configured
    motor_trajectory_state_t         state;          ///< Current state
    motor_trajectory_linear_params_t current_target; ///< Current target parameters
    motor_trajectory_linear_params_t queued_target;  ///< Next target parameters queued
    float    current_reference;                      ///< Current reference value
    float    current_reference_prev;                 ///< Current reference previous value
    float    initial_value;                          ///< Initial value (value at start)
    uint32_t last_callback_time;                     ///< Time of previous callback
    float    cycle_time_sec;                         ///< Cycle time (seconds)
    void (* p_user_callback)(void * p_args);         ///< User callback function
    void   * p_user_callback_args;                   ///< User callback arguments
    bool     bypass_mode;                            ///< Bypass mode flag
    bool     force_align;                            ///< Force alignment flag and alignment value
    float    align_value;                            ///< Force alignment flag and alignment value
    uint8_t  accel_state;                            ///< Acceleration/Deceleration state
    uint8_t  direction;                              ///< Rotation direction CW/CCW
    uint32_t current_time_ms;                        ///< Current time in milliseconds
} motor_trajectory_linear_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const motor_trajectory_api_t g_motor_trajectory_on_motor_trajectory_linear;

/** @endcond */

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Open(motor_trajectory_ctrl_t * const      p_ctrl,
                                          motor_trajectory_cfg_t const * const p_cfg);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Close(motor_trajectory_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Reset(motor_trajectory_ctrl_t * const p_ctrl);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_TargetSet(motor_trajectory_ctrl_t * const           p_ctrl,
                                               const motor_trajectory_target_t         * p_target,
                                               motor_trajectory_command_status_t * const p_cmd_status);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_StatusGet(motor_trajectory_ctrl_t * const  p_ctrl,
                                               motor_trajectory_state_t * const p_state);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_BypassModeSet(motor_trajectory_ctrl_t * const p_ctrl,
                                                   motor_trajectory_bypass_mode_t  bypass_mode);
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Callback(motor_trajectory_ctrl_t * const          p_ctrl,
                                              motor_trajectory_callback_args_t * const p_args);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RM_MOTOR_TRAJECTORY_LINEAR_H */

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_TRAJECTORY_LINEAR)
 **********************************************************************************************************************/
