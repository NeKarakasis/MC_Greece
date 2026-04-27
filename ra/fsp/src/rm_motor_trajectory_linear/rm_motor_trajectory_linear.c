/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <string.h>
#include <math.h>

#include "rm_motor_trajectory_linear.h"
#include "rm_motor_trajectory_linear_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_TRAJECTORY_LINEAR_OPEN                (0x4D544C4EU) // "MTLN" in hex
#define MOTOR_TRAJECTORY_LINEAR_TARGET_TOLERANCE    (0.001F)      ///< Target reached tolerance
#define MOTOR_TRAJECTORY_LINEAR_TIME_EPSILON        (0.000001F)   ///< Minimum elapsed time

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static float rm_motor_calculate_next_reference(motor_trajectory_linear_ctrl_t * p_ctrl, float elapsed_time);
static void  rm_motor_trajectory_linear_update_direction_accel(motor_trajectory_linear_ctrl_t * p_instance_ctrl);

/***********************************************************************************************************************
 * Global variabless
 **********************************************************************************************************************/
const motor_trajectory_api_t g_motor_trajectory_on_motor_trajectory_linear =
{
    .open          = RM_MOTOR_TRAJECTORY_LINEAR_Open,
    .close         = RM_MOTOR_TRAJECTORY_LINEAR_Close,
    .reset         = RM_MOTOR_TRAJECTORY_LINEAR_Reset,
    .targetSet     = RM_MOTOR_TRAJECTORY_LINEAR_TargetSet,
    .statusGet     = RM_MOTOR_TRAJECTORY_LINEAR_StatusGet,
    .bypassModeSet = RM_MOTOR_TRAJECTORY_LINEAR_BypassModeSet,
    .callback      = RM_MOTOR_TRAJECTORY_LINEAR_Callback,
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Opens and configures. Implements @ref motor_trajectory_api_t::open.
 *
 * @retval FSP_SUCCESS              Motor Driver successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Open (motor_trajectory_ctrl_t * const      p_ctrl,
                                           motor_trajectory_cfg_t const * const p_cfg)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_cfg->p_extend);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
#endif

    motor_trajectory_linear_extended_cfg_t * p_extended_cfg =
        (motor_trajectory_linear_extended_cfg_t *) p_cfg->p_extend;

#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_extended_cfg);
#endif

    /* Initialization */
    p_instance_ctrl->state                = MOTOR_TRAJECTORY_STATE_IDLE;
    p_instance_ctrl->current_reference    = 0.0F;
    p_instance_ctrl->initial_value        = 0.0F;
    p_instance_ctrl->last_callback_time   = 0;
    p_instance_ctrl->cycle_time_sec       = p_extended_cfg->cycle_time_sec;
    p_instance_ctrl->p_user_callback      = p_cfg->p_user_callback;
    p_instance_ctrl->p_user_callback_args = p_cfg->p_user_callback_args;
    p_instance_ctrl->bypass_mode          = p_extended_cfg->bypass_mode;
    p_instance_ctrl->force_align          = p_extended_cfg->force_align;
    p_instance_ctrl->align_value          = 0.0F;
    p_instance_ctrl->current_time_ms      = 0;

    /* Mark the module as open */
    p_instance_ctrl->open = MOTOR_TRAJECTORY_LINEAR_OPEN;

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_Open */

/*******************************************************************************************************************//**
 * @brief Disables specified. Implements @ref motor_trajectory_api_t::close.
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Close (motor_trajectory_ctrl_t * const p_ctrl)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Mark the module as closed */
    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_Close */

/*******************************************************************************************************************//**
 * @brief Resets the configurations Implements @ref motor_trajectory_api_t::reset.
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Reset (motor_trajectory_ctrl_t * const p_ctrl)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Reset processing */
    p_instance_ctrl->state             = MOTOR_TRAJECTORY_STATE_IDLE;
    p_instance_ctrl->current_reference = 0.0F;
    p_instance_ctrl->initial_value     = 0.0F;
    p_instance_ctrl->bypass_mode       = false;

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_Reset */

