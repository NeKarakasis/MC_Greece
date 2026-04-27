/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdint.h>
#include "rm_motor_pm_foc.h"
#include "rm_motor_pm_foc_internal.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Speed reference status */
#define     MOTOR_OUTER_SPEED_ZERO_CONST       (0)
#define     MOTOR_OUTER_POSITION_CONTROL       (1)
#define     MOTOR_OUTER_SPEED_CHANGE           (2)
#define     MOTOR_OUTER_OPEN_LOOP_INDUCTION    (3)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/** Callback function */
void rm_motor_outer_cyclic(timer_callback_args_t * p_args);

/** Internal function */
static float motor_outer_set_speed_ref(motor_pm_foc_instance_ctrl_t * p_ctrl);
static float motor_outer_speed_rate_limit(motor_pm_foc_instance_ctrl_t * p_ctrl);
static void  motor_outer_set_i_ref(motor_pm_foc_instance_ctrl_t * p_ctrl, float * f4_id_ref_out, float * f4_iq_ref_out);
static float motor_outer_speed_pi(motor_pm_foc_outer_instance_ctrl_t * p_ctrl, float f_speed_rad);
static void  motor_outer_reset_shared_data(motor_shared_outer_to_inner_foc_t * p_output);
static void  motor_outer_error_check(motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl,
                                     motor_pm_foc_extended_cfg_t        * p_extended_cfg,
                                     uint16_t                           * p_error_flags);
