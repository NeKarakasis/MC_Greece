/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdio.h>
#include <bsp_api.h>
#include "rm_motor_sensor_bemf_cfg.h"
#include "rm_motor_sensor_bemf.h"
#include "rm_motor_mc_lib.h"
#include "rm_motor_sensor_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define     MOTOR_SENSOR_BEMF_OPEN    (0X42454D46ULL) /* "BEMF" */

#ifndef MOTOR_SENSOR_BEMF_ERROR_RETURN

 #define MOTOR_SENSOR_BEMF_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif

/* Id reference status */
#define     MOTOR_OPENLOOP_ID_UP                 (0)
#define     MOTOR_OPENLOOP_ID_CONST              (1)
#define     MOTOR_OPENLOOP_ID_DOWN               (2)
#define     MOTOR_OPENLOOP_ID_ZERO_CONST         (3)
#define     MOTOR_OPENLOOP_ID_FLUXWKN            (4)
#define     MOTOR_OPENLOOP_ID_OPENLOOP           (5)

/* Iq reference status */
#define     MOTOR_OPENLOOP_IQ_ZERO_CONST         (0)
#define     MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT    (1)
#define     MOTOR_OPENLOOP_IQ_AUTO_ADJ           (2)
#define     MOTOR_OPENLOOP_IQ_DOWN               (3)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

static float rm_motor_sensor_bemf_set_id_ref(motor_sensor_ctrl_t * p_ctrl);
static float rm_motor_sensor_bemf_set_iq_ref(motor_sensor_ctrl_t * p_ctrl);
static void  rm_motor_sensor_ol_dbg_mode(motor_sensor_ctrl_t * p_ctrl, motor_sensor_input_inner_t * p_inner);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
const motor_sensor_api_t g_motor_sensor_on_motor_sensor_bemf =
{
    .open             = RM_MOTOR_SENSOR_BEMF_Open,
    .close            = RM_MOTOR_SENSOR_BEMF_Close,
    .reset            = RM_MOTOR_SENSOR_BEMF_Reset,
    .parameterUpdate  = RM_MOTOR_SENSOR_BEMF_ParameterUpdate,
    .angleSpeedInit   = RM_MOTOR_SENSOR_BEMF_AngleSpeedInit,
    .angleSpeedCalc   = RM_MOTOR_SENSOR_BEMF_AngleSpeedCalc,
    .positionSpeedGet = RM_MOTOR_SENSOR_BEMF_PositionSpeedGet,
};

