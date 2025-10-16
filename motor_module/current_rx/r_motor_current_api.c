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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_current_api.c
* Description : The current control module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
*         : 01.03.2024 1.20
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_motor_current.h"
#include "r_motor_current_api.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_current_control_t      g_st_cc;           /* Current control structure */

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

    p_st_cc->u1_flag_volt_err_comp_use       = CURRENT_CFG_VOLT_ERR_COMP;
    p_st_cc->u1_state_id_ref                 = CURRENT_STATE_ID_ZERO_CONST;
    p_st_cc->u1_state_iq_ref                 = CURRENT_STATE_IQ_ZERO_CONST;
    p_st_cc->u1_flag_offset_calc             = MTR_FLG_CLR;
    p_st_cc->u2_offset_calc_time             = CURRENT_CFG_OFFSET_CALC_TIME;
    p_st_cc->u2_crnt_offset_cnt              = 0;
    p_st_cc->f4_ctrl_period                  = MOTOR_COMMON_CTRL_PERIOD;
    p_st_cc->f4_lim_iq                       = MOTOR_COMMON_LIMIT_IQ(g_st_cc.u1_cc_motor_id);
    p_st_cc->f4_offset_iu                    = 0.0f;
    p_st_cc->f4_offset_iw                    = 0.0f;
    p_st_cc->f4_sum_iu_ad                    = 0.0f;
    p_st_cc->f4_sum_iw_ad                    = 0.0f;

    p_st_cc->f4_iq_down_step                 = 0.0f;
    p_st_cc->f4_iq_down_step_time_inv        = CURRENT_CFG_IQ_DOWN_STEP_TIME_INV;
