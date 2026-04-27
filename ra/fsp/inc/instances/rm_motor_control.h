/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_CONTROL
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_CONTROL_H
#define RM_MOTOR_CONTROL_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_api.h"
#include "rm_motor_trajectory_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Motor control status */
typedef enum e_motor_control_status
{
    MOTOR_CONTROL_STATUS_STOP  = 0,    ///< Motor control stop state
    MOTOR_CONTROL_STATUS_RUN   = 1,    ///< Motor control run state
    MOTOR_CONTROL_STATUS_ERROR = 2,    ///< Motor control error state
} motor_control_status_t;

/** Motor control event */
typedef enum e_motor_control_event
{
    MOTOR_CONTROL_EVENT_STOP  = 0,     ///< Stop event
    MOTOR_CONTROL_EVENT_RUN   = 1,     ///< Run event
    MOTOR_CONTROL_EVENT_ERROR = 2,     ///< Error event
    MOTOR_CONTROL_EVENT_RESET = 3      ///< Reset event
} motor_control_event_t;

/** Motor control state machine structure */
typedef struct st_motor_control_statemachine
{
    motor_control_status_t status;        ///< Current system status
    motor_control_status_t status_next;   ///< Next system status
    motor_control_event_t  current_event; ///< Current event index
    uint16_t               error_status;  ///< Error information
} motor_control_statemachine_t;

/** Motor Extended Control congig structure */
typedef struct st_motor_control_extended_cfg
{
    motor_algorithm_instance_t const  * p_motor_algorithm_instance;  ///< Vector Control Instance
    motor_trajectory_instance_t const * p_motor_trajectory_instance; ///< Trajectory Control Instance
} motor_control_extended_cfg_t;

/** Motor control instance control block */
typedef struct st_motor_control_instance_ctrl
{
    uint32_t open;                              ///< Module open flag
    uint32_t error_info;                        ///< Happened error

    motor_control_statemachine_t state_machine; ///< State machine structure

    float speed_ref_rpm_setpoint;               ///< Speed reference setpoint [rpm]
    float speed_rpm;                            ///< Actual speed [rpm]

    motor_cfg_t const * p_cfg;                  ///< Pointer of configuration structure
} motor_control_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const motor_api_t g_motor_on_motor_control;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Open(motor_ctrl_t * const p_ctrl, motor_cfg_t const * const p_cfg);

fsp_err_t RM_MOTOR_CONTROL_Close(motor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_CONTROL_Run(motor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_CONTROL_Stop(motor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_CONTROL_Reset(motor_ctrl_t * const p_ctrl);

fsp_err_t RM_MOTOR_CONTROL_ErrorSet(motor_ctrl_t * const p_ctrl, motor_error_t const error);

fsp_err_t RM_MOTOR_CONTROL_SpeedSet(motor_ctrl_t * const p_ctrl, float const speed_rpm);

fsp_err_t RM_MOTOR_CONTROL_StatusGet(motor_ctrl_t * const p_ctrl, uint8_t * const p_status);

fsp_err_t RM_MOTOR_CONTROL_SpeedGet(motor_ctrl_t * const p_ctrl, float * const p_speed_rpm);

fsp_err_t RM_MOTOR_CONTROL_ErrorCheck(motor_ctrl_t * const p_ctrl, uint16_t * const p_error);

fsp_err_t RM_MOTOR_CONTROL_PositionSet(motor_ctrl_t * const                      p_ctrl,
                                       motor_speed_position_data_t const * const p_position);

fsp_err_t RM_MOTOR_CONTROL_WaitStopFlagGet(motor_ctrl_t * const p_ctrl, motor_wait_stop_flag_t * const p_flag);

fsp_err_t RM_MOTOR_CONTROL_FunctionSelect(motor_ctrl_t * const p_ctrl, motor_function_select_t const function);

fsp_err_t RM_MOTOR_CONTROL_TargetSet(motor_ctrl_t * const                      p_ctrl,
                                     const motor_trajectory_target_t         * p_target,
                                     motor_trajectory_command_status_t * const p_cmd_status);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_CONTROL_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_CONTROL)
 **********************************************************************************************************************/