/*******************************************************************************************************************//**
 * @addtogroup MOTOR_SENSOR_BEMF
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Opens and configures the Angle Estimation module. Implements @ref motor_sensor_api_t::open.
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_Open
 *
 * @retval FSP_SUCCESS              MTR_ANGL_EST successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 * @retval FSP_ERR_INVALID_ARGUMENT Configuration parameter error.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_Open (motor_sensor_ctrl_t * const p_ctrl, motor_sensor_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_ctrl != NULL);
    FSP_ASSERT(p_cfg != NULL);
    motor_sensor_bemf_extended_cfg_t * p_extended_cfg = (motor_sensor_bemf_extended_cfg_t *) p_cfg->p_extend;
    FSP_ASSERT(p_extended_cfg != NULL);
#endif

    FSP_ERROR_RETURN(MOTOR_SENSOR_BEMF_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);

    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->signals.state_id_ref         = MOTOR_OPENLOOP_ID_UP;
    p_instance_ctrl->signals.state_iq_ref         = MOTOR_OPENLOOP_IQ_ZERO_CONST;
    p_instance_ctrl->signals.to_inner.mode_sensor = MOTOR_CTRLMODE_ID_UP_REF;

    p_instance_ctrl->signals.speed_el_lpf         = 0.0F;
    p_instance_ctrl->signals.flag_down_to_ol      = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_instance_ctrl->signals.inner_control_period = 0.0F;
    p_instance_ctrl->signals.outer_control_period = 0.0F;

    p_instance_ctrl->signals.angle_el             = 0.0F;
    p_instance_ctrl->signals.speed_pi_status_flag = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_instance_ctrl->signals.pll_status_flag      = MOTOR_FUNDLIB_FLAG_SET;
    p_instance_ctrl->signals.flag_id_up_executed  = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_instance_ctrl->open = MOTOR_SENSOR_BEMF_OPEN;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Disables specified Angle Estimation module. Implements @ref motor_sensor_api_t::close.
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_Close
 *
 * @retval FSP_SUCCESS              Successfully closed.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_Close (motor_sensor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
#endif

    rm_motor_bemf_obs_reset(&(p_instance_ctrl->signals.bemf_obs_ctrl));
    rm_motor_bemf_obs_pll_reset(&(p_instance_ctrl->signals.pll_est_ctrl));
    rm_motor_bemf_obs_reset(&(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl));
    rm_motor_bemf_obs_pll_reset(&(p_instance_ctrl->signals.ol_adjust_ctrl.pll_est_ctrl));
    p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_instance_ctrl->signals.speed_pi_status_flag = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_instance_ctrl->open = 0U;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Reset variables of Angle Estimation module. Implements @ref motor_sensor_api_t::reset
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_Reset
 *
 * @retval FSP_SUCCESS              Successfully reset.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_Reset (motor_sensor_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
#endif

    rm_motor_bemf_obs_reset(&(p_instance_ctrl->signals.bemf_obs_ctrl));
    rm_motor_bemf_obs_pll_reset(&(p_instance_ctrl->signals.pll_est_ctrl));
    rm_motor_bemf_obs_reset(&(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl));
    rm_motor_bemf_obs_pll_reset(&(p_instance_ctrl->signals.ol_adjust_ctrl.pll_est_ctrl));
    p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_instance_ctrl->signals.speed_el = 0.0F;
    p_instance_ctrl->signals.angle_el = 0.0F;

    p_instance_ctrl->signals.angle_err_el         = 0.0F;
    p_instance_ctrl->signals.e_d                  = 0.0F;
    p_instance_ctrl->signals.e_q                  = 0.0F;
    p_instance_ctrl->signals.speed_pi_status_flag = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_instance_ctrl->signals.pll_status_flag      = MOTOR_FUNDLIB_FLAG_SET;
    p_instance_ctrl->signals.state_id_ref         = MOTOR_OPENLOOP_ID_UP;
    p_instance_ctrl->signals.state_iq_ref         = MOTOR_OPENLOOP_IQ_ZERO_CONST;
    p_instance_ctrl->signals.to_inner.i_d_ref_ow  = 0.0F;
    p_instance_ctrl->signals.to_inner.mode_sensor = MOTOR_CTRLMODE_ID_UP_REF;

    rm_motor_opl_damp_reset(&(p_instance_ctrl->signals.openloop_ctrl));

    return err;
}

/*******************************************************************************************************************//**
 * @brief Update the parameters of Angle&Speed Estimation. Implements @ref motor_sensor_api_t::parameterUpdate
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_ParameterUpdate
 *
 * @retval FSP_SUCCESS              Successfully data is update.
 * @retval FSP_ERR_ASSERTION        Null pointer.
 * @retval FSP_ERR_NOT_OPEN         Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_ParameterUpdate (motor_sensor_ctrl_t * const      p_ctrl,
                                                motor_sensor_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ASSERT(p_cfg != NULL);
#endif
    motor_sensor_bemf_extended_cfg_t * p_extended_cfg = (motor_sensor_bemf_extended_cfg_t *) p_cfg->p_extend;

    if (FSP_SUCCESS == err)
    {
        p_instance_ctrl->p_cfg = p_cfg;

        rm_motor_bemf_obs_gain_calc(p_instance_ctrl->p_from_inner->p_motor_params,
                                    &(p_instance_ctrl->signals.bemf_obs_ctrl),
                                    p_extended_cfg->e_obs_omega,
                                    p_extended_cfg->e_obs_zeta,
                                    p_instance_ctrl->signals.inner_control_period);

        rm_motor_bemf_obs_pll_gain_calc(&(p_instance_ctrl->signals.pll_est_ctrl),
                                        p_extended_cfg->pll_est_omega,
                                        p_extended_cfg->pll_est_zeta,
                                        p_instance_ctrl->signals.inner_control_period);

        rm_motor_opl_damp_init(&(p_instance_ctrl->signals.openloop_ctrl),
                               p_extended_cfg->ol_sub_param.f_fb_speed_limit_rate);
        rm_motor_opl_damp_r_set_gain(&(p_instance_ctrl->signals.openloop_ctrl),
                                     p_instance_ctrl->p_from_inner->p_motor_params->u2_mtr_pp,
                                     p_instance_ctrl->p_from_inner->p_motor_params->f4_mtr_m,
                                     p_instance_ctrl->p_from_inner->p_motor_params->f4_mtr_j,
                                     p_extended_cfg->d_param.ol_damping_zeta,
                                     p_extended_cfg->d_param.ed_hpf_omega,
                                     p_extended_cfg->ol_param.f4_ol_id_ref,
                                     p_extended_cfg->ol_param.f4_id_down_speed_mech_rad * p_instance_ctrl->p_from_inner->p_motor_params->u2_mtr_pp,
                                     p_instance_ctrl->signals.outer_control_period);

        rm_motor_bemf_obs_gain_calc(p_instance_ctrl->p_from_inner->p_motor_params,
                                    &(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl),
                                    p_extended_cfg->e_obs_omega,
                                    p_extended_cfg->e_obs_zeta,
                                    p_instance_ctrl->signals.inner_control_period);

        rm_motor_bemf_obs_pll_gain_calc(&(p_instance_ctrl->signals.ol_adjust_ctrl.pll_est_ctrl),
                                        p_extended_cfg->pll_est_omega,
                                        p_extended_cfg->pll_est_zeta,
                                        p_instance_ctrl->signals.inner_control_period);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief Sets pointers to angle and speed variables, and configures extra data structures for the BEMF observer.
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_AngleSpeedInit
 *
 * This function initializes pointer outputs for speed and angle, sets up shared data pointers for communication
 * between control layers, and updates motor parameters from the inner loop input. It also triggers parameter
 * recalculation for the observer.
 *
 * @param[in]  p_ctrl                 Pointer to motor angle control structure.
 * @param[out] pp_speed               Pointer to output variable for speed pointer.
 * @param[out] pp_angle               Pointer to output variable for angle pointer.
 * @param[out] p_out_extra_data       Pointer to extra output data structure pointer.
 *
 * @retval FSP_SUCCESS                Successfully set pointers.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_AngleSpeedInit (motor_sensor_ctrl_t * const          p_ctrl,
                                               float                             ** pp_speed,
                                               float                             ** pp_angle,
                                               motor_sensor_data_exchange_t * const p_out_extra_data)
{
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ASSERT(p_out_extra_data != NULL);
    FSP_ASSERT(pp_speed != NULL);
    FSP_ASSERT(pp_angle != NULL);
    FSP_ERROR_RETURN(MOTOR_SENSOR_BEMF_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif
    const motor_shared_sensor_data_exchange_t * p_extra_data =
        (motor_shared_sensor_data_exchange_t *) p_out_extra_data;
    const motor_shared_inner_to_position_speed_t * p_from_inner =
        (const motor_shared_inner_to_position_speed_t *) p_extra_data->p_in_extra_data;

    // Set the pointers
    *pp_speed = &(p_instance_ctrl->signals.speed_el);
    *pp_angle = &(p_instance_ctrl->signals.angle_el);

    // Set the pointers to the extra data to inner loop
    *(p_extra_data->pp_out_extra_data) = &(p_instance_ctrl->signals.to_inner);

    // Initial configuration from inner
    p_instance_ctrl->p_from_inner                 = p_from_inner;
    p_instance_ctrl->signals.inner_control_period = p_from_inner->inner_control_period;
    p_instance_ctrl->signals.outer_control_period = p_from_inner->outer_control_period;

    // Recalculate parameters
    RM_MOTOR_SENSOR_BEMF_ParameterUpdate(p_ctrl, p_instance_ctrl->p_cfg);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Calculates angle and speed using BEMF observer and open-loop estimator.
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_AngleSpeedCalc
 *
 * This function estimates the rotor angle and speed based on motor feedback and control mode.
 * It updates internal references and manages open-loop/PI transitions.
 *
 * @param[in]  p_ctrl      Pointer to motor angle control structure.
 * @param[in]  p_input_inner     Pointer to inner loop input data.
 * @param[in]  p_input_outer     Pointer to outer loop input data.
 *
 * @retval FSP_SUCCESS         Calculation successful.
 * @retval FSP_ERR_ASSERTION   Null pointer or not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_AngleSpeedCalc (motor_sensor_ctrl_t * const        p_ctrl,
                                               motor_sensor_input_inner_t * const p_input_inner,
                                               motor_sensor_input_outer_t * const p_input_outer)
{
    float     angle_el_temp;
    float     f4_temp0;
    fsp_err_t err = FSP_SUCCESS;
    motor_sensor_bemf_instance_ctrl_t      * p_instance_ctrl   = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;
    motor_shared_inner_to_outer_foc_t      * p_from_inner_data = (motor_shared_inner_to_outer_foc_t *) p_input_inner;
    motor_shared_outer_to_position_speed_t * p_outer_data      =
        (motor_shared_outer_to_position_speed_t *) p_input_outer;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ASSERT(p_input_inner != NULL);
    FSP_ASSERT(p_input_outer != NULL);
    FSP_ERROR_RETURN(MOTOR_SENSOR_BEMF_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    motor_parameter_ramp_t * p_motor_params = p_instance_ctrl->p_from_inner->p_motor_params;

    if (FSP_SUCCESS == err)
    {
        if (p_instance_ctrl->signals.ol_adjust_ctrl.activate_ol_adjust_mode == MOTOR_FUNDLIB_FLAG_SET)
        {
            rm_motor_sensor_ol_dbg_mode(p_instance_ctrl, p_from_inner_data);
        }

        motor_sensor_bemf_extended_cfg_t * p_extended_cfg =
            (motor_sensor_bemf_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

        /* Estimate voltage disturbance */
        rm_motor_bemf_obs_observer(&(p_instance_ctrl->signals.bemf_obs_ctrl),
                                   p_motor_params,
                                   p_from_inner_data->v_d_ref,
                                   p_from_inner_data->v_q_ref,
                                   p_from_inner_data->i_d,
                                   p_from_inner_data->i_q);

        /* Calculate BEMF */
        p_instance_ctrl->signals.e_d = rm_motor_bemf_obs_calc_d(&(p_instance_ctrl->signals.bemf_obs_ctrl),
                                                                p_motor_params,
                                                                p_instance_ctrl->signals.speed_el,
                                                                p_from_inner_data->i_q);
        p_instance_ctrl->signals.e_q = rm_motor_bemf_obs_calc_q(&(p_instance_ctrl->signals.bemf_obs_ctrl),
                                                                p_motor_params,
                                                                p_instance_ctrl->signals.speed_el,
                                                                p_from_inner_data->i_d);

        /* Calculate phase error */
        p_instance_ctrl->signals.angle_err_el =
            atan2f(p_instance_ctrl->signals.e_d / p_instance_ctrl->signals.e_q, 1.0F);

        /* Prevent phase error becoming NAN when eq is 0 */
        if (isnan(p_instance_ctrl->signals.angle_err_el))
        {
            p_instance_ctrl->signals.angle_err_el = 0.0F;
        }

        if (MOTOR_FUNDLIB_FLAG_SET == p_outer_data->speed_pi_status_flag)
        {
            /* Angle calculated by BEMF observer */
            if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->signals.pll_status_flag)
            {
                p_instance_ctrl->signals.pll_est_ctrl.f4_i_est_speed = p_outer_data->speed_mech_ref_ctrl *
                                                                       p_motor_params->u2_mtr_pp;
                p_instance_ctrl->signals.pll_status_flag = MOTOR_FUNDLIB_FLAG_CLEAR;
            }

            /* Estimate angle and speed */
            rm_motor_bemf_obs_pll(&(p_instance_ctrl->signals.pll_est_ctrl), p_instance_ctrl->signals.angle_err_el,
                                  &(p_instance_ctrl->signals.speed_el));
        }
        else
        {
            /* Angle calculated by Openloop */
            p_instance_ctrl->signals.pll_status_flag = MOTOR_FUNDLIB_FLAG_SET;

            if (MOTOR_MCSENSOR_OPENLOOP_DAMPING_ENABLE == p_extended_cfg->openloop_damping)
            {
                p_instance_ctrl->signals.speed_el =
                    (p_outer_data->speed_mech_ref_ctrl * p_motor_params->u2_mtr_pp) -
                    p_instance_ctrl->signals.speed_el_damp_comp;
            }
            else
            {
                p_instance_ctrl->signals.speed_el = p_outer_data->speed_mech_ref_ctrl * p_motor_params->u2_mtr_pp;
            }
        }

        /* 1/s */
        angle_el_temp  = p_instance_ctrl->signals.angle_el;
        angle_el_temp += (p_instance_ctrl->signals.speed_el * (p_instance_ctrl->signals.inner_control_period));

        if (angle_el_temp >= MOTOR_FUNDLIB_TWOPI)
        {
            angle_el_temp = angle_el_temp - MOTOR_FUNDLIB_TWOPI;
        }
        else if (angle_el_temp < 0.0F)
        {
            angle_el_temp = angle_el_temp + MOTOR_FUNDLIB_TWOPI;
        }
        else
        {
            /* Do nothing */
        }

        p_instance_ctrl->signals.angle_el = angle_el_temp;

        /* Speed command reference */
        (p_instance_ctrl)->signals.speed_ref_el_ctrl = p_outer_data->speed_mech_ref_ctrl * p_motor_params->u2_mtr_pp;

        /* LPF for phase error */
        p_instance_ctrl->signals.phase_err_rad_lpf = p_instance_ctrl->signals.angle_err_el;

        /* LPF for speed */
        p_instance_ctrl->signals.speed_el_lpf = p_instance_ctrl->signals.speed_el;

        if (MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT == p_instance_ctrl->signals.state_iq_ref)
        {
            /* f4_temp0 : The absolute value of speed command [rad/s] */
            f4_temp0 = fabsf(p_instance_ctrl->signals.speed_el_lpf);

            // Changing to Openloop
            if (f4_temp0 < (p_extended_cfg->ol_param.f4_id_up_speed_mech_rad * p_motor_params->u2_mtr_pp))
            {
                p_instance_ctrl->signals.flag_down_to_ol   = MOTOR_FUNDLIB_FLAG_SET;
                p_instance_ctrl->signals.speed_ref_el_ctrl = p_instance_ctrl->signals.speed_el_lpf;
            }
        }

        // Store Id*/Iq* reference
        float f4_id0_ref = 0.0F;
        float f4_iq0_ref = 0.0F;

        f4_id0_ref = rm_motor_sensor_bemf_set_id_ref((motor_sensor_bemf_instance_ctrl_t *) p_ctrl);
        f4_iq0_ref = rm_motor_sensor_bemf_set_iq_ref((motor_sensor_bemf_instance_ctrl_t *) p_ctrl);

        // Apply to internal id/iq ref buffer
        p_instance_ctrl->signals.to_inner.i_d_ref_ow = f4_id0_ref;
        p_instance_ctrl->signals.to_inner.i_q_ref_ow = f4_iq0_ref;

        // Control Mode: switching between Openloop Id/Iq ref and PI mode
        if (p_instance_ctrl->signals.state_id_ref == MOTOR_OPENLOOP_ID_UP)
        {
            if (MOTOR_FUNDLIB_FLAG_CLEAR == p_instance_ctrl->signals.flag_id_up_executed)
            {
                p_instance_ctrl->signals.to_inner.mode_sensor = MOTOR_CTRLMODE_ID_UP_REF;
                p_instance_ctrl->signals.flag_id_up_executed  = MOTOR_FUNDLIB_FLAG_SET;
            }
        }
        else
        {
            switch (p_instance_ctrl->signals.state_iq_ref)
            {
                case MOTOR_OPENLOOP_IQ_ZERO_CONST:
                case MOTOR_OPENLOOP_IQ_AUTO_ADJ:
                case MOTOR_OPENLOOP_IQ_DOWN:
                {
                    p_instance_ctrl->signals.to_inner.mode_sensor = MOTOR_CTRLMODE_IDIQ_REF;
                    break;
                }

                case MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT:
                {
                    p_instance_ctrl->signals.to_inner.mode_sensor = MOTOR_CTRLMODE_DEFAULT;
                    break;
                }
            }
        }

        p_instance_ctrl->signals.flag_down_to_ol = MOTOR_FUNDLIB_FLAG_CLEAR;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief Retrieves the rotor position and speed in mechanical [rad] and [rad/s].
 *
 * Example:
 * @snippet rm_motor_sensor_bemf_example.c RM_MOTOR_SENSOR_BEMF_PositionSpeedGet
 *
 * This function calculates the mechanical speed from the internal electrical speed state,
 * converting it using the motor's pole pair count. The position output is currently not implemented.
 *
 * @param[in]  p_ctrl   Pointer to the motor sensor control instance.
 * @param[out] p_pos    Pointer to store the calculated mechanical position [rad]. (Currently not implemented.)
 * @param[out] p_speed  Pointer to store the calculated mechanical speed [rad/s].
 *
 * @retval FSP_SUCCESS         Calculation successful.
 * @retval FSP_ERR_ASSERTION   Null pointer or not open.
 **********************************************************************************************************************/
