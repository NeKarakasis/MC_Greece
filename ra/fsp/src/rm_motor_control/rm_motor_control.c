/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "rm_motor_control.h"
#include "rm_motor_trajectory_api.h"
#include "bsp_api.h"
#include "bsp_cfg.h"
#include "rm_motor_control_cfg.h"
#include <stdio.h>

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define MOTOR_CONTROL_OPEN                                  (('M' << 24U) | ('O' << 16U) | ('T' << 8U) | ('C' << 0U)) /* "MOTC" */

/* For statemachine */
#define MOTOR_CONTROL_STATEMACHINE_SIZE_STATE               (3)
#define MOTOR_CONTROL_STATEMACHINE_SIZE_EVENT               (4)

/* State machine error flags */
#define MOTOR_CONTROL_STATEMACHINE_ERROR_NONE               (0x00) /* None error flag */
#define MOTOR_CONTROL_STATEMACHINE_ERROR_EVENTOUTBOUND      (0x01) /* The event index is out of bound */
#define MOTOR_CONTROL_STATEMACHINE_ERROR_STATEOUTBOUND      (0x02) /* The state index is out of bound */
#define MOTOR_CONTROL_STATEMACHINE_ERROR_ACTIONEXCEPTION    (0x04) /* The action function returns failure */

#ifndef MOTOR_CONTROL_ERROR_RETURN

 #define  MOTOR_CONTROL_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/* Speed control <=> Current control interface functions */
void rm_motor_control_callback(motor_algorithm_callback_args_t * p_args);

/* Action functions */
static uint8_t rm_motor_control_active(motor_control_instance_ctrl_t * p_ctrl);
static uint8_t rm_motor_control_inactive(motor_control_instance_ctrl_t * p_ctrl);
static uint8_t rm_motor_control_reset(motor_control_instance_ctrl_t * p_ctrl);
static uint8_t rm_motor_control_error(motor_control_instance_ctrl_t * p_ctrl);
static uint8_t rm_motor_control_nowork(motor_control_instance_ctrl_t * p_ctrl);

static const uint8_t state_transition_table[MOTOR_CONTROL_STATEMACHINE_SIZE_EVENT][
    MOTOR_CONTROL_STATEMACHINE_SIZE_STATE
] =
{
/* State             0:MOTOR_CONTROL_STATUS_STOP, 1:MOTOR_CONTROL_STATUS_RUN, 2:MOTOR_CONTROL_STATUS_ERROR */
/* Event */
/* 0:EVENT_STOP     */ {MOTOR_CONTROL_STATUS_STOP,  MOTOR_CONTROL_STATUS_STOP,   MOTOR_CONTROL_STATUS_ERROR     },

/* 1:EVENT_RUN      */ {MOTOR_CONTROL_STATUS_RUN,   MOTOR_CONTROL_STATUS_RUN,    MOTOR_CONTROL_STATUS_ERROR     },

/* 2:EVENT_ERROR    */ {MOTOR_CONTROL_STATUS_ERROR, MOTOR_CONTROL_STATUS_ERROR,  MOTOR_CONTROL_STATUS_ERROR     },

/* 3:EVENT_RESET    */ {MOTOR_CONTROL_STATUS_STOP,  MOTOR_CONTROL_STATUS_ERROR,  MOTOR_CONTROL_STATUS_STOP      },
};

typedef uint8_t (* motor_control_action_func_t)(motor_control_instance_ctrl_t * p_ctrl);

static const motor_control_action_func_t motor_control_action_table[MOTOR_CONTROL_STATEMACHINE_SIZE_EVENT][
    MOTOR_CONTROL_STATEMACHINE_SIZE_STATE] =
{
/* State                0:STOP,                       1:RUN,                        2:ERROR */
/* Event */
/* 0:EVENT_STOP     */ {rm_motor_control_inactive, rm_motor_control_inactive, rm_motor_control_nowork      },

/* 1:EVENT_RUN      */ {rm_motor_control_active,   rm_motor_control_nowork,   rm_motor_control_nowork      },

/* 2:EVENT_ERROR    */ {rm_motor_control_error,    rm_motor_control_error,    rm_motor_control_nowork      },

/* 3:EVENT_RESET    */ {rm_motor_control_reset,    rm_motor_control_error,    rm_motor_control_reset       },
};