static void motor_outer_trajectory_call(motor_pm_foc_outer_instance_ctrl_t * p_ctrl);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Initializes and configures the Motor Outer Control Module.
 *
 * This function sets up the outer control instance, including shared data buffers, active sensor instance,
 * PI controller parameters, flux weakening control, speed observer, and timer instance.
 * It also resets shared data structures and clears error flags.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_outer_init (motor_pm_foc_instance_ctrl_t * const p_ctrl)
{
    motor_algorithm_cfg_t       * p_cfg          = (motor_algorithm_cfg_t *) p_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_outer_instance_ctrl = p_ctrl->p_outer_instance_ctrl;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl->p_inner_instance_ctrl;

    p_outer_instance_ctrl->p_cfg = p_extended_cfg->p_motor_outer_cfg;

    // set up the shared data buffers (inner to outer, outer to inner)
    p_outer_instance_ctrl->p_from_inner  = &(p_extended_cfg->p_shared_data->shared_inner_to_outer_A);
    p_outer_instance_ctrl->p_to_inner[0] = &(p_extended_cfg->p_shared_data->shared_outer_to_inner_A);
    p_outer_instance_ctrl->p_to_inner[1] = &(p_extended_cfg->p_shared_data->shared_outer_to_inner_B);

    // assigning active pointer
    p_outer_instance_ctrl->p_to_outer_active = p_outer_instance_ctrl->p_to_inner[0];
    p_extended_cfg->p_shared_data->pp_shared_outer_to_inner_active =
        (void **) &(p_outer_instance_ctrl->p_to_outer_active);

    // Identify the active slot for position and speed measurement/estimation
    if (p_extended_cfg->data.sensor_slot == MOTOR_INNER_SLOT_1_ENABLE)
    {
        if (p_inner_instance_ctrl->p_sensor_instance != NULL)
        {
            p_outer_instance_ctrl->p_sensor_active_instance = p_inner_instance_ctrl->p_sensor_instance;
        }
    }
    else
    {
        if (p_inner_instance_ctrl->p_sensor_instance2 != NULL)
        {
            p_outer_instance_ctrl->p_sensor_active_instance = p_inner_instance_ctrl->p_sensor_instance2;
        }
    }

    p_outer_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_outer_instance_ctrl->signals.speed_mech_lpf = 0.0F;

    p_outer_instance_ctrl->signals.speed_mech_que_ref = 0.0F;

    p_outer_instance_ctrl->signals.speed_pi_ctrl.f4_ilimit = p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3;

    rm_motor_speed_pi_gain_calc(p_extended_cfg->pm_motor_parameter,
                                &(p_outer_instance_ctrl->signals.speed_pi_ctrl),
                                p_extended_cfg->pm_outer_design_parameter->speed_omega,
                                p_extended_cfg->pm_outer_design_parameter->speed_zeta,
                                p_extended_cfg->data.outer_control_period);

    rm_motor_fluxwkn_init(&(p_outer_instance_ctrl->signals.flux_weakening_ctrl),
                          p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3,
                          1.0F,
                          p_extended_cfg->pm_motor_parameter);

    rm_motor_filter_first_order_lpff_init(&(p_outer_instance_ctrl->signals.speed_lpf_ctrl));
    rm_motor_filter_first_order_lpff_gain_calc(&(p_outer_instance_ctrl->signals.speed_lpf_ctrl),
                                               p_extended_cfg->data.speed_lpf_omega_t,
                                               p_extended_cfg->data.outer_control_period);

    rm_motor_fluxwkn_init(&(p_outer_instance_ctrl->signals.flux_weakening_ctrl),
                          p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3,
                          1.0F,
                          p_extended_cfg->pm_motor_parameter);

    /* Speed Observer */
    if (p_extended_cfg->outerfunc_table.spdobsv_ctrl != &algorithm_disabled)
    {
        if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_spdobsrv_ctrl)
        {
            rm_motor_spdobserver_init(&(p_outer_instance_ctrl->signals.speed_observer_ctrl));
            rm_motor_spdobserver_gain_calc(&(p_outer_instance_ctrl->signals.speed_observer_ctrl),
                                           p_extended_cfg->pm_outer_design_parameter->observer_omega,
                                           p_extended_cfg->pm_outer_design_parameter->observer_zeta,
                                           p_extended_cfg->data.outer_control_period);
        }
        else
        {
            /* Nothing to do */
        }
    }

    /* Set used timer instance */
    if (p_cfg->p_timer_instance != NULL)
    {
        p_cfg->p_timer_instance->p_api->open(p_cfg->p_timer_instance->p_ctrl, p_cfg->p_timer_instance->p_cfg);
        p_cfg->p_timer_instance->p_api->start(p_cfg->p_timer_instance->p_ctrl);
    }

    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[0]);
    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[1]);

    p_outer_instance_ctrl->p_to_outer_copy = p_outer_instance_ctrl->p_to_inner[1];

    /* clear outer error flag */
    p_outer_instance_ctrl->signals.error_info = MOTOR_PM_FOC_ERROR_NONE;
}                                      /* End of function rm_motor_outer_init */

/*******************************************************************************************************************//**
 * @brief Closes the Motor Outer Control Module.
 *
 * This function disables the outer control, resets the speed observer, closes the timer instance,
 * and clears shared data structures.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_outer_close (motor_pm_foc_instance_ctrl_t * const p_ctrl)
{
    motor_algorithm_cfg_t       * p_cfg          = (motor_algorithm_cfg_t *) p_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_outer_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    p_outer_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;

    if (p_extended_cfg->outerfunc_table.spdobsv_ctrl != &algorithm_disabled)
    {
        if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_spdobsrv_ctrl)
        {
            rm_motor_spdobserver_reset(&(p_outer_instance_ctrl->signals.speed_observer_ctrl));
        }
        else if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_extobsrv_ctrl)
        {
            rm_motor_extobserver_reset(&(p_outer_instance_ctrl->signals.extobserver_ctrl));
        }
        else
        {
            /* Nothing to do */
        }
    }

    /* Close used timer instance */
    if (p_cfg->p_timer_instance != NULL)
    {
        p_cfg->p_timer_instance->p_api->close(p_cfg->p_timer_instance->p_ctrl);
    }

    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[0]);
    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[1]);
}                                      /* End of function rm_motor_outer_close */

