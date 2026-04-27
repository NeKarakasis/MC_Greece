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

#define MOTOR_INNER_NUM_0P5    0.5F

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
void rm_motor_inner_cyclic(motor_hal_driver_callback_args_t * p_args);

static void motor_inner_phase_voltage_get(motor_pm_foc_instance_ctrl_t    * p_ctrl,
                                          motor_inner_get_voltage_t * const p_voltage);
static void motor_inner_angle_cyclic(motor_pm_foc_inner_instance_ctrl_t * p_ctrl);

static void motor_inner_reset(motor_pm_foc_inner_instance_ctrl_t * p_ctrl);

static void motor_inner_pi_calculation(motor_pm_foc_inner_instance_ctrl_t * p_ctrl);
static void motor_inner_reset_shared_data(motor_shared_inner_to_outer_foc_t * p_output);
static void rm_motor_inner_error_check(motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl,
                                       const motor_algorithm_cfg_t        * p_cfg,
                                       uint32_t                           * p_error_flags);
static fsp_err_t motor_inner_handle_tuner(motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl,
                                          motor_tuner_instance_t const       * p_tuner_instance,
                                          motor_hal_driver_instance_t const  * p_driver_instance);

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
 * @brief Opens and configures the Motor Inner Control Module.
 *
 * This function initializes the inner control instance, including shared data buffers, sensor instances,
 * PI controller parameters, voltage error compensation, and HAL driver. It also resets shared data structures
 * and clears error flags.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_init (motor_pm_foc_instance_ctrl_t * const p_ctrl)
{
    motor_shared_pos_speed_to_inner_t * p_dummy_monitor_out = NULL; // dummy pointer for sensor extra_output in monitor mode
    motor_shared_sensor_data_exchange_t p_sensor_data_exchange;
    motor_algorithm_cfg_t             * p_cfg          = (motor_algorithm_cfg_t *) p_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t       * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl->p_inner_instance_ctrl;

    p_inner_instance_ctrl->p_hal_driver_instance = p_cfg->p_motor_hal_driver_instance;
    p_inner_instance_ctrl->p_sensor_instance     = p_cfg->p_motor_sensor_instance;
    p_inner_instance_ctrl->p_sensor_instance2    = p_cfg->p_motor_sensor_instance2;
    p_inner_instance_ctrl->p_cfg                 = p_extended_cfg->p_motor_inner_cfg;

    // set up the shared data buffers (inner to outer, outer to inner)
    p_inner_instance_ctrl->p_from_outer      = &(p_extended_cfg->p_shared_data->shared_outer_to_inner_A);
    p_inner_instance_ctrl->p_to_outer[0]     = &(p_extended_cfg->p_shared_data->shared_inner_to_outer_A);
    p_inner_instance_ctrl->p_to_outer[1]     = &(p_extended_cfg->p_shared_data->shared_inner_to_outer_B);
    p_inner_instance_ctrl->p_to_outer_active = p_inner_instance_ctrl->p_to_outer[0]; /* active : st_shared_inner_to_outer_A */
    p_extended_cfg->p_shared_data->pp_shared_inner_to_outer_active =
        (void **) &(p_inner_instance_ctrl->p_to_outer_active);

    motor_inner_reset(p_inner_instance_ctrl);

    p_inner_instance_ctrl->signals.id_pi_ctrl.f4_ilimit = p_extended_cfg->data.current_limit;
    p_inner_instance_ctrl->signals.iq_pi_ctrl.f4_ilimit = p_extended_cfg->data.current_limit;

    rm_motor_pi_gain_calc(p_extended_cfg->pm_motor_parameter,
                          p_extended_cfg->pm_inner_design_parameter,
                          &(p_inner_instance_ctrl->signals.id_pi_ctrl),
                          &(p_inner_instance_ctrl->signals.iq_pi_ctrl),
                          p_extended_cfg->data.inner_control_period);

    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_v[0]             = p_extended_cfg->data.comp_v[0];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_v[1]             = p_extended_cfg->data.comp_v[1];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_v[2]             = p_extended_cfg->data.comp_v[2];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_v[3]             = p_extended_cfg->data.comp_v[3];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_v[4]             = p_extended_cfg->data.comp_v[4];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_i[0]             = p_extended_cfg->data.comp_i[0];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_i[1]             = p_extended_cfg->data.comp_i[1];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_i[2]             = p_extended_cfg->data.comp_i[2];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_i[3]             = p_extended_cfg->data.comp_i[3];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_i[4]             = p_extended_cfg->data.comp_i[4];
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_vdc_ref          = p_extended_cfg->data.comp_vdc_ref;
    p_inner_instance_ctrl->signals.vcomp_ctrl.f_comp_limit_factor     = p_extended_cfg->data.comp_limit_factor;
    p_inner_instance_ctrl->signals.vcomp_ctrl.u1_volt_err_comp_enable = p_extended_cfg->data.vcomp_enable;
    rm_motor_volt_error_comp_init(&(p_inner_instance_ctrl->signals.vcomp_ctrl));

    /* Open Motor Driver Access Module */
    if (p_inner_instance_ctrl->p_hal_driver_instance != NULL)
    {
        p_inner_instance_ctrl->p_hal_driver_instance->p_api->open(p_inner_instance_ctrl->p_hal_driver_instance->p_ctrl,
                                                                  p_inner_instance_ctrl->p_hal_driver_instance->p_cfg);
    }

    /* Handover cotrol period */
    p_inner_instance_ctrl->signals.to_pos_speed.inner_control_period = p_extended_cfg->data.inner_control_period;
    p_inner_instance_ctrl->signals.to_pos_speed.outer_control_period = p_extended_cfg->data.outer_control_period;

    /* Handover Motor Parameter*/
    p_inner_instance_ctrl->signals.to_pos_speed.p_motor_params = p_extended_cfg->pm_motor_parameter;

    /* Open Motor Angle Module */
    if (p_inner_instance_ctrl->p_sensor_instance != NULL)
    {
        // Slot 1
        p_inner_instance_ctrl->p_sensor_instance->p_api->open(p_inner_instance_ctrl->p_sensor_instance->p_ctrl,
                                                              p_inner_instance_ctrl->p_sensor_instance->p_cfg);

        if (p_extended_cfg->data.sensor_slot == MOTOR_INNER_SLOT_1_ENABLE)
        {
            p_sensor_data_exchange.p_in_extra_data   = &p_inner_instance_ctrl->signals.to_pos_speed;
            p_sensor_data_exchange.pp_out_extra_data =
                (motor_shared_pos_speed_to_inner_t **) &p_inner_instance_ctrl->p_from_pos_speed;

            /* Set the pointers of the output buffer to the angle instance */
            p_inner_instance_ctrl->p_sensor_instance->p_api->angleSpeedInit(
                p_inner_instance_ctrl->p_sensor_instance->p_ctrl,
                &p_inner_instance_ctrl->p_speed_el,
                &p_inner_instance_ctrl->p_angle_el,
                (motor_sensor_data_exchange_t *) &p_sensor_data_exchange);
        }
    }

    if (p_inner_instance_ctrl->p_sensor_instance2 != NULL)
    {
        // Slot 2
        p_inner_instance_ctrl->p_sensor_instance2->p_api->open(p_inner_instance_ctrl->p_sensor_instance2->p_ctrl,
                                                               p_inner_instance_ctrl->p_sensor_instance2->p_cfg);

        if (p_extended_cfg->data.sensor_slot == MOTOR_INNER_SLOT_2_ENABLE)
        {
            p_sensor_data_exchange.p_in_extra_data   = &p_inner_instance_ctrl->signals.to_pos_speed;
            p_sensor_data_exchange.pp_out_extra_data =
                (motor_shared_pos_speed_to_inner_t **) &p_inner_instance_ctrl->p_from_pos_speed;

            /* Set the pointers of the output buffer to the angle instance */
            p_inner_instance_ctrl->p_sensor_instance2->p_api->angleSpeedInit(
                p_inner_instance_ctrl->p_sensor_instance2->p_ctrl,
                &p_inner_instance_ctrl->p_speed_el,
                &p_inner_instance_ctrl->p_angle_el,
                (motor_sensor_data_exchange_t *) &p_sensor_data_exchange);
        }
    }

    /* The other sensor becomes monitor mode*/
    switch (p_extended_cfg->data.sensor_slot)
    {
        case MOTOR_INNER_SLOT_1_ENABLE:
        {
            /* Sensor slot 2 is monitor mode.*/
            if (p_inner_instance_ctrl->p_sensor_instance2 != NULL)
            {
                p_sensor_data_exchange.p_in_extra_data   = &p_inner_instance_ctrl->signals.to_pos_speed;
                p_sensor_data_exchange.pp_out_extra_data = &p_dummy_monitor_out;

                p_inner_instance_ctrl->p_sensor_instance2->p_api->angleSpeedInit(
                    p_inner_instance_ctrl->p_sensor_instance2->p_ctrl,
                    &p_inner_instance_ctrl->p_speed_el_monitor,
                    &p_inner_instance_ctrl->p_angle_el_monitor,
                    (motor_sensor_data_exchange_t *) &p_sensor_data_exchange);
            }
            else
            {
                /* Nothing to do */
            }

            break;
        }

        case MOTOR_INNER_SLOT_2_ENABLE:
        {
            if (p_inner_instance_ctrl->p_sensor_instance != NULL)
            {
                p_sensor_data_exchange.p_in_extra_data   = &p_inner_instance_ctrl->signals.to_pos_speed;
                p_sensor_data_exchange.pp_out_extra_data = &p_dummy_monitor_out;

                /* Sensor slot 1 is monitor mode.*/
                p_inner_instance_ctrl->p_sensor_instance->p_api->angleSpeedInit(
                    p_inner_instance_ctrl->p_sensor_instance->p_ctrl,
                    &p_inner_instance_ctrl->p_speed_el_monitor,
                    &p_inner_instance_ctrl->p_angle_el_monitor,
                    (motor_sensor_data_exchange_t *) &p_sensor_data_exchange);
            }
            else
            {
                /* Nothing to do */
            }

            break;
        }
    }

    /* shared buffer clear */
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[0]);
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[1]);

    p_inner_instance_ctrl->p_to_outer_copy = p_inner_instance_ctrl->p_to_outer[1];

    /* clear inner error flag */
    p_inner_instance_ctrl->signals.error_info = MOTOR_PM_FOC_ERROR_NONE;
}                                      /* End of function rm_motor_inner_init */