/* Statemachine functions */
static void     rm_motor_control_statemachine_init(motor_control_statemachine_t * p_state_machine);
static void     rm_motor_control_statemachine_reset(motor_control_statemachine_t * p_state_machine);
static uint16_t rm_motor_control_statemachine_event(motor_control_instance_ctrl_t * p_ctrl,
                                                    motor_control_event_t           event);

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
const motor_api_t g_motor_on_motor_control =
{
    .open            = RM_MOTOR_CONTROL_Open,
    .close           = RM_MOTOR_CONTROL_Close,
    .run             = RM_MOTOR_CONTROL_Run,
    .stop            = RM_MOTOR_CONTROL_Stop,
    .reset           = RM_MOTOR_CONTROL_Reset,
    .errorSet        = RM_MOTOR_CONTROL_ErrorSet,
    .speedSet        = RM_MOTOR_CONTROL_SpeedSet,
    .statusGet       = RM_MOTOR_CONTROL_StatusGet,
    .speedGet        = RM_MOTOR_CONTROL_SpeedGet,
    .errorCheck      = RM_MOTOR_CONTROL_ErrorCheck,
    .positionSet     = RM_MOTOR_CONTROL_PositionSet,
    .waitStopFlagGet = RM_MOTOR_CONTROL_WaitStopFlagGet,
    .functionSelect  = RM_MOTOR_CONTROL_FunctionSelect,
};