/*******************************************************************************************************************//**
 * @brief Resets the variables of Motor Outer Control Module.
 *
 * This function resets the outer control instance variables, including speed references,
 * PI controller states, flux weakening control, speed observer, and shared data structures.
 * It also clears error flags.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_outer_reset (motor_pm_foc_instance_ctrl_t * const p_ctrl)
{
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_outer_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    p_outer_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_outer_instance_ctrl->signals.speed_mech_lpf = 0.0F;

    p_outer_instance_ctrl->signals.speed_mech_que_ref = 0.0F;

    p_outer_instance_ctrl->signals.speed_lpf_ctrl.f4_pre_output = 0.0F;
    p_outer_instance_ctrl->signals.speed_lpf_ctrl.f4_pre_input  = 0.0F;

    /* Reset PI parameters */
    p_outer_instance_ctrl->signals.speed_pi_ctrl.f4_err  = 0.0F;
    p_outer_instance_ctrl->signals.speed_pi_ctrl.f4_refi = 0.0F;

    rm_motor_fluxwkn_reset(&(p_outer_instance_ctrl->signals.flux_weakening_ctrl));

    if (p_extended_cfg->outerfunc_table.spdobsv_ctrl != &algorithm_disabled)
    {
        if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_spdobsrv_ctrl)
        {
            rm_motor_spdobserver_reset(&(p_outer_instance_ctrl->signals.speed_observer_ctrl));
        }
        else if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_extobsrv_ctrl)
        {
            rm_motor_extobserver_reset(&(p_outer_instance_ctrl->signals.extobserver_ctrl));
        }
        else
        {
            /* Nothing to do */
        }
    }

    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[0]);
    motor_outer_reset_shared_data(p_outer_instance_ctrl->p_to_inner[1]);

    /* clear outer error flag */
    p_outer_instance_ctrl->signals.error_info = MOTOR_PM_FOC_ERROR_NONE;
}                                      /* End of function rm_motor_outer_reset */

/*******************************************************************************************************************//**
 * @brief Calculates the d/q-axis current reference for position control.
 *
 * This function is reserved for future implementation of position control.
 *
 * @param[in,out] p_ctrl  Pointer to control structure
 * @param[in]     p_cfg   Pointer to configuration structure
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_position_control (void * p_ctrl, void * p_cfg)
{
    /* To be considered at 1st release */
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);
}

/*******************************************************************************************************************//**
 * @brief Calculates the d/q-axis current reference for speed control.
 *
 * This function performs the main process of outer control, including speed filtering,
 * trajectory control, speed reference setting, and current reference calculation.
 * It also applies MTPA and flux weakening control via function pointers.
 *
 * @param[in,out] p_ctrl  Pointer to control structure
 * @param[in]     p_cfg   Pointer to configuration structure
 *
 * @retval None
 **********************************************************************************************************************/