/*******************************************************************************************************************//**
 * @brief Closes the Motor Inner Control Module.
 *
 * This function disables the inner control, resets voltage error compensation, closes the HAL driver instance
 * and sensor instances, and clears shared data structures.
 *
 * @param[in,out] p_ctrl  Pointer to motor inner instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_close (motor_pm_foc_inner_instance_ctrl_t * const p_ctrl)
{
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl;

    motor_inner_reset(p_inner_instance_ctrl);

    rm_motor_volt_error_comp_init(&(p_inner_instance_ctrl->signals.vcomp_ctrl));

    /* Close motor driver access */
    if (p_inner_instance_ctrl->p_hal_driver_instance != NULL)
    {
        p_inner_instance_ctrl->p_hal_driver_instance->p_api->close(p_inner_instance_ctrl->p_hal_driver_instance->p_ctrl);
    }

    /* Close motor angle */
    if (p_inner_instance_ctrl->p_sensor_instance != NULL)
    {
        p_inner_instance_ctrl->p_sensor_instance->p_api->close(p_inner_instance_ctrl->p_sensor_instance->p_ctrl);
    }

    /* shared buffer clear */
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[0]);
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[1]);
}                                      /* End of function rm_pm_foc_inner_close */

/*******************************************************************************************************************//**
 * @brief Resets the variables of Motor Inner Control Module.
 *
 * This function resets the inner control instance variables, including active flag, voltage references,
 * PI controller states, voltage error compensation, sensor instances, and shared data structures.
 * It also clears error flags.
 *
 * @param[in,out] p_ctrl  Pointer to motor inner instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_reset (motor_pm_foc_inner_instance_ctrl_t * const p_ctrl)
{
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl;

    p_inner_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;

    /* Reset motor driver access */
    if (p_inner_instance_ctrl->p_hal_driver_instance != NULL)
    {
        p_inner_instance_ctrl->p_hal_driver_instance->p_api->reset(p_inner_instance_ctrl->p_hal_driver_instance->p_ctrl);
    }

    motor_inner_reset(p_inner_instance_ctrl);

    rm_motor_volt_error_comp_reset(&(p_inner_instance_ctrl->signals.vcomp_ctrl));

    /* Reset motor angle */
    if (p_inner_instance_ctrl->p_sensor_instance != NULL)
    {
        p_inner_instance_ctrl->p_sensor_instance->p_api->reset(p_inner_instance_ctrl->p_sensor_instance->p_ctrl);
    }

    /* shared buffer clear */
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[0]);
    motor_inner_reset_shared_data(p_inner_instance_ctrl->p_to_outer[1]);

    /* clear inner error flag */
    p_inner_instance_ctrl->signals.error_info = MOTOR_PM_FOC_ERROR_NONE;
}                                      /* End of function rm_pm_foc_inner_reset */