fsp_err_t RM_MOTOR_SENSOR_BEMF_PositionSpeedGet (motor_sensor_ctrl_t * const p_ctrl,
                                                 float * const               p_pos,
                                                 float * const               p_speed)
{
    fsp_err_t err = FSP_SUCCESS;

    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;

#if (RM_MOTOR_SENSOR_BEMF_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl != NULL);
    FSP_ASSERT(p_pos != NULL);
    FSP_ASSERT(p_speed != NULL);
#endif

    motor_parameter_ramp_t * p_motor_params = p_instance_ctrl->p_from_inner->p_motor_params;

    *p_speed = p_instance_ctrl->signals.speed_el / p_motor_params->u2_mtr_pp; // [rad/s mech]

    *p_pos = 0.0F;

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup MOTOR_SENSOR_BEMF)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Updates the q-axis current reference depending on the open-loop/PI state.
 *
 * This function calculates and updates the Iq reference value based on the current state machine
 * for open-loop and PI mode transitions. It applies limits and handles state transitions.
 *
 * @param[in]  p_ctrl   Pointer to the motor angle control structure.
 * @return     Iq reference value.
 **********************************************************************************************************************/
static float rm_motor_sensor_bemf_set_iq_ref (motor_sensor_ctrl_t * p_ctrl)
{
    float   i_q_ref_buf = 0.0F;
    float   temp_value  = 0.0F;
    uint8_t temp_flag   = MOTOR_FUNDLIB_FLAG_CLEAR;

    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;
    motor_sensor_bemf_extended_cfg_t  * p_extended_cfg  =
        (motor_sensor_bemf_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->signals.flag_down_to_ol)
    {
        p_instance_ctrl->signals.state_iq_ref    = MOTOR_OPENLOOP_IQ_DOWN;
        p_instance_ctrl->signals.ol_iq_down_step = (p_extended_cfg->ol_param.f4_ol_iq_down_step_ratio) *
                                                   MOTOR_FUNDLIB_DIV_8BIT *
                                                   p_instance_ctrl->signals.to_inner.i_q_ref_ow;
    }

    switch (p_instance_ctrl->signals.state_iq_ref)
    {
        case MOTOR_OPENLOOP_IQ_ZERO_CONST:
        {
            /* Iq command will always be zero in this state */
            i_q_ref_buf = 0.0F;

            temp_value = fabsf(p_instance_ctrl->signals.speed_ref_el_ctrl);

            if (MOTOR_MCSENSOR_OPENLOOP_DAMPING_ENABLE == p_extended_cfg->openloop_damping)
            {
                /* Open loop damping control*/
                p_instance_ctrl->signals.speed_el_damp_comp =
                    rm_motor_opl_damp_ctrl(&(p_instance_ctrl->signals.openloop_ctrl),
                                           p_instance_ctrl->signals.e_d,
                                           p_instance_ctrl->signals.speed_ref_el_ctrl);
            }

            if (temp_value >=
                (p_extended_cfg->ol_param.f4_id_down_speed_mech_rad *
                 p_instance_ctrl->p_from_inner->p_motor_params->u2_mtr_pp))
            {
                /* State transient to next state */
                if (MOTOR_MCSENSOR_LESS_SWITCH_ENABLE == p_extended_cfg->less_switch)
                {
                    p_instance_ctrl->signals.state_iq_ref       = MOTOR_OPENLOOP_IQ_AUTO_ADJ;
                    p_instance_ctrl->signals.opl_torque_current =
                        rm_motor_opl2less_torque_current_calc(p_instance_ctrl->p_from_inner->p_motor_params,
                                                              p_extended_cfg->ol_param.f4_opl2less_sw_time,
                                                              p_extended_cfg->ol_param.f4_ol_id_ref,
                                                              p_instance_ctrl->signals.phase_err_rad_lpf);
                }
                else
                {
                    p_instance_ctrl->signals.state_iq_ref = MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT;
                }

                /* Openloop Damping is active */
                if (MOTOR_FUNDLIB_FLAG_SET == p_extended_cfg->openloop_damping)
                {
                    /* Open-loop damping control reset */
                    rm_motor_opl_damp_reset(&(p_instance_ctrl->signals.openloop_ctrl));
                    p_instance_ctrl->signals.speed_el_damp_comp = 0.0F;
                }
            }

            break;
        }

        case MOTOR_OPENLOOP_IQ_AUTO_ADJ:
        {
            /* Iq auto adjustment */
            i_q_ref_buf = rm_motor_opl2less_iq_calc(p_instance_ctrl->signals.e_d,
                                                    p_instance_ctrl->signals.e_q,
                                                    p_instance_ctrl->signals.to_inner.i_d_ref_ow,
                                                    p_instance_ctrl->signals.opl_torque_current,
                                                    p_instance_ctrl->signals.angle_err_el);

            /* Limit Iq reference */
            i_q_ref_buf =
                rm_motor_fundlib_limitfabs(i_q_ref_buf,
                                           p_instance_ctrl->p_from_inner->p_motor_params->f4_mtr_nominal_current *
                                           MOTOR_FUNDLIB_SQRT_3);

            if (0.0F <= p_instance_ctrl->signals.speed_el_lpf)
            {
                temp_flag =
                    (p_instance_ctrl->signals.angle_err_el <=
                     (p_extended_cfg->ol_param.f4_switch_phase_err_rad) * MOTOR_FUNDLIB_RAD_TRANS);
            }
            else if (0.0F > p_instance_ctrl->signals.speed_el_lpf)
            {
                temp_flag =
                    (p_instance_ctrl->signals.angle_err_el >=
                     -(p_extended_cfg->ol_param.f4_switch_phase_err_rad) * MOTOR_FUNDLIB_RAD_TRANS);
            }
            else
            {
                /* Do nothing */
            }

            if (MOTOR_FUNDLIB_FLAG_SET == temp_flag)
            {
                p_instance_ctrl->signals.state_iq_ref       = MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT;
                p_instance_ctrl->signals.init_phase_err_rad = 0.0F;

                /* Preset for suppress speed hunting */
                p_instance_ctrl->signals.speed_ref_el_ctrl = p_instance_ctrl->signals.speed_el_lpf;
            }

            break;
        }

        case MOTOR_OPENLOOP_IQ_SPEED_PI_OUTPUT:
        {
            // Nothing to do
            break;
        }

        case MOTOR_OPENLOOP_IQ_DOWN:
        {
            i_q_ref_buf = p_instance_ctrl->signals.to_inner.i_q_ref_ow - p_instance_ctrl->signals.ol_iq_down_step;
            temp_value  = p_instance_ctrl->signals.ol_iq_down_step * i_q_ref_buf; /* Check sign */
            if (temp_value <= 0.0F)
            {
                i_q_ref_buf = 0.0F;
                p_instance_ctrl->signals.state_iq_ref = MOTOR_OPENLOOP_IQ_ZERO_CONST;
            }

            break;
        }

        default:
        {
            /* Do noting */
            break;
        }
    }

    /* Return iq reference */
    return i_q_ref_buf;
}                                      /* End of function rm_motor_speed_set_iq_ref */