/*    if (g_st_cc.u1_motor_id == 1)
    {
        p_st_cc->f4_id_up_step                   = CIRC_MOTOR_COMMON_ID_UP_STEP_RATE;
        p_st_cc->f4_id_down_step                 = CIRC_MOTOR_COMMON_ID_DOWN_STEP_RATE;
    	p_st_cc->f4_ol_ref_id                    = CIRC_CURRENT_CFG_REF_ID_OPENLOOP;
    }
    else
    {
        p_st_cc->f4_id_down_step                 = DRY_MOTOR_COMMON_ID_DOWN_STEP_RATE;
        p_st_cc->f4_id_up_step                   = DRY_MOTOR_COMMON_ID_UP_STEP_RATE;
    	p_st_cc->f4_ol_ref_id                    = DRY_CURRENT_CFG_REF_ID_OPENLOOP;
    }
    */
    p_st_cc->f4_id_up_step 					 = MOTOR_COMMON_ID_UP_STEP_RATE(g_st_cc.u1_cc_motor_id);
    p_st_cc->f4_id_down_step				 = MOTOR_COMMON_ID_DOWN_STEP_RATE(g_st_cc.u1_cc_motor_id);
    p_st_cc->f4_ol_ref_id 					 = CURRENT_CFG_REF_ID_OPENLOOP(g_st_cc.u1_cc_motor_id);

    p_st_cc->f4_va_max                       = 0.0f;
    p_st_cc->st_pi_id.f4_ilimit              = CURRENT_CFG_PI_INTEGRAL_LIMIT_VD;
    p_st_cc->st_pi_iq.f4_ilimit              = CURRENT_CFG_PI_INTEGRAL_LIMIT_VQ;
    p_st_cc->st_motor.u2_mtr_pp              = MOTOR_CFG_POLE_PAIRS(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_mtr_r               = MOTOR_CFG_RESISTANCE(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_mtr_ld              = MOTOR_CFG_D_INDUCTANCE(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_mtr_lq              = MOTOR_CFG_Q_INDUCTANCE(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_mtr_m               = MOTOR_CFG_MAGNETIC_FLUX(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_mtr_j               = MOTOR_CFG_ROTOR_INERTIA(g_st_cc.u1_cc_motor_id);
    p_st_cc->st_motor.f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS(g_st_cc.u1_cc_motor_id);
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    p_st_cc->s2_difference_minimum           = CURRENT_CFG_MIN_DIFFERENCE_DUTY;
    p_st_cc->s2_adjust_adc_delay             = CURRENT_CFG_ADJUST_ADC_DELAY;
    p_st_cc->f4_pwm_period_cnt               = MOTOR_COMMON_CARRIER_SET_BASE;
    p_st_cc->f4_pwm_dead_time_cnt            = MOTOR_COMMON_DEADTIME_SET;
#endif

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
                                          CURRENT_CFG_E_OBS_OMEGA(g_st_cc.u1_cc_motor_id),
                                          CURRENT_CFG_E_OBS_ZETA,
                                          MOTOR_COMMON_CTRL_PERIOD);
    (p_st_cc->st_bemf_observer).st_d_axis.f4_d_est_limit = MOTOR_COMMON_CURRENT_OBS_LIMIT(g_st_cc.u1_cc_motor_id);
    (p_st_cc->st_bemf_observer).st_q_axis.f4_d_est_limit = MOTOR_COMMON_CURRENT_OBS_LIMIT(g_st_cc.u1_cc_motor_id);

    motor_current_angle_speed_init(&(p_st_cc->st_pll_est));
    motor_current_angle_speed_gain_calc(&(p_st_cc->st_pll_est),
                                        CURRENT_CFG_PLL_EST_OMEGA(g_st_cc.u1_cc_motor_id),
                                        CURRENT_CFG_PLL_EST_ZETA,
                                        MOTOR_COMMON_CTRL_PERIOD);
    /* Rotor angle parameter set */
    motor_current_rotor_angle_init(&(p_st_cc->st_rotor_angle));

    f4_pwm_period_us = (1000.0f / MOTOR_MCU_CFG_CARRIER_FREQ);
    f4_duty_cycle_deadtime = INVERTER_CFG_DEADTIME / f4_pwm_period_us;
    f4_min_duty_cycle = f4_duty_cycle_deadtime;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    f4_max_duty_cycle = (1.0f - f4_duty_cycle_deadtime);
#else
    f4_max_duty_cycle = (1.0f - (f4_duty_cycle_deadtime + (MOTOR_COMMON_AD_SAMPLING_TIME / f4_pwm_period_us)));
#endif
    motor_current_mod_max_duty_set(&p_st_cc->st_mod, f4_max_duty_cycle);
    motor_current_mod_min_duty_set(&p_st_cc->st_mod, f4_min_duty_cycle);
    motor_current_mod_volt_err_ratio_set(&p_st_cc->st_mod, f4_duty_cycle_deadtime);
    motor_current_volt_err_vlimit_set(&p_st_cc->st_volt_comp, CURRENT_VERR_COMP_LIMIT);
    /* Initialization of the current filter*/
    motor_filter_first_order_lpff_init(&p_st_cc->st_current_filter_u);
    motor_filter_first_order_lpff_gain_calc(&p_st_cc->st_current_filter_u,
            CURRENT_CFG_LPF_OMEGA, CURRENT_CFG_CTRL_PERIOD);

    motor_filter_first_order_lpff_init(&p_st_cc->st_current_filter_v);
    motor_filter_first_order_lpff_gain_calc(&p_st_cc->st_current_filter_v,
            CURRENT_CFG_LPF_OMEGA, CURRENT_CFG_CTRL_PERIOD);

    motor_filter_first_order_lpff_init(&p_st_cc->st_current_filter_w);
    motor_filter_first_order_lpff_gain_calc(&p_st_cc->st_current_filter_w,
            CURRENT_CFG_LPF_OMEGA, CURRENT_CFG_CTRL_PERIOD);


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
    p_st_cc->f4_vq_ref           = 0.0f;
    p_st_cc->f4_id_ref           = 0.0f;
    p_st_cc->f4_iq_ref           = 0.0f;
    p_st_cc->f4_id_ad            = 0.0f;
    p_st_cc->f4_iq_ad            = 0.0f;
    p_st_cc->f4_vdc_ad           = 0.0f;
    p_st_cc->f4_iu_ad            = 0.0f;
    p_st_cc->f4_iv_ad            = 0.0f;
    p_st_cc->f4_iw_ad            = 0.0f;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    p_st_cc->u2_duty_u           = 0;
    p_st_cc->u2_duty_v           = 0;
    p_st_cc->u2_duty_w           = 0;
    p_st_cc->u2_duty_buff_u      = 0;
    p_st_cc->u2_duty_buff_v      = 0;
    p_st_cc->u2_duty_buff_w      = 0;
#else
    p_st_cc->f4_modu             = 0.0f;
    p_st_cc->f4_modv             = 0.0f;
    p_st_cc->f4_modw             = 0.0f;
#endif
    p_st_cc->f4_speed_rad        = 0.0f;
    p_st_cc->f4_ref_id_ctrl      = 0.0f;
    p_st_cc->f4_ref_iq_ctrl      = 0.0f;
    p_st_cc->f4_ed               = 0.0f;
    p_st_cc->f4_eq               = 0.0f;
    p_st_cc->f4_phase_err_rad    = 0.0f;
    p_st_cc->f4_ol_speed_rad     = 0.0f;
    p_st_cc->st_pi_id.f4_err     = 0.0f;
    p_st_cc->st_pi_iq.f4_err     = 0.0f;
    p_st_cc->st_pi_id.f4_refi    = 0.0f;
    p_st_cc->st_pi_iq.f4_refi    = 0.0f;

    motor_current_rotor_angle_reset(&(p_st_cc->st_rotor_angle));

    /* Reset BEMF observer and PLL */
    motor_current_bemf_observer_reset(&(p_st_cc->st_bemf_observer));
    motor_current_angle_speed_reset(&(p_st_cc->st_pll_est));

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
    p_st_cc->u1_state_id_ref = p_st_current_input->u1_state_id_ref;
    p_st_cc->u1_state_iq_ref = p_st_current_input->u1_state_iq_ref;
    p_st_cc->f4_iu_ad        = p_st_current_input->f4_iu_ad;
    p_st_cc->f4_iv_ad        = p_st_current_input->f4_iv_ad;
    p_st_cc->f4_iw_ad        = p_st_current_input->f4_iw_ad;
    p_st_cc->f4_vdc_ad       = p_st_current_input->f4_vdc_ad;
    p_st_cc->f4_iq_ref       = p_st_current_input->f4_iq_ref;
    p_st_cc->f4_id_ref       = p_st_current_input->f4_id_ref;
    p_st_cc->f4_ol_speed_rad = p_st_current_input->f4_ol_speed_rad;
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
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
    p_st_current_output->f4_modu             = p_st_cc->f4_modu;
    p_st_current_output->f4_modv             = p_st_cc->f4_modv;
    p_st_current_output->f4_modw             = p_st_cc->f4_modw;
#endif
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
    float f4_ia_max;
    p_st_cc->u1_flag_volt_err_comp_use = p_st_current_cfg->u1_flag_volt_err_comp_use;
    p_st_cc->u2_offset_calc_time       = p_st_current_cfg->u2_offset_calc_time;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    p_st_cc->s2_difference_minimum     = p_st_current_cfg->s2_difference_minimum;
    p_st_cc->s2_adjust_adc_delay       = p_st_current_cfg->s2_adjust_adc_delay;
#endif
    p_st_cc->f4_ctrl_period            = p_st_current_cfg->f4_ctrl_period;
    p_st_cc->f4_id_up_step             = p_st_current_cfg->f4_id_up_step;
    p_st_cc->f4_id_down_step           = p_st_current_cfg->f4_id_down_step;
    p_st_cc->f4_iq_down_step_time_inv  = p_st_current_cfg->f4_iq_down_step_time_inv;
    p_st_cc->f4_ol_ref_id              = p_st_current_cfg->f4_ol_ref_id;
    p_st_cc->st_motor                  = p_st_current_cfg->st_motor;
    f4_ia_max                          = p_st_cc->st_motor.f4_nominal_current_rms * MOTOR_COMMON_CFG_IA_MAX_CALC_MULT;
    p_st_cc->f4_lim_iq                 = f4_ia_max;

    /* The dq-axis current PI gains */
    motor_current_pi_gain_calc(p_st_cc, p_st_current_cfg->f4_current_omega_hz, p_st_current_cfg->f4_current_zeta);
} /* End of function R_MOTOR_CURRENT_ParameterUpdate */

#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_1ShuntCurrentGet
* Description   : Determine the three phase current for 1shunt
* Arguments     : p_st_cc     - The pointer to the current control structure
*                 p_f4_iu_ad  - U phase current A/D conversion value (pointer)
*                 p_f4_iv_ad  - V phase current A/D conversion value (pointer)
*                 p_f4_iw_ad  - W phase current A/D conversion value (pointer)
*                 p_f4_Iac_ad - Phase current A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_1ShuntCurrentGet(st_current_control_t *p_st_cc,
                                      float * p_f4_iu_ad,
                                      float * p_f4_iv_ad,
                                      float * p_f4_iw_ad,
                                      float * p_f4_Iac_ad)
{
    float f4_addata[3] = {0.0f};
    float f4_Iac_ad2   = 0.0f;

    /* Translate double buffer A/D data to 3 phase currents */
    p_f4_Iac_ad[0] = p_f4_Iac_ad[0] - p_st_cc->f4_offset_iu;
    p_f4_Iac_ad[1] = p_f4_Iac_ad[1] - p_st_cc->f4_offset_iw;

    f4_Iac_ad2 = -(p_f4_Iac_ad[0] + p_f4_Iac_ad[1]);

    switch (p_st_cc->min_phase)
    {
        case CURRENT_PHASE_U_PHASE:

            f4_addata[0] = p_f4_Iac_ad[0];
            if (CURRENT_PHASE_V_PHASE == p_st_cc->mid_phase)
            {
                f4_addata[2] = p_f4_Iac_ad[1];
                f4_addata[1] = f4_Iac_ad2;
            }
            else
            {
                f4_addata[1] = p_f4_Iac_ad[1];
                f4_addata[2] = f4_Iac_ad2;
            }
        break;

        case CURRENT_PHASE_V_PHASE:
            f4_addata[1] = p_f4_Iac_ad[0];
            if (CURRENT_PHASE_W_PHASE == p_st_cc->mid_phase)
            {
                f4_addata[0] = p_f4_Iac_ad[1];
                f4_addata[2] = f4_Iac_ad2;
            }
            else
            {
                f4_addata[2] = p_f4_Iac_ad[1];
                f4_addata[0] = f4_Iac_ad2;
            }
        break;

        case CURRENT_PHASE_W_PHASE:
            f4_addata[2] = p_f4_Iac_ad[0];
            if (CURRENT_PHASE_U_PHASE == p_st_cc->mid_phase)
            {
                f4_addata[1] = p_f4_Iac_ad[1];
                f4_addata[0] = f4_Iac_ad2;
            }
            else
            {
                f4_addata[0] = p_f4_Iac_ad[1];
                f4_addata[1] = f4_Iac_ad2;
            }
        break;

        default:
            /* Do Nothing */
        break;
    }

    if (MTR_FLG_SET == p_st_cc->u1_flag_offset_calc)
    {
        *p_f4_iu_ad = f4_addata[0];
        *p_f4_iv_ad = f4_addata[1];
        *p_f4_iw_ad = f4_addata[2];
    }
    else
    /* Offset measurement with ADC raw data */
    {
        *p_f4_iu_ad = p_f4_Iac_ad[0];
        *p_f4_iw_ad = p_f4_Iac_ad[1];
    }
} /* End of function R_MOTOR_CURRENT_1ShuntCurrentGet */
#endif

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_CurrentCyclic
* Description   : The cyclic processing of current control
* Arguments     : p_st_cc       - The cyclic processing of current control
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_CurrentCyclic(st_current_control_t *p_st_cc)
{
    if (MTR_FLG_SET == p_st_cc->u1_active)
    {
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
        motor_current_mod_vdc_set(&p_st_cc->st_mod, p_st_cc->f4_vdc_ad);
#endif

        p_st_cc->f4_ref_iq_ctrl = motor_current_reference_iq_set(p_st_cc);
        p_st_cc->f4_ref_id_ctrl = motor_current_reference_id_set(p_st_cc);

        /*=============================================*/
        /*     Coordinate transformation (UVW->dq)     */
        /*=============================================*/
        motor_current_transform_uvw_dq_abs(&p_st_cc->st_rotor_angle, &(p_st_cc->f4_iu_ad), &(p_st_cc->f4_id_ad));

        /*==============================*/
        /*     Angle & speed setting    */
        /*==============================*/
        motor_current_angle_speed_detect(p_st_cc);

        /*====================================*/
        /*     Feedback control (Current)     */
        /*====================================*/
        motor_current_pi_control(p_st_cc);

        /*============================*/
        /*     Decoupling control     */
        /*============================*/
        motor_current_decoupling(p_st_cc, p_st_cc->f4_speed_rad, &(p_st_cc->st_motor));

        /*======================*/
        /* Limit voltage vector */
        /*======================*/
        motor_current_volt_limit(p_st_cc);

        /*=================================================*/
        /*     Coordinate transformation (dq->uvw)         */
        /*=================================================*/
        motor_current_transform_dq_uvw_abs(&p_st_cc->st_rotor_angle, &(p_st_cc->f4_vd_ref), &p_st_cc->f4_refu);

        /*====================================*/
        /*     Voltage error compensation     */
        /*====================================*/
        if ((MTR_FLG_SET == p_st_cc->u1_flag_volt_err_comp_use) && (MTR_FLG_SET == p_st_cc->u1_flag_offset_calc))
        {
            float f4_iuvw_ref[3];
            motor_current_transform_dq_uvw_abs(&p_st_cc->st_rotor_angle, &(p_st_cc->f4_ref_id_ctrl), f4_iuvw_ref);
            motor_current_volt_err_comp_main(&(p_st_cc->st_volt_comp),
                    &p_st_cc->f4_refu, f4_iuvw_ref, p_st_cc->f4_vdc_ad);
        }

        /*==================================*/
        /*     Space vector modulation      */
        /*==================================*/
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
        motor_current_mod_start(&(p_st_cc->st_mod), &p_st_cc->f4_refu, &p_st_cc->f4_modu);
#endif
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
        p_st_cc->u1_flag_offset_calc = MTR_FLG_SET;
        p_st_cc->u2_crnt_offset_cnt  = p_st_cc->u2_offset_calc_time;
        p_st_cc->f4_offset_iu = p_st_cc->f4_sum_iu_ad / p_st_cc->u2_offset_calc_time;
        p_st_cc->f4_offset_iw = p_st_cc->f4_sum_iw_ad / p_st_cc->u2_offset_calc_time;
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
void R_MOTOR_CURRENT_CurrentOffsetRemove(st_current_control_t *p_st_cc, float *p_f4_iu,float *p_f4_iv, float *p_f4_iw)
{
    /* Current offset adjustment */
    *p_f4_iu = (*p_f4_iu) - p_st_cc->f4_offset_iu;
    *p_f4_iv = (*p_f4_iv) - p_st_cc->f4_offset_iv;
    *p_f4_iw = (*p_f4_iw) - p_st_cc->f4_offset_iw;
} /* End of function R_MOTOR_CURRENT_CurrentOffsetRemove */

/**********************************************************************************************************************
 * Function Name: R_MOTOR_CURRENT_VoltErrCompParamSet
 * Description  : Set voltage error compensation parameters
 * Arguments    : p_st_cc -
 *                   The pointer to the current control structure
 *                crnt_array -
 *                   The current array [A]
 *                volterr_array
 *                   The voltage error array [V]
 *                ref_voltage
 *                   The DC bus voltage that table created for
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
    motor_current_bemf_observer_gain_calc(&p_st_cc->st_bemf_observer,
                                          &p_st_cc->st_motor,
                                          p_st_bemf_obs_cfg->f4_e_obs_omega_hz,
                                          p_st_bemf_obs_cfg->f4_e_obs_zeta,
                                          p_st_cc->f4_ctrl_period);

    motor_current_angle_speed_gain_calc(&p_st_cc->st_pll_est,
                                        p_st_bemf_obs_cfg->f4_pll_est_omega_hz,
                                        p_st_bemf_obs_cfg->f4_pll_est_zeta,
                                        p_st_cc->f4_ctrl_period);
} /* End of function R_MOTOR_CURRENT_BEMFObserverParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_HuntingSuppress
* Description   : Preset for suppress speed hunting
* Arguments     : p_st_cc - The pointer to current control structure
*                 f4_ref_speed_rad_ctrl - Command speed value
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_HuntingSuppress(st_current_control_t * p_st_cc, float f4_ref_speed_rad_ctrl)
{
    p_st_cc->st_pll_est.f4_i_est_speed = f4_ref_speed_rad_ctrl * p_st_cc->st_motor.u2_mtr_pp;
} /* End of function R_MOTOR_CURRENT_HuntingSuppress */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_SetMotorID
* Description   : Set the motor ID, ID -> 0 circulation motor, ID -> 1 dry motor
* Arguments     : The motor ID
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_SetMotorID(uint8_t motor_id)
{
	g_st_cc.u1_cc_motor_id = motor_id;
}
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/***********************************************************************************************************************
* Function Name: R_MOTOR_CURRENT_1shuntModulation
* Description  : Perform PWM modulation for 1shunt
* Arguments    : p_st_cc - The pointer to the current control structure
* Return Value : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_1shuntModulation(st_current_control_t *p_st_cc)
{
    float   f4_v_in[3]     = {0.0f};
    float   f4_mod_out[3]  = {0.0f};
    int16_t s2_ref_max1    = 0;
    int16_t s2_ref_max2    = 0;
    int16_t s2_ref_mid1    = 0;
    int16_t s2_ref_mid2    = 0;
    int16_t s2_ref_temp    = 0;
    int16_t s2_raw_InvPeak = 0;
    int16_t s2_ref_max     = 0;
    int16_t s2_ref_mid     = 0;
    int16_t s2_ref_min     = 0;
    int16_t s2_AD1_trigger = 10;
    int16_t s2_AD2_trigger = 10;
    int16_t s2_mu_ref1     = 0;
    int16_t s2_mu_ref2     = 0;
    int16_t s2_mv_ref1     = 0;
    int16_t s2_mv_ref2     = 0;
    int16_t s2_mw_ref1     = 0;
    int16_t s2_mw_ref2     = 0;

    e_mtr_current_phase_t max_phase;
    e_mtr_current_phase_t temp_phase;
    uint16_t              u2_temp_base  = p_st_cc->f4_pwm_period_cnt;
    uint16_t              u2_temp_deadt = p_st_cc->f4_pwm_dead_time_cnt;

    s2_raw_InvPeak = (int16_t) p_st_cc->PWMMaxCountGet();

    motor_current_mod_vdc_set(&p_st_cc->st_mod, p_st_cc->f4_vdc_ad);

    f4_v_in[0] = p_st_cc->f4_refu;
    f4_v_in[1] = p_st_cc->f4_refv;
    f4_v_in[2] = p_st_cc->f4_refw;

    motor_current_mod_start(&(p_st_cc->st_mod), &(f4_v_in[0]), &(f4_mod_out[0]));

    s2_ref_max = (uint16_t) ((u2_temp_base * (1.0f - f4_mod_out[0])) + (u2_temp_deadt * 0.5f));
    s2_ref_mid = (uint16_t) ((u2_temp_base * (1.0f - f4_mod_out[1])) + (u2_temp_deadt * 0.5f));
    s2_ref_min = (uint16_t) ((u2_temp_base * (1.0f - f4_mod_out[2])) + (u2_temp_deadt * 0.5f));

    max_phase          = CURRENT_PHASE_U_PHASE;
    p_st_cc->mid_phase = CURRENT_PHASE_V_PHASE;
    p_st_cc->min_phase = CURRENT_PHASE_W_PHASE;

    /* Judge the phase value */
    if (s2_ref_max < s2_ref_mid)
    {
        s2_ref_temp        = s2_ref_max;
        s2_ref_max         = s2_ref_mid;
        s2_ref_mid         = s2_ref_temp;
        temp_phase         = max_phase;
        max_phase          = p_st_cc->mid_phase;
        p_st_cc->mid_phase = temp_phase;
    }

    if (s2_ref_max < s2_ref_min)
    {
        s2_ref_temp        = s2_ref_max;
        s2_ref_max         = s2_ref_min;
        s2_ref_min         = s2_ref_temp;
        temp_phase         = max_phase;
        max_phase          = p_st_cc->min_phase;
        p_st_cc->min_phase = temp_phase;
    }

    if (s2_ref_mid < s2_ref_min)
    {
        s2_ref_temp        = s2_ref_mid;
        s2_ref_mid         = s2_ref_min;
        s2_ref_min         = s2_ref_temp;
        temp_phase         = p_st_cc->mid_phase;
        p_st_cc->mid_phase = p_st_cc->min_phase;
        p_st_cc->min_phase = temp_phase;
    }

    /* Adjust middle PWM timing */
    if ((s2_ref_mid - s2_ref_min) < p_st_cc->s2_difference_minimum)
    {
        s2_ref_mid1 = s2_ref_min + p_st_cc->s2_difference_minimum;
        if (s2_ref_mid1 > (s2_raw_InvPeak - 1))
        {
            s2_ref_mid1 = s2_raw_InvPeak - 1;
        }

        s2_ref_mid2 = s2_ref_mid - (s2_ref_mid1 - s2_ref_mid);
    }
    else
    {
        s2_ref_mid1 = s2_ref_mid;
        s2_ref_mid2 = s2_ref_mid;
    }

    /* Adjust maximum PWM timing */
    if ((s2_ref_max - s2_ref_mid1) < p_st_cc->s2_difference_minimum)
    {
        s2_ref_max1 = s2_ref_mid1 + p_st_cc->s2_difference_minimum;
        if (s2_ref_max1 > (s2_raw_InvPeak - 1))
        {
            s2_ref_max1 = s2_raw_InvPeak - 1;
        }

        s2_ref_max2 = s2_ref_max - (s2_ref_max1 - s2_ref_max);
    }
    else
    {
        s2_ref_max1 = s2_ref_max;
        s2_ref_max2 = s2_ref_max;
    }

    /* Shift limit */
    if (s2_ref_mid2 < 1)
    {
        s2_ref_mid2 = 1;
    }

    if (s2_ref_max2 < 1)
    {
        s2_ref_max2 = 1;
    }

    /* Set A/D trigger timing */
    s2_AD1_trigger = s2_ref_mid1 - (p_st_cc->s2_difference_minimum - p_st_cc->s2_adjust_adc_delay);
    s2_AD2_trigger = s2_ref_max1 - (p_st_cc->s2_difference_minimum - p_st_cc->s2_adjust_adc_delay);

    switch (p_st_cc->min_phase)
    {
        case CURRENT_PHASE_U_PHASE:
            s2_mu_ref1 = s2_ref_min;
            s2_mu_ref2 = s2_ref_min;
        break;

        case CURRENT_PHASE_V_PHASE:
            s2_mv_ref1 = s2_ref_min;
            s2_mv_ref2 = s2_ref_min;
        break;

        case CURRENT_PHASE_W_PHASE:
            s2_mw_ref1 = s2_ref_min;
            s2_mw_ref2 = s2_ref_min;
        break;

        default:
            /* Do Nothing */
        break;
    }

    switch (p_st_cc->mid_phase)
    {
        case CURRENT_PHASE_U_PHASE:
            s2_mu_ref1 = s2_ref_mid1;
            s2_mu_ref2 = s2_ref_mid2;
        break;

        case CURRENT_PHASE_V_PHASE:
            s2_mv_ref1 = s2_ref_mid1;
            s2_mv_ref2 = s2_ref_mid2;
        break;

        case CURRENT_PHASE_W_PHASE:
            s2_mw_ref1 = s2_ref_mid1;
            s2_mw_ref2 = s2_ref_mid2;
        break;

        default:
            /* Do Nothing */
        break;
    }

    switch (max_phase)
    {
        case CURRENT_PHASE_U_PHASE:
            s2_mu_ref1 = s2_ref_max1;
            s2_mu_ref2 = s2_ref_max2;
        break;

        case CURRENT_PHASE_V_PHASE:
            s2_mv_ref1 = s2_ref_max1;
            s2_mv_ref2 = s2_ref_max2;
        break;

        case CURRENT_PHASE_W_PHASE:
            s2_mw_ref1 = s2_ref_max1;
            s2_mw_ref2 = s2_ref_max2;
        break;

        default:
            /* Do Nothing */
        break;
    }

    p_st_cc->u2_duty_u      = (uint16_t) s2_mu_ref1;
    p_st_cc->u2_duty_v      = (uint16_t) s2_mv_ref1;
    p_st_cc->u2_duty_w      = (uint16_t) s2_mw_ref1;
    p_st_cc->u2_duty_buff_u = (uint16_t) s2_mu_ref2;
    p_st_cc->u2_duty_buff_v = (uint16_t) s2_mv_ref2;
    p_st_cc->u2_duty_buff_w = (uint16_t) s2_mw_ref2;

    p_st_cc->BLDC1ShuntDutySet(p_st_cc->u2_duty_u,
                               p_st_cc->u2_duty_v,
                               p_st_cc->u2_duty_w,
                               p_st_cc->u2_duty_buff_u,
                               p_st_cc->u2_duty_buff_v,
                               p_st_cc->u2_duty_buff_w);

    p_st_cc->ADCTriggerSet((uint16_t) s2_AD1_trigger, (uint16_t) s2_AD2_trigger);
} /* End of function R_MOTOR_CURRENT_1shuntModulation */

/***********************************************************************************************************************
* Function Name : R_MOTOR_CURRENT_DriverParameterUpdate
* Description   : Update the current driver parameter
* Arguments     : p_st_cc                 - The pointer to current control structure
*                 p_st_current_driver_cfg - The pointer to current driver configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_CURRENT_DriverParameterUpdate(st_current_control_t * p_st_cc,
                                           const st_current_driver_cfg_t * p_st_current_driver_cfg)
{
    p_st_cc->BLDC1ShuntDutySet         = p_st_current_driver_cfg->BLDC1ShuntDutySet;
    p_st_cc->ADCTriggerSet             = p_st_current_driver_cfg->ADCTriggerSet;
    p_st_cc->PWMMaxCountGet            = p_st_current_driver_cfg->PWMMaxCountGet;
} /* End of function R_MOTOR_CURRENT_DriverParameterUpdate */
#endif