void motor_outer_speed_control (void * p_ctrl, void * p_cfg)
{
    float f4_idq_ref[2] = {0.0F};
    motor_pm_foc_instance_ctrl_t       * p_pm_foc_ctrl   = (motor_pm_foc_instance_ctrl_t *) p_ctrl;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = p_pm_foc_ctrl->p_outer_instance_ctrl;
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  =
        (motor_pm_foc_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    uint8_t u1_control_mode = p_instance_ctrl->signals.inner_mode_data.mode_outer;

    FSP_PARAMETER_NOT_USED(p_cfg);

    /* Filters for speed */
    p_extended_cfg->outerfunc_table.spdobsv_ctrl(p_instance_ctrl, p_extended_cfg);

    /***** Id, Iq, speed reference setting *****/
    if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->p_from_inner->current_ref_update_flag)
    {
        /* set speed or position function */
        motor_outer_trajectory_call(p_instance_ctrl);
        p_instance_ctrl->signals.speed_mech_que_ref = motor_outer_set_speed_ref(p_ctrl);
        if (u1_control_mode != MOTOR_CTRLMODE_ID_UP_REF)
        {
            p_instance_ctrl->p_to_outer_copy->pos_speed_input_data.speed_mech_ref_ctrl =
                p_instance_ctrl->signals.speed_mech_que_ref;
        }

        motor_outer_set_i_ref(p_ctrl, &f4_idq_ref[0], &f4_idq_ref[1]);
    }

    // Function Pointer Table supporting Position/Speed/MTPA/Fluxweaken
    p_instance_ctrl->p_to_outer_copy->i_d_ref = f4_idq_ref[0];
    p_instance_ctrl->p_to_outer_copy->i_q_ref = f4_idq_ref[1];

    if (MOTOR_CTRLMODE_DEFAULT == p_instance_ctrl->signals.inner_mode_data.mode_outer)
    {
        // Flux Weakening
        p_extended_cfg->outerfunc_table.fw_ctrl(p_instance_ctrl, p_extended_cfg);

        // MTPA
        p_extended_cfg->outerfunc_table.mtpa_ctrl(p_instance_ctrl, p_extended_cfg);
    }

    p_instance_ctrl->p_to_outer_copy->pos_speed_input_data.speed_pi_status_flag = MOTOR_FUNDLIB_FLAG_CLEAR;

    if (MOTOR_CTRLMODE_DEFAULT == u1_control_mode)
    {
        p_instance_ctrl->p_to_outer_copy->pos_speed_input_data.speed_pi_status_flag = MOTOR_FUNDLIB_FLAG_SET;
    }
}                                      /* End of function motor_outer_speed_control */

/*******************************************************************************************************************//**
 * @brief Updates the parameters of outer control calculation.
 *
 * This function recalculates PI controller gains, LPF parameters, flux weakening parameters,
 * and speed observer gains based on the updated configuration.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_outer_parameter_update (motor_pm_foc_instance_ctrl_t * const p_ctrl)
{
    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    p_instance_ctrl->p_cfg = p_extended_cfg->p_motor_outer_cfg;

    p_instance_ctrl->signals.speed_pi_ctrl.f4_ilimit = p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3;

    rm_motor_speed_pi_gain_calc(p_extended_cfg->pm_motor_parameter,
                                &(p_instance_ctrl->signals.speed_pi_ctrl),
                                p_extended_cfg->pm_outer_design_parameter->speed_omega,
                                p_extended_cfg->pm_outer_design_parameter->speed_zeta,
                                p_extended_cfg->data.outer_control_period);

    rm_motor_filter_first_order_lpff_gain_calc(&(p_instance_ctrl->signals.speed_lpf_ctrl),
                                               p_extended_cfg->data.speed_lpf_omega_t,
                                               p_extended_cfg->data.outer_control_period);

    rm_motor_fluxwkn_init(&(p_instance_ctrl->signals.flux_weakening_ctrl),
                          p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3,
                          1.0F,
                          p_extended_cfg->pm_motor_parameter);

    if (p_extended_cfg->outerfunc_table.spdobsv_ctrl != &algorithm_disabled)
    {
        if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_spdobsrv_ctrl)
        {
            rm_motor_spdobserver_gain_calc(&(p_instance_ctrl->signals.speed_observer_ctrl),
                                           p_extended_cfg->pm_outer_design_parameter->observer_omega,
                                           p_extended_cfg->pm_outer_design_parameter->observer_zeta,
                                           p_extended_cfg->data.outer_control_period);
        }
        else
        {
            /* Nothing to do */
        }
    }
}                                      /* End of function rm_motor_outer_parameter_update */

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name : rm_motor_outer_cyclic
 * Description   : Cyclic process of outer control (called at timer interrupt)
 * Arguments     : p_args - Pointer to timer callback arguments
 * Return Value  : None
 **********************************************************************************************************************/