/*******************************************************************************************************************//**
 * @addtogroup RM_MOTOR_CONTROL
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Configure the MOTOR in register start mode. Implements @ref motor_api_t::open.
 *
 * This function should only be called once as MOTOR configuration registers can only be written to once so subsequent
 * calls will have no effect.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_Open
 *
 * @retval FSP_SUCCESS              MOTOR successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 * @retval FSP_ERR_INVALID_ARGUMENT Configuration parameter error.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Open (motor_ctrl_t * const p_ctrl, motor_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_cfg->p_extend);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);

    motor_control_extended_cfg_t * p_extended_cfg = (motor_control_extended_cfg_t *) p_cfg->p_extend;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_extended_cfg->p_motor_algorithm_instance);
#endif

    p_instance_ctrl->p_cfg = p_cfg;

    p_extended_cfg->p_motor_algorithm_instance->p_api->open(p_extended_cfg->p_motor_algorithm_instance->p_ctrl,
                                                            p_extended_cfg->p_motor_algorithm_instance->p_cfg);

    p_instance_ctrl->error_info             = MOTOR_ERROR_NONE;
    p_instance_ctrl->speed_rpm              = 0.0F;
    p_instance_ctrl->speed_ref_rpm_setpoint = 0.0F;

    rm_motor_control_statemachine_init(&(p_instance_ctrl->state_machine));

    /* Trajectory open */
    if (p_extended_cfg->p_motor_trajectory_instance != NULL)
    {
        p_extended_cfg->p_motor_trajectory_instance->p_api->open(p_extended_cfg->p_motor_trajectory_instance->p_ctrl,
                                                                 p_extended_cfg->p_motor_trajectory_instance->p_cfg);
    }

    /* Mark driver as open */
    p_instance_ctrl->open = MOTOR_CONTROL_OPEN;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Disables specified Motor Control block. Implements @ref motor_api_t::close.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_Close
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Close (motor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg->p_extend);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        p_extended_cfg->p_motor_algorithm_instance->p_api->close(
            (motor_algorithm_instance_t *) p_extended_cfg->p_motor_algorithm_instance->p_ctrl);
    }

    p_instance_ctrl->open = 0;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Reset Motor Control block. Implements @ref motor_api_t::reset.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_Reset
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Reset (motor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Reset is valid only at "error" status */
    if (MOTOR_ERROR_NONE != p_instance_ctrl->error_info)
    {
        rm_motor_control_statemachine_event(p_instance_ctrl, MOTOR_CONTROL_EVENT_RESET);

        /* Clear error information */
        p_instance_ctrl->error_info = MOTOR_ERROR_NONE;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief Run Motor (Start motor rotation). Implements @ref motor_api_t::run.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_Run
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Run (motor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    rm_motor_control_statemachine_event(p_instance_ctrl, MOTOR_CONTROL_EVENT_RUN);

    return err;
}

/*******************************************************************************************************************//**
 * @brief Stop Motor (Stop motor rotation). Implements @ref motor_api_t::stop.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_Stop
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_Stop (motor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    rm_motor_control_statemachine_event(p_instance_ctrl, MOTOR_CONTROL_EVENT_STOP);

    return err;
}

/*******************************************************************************************************************//**
 * @brief Set error information. Implements @ref motor_api_t::errorSet.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_ErrorSet
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_ErrorSet (motor_ctrl_t * const p_ctrl, motor_error_t const error)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    p_instance_ctrl->error_info |= (uint16_t) error;
    rm_motor_control_statemachine_event(p_instance_ctrl, MOTOR_CONTROL_EVENT_ERROR);

    return err;
}

/*******************************************************************************************************************//**
 * @brief Set speed reference[rpm]. Implements @ref motor_api_t::speedSet.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_SpeedSet
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_SpeedSet (motor_ctrl_t * const p_ctrl, float const speed_rpm)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    p_instance_ctrl->speed_ref_rpm_setpoint = speed_rpm;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Get current control status. Implements @ref motor_api_t::statusGet.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_StatusGet
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Data received pointer is invalid..
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_StatusGet (motor_ctrl_t * const p_ctrl, uint8_t * const p_status)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_CONTROL_ERROR_RETURN(p_status != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    *p_status = (uint8_t) (p_instance_ctrl->state_machine.status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief Get rotational speed. Implements @ref motor_api_t::speedGet.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_SpeedGet
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Data received pointer is invalid..
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_SpeedGet (motor_ctrl_t * const p_ctrl, float * const p_speed_rpm)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_CONTROL_ERROR_RETURN(p_speed_rpm != NULL, FSP_ERR_INVALID_ARGUMENT);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    *p_speed_rpm = p_instance_ctrl->speed_rpm;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Check the occurunce of Error. Implements @ref motor_api_t::errorCheck.
 *
 * Example:
 * @snippet rm_motor_control_example.c RM_MOTOR_CONTROL_ErrorCheck
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Data received pointer is invalid..
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_ErrorCheck (motor_ctrl_t * const p_ctrl, uint16_t * const p_error)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    MOTOR_CONTROL_ERROR_RETURN(p_error != NULL, FSP_ERR_INVALID_ARGUMENT);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg->p_extend);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    /* Call error check function of PM FOC layer */
    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        err = p_extended_cfg->p_motor_algorithm_instance->p_api->errorGet(
            (motor_algorithm_instance_t *) p_extended_cfg->p_motor_algorithm_instance->p_ctrl,
            &p_instance_ctrl->error_info);
    }

    *p_error = (uint16_t) p_instance_ctrl->error_info;

    if (MOTOR_ERROR_NONE != p_instance_ctrl->error_info)
    {
        rm_motor_control_statemachine_event(p_instance_ctrl, MOTOR_CONTROL_EVENT_ERROR);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief Set position reference. Implements @ref motor_api_t::positionSet.
 *
 * @retval FSP_ERR_UNSUPPORTED      Unsupported.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_PositionSet (motor_ctrl_t * const                      p_ctrl,
                                        motor_speed_position_data_t const * const p_position)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_position);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief Get wait stop flag. Implements @ref motor_api_t::waitStopFlagGet.
 *
 * @retval FSP_ERR_UNSUPPORTED      Unsupported.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_WaitStopFlagGet (motor_ctrl_t * const p_ctrl, motor_wait_stop_flag_t * const p_flag)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_flag);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief Select function. Implements @ref motor_api_t::functionSelect.
 *
 * @retval FSP_ERR_UNSUPPORTED      Unsupported.
 *
 * @note
 *
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_FunctionSelect (motor_ctrl_t * const p_ctrl, motor_function_select_t const function)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(function);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief Target set.
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_CONTROL_TargetSet (motor_ctrl_t * const                      p_ctrl,
                                      const motor_trajectory_target_t         * p_target,
                                      motor_trajectory_command_status_t * const p_cmd_status)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_control_instance_ctrl_t * p_instance_ctrl = (motor_control_instance_ctrl_t *) p_ctrl;