/*******************************************************************************************************************//**
 * @brief Updates the d-axis current reference during open-loop operation.
 *
 * This function manages the state machine for the d-axis current reference (Id) in open-loop mode.
 * It handles ramp-up, constant, ramp-down, and zero/flux-weakening states, adjusting the Id reference
 * value according to the cmurrent state, step sizes, and thresholds configured in the extended parameters.
 * State transitions occur based on speed and Id reference limits.
 *
 * @param[in]  p_ctrl   Pointer to the motor sensor control instance.
 * @return     float    Updated Id reference value.
 **********************************************************************************************************************/
static float rm_motor_sensor_bemf_set_id_ref (motor_sensor_ctrl_t * p_ctrl)
{
    float temp_value  = 0.0F;
    float i_d_ref_buf = 0.0F;

    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;
    motor_sensor_bemf_extended_cfg_t  * p_extended_cfg  =
        (motor_sensor_bemf_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->signals.flag_down_to_ol)
    {
        p_instance_ctrl->signals.state_id_ref = MOTOR_OPENLOOP_ID_UP;
    }

    switch (p_instance_ctrl->signals.state_id_ref)
    {
        case MOTOR_OPENLOOP_ID_UP:
        {
            i_d_ref_buf = p_instance_ctrl->signals.to_inner.i_d_ref_ow + p_extended_cfg->ol_param.f4_ol_id_up_step;
            if (i_d_ref_buf >= p_extended_cfg->ol_param.f4_ol_id_ref)
            {
                i_d_ref_buf = p_extended_cfg->ol_param.f4_ol_id_ref;
                p_instance_ctrl->signals.state_id_ref = MOTOR_OPENLOOP_ID_CONST;
            }

            break;
        }

        case MOTOR_OPENLOOP_ID_CONST:
        {
            i_d_ref_buf = p_instance_ctrl->signals.to_inner.i_d_ref_ow;
            temp_value  = fabsf(p_instance_ctrl->signals.speed_ref_el_ctrl);

            if (temp_value >=
                p_extended_cfg->ol_param.f4_id_down_speed_mech_rad *
                p_instance_ctrl->p_from_inner->p_motor_params->u2_mtr_pp)
            {
                p_instance_ctrl->signals.state_id_ref = MOTOR_OPENLOOP_ID_DOWN;
            }

            break;
        }

        case MOTOR_OPENLOOP_ID_DOWN:
        {
            i_d_ref_buf = p_instance_ctrl->signals.to_inner.i_d_ref_ow - p_extended_cfg->ol_param.f4_ol_id_down_step;
            if (0.0F >= i_d_ref_buf)
            {
                i_d_ref_buf = 0.0F;
                p_instance_ctrl->signals.state_id_ref = MOTOR_OPENLOOP_ID_ZERO_CONST;
            }

            break;
        }

        case MOTOR_OPENLOOP_ID_ZERO_CONST:
        case MOTOR_OPENLOOP_ID_FLUXWKN:
        {
            i_d_ref_buf = 0.0F;

            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    /* Return id reference */
    return i_d_ref_buf;
}                                      /* End of function rm_motor_mcsensor_set_id_ref */

/*******************************************************************************************************************//**
 * @brief Executes the Back-EMF Observer and PLL in open loop mode for tuning and debug purposes.
 *
 * This function is intended for use during the tuning and debugging of open loop motor control. It transforms
 * measured currents and voltages from the dq-frame to the abc-frame using the forced angle, then back to the dq-frame
 * using the PLL-estimated angle. It then runs the back-EMF observer and PLL estimators using these transformed variables.
 * If the PLL engage conditions are met (based on speed or engage flag), the PLL and observer are executed and the
 * estimated angle, speed, and back-EMF are updated. Otherwise, the PLL and observer state variables are reset.
 *
 * @param[in]  p_ctrl   Pointer to the instance control structure.
 * @param[in]  p_inner  Pointer to the structure holding inner control loop data.
 *
 * @retval     None
 **********************************************************************************************************************/
static void rm_motor_sensor_ol_dbg_mode (motor_sensor_ctrl_t * p_ctrl, motor_sensor_input_inner_t * p_inner)
{
    motor_sensor_bemf_instance_ctrl_t * p_instance_ctrl   = (motor_sensor_bemf_instance_ctrl_t *) p_ctrl;
    motor_shared_inner_to_outer_foc_t * p_from_inner_data = (motor_shared_inner_to_outer_foc_t *) p_inner;
    motor_parameter_ramp_t            * p_motor_params    = p_instance_ctrl->p_from_inner->p_motor_params;
    float temp_uvw[3];
    float idq[2];
    float vdq[2];
    float angle_el_temp;

    // go back to abc and recalculate the dq with the PLL angle (only if speed is higher than the PLL start threshold)

    if ((p_instance_ctrl->signals.speed_el >= p_instance_ctrl->signals.ol_adjust_ctrl.pll_start_speed) ||
        (p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status == MOTOR_FUNDLIB_FLAG_SET))
    {
        if (p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status == MOTOR_FUNDLIB_FLAG_CLEAR)
        {
            p_instance_ctrl->signals.ol_adjust_ctrl.pll_engage_status = MOTOR_FUNDLIB_FLAG_SET;
        }

        // Apply inverse Park Clarke with the force angle - currents
        rm_motor_transform_dq_uvw_abs(p_instance_ctrl->signals.angle_el, &(p_from_inner_data->i_d), &temp_uvw[0]);

        // Apply Clarke Park  with the PLL angle
        rm_motor_transform_uvw_dq_abs(p_instance_ctrl->signals.ol_adjust_ctrl.angle_el_pll, &temp_uvw[0], &idq[0]);

        // Apply inverse Park Clarke with the force angle - voltages
        rm_motor_transform_dq_uvw_abs(p_instance_ctrl->signals.angle_el, &(p_from_inner_data->v_d_ref), &temp_uvw[0]);

        // Apply Clarke Park  with the PLL angle
        rm_motor_transform_uvw_dq_abs(p_instance_ctrl->signals.ol_adjust_ctrl.angle_el_pll, &temp_uvw[0], &vdq[0]);

        // execute Observer and PLL

        /* Estimate voltage disturbance */
        rm_motor_bemf_obs_observer(&(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl),
                                   p_motor_params,
                                   vdq[0],
                                   vdq[1],
                                   idq[0],
                                   idq[1]);

        /* Calculate BEMF */
        p_instance_ctrl->signals.ol_adjust_ctrl.e_d =
            rm_motor_bemf_obs_calc_d(&(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl),
                                     p_motor_params,
                                     p_instance_ctrl->signals.ol_adjust_ctrl.speed_el_pll,
                                     idq[1]);
        p_instance_ctrl->signals.ol_adjust_ctrl.e_q =
            rm_motor_bemf_obs_calc_q(&(p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl),
                                     p_motor_params,
                                     p_instance_ctrl->signals.ol_adjust_ctrl.speed_el_pll,
                                     idq[0]);

        /* Calculate phase error */
        p_instance_ctrl->signals.ol_adjust_ctrl.angle_err_el = atan2f(
            p_instance_ctrl->signals.ol_adjust_ctrl.e_d / p_instance_ctrl->signals.ol_adjust_ctrl.e_q,
            1.0F);

        /* Prevent phase error becoming NAN when eq is 0 */
        if (isnan(p_instance_ctrl->signals.ol_adjust_ctrl.angle_err_el))
        {
            p_instance_ctrl->signals.ol_adjust_ctrl.angle_err_el = 0.0F;
        }

        /* Estimate angle and speed */
        rm_motor_bemf_obs_pll(&(p_instance_ctrl->signals.ol_adjust_ctrl.pll_est_ctrl),
                              p_instance_ctrl->signals.ol_adjust_ctrl.angle_err_el,
                              &(p_instance_ctrl->signals.ol_adjust_ctrl.speed_el_pll));

        /* 1/s */
        angle_el_temp  = p_instance_ctrl->signals.ol_adjust_ctrl.angle_el_pll;
        angle_el_temp +=
            (p_instance_ctrl->signals.ol_adjust_ctrl.speed_el_pll * (p_instance_ctrl->signals.inner_control_period));

        if (angle_el_temp >= MOTOR_FUNDLIB_TWOPI)
        {
            angle_el_temp = angle_el_temp - MOTOR_FUNDLIB_TWOPI;
        }
        else if (angle_el_temp < 0.0F)
        {
            angle_el_temp = angle_el_temp + MOTOR_FUNDLIB_TWOPI;
        }
        else
        {
            /* Do nothing */
        }

        p_instance_ctrl->signals.ol_adjust_ctrl.angle_el_pll = angle_el_temp;
    }
    else
    {
        // keep resetting PLL quantities and BEMF observer
        p_instance_ctrl->signals.ol_adjust_ctrl.pll_est_ctrl.f4_i_est_speed = p_instance_ctrl->signals.speed_el;
        p_instance_ctrl->signals.ol_adjust_ctrl.speed_el_pll                = p_instance_ctrl->signals.speed_el;
        p_instance_ctrl->signals.ol_adjust_ctrl.angle_el_pll                = p_instance_ctrl->signals.angle_el;

        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_d_axis.f4_d_est_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_d_axis.f4_d_est_pre;
        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_d_axis.f4_i_est_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_d_axis.f4_i_est_pre;
        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_d_axis.f4_i_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_d_axis.f4_i_pre;

        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_q_axis.f4_d_est_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_q_axis.f4_d_est_pre;
        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_q_axis.f4_i_est_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_q_axis.f4_i_est_pre;
        p_instance_ctrl->signals.ol_adjust_ctrl.bemf_obs_ctrl.st_q_axis.f4_i_pre =
            p_instance_ctrl->signals.bemf_obs_ctrl.st_q_axis.f4_i_pre;
    }
}