/*******************************************************************************************************************//**
 * @brief Updates the parameters of Inner Control.
 *
 * This function recalculates PI controller gains and voltage error compensation parameters
 * based on the updated configuration.
 *
 * @param[in,out] p_ctrl  Pointer to motor PM FOC instance control structure
 * @param[in]     p_cfg   Pointer to motor algorithm configuration structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_parameter_update (motor_pm_foc_instance_ctrl_t * const p_ctrl,
                                      motor_algorithm_cfg_t const * const  p_cfg)
{
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl->p_inner_instance_ctrl;

    FSP_PARAMETER_NOT_USED(p_cfg);

    p_inner_instance_ctrl->p_cfg = p_extended_cfg->p_motor_inner_cfg;

    p_inner_instance_ctrl->signals.id_pi_ctrl.f4_ilimit = p_extended_cfg->data.current_limit;
    p_inner_instance_ctrl->signals.iq_pi_ctrl.f4_ilimit = p_extended_cfg->data.current_limit;

    rm_motor_pi_gain_calc(p_extended_cfg->pm_motor_parameter,
                          p_extended_cfg->pm_inner_design_parameter,
                          &(p_inner_instance_ctrl->signals.id_pi_ctrl),
                          &(p_inner_instance_ctrl->signals.iq_pi_ctrl),
                          p_extended_cfg->data.inner_control_period);

    rm_motor_volt_error_comp_init(&(p_inner_instance_ctrl->signals.vcomp_ctrl));
}                                      /* End of function rm_motor_inner_parameter_update */

/*******************************************************************************************************************//**
 * @brief Gets the rotational speed from inner control.
 *
 * This function retrieves the electrical speed from the inner control instance and converts it
 * to mechanical speed in rpm.
 *
 * @param[in]  p_ctrl       Pointer to motor PM FOC instance control structure
 * @param[in]  p_cfg        Pointer to motor algorithm configuration structure
 * @param[out] p_speed_rpm  Pointer to output speed value [rpm]
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_speed_get (motor_pm_foc_instance_ctrl_t * const p_ctrl,
                               motor_algorithm_cfg_t const * const  p_cfg,
                               float * const                        p_speed_rpm)
{
    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl->p_inner_instance_ctrl;
    FSP_PARAMETER_NOT_USED(p_cfg);

    *p_speed_rpm = *(p_inner_instance_ctrl->p_speed_el) /
                   p_extended_cfg->pm_motor_parameter->u2_mtr_pp * MOTOR_FUNDLIB_TWOPI_60;
}                                      /* End of function rm_motor_inner_speed_get */