void rm_motor_outer_cyclic (timer_callback_args_t * p_args)
{
    uint16_t error_flags = MOTOR_PM_FOC_ERROR_NONE;
    motor_algorithm_instance_t    * p_instance = (motor_algorithm_instance_t *) p_args->p_context;
    motor_algorithm_callback_args_t temp_args_t;
    motor_pm_foc_instance_ctrl_t  * p_instance_user_ctrl =
        (motor_pm_foc_instance_ctrl_t *) p_instance->p_ctrl;
    motor_algorithm_cfg_t * p_instance_user_cfg =
        (motor_algorithm_cfg_t *) p_instance_user_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t * p_instance_user_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_instance_user_cfg->p_extend;
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_instance->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl       = p_extended_cfg->pm_foc_outer_instance_ctrl;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_extended_cfg->pm_foc_inner_instance_ctrl;

    motor_pm_foc_outer_cfg_t * p_outer_cfg = (motor_pm_foc_outer_cfg_t *) p_extended_cfg->p_motor_outer_cfg;

    float f_mech_speed;

    /* Tuner processing interrupts */
    if (p_extended_cfg->p_tuner_instance != NULL)
    {
        motor_tuner_instance_t const * p_tuner = p_extended_cfg->p_tuner_instance;
        bool tuner_active = false;

        p_tuner->p_api->isActive(p_tuner->p_ctrl, &tuner_active);
        if (tuner_active)
        {
            /* Tuner outer loop processing */
            p_tuner->p_api->outerLoopProcess(p_tuner->p_ctrl);

            return;
        }
    }

    /* getting position and speed information from the active sensor */
    p_instance_ctrl->p_sensor_active_instance->p_api->positionSpeedGet(
        p_instance_ctrl->p_sensor_active_instance->p_ctrl,
        &p_instance_ctrl->signals.angle_mech,
        &f_mech_speed);
    p_instance_ctrl->signals.speed_mech = f_mech_speed;

    // getting the control mode request from inner loop
    rm_motor_inner_outer_ref_ctrl_mode_get(p_inner_instance_ctrl, &p_instance_ctrl->signals.inner_mode_data);

    p_instance_ctrl->p_from_inner = *(p_extended_cfg->p_shared_data->pp_shared_inner_to_outer_active);

    /* Perform Speed Control Process */
    if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->signals.active)
    {
        /* Check for errors during outer loop execution */
        motor_outer_error_check(p_instance_ctrl, p_instance_user_extended_cfg, &error_flags);
        if (error_flags != MOTOR_PM_FOC_ERROR_NONE)
        {
            /* Notify error to upper layer */
            if (NULL != p_extended_cfg->p_callback)
            {
                motor_algorithm_callback_args_t temp_outer_args_t;
                temp_outer_args_t.event      = MOTOR_ALGORITHM_EVENT_ERROR;
                temp_outer_args_t.error_info = error_flags;
                temp_outer_args_t.p_context  = p_outer_cfg->p_context;
                (p_extended_cfg->p_callback)(&temp_outer_args_t);
            }

            return;                    // Exit early on error
        }

        /* Speed Control / Position Control main process */
        p_extended_cfg->outerfunc_table.control_mode_ctrl(p_instance->p_ctrl, p_extended_cfg);

        // swap the outer to inner shared data pointer
        if (p_instance_ctrl->p_to_outer_active == p_instance_ctrl->p_to_inner[0])
        {
            p_instance_ctrl->p_to_outer_active = p_instance_ctrl->p_to_inner[1]; // need to be atomic instruction
            p_instance_ctrl->p_to_outer_copy   = p_instance_ctrl->p_to_inner[0];
        }
        else
        {
            p_instance_ctrl->p_to_outer_active = p_instance_ctrl->p_to_inner[0]; // need to be atomic instruction
            p_instance_ctrl->p_to_outer_copy   = p_instance_ctrl->p_to_inner[1];
        }
    }

    /* Invoke the callback function if it is set. */
    if (NULL != p_extended_cfg->p_callback)
    {
        temp_args_t.event     = MOTOR_ALGORITHM_EVENT_OUTER;
        temp_args_t.p_context = p_outer_cfg->p_context;
        (p_extended_cfg->p_callback)(&temp_args_t);
    }
}                                      /* End of function rm_motor_outer_cyclic */

