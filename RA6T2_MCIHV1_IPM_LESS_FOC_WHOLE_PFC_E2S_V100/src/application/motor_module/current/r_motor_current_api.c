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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_current_api.c
* Description : The current control module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*           30.10.2021 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_current.h"
#include "r_motor_current_api.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_current_control_t      g_st_cc;           /* Current control structure */
uint8_t g_u1_trq_vibration_comp_update_flag;

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Open
* Description   : Open current control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Open(void)
{
    float f4_pwm_period_us;
    float f4_duty_cycle_deadtime;
    float f4_max_duty_cycle;
    float f4_min_duty_cycle;
    st_current_control_t *p_st_cc;
    p_st_cc = &g_st_cc;
    st_stall_detect_cfg_t st_stall_det_cfg;
    st_trqvib_comp_cfg_t st_trqvib_comp_cfg;

    p_st_cc->u1_flag_volt_err_comp_use       = CURRENT_CFG_VOLT_ERR_COMP;
    p_st_cc->u1_flag_stall_detection_use     = CURRENT_CFG_STALL_DETECTION;
    p_st_cc->u1_flag_trq_vibration_comp_use  = MTR_FLG_CLR;
    p_st_cc->u1_flag_offset_calc             = MTR_FLG_CLR;
    p_st_cc->u2_offset_calc_time             = CURRENT_CFG_OFFSET_CALC_TIME;
    p_st_cc->u2_crnt_offset_cnt              = 0;
    p_st_cc->f4_ctrl_period                  = MOTOR_COMMON_CTRL_PERIOD;
    p_st_cc->f4_lim_iq                       = MOTOR_COMMON_LIMIT_IQ;
    p_st_cc->f4_offset_iu                    = 0.0f;
    p_st_cc->f4_offset_iw                    = 0.0f;
    p_st_cc->f4_sum_iu_ad                    = 0.0f;
    p_st_cc->f4_sum_iw_ad                    = 0.0f;
    p_st_cc->f4_va_max                       = 0.0f;
    p_st_cc->st_pi_id.f4_ilimit              = CURRENT_CFG_PI_INTEGRAL_LIMIT_VD;
    p_st_cc->st_pi_iq.f4_ilimit              = CURRENT_CFG_PI_INTEGRAL_LIMIT_VQ;
    p_st_cc->st_motor.u2_mtr_pp              = MOTOR_CFG_POLE_PAIRS;
    p_st_cc->st_motor.f4_mtr_r               = MOTOR_CFG_RESISTANCE;
    p_st_cc->st_motor.f4_mtr_ld              = MOTOR_CFG_D_INDUCTANCE;
    p_st_cc->st_motor.f4_mtr_lq              = MOTOR_CFG_Q_INDUCTANCE;
    p_st_cc->st_motor.f4_mtr_m               = MOTOR_CFG_MAGNETIC_FLUX;
    p_st_cc->st_motor.f4_mtr_j               = MOTOR_CFG_ROTOR_INERTIA;
    p_st_cc->st_motor.f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS;

    /* low speed sensorless */
    p_st_cc->u1_state_estmode = CURRENT_STATE_ESTMODE_POWEROFF;
    motor_current_lowspdsensorless_init(&(p_st_cc->st_lowspd),
                                        MOTOR_CFG_D_INDUCTANCE,
                                        MOTOR_CFG_Q_INDUCTANCE,
                                        CURRENT_CFG_ESTLOW_PULSEVOLT,
                                        MOTOR_COMMON_CTRL_PERIOD,
                                        CURRENT_CFG_ESTLOW_PULSEFREQ_BOOT);

    /* Current pi control gain */
    motor_current_pi_gain_calc(p_st_cc, CURRENT_CFG_OMEGA, CURRENT_CFG_ZETA);

    /* Modulation */
    motor_current_mod_init(&(p_st_cc->st_mod));

    /* Voltage error compensation */
    motor_current_volt_err_comp_init(&(p_st_cc->st_volt_comp),VERROR_COMP_USE_UVW);

    /* BEMF observer and PLL */
    motor_current_bemf_observer_init(&(p_st_cc->st_bemf_observer), &(p_st_cc->st_motor));
    motor_current_bemf_observer_gain_calc(&(p_st_cc->st_bemf_observer),
                                          &(p_st_cc->st_motor),
                                          CURRENT_CFG_E_OBS_OMEGA,
                                          CURRENT_CFG_E_OBS_ZETA,
                                          MOTOR_COMMON_CTRL_PERIOD);

    (p_st_cc->st_bemf_observer).st_d_axis.f4_d_est_limit = MOTOR_COMMON_CURRENT_OBS_LIMIT;
    (p_st_cc->st_bemf_observer).st_q_axis.f4_d_est_limit = MOTOR_COMMON_CURRENT_OBS_LIMIT;

    motor_current_angle_speed_init(&(p_st_cc->st_pll_est));
    motor_current_angle_speed_gain_calc(&(p_st_cc->st_pll_est),
                                        CURRENT_CFG_PLL_EST_OMEGA,
                                        CURRENT_CFG_PLL_EST_ZETA,
                                        MOTOR_COMMON_CTRL_PERIOD);
    /* low speed sensorless */
    motor_current_angle_speed_gain_calc(&(p_st_cc->st_pll_est_low),
                                        CURRENT_CFG_PLL_ESTLOW_OMEGA,
                                        CURRENT_CFG_PLL_ESTLOW_ZETA,
                                        MOTOR_COMMON_CTRL_PERIOD);

    /* Rotor angle parameter set */
    motor_current_rotor_angle_init(&(p_st_cc->st_rotor_angle));
    /* low speed sensorless */
    motor_current_rotor_angle_init(&(p_st_cc->st_rotor_angle_phasecomp));

    f4_pwm_period_us        = (1000.0f / MOTOR_MCU_CFG_CARRIER_FREQ);
    f4_duty_cycle_deadtime  = INVERTER_CFG_DEADTIME / f4_pwm_period_us;
    f4_min_duty_cycle       = f4_duty_cycle_deadtime;
    f4_max_duty_cycle       = (1.0f - (f4_duty_cycle_deadtime + (MOTOR_COMMON_AD_SAMPLING_TIME / f4_pwm_period_us)));
    motor_current_mod_max_duty_set(&p_st_cc->st_mod, f4_max_duty_cycle);
    motor_current_mod_min_duty_set(&p_st_cc->st_mod, f4_min_duty_cycle);
    motor_current_mod_volt_err_ratio_set(&p_st_cc->st_mod, f4_duty_cycle_deadtime);
    motor_current_volt_err_vlimit_set(&p_st_cc->st_volt_comp, CURRENT_VERR_COMP_LIMIT);

    /* Stall detection */
    st_stall_det_cfg.f4_id_hpf_time     = CURRENT_CFG_STALL_D_HPF_GAIN;
    st_stall_det_cfg.f4_iq_hpf_time     = CURRENT_CFG_STALL_Q_HPF_GAIN;
    st_stall_det_cfg.f4_threshold_level = CURRENT_CFG_STALL_THRESHOLD_LEVEL;
    st_stall_det_cfg.f4_threshold_time  = CURRENT_CFG_STALL_THRESHOLD_TIME;
    motor_current_stall_detection_init(&p_st_cc->st_stalldet, &st_stall_det_cfg);

    /* Torque vibration compensation */
    st_trqvib_comp_cfg.u1_flag_trqvib_comp_learning = MTR_FLG_CLR;
    st_trqvib_comp_cfg.f4_tf_lpf_time               = CURRENT_CFG_TRQVIB_LPF_GAIN;
    st_trqvib_comp_cfg.f4_output_gain               = CURRENT_CFG_TRQVIB_OUTPUT_GAIN;
    st_trqvib_comp_cfg.u2_k2_timelead               = (uint16_t)(CURRENT_CFG_TRQVIB_TIMELEAP
                                                            * TRQVIB_COMP_ARY_SIZE / MTR_TWOPI);
    st_trqvib_comp_cfg.f4_input_weight2             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_2;
    st_trqvib_comp_cfg.f4_input_weight1             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_1;
    st_trqvib_comp_cfg.f4_input_weight0             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_0;
    st_trqvib_comp_cfg.u2_motor_pp                  = MOTOR_CFG_POLE_PAIRS;
    motor_current_trq_vibration_compensation_init(&p_st_cc->st_trqvib_comp, &st_trqvib_comp_cfg);

    R_MOTOR_CURRENT_Reset(p_st_cc);
} /* End of function R_MOTOR_CURRENT_Open */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Close
* Description   : Close current control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Close(void)
{
    st_current_control_t *p_st_cc;
    p_st_cc = &g_st_cc;

    R_MOTOR_CURRENT_Reset(p_st_cc);
} /* End of function R_MOTOR_CURRENT_Close */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Reset
* Description   : Resets current control module
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Reset(st_current_control_t *p_st_cc)
{
    p_st_cc->u1_active           = MTR_FLG_CLR;

    p_st_cc->f4_refu             = 0.0f;
    p_st_cc->f4_refv             = 0.0f;
    p_st_cc->f4_refw             = 0.0f;
    p_st_cc->f4_vd_ref           = 0.0f;
    p_st_cc->f4_vd_ref2          = 0.0f; /* low speed sennsorless*/
    p_st_cc->f4_vq_ref           = 0.0f;
    p_st_cc->f4_id_ref           = 0.0f;
    p_st_cc->f4_iq_ref           = 0.0f;
    p_st_cc->f4_id_ad            = 0.0f;
    p_st_cc->f4_iq_ad            = 0.0f;
    p_st_cc->f4_id_ad2           = 0.0f; /* low speed sennsorless*/
    p_st_cc->f4_iq_ad2           = 0.0f; /* low speed sennsorless*/
    p_st_cc->f4_vdc_ad           = 0.0f;
    p_st_cc->f4_iu_ad            = 0.0f;
    p_st_cc->f4_iv_ad            = 0.0f;
    p_st_cc->f4_iw_ad            = 0.0f;
    p_st_cc->f4_modu             = 0.0f;
    p_st_cc->f4_modv             = 0.0f;
    p_st_cc->f4_modw             = 0.0f;
    p_st_cc->f4_speed_rad        = 0.0f;
    p_st_cc->f4_ref_id_ctrl      = 0.0f;
    p_st_cc->f4_ref_iq_ctrl      = 0.0f;
    p_st_cc->f4_ed               = 0.0f;
    p_st_cc->f4_eq               = 0.0f;
    p_st_cc->f4_phase_err_rad    = 0.0f;
    p_st_cc->st_pi_id.f4_err     = 0.0f;
    p_st_cc->st_pi_iq.f4_err     = 0.0f;
    p_st_cc->st_pi_id.f4_refi    = 0.0f;
    p_st_cc->st_pi_iq.f4_refi    = 0.0f;

    /* low speed sensorless */
    motor_current_lowspdsensorless_init(&(p_st_cc->st_lowspd),
                                        MOTOR_CFG_D_INDUCTANCE,
                                        MOTOR_CFG_Q_INDUCTANCE,
                                        CURRENT_CFG_ESTLOW_PULSEVOLT,
                                        MOTOR_COMMON_CTRL_PERIOD,
                                        CURRENT_CFG_ESTLOW_PULSEFREQ_BOOT);

    motor_current_rotor_angle_reset(&(p_st_cc->st_rotor_angle));
    motor_current_rotor_angle_reset(&(p_st_cc->st_rotor_angle_phasecomp));

    /* Reset BEMF observer and PLL */
    motor_current_bemf_observer_reset(&(p_st_cc->st_bemf_observer));
    motor_current_angle_speed_reset(&(p_st_cc->st_pll_est));
    motor_current_angle_speed_reset(&(p_st_cc->st_pll_est_low));/* low speed sennsorless*/
    /* Reset Stall detection */
    motor_current_stall_detection_reset(&(p_st_cc->st_stalldet));

    /* Reset Torque vibration compensation */
    motor_current_trq_vibration_compensation_reset(&(p_st_cc->st_trqvib_comp));
} /* End of function R_MOTOR_CURRENT_Reset */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_Run
* Description   : Run current control module
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Run(st_current_control_t *p_st_cc)
{
    p_st_cc->u1_active           = MTR_FLG_SET;
} /* End of function R_MOTOR_CURRENT_Run */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterSet
* Description   : Set the current control parameter
* Arguments     : p_st_cc            - The pointer to the current control structure
*                 p_st_current_input - The pointer to the current input parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterSet(st_current_control_t *p_st_cc, const st_current_input_t * p_st_current_input)
{
    p_st_cc->f4_iu_ad               = p_st_current_input->f4_iu_ad;
    p_st_cc->f4_iv_ad               = p_st_current_input->f4_iv_ad;
    p_st_cc->f4_iw_ad               = p_st_current_input->f4_iw_ad;
    p_st_cc->f4_vdc_ad              = p_st_current_input->f4_vdc_ad;
    p_st_cc->f4_iq_ref              = p_st_current_input->f4_iq_ref;
    p_st_cc->f4_id_ref              = p_st_current_input->f4_id_ref;
    p_st_cc->f4_ref_speed_rad_ctrl  = p_st_current_input->f4_ref_speed_rad_ctrl;
} /* End of function R_MOTOR_CURRENT_ParameterSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterGet
* Description   : Get the current control parameter
* Arguments     : p_st_cc             - The pointer to the current control structure
*                 p_st_current_output - The pointer to the current control output parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterGet(st_current_control_t *p_st_cc, st_current_output_t *p_st_current_output)
{
    p_st_current_output->u1_flag_offset_calc = p_st_cc->u1_flag_offset_calc;
    p_st_current_output->f4_modu             = p_st_cc->f4_modu;
    p_st_current_output->f4_modv             = p_st_cc->f4_modv;
    p_st_current_output->f4_modw             = p_st_cc->f4_modw;
    p_st_current_output->f4_neutral_duty     = p_st_cc->st_mod.f4_neutral_duty;
    p_st_current_output->f4_va_max           = p_st_cc->f4_va_max;
    p_st_current_output->f4_ref_id_ctrl      = p_st_cc->f4_ref_id_ctrl;
    p_st_current_output->f4_speed_rad        = p_st_cc->f4_speed_rad / p_st_cc->st_motor.u2_mtr_pp;
    p_st_current_output->f4_ed               = p_st_cc->f4_ed;
    p_st_current_output->f4_eq               = p_st_cc->f4_eq;
    p_st_current_output->f4_phase_err_rad    = p_st_cc->f4_phase_err_rad;
} /* End of function R_MOTOR_CURRENT_ParameterGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_ParameterUpdate
* Description   : Update the current control parameter
* Arguments     : p_st_cc          - The pointer to the current control structure
*                 p_st_current_cfg - The pointer to the current control configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_ParameterUpdate(st_current_control_t *p_st_cc, const st_current_cfg_t *p_st_current_cfg)
{   

    st_stall_detect_cfg_t st_stall_det_cfg;
    st_trqvib_comp_cfg_t st_trqvib_comp_cfg;

    float f4_ia_max;
    p_st_cc->u1_flag_volt_err_comp_use      = p_st_current_cfg->u1_flag_volt_err_comp_use;
    p_st_cc->u1_flag_stall_detection_use    = p_st_current_cfg->u1_flag_stall_detection_use;
    p_st_cc->u1_flag_trq_vibration_comp_use = p_st_current_cfg->u1_flag_trq_vibration_comp_use;
    p_st_cc->u2_offset_calc_time            = p_st_current_cfg->u2_offset_calc_time;
    p_st_cc->f4_ctrl_period                 = p_st_current_cfg->f4_ctrl_period;
    p_st_cc->st_motor                       = p_st_current_cfg->st_motor;
    f4_ia_max                               = p_st_cc->st_motor.f4_nominal_current_rms
                                                    * MOTOR_COMMON_CFG_IA_MAX_CALC_MULT;
    p_st_cc->f4_lim_iq                      = f4_ia_max;

    /* low speed sensorless */
    p_st_cc->st_lowspd.f4_ld_pulse     = p_st_current_cfg->st_motor.f4_mtr_ld;
    p_st_cc->st_lowspd.f4_lq_pulse     = p_st_current_cfg->st_motor.f4_mtr_lq;
    p_st_cc->st_lowspd.f4_highspd_threshold = p_st_current_cfg->f4_highspd_threshold * MTR_RPM2RAD;
    p_st_cc->st_lowspd.f4_lowspd_threshold  = p_st_current_cfg->f4_lowspd_threshold * MTR_RPM2RAD;

    /* The dq-axis current PI gains */
    motor_current_pi_gain_calc(p_st_cc, p_st_current_cfg->f4_current_omega_hz, p_st_current_cfg->f4_current_zeta);

    st_stall_det_cfg.f4_id_hpf_time     = p_st_current_cfg->f4_id_hpf_time;
    st_stall_det_cfg.f4_iq_hpf_time     = p_st_current_cfg->f4_iq_hpf_time;
    st_stall_det_cfg.f4_threshold_level = p_st_current_cfg->f4_threshold_level;
    st_stall_det_cfg.f4_threshold_time  = p_st_current_cfg->f4_threshold_time;
    motor_current_stall_detection_parameter_set(&p_st_cc->st_stalldet, &st_stall_det_cfg);

    st_trqvib_comp_cfg.u1_flag_trqvib_comp_learning = p_st_current_cfg->u1_flag_trqvib_comp_learning;
    st_trqvib_comp_cfg.f4_tf_lpf_time               = p_st_current_cfg->f4_tf_lpf_time;
    st_trqvib_comp_cfg.f4_output_gain               = p_st_current_cfg->f4_output_gain;
    st_trqvib_comp_cfg.u2_k2_timelead               = (uint16_t)(p_st_current_cfg->f4_timelead
                                                            * TRQVIB_COMP_ARY_SIZE / MTR_TWOPI);
    st_trqvib_comp_cfg.f4_input_weight2             = p_st_current_cfg->f4_input_weight2;
    st_trqvib_comp_cfg.f4_input_weight1             = p_st_current_cfg->f4_input_weight1;
    st_trqvib_comp_cfg.f4_input_weight0             = p_st_current_cfg->f4_input_weight0;
    st_trqvib_comp_cfg.u2_motor_pp                  = p_st_current_cfg->st_motor.u2_mtr_pp;
    motor_current_trq_vibration_compensation_parameter_set(&p_st_cc->st_trqvib_comp, &st_trqvib_comp_cfg);
} /* End of function R_MOTOR_CURRENT_ParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_CurrentCyclic
* Description   : The cyclic processing of current control
* Arguments     : p_st_cc       - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_CurrentCyclic(st_current_control_t *p_st_cc)
{
    float f4_angle_rad;
    if (MTR_FLG_SET == p_st_cc->u1_active)
    {
        motor_current_mod_vdc_set(&p_st_cc->st_mod, p_st_cc->f4_vdc_ad);

        p_st_cc->f4_ref_iq_ctrl = motor_current_reference_iq_set(p_st_cc);
        p_st_cc->f4_ref_id_ctrl = motor_current_reference_id_set(p_st_cc);

        /*=============================================*/
        /*     Coordinate transformation (UVW->dq)     */
        /*=============================================*/
        motor_current_transform_uvw_dq_abs(&p_st_cc->st_rotor_angle, &(p_st_cc->f4_iu_ad), &(p_st_cc->f4_id_ad));


        p_st_cc->f4_ia_max  = sqrt(p_st_cc->f4_id_ad*p_st_cc->f4_id_ad + p_st_cc->f4_iq_ad*p_st_cc->f4_iq_ad);

        /*==============================*/
        /*     Angle & speed setting    */
        /*==============================*/
        motor_current_angle_speed_detect(p_st_cc);

        /*==============================*/
        /*     Stall detection          */
        /*==============================*/
        if (MTR_FLG_SET == p_st_cc->u1_flag_stall_detection_use)
        {
            motor_current_stall_detection_main(&(p_st_cc->st_stalldet), &(p_st_cc->f4_id_ad));
        }

        /*=============================================*/
        /*     Torque Vibration Compensation           */
        /*=============================================*/
        if (MTR_FLG_SET == p_st_cc->u1_flag_trq_vibration_comp_use)
        {
            g_u1_trq_vibration_comp_update_flag = MTR_FLG_SET;

            motor_current_trq_vibration_compensation_preproc(&(p_st_cc->st_trqvib_comp),
                                                             p_st_cc->st_rotor_angle.f4_rotor_angle_rad,
                                                             p_st_cc->f4_ref_speed_rad_ctrl,
                                                             p_st_cc->f4_speed_rad / p_st_cc->st_motor.u2_mtr_pp);

            /* low speed sensorles:u1_state_estmodeのモータ駆動時のステータスに変更する */
            if((p_st_cc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_SLOW) ||
                (p_st_cc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_MID) ||
                (p_st_cc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_MID_M) ||
                (p_st_cc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_HIGH) )

            {
                motor_current_trq_vibration_compensation_main(&(p_st_cc->st_trqvib_comp),
                                                              p_st_cc->f4_ref_speed_rad_ctrl,
                                                              p_st_cc->f4_speed_rad / p_st_cc->st_motor.u2_mtr_pp);
            }

            p_st_cc->f4_ref_iq_ctrl += p_st_cc->st_trqvib_comp.f4_last_spd_comp;
        }
        else
        {
            if (MTR_FLG_SET == g_u1_trq_vibration_comp_update_flag)
            {
                motor_current_trq_vibration_compensation_reset(&(p_st_cc->st_trqvib_comp));
                g_u1_trq_vibration_comp_update_flag = MTR_FLG_CLR;
            }
        }

        /*====================================*/
        /*     Feedback control (Current)     */
        /*====================================*/
        motor_current_pi_control(p_st_cc);

        /*============================*/
        /*     Decoupling control     */
        /*============================*/
        motor_current_decoupling(p_st_cc, p_st_cc->f4_speed_rad, &(p_st_cc->st_motor));

        /*============================*/
        /*     Pule Injection     */
        /*============================*/
        switch( p_st_cc->u1_state_estmode)
        {
            case CURRENT_STATE_ESTMODE_POSEST:
                p_st_cc->f4_vd_ref = p_st_cc->st_lowspd.f4_vd_ref_pulse_inj;
                p_st_cc->f4_vd_ref2 = 0.0f;
                p_st_cc->f4_vq_ref = 0.0f;
                break;

            case CURRENT_STATE_ESTMODE_DRIVE_SLOW:
                p_st_cc->f4_vd_ref2 = p_st_cc->f4_vd_ref;
                p_st_cc->f4_vd_ref += p_st_cc->st_lowspd.f4_vd_ref_pulse_inj;
                break;

            case CURRENT_STATE_ESTMODE_DRIVE_MID:
                p_st_cc->f4_vd_ref2 = p_st_cc->f4_vd_ref;

                if( p_st_cc->st_lowspd.u1_cur_chg_cnt <= 1)
                {
                    /* first time of this state, add 1/2 injection pulse */
                    p_st_cc->f4_vd_ref += p_st_cc->st_lowspd.f4_vd_ref_pulse_inj / 2.0f;
                }
                else
                {
                    /* No injection (changing to BEMF observer, suppress ripple current.) */
                }

                break;

            case CURRENT_STATE_ESTMODE_DRIVE_MID_M:
                p_st_cc->f4_vd_ref2 = p_st_cc->f4_vd_ref;
                p_st_cc->f4_vd_ref += p_st_cc->st_lowspd.f4_vd_ref_pulse_inj / 2.0f;
                break;

            case CURRENT_STATE_ESTMODE_DRIVE_HIGH:
                p_st_cc->f4_vd_ref2 = p_st_cc->f4_vd_ref;
                break;

            case CURRENT_STATE_ESTMODE_POWEROFF:
            case CURRENT_STATE_ESTMODE_INIT:
                p_st_cc->f4_vd_ref2 = 0.0f;
                p_st_cc->f4_vd_ref = 0.0f;
                p_st_cc->f4_vq_ref = 0.0f;
                break;

        }

        /*==============================*/
        /*     Limit voltage vector     */
        /*==============================*/
        motor_current_volt_limit(p_st_cc);

        /*=====================================*/
        /*     Sampling delay compensation     */
        /*=====================================*/
        f4_angle_rad = motor_current_control_smpldly_comp(p_st_cc,
                                                          p_st_cc->st_rotor_angle.f4_rotor_angle_rad,
                                                          p_st_cc->f4_speed_rad);
        motor_current_rotor_angle_update(&p_st_cc->st_rotor_angle_phasecomp, f4_angle_rad);

        /*=================================================*/
        /*     Coordinate transformation (dq->uvw)         */
        /*=================================================*/
        motor_current_transform_dq_uvw_abs(&p_st_cc->st_rotor_angle_phasecomp,
                                           &(p_st_cc->f4_vd_ref),
                                           &p_st_cc->f4_refu);

        /*====================================*/
        /*     Voltage error compensation     */
        /*====================================*/
        if ((MTR_FLG_SET == p_st_cc->u1_flag_volt_err_comp_use) && (MTR_FLG_SET == p_st_cc->u1_flag_offset_calc))
        {
            float f4_iuvw_ref[3];
            motor_current_transform_dq_uvw_abs(&p_st_cc->st_rotor_angle_phasecomp,
                                               &(p_st_cc->f4_ref_id_ctrl),
                                               f4_iuvw_ref);
            motor_current_volt_err_comp_main(&(p_st_cc->st_volt_comp),
                                             &p_st_cc->f4_refu,
                                             f4_iuvw_ref,
                                             p_st_cc->f4_vdc_ad);
        }

        /*==================================*/
        /*     Space vector modulation      */
        /*==================================*/
        motor_current_mod_start(&(p_st_cc->st_mod), &p_st_cc->f4_refu, &p_st_cc->f4_modu);
    }
} /* End of function R_MOTOR_CURRENT_CurrentCyclic */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_OffsetCalibration
* Description   : Measures the current offset
* Arguments     : p_st_cc - The pointer to the current control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_OffsetCalibration(st_current_control_t *p_st_cc)
{
    p_st_cc->f4_offset_iu = 0.0f;
    p_st_cc->f4_offset_iw = 0.0f;

    /*==================================*/
    /*    Current offset calibration    */
    /*==================================*/
    p_st_cc->u2_crnt_offset_cnt++;
    if (p_st_cc->u2_offset_calc_time <= p_st_cc->u2_crnt_offset_cnt)
    {
        p_st_cc->u1_flag_offset_calc    = MTR_FLG_SET;
        p_st_cc->u2_crnt_offset_cnt     = p_st_cc->u2_offset_calc_time;
        p_st_cc->f4_offset_iu           = p_st_cc->f4_sum_iu_ad / p_st_cc->u2_offset_calc_time;
        p_st_cc->f4_offset_iw           = p_st_cc->f4_sum_iw_ad / p_st_cc->u2_offset_calc_time;
    }
    else
    {
        p_st_cc->f4_sum_iu_ad += p_st_cc->f4_iu_ad;
        p_st_cc->f4_sum_iw_ad += p_st_cc->f4_iw_ad;
    }
} /* End of function R_MOTOR_CURRENT_OffsetCalibration */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_CurrentOffsetRemove
* Description   : Calibrates the current offset
* Arguments     : p_st_cc - The pointer to the current control structure
*                 p_f4_iu - The pointer to measured phase u current
*                 p_f4_iw - The pointer to measured phase w current
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_CurrentOffsetRemove(st_current_control_t *p_st_cc, float *p_f4_iu, float *p_f4_iw)
{
    /* Current offset adjustment */
    *p_f4_iu = (*p_f4_iu) - p_st_cc->f4_offset_iu;
    *p_f4_iw = (*p_f4_iw) - p_st_cc->f4_offset_iw;
} /* End of function R_MOTOR_CURRENT_CurrentOffsetRemove */

