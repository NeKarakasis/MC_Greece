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
* File Name   : r_motor_speed_api.c
* Description : Speed control API
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */

/* Main associated header file */
#include "r_motor_speed_api.h"
#include "r_motor_speed.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_speed_control_t      g_st_sc;          /* Speed control structure */

/***********************************************************************************************************************
* Static variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Open
* Description   : Open speed control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Open(void)
{
    st_speed_control_t * p_st_sc;
    p_st_sc = &g_st_sc;

    p_st_sc->u1_flag_fluxwkn_use             = SPEED_CFG_FLUX_WEAKENING;
    p_st_sc->u1_flag_mtpa_use                = SPEED_CFG_MTPA;
    p_st_sc->f4_speed_ctrl_period            = SPEED_CFG_CTRL_PERIOD;
    p_st_sc->f4_ref_speed_rad                = 0.0f;
    p_st_sc->f4_ref_speed_rad_manual         = 0.0f;
    p_st_sc->f4_max_speed_rad                = (MOTOR_CFG_MAX_SPEED_RPM * MTR_RPM2RAD);
    p_st_sc->f4_speed_rate_limit_rad         = (SPEED_CFG_CTRL_PERIOD * SPEED_CFG_RATE_LIMIT_RPM * MTR_RPM2RAD);
    p_st_sc->f4_opl2less_sw_time             = SPEED_OPL2LESS_SWITCH_TIME;
    p_st_sc->st_motor.u2_mtr_pp              = MOTOR_CFG_POLE_PAIRS;
    p_st_sc->st_motor.f4_mtr_r               = MOTOR_CFG_RESISTANCE;
    p_st_sc->st_motor.f4_mtr_ld              = MOTOR_CFG_D_INDUCTANCE;
    p_st_sc->st_motor.f4_mtr_lq              = MOTOR_CFG_Q_INDUCTANCE;
    p_st_sc->st_motor.f4_mtr_m               = MOTOR_CFG_MAGNETIC_FLUX;
    p_st_sc->st_motor.f4_mtr_j               = MOTOR_CFG_ROTOR_INERTIA;
    p_st_sc->st_motor.f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS;

    /* Initialize member of PI control structure */
    motor_pi_ctrl_integral_limit_set(&p_st_sc->st_pi_speed, MOTOR_COMMON_PI_INTEGRAL_LIMIT_IQ);
    motor_speed_pi_gain_calc(p_st_sc, SPEED_CFG_OMEGA, SPEED_CFG_ZETA);

    /* Initialize Flux-weakening module */
    motor_speed_flux_weakn_init(&p_st_sc->st_fluxwkn, MOTOR_COMMON_LIMIT_IQ,
            INVERTER_CFG_INPUT_V, FLUXWKN_DEF_VFWRATIO, &p_st_sc->st_motor);

    /* Initialize open-loop damping control module */
    motor_speed_opl_damp_init(&p_st_sc->st_opl_damp, SPEED_OPL_DAMP_FB_SPEED_LIMIT_RATE);
    motor_speed_opl_damp_r_gain_set(&p_st_sc->st_opl_damp,
                                    p_st_sc->st_motor.u2_mtr_pp,
                                    p_st_sc->st_motor.f4_mtr_m,
                                    p_st_sc->st_motor.f4_mtr_j,
                                    SPEED_OPL_DAMP_ZETA,
                                    SPEED_OPL_DAMP_ED_HPF_OMEGA,
                                    CURRENT_CFG_REF_ID_OPENLOOP,
                                    SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM * MTR_RPM2RAD * MOTOR_CFG_POLE_PAIRS,
                                    SPEED_CFG_CTRL_PERIOD);

    /* Initialize speed LPF */
    motor_filter_first_order_lpff_init(&p_st_sc->st_slpf);
    motor_filter_first_order_lpff_gain_calc(&p_st_sc->st_slpf,
            SPEED_CFG_LPF_OMEGA, SPEED_CFG_CTRL_PERIOD);

    /* Initialize MTPA */
    motor_speed_mtpa_init(&p_st_sc->st_mtpa, &p_st_sc->st_motor);

    R_MOTOR_SPEED_Reset(p_st_sc);
} /* End of function R_MOTOR_SPEED_Open */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Close
* Description   : Close speed control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Close(void)
{
    st_speed_control_t * p_st_sc;
    p_st_sc = &g_st_sc;

    R_MOTOR_SPEED_Reset(p_st_sc);
} /* End of function R_MOTOR_SPEED_Close */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Reset
* Description   : Reset variables when motor speed control
* Arguments     : p_st_sc            - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Reset(st_speed_control_t * p_st_sc)
{
    p_st_sc->u1_active                = MTR_FLG_CLR;
    p_st_sc->u1_state_speed_ref       = SPEED_STATE_ZERO_CONST;
    p_st_sc->u1_flag_switching        = MTR_FLG_CLR;
    p_st_sc->f4_ref_speed_rad_ctrl    = 0.0f;
    p_st_sc->f4_speed_rad_ctrl        = 0.0f;
    p_st_sc->f4_speed_rad             = 0.0f;
    p_st_sc->f4_id_ref_output         = 0.0f;
    p_st_sc->f4_iq_ref_output         = 0.0f;
    p_st_sc->f4_va_max                = 0.0f;
    p_st_sc->f4_id_ad                 = 0.0f;
    p_st_sc->f4_iq_ad                 = 0.0f;
    p_st_sc->f4_torque_current        = 0.0f;

    /* PI control structure */
    motor_pi_ctrl_reset(&p_st_sc->st_pi_speed);

    /* Flux-weakening structure */
    motor_speed_flux_weakn_reset(&p_st_sc->st_fluxwkn);

    /* Open loop damping control structure */
    motor_speed_opl_damp_reset(&p_st_sc->st_opl_damp);

    /* speed LPF */
    motor_filter_first_order_lpff_reset(&p_st_sc->st_slpf);
} /* End of function R_MOTOR_SPEED_Reset */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Run
* Description   : Run the motor speed control
* Arguments     : p_st_sc            - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Run(st_speed_control_t * p_st_sc)
{
    p_st_sc->u1_active = MTR_FLG_SET;
} /* End of function R_MOTOR_SPEED_Run */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterSet
* Description   : Set the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_input   - The pointer to speed control input parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterSet(st_speed_control_t * p_st_sc, const st_speed_input_t * p_st_speed_input)
{
    p_st_sc->u1_state_speed_ref = p_st_speed_input->u1_state_speed_ref;
    p_st_sc->f4_speed_rad       = p_st_speed_input->f4_speed_rad;
    p_st_sc->f4_va_max          = p_st_speed_input->f4_va_max;
    p_st_sc->u1_state_id_ref    = p_st_speed_input->u1_state_id_ref;
    p_st_sc->u1_state_iq_ref    = p_st_speed_input->u1_state_iq_ref;
} /* End of function R_MOTOR_SPEED_ParameterSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterGet
* Description   : Get the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_output  - The pointer to speed control output parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterGet(st_speed_control_t * p_st_sc, st_speed_output_t * p_st_speed_output)
{
    p_st_speed_output->f4_id_ref             = p_st_sc->f4_id_ref_output;
    p_st_speed_output->f4_iq_ref             = p_st_sc->f4_iq_ref_output;
    p_st_speed_output->f4_ref_speed_rad_ctrl = p_st_sc->f4_ref_speed_rad_ctrl;
    p_st_speed_output->f4_speed_rad_lpf      = p_st_sc->f4_speed_rad_ctrl;
} /* End of function R_MOTOR_SPEED_ParameterGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ParameterUpdate
* Description   : Update the speed control parameter
* Arguments     : p_st_sc            - The pointer to speed control structure
*                 p_st_speed_cfg     - The pointer to speed control configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ParameterUpdate(st_speed_control_t * p_st_sc, const st_speed_cfg_t * p_st_speed_cfg)
{
    float f4_id_down_speed_rad;
    float f4_ia_max;

    p_st_sc->u1_flag_fluxwkn_use     = p_st_speed_cfg->u1_flag_fluxwkn_use;
    p_st_sc->u1_flag_mtpa_use        = p_st_speed_cfg->u1_flag_mtpa_use;
    p_st_sc->f4_speed_ctrl_period    = p_st_speed_cfg->f4_speed_ctrl_period;
    p_st_sc->f4_opl2less_sw_time     = p_st_speed_cfg->f4_opl2less_sw_time;
    p_st_sc->st_motor                = p_st_speed_cfg->st_motor;

    /* Update speed PI gain */
    motor_speed_pi_gain_calc(p_st_sc, p_st_speed_cfg->f4_speed_omega_hz, p_st_speed_cfg->f4_speed_zeta);

    /* Setup max speed */
    motor_speed_max_speed_set(p_st_sc, p_st_speed_cfg->f4_max_speed_rpm);

    /* Setup limit of speed change */
    motor_speed_rate_limit_set(p_st_sc, p_st_speed_cfg->f4_speed_rate_limit_rpm);

    /* Re-calculate gains for LPF */
    motor_filter_first_order_lpff_gain_calc(&p_st_sc->st_slpf,
                                            p_st_speed_cfg->f4_speed_lpf_hz,
                                            p_st_sc->f4_speed_ctrl_period);

    /* Setup the motor parameter to flux-weakening structure */
    motor_speed_flux_weakn_motor_set(&p_st_sc->st_fluxwkn, &p_st_sc->st_motor);

    f4_ia_max = p_st_sc->st_motor.f4_nominal_current_rms * MOTOR_COMMON_CFG_IA_MAX_CALC_MULT;
    p_st_sc->st_pi_speed.f4_ilimit = f4_ia_max;
    motor_speed_flux_weakn_iamax_set(&p_st_sc->st_fluxwkn, f4_ia_max);

    /* Open-loop damping control gain */
    f4_id_down_speed_rad = p_st_speed_cfg->f4_id_down_speed_rpm * MTR_RPM2RAD;
    motor_speed_opl_damp_r_gain_set(&p_st_sc->st_opl_damp,
                                    p_st_sc->st_motor.u2_mtr_pp,
                                    p_st_sc->st_motor.f4_mtr_m,
                                    p_st_sc->st_motor.f4_mtr_j,
                                    p_st_speed_cfg->f4_ol_damping_zeta,
                                    p_st_speed_cfg->f4_ed_hpf_omega,
                                    p_st_speed_cfg->f4_ol_ref_id,
                                    f4_id_down_speed_rad * p_st_sc->st_motor.u2_mtr_pp,
                                    p_st_sc->f4_speed_ctrl_period);

    /* Rate[krpm/s] of reference speed for open-loop damping control feedback speed limiter */
    motor_speed_opl_damp_limit_set(&p_st_sc->st_opl_damp, p_st_speed_cfg->f4_ol_damping_fb_limit_rate);

    /* Setup the motor parameter to MTPA structure */
    motor_speed_mtpa_motor_param_set(&p_st_sc->st_mtpa, &p_st_sc->st_motor);
} /* End of function R_MOTOR_SPEED_ParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SpdRefSet
* Description   : Sets parameter for reference speed
* Arguments     : p_st_sc             - The pointer to speed control structure
*                 f4_ref_speed_rpm    - reference speed [rpm]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SpdRefSet(st_speed_control_t * p_st_sc, float f4_ref_speed_rpm)
{
    /* [rpm]->[rad/s] */
    p_st_sc->f4_ref_speed_rad_manual = f4_ref_speed_rpm * MTR_RPM2RAD;
} /* End of function R_MOTOR_SPEED_SpdRefSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SpeedCyclic
* Description   : The cyclic processing of speed control
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SpeedCyclic(st_speed_control_t * p_st_sc)
{
    p_st_sc->f4_speed_rad_ctrl = motor_filter_first_order_lpff(&p_st_sc->st_slpf, p_st_sc->f4_speed_rad);

    if (MTR_FLG_SET == p_st_sc->u1_active)
    {
        /*====================================*/
        /*   Setting of speed command value   */
        /*====================================*/
        p_st_sc->f4_ref_speed_rad_ctrl = motor_speed_ref_speed_set(p_st_sc);

        /*==============================================*/
        /*   Setting of dq-axis current command value   */
        /*==============================================*/

        switch (p_st_sc->u1_state_iq_ref)
        {
        case CURRENT_SPD_STATE_IQ_ZERO_CONST:
        case CURRENT_SPD_STATE_IQ_AUTO_ADJ:
        case CURRENT_SPD_STATE_IQ_DOWN:
            /* Do Nothing */
        break;

        case CURRENT_SPD_STATE_IQ_SPEED_PI_OUTPUT:
            p_st_sc->f4_iq_ref_output = motor_speed_pi_control(p_st_sc, p_st_sc->f4_speed_rad_ctrl);

            /*=================================*/
            /*   Executes the Flux-weakening   */
            /*=================================*/
            if (MTR_FLG_SET == p_st_sc->u1_flag_fluxwkn_use)
            {
                motor_speed_flux_weakening(p_st_sc);
            }
            else
            {
                /* Do Nothing */
            }

            if (MTR_FLG_SET == p_st_sc->u1_flag_mtpa_use)
            {
                /* This function will over-write the dq-axis current command */
                motor_speed_mtpa(p_st_sc);
            }
            else
            {
                /* Do nothing */
            }
        break;

        default:
            /* Do Nothing */
        break;
        }
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function R_MOTOR_SPEED_SpeedCyclic */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Opl2lessReferenceIqCalc
* Description   : Set Iq reference when sensor-less switch control
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_ed - d-axis BEMF
*                 f4_eq - q-axis BEMF
*                 f4_id_ref - d-axis current reference
*                 f4_phase_err - phase error between the real motor axis and the controlled axis
* Return Value  : Iq reference
***********************************************************************************************************************/
float R_MOTOR_SPEED_Opl2lessReferenceIqCalc(st_speed_control_t * p_st_sc,
                                            float f4_ed,
                                            float f4_eq,
                                            float f4_id_ref,
                                            float f4_phase_err)
{
    float f4_temp_iq_ref;

    f4_temp_iq_ref = motor_speed_opl2less_iq_calc(f4_ed,
                                                  f4_eq,
                                                  f4_id_ref,
                                                  p_st_sc->f4_torque_current,
                                                  f4_phase_err);

    return (f4_temp_iq_ref);
} /* End of function R_MOTOR_SPEED_Opl2lessReferenceIqCalc */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_Opl2lessPreprocess
* Description   : Preparations before switching from open-loop to normal FOC
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_id_ref - The Id reference in open-loop drive mode
*                 f4_phase_err_rad_lpf - The phase error [rad] processed by an LPF
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_Opl2lessPreprocess(st_speed_control_t * p_st_sc,
                                      float f4_id_ref,
                                      float f4_phase_err_rad_lpf)
{
    p_st_sc->f4_torque_current = motor_speed_opl2less_torque_current_calc(&p_st_sc->st_motor,
                                                                          p_st_sc->f4_opl2less_sw_time,
                                                                          f4_id_ref,
                                                                          f4_phase_err_rad_lpf);
} /* End of function R_MOTOR_SPEED_Opl2lessPreprocess */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_OplDampCtrl
* Description   : Open-loop damping control
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_ed - d-axis BEMF
* Return Value  : Feedback value for reference speed
***********************************************************************************************************************/
float R_MOTOR_SPEED_OplDampCtrl(st_speed_control_t * p_st_sc, float f4_ed)
{
    float f4_damp_comp_speed;
    float f4_ref_speed_elec;

    f4_ref_speed_elec = p_st_sc->f4_ref_speed_rad_ctrl * p_st_sc->st_motor.u2_mtr_pp;
    f4_damp_comp_speed = motor_speed_opl_damp_ctrl(&p_st_sc->st_opl_damp,
                                                   f4_ed,
                                                   f4_ref_speed_elec);
    f4_damp_comp_speed /= p_st_sc->st_motor.u2_mtr_pp;

    return (f4_damp_comp_speed);
} /* End of function R_MOTOR_SPEED_OplDampCtrl */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_OplDampReset
* Description   : Resets open-loop damping control (except for gains and limiters)
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_OplDampReset(st_speed_control_t * p_st_sc)
{
    motor_speed_opl_damp_reset(&p_st_sc->st_opl_damp);
} /* End of function R_MOTOR_SPEED_OplDampReset */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_HuntingSuppress
* Description   : Preset for suppress speed hunting
* Arguments     : p_st_sc - The pointer to speed control structure
*                 f4_iq_ref - The reference q-axis current value [A]
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_HuntingSuppress(st_speed_control_t * p_st_sc, float f4_iq_ref)
{
    p_st_sc->st_pi_speed.f4_refi = f4_iq_ref;
} /* End of function R_MOTOR_SPEED_HuntingSuppress */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_SwitchingFlagSet
* Description   : Set the flag to set the estimated speed
* Arguments     : p_st_sc - The pointer to speed control structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_SwitchingFlagSet(st_speed_control_t * p_st_sc)
{
    p_st_sc->u1_flag_switching = MTR_FLG_SET;
} /* End of function R_MOTOR_SPEED_SwitchingFlagSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ControlParamSet
* Description   : Preset the initial speed 
* Arguments     : p_st_sc - The pointer to speed control structure
                : f4_speed_rad_ctrl - mechanical angle velocity (rad/sec)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ControlParamSet(st_speed_control_t * p_st_sc, float f4_speed_rad_ctrl)
{
    float f4_temp_speed;

    f4_temp_speed = f4_speed_rad_ctrl;

    p_st_sc->f4_ref_speed_rad_ctrl = f4_temp_speed;
    p_st_sc->f4_speed_rad_ctrl     = f4_temp_speed;
    p_st_sc->f4_speed_rad          = f4_temp_speed;
    p_st_sc->st_slpf.f4_pre_input  = f4_temp_speed;
    p_st_sc->st_slpf.f4_pre_output = f4_temp_speed;
} /* End of function R_MOTOR_SPEED_ControlParamSet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_RefstateSet
* Description   : Presets the speed control command status
* Arguments     : p_st_sc - The pointer to speed control structure
*               : u1_state_speed_ref - The speed command status
*               : u1_state_id_ref - The d-axis current command state
*               : u1_state_iq_ref - The q-axis current command state
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_RefstateSet(st_speed_control_t * p_st_sc,
                               uint8_t u1_state_speed_ref,
                               uint8_t u1_state_id_ref,
                               uint8_t u1_state_iq_ref)
{
    p_st_sc->u1_state_id_ref    = u1_state_id_ref;
    p_st_sc->u1_state_iq_ref    = u1_state_iq_ref;
    p_st_sc->u1_state_speed_ref = u1_state_speed_ref;
} /* End of function R_MOTOR_SPEED_RefstateSet */