/***********************************************************************************************************************
 * Function Name : motor_outer_speed_rate_limit
 * Description   : Limits the rate of change of speed reference
 * Arguments     : p_ctrl - Pointer to motor PM FOC instance control structure
 * Return Value  : Limited speed reference [rad/s]
 **********************************************************************************************************************/
static float motor_outer_speed_rate_limit (motor_pm_foc_instance_ctrl_t * p_ctrl)
{
    float f4_temp0;
    float f4_temp1;
    float f4_speed_ref_calc_rad;
    float f4_speed_ctrl_period;

    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    f4_temp0 = p_instance_ctrl->signals.speed_mech_ref -
               p_instance_ctrl->signals.speed_mech_que_ref;

    f4_speed_ctrl_period = p_extended_cfg->data.outer_control_period;

    f4_temp1 =
        fminf((p_extended_cfg->data.speed_mech_change_limit) * f4_speed_ctrl_period, fabsf(f4_temp0));

    f4_speed_ref_calc_rad = p_instance_ctrl->signals.speed_mech_que_ref + copysignf(f4_temp1, f4_temp0);

    return f4_speed_ref_calc_rad;
}                                      /* End of function motor_outer_speed_rate_limit */

/***********************************************************************************************************************
 * Function Name : motor_outer_speed_pi
 * Description   : Speed PI control
 * Arguments     : p_ctrl       - Pointer to outer control instance
 *                 f_speed_rad  - Electrical speed [rad/s]
 * Return Value  : q-axis current reference [A]
 **********************************************************************************************************************/
static float motor_outer_speed_pi (motor_pm_foc_outer_instance_ctrl_t * p_ctrl, float f_speed_rad)
{
    float f4_iq_ref_calc;

    p_ctrl->signals.speed_pi_ctrl.f4_err = p_ctrl->signals.speed_mech_que_ref - f_speed_rad;
    f4_iq_ref_calc = rm_motor_pi_ctrl(&(p_ctrl->signals.speed_pi_ctrl));

    return f4_iq_ref_calc;
}                                      /* End of function motor_outer_speed_pi */

/***********************************************************************************************************************
 * Function Name : motor_outer_set_speed_ref
 * Description   : Updates the speed reference based on control mode
 * Arguments     : p_ctrl - Pointer to motor PM FOC instance control structure
 * Return Value  : Speed reference [rad/s]
 **********************************************************************************************************************/
static float motor_outer_set_speed_ref (motor_pm_foc_instance_ctrl_t * p_ctrl)
{
    float f4_speed_rad_ref_buff = 0.0F;

    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    uint8_t u1_control_mode = p_instance_ctrl->signals.inner_mode_data.mode_outer;

    switch (u1_control_mode)
    {
        case MOTOR_CTRLMODE_DEFAULT:
        case MOTOR_CTRLMODE_IDIQ_REF:
        case MOTOR_CTRLMODE_VDVQ_INJ:
        {
            f4_speed_rad_ref_buff = motor_outer_speed_rate_limit(p_ctrl);
            break;
        }

        case MOTOR_CTRLMODE_ID_UP_REF:
        case MOTOR_CTRLMODE_VDVQ_REF:
        case MOTOR_CTRLMODE_VUVW_REF:
        {
            f4_speed_rad_ref_buff = 0.0F;
            break;
        }
    }

    /* Speed reference limit */
    f4_speed_rad_ref_buff = rm_motor_fundlib_limitfabs(f4_speed_rad_ref_buff,
                                                       (p_extended_cfg->data.speed_mech_cmd_limit));

    /* Return speed reference */
    return f4_speed_rad_ref_buff;
}                                      /* End of function motor_outer_set_speed_ref */

/***********************************************************************************************************************
 * Function Name : motor_outer_set_i_ref
 * Description   : Updates the d/q-axis current references based on control mode
 * Arguments     : p_ctrl        - Pointer to motor PM FOC instance control structure
 *                 f4_id_ref_out - Pointer to output d-axis current reference [A]
 *                 f4_iq_ref_out - Pointer to output q-axis current reference [A]
 * Return Value  : None
 **********************************************************************************************************************/