/**********************************************************************************************************************
 * Function Name: R_MOTOR_CURRENT_VoltErrCompParamSet
 * Description  : Set voltage error compensation parameters
 * Arguments    : p_st_cc - The pointer to the current control structure
 *                crnt_array - The current array [A]
 *                volterr_array - The voltage error array [V]
 *                ref_voltage - The DC bus voltage that table created for
 * Return Value : None
 *********************************************************************************************************************/
void R_MOTOR_CURRENT_VoltErrCompParamSet(st_current_control_t *p_st_cc,
                                         const float *crnt_array,
                                         const float *volterr_array,
                                         float ref_voltage)
{
    motor_current_volt_err_comp_table_set(&p_st_cc->st_volt_comp, crnt_array, volterr_array, ref_voltage);
} /* End of function R_MOTOR_CURRENT_VoltErrCompParamSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_BEMFObserverParameterUpdate
* Description   : Update the BEMF observer parameter
* Arguments     : p_st_cc           - The pointer to current control structure
*                 p_st_bemf_obs_cfg - The pointer to BEMF observer configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_BEMFObserverParameterUpdate(st_current_control_t * p_st_cc,
                                                 st_bemf_observer_cfg_t * p_st_bemf_obs_cfg)
{
    float f4_est_limit = (float)(p_st_cc->st_motor.f4_nominal_current_rms * MTR_SQRT_3 * 3.0);

    motor_current_bemf_observer_gain_calc(&p_st_cc->st_bemf_observer,
                                          &p_st_cc->st_motor,
                                          p_st_bemf_obs_cfg->f4_e_obs_omega_hz,
                                          p_st_bemf_obs_cfg->f4_e_obs_zeta,
                                          p_st_cc->f4_ctrl_period);

    motor_current_angle_speed_gain_calc(&p_st_cc->st_pll_est,
                                        p_st_bemf_obs_cfg->f4_pll_est_omega_hz,
                                        p_st_bemf_obs_cfg->f4_pll_est_zeta,
                                        p_st_cc->f4_ctrl_period);
    /* low speed sensorless */
    motor_current_angle_speed_gain_calc(&p_st_cc->st_pll_est_low,
                                        p_st_bemf_obs_cfg->f4_pll_estlow_omega_hz,
                                        p_st_bemf_obs_cfg->f4_pll_estlow_zeta,
                                        p_st_cc->f4_ctrl_period);

    p_st_cc->st_bemf_observer.st_d_axis.f4_d_est_limit = f4_est_limit;
    p_st_cc->st_bemf_observer.st_q_axis.f4_d_est_limit = f4_est_limit;

} /* End of function R_MOTOR_CURRENT_BEMFObserverParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_UpdateAngleNSpole
* Description   : Update the rotor angle by NS poles detection(PF)
* Arguments     : p_st_cc - The pointer to the current control structure
*                 f4_pf - PF value generated by NS pole detection
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_UpdateAngleNSpole(st_current_control_t *p_st_cc, float f4_pf)
{
    float f4_angle_rad = motor_current_angle_get(&p_st_cc->st_rotor_angle);

    /* PF angle update */
    if( f4_pf > 0.0f )
    {
        /* PF > 0 */
        /* Add 180deg (3.14 rad) */
        f4_angle_rad = f4_angle_rad + (MTR_TWOPI/2.0f);
    }
    else
    {
        /* PF < 0 */
    }

    /* 0-2pi Limit */
    if( f4_angle_rad > MTR_TWOPI)
    {
        f4_angle_rad = f4_angle_rad - MTR_TWOPI;
    }
    else if( f4_angle_rad < 0.0f)
    {
        f4_angle_rad = f4_angle_rad + MTR_TWOPI;
    }
    else
    {
        /* Do nothing */
    }

    motor_current_rotor_angle_update(&p_st_cc->st_rotor_angle, f4_angle_rad);
} /* End of function R_MOTOR_CURRENT_UpdateAngleNSpole */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_PLLSpeedSet
* Description   : Preset the PLL integral term
* Arguments     : p_st_cc - The pointer to current control structure
*                 f4_speed_ini_est - initial speed value
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_PLLSpeedSet(st_current_control_t * p_st_cc, float f4_speed_ini_est)
{
    p_st_cc->f4_speed_rad               = f4_speed_ini_est;
    p_st_cc->st_pll_est.f4_i_est_speed  = f4_speed_ini_est;
    p_st_cc->st_pll_est_low.f4_i_est_speed  = f4_speed_ini_est;
} /* End of function R_MOTOR_CURRENT_PLLSpeedSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_RotorAngleSet
* Description   : Preset the angle and related trigonometric values
* Arguments     : p_st_cc - The pointer to current control structure
*               : f4_angle_rad - The angle[rad] used to update the structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_RotorAngleSet(st_current_control_t * p_st_cc, float f4_angle_rad)
{
    motor_current_rotor_angle_update(&p_st_cc->st_rotor_angle, f4_angle_rad);
} /* End of function R_MOTOR_CURRENT_RotorAngleSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_BEMFObserverParameterSet
* Description   : Preset Q-axis estimated voltage disturbance
* Arguments     : p_st_cc - The pointer to current control structure
*               : f4_d_est - Q-axis estimated voltage disturbance
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_BEMFObserverParameterSet(st_current_control_t * p_st_cc, float f4_d_est)
{
    p_st_cc->st_bemf_observer.st_q_axis.f4_d_est        = -f4_d_est;
    p_st_cc->st_bemf_observer.st_q_axis.f4_d_est_pre    = -f4_d_est;
} /* End of function R_MOTOR_CURRENT_BEMFObserverParameterSet */