/*******************************************************************************************************************//**
 * @brief Cyclic process of inner control (called at ADC finish interrupt).
 *
 * This function performs the main process of inner control, including current measurement,
 * coordinate transformation, angle/speed calculation, current control, and voltage output.
 * It also handles tuner processing if active and performs error checking.
 *
 * @param[in] p_args  Pointer to HAL driver callback arguments
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_cyclic (motor_hal_driver_callback_args_t * p_args)
{
    float    f_ref[3]    = {0.0F};
    uint32_t error_flags = MOTOR_PM_FOC_ERROR_NONE;

    motor_hal_driver_current_dataset_t temp_drv_crnt_get;

    motor_algorithm_instance_t   * p_instance           = (motor_algorithm_instance_t *) p_args->p_context;
    motor_pm_foc_instance_ctrl_t * p_instance_user_ctrl = (motor_pm_foc_instance_ctrl_t *) p_instance->p_ctrl;
    motor_algorithm_cfg_t        * p_instance_user_cfg  = (motor_algorithm_cfg_t *) p_instance_user_ctrl->p_cfg;
    motor_pm_foc_extended_cfg_t  * p_extended_cfg       =
        (motor_pm_foc_extended_cfg_t *) p_instance->p_cfg->p_extend;
    motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl = p_extended_cfg->pm_foc_inner_instance_ctrl;

    motor_pm_foc_inner_cfg_t * p_inner_cfg = (motor_pm_foc_inner_cfg_t *) p_extended_cfg->p_motor_inner_cfg;

    motor_hal_driver_instance_t const * p_driver_instance = p_instance_ctrl->p_hal_driver_instance;

    motor_algorithm_callback_args_t temp_args_t;

    motor_inner_get_voltage_t temp_get_voltage;

    // assign the input buffer (double buffer to ensure complete data update from outer to inner even in case of inner interrupting outer loop execution)
    p_instance_ctrl->p_from_outer = *(p_extended_cfg->p_shared_data->pp_shared_outer_to_inner_active);

    switch (p_args->event)
    {
        /* Current Control Timing */
        case MOTOR_HAL_DRIVER_EVENT_CURRENT:
        {
            /* Get A/D converted data */
            p_driver_instance->p_api->currentGet(p_driver_instance->p_ctrl, &temp_drv_crnt_get);
            p_instance_ctrl->signals.to_pos_speed.i_u    = temp_drv_crnt_get.i_u;
            p_instance_ctrl->signals.to_pos_speed.i_v    = temp_drv_crnt_get.i_v;
            p_instance_ctrl->signals.to_pos_speed.i_w    = temp_drv_crnt_get.i_w;
            p_instance_ctrl->signals.v_dc                = temp_drv_crnt_get.v_dc;
            p_instance_ctrl->p_to_outer_copy->v_ampl_max = temp_drv_crnt_get.va_max;

            if (p_extended_cfg->p_tuner_instance)
            {
                motor_inner_handle_tuner(p_instance_ctrl, p_extended_cfg->p_tuner_instance, p_driver_instance);
            }

            /* Active Current Control */
            if (MOTOR_FUNDLIB_FLAG_SET == p_instance_ctrl->signals.active)
            {
                /* Measure current offset values */
                uint8_t tmp_flag_boot;
                p_driver_instance->p_api->flagCurrentOffsetGet(p_driver_instance->p_ctrl, &tmp_flag_boot);

                /* After current offset was measured */
                if (MOTOR_FUNDLIB_FLAG_SET == tmp_flag_boot)
                {
                    p_driver_instance->p_api->flagBootstrapGet(p_driver_instance->p_ctrl,
                                                               &(p_instance_ctrl->p_to_outer_copy->
                                                                 current_ref_update_flag));
                    if (MOTOR_FUNDLIB_FLAG_CLEAR == p_instance_ctrl->p_to_outer_copy->current_ref_update_flag)
                    {
                        p_driver_instance->p_api->bootstrapCharge(p_driver_instance->p_ctrl);
                    }
                    else
                    {
                        /* Check for errors after AD conversion */
                        rm_motor_inner_error_check(p_instance_ctrl, p_instance_user_cfg, &error_flags);
                        if (error_flags != MOTOR_PM_FOC_ERROR_NONE)
                        {
                            /* Notify error to upper layer */
                            if (NULL != p_extended_cfg->p_callback)
                            {
                                temp_args_t.event      = MOTOR_ALGORITHM_EVENT_ERROR;
                                temp_args_t.error_info = error_flags;
                                temp_args_t.p_context  = p_inner_cfg->p_context;
                                (p_extended_cfg->p_callback)(&temp_args_t);
                            }

                            break;     // Exit early on error
                        }

                        /* Coordinate transformation (UVW->dq) */
                        rm_motor_transform_uvw_dq_abs((p_instance_ctrl->signals.angle_el),
                                                      &(p_instance_ctrl->signals.to_pos_speed.i_u),
                                                      &(p_instance_ctrl->p_to_outer_copy->i_d));

                        /* Angle & speed process */
                        motor_inner_angle_cyclic(p_instance_ctrl);

                        /* Current control */
                        motor_inner_phase_voltage_get(p_instance->p_ctrl, &temp_get_voltage);

                        f_ref[0] = temp_get_voltage.v_u_ref;
                        f_ref[1] = temp_get_voltage.v_v_ref;
                        f_ref[2] = temp_get_voltage.v_w_ref;

                        /* Space vector modulation */
                        p_driver_instance->p_api->phaseVoltageSet(p_driver_instance->p_ctrl,
                                                                  f_ref[0],
                                                                  f_ref[1],
                                                                  f_ref[2]);
                    }
                }

                // swap the outer to inner shared data pointer
                if (p_instance_ctrl->p_to_outer_active == p_instance_ctrl->p_to_outer[0])
                {
                    p_instance_ctrl->p_to_outer_active = p_instance_ctrl->p_to_outer[1]; // need to be atomic instruction
                    p_instance_ctrl->p_to_outer_copy   = p_instance_ctrl->p_to_outer[0];
                }
                else
                {
                    p_instance_ctrl->p_to_outer_active = p_instance_ctrl->p_to_outer[0]; // need to be atomic instruction
                    p_instance_ctrl->p_to_outer_copy   = p_instance_ctrl->p_to_outer[1];
                }
            }

            /* Invoke the callback function if it is set. */
            if (NULL != p_extended_cfg->p_callback)
            {
                temp_args_t.event     = MOTOR_ALGORITHM_EVENT_INNER;
                temp_args_t.p_context = p_inner_cfg->p_context;
                (p_extended_cfg->p_callback)(&temp_args_t);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}                                      /* End of function rm_motor_inner_cyclic */

/*******************************************************************************************************************//**
 * @brief Gets the phase voltage references for output.
 *
 * This function calculates d/q-axis voltage references through PI control, applies decoupling control,
 * limits voltage vector, compensates for sample delay if enabled, transforms to uvw frame,
 * and applies voltage error compensation.
 *
 * @param[in]  p_ctrl     Pointer to motor PM FOC instance control structure
 * @param[out] p_voltage  Pointer to voltage output structure
 *
 * @retval None
 **********************************************************************************************************************/
static void motor_inner_phase_voltage_get (motor_pm_foc_instance_ctrl_t    * p_ctrl,
                                           motor_inner_get_voltage_t * const p_voltage)
{
    float f4_iuvw_ref[3]   = {0.0F};
    float f4_angle_comp_el = 0.0F;

    motor_pm_foc_extended_cfg_t * p_extended_cfg =
        (motor_pm_foc_extended_cfg_t *) p_ctrl->p_cfg->p_extend;
    motor_pm_foc_inner_instance_ctrl_t * p_inner_instance_ctrl = p_ctrl->p_inner_instance_ctrl;

    /* Feedback control (Current) */
    motor_inner_pi_calculation(p_inner_instance_ctrl);

    /* Decoupling control */
    rm_motor_decoupling(p_inner_instance_ctrl->p_from_outer->i_d_ref, p_inner_instance_ctrl->p_from_outer->i_q_ref,
                        *(p_inner_instance_ctrl->p_speed_el), &(p_inner_instance_ctrl->p_to_outer_copy->v_d_ref),
                        &(p_inner_instance_ctrl->p_to_outer_copy->v_q_ref), p_extended_cfg->pm_motor_parameter);

    /* Limit voltage vector */
    rm_motor_voltage_limit(&p_inner_instance_ctrl->p_to_outer_copy->v_d_ref,
                           &p_inner_instance_ctrl->p_to_outer_copy->v_q_ref,
                           p_inner_instance_ctrl->p_to_outer_copy->v_ampl_max);
    p_voltage->v_d_ref = p_inner_instance_ctrl->p_to_outer_copy->v_d_ref;
    p_voltage->v_q_ref = p_inner_instance_ctrl->p_to_outer_copy->v_q_ref;

    // Encoder New Process (Angle Update by sample delay compensation)
    if (MOTOR_INNER_SAMPLE_DELAY_COMPENSATION_ENABLE == p_extended_cfg->data.sample_delay_comp_enable)
    {
        f4_angle_comp_el =
            rm_motor_sample_delay_comp((p_inner_instance_ctrl->signals.angle_el),
                                       *(p_inner_instance_ctrl->p_speed_el),
                                       p_extended_cfg->data.inner_control_period,
                                       p_extended_cfg->data.period_magnitude_value);
    }
    else
    {
        f4_angle_comp_el = p_inner_instance_ctrl->signals.angle_el;
    }

    /* Coordinate transformation (dq->uvw) */
    rm_motor_transform_dq_uvw_abs(f4_angle_comp_el, &(p_inner_instance_ctrl->p_to_outer_copy->v_d_ref),
                                  &(p_inner_instance_ctrl->signals.v_u_ref));

    /* Voltage error compensation */
    rm_motor_transform_dq_uvw_abs(f4_angle_comp_el, &(p_inner_instance_ctrl->p_from_outer->i_d_ref), &(f4_iuvw_ref[0]));

    rm_motor_volt_error_comp_main(&(p_inner_instance_ctrl->signals.vcomp_ctrl),
                                  &(p_inner_instance_ctrl->signals.v_u_ref),
                                  &(f4_iuvw_ref[0]),
                                  p_inner_instance_ctrl->signals.v_dc);

    p_voltage->v_u_ref = p_inner_instance_ctrl->signals.v_u_ref;
    p_voltage->v_v_ref = p_inner_instance_ctrl->signals.v_v_ref;
    p_voltage->v_w_ref = p_inner_instance_ctrl->signals.v_w_ref;
}                                      /* End of function motor_inner_phase_voltage_get */

/*******************************************************************************************************************//**
 * @brief Performs angle and speed calculation in inner control cyclic process.
 *
 * This function calls the sensor instances to calculate electrical angle and speed
 * based on measured currents and outer loop inputs.
 *
 * @param[in,out] p_ctrl  Pointer to motor inner instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
static void motor_inner_angle_cyclic (motor_pm_foc_inner_instance_ctrl_t * p_ctrl)
{
    motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl = p_ctrl;

    if (p_instance_ctrl->p_sensor_instance != NULL)
    {
        p_instance_ctrl->p_sensor_instance->p_api->angleSpeedCalc(p_instance_ctrl->p_sensor_instance->p_ctrl,
                                                                  (motor_sensor_input_inner_t *) (p_instance_ctrl->
                                                                                                  p_to_outer_copy),
                                                                  (motor_sensor_input_outer_t *) &(p_instance_ctrl->
                                                                                                   p_from_outer->
                                                                                                   pos_speed_input_data));
    }

    if (p_instance_ctrl->p_sensor_instance2 != NULL)
    {
        p_instance_ctrl->p_sensor_instance2->p_api->angleSpeedCalc(p_instance_ctrl->p_sensor_instance2->p_ctrl,
                                                                   (motor_sensor_input_inner_t *) (p_instance_ctrl->
                                                                                                   p_to_outer_copy),
                                                                   (motor_sensor_input_outer_t *) &(p_instance_ctrl->
                                                                                                    p_from_outer->
                                                                                                    pos_speed_input_data));
    }

    p_instance_ctrl->signals.angle_el = *(p_instance_ctrl->p_angle_el);
}                                      /* End of function motor_inner_angle_cyclic */

/*******************************************************************************************************************//**
 * @brief Resets the inner control variables to initial state.
 *
 * This function clears active flag, voltage references, PI controller states,
 * shared data buffers, and resets sensor instances.
 *
 * @param[in,out] p_ctrl  Pointer to motor inner instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
static void motor_inner_reset (motor_pm_foc_inner_instance_ctrl_t * p_ctrl)
{
    p_ctrl->signals.active  = MOTOR_FUNDLIB_FLAG_CLEAR;
    p_ctrl->signals.v_dc    = 0.0F;
    p_ctrl->signals.v_u_ref = 0.0F;
    p_ctrl->signals.v_v_ref = 0.0F;
    p_ctrl->signals.v_w_ref = 0.0F;

    p_ctrl->signals.id_pi_ctrl.f4_err  = 0.0F;
    p_ctrl->signals.iq_pi_ctrl.f4_err  = 0.0F;
    p_ctrl->signals.id_pi_ctrl.f4_refi = 0.0F;
    p_ctrl->signals.iq_pi_ctrl.f4_refi = 0.0F;

    // clearing inner to outer buffer
    p_ctrl->p_to_outer[0]->i_d                     = 0.0F;
    p_ctrl->p_to_outer[0]->i_q                     = 0.0F;
    p_ctrl->p_to_outer[0]->v_ampl_max              = 0.0F;
    p_ctrl->p_to_outer[0]->v_d_ref                 = 0.0F;
    p_ctrl->p_to_outer[0]->v_q_ref                 = 0.0F;
    p_ctrl->p_to_outer[0]->current_ref_update_flag = MOTOR_FUNDLIB_FLAG_CLEAR;

    p_ctrl->p_to_outer[1]->i_d                     = 0.0F;
    p_ctrl->p_to_outer[1]->i_q                     = 0.0F;
    p_ctrl->p_to_outer[1]->v_ampl_max              = 0.0F;
    p_ctrl->p_to_outer[1]->v_d_ref                 = 0.0F;
    p_ctrl->p_to_outer[1]->v_q_ref                 = 0.0F;
    p_ctrl->p_to_outer[1]->current_ref_update_flag = MOTOR_FUNDLIB_FLAG_CLEAR;

    if (p_ctrl->p_sensor_instance != NULL)
    {
        p_ctrl->p_sensor_instance->p_api->reset(p_ctrl->p_sensor_instance->p_ctrl);
    }

    if (p_ctrl->p_sensor_instance2 != NULL)
    {
        p_ctrl->p_sensor_instance2->p_api->reset(p_ctrl->p_sensor_instance2->p_ctrl);
    }
}                                      /* End of function motor_inner_reset */

/*******************************************************************************************************************//**
 * @brief Performs current PI control calculation.
 *
 * This function calculates d- and q-axis voltage references from current errors
 * using PI controllers.
 *
 * @param[in,out] p_ctrl  Pointer to motor inner instance control structure
 *
 * @retval None
 **********************************************************************************************************************/
static void motor_inner_pi_calculation (motor_pm_foc_inner_instance_ctrl_t * p_ctrl)
{
    /* The d-axis */
    p_ctrl->signals.id_pi_ctrl.f4_err = p_ctrl->p_from_outer->i_d_ref - p_ctrl->p_to_outer_copy->i_d;
    p_ctrl->p_to_outer_copy->v_d_ref  = rm_motor_pi_ctrl(&(p_ctrl->signals.id_pi_ctrl));

    /* The q-axis */
    p_ctrl->signals.iq_pi_ctrl.f4_err = p_ctrl->p_from_outer->i_q_ref - p_ctrl->p_to_outer_copy->i_q;
    p_ctrl->p_to_outer_copy->v_q_ref  = rm_motor_pi_ctrl(&(p_ctrl->signals.iq_pi_ctrl));
}                                      /* End of function motor_inner_pi_calculation */

/*******************************************************************************************************************//**
 * @brief Resets shared data structure to initial values.
 *
 * This function clears all fields in the inner-to-outer shared data structure.
 *
 * @param[out] p_output  Pointer to shared data structure to be reset
 *
 * @retval None
 **********************************************************************************************************************/
static void motor_inner_reset_shared_data (motor_shared_inner_to_outer_foc_t * p_output)
{
    p_output->i_d                     = 0.0F;
    p_output->i_q                     = 0.0F;
    p_output->v_ampl_max              = 0.0F;
    p_output->v_d_ref                 = 0.0F;
    p_output->v_q_ref                 = 0.0F;
    p_output->current_ref_update_flag = MOTOR_FUNDLIB_FLAG_CLEAR;
}                                      /* End of function motor_inner_reset_shared_data */

/*******************************************************************************************************************//**
 * @brief Checks for errors after AD conversion in inner loop.
 *
 * This function detects over-current, over-voltage, and low-voltage errors.
 * If any error is detected, it immediately stops the motor and sets error flags.
 *
 * @param[in]     p_instance_ctrl  Pointer to inner control instance
 * @param[in]     p_cfg            Pointer to motor algorithm configuration
 * @param[in,out] p_error_flags    Pointer to error flags
 *
 * @retval None
 **********************************************************************************************************************/
static void rm_motor_inner_error_check (motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl,
                                        const motor_algorithm_cfg_t        * p_cfg,
                                        uint32_t                           * p_error_flags)
{
    uint32_t error_flags = MOTOR_PM_FOC_ERROR_NONE;

    motor_pm_foc_extended_cfg_t const * p_extended_cfg    = p_cfg->p_extend;
    motor_hal_driver_instance_t const * p_driver_instance = p_instance_ctrl->p_hal_driver_instance;

    /* Current-related error checks */
    float f_iu  = p_instance_ctrl->signals.to_pos_speed.i_u;
    float f_iv  = p_instance_ctrl->signals.to_pos_speed.i_v;
    float f_iw  = p_instance_ctrl->signals.to_pos_speed.i_w;
    float f_vdc = p_instance_ctrl->signals.v_dc;

    /* Over-current hard input check */
    if ((*p_error_flags) & MOTOR_PM_FOC_ERROR_OVER_CURRENT_HW)
    {
        const motor_tuner_instance_t * p_tuner_instance = p_extended_cfg->p_tuner_instance;
        if (p_tuner_instance != NULL)
        {
            p_tuner_instance->p_api->abort(p_extended_cfg->p_tuner_instance->p_ctrl,
                                           MOTOR_PM_FOC_ERROR_OVER_CURRENT_HW);
        }
    }

    /* Over-current error check */
    if ((fabsf(f_iu) > p_extended_cfg->data.overcurrent_limit) ||
        (fabsf(f_iv) > p_extended_cfg->data.overcurrent_limit) ||
        (fabsf(f_iw) > p_extended_cfg->data.overcurrent_limit))
    {
        error_flags |= MOTOR_PM_FOC_ERROR_OVER_CURRENT_SW;
    }

    /* Over-voltage error check */
    if (f_vdc > p_extended_cfg->data.overvoltage_limit)
    {
        error_flags |= MOTOR_PM_FOC_ERROR_OVER_VOLTAGE;
    }

    /* Low-voltage error check */
    if (f_vdc < p_extended_cfg->data.lowvoltage_limit)
    {
        error_flags |= MOTOR_PM_FOC_ERROR_LOW_VOLTAGE;
    }

    /* If any error is detected, immediately stop the motor and notify */
    if (error_flags != MOTOR_PM_FOC_ERROR_NONE)
    {
        /* Stop motor immediately */
        p_instance_ctrl->signals.active = MOTOR_FUNDLIB_FLAG_CLEAR;

        /* Reset PWM outputs to safe state */
        p_driver_instance->p_api->phaseVoltageSet(p_driver_instance->p_ctrl, 0.0F, 0.0F, 0.0F);
    }

    p_instance_ctrl->signals.error_info |= error_flags;
    *p_error_flags |= error_flags;
}                                      /* End of function rm_motor_inner_error_check */

/*******************************************************************************************************************//**
 * @brief Retrieves error flags from inner control instance.
 *
 * This function returns the accumulated error flags from the inner control instance.
 *
 * @param[in]  p_ctrl        Pointer to motor inner instance control structure
 * @param[out] p_error_flags Pointer to error flags
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_error_get (motor_pm_foc_inner_instance_ctrl_t * p_ctrl, uint32_t * p_error_flags)
{
    *p_error_flags = p_ctrl->signals.error_info;
}                                      /* End of function rm_motor_inner_error_get */

/*******************************************************************************************************************//**
 * @brief Handles tuner processing if active.
 *
 * This function checks if the tuner is active and transfers PWM control to the tuner.
 * It performs tuner inner loop processing including current measurement and voltage output.
 *
 * @param[in,out] p_instance_ctrl   Pointer to motor inner instance control structure
 * @param[in]     p_tuner_instance   Pointer to motor tuner instance
 * @param[in]     p_driver_instance  Pointer to motor HAL driver instance
 *
 * @retval FSP_SUCCESS  Successfully handled tuner processing
 **********************************************************************************************************************/
static fsp_err_t motor_inner_handle_tuner (motor_pm_foc_inner_instance_ctrl_t * p_instance_ctrl,
                                           motor_tuner_instance_t const       * p_tuner_instance,
                                           motor_hal_driver_instance_t const  * p_driver_instance)
{
    bool              tuner_active = false;
    motor_tuner_in_t  tuner_in;
    motor_tuner_out_t tuner_out =
    {
        .vu      = MOTOR_INNER_NUM_0P5,
        .vv      = MOTOR_INNER_NUM_0P5,
        .vw      = MOTOR_INNER_NUM_0P5,
        .request = MOTOR_TUNER_REQUEST_NONE,
        .status  = MOTOR_TUNER_STATUS_READY
    };
    motor_hal_driver_current_dataset_t drv_ad_result;

    /* All input instance should be checked before invoke this function */
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_tuner_instance);
    FSP_ASSERT(p_driver_instance);

    p_tuner_instance->p_api->isActive(p_tuner_instance->p_ctrl, &tuner_active);

    /* Skip if not active */
    if (!tuner_active)
    {

        /* Exist but not activated */
        return FSP_SUCCESS;
    }

    /* Check if current has been calibrated */
    uint8_t is_current_offset_calibrated;
    p_driver_instance->p_api->flagCurrentOffsetGet(p_driver_instance->p_ctrl, &is_current_offset_calibrated);
    if (MOTOR_FUNDLIB_FLAG_CLEAR == is_current_offset_calibrated)
    {
        /* Activate driver in case of cold start */
        p_driver_instance->p_api->activate(p_driver_instance->p_ctrl);

        /* Skip when current calibration is not completed */
        return FSP_SUCCESS;
    }

    /* Check bootstrap */
    p_driver_instance->p_api->flagBootstrapGet(p_driver_instance->p_ctrl,
                                               &p_instance_ctrl->p_to_outer_copy->current_ref_update_flag);
    if (MOTOR_FUNDLIB_FLAG_CLEAR == p_instance_ctrl->p_to_outer_copy->current_ref_update_flag)
    {
        p_driver_instance->p_api->bootstrapCharge(p_driver_instance->p_ctrl);

        return FSP_SUCCESS;
    }

    /*
     * Main procedure
     */

    /* Tuner inner loop processing */
    p_driver_instance->p_api->currentGet(p_driver_instance->p_ctrl, &drv_ad_result);
    tuner_in.iu     = drv_ad_result.i_u;
    tuner_in.iv     = drv_ad_result.i_v;
    tuner_in.iw     = drv_ad_result.i_w;
    tuner_in.vdc    = drv_ad_result.v_dc;
    tuner_in.va_max = p_instance_ctrl->p_to_outer_copy->v_ampl_max;

    p_tuner_instance->p_api->innerLoopProcess(p_tuner_instance->p_ctrl, &tuner_in, &tuner_out);
    switch (tuner_out.request)
    {
        case MOTOR_TUNER_REQUEST_STOP:
        {
            /* Return driver to PM FOC */
            p_driver_instance->p_api->deactivate(p_driver_instance->p_ctrl);
            break;
        }

        case MOTOR_TUNER_REQUEST_START:
        {
            /* Keep tuner control */
            p_driver_instance->p_api->activate(p_driver_instance->p_ctrl);
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }

    p_driver_instance->p_api->phaseVoltageSet(p_driver_instance->p_ctrl, tuner_out.vu, tuner_out.vv, tuner_out.vw);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Gets control mode request from inner to outer loop.
 *
 * This function retrieves the control mode and current reference overwrite values
 * from the sensor module to the outer control instance.
 *
 * @param[in]  p_ctrl           Pointer to motor inner instance control structure
 * @param[out] p_inner_mode_data Pointer to inner-to-outer mode data structure
 *
 * @retval None
 **********************************************************************************************************************/
void rm_motor_inner_outer_ref_ctrl_mode_get (motor_pm_foc_inner_instance_ctrl_t * p_ctrl,
                                             motor_mode_inner_to_outer_t        * p_inner_mode_data)
{
    /* getting the outer ctrl mode request from inner to outer loop*/
    p_inner_mode_data->mode_outer = p_ctrl->p_from_pos_speed->mode_sensor;
    p_inner_mode_data->id_ref_ow  = p_ctrl->p_from_pos_speed->i_d_ref_ow;
    p_inner_mode_data->iq_ref_ow  = p_ctrl->p_from_pos_speed->i_q_ref_ow;
}
