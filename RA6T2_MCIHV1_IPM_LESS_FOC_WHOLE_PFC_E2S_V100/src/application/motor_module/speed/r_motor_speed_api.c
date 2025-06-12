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
* File Name   : r_motor_speed_api.c
* Description : Speed control API
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 19.12.2023 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */

/* Main associated header file */
#include "r_motor_speed_api.h"
#include "r_motor_speed.h"
/* low speed sensorless */
#include "r_motor_current_api.h"


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

    p_st_sc->u1_flag_extobserver_use         = SPEED_CFG_OBSERVER; /* low speed sensorless */
    p_st_sc->u1_flag_fluxwkn_use             = SPEED_CFG_FLUX_WEAKENING;
    p_st_sc->u1_flag_mtpa_use                = SPEED_CFG_MTPA;
    p_st_sc->f4_speed_ctrl_period            = SPEED_CFG_CTRL_PERIOD;
    p_st_sc->f4_ref_speed_rad                = 0.0f;
    p_st_sc->f4_ref_speed_rad_manual         = 0.0f;
    p_st_sc->f4_max_speed_rad                = (MOTOR_CFG_MAX_SPEED_RPM * MTR_RPM2RAD);
    p_st_sc->f4_speed_rate_limit_rad         = (SPEED_CFG_CTRL_PERIOD * SPEED_CFG_RATE_LIMIT_RPM * MTR_RPM2RAD);
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

    /* Initialize speed observer structure */
    /* low speed sensorless*/
    motor_speed_extobserver_init(&p_st_sc->st_extobs,
                                 SPEED_CFG_SOB_OMEGA,
                                 p_st_sc->f4_speed_ctrl_period,
                                 p_st_sc->st_motor.f4_mtr_j,
                                 SPEED_CFG_SOB_OUTLIM_START_RPM,
                                 SPEED_CFG_SOB_OUTLIM_END_RPM,
                                 MOTOR_CFG_NOMINAL_CURRENT_RMS );

    /* Initialize Flux-weakening module */
    motor_speed_flux_weakn_init(&p_st_sc->st_fluxwkn, MOTOR_COMMON_LIMIT_IQ,
            INVERTER_CFG_INPUT_V, FLUXWKN_DEF_VFWRATIO, &p_st_sc->st_motor);

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
    p_st_sc->u1_state_estmode         = CURRENT_STATE_ESTMODE_POWEROFF;/* low speed sensorless*/
    p_st_sc->f4_speed_obsrv_rad       = 0.0f; /* low speed sensorless */
    p_st_sc->f4_ref_speed_rad_ctrl    = 0.0f;
    p_st_sc->f4_speed_rad_ctrl        = 0.0f;
    p_st_sc->f4_speed_rad             = 0.0f;
    p_st_sc->f4_id_ref_output         = 0.0f;
    p_st_sc->f4_iq_ref_output         = 0.0f;
    p_st_sc->f4_va_max                = 0.0f;
    p_st_sc->f4_id_ad                 = 0.0f;
    p_st_sc->f4_iq_ad                 = 0.0f;
    p_st_sc->f4_torque_current        = 0.0f;
    /* low speed sensorless*/
    p_st_sc->f4_max_ta                = (MOTOR_CFG_MAGNETIC_FLUX * 0.5f /
                                        (MOTOR_CFG_Q_INDUCTANCE - MOTOR_CFG_D_INDUCTANCE));
    p_st_sc->f4_id_limit              = 0.0f;

    /* PI control structure */
    motor_pi_ctrl_reset(&p_st_sc->st_pi_speed);

    /* speed observer structure */
    /* low speed sensorless */
    motor_speed_extobserver_reset(&p_st_sc->st_extobs);

    /* Flux-weakening structure */
    motor_speed_flux_weakn_reset(&p_st_sc->st_fluxwkn);
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
    float f4_ia_max;

    p_st_sc->u1_flag_fluxwkn_use     = p_st_speed_cfg->u1_flag_fluxwkn_use;
    p_st_sc->u1_flag_extobserver_use = p_st_speed_cfg->u1_flag_extobserver_use; /* low speed sensorless */
    p_st_sc->u1_flag_mtpa_use        = p_st_speed_cfg->u1_flag_mtpa_use;
    p_st_sc->f4_speed_ctrl_period    = p_st_speed_cfg->f4_speed_ctrl_period;
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
    /* extobserver disturbance */
    float f4_torque_nm;
    float f4_ditrub_current = 0.0f;
    if (MTR_FLG_SET == p_st_sc->u1_flag_extobserver_use)
    {
        /* Calculate torque */
        f4_torque_nm = ((float)p_st_sc->st_motor.u2_mtr_pp * p_st_sc->st_motor.f4_mtr_m) * p_st_sc->f4_iq_ref_output;
        /* Extended observer */
        motor_speed_extobserver_start(&p_st_sc->st_extobs, f4_torque_nm, p_st_sc->f4_speed_rad);
        p_st_sc->f4_speed_obsrv_rad = motor_speed_extobserver_mech_speed_get(&p_st_sc->st_extobs);
        p_st_sc->f4_speed_rad_ctrl  = p_st_sc->f4_speed_obsrv_rad;

        /* Torque command for ASR FF */
        f4_ditrub_current = motor_speed_extobserver_disturbance_get(&p_st_sc->st_extobs) /
                            ((float)p_st_sc->st_motor.u2_mtr_pp * p_st_sc->st_motor.f4_mtr_m);
    }
    else
    {
        p_st_sc->f4_speed_rad_ctrl = motor_filter_first_order_lpff(&p_st_sc->st_slpf, p_st_sc->f4_speed_rad);
    }

    if (MTR_FLG_SET == p_st_sc->u1_active)
    {
        /*====================================*/
        /*   Setting of speed command value   */
        /*====================================*/
        p_st_sc->f4_ref_speed_rad_ctrl = motor_speed_ref_speed_set(p_st_sc);

        /* Limited by speed command. Set to 0 on the slope from 25rpm to 30rpm. */
        /* extobserver disturbance */
        f4_ditrub_current = motor_speed_extobserver_disturbance_current_limit(&p_st_sc->st_extobs,
                                                                              p_st_sc->f4_ref_speed_rad_ctrl,
                                                                              f4_ditrub_current);

        /*==============================================*/
        /*   Setting of dq-axis current command value   */
        /*==============================================*/
        /* low speed sensorless (case文に変更する) */
        if ((p_st_sc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_SLOW) ||
            (p_st_sc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_HIGH) ||
            (p_st_sc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_MID) ||
            (p_st_sc->u1_state_estmode == CURRENT_STATE_ESTMODE_DRIVE_MID_M))
        {
            p_st_sc->f4_iq_ref_output =
                motor_speed_pi_control(p_st_sc, p_st_sc->f4_speed_rad_ctrl) + f4_ditrub_current;
        }
        else
        {
            p_st_sc->f4_iq_ref_output =  0;
        }
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
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function R_MOTOR_SPEED_SpeedCyclic */

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
                               uint8_t u1_state_speed_ref
                              )
{
    p_st_sc->u1_state_speed_ref = u1_state_speed_ref;
} /* End of function R_MOTOR_SPEED_RefstateSet */


/***********************************************************************************************************************
* Function Name : R_MOTOR_SPEED_ExtObserverParameterUpdate
* Description   : Update the extended observer parameter
* Arguments     : p_st_sc                 - The pointer to speed observer structure
*                 p_st_ext_obs_cfg        - The pointer to Extended observer configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_SPEED_ExtObserverParameterUpdate(st_speed_control_t * p_st_sc, st_ext_observer_cfg_t * p_st_ext_obs_cfg)
{
    motor_speed_extobserver_inertia_set(&p_st_sc->st_extobs, p_st_sc->st_motor.f4_mtr_j);
    motor_speed_extobserver_natural_freq_set(&p_st_sc->st_extobs,
                                             p_st_ext_obs_cfg->f4_extobs_omega,
                                             p_st_sc->f4_speed_ctrl_period);
    motor_speed_extobserver_current_limit_set(&p_st_sc->st_extobs, p_st_sc->st_motor.f4_nominal_current_rms);
} /* End of function R_MOTOR_SPEED_ExtObserverParameterUpdate */