#if RM_MOTOR_CONTROL_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg);
    FSP_ASSERT(NULL != p_instance_ctrl->p_cfg->p_extend);
#endif
    MOTOR_CONTROL_ERROR_RETURN(MOTOR_CONTROL_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    /* Invoke the callback function if it is set. */
    motor_trajectory_instance_t * p_trajectory_instance =
        (motor_trajectory_instance_t *) p_extended_cfg->p_motor_trajectory_instance;

    if (NULL != p_trajectory_instance)
    {
        motor_trajectory_ctrl_t * p_motor_trajectory_instance_ctrl = p_trajectory_instance->p_ctrl;

        /* Call Trajectories API for TargetSet */
        p_trajectory_instance->p_api->targetSet(p_motor_trajectory_instance_ctrl, p_target, p_cmd_status);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_CONTROL)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private functions
 **********************************************************************************************************************/

/*****  For status transition *****/

/***********************************************************************************************************************
 * @brief Activates the motor control system and enables PWM output.
 *
 * This function enables the motor control by triggering the run operation
 * of the PM FOC (Permanent Magnet Field Oriented Control) instance, if available.
 *
 * @param[in] p_ctrl  Pointer to the motor control structure.
 *
 * @retval error information as uint8_t.
 **********************************************************************************************************************/
static uint8_t rm_motor_control_active (motor_control_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        p_extended_cfg->p_motor_algorithm_instance->p_api->run(
            (motor_algorithm_instance_t *) p_extended_cfg->p_motor_algorithm_instance->p_ctrl);
    }

    return (uint8_t) err;
}                                      /* End of function rm_motor_control_active() */

/***********************************************************************************************************************
 * @brief Deactivates the motor control system and disables PWM output.
 *
 * This function disables the motor control by triggering the reset operation
 * of the PM FOC (Permanent Magnet Field Oriented Control) instance, if available.
 *
 * @param[in] p_ctrl  Pointer to the  motor control structure.
 *
 * @retval error information as uint8_t.
 **********************************************************************************************************************/
static uint8_t rm_motor_control_inactive (motor_control_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        p_extended_cfg->p_motor_algorithm_instance->p_api->reset(
            (motor_algorithm_instance_t *) p_extended_cfg->p_motor_algorithm_instance->p_ctrl);
    }

    return (uint8_t) err;
}                                      /* End of function rm_motor_control_inactive() */

/***********************************************************************************************************************
 * @brief Dummy function for state machine action table.
 *
 * This function is used as a placeholder in the action table where no action is required.
 *
 * @param[in] p_ctrl Pointer to the Motor control structure.
 *
 * @retval 0 Always returns success (0).
 **********************************************************************************************************************/
static uint8_t rm_motor_control_nowork (motor_control_instance_ctrl_t * p_ctrl)
{
    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        /* Do nothing */
    }

    return 0;
}                                      /* End of function rm_motor_control_nowork() */

/***********************************************************************************************************************
 * @brief Resets the motor  control instance.
 *
 * This function resets the motor control configuration to its default state and clears any error flags.
 * It calls the reset operation on the PM FOC (Permanent Magnet Field Oriented Control) instance if available.
 *
 * @param[in] p_ctrl Pointer to the motor control structure.
 *
 * @retval 0 Always returns success (0).
 **********************************************************************************************************************/
static uint8_t rm_motor_control_reset (motor_control_instance_ctrl_t * p_ctrl)
{
    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        p_extended_cfg->p_motor_algorithm_instance->p_api->reset(p_extended_cfg->p_motor_algorithm_instance->p_ctrl);
    }

    return 0;
}                                      /* End of function rm_motor_control_reset() */