/*******************************************************************************************************************//**
 * @brief Target set Implements @ref motor_trajectory_api_t::targetSet.
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid argument.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_TargetSet (motor_trajectory_ctrl_t * const           p_ctrl,
                                                const motor_trajectory_target_t         * p_target,
                                                motor_trajectory_command_status_t * const p_cmd_status)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_target);
    FSP_ASSERT(NULL != p_target->p_params);
    FSP_ASSERT(NULL != p_cmd_status);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Type checking */
    if (p_target->p_params->type_id != MOTOR_TRAJECTORY_TYPE_ID_LINEAR)
    {
        *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_REJECTED;

        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Version Check  */
    if (p_target->p_params->version != MOTOR_TRAJECTORY_PARAMS_VERSION_1)
    {
        *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_REJECTED;

        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Size Check */
    if (p_target->p_params->size != sizeof(motor_trajectory_linear_params_t))
    {
        *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_REJECTED;

        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Safe Cast */
    const motor_trajectory_linear_params_t * p_linear_params =
        (const motor_trajectory_linear_params_t *) p_target->p_params;

    /* Action if immediate flag is set */
    if (p_linear_params->base.command_flags & MOTOR_TRAJECTORY_COMMAND_STATUS_FLAG_IMMEDIATE)
    {
        /* Aborts the current trajectory, clears the queue, and immediately executes a new command. */

        /* Replace the current command with the new command */
        p_instance_ctrl->current_target    = *p_linear_params;
        p_instance_ctrl->initial_value     = p_instance_ctrl->current_reference;
        p_instance_ctrl->current_reference = p_linear_params->target;

        /* Clear the queue (if in QUEUED state)） */
        if (p_instance_ctrl->state == MOTOR_TRAJECTORY_STATE_QUEUED)
        {
            /* Discard queued commands */
            memset(&p_instance_ctrl->queued_target, 0, sizeof(p_instance_ctrl->queued_target));
        }

        /* Set the state to IN_PROGRESS */
        p_instance_ctrl->state              = MOTOR_TRAJECTORY_STATE_IN_PROGRESS;
        p_instance_ctrl->last_callback_time = p_instance_ctrl->current_time_ms;

        *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_ACCEPTED_IMMEDIATE;

        return FSP_SUCCESS;
    }

    /* Command processing */
    switch (p_instance_ctrl->state)
    {
        case MOTOR_TRAJECTORY_STATE_IDLE:
        {
            /* Copy Parameters */
            p_instance_ctrl->current_target     = *p_linear_params;
            p_instance_ctrl->initial_value      = p_instance_ctrl->current_reference;
            p_instance_ctrl->state              = MOTOR_TRAJECTORY_STATE_IN_PROGRESS;
            p_instance_ctrl->last_callback_time = 0;

            if (p_instance_ctrl->p_user_callback != NULL)
            {
                p_instance_ctrl->p_user_callback(p_instance_ctrl->p_user_callback_args);
            }

            *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_ACCEPTED_IMMEDIATE;
            break;
        }

        case MOTOR_TRAJECTORY_STATE_IN_PROGRESS:
        {
            /* Queue the command if currently in progress */
            p_instance_ctrl->queued_target = *p_linear_params;
            p_instance_ctrl->state         = MOTOR_TRAJECTORY_STATE_QUEUED;

            *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_QUEUED;
            break;
        }

        default:                       /* MOTOR_TRAJECTORY_STATE_QUEUED */
        {
            /* Reject if queue is already full */
            *p_cmd_status = MOTOR_TRAJECTORY_COMMAND_STATUS_REJECTED;
            break;
        }
    }

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_TargetSet */

/*******************************************************************************************************************//**
 * @brief Status get Implements @ref motor_trajectory_api_t::statusGet.
 *
 * @retval FSP_SUCCESS              Successfully resetted.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_StatusGet (motor_trajectory_ctrl_t * const  p_ctrl,
                                                motor_trajectory_state_t * const p_state)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_state);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Return the state */
    *p_state = p_instance_ctrl->state;

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_StatusGet */

/*******************************************************************************************************************//**
 * @brief Sets bypass mode Implements @ref motor_trajectory_api_t::bypassModeSet.
 *
 * @param[in] p_ctrl         Pointer to the control structure.
 * @param[in] bypass_mode    True to enable bypass mode, false to disable
 *
 * @retval FSP_SUCCESS              Successfully set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_BypassModeSet (motor_trajectory_ctrl_t * const p_ctrl,
                                                    motor_trajectory_bypass_mode_t  bypass_mode)
{
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
#if RM_MOTOR_TRAJECTORY_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(MOTOR_TRAJECTORY_LINEAR_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Set bypass mode */
    p_instance_ctrl->bypass_mode = bypass_mode;

    return FSP_SUCCESS;
}                                      /* End of function  RM_MOTOR_TRAJECTORY_LINEAR_BypassModeSet*/

/*******************************************************************************************************************//**
 * @brief Callback function called by the algorithm layer to get the next reference value.
 *
 * @param[in] p_ctrl         Pointer to the control structure.
 * @param[in] p_args         Pointer to the control structure.
 *
 * @retval FSP_SUCCESS              Successfully set.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_TRAJECTORY_LINEAR_Callback (motor_trajectory_ctrl_t * const          p_ctrl,
                                               motor_trajectory_callback_args_t * const p_args)
{
    float elapsed_time;
    motor_trajectory_linear_ctrl_t * p_instance_ctrl = (motor_trajectory_linear_ctrl_t *) p_ctrl;

    /** Parameter validation */
    if ((NULL == p_instance_ctrl))
    {
        return FSP_ERR_ASSERTION;
    }

    /* Check if module is open */
    if (MOTOR_TRAJECTORY_LINEAR_OPEN != p_instance_ctrl->open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    p_instance_ctrl->current_time_ms++;

    /* p_args NULL check */
    if (NULL == p_args)
    {
        return FSP_ERR_ASSERTION;
    }

    /* p_reference NULL hceck */
    if (NULL == p_args->p_reference)
    {
        return FSP_ERR_ASSERTION;
    }

    /* Bypass mode handling */
    if (p_instance_ctrl->bypass_mode)
    {
        /* Return current reference value as is */
        *p_args->p_reference = p_instance_ctrl->current_reference;

        return FSP_SUCCESS;
    }

    /* Force alignment handling */
    if (p_args->force_align)
    {
        p_instance_ctrl->current_reference = p_args->align_value;
        *p_args->p_reference               = p_args->align_value;

        return FSP_SUCCESS;
    }

    /* Calculate elapsed time */
    if (p_instance_ctrl->last_callback_time == 0)
    {
        elapsed_time = 0.0F;
    }
    else
    {
        elapsed_time = (float) (p_instance_ctrl->current_time_ms - p_instance_ctrl->last_callback_time) *
                       p_instance_ctrl->cycle_time_sec;
    }

    p_instance_ctrl->last_callback_time = p_instance_ctrl->current_time_ms;

    /* Process based on trajectory state */
    if ((p_instance_ctrl->state == MOTOR_TRAJECTORY_STATE_IN_PROGRESS) ||
        (p_instance_ctrl->state == MOTOR_TRAJECTORY_STATE_QUEUED))
    {
        /* Calculate next reference value */
        p_instance_ctrl->current_reference = rm_motor_calculate_next_reference(p_instance_ctrl, elapsed_time);

        /* direction/acceleration state set*/
        rm_motor_trajectory_linear_update_direction_accel(p_instance_ctrl);

        /* Check if target has been reached */
        if (fabsf(p_instance_ctrl->current_reference - p_instance_ctrl->current_target.target) <
            MOTOR_TRAJECTORY_LINEAR_TARGET_TOLERANCE)
        {
            /* Target reached */
            p_instance_ctrl->current_reference = p_instance_ctrl->current_target.target;

            /* If there's a queued command */
            if (p_instance_ctrl->state == MOTOR_TRAJECTORY_STATE_QUEUED)
            {
                /* Execute the queued command */
                p_instance_ctrl->current_target = p_instance_ctrl->queued_target;
                p_instance_ctrl->initial_value  = p_instance_ctrl->current_reference;
                p_instance_ctrl->state          = MOTOR_TRAJECTORY_STATE_IN_PROGRESS;
            }
            else
            {
                /* Trajectory complete */
                p_instance_ctrl->state = MOTOR_TRAJECTORY_STATE_IDLE;

                /* Call user callback */
                if (p_instance_ctrl->p_user_callback != NULL)
                {
                    p_instance_ctrl->p_user_callback(p_instance_ctrl->p_user_callback_args);
                }
            }
        }
    }

    /* Return the calculated reference value */
    *p_args->p_reference = p_instance_ctrl->current_reference;

    return FSP_SUCCESS;
}                                      /* End of function RM_MOTOR_TRAJECTORY_LINEAR_Callback */

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Calculates the next reference value based on the current state and elapsed time.
 *
 * @param[in] p_ctrl            Pointer to the control structure.
 * @param[in] elapsed_time      Time elapsed since the last calculation.
 *
 * @return The calculated reference value.
 **********************************************************************************************************************/
static float rm_motor_calculate_next_reference (motor_trajectory_linear_ctrl_t * p_ctrl, float elapsed_time)
{
    float target    = p_ctrl->current_target.target;
    float current   = p_ctrl->current_reference;
    float slew_rate = p_ctrl->current_target.slew_rate;
    float delta;
    float change_amount;

    /* Calculate difference between target and current value */
    delta = target - current;

    /* If elapsed time is too small, avoid division by zero */
    if (elapsed_time < MOTOR_TRAJECTORY_LINEAR_TIME_EPSILON)
    {
        return current;
    }

    /* Calculate change amount (rate * elapsed time) */
    change_amount = slew_rate * elapsed_time;

    /* Limit change amount (to avoid overshooting the target) */
    if (fabsf(delta) <= change_amount)
    {
        return target;
    }

    /* Linear change toward target */
    return current + (delta > 0.0F ? change_amount : -change_amount);
}                                      /* End of function rm_motor_calculate_next_reference */

/*********************************************************************************************************************
 * @brief Calculates the next reference value based on the current state and elapsed time.
 *
 * @param[in,out] p_instance_ctrl  Pointer to the control structure.
 *
 * @return The calculated reference value.
 **********************************************************************************************************************/
static void rm_motor_trajectory_linear_update_direction_accel (motor_trajectory_linear_ctrl_t * p_instance_ctrl)
{
    float delta = p_instance_ctrl->current_reference_prev - p_instance_ctrl->current_reference;

    /* Checking in progress */
    if (p_instance_ctrl->current_reference < 0)
    {
        /* CCW */
        p_instance_ctrl->direction = (uint8_t) MOTOR_TRAJECTORY_DIRECTION_CCW;
        if (delta > 0)
        {
            p_instance_ctrl->accel_state = (uint8_t) MOTOR_TRAJECTORY_ACCEL_STATE_ACCELERATING;
        }
        else if (delta < 0)
        {
            p_instance_ctrl->accel_state = (uint8_t) MOTOR_TRAJECTORY_ACCEL_STATE_DECELERATING;
        }
        else
        {
            /* No change in acceleration/deceleration */
            ;
        }
    }
    else if (p_instance_ctrl->current_reference > 0)
    {
        /* CW */
        p_instance_ctrl->direction = (uint8_t) MOTOR_TRAJECTORY_DIRECTION_CW;
        if (delta > 0)
        {
            p_instance_ctrl->accel_state = (uint8_t) MOTOR_TRAJECTORY_ACCEL_STATE_DECELERATING;
        }
        else if (delta < 0)
        {
            p_instance_ctrl->accel_state = (uint8_t) MOTOR_TRAJECTORY_ACCEL_STATE_ACCELERATING;
        }
        else
        {
            /* No change in acceleration/deceleration */
            ;
        }
    }
    else
    {
        /* speed or position 0 */
        ;
    }

    p_instance_ctrl->current_reference_prev = p_instance_ctrl->current_reference;
}
