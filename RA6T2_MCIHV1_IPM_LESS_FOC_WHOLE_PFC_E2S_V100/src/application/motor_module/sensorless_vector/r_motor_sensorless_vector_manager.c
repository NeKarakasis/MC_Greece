/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_sensorless_vector_manager.c
* Description : The processes of motor control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <math.h>

#include <stdint.h>

/* Main associated header file */
#include "r_motor_sensorless_vector_manager.h"

/* Project headers */
#include "r_motor_common.h"
#include "r_motor_pi_control.h"
#include "r_motor_sensorless_vector_statemachine.h"
#include "r_motor_sensorless_vector_protection.h"
#include "r_motor_current_api.h"
#include "r_motor_speed_api.h"
#include "r_motor_driver.h"

#include "r_motor_driver_hal.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_default_init
* Description   : Initializes motor drive modules with default configuration
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_default_init(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    st_current_cfg_t        st_cur_cfg;
    st_bemf_observer_cfg_t  st_bemf_obs_cfg;
    st_speed_cfg_t          st_spd_cfg;
    st_motor_driver_cfg_t   st_driver_cfg;
    st_flying_start_cfg_t   st_flystart_cfg;

    static const float  volterr_array[VERR_COMP_ARY_SIZE] =
    {
        INVERTER_CFG_COMP_V0,    /* f4_comp_v[0] */
        INVERTER_CFG_COMP_V1,    /* f4_comp_v[1] */
        INVERTER_CFG_COMP_V2,    /* f4_comp_v[2] */
        INVERTER_CFG_COMP_V3,    /* f4_comp_v[3] */
        INVERTER_CFG_COMP_V4,    /* f4_comp_v[4] */
    };
    static const float  crnt_array[VERR_COMP_ARY_SIZE] =
    {
        INVERTER_CFG_COMP_I0,    /* f4_comp_i[0] */
        INVERTER_CFG_COMP_I1,    /* f4_comp_i[1] */
        INVERTER_CFG_COMP_I2,    /* f4_comp_i[2] */
        INVERTER_CFG_COMP_I3,    /* f4_comp_i[3] */
        INVERTER_CFG_COMP_I4     /* f4_comp_i[4] */
    };
    float ref_voltage;

    /* Initialize member of motor parameter structure */
    p_st_sensorless_vector->st_motor.u2_mtr_pp = MOTOR_CFG_POLE_PAIRS;
    p_st_sensorless_vector->st_motor.f4_mtr_r  = MOTOR_CFG_RESISTANCE;
    p_st_sensorless_vector->st_motor.f4_mtr_ld = MOTOR_CFG_D_INDUCTANCE;
    p_st_sensorless_vector->st_motor.f4_mtr_lq = MOTOR_CFG_Q_INDUCTANCE;
    p_st_sensorless_vector->st_motor.f4_mtr_m  = MOTOR_CFG_MAGNETIC_FLUX;
    p_st_sensorless_vector->st_motor.f4_mtr_j  = MOTOR_CFG_ROTOR_INERTIA;
    p_st_sensorless_vector->st_motor.f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS;

    if (MTR_FLG_CLR == p_st_sensorless_vector->p_st_cc->u1_flag_offset_calc)
    {
        p_st_sensorless_vector->u1_flag_flying_start_use = MTR_DISABLE;
    }
    else
    {
        p_st_sensorless_vector->u1_flag_flying_start_use = CURRENT_CFG_FLYING_START;
    }
    p_st_sensorless_vector->u1_flag_less_switch_use      = SPEED_CFG_LESS_SWITCH;
    p_st_sensorless_vector->u1_flag_openloop_damping_use = SPEED_CFG_OPENLOOP_DAMPING;
    p_st_sensorless_vector->u1_direction                 = MTR_CW;
    p_st_sensorless_vector->u1_ctrl_loop_mode            = MOTOR_COMMON_CFG_LOOP_MODE;
    p_st_sensorless_vector->f4_overcurrent_limit         = MOTOR_COMMON_OVERCURRENT_LIMIT;
    p_st_sensorless_vector->f4_overvoltage_limit         = INVERTER_CFG_OVERVOLTAGE_LIMIT;
    p_st_sensorless_vector->f4_undervoltage_limit        = INVERTER_CFG_UNDERVOLTAGE_LIMIT;
    p_st_sensorless_vector->f4_overspeed_limit_rad       = (SPEED_CFG_SPEED_LIMIT_RPM * MTR_RPM2RAD);
    p_st_sensorless_vector->f4_switch_phase_err_rad      = SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_RAD;
    p_st_sensorless_vector->f4_id_down_speed_rad         = (SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM * MTR_RPM2RAD);
    p_st_sensorless_vector->f4_id_up_speed_rad           = (SENSORLESS_VECTOR_ID_UP_SPEED_RPM * MTR_RPM2RAD);

    /* Initialize phase error LPF */
    motor_filter_first_order_lpff_init(&p_st_sensorless_vector->st_phase_err_lpf);
    motor_sensorless_phase_err_lpf_setup(p_st_sensorless_vector,
                                         SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_LPF_CUT_FREQ,
                                         SPEED_CFG_CTRL_PERIOD);

    /* Current configuration set */
    st_cur_cfg.u1_flag_volt_err_comp_use       = CURRENT_CFG_VOLT_ERR_COMP;
    st_cur_cfg.u2_offset_calc_time             = CURRENT_CFG_OFFSET_CALC_TIME;
    st_cur_cfg.u2_charge_bootstrap_time        = CURRENT_CFG_CHARGE_BOOTSTRAP_TIME;
    st_cur_cfg.f4_ctrl_period                  = MOTOR_COMMON_CTRL_PERIOD;
    st_cur_cfg.f4_current_omega_hz             = CURRENT_CFG_OMEGA;
    st_cur_cfg.f4_current_zeta                 = CURRENT_CFG_ZETA;
    st_cur_cfg.f4_id_up_step                   = MOTOR_COMMON_ID_UP_STEP_RATE;
    st_cur_cfg.f4_id_down_step                 = MOTOR_COMMON_ID_DOWN_STEP_RATE;
    st_cur_cfg.f4_iq_down_step_time_inv        = CURRENT_CFG_IQ_DOWN_STEP_TIME_INV;
    st_cur_cfg.f4_ol_ref_id                    = CURRENT_CFG_REF_ID_OPENLOOP;
    st_cur_cfg.st_motor                        = p_st_sensorless_vector->st_motor;
    st_cur_cfg.u1_flag_stall_detection_use     = CURRENT_CFG_STALL_DETECTION;
    st_cur_cfg.u1_flag_trq_vibration_comp_use  = MTR_FLG_CLR;
    st_cur_cfg.u1_flag_trq_vibration_comp_mode = CURRENT_CFG_TRQVIB_COMP_MODE;
    st_cur_cfg.f4_id_hpf_time                  = CURRENT_CFG_STALL_D_HPF_GAIN;
    st_cur_cfg.f4_iq_hpf_time                  = CURRENT_CFG_STALL_Q_HPF_GAIN;
    st_cur_cfg.f4_threshold_level              = CURRENT_CFG_STALL_THRESHOLD_LEVEL;
    st_cur_cfg.f4_threshold_time               = CURRENT_CFG_STALL_THRESHOLD_TIME;
    st_cur_cfg.f4_tf_lpf_omega                 = CURRENT_CFG_TRQVIB_TF_LPF_OMEGA;
    st_cur_cfg.u1_target_2f                    = CURRENT_CFG_TRQVIB_TARGET_2F;
    st_cur_cfg.f4_output_gain_1f               = CURRENT_CFG_TRQVIB_OUTPUT_GAIN_1F;
    st_cur_cfg.f4_output_gain_2f               = CURRENT_CFG_TRQVIB_OUTPUT_GAIN_2F;
    st_cur_cfg.f4_timelead_1f                  = CURRENT_CFG_TRQVIB_TIMELEAP_1F;
    st_cur_cfg.f4_timelead_2f                  = CURRENT_CFG_TRQVIB_TIMELEAP_2F;
    st_cur_cfg.f4_input_weight2                = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_2;
    st_cur_cfg.f4_input_weight1                = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_1;
    st_cur_cfg.f4_input_weight0                = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_0;
    st_cur_cfg.f4_suppression_th_1f            = CURRENT_CFG_TRQVIB_SUPP_TH_1F;
    st_cur_cfg.f4_suppression_th_2f            = CURRENT_CFG_TRQVIB_SUPP_TH_2F;
    st_cur_cfg.f4_abnormal_output_th_1f        = CURRENT_CFG_TRQVIB_ABNORMAL_TH_1F;
    st_cur_cfg.f4_abnormal_output_th_2f        = CURRENT_CFG_TRQVIB_ABNORMAL_TH_2F;
    R_MOTOR_CURRENT_ParameterUpdate(p_st_sensorless_vector->p_st_cc, &st_cur_cfg);

    /* BEMF observer */
    st_bemf_obs_cfg.f4_e_obs_omega_hz   = CURRENT_CFG_E_OBS_OMEGA;
    st_bemf_obs_cfg.f4_e_obs_zeta       = CURRENT_CFG_E_OBS_ZETA;
    st_bemf_obs_cfg.f4_pll_est_omega_hz = CURRENT_CFG_PLL_EST_OMEGA;
    st_bemf_obs_cfg.f4_pll_est_zeta     = CURRENT_CFG_PLL_EST_ZETA;
    R_MOTOR_CURRENT_BEMFObserverParameterUpdate(p_st_sensorless_vector->p_st_cc, &st_bemf_obs_cfg);

    /* Voltage error compensation*/
    ref_voltage = INVERTER_CFG_INPUT_V;
    R_MOTOR_CURRENT_VoltErrCompParamSet(p_st_sensorless_vector->p_st_cc,
                                        crnt_array,
                                        volterr_array,
                                        ref_voltage);

    /* Speed configuration set */
    st_spd_cfg.u1_flag_fluxwkn_use         = SPEED_CFG_FLUX_WEAKENING;
    st_spd_cfg.u1_flag_mtpa_use            = SPEED_CFG_MTPA;
    st_spd_cfg.f4_speed_ctrl_period        = SPEED_CFG_CTRL_PERIOD;
    st_spd_cfg.f4_max_speed_rpm            = MOTOR_CFG_MAX_SPEED_RPM;
    st_spd_cfg.f4_speed_rate_limit_rpm     = SPEED_CFG_RATE_LIMIT_RPM;
    st_spd_cfg.f4_speed_omega_hz           = SPEED_CFG_OMEGA;
    st_spd_cfg.f4_speed_zeta               = SPEED_CFG_ZETA;
    st_spd_cfg.f4_speed_lpf_hz             = SPEED_CFG_LPF_OMEGA;
    st_spd_cfg.f4_opl2less_sw_time         = SPEED_OPL2LESS_SWITCH_TIME;
    st_spd_cfg.f4_ed_hpf_omega             = SPEED_OPL_DAMP_ED_HPF_OMEGA;
    st_spd_cfg.f4_ol_damping_zeta          = SPEED_OPL_DAMP_ZETA;
    st_spd_cfg.f4_ol_damping_fb_limit_rate = SPEED_OPL_DAMP_FB_SPEED_LIMIT_RATE;
    st_spd_cfg.f4_ol_ref_id                = CURRENT_CFG_REF_ID_OPENLOOP;
    st_spd_cfg.f4_id_down_speed_rpm        = SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM;
    st_spd_cfg.st_motor                    = p_st_sensorless_vector->st_motor;
    R_MOTOR_SPEED_ParameterUpdate(p_st_sensorless_vector->p_st_sc, &st_spd_cfg);

    /* Setup current limits by nominal current of motor */
    motor_sensorless_vector_nominal_current_set(p_st_sensorless_vector, MOTOR_CFG_NOMINAL_CURRENT_RMS);

    /* driver */
    st_driver_cfg.ADCDataGet                = DRIVER_CFG_FUNC_ADC_DATA_GET;
    st_driver_cfg.BLDCDutySet               = DRIVER_CFG_FUNC_DUTY_SET;
    st_driver_cfg.BLDCZeroDutySet           = DRIVER_CFG_FUNC_ZERO_DUTY_SET;
    st_driver_cfg.BLDCCompareDutySet        = DRIVER_CFG_FUNC_COMPARE_DUTY_SET;
    st_driver_cfg.PWMOutputStart            = DRIVER_CFG_FUNC_PWM_OUTPUT_START;
    st_driver_cfg.PWMOutputStop             = DRIVER_CFG_FUNC_PWM_OUTPUT_STOP;
    st_driver_cfg.f4_shunt_ohm              = INVERTER_CFG_SHUNT_RESIST;
    st_driver_cfg.f4_volt_gain              = INVERTER_CFG_VOLTAGE_GAIN;
    st_driver_cfg.f4_crnt_amp_gain          = INVERTER_CFG_CURRENT_AMP_GAIN;
    st_driver_cfg.f4_pwm_period_cnt         = MOTOR_COMMON_CARRIER_SET_BASE;
    st_driver_cfg.f4_pwm_dead_time_cnt      = MOTOR_COMMON_DEADTIME_SET;
    R_MOTOR_DRIVER_ParameterUpdate(p_st_sensorless_vector->p_st_driver, &st_driver_cfg);

    /* flying start */
    st_flystart_cfg.f4_ctrl_period           = MOTOR_COMMON_CTRL_PERIOD;
    st_flystart_cfg.f4_restart_speed         = (SENSORLESS_VECTOR_FLY_START_RESTART_SPEED_LIMIT * MTR_RPM2RAD);
    st_flystart_cfg.f4_off_time              = SENSORLESS_VECTOR_FLY_START_OFF_TIME_SEC;
    st_flystart_cfg.u2_off_time_cnt          = (uint16_t)(SENSORLESS_VECTOR_FLY_START_OFF_TIME_SEC
                                                                / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.u2_over_time_cnt         = (uint16_t)(SENSORLESS_VECTOR_FLY_START_OVER_TIME_SEC
                                                                / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.u2_active_brake_time_cnt = (uint16_t)(SENSORLESS_VECTOR_FLY_START_ACTIVE_BRAKE_TIME_SEC
                                                                / MOTOR_COMMON_CTRL_PERIOD);
    st_flystart_cfg.p_st_motor               = &p_st_sensorless_vector->st_motor;
    motor_flying_start_parameter_set(&p_st_sensorless_vector->st_flystart, &st_flystart_cfg);

    /* Reset motor */
    motor_sensorless_vector_reset(p_st_sensorless_vector);
} /* End of function motor_sensorless_vector_default_init */

/**********************************************************************************************************************
* Function Name : motor_sensorless_vector_reset
* Description   : Resets motor drive modules, configurations will not be reset
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_reset(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    /* Reset status structure members */
    p_st_sensorless_vector->u1_flag_down_to_ol   = MTR_FLG_CLR;
    p_st_sensorless_vector->u1_state_id_ref      = CURRENT_STATE_ID_ZERO_CONST;
    p_st_sensorless_vector->u1_state_iq_ref      = CURRENT_STATE_IQ_ZERO_CONST;
    p_st_sensorless_vector->u1_state_speed_ref   = SPEED_STATE_ZERO_CONST;
    p_st_sensorless_vector->u2_error_status      = MOTOR_SENSORLESS_VECTOR_ERROR_NONE;
    p_st_sensorless_vector->u2_run_mode          = MOTOR_MODE_INIT;
    p_st_sensorless_vector->f4_vdc_ad            = 0.0f;
    p_st_sensorless_vector->f4_iu_ad             = 0.0f;
    p_st_sensorless_vector->f4_iv_ad             = 0.0f;
    p_st_sensorless_vector->f4_iw_ad             = 0.0f;
    p_st_sensorless_vector->f4_phase_err_rad_lpf = 0.0f;
    p_st_sensorless_vector->f4_damp_comp_speed   = 0.0f;
    p_st_sensorless_vector->f4_ol_speed_rad      = 0.0f;

    /* Reset phase error LPF */
    motor_filter_first_order_lpff_reset(&p_st_sensorless_vector->st_phase_err_lpf);

    /* Current output reset */
    if (MTR_DISABLE == p_st_sensorless_vector->u1_flag_flying_start_use)
    {
        p_st_sensorless_vector->st_current_output.u1_flag_offset_calc       = MTR_FLG_CLR;
        p_st_sensorless_vector->st_current_output.u1_flag_charge_bootstrap  = MTR_FLG_CLR;
        p_st_sensorless_vector->p_st_cc->u1_flag_charge_bootstrap           = MTR_FLG_CLR;
        p_st_sensorless_vector->p_st_cc->u2_charge_bootstrap_cnt            = 0;
    }
    else if (MTR_ENABLE == p_st_sensorless_vector->u1_flag_flying_start_use)
    {
        p_st_sensorless_vector->st_current_output.u1_flag_charge_bootstrap  = MTR_FLG_SET;
        p_st_sensorless_vector->p_st_cc->u1_flag_charge_bootstrap           = MTR_FLG_SET;
    }
    else
    {
        /* Do nothing */
    }

    p_st_sensorless_vector->st_current_output.f4_modu             = 0.5f;
    p_st_sensorless_vector->st_current_output.f4_modv             = 0.5f;
    p_st_sensorless_vector->st_current_output.f4_modw             = 0.5f;
    p_st_sensorless_vector->st_current_output.f4_neutral_duty     = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_va_max           = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_ref_id_ctrl      = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_speed_rad        = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_ed               = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_eq               = 0.0f;
    p_st_sensorless_vector->st_current_output.f4_phase_err_rad    = 0.0f;
    /* Speed output reset */
    p_st_sensorless_vector->st_speed_output.f4_id_ref             = 0.0f;
    p_st_sensorless_vector->st_speed_output.f4_iq_ref             = 0.0f;
    p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl = 0.0f;
    p_st_sensorless_vector->st_speed_output.f4_speed_rad_lpf      = 0.0f;

    /* Reset Flying-satrt module */
    motor_flying_start_reset(&p_st_sensorless_vector->st_flystart);
    /* Reset current control module */
    R_MOTOR_CURRENT_Reset(p_st_sensorless_vector->p_st_cc);
    /* Reset speed control module */
    R_MOTOR_SPEED_Reset(p_st_sensorless_vector->p_st_sc);
} /* End of function motor_sensorless_vector_reset */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_decide_direction
* Description   : Decide direction
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_decide_direction(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    if (0.0f <= p_st_sensorless_vector->st_speed_output.f4_speed_rad_lpf)
    {
        p_st_sensorless_vector->u1_direction = MTR_CW;
    }
    else if (0.0f > p_st_sensorless_vector->st_speed_output.f4_speed_rad_lpf)
    {
        p_st_sensorless_vector->u1_direction = MTR_CCW;
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function motor_sensorless_vector_decide_direction */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_error_process
* Description   : Sets error flags and trigger error event when u2_error_flag is not zero
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 u2_error_flag - The error flags of the errors to report
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_error_process(st_sensorless_vector_control_t *p_st_sensorless_vector,
                                           uint16_t u2_error_flag)
{
    if (MOTOR_SENSORLESS_VECTOR_ERROR_NONE != u2_error_flag)
    {
        p_st_sensorless_vector->u2_error_status |= u2_error_flag;
        motor_sensorless_vector_statemachine_event(&p_st_sensorless_vector->st_stm,
                                                   p_st_sensorless_vector,
                                                   STATEMACHINE_EVENT_ERROR);
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function motor_sensorless_vector_error_process */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_speed_status_select
* Description   : Performs state transition related to speed reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_speed_status_select(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    if ((SPEED_STATE_ZERO_CONST == p_st_sensorless_vector->u1_state_speed_ref) &&
        (MOTOR_MODE_DRIVE == p_st_sensorless_vector->u2_run_mode))
    {
        p_st_sensorless_vector->u1_state_speed_ref = SPEED_STATE_MANUAL;
    }
} /* End of function motor_sensorless_vector_speed_status_select */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_iq_status_select
* Description   : Performs state transition related to q-axis current reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_iq_status_select(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    uint8_t u1_temp0;
    float   f4_temp0;

    if (MTR_FLG_SET == p_st_sensorless_vector->u1_flag_down_to_ol)
    {
        p_st_sensorless_vector->u1_state_iq_ref = CURRENT_STATE_IQ_DOWN;
    }

    switch (p_st_sensorless_vector->u1_state_iq_ref)
    {
        case CURRENT_STATE_IQ_ZERO_CONST:
            f4_temp0 = fabsf(p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl);

            if (MTR_FLG_SET == p_st_sensorless_vector->u1_flag_openloop_damping_use)
            {
                /* Open loop damping control*/
                p_st_sensorless_vector->f4_damp_comp_speed =
                    R_MOTOR_SPEED_OplDampCtrl(p_st_sensorless_vector->p_st_sc,
                                              p_st_sensorless_vector->st_current_output.f4_ed);
            }

            if (f4_temp0 >= p_st_sensorless_vector->f4_id_down_speed_rad)
            {
                if (MTR_FLG_SET == p_st_sensorless_vector->u1_flag_less_switch_use)
                {
                    p_st_sensorless_vector->u1_state_iq_ref = CURRENT_STATE_IQ_AUTO_ADJ;
                    R_MOTOR_SPEED_Opl2lessPreprocess(p_st_sensorless_vector->p_st_sc,
                                                     p_st_sensorless_vector->st_current_output.f4_ref_id_ctrl,
                                                     p_st_sensorless_vector->f4_phase_err_rad_lpf);
                }
                else
                {
                    p_st_sensorless_vector->u1_state_iq_ref = CURRENT_STATE_IQ_SPEED_PI_OUTPUT;
                    R_MOTOR_CURRENT_HuntingSuppress(p_st_sensorless_vector->p_st_cc,
                                                    p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl);
                }
                if (MTR_FLG_SET == p_st_sensorless_vector->u1_flag_openloop_damping_use)
                {
                    R_MOTOR_SPEED_OplDampReset(p_st_sensorless_vector->p_st_sc);
                    p_st_sensorless_vector->f4_damp_comp_speed = 0.0f;
                }
            }
        break;

        case CURRENT_STATE_IQ_AUTO_ADJ:
            p_st_sensorless_vector->st_speed_output.f4_iq_ref =
                    R_MOTOR_SPEED_Opl2lessReferenceIqCalc(p_st_sensorless_vector->p_st_sc,
                                                          p_st_sensorless_vector->st_current_output.f4_ed,
                                                          p_st_sensorless_vector->st_current_output.f4_eq,
                                                          p_st_sensorless_vector->st_current_output.f4_ref_id_ctrl,
                                                          p_st_sensorless_vector->st_current_output.f4_phase_err_rad);

            if (MTR_CW == p_st_sensorless_vector->u1_direction)
            {
                u1_temp0 = (p_st_sensorless_vector->st_current_output.f4_phase_err_rad <=
                                p_st_sensorless_vector->f4_switch_phase_err_rad);
            }
            else if (MTR_CCW == p_st_sensorless_vector->u1_direction)
            {
                u1_temp0 = (p_st_sensorless_vector->st_current_output.f4_phase_err_rad >=
                                (-p_st_sensorless_vector->f4_switch_phase_err_rad));
            }
            else
            {
                u1_temp0 = MTR_FLG_CLR;
            }

            if (MTR_FLG_SET == u1_temp0)
            {
                p_st_sensorless_vector->u1_state_iq_ref = CURRENT_STATE_IQ_SPEED_PI_OUTPUT;

                /* Preset for suppress speed hunting */
                R_MOTOR_SPEED_SwitchingFlagSet(p_st_sensorless_vector->p_st_sc);
                R_MOTOR_CURRENT_HuntingSuppress(p_st_sensorless_vector->p_st_cc,
                                                p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl);
                R_MOTOR_SPEED_HuntingSuppress(p_st_sensorless_vector->p_st_sc,
                                              p_st_sensorless_vector->st_speed_output.f4_iq_ref);
            }
        break;

        case CURRENT_STATE_IQ_DOWN:
            if (p_st_sensorless_vector->p_st_cc->f4_ref_iq_ctrl <= 0.0f)
            {
                p_st_sensorless_vector->u1_state_iq_ref = CURRENT_STATE_IQ_ZERO_CONST;
            }
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function motor_sensorless_vector_iq_status_select */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_id_status_select
* Description   : Performs state transition related to d-axis current reference
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_id_status_select(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    float f4_temp0;

    if (MTR_FLG_SET == p_st_sensorless_vector->u1_flag_down_to_ol)
    {
        p_st_sensorless_vector->u1_state_id_ref = CURRENT_STATE_ID_UP;
    }

    switch (p_st_sensorless_vector->u1_state_id_ref)
    {
        case CURRENT_STATE_ID_ZERO_CONST:
            if (p_st_sensorless_vector->u2_run_mode == MOTOR_MODE_BOOT)
            {
                p_st_sensorless_vector->u1_state_id_ref = CURRENT_STATE_ID_UP;
            }
        break;

        case CURRENT_STATE_ID_UP:
            if (p_st_sensorless_vector->p_st_cc->f4_ref_id_ctrl >= p_st_sensorless_vector->p_st_cc->f4_ol_ref_id)
            {
                p_st_sensorless_vector->u1_state_id_ref = CURRENT_STATE_ID_MANUAL;
            }
        break;

        case CURRENT_STATE_ID_MANUAL:
            f4_temp0 = fabsf(p_st_sensorless_vector->st_speed_output.f4_ref_speed_rad_ctrl);
            if (f4_temp0 >= p_st_sensorless_vector->f4_id_down_speed_rad)
            {
                p_st_sensorless_vector->u1_state_id_ref = CURRENT_STATE_ID_DOWN;
            }
        break;

        case CURRENT_STATE_ID_DOWN:
            if (0.0f >= p_st_sensorless_vector->p_st_cc->f4_ref_id_ctrl)
            {
                p_st_sensorless_vector->u1_state_id_ref = CURRENT_STATE_ID_INPUT;
            }
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function motor_sensorless_vector_id_status_select */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_nominal_current_set
* Description   : Setup current related limit values by the nominal current of motor
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 f4_nominal_current_rms - The nominal current[Arms] of motor
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_nominal_current_set(st_sensorless_vector_control_t *p_st_sensorless_vector,
                                                 float f4_nominal_current_rms)
{
    p_st_sensorless_vector->f4_overcurrent_limit = (f4_nominal_current_rms * MTR_SQRT_2)
                                                   * MOTOR_COMMON_CFG_OVERCURRENT_MARGIN_MULT;

    if (p_st_sensorless_vector->f4_overcurrent_limit > INVERTER_CFG_CURRENT_LIMIT)
    {
        p_st_sensorless_vector->f4_overcurrent_limit = INVERTER_CFG_CURRENT_LIMIT;
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function motor_sensorless_vector_nominal_current_set */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_module_activate
* Description   : Activate modules
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_module_activate(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    switch (p_st_sensorless_vector->u1_ctrl_loop_mode)
    {
        case MOTOR_LOOP_SPEED:
            R_MOTOR_CURRENT_Run(p_st_sensorless_vector->p_st_cc);
            R_MOTOR_SPEED_Run(p_st_sensorless_vector->p_st_sc);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function motor_sensorless_vector_module_activate */

/***********************************************************************************************************************
* Function Name : motor_sensorless_phase_err_lpf_setup
* Description   : Setups LPF for estimated phase error by given cutoff frequency[Hz]
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
*                 f4_phase_err_lpf_cut_freq - Cut off frequency[Hz] of phase error LPF
*                 f4_ctrl_period - Control period [s]
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_phase_err_lpf_setup(st_sensorless_vector_control_t *p_st_sensorless_vector,
                                          float f4_phase_err_lpf_cut_freq,
                                          float f4_ctrl_period)
{
    motor_filter_first_order_lpff_gain_calc(&p_st_sensorless_vector->st_phase_err_lpf,
                                            f4_phase_err_lpf_cut_freq,
                                            f4_ctrl_period);
} /* End of function motor_sensorless_phase_err_lpf_setup */