static void motor_outer_set_i_ref (motor_pm_foc_instance_ctrl_t * p_ctrl, float * f4_id_ref_out, float * f4_iq_ref_out)
{
    float f4_id_ref_buff = 0.0F;
    float f4_iq_ref_buff = 0.0F;

    motor_pm_foc_extended_cfg_t        * p_extended_cfg  = (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl = p_ctrl->p_outer_instance_ctrl;

    uint8_t u1_control_mode = p_instance_ctrl->signals.inner_mode_data.mode_outer;

    /* Control mode selection using function pointer table */
    switch (u1_control_mode)
    {
        case MOTOR_CTRLMODE_DEFAULT:
        {
            /* Speed PI control */
            f4_iq_ref_buff = motor_outer_speed_pi(p_instance_ctrl, p_instance_ctrl->signals.speed_mech_lpf);

            /* Iq reference limit */
            f4_iq_ref_buff = rm_motor_fundlib_limitfabs(f4_iq_ref_buff,
                                                        p_extended_cfg->data.iq_limit * MOTOR_FUNDLIB_SQRT_3);

            /* MTPA control */
            /* Fluxweak control */
            f4_id_ref_buff = p_instance_ctrl->signals.inner_mode_data.id_ref_ow;

            break;
        }

        case MOTOR_CTRLMODE_IDIQ_REF:
        case MOTOR_CTRLMODE_ID_UP_REF:
        {
            f4_id_ref_buff = p_instance_ctrl->signals.inner_mode_data.id_ref_ow;
            f4_iq_ref_buff = p_instance_ctrl->signals.inner_mode_data.iq_ref_ow;
            p_instance_ctrl->signals.speed_pi_ctrl.f4_refi = f4_iq_ref_buff;
            break;
        }

        case MOTOR_CTRLMODE_VDVQ_INJ:
        case MOTOR_CTRLMODE_VDVQ_REF:
        case MOTOR_CTRLMODE_VUVW_REF:
        default:
        {
            // Nothing to do
            break;
        }
    }

    // Set id/iq current
    *(f4_id_ref_out) = f4_id_ref_buff;
    *(f4_iq_ref_out) = f4_iq_ref_buff;
}

/***********************************************************************************************************************
 * Function Name : motor_outer_reset_shared_data
 * Description   : Resets shared data structure to initial values
 * Arguments     : p_output - Pointer to shared data structure to be reset
 * Return Value  : None
 **********************************************************************************************************************/
static void motor_outer_reset_shared_data (motor_shared_outer_to_inner_foc_t * p_output)
{
    p_output->i_d_ref = 0.0F;
    p_output->i_q_ref = 0.0F;
    p_output->pos_speed_input_data.speed_mech_ref_ctrl  = 0.0F;
    p_output->pos_speed_input_data.speed_pi_status_flag = MOTOR_SENSORLESS_FLG_CLR;
}                                      /* End of function motor_outer_reset_shared_data */

/***********************************************************************************************************************
 * Function Name : rm_motor_outer_error_check
 * Description   : Detects errors in outer loop during cyclic execution
 * Arguments     : p_instance_ctrl - Pointer to outer control instance
 *                 p_extended_cfg  - Pointer to extended configuration
 *                 p_error_flags   - Pointer to error flags
 * Return Value  : None
 **********************************************************************************************************************/
static void motor_outer_error_check (motor_pm_foc_outer_instance_ctrl_t * p_instance_ctrl,
                                     motor_pm_foc_extended_cfg_t        * p_extended_cfg,
                                     uint16_t                           * p_error_flags)
{
    uint16_t error_flags = MOTOR_PM_FOC_ERROR_NONE;

    /* Speed-related error checks */
    float f_speed = p_instance_ctrl->signals.speed_mech;

    /* Over-speed error check */
    if (fabsf(f_speed) > p_extended_cfg->data.overspeed_limit)
    {
        error_flags |= MOTOR_PM_FOC_ERROR_OVER_SPEED;
    }

    /* Flux weakening control error check */
    if (p_extended_cfg->outerfunc_table.fw_ctrl != &algorithm_disabled)
    {
        if (p_instance_ctrl->signals.flux_weakening_ctrl.u2_fw_status & MOTOR_FLUXWKN_CHK_MASK)
        {
            error_flags |= MOTOR_PM_FOC_ERROR_FLUX_WEAKENING;
        }
    }

    /* Speed observer error check */
    if (p_extended_cfg->outerfunc_table.spdobsv_ctrl != &algorithm_disabled)
    {
        if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_spdobsrv_ctrl)
        {
            /* Normal speed observer error check */
            /* Implementation needed if applicable */
        }
        else if (p_extended_cfg->outerfunc_table.spdobsv_ctrl == &motor_outer_fnctbl_extobsrv_ctrl)
        {
            /* Disturbance observer error check */
            /* Implementation needed if applicable */
        }
        else
        {
            /* The other methods */
        }
    }

    /* If any error is detected, immediately stop the motor */
    if (error_flags != MOTOR_PM_FOC_ERROR_NONE)
    {
        /* Stop motor immediately */
        p_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;
    }

    p_instance_ctrl->signals.error_info |= error_flags;
    *p_error_flags |= error_flags;
}                                      /* End of function rm_motor_outer_error_check */