/***********************************************************************************************************************
 * @brief Executes post-processing when an error is detected in motor control.
 *
 * This function performs the necessary actions after an error has been detected,
 * including stopping the PWM output and resetting the PM FOC (Permanent Magnet Field Oriented Control)
 * instance if available.
 *
 * @param[in] p_ctrl Pointer to the motor control structure.
 *
 * @retval 0 Always returns success (0).
 **********************************************************************************************************************/
static uint8_t rm_motor_control_error (motor_control_instance_ctrl_t * p_ctrl)
{
    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    if (p_extended_cfg->p_motor_algorithm_instance != NULL)
    {
        p_extended_cfg->p_motor_algorithm_instance->p_api->reset(
            (motor_algorithm_instance_t *) p_extended_cfg->p_motor_algorithm_instance->p_ctrl);
    }

    return 0;
}                                      /* End of function rm_motor_control_error */

/***********************************************************************************************************************
 * @brief Initializes the state machine for the motor control system.
 *
 * This function initializes the state machine structure, setting the status and error fields
 * to their default values for proper operation of the motor control.
 *
 * @param[in] p_state_machine Pointer to the state machine data structure.
 *
 * @return None.
 **********************************************************************************************************************/
static void rm_motor_control_statemachine_init (motor_control_statemachine_t * p_state_machine)
{
    rm_motor_control_statemachine_reset(p_state_machine);
}                                      /* End of function rm_motor_control_statemachine_init */

/***********************************************************************************************************************
 * @brief Resets the motor control state machine to its default state.
 *
 * This function initializes the state machine structure by setting the current status,
 * next status, and error status fields to their default values. It is typically called
 * during initialization or when a full reset of the motor control system is required.
 *
 * @param[in] p_state_machine Pointer to the state machine data structure to be reset.
 *
 * @return None
 **********************************************************************************************************************/
static void rm_motor_control_statemachine_reset (motor_control_statemachine_t * p_state_machine)
{
    p_state_machine->status       = MOTOR_CONTROL_STATUS_STOP;
    p_state_machine->status_next  = MOTOR_CONTROL_STATUS_STOP;
    p_state_machine->error_status = MOTOR_CONTROL_STATEMACHINE_ERROR_NONE;
}                                      /* End of function rm_motor_control_statemachine_reset */

/***********************************************************************************************************************
 * @brief Handles a state machine event for motor control.
 *
 * This function processes a given event for the motor control state machine.
 * It checks for event and state validity, executes the corresponding action, updates the state,
 * and sets appropriate error flags if any issues occur.
 *
 * @param[in]  p_ctrl   Pointer to the motor control instance.
 * @param[in]  event    Event index to be executed. Valid values:
 *                        - MOTOR_CONTROL_EVENT_STOP:     Stop motor drive system.
 *                        - MOTOR_CONTROL_EVENT_RUN:      Activate motor drive system.
 *                        - MOTOR_CONTROL_EVENT_ERROR:    Handle error and stop drive.
 *                        - MOTOR_CONTROL_EVENT_RESET:    Reset motor drive system configuration.
 *
 * @retval Error flags for the state machine:
 *           - BIT0: Event index out of bounds.
 *           - BIT1: State index out of bounds.
 *           - BIT2: Action error (non-zero value returned from action).
 **********************************************************************************************************************/
static uint16_t rm_motor_control_statemachine_event (motor_control_instance_ctrl_t * p_ctrl,
                                                     motor_control_event_t           event)
{
    motor_control_action_func_t action_func;
    uint8_t action_ret;

    p_ctrl->state_machine.error_status = MOTOR_CONTROL_STATEMACHINE_ERROR_NONE;

    /* Check if accessing state transition table out of bound */
    if (MOTOR_CONTROL_STATEMACHINE_SIZE_EVENT <= event)
    {
        /* Event is out of bound */
        event = MOTOR_CONTROL_EVENT_ERROR;
        p_ctrl->state_machine.error_status |= MOTOR_CONTROL_STATEMACHINE_ERROR_EVENTOUTBOUND;
    }

    if (MOTOR_CONTROL_STATEMACHINE_SIZE_STATE <= p_ctrl->state_machine.status)
    {
        /* State is out of bound */
        event = MOTOR_CONTROL_EVENT_ERROR;
        p_ctrl->state_machine.status        = MOTOR_CONTROL_STATUS_STOP;
        p_ctrl->state_machine.error_status |= MOTOR_CONTROL_STATEMACHINE_ERROR_STATEOUTBOUND;
    }

    /*
     * u1_current_event : Event happening
     * u1_status        : Current status
     * u1_status_next   : Status after action executed
     */
    p_ctrl->state_machine.current_event = event;
    p_ctrl->state_machine.status_next   =
        (motor_control_status_t) (state_transition_table[(uint8_t) event][(uint8_t) p_ctrl->state_machine.status]);

    /* Get action function from action table and execute action */
    action_func = motor_control_action_table[event][p_ctrl->state_machine.status];
    action_ret  = action_func(p_ctrl);

    /* If return value is not zero, set the action exception flag */
    if (action_ret != 0U)
    {
        p_ctrl->state_machine.error_status |= MOTOR_CONTROL_STATEMACHINE_ERROR_ACTIONEXCEPTION;
    }

    p_ctrl->state_machine.status = p_ctrl->state_machine.status_next;

    return p_ctrl->state_machine.error_status;
}                                      /* End of function rm_motor_control_statemachine_event */

/***********************************************************************************************************************
 * @brief Callback function for motor control events from PM FOC (Permanent Magnet Field Oriented Control).
 *
 * This function handles events triggered by the PM FOC control interface, including speed and current feedback,
 * as well as error notifications. It updates the error information, triggers state machine transitions on errors,
 * and invokes the user-defined callback function if registered.
 *
 * @param[in] p_args Pointer to the PM FOC callback argument structure containing event and context information.
 *
 * @return None
 **********************************************************************************************************************/
void rm_motor_control_callback (motor_algorithm_callback_args_t * p_args)
{
    motor_control_instance_ctrl_t * p_ctrl = (motor_control_instance_ctrl_t *) p_args->p_context;
    motor_callback_args_t           temp_args_t;

    motor_control_extended_cfg_t * p_extended_cfg =
        (motor_control_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    switch (p_args->event)
    {
        case MOTOR_ALGORITHM_EVENT_OUTER:
        {
            /* Invoke the callback function if it is set. */
            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_CALLBACK_EVENT_SPEED_BACKWARD;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        case MOTOR_ALGORITHM_EVENT_INNER:
        {
            /* Invoke the callback function if it is set. */
            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_CALLBACK_EVENT_CURRENT_BACKWARD;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        case MOTOR_ALGORITHM_EVENT_TRAJECTORY:
        {
            /* Invoke the callback function if it is set. */
            motor_trajectory_instance_t * p_trajectory_instance =
                (motor_trajectory_instance_t *) p_extended_cfg->p_motor_trajectory_instance;
            if (NULL != p_trajectory_instance)
            {
                motor_trajectory_callback_args_t args;
                motor_trajectory_ctrl_t        * p_motor_trajectory_instance_ctrl = p_trajectory_instance->p_ctrl;

                args.p_reference = p_args->p_speed_mech_ref_rpm;
                args.force_align = false;
                args.align_value = 0;

                /* Call Trajectories callback for parameter updating */
                p_trajectory_instance->p_api->callback(p_motor_trajectory_instance_ctrl, &args);
            }
            else
            {
                /* Set speed set point instead of trajectory command */
                *(p_args->p_speed_mech_ref_rpm) = p_ctrl->speed_ref_rpm_setpoint;
            }

            /* Get actual speed via callback arguments from algorithm layer */
            p_ctrl->speed_rpm = *(p_args->p_speed_mech_rpm);

            break;
        }

        case MOTOR_ALGORITHM_EVENT_ERROR:
        {
            /* What to do when an error is detected*/
            p_ctrl->error_info |= p_args->error_info;

            /* Transition to error state */
            rm_motor_control_statemachine_event(p_ctrl, MOTOR_CONTROL_EVENT_ERROR);

            /* Invoke the callback function if it is set. */
            if (NULL != p_ctrl->p_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_CALLBACK_EVENT_ERROR;
                temp_args_t.p_context = p_ctrl->p_cfg->p_context;
                (p_ctrl->p_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}                                      /* End of function rm_motor_control_callback() */