/***********************************************************************************************************************
 * Function Name : rm_motor_outer_error_get
 * Description   : Retrieves error flags from outer control instance
 * Arguments     : p_ctrl        - Pointer to outer control instance
 *                 p_error_flags - Pointer to error flags
 * Return Value  : None
 **********************************************************************************************************************/
void rm_motor_outer_error_get (motor_pm_foc_outer_instance_ctrl_t * p_ctrl, uint32_t * p_error_flags)
{
    *p_error_flags = p_ctrl->signals.error_info;
}                                      /* End of function rm_motor_outer_error_get */

/***********************************************************************************************************************
 * Function Name : motor_outer_trajectory_call
 * Description   : Calls trajectory callback to update speed reference
 * Arguments     : p_ctrl - Pointer to outer control instance
 * Return Value  : None
 **********************************************************************************************************************/
void motor_outer_trajectory_call (motor_pm_foc_outer_instance_ctrl_t * p_ctrl)
{
    motor_pm_foc_outer_cfg_t      * p_outer_cfg    = (motor_pm_foc_outer_cfg_t *) p_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t   * p_extended_cfg = (motor_pm_foc_extended_cfg_t *) p_outer_cfg->p_extend;
    motor_algorithm_callback_args_t temp_args_t;
    float speed_ref = 0.0F;
    float speed_rpm = 0.0F;

    /* Check if trajectory callback is registered */
    if (NULL != p_extended_cfg->p_callback)
    {
        speed_ref = p_ctrl->signals.speed_mech_ref * MOTOR_FUNDLIB_RAD2RPM; // rad/s to rpm
        speed_rpm = p_ctrl->signals.speed_mech * MOTOR_FUNDLIB_RAD2RPM;

        /* Call trajectory callback to update speed reference */
        temp_args_t.event                = MOTOR_ALGORITHM_EVENT_TRAJECTORY;
        temp_args_t.p_context            = p_outer_cfg->p_context;
        temp_args_t.p_speed_mech_ref_rpm = &speed_ref;
        temp_args_t.p_speed_mech_rpm     = &speed_rpm;
        (p_extended_cfg->p_callback)(&temp_args_t);

        /* Update the reference speed with trajectory output */
        p_ctrl->signals.speed_mech_ref = speed_ref * MOTOR_FUNDLIB_RPM2RAD; // rpm to rad/s
    }
} /* End of function motor_outer_trajectory_call */
