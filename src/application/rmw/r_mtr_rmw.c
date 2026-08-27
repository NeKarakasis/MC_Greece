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
* File Name   : r_mtr_rmw.c
* Description : Processes of a user interface (tool)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 19.01.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "mtr_main.h"
#include "r_mtr_rmw.h"
#include "hal_data.h"
#include "r_mtr_motor_parameter.h"
#include "r_mtr_control_parameter.h"
static uint8_t u1_cnt_ics;                  /* Counter for period of calling "scope_watchpoint" */

static float rm_motor_speed_get_torque_limit_mnm(
    motor_speed_instance_ctrl_t * p_ctrl);


float    g_f4_id_ref_monitor;               /* The reference d-axis current value [A] */
float    g_f4_id_ad_monitor;                /* The d-axis current value [A] */
float    g_f4_iq_ref_monitor;               /* The reference q-axis current value [A] */
float    g_f4_iq_ad_monitor;                /* The q-axis current value [A] */
float    g_f4_iu_ad_monitor;                /* U-phase current value [A] */
float    g_f4_iv_ad_monitor;                /* V-phase current value [A] */
float    g_f4_iw_ad_monitor;                /* W-phase current value [A] */
float    g_f4_vdc_ad_monitor;               /* Main Line Voltage[V] */
float    g_f4_vd_ref_monitor;               /* The reference d-axis voltage value [V] */
float    g_f4_vq_ref_monitor;               /* The reference q-axis voltage value [V] */
float    g_f4_refu_monitor;                 /* U-phase reference voltage value [V] */
float    g_f4_refv_monitor;                 /* V-phase reference voltage value [V] */
float    g_f4_refw_monitor;                 /* W-phase reference voltage value [V] */
float    g_f4_ed_monitor;                   /* Estimated d-axis component[V] of flux due to the permanent magnet */
float    g_f4_eq_monitor;                   /* Estimated q-axis component[V] of flux due to the permanent magnet */
float    g_f4_phase_err_monitor;            /* Estimated phase error[rad] from actual rotor electrical angle */
float    g_f4_angle_rad_monitor;            /* Angle of rotor [rad] */
float    g_f4_speed_est_monitor;            /* Estimated speed [rad/s] */
float    g_f4_speed_ref_monitor;            /* command speed value for speed PI control [rad/s] */
uint8_t  g_u1_state_id_ref_monitor;         /* The d-axis current command status */
uint8_t  g_u1_state_iq_ref_monitor;         /* The q-axis current command status */
uint8_t  g_u1_state_speed_ref_monitor;      /* The speed command status */
float    g_f4_speed_kp_monitor;             /* Kp for speed loop */
float    g_f4_speed_ki_monitor;             /* Ki for speed loop */
float    g_f4_current_kp_d_monitor;         /* Kp for d-axis current loop */
float    g_f4_current_ki_d_monitor;         /* Ki for d-axis current loop */
float    g_f4_current_kp_q_monitor;         /* Kp for q-axis current loop */
float    g_f4_current_ki_q_monitor;         /* Ki for q-axis current loop */
float    g_f4_k_e_obs_1_d_monitor;          /* BEMF observer gain 1 for d-axis */
float    g_f4_k_e_obs_2_d_monitor;          /* BEMF observer gain 2 for d-axis */
float    g_f4_k_e_obs_1_q_monitor;          /* BEMF observer gain 1 for q-axis */
float    g_f4_k_e_obs_2_q_monitor;          /* BEMF observer gain 2 for q-axis */
float    g_f4_kp_est_speed_monitor;         /* The proportional gain for PLL */
float    g_f4_ki_est_speed_monitor;         /* The integral gain for PLL*/
float    g_f4_speed_rpm_monitor;            /* Rotation speed [rpm] */

float    g_f4_id_ref_monitor1;              /* The reference d-axis current value [A] */
float    g_f4_id_ad_monitor1;               /* The d-axis current value [A] */
float    g_f4_iq_ref_monitor1;              /* The reference q-axis current value [A] */
float    g_f4_iq_ad_monitor1;               /* The q-axis current value [A] */
float    g_f4_iu_ad_monitor1;               /* U-phase current value [A] */
float    g_f4_iv_ad_monitor1;               /* V-phase current value [A] */
float    g_f4_iw_ad_monitor1;               /* W-phase current value [A] */
float    g_f4_vdc_ad_monitor1;              /* Main Line Voltage[V] */
float    g_f4_vd_ref_monitor1;              /* The reference d-axis voltage value [V] */
float    g_f4_vq_ref_monitor1;              /* The reference q-axis voltage value [V] */
float    g_f4_refu_monitor1;                /* U-phase reference voltage value [V] */
float    g_f4_refv_monitor1;                /* V-phase reference voltage value [V] */
float    g_f4_refw_monitor1;                /* W-phase reference voltage value [V] */
float    g_f4_ed_monitor1;                  /* Estimated d-axis component[V] of flux due to the permanent magnet */
float    g_f4_eq_monitor1;                  /* Estimated q-axis component[V] of flux due to the permanent magnet */
float    g_f4_phase_err_monitor1;           /* Estimated phase error[rad] from actual rotor electrical angle */
float    g_f4_angle_rad_monitor1;           /* Angle of rotor [rad] */
float    g_f4_speed_est_monitor1;           /* Estimated speed [rad/s] */
float    g_f4_speed_ref_monitor1;           /* command speed value for speed PI control [rad/s] */
uint8_t  g_u1_state_id_ref_monitor1;        /* The d-axis current command status */
uint8_t  g_u1_state_iq_ref_monitor1;        /* The q-axis current command status */
uint8_t  g_u1_state_speed_ref_monitor1;     /* The speed command status */
float    g_f4_speed_kp_monitor1;            /* Kp for speed loop */
float    g_f4_speed_ki_monitor1;            /* Ki for speed loop */
float    g_f4_current_kp_d_monitor1;        /* Kp for d-axis current loop */
float    g_f4_current_ki_d_monitor1;        /* Ki for d-axis current loop */
float    g_f4_current_kp_q_monitor1;        /* Kp for q-axis current loop */
float    g_f4_current_ki_q_monitor1;        /* Ki for q-axis current loop */
float    g_f4_k_e_obs_1_d_monitor1;         /* BEMF observer gain 1 for d-axis */
float    g_f4_k_e_obs_2_d_monitor1;         /* BEMF observer gain 2 for d-axis */
float    g_f4_k_e_obs_1_q_monitor1;         /* BEMF observer gain 1 for q-axis */
float    g_f4_k_e_obs_2_q_monitor1;         /* BEMF observer gain 2 for q-axis */
float    g_f4_kp_est_speed_monitor1;        /* The proportional gain for PLL */
float    g_f4_ki_est_speed_monitor1;        /* The integral gain for PLL*/
float    g_f4_speed_rpm_monitor1;           /* Rotation speed [rpm] */
float g_f4_max_torque_mnm  = 0.0F;
float g_f4_min_torque_mnm  = 0.0F;

float g_f4_max_torque_mnm1 = 0.0F;
float g_f4_min_torque_mnm1 = 0.0F;

float    com_f4_ref_speed_rpm = 0.0F;       /* Motor speed reference [rpm] (mechanical) */
float com_f4_torque_ref_mnm  = 0.0F;        /* Motor torque reference [mnm] (mechanical) */
float com_f4_torque_ref_mnm1 = 0.0F;
uint8_t com_u1_control_mode  = MOTOR_SPEED_CONTROL_MODE_SPEED; /* the control mode for motor 0 */
uint8_t com_u1_control_mode1 = MOTOR_SPEED_CONTROL_MODE_SPEED;
float com_f4_max_torque_mnm  = 0.0F;        /* The maximum torque limit based of the motor parameters*/
float com_f4_min_torque_mnm  = 0.0F;
float com_f4_max_torque_mnm1 = 0.0F;         /* The minimum (negative) torque limit based of the motor parameters*/
float com_f4_min_torque_mnm1 = 0.0F;
uint8_t com_u1_speed_mode =    1;
uint8_t com_u1_torque_mode =   0;
uint8_t com_u1_speed_mode1 =   1;
uint8_t com_u1_torque_mode1 =  0;

uint16_t com_u2_mtr_pp;                     /* Pole pairs */
float    com_f4_mtr_r;                      /* Resistance [ohm] */
float    com_f4_mtr_ld;                     /* D-axis inductance [H] */
float    com_f4_mtr_lq;                     /* Q-axis inductance [H] */
float    com_f4_mtr_m;                      /* Permanent magnetic flux [Wb] */
float    com_f4_mtr_j;                      /* Rotor inertia [kgm^2] */
float    com_f4_current_omega;              /* Natural frequency for current loop [Hz] */
float    com_f4_current_zeta;               /* Damping ratio for current loop */
float    com_f4_speed_omega;                /* Natural frequency for speed loop [Hz] */
float    com_f4_speed_zeta;                 /* Damping ratio for speed loop */
float    com_f4_e_obs_omega;                /* Natural frequency for BEMF observer [Hz] */
float    com_f4_e_obs_zeta;                 /* Damping ratio for BEMF observer */
float    com_f4_pll_est_omega;              /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
float    com_f4_pll_est_zeta;               /* Damping ratio for rotor position Phase-Locked Loop */
float    com_f4_ref_id;                     /* Id reference when open loop [A] */
float    com_f4_ol_id_up_step;              /* The d-axis current reference ramping up rate */
float    com_f4_ol_id_down_step;            /* The d-axis current reference ramping down rate */
float    com_f4_id_down_speed_rpm;          /* The speed threshold to ramp down the d-axis current */
float    com_f4_id_up_speed_rpm;            /* The speed threshold to ramp up the d-axis current */
float    com_f4_max_speed_rpm;              /* Maximum speed */
float    com_f4_overspeed_limit_rpm;        /* Over speed limit */
float    com_f4_overcurrent_limit;          /* Over current limit */
float    com_f4_iq_limit;                   /* Q-axis current limit */
float    com_f4_limit_speed_change;         /* Limit of speed change */
float    com_f4_nominal_current;            /* Nominal current */

float    com_f4_ref_speed_rpm1 = 0.0F;      /* Motor speed reference [rpm] (mechanical) */
uint16_t com_u2_mtr_pp1;                    /* Pole pairs */
float    com_f4_mtr_r1;                     /* Resistance [ohm] */
float    com_f4_mtr_ld1;                    /* D-axis inductance [H] */
float    com_f4_mtr_lq1;                    /* Q-axis inductance [H] */
float    com_f4_mtr_m1;                     /* Permanent magnetic flux [Wb] */
float    com_f4_mtr_j1;                     /* Rotor inertia [kgm^2] */
float    com_f4_current_omega1;             /* Natural frequency for current loop [Hz] */
float    com_f4_current_zeta1;              /* Damping ratio for current loop */
float    com_f4_speed_omega1;               /* Natural frequency for speed loop [Hz] */
float    com_f4_speed_zeta1;                /* Damping ratio for speed loop */
float    com_f4_e_obs_omega1;               /* Natural frequency for BEMF observer [Hz] */
float    com_f4_e_obs_zeta1;                /* Damping ratio for BEMF observer */
float    com_f4_pll_est_omega1;             /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
float    com_f4_pll_est_zeta1;              /* Damping ratio for rotor position Phase-Locked Loop */
float    com_f4_ref_id1;                    /* Id reference when open loop [A] */
float    com_f4_ol_id_up_step1;             /* The d-axis current reference ramping up rate */
float    com_f4_ol_id_down_step1;           /* The d-axis current reference ramping down rate */
float    com_f4_id_down_speed_rpm1;         /* The speed threshold to ramp down the d-axis current */
float    com_f4_id_up_speed_rpm1;           /* The speed threshold to ramp up the d-axis current */
float    com_f4_max_speed_rpm1;             /* Maximum speed */
float    com_f4_overspeed_limit_rpm1;       /* Over speed limit */
float    com_f4_overcurrent_limit1;         /* Over current limit */
float    com_f4_iq_limit1;                  /* Q-axis current limit */
float    com_f4_limit_speed_change1;        /* Limit of speed change */
float    com_f4_nominal_current1;           /* Nominal current */

uint8_t  com_u1_enable_write = 0U;          /* ICS write enable flag */
uint8_t  g_u1_enable_write = 0U;            /* ICS write enable flag */

extern   const motor_instance_t  *p_motor_instance;
extern   motor_cfg_t g_user_motor_cfg;
extern   motor_sensorless_extended_cfg_t g_user_motor_sensorless_extended_cfg;
extern   motor_speed_cfg_t g_user_motor_speed_cfg;
extern   motor_speed_extended_cfg_t g_user_motor_speed_extended_cfg;
extern   motor_current_cfg_t g_user_motor_current_cfg;
extern   motor_current_extended_cfg_t g_user_motor_current_extended_cfg;
extern   motor_angle_cfg_t g_user_motor_angle_cfg;
extern   motor_estimate_extended_cfg_t g_user_motor_estimate_extended_cfg;
extern   uint16_t g_u2_max_speed_rpm;

extern   const motor_instance_t  *p_motor1_instance;
extern   motor_cfg_t g_user_motor1_cfg;
extern   motor_sensorless_extended_cfg_t g_user_motor_sensorless1_extended_cfg;
extern   motor_speed_cfg_t g_user_motor_speed1_cfg;
extern   motor_speed_extended_cfg_t g_user_motor_speed1_extended_cfg;
extern   motor_current_cfg_t g_user_motor_current1_cfg;
extern   motor_current_extended_cfg_t g_user_motor_current1_extended_cfg;
extern   motor_angle_cfg_t g_user_motor_angle1_cfg;
extern   motor_estimate_extended_cfg_t g_user_motor_estimate1_extended_cfg;
extern   uint16_t g_u2_max_speed_rpm1;

static   motor_speed_instance_t   const *p_speed_instance;
static   motor_speed_instance_t   const *p_speed1_instance;
static   motor_current_instance_t const *p_current_instance;
static   motor_current_instance_t const *p_current1_instance;
static   motor_angle_instance_t   const *p_angle_instance;
static   motor_angle_instance_t   const *p_angle1_instance;

static   uint8_t  g_u1_trig_enable_write = MTR_FLG_CLR;

/***********************************************************************************************************************
* Function Name : mtr_set_com_variables
* Description   : Set com variables
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_set_com_variables(void)
{
    motor_estimate_instance_ctrl_t *p_angle_ctrl = (motor_estimate_instance_ctrl_t *)p_angle_instance->p_ctrl;

    if (com_u1_enable_write == g_u1_enable_write)
    {
        /* Limit speed reference and set */
        if (com_f4_ref_speed_rpm > (float)g_u2_max_speed_rpm)
        {
            com_f4_ref_speed_rpm = (float)g_u2_max_speed_rpm;
        }
        else if (com_f4_ref_speed_rpm < -(float)g_u2_max_speed_rpm)
        {
            com_f4_ref_speed_rpm = -(float)g_u2_max_speed_rpm;
        }
        p_motor_instance->p_api->speedSet(p_motor_instance->p_ctrl, com_f4_ref_speed_rpm);

        /* Validate control mode */
        if (com_u1_control_mode > (uint8_t) MOTOR_SPEED_CONTROL_MODE_TORQUE)
        {
            com_u1_control_mode = (uint8_t) MOTOR_SPEED_CONTROL_MODE_SPEED;
            com_u1_speed_mode1 = 0;
            com_u1_torque_mode1 = 1;

        }
        /* Calculate torque limits */
        g_f4_max_torque_mnm = (float) com_u2_mtr_pp * com_f4_mtr_m * (com_f4_iq_limit * MTR_SQRT_3) * MTR_TORQUE_LIMIT_MARGIN * 1000.0F;

        g_f4_min_torque_mnm = -g_f4_max_torque_mnm;

        /* Limit torque reference */
        if (com_f4_torque_ref_mnm > g_f4_max_torque_mnm)
        {
            com_f4_torque_ref_mnm = g_f4_max_torque_mnm;
        }
        else if (com_f4_torque_ref_mnm < g_f4_min_torque_mnm)
        {
            com_f4_torque_ref_mnm = g_f4_min_torque_mnm;
        }

        /* Set torque command motor 0 */
        p_speed_instance->p_api->torqueReferenceSet(p_speed_instance->p_ctrl, com_f4_torque_ref_mnm);

        /* Set mode last */
        if (com_u1_control_mode == MOTOR_SPEED_CONTROL_MODE_TORQUE)
        {
            com_u1_speed_mode = 0;
            com_u1_torque_mode = 1;
        }
        else
        {
            com_u1_speed_mode = 1;
            com_u1_torque_mode = 0;
        }
        p_speed_instance->p_api->controlModeSet( p_speed_instance->p_ctrl, (motor_speed_control_mode_t) com_u1_control_mode);

        if (com_f4_ref_speed_rpm1 > (float)g_u2_max_speed_rpm1)
        {
            com_f4_ref_speed_rpm1 = (float)g_u2_max_speed_rpm1;
        }
        else if (com_f4_ref_speed_rpm1 < -(float)g_u2_max_speed_rpm1)
        {
            com_f4_ref_speed_rpm1 = -(float)g_u2_max_speed_rpm1;
        }
        p_motor1_instance->p_api->speedSet(p_motor1_instance->p_ctrl, com_f4_ref_speed_rpm1);


        /* Set torque command motor 1 */

        if (com_u1_control_mode1 > (uint8_t) MOTOR_SPEED_CONTROL_MODE_TORQUE)
        {
            com_u1_control_mode1 = (uint8_t) MOTOR_SPEED_CONTROL_MODE_SPEED;
            com_u1_speed_mode1 = 0;
            com_u1_torque_mode1 = 1;
        }


        com_f4_max_torque_mnm1 = (float) com_u2_mtr_pp1 * com_f4_mtr_m1 * (com_f4_iq_limit1 * MTR_SQRT_3) * MTR_TORQUE_LIMIT_MARGIN * 1000.0F;

        com_f4_min_torque_mnm1 = -com_f4_max_torque_mnm1;


        if (com_f4_torque_ref_mnm1 > com_f4_max_torque_mnm1)
        {
            com_f4_torque_ref_mnm1 = com_f4_max_torque_mnm1;
        }
        else if (com_f4_torque_ref_mnm1 < com_f4_min_torque_mnm1)
        {
            com_f4_torque_ref_mnm1 = com_f4_min_torque_mnm1;
        }
        p_speed1_instance->p_api->torqueReferenceSet( p_speed1_instance->p_ctrl, com_f4_torque_ref_mnm1);
        if (com_u1_control_mode1 == MOTOR_SPEED_CONTROL_MODE_TORQUE)
        {
            com_u1_speed_mode1 = 0;
            com_u1_torque_mode1 = 1;
        }
        else
        {
            com_u1_speed_mode1 = 1;
            com_u1_torque_mode1 = 0;
        }
        p_speed1_instance->p_api->controlModeSet(p_speed1_instance->p_ctrl,(motor_speed_control_mode_t) com_u1_control_mode1);

        /* 1st motor */
        g_user_motor_sensorless_extended_cfg.f_overspeed_limit   = com_f4_overspeed_limit_rpm;
        g_user_motor_sensorless_extended_cfg.f_overcurrent_limit
            = com_f4_overcurrent_limit * MTR_SQRT_2 * MTR_OVERCURRENT_MARGIN_MULT;

        g_user_motor_speed_extended_cfg.f_limit_speed_change          = com_f4_limit_speed_change * com_u2_mtr_pp;
        g_user_motor_speed_extended_cfg.f_iq_limit                    = com_f4_iq_limit;
        g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_ref         = com_f4_ref_id;
        g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_up_step     = com_f4_ol_id_up_step * com_f4_ref_id;
        g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_down_step   = com_f4_ol_id_down_step * com_f4_ref_id;
        g_user_motor_speed_extended_cfg.ol_param.f4_id_down_speed_rpm = com_f4_id_down_speed_rpm * com_u2_mtr_pp;
        g_user_motor_speed_extended_cfg.ol_param.f4_id_up_speed_rpm   = com_f4_id_up_speed_rpm * com_u2_mtr_pp;
        g_user_motor_speed_extended_cfg.f_maximum_speed_rpm           = com_f4_max_speed_rpm * com_u2_mtr_pp;
        g_user_motor_speed_extended_cfg.d_param.f_speed_omega         = com_f4_speed_omega;
        g_user_motor_speed_extended_cfg.d_param.f_speed_zeta          = com_f4_speed_zeta;
        g_user_motor_speed_extended_cfg.mtr_param.u2_mtr_pp           = com_u2_mtr_pp;
        g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_r            = com_f4_mtr_r;
        g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_ld           = com_f4_mtr_ld;
        g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_lq           = com_f4_mtr_lq;
        g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_m            = com_f4_mtr_m;
        g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_j            = com_f4_mtr_j;

        g_user_motor_current_extended_cfg.p_motor_parameter->u2_mtr_pp        = com_u2_mtr_pp;
        g_user_motor_current_extended_cfg.p_motor_parameter->f4_mtr_r         = com_f4_mtr_r;
        g_user_motor_current_extended_cfg.p_motor_parameter->f4_mtr_ld        = com_f4_mtr_ld;
        g_user_motor_current_extended_cfg.p_motor_parameter->f4_mtr_lq        = com_f4_mtr_lq;
        g_user_motor_current_extended_cfg.p_motor_parameter->f4_mtr_m         = com_f4_mtr_m;
        g_user_motor_current_extended_cfg.p_motor_parameter->f4_mtr_j         = com_f4_mtr_j;
        g_user_motor_current_extended_cfg.p_design_parameter->f_current_omega = com_f4_current_omega;
        g_user_motor_current_extended_cfg.p_design_parameter->f_current_zeta  = com_f4_current_zeta;

        g_user_motor_estimate_extended_cfg.f_e_obs_omega                 = com_f4_e_obs_omega;
        g_user_motor_estimate_extended_cfg.f_e_obs_zeta                  = com_f4_e_obs_zeta;
        g_user_motor_estimate_extended_cfg.f_pll_est_omega               = com_f4_pll_est_omega;
        g_user_motor_estimate_extended_cfg.f_pll_est_zeta                = com_f4_pll_est_zeta;
        g_user_motor_estimate_extended_cfg.st_motor_params.u2_mtr_pp     = com_u2_mtr_pp;
        g_user_motor_estimate_extended_cfg.st_motor_params.f4_mtr_r      = com_f4_mtr_r;
        g_user_motor_estimate_extended_cfg.st_motor_params.f4_mtr_ld     = com_f4_mtr_ld;
        g_user_motor_estimate_extended_cfg.st_motor_params.f4_mtr_lq     = com_f4_mtr_lq;
        g_user_motor_estimate_extended_cfg.st_motor_params.f4_mtr_m      = com_f4_mtr_m;
        g_user_motor_estimate_extended_cfg.st_motor_params.f4_mtr_j      = com_f4_mtr_j;
        p_angle_ctrl->st_bemf_obs.st_motor_params.u2_mtr_pp              = com_u2_mtr_pp;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_r               = com_f4_mtr_r;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_ld              = com_f4_mtr_ld;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_lq              = com_f4_mtr_lq;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_m               = com_f4_mtr_m;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_j               = com_f4_mtr_j;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_nominal_current = com_f4_nominal_current;

        /* 2nd motor */
        p_angle_ctrl = (motor_estimate_instance_ctrl_t *)p_angle1_instance->p_ctrl;

        g_user_motor_sensorless1_extended_cfg.f_overspeed_limit   = com_f4_overspeed_limit_rpm1;
        g_user_motor_sensorless1_extended_cfg.f_overcurrent_limit
            = com_f4_overcurrent_limit1 * MTR_SQRT_2 * MTR_OVERCURRENT_MARGIN_MULT;

        g_user_motor_speed1_extended_cfg.f_limit_speed_change          = com_f4_limit_speed_change1 * com_u2_mtr_pp1;
        g_user_motor_speed1_extended_cfg.f_iq_limit                    = com_f4_iq_limit1;
        g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_ref         = com_f4_ref_id1;
        g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_up_step     = com_f4_ol_id_up_step1 * com_f4_ref_id1;
        g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_down_step   = com_f4_ol_id_down_step1 * com_f4_ref_id1;
        g_user_motor_speed1_extended_cfg.ol_param.f4_id_down_speed_rpm = com_f4_id_down_speed_rpm1 * com_u2_mtr_pp1;
        g_user_motor_speed1_extended_cfg.ol_param.f4_id_up_speed_rpm   = com_f4_id_up_speed_rpm1 * com_u2_mtr_pp1;
        g_user_motor_speed1_extended_cfg.f_maximum_speed_rpm           = com_f4_max_speed_rpm1 * com_u2_mtr_pp1;
        g_user_motor_speed1_extended_cfg.d_param.f_speed_omega         = com_f4_speed_omega1;
        g_user_motor_speed1_extended_cfg.d_param.f_speed_zeta          = com_f4_speed_zeta1;
        g_user_motor_speed1_extended_cfg.mtr_param.u2_mtr_pp           = com_u2_mtr_pp1;
        g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_r            = com_f4_mtr_r1;
        g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_ld           = com_f4_mtr_ld1;
        g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_lq           = com_f4_mtr_lq1;
        g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_m            = com_f4_mtr_m1;
        g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_j            = com_f4_mtr_j1;

        g_user_motor_current1_extended_cfg.p_motor_parameter->u2_mtr_pp        = com_u2_mtr_pp1;
        g_user_motor_current1_extended_cfg.p_motor_parameter->f4_mtr_r         = com_f4_mtr_r1;
        g_user_motor_current1_extended_cfg.p_motor_parameter->f4_mtr_ld        = com_f4_mtr_ld1;
        g_user_motor_current1_extended_cfg.p_motor_parameter->f4_mtr_lq        = com_f4_mtr_lq1;
        g_user_motor_current1_extended_cfg.p_motor_parameter->f4_mtr_m         = com_f4_mtr_m1;
        g_user_motor_current1_extended_cfg.p_motor_parameter->f4_mtr_j         = com_f4_mtr_j1;
        g_user_motor_current1_extended_cfg.p_design_parameter->f_current_omega = com_f4_current_omega1;
        g_user_motor_current1_extended_cfg.p_design_parameter->f_current_zeta  = com_f4_current_zeta1;

        g_user_motor_estimate1_extended_cfg.f_e_obs_omega             = com_f4_e_obs_omega1;
        g_user_motor_estimate1_extended_cfg.f_e_obs_zeta              = com_f4_e_obs_zeta1;
        g_user_motor_estimate1_extended_cfg.f_pll_est_omega           = com_f4_pll_est_omega1;
        g_user_motor_estimate1_extended_cfg.f_pll_est_zeta            = com_f4_pll_est_zeta1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.u2_mtr_pp = com_u2_mtr_pp1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.f4_mtr_r  = com_f4_mtr_r1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.f4_mtr_ld = com_f4_mtr_ld1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.f4_mtr_lq = com_f4_mtr_lq1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.f4_mtr_m  = com_f4_mtr_m1;
        g_user_motor_estimate1_extended_cfg.st_motor_params.f4_mtr_j  = com_f4_mtr_j1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.u2_mtr_pp    = com_u2_mtr_pp1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_r     = com_f4_mtr_r1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_ld    = com_f4_mtr_ld1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_lq    = com_f4_mtr_lq1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_m     = com_f4_mtr_m1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_j     = com_f4_mtr_j1;
        p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_nominal_current = com_f4_nominal_current1;

        g_u1_trig_enable_write = MTR_FLG_SET;
        g_u1_enable_write ^= 1;                         /* Change every time 0 and 1 */
    }
}   /* End of function mtr_set_com_variables */

/***********************************************************************************************************************
* Function Name : mtr_ics_variables_init
* Description   : Initialize valiables for Analyzer interface
* Arguments     : u1_id ; motor identification number(0 or 1)
* Return Value  : None
***********************************************************************************************************************/
void mtr_ics_variables_init(uint8_t u1_id)
{
    p_speed_instance    = g_user_motor_cfg.p_motor_speed_instance;
    p_current_instance  = g_user_motor_cfg.p_motor_current_instance;
    p_angle_instance    = g_user_motor_current_cfg.p_motor_angle_instance;
    p_speed1_instance   = g_user_motor1_cfg.p_motor_speed_instance;
    p_current1_instance = g_user_motor1_cfg.p_motor_current_instance;
    p_angle1_instance   = g_user_motor_current1_cfg.p_motor_angle_instance;

    motor_estimate_instance_ctrl_t *p_angle_ctrl;

    g_u1_enable_write = 0U;
    com_u1_enable_write = 0U;

    switch (u1_id)
    {
        case MTR_ID_0:
#if MTR_MOTOR_PARAMETER
            com_u2_mtr_pp            = MP_POLE_PAIRS;
            com_f4_mtr_r             = MP_RESISTANCE;
            com_f4_mtr_ld            = MP_D_INDUCTANCE;
            com_f4_mtr_lq            = MP_Q_INDUCTANCE;
            com_f4_mtr_m             = MP_MAGNETIC_FLUX;
            com_f4_mtr_j             = MP_ROTOR_INERTIA;
            com_f4_overcurrent_limit = MP_NOMINAL_CURRENT_RMS;
            com_f4_iq_limit          = MP_NOMINAL_CURRENT_RMS;
            com_f4_nominal_current   = MP_NOMINAL_CURRENT_RMS;
#else
            p_angle_ctrl = (motor_estimate_instance_ctrl_t *)p_angle_instance->p_ctrl;
            com_u2_mtr_pp            = g_user_motor_speed_extended_cfg.mtr_param.u2_mtr_pp;
            com_f4_mtr_r             = g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_r;
            com_f4_mtr_ld            = g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_ld;
            com_f4_mtr_lq            = g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_lq;
            com_f4_mtr_m             = g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_m;
            com_f4_mtr_j             = g_user_motor_speed_extended_cfg.mtr_param.f4_mtr_j;
            com_f4_overcurrent_limit
                = g_user_motor_sensorless_extended_cfg.f_overcurrent_limit / (MTR_SQRT_2 * MTR_OVERCURRENT_MARGIN_MULT);
            com_f4_iq_limit          = g_user_motor_speed_extended_cfg.f_iq_limit;
            com_f4_nominal_current   = p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_nominal_current;
#endif
            com_f4_limit_speed_change
                = g_user_motor_speed_extended_cfg.f_limit_speed_change / com_u2_mtr_pp;

#if MTR_CONTROL_PARAMETER
            com_f4_current_omega       = CP_CURRENT_OMEGA;
            com_f4_current_zeta        = CP_CURRENT_ZETA;
            com_f4_speed_omega         = CP_SPEED_OMEGA;
            com_f4_speed_zeta          = CP_SPEED_ZETA;
            com_f4_e_obs_omega         = CP_E_OBS_OMEGA;
            com_f4_e_obs_zeta          = CP_E_OBS_ZETA;
            com_f4_pll_est_omega       = CP_PLL_EST_OMEGA;
            com_f4_pll_est_zeta        = CP_PLL_EST_ZETA;
            com_f4_id_down_speed_rpm   = CP_ID_DOWN_SPEED_RPM;
            com_f4_id_up_speed_rpm     = CP_ID_UP_SPEED_RPM;
            com_f4_max_speed_rpm       = CP_MAX_SPEED_RPM;
            com_f4_overspeed_limit_rpm = CP_OVERSPEED_LIMIT_RPM;
            com_f4_ref_id              = CP_OL_ID_REF;
#else
            com_f4_current_omega       = g_user_motor_current_extended_cfg.p_design_parameter->f_current_omega;
            com_f4_current_zeta        = g_user_motor_current_extended_cfg.p_design_parameter->f_current_zeta;
            com_f4_speed_omega         = g_user_motor_speed_extended_cfg.d_param.f_speed_omega;
            com_f4_speed_zeta          = g_user_motor_speed_extended_cfg.d_param.f_speed_zeta;
            com_f4_e_obs_omega         = g_user_motor_estimate_extended_cfg.f_e_obs_omega;
            com_f4_e_obs_zeta          = g_user_motor_estimate_extended_cfg.f_e_obs_zeta;
            com_f4_pll_est_omega       = g_user_motor_estimate_extended_cfg.f_pll_est_omega;
            com_f4_pll_est_zeta        = g_user_motor_estimate_extended_cfg.f_pll_est_zeta;
            com_f4_id_down_speed_rpm
                = g_user_motor_speed_extended_cfg.ol_param.f4_id_down_speed_rpm / com_u2_mtr_pp;
            com_f4_id_up_speed_rpm
                = g_user_motor_speed_extended_cfg.ol_param.f4_id_up_speed_rpm / com_u2_mtr_pp;
            com_f4_max_speed_rpm
                = g_user_motor_speed_extended_cfg.f_maximum_speed_rpm / com_u2_mtr_pp;
            com_f4_overspeed_limit_rpm = g_user_motor_sensorless_extended_cfg.f_overspeed_limit;
            com_f4_ref_id              = g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_ref;
#endif
            com_f4_ol_id_up_step       = g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_up_step / com_f4_ref_id;
            com_f4_ol_id_down_step     = g_user_motor_speed_extended_cfg.ol_param.f4_ol_id_down_step / com_f4_ref_id;
        break;

        case MTR_ID_1:
#if MTR_MOTOR_PARAMETER
            com_u2_mtr_pp1            = MP_POLE_PAIRS1;
            com_f4_mtr_r1             = MP_RESISTANCE1;
            com_f4_mtr_ld1            = MP_D_INDUCTANCE1;
            com_f4_mtr_lq1            = MP_Q_INDUCTANCE1;
            com_f4_mtr_m1             = MP_MAGNETIC_FLUX1;
            com_f4_mtr_j1             = MP_ROTOR_INERTIA;
            com_f4_overcurrent_limit1 = MP_NOMINAL_CURRENT_RMS1;
            com_f4_iq_limit1          = MP_NOMINAL_CURRENT_RMS1;
            com_f4_nominal_current1   = MP_NOMINAL_CURRENT_RMS;
#else
            p_angle_ctrl = (motor_estimate_instance_ctrl_t *)p_angle1_instance->p_ctrl;
            com_u2_mtr_pp1            = g_user_motor_speed1_extended_cfg.mtr_param.u2_mtr_pp;
            com_f4_mtr_r1             = g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_r;
            com_f4_mtr_ld1            = g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_ld;
            com_f4_mtr_lq1            = g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_lq;
            com_f4_mtr_m1             = g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_m;
            com_f4_mtr_j1             = g_user_motor_speed1_extended_cfg.mtr_param.f4_mtr_j;
            com_f4_overcurrent_limit1
                = g_user_motor_sensorless1_extended_cfg.f_overcurrent_limit
                    / (MTR_SQRT_2 * MTR_OVERCURRENT_MARGIN_MULT);
            com_f4_iq_limit1          = g_user_motor_speed1_extended_cfg.f_iq_limit;
            com_f4_nominal_current1   = p_angle_ctrl->st_bemf_obs.st_motor_params.f4_mtr_nominal_current;
#endif
            com_f4_limit_speed_change1
                = g_user_motor_speed1_extended_cfg.f_limit_speed_change / com_u2_mtr_pp1;

#if MTR_CONTROL_PARAMETER
            com_f4_current_omega1       = CP_CURRENT_OMEGA1;
            com_f4_current_zeta1        = CP_CURRENT_ZETA1;
            com_f4_speed_omega1         = CP_SPEED_OMEGA1;
            com_f4_speed_zeta1          = CP_SPEED_ZETA1;
            com_f4_e_obs_omega1         = CP_E_OBS_OMEGA1;
            com_f4_e_obs_zeta1          = CP_E_OBS_ZETA1;
            com_f4_pll_est_omega1       = CP_PLL_EST_OMEGA1;
            com_f4_pll_est_zeta1        = CP_PLL_EST_ZETA1;
            com_f4_id_down_speed_rpm1   = CP_ID_DOWN_SPEED_RPM1;
            com_f4_id_up_speed_rpm1     = CP_ID_UP_SPEED_RPM1;
            com_f4_max_speed_rpm1       = CP_MAX_SPEED_RPM1;
            com_f4_overspeed_limit_rpm1 = CP_OVERSPEED_LIMIT_RPM1;
            com_f4_ref_id1              = CP_OL_ID_REF1;
#else
            com_f4_current_omega1       = g_user_motor_current1_extended_cfg.p_design_parameter->f_current_omega;
            com_f4_current_zeta1        = g_user_motor_current1_extended_cfg.p_design_parameter->f_current_zeta;
            com_f4_speed_omega1         = g_user_motor_speed1_extended_cfg.d_param.f_speed_omega;
            com_f4_speed_zeta1          = g_user_motor_speed1_extended_cfg.d_param.f_speed_zeta;
            com_f4_e_obs_omega1         = g_user_motor_estimate1_extended_cfg.f_e_obs_omega;
            com_f4_e_obs_zeta1          = g_user_motor_estimate1_extended_cfg.f_e_obs_zeta;
            com_f4_pll_est_omega1       = g_user_motor_estimate1_extended_cfg.f_pll_est_omega;
            com_f4_pll_est_zeta1        = g_user_motor_estimate1_extended_cfg.f_pll_est_zeta;
            com_f4_id_down_speed_rpm1
                = g_user_motor_speed1_extended_cfg.ol_param.f4_id_down_speed_rpm / com_u2_mtr_pp1;
            com_f4_id_up_speed_rpm1
                = g_user_motor_speed1_extended_cfg.ol_param.f4_id_up_speed_rpm / com_u2_mtr_pp1;
            com_f4_max_speed_rpm1
                = g_user_motor_speed1_extended_cfg.f_maximum_speed_rpm / com_u2_mtr_pp1;
            com_f4_overspeed_limit_rpm1 = g_user_motor_sensorless1_extended_cfg.f_overspeed_limit;
            com_f4_ref_id1              = g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_ref;
#endif
            com_f4_ol_id_up_step1   = g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_up_step / com_f4_ref_id1;
            com_f4_ol_id_down_step1 = g_user_motor_speed1_extended_cfg.ol_param.f4_ol_id_down_step / com_f4_ref_id1;
        break;

        default:
            /* Do Nothing */
        break;
    }
}   /* End of function mtr_ics_variables_init */

/***********************************************************************************************************************
* Function Name : mtr_ics_interrupt_process
* Description   : Interrupt process to reflect data into RMW (called at current backward event)
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_ics_interrupt_process(void)
{
    // Reflect user settig parameters (com_xxx)
    mtr_set_com_variables();

    /* Decimation of ICS call */
    u1_cnt_ics++;
    if (MTR_ICS_DECIMATION <= u1_cnt_ics)
    {
        motor_current_instance_ctrl_t *p_current_ctrl = (motor_current_instance_ctrl_t *)p_current_instance->p_ctrl;
        motor_estimate_instance_ctrl_t *p_angle_ctrl  = (motor_estimate_instance_ctrl_t *)p_angle_instance->p_ctrl;
        motor_speed_instance_ctrl_t *p_speed_ctrl     = (motor_speed_instance_ctrl_t *)p_speed_instance->p_ctrl;

        g_f4_id_ref_monitor           = p_current_ctrl->f_id_ref;
        g_f4_id_ad_monitor            = p_current_ctrl->f_id_ad;
        g_f4_iq_ref_monitor           = p_current_ctrl->f_iq_ref;
        g_f4_iq_ad_monitor            = p_current_ctrl->f_iq_ad;
        g_f4_iu_ad_monitor            = p_current_ctrl->f_iu_ad;
        g_f4_iv_ad_monitor            = p_current_ctrl->f_iv_ad;
        g_f4_iw_ad_monitor            = p_current_ctrl->f_iw_ad;
        g_f4_vdc_ad_monitor           = p_current_ctrl->st_vcomp.f_vdc;
        g_f4_vd_ref_monitor           = p_current_ctrl->f_vd_ref;
        g_f4_vq_ref_monitor           = p_current_ctrl->f_vq_ref;
        g_f4_refu_monitor             = p_current_ctrl->f_refu;
        g_f4_refv_monitor             = p_current_ctrl->f_refv;
        g_f4_refw_monitor             = p_current_ctrl->f_refw;
        g_f4_speed_est_monitor        = p_current_ctrl->f_speed_rad;
        g_f4_current_kp_d_monitor     = p_current_ctrl->st_pi_id.f_kp;
        g_f4_current_ki_d_monitor     = p_current_ctrl->st_pi_id.f_ki;
        g_f4_current_kp_q_monitor     = p_current_ctrl->st_pi_iq.f_kp;
        g_f4_current_ki_q_monitor     = p_current_ctrl->st_pi_iq.f_ki;

        g_f4_speed_ref_monitor        = p_speed_ctrl->f_ref_speed_rad_ctrl;
        g_u1_state_id_ref_monitor     = p_speed_ctrl->u1_state_id_ref;
        g_u1_state_iq_ref_monitor     = p_speed_ctrl->u1_state_iq_ref;
        g_u1_state_speed_ref_monitor  = p_speed_ctrl->u1_state_speed_ref;
        g_f4_speed_kp_monitor         = p_speed_ctrl->pi_param.f_kp;
        g_f4_speed_ki_monitor         = p_speed_ctrl->pi_param.f_ki;

        g_f4_ed_monitor               = p_angle_ctrl->f4_ed;
        g_f4_eq_monitor               = p_angle_ctrl->f4_eq;
        g_f4_phase_err_monitor        = p_angle_ctrl->f4_phase_err_rad;
        g_f4_angle_rad_monitor        = p_angle_ctrl->f4_angle_rad;
        g_f4_k_e_obs_1_d_monitor      = p_angle_ctrl->st_bemf_obs.st_d_axis.f4_k_e_obs_1;
        g_f4_k_e_obs_2_d_monitor      = p_angle_ctrl->st_bemf_obs.st_d_axis.f4_k_e_obs_2;
        g_f4_k_e_obs_1_q_monitor      = p_angle_ctrl->st_bemf_obs.st_q_axis.f4_k_e_obs_1;
        g_f4_k_e_obs_2_q_monitor      = p_angle_ctrl->st_bemf_obs.st_q_axis.f4_k_e_obs_2;
        g_f4_kp_est_speed_monitor     = p_angle_ctrl->st_pll_est.f4_kp_est_speed;
        g_f4_ki_est_speed_monitor     = p_angle_ctrl->st_pll_est.f4_ki_est_speed;

        g_f4_speed_rpm_monitor        = g_f4_speed_est_monitor * MTR_RAD_RPM
                                            / (float)g_user_motor_current_extended_cfg.p_motor_parameter->u2_mtr_pp;

        p_current_ctrl = (motor_current_instance_ctrl_t *)p_current1_instance->p_ctrl;
        p_angle_ctrl   = (motor_estimate_instance_ctrl_t *)p_angle1_instance->p_ctrl;
        p_speed_ctrl   = (motor_speed_instance_ctrl_t *)p_speed1_instance->p_ctrl;

        g_f4_id_ref_monitor1           = p_current_ctrl->f_id_ref;
        g_f4_id_ad_monitor1            = p_current_ctrl->f_id_ad;
        g_f4_iq_ref_monitor1           = p_current_ctrl->f_iq_ref;
        g_f4_iq_ad_monitor1            = p_current_ctrl->f_iq_ad;
        g_f4_iu_ad_monitor1            = p_current_ctrl->f_iu_ad;
        g_f4_iv_ad_monitor1            = p_current_ctrl->f_iv_ad;
        g_f4_iw_ad_monitor1            = p_current_ctrl->f_iw_ad;
        g_f4_vdc_ad_monitor1           = p_current_ctrl->st_vcomp.f_vdc;
        g_f4_vd_ref_monitor1           = p_current_ctrl->f_vd_ref;
        g_f4_vq_ref_monitor1           = p_current_ctrl->f_vq_ref;
        g_f4_refu_monitor1             = p_current_ctrl->f_refu;
        g_f4_refv_monitor1             = p_current_ctrl->f_refv;
        g_f4_refw_monitor1             = p_current_ctrl->f_refw;
        g_f4_speed_est_monitor1        = p_current_ctrl->f_speed_rad;
        g_f4_current_kp_d_monitor1     = p_current_ctrl->st_pi_id.f_kp;
        g_f4_current_ki_d_monitor1     = p_current_ctrl->st_pi_id.f_ki;
        g_f4_current_kp_q_monitor1     = p_current_ctrl->st_pi_iq.f_kp;
        g_f4_current_ki_q_monitor1     = p_current_ctrl->st_pi_iq.f_ki;

        g_f4_speed_ref_monitor1        = p_speed_ctrl->f_ref_speed_rad_ctrl;
        g_u1_state_id_ref_monitor1     = p_speed_ctrl->u1_state_id_ref;
        g_u1_state_iq_ref_monitor1     = p_speed_ctrl->u1_state_iq_ref;
        g_u1_state_speed_ref_monitor1  = p_speed_ctrl->u1_state_speed_ref;
        g_f4_speed_kp_monitor1         = p_speed_ctrl->pi_param.f_kp;
        g_f4_speed_ki_monitor1         = p_speed_ctrl->pi_param.f_ki;

        g_f4_ed_monitor1               = p_angle_ctrl->f4_ed;
        g_f4_eq_monitor1               = p_angle_ctrl->f4_eq;
        g_f4_phase_err_monitor1        = p_angle_ctrl->f4_phase_err_rad;
        g_f4_angle_rad_monitor1        = p_angle_ctrl->f4_angle_rad;
        g_f4_k_e_obs_1_d_monitor1      = p_angle_ctrl->st_bemf_obs.st_d_axis.f4_k_e_obs_1;
        g_f4_k_e_obs_2_d_monitor1      = p_angle_ctrl->st_bemf_obs.st_d_axis.f4_k_e_obs_2;
        g_f4_k_e_obs_1_q_monitor1      = p_angle_ctrl->st_bemf_obs.st_q_axis.f4_k_e_obs_1;
        g_f4_k_e_obs_2_q_monitor1      = p_angle_ctrl->st_bemf_obs.st_q_axis.f4_k_e_obs_2;
        g_f4_kp_est_speed_monitor1     = p_angle_ctrl->st_pll_est.f4_kp_est_speed;
        g_f4_ki_est_speed_monitor1     = p_angle_ctrl->st_pll_est.f4_ki_est_speed;

        g_f4_speed_rpm_monitor1        = g_f4_speed_est_monitor1 * MTR_RAD_RPM
                                            / (float)g_user_motor_current1_extended_cfg.p_motor_parameter->u2_mtr_pp;

        u1_cnt_ics = 0U;

        /* Call ICS */
        ics2_watchpoint();
    }

    /* When trigger enable is set, reflect user settings */
    if (MTR_FLG_SET == g_u1_trig_enable_write)
    {
        p_speed_instance->p_api->parameterUpdate(p_speed_instance->p_ctrl, &g_user_motor_speed_cfg);
        p_current_instance->p_api->parameterUpdate(p_current_instance->p_ctrl, &g_user_motor_current_cfg);
        p_angle_instance->p_api->parameterUpdate(p_angle_instance->p_ctrl, &g_user_motor_angle_cfg);

        p_speed1_instance->p_api->parameterUpdate(p_speed1_instance->p_ctrl, &g_user_motor_speed1_cfg);
        p_current1_instance->p_api->parameterUpdate(p_current1_instance->p_ctrl, &g_user_motor_current1_cfg);
        p_angle1_instance->p_api->parameterUpdate(p_angle1_instance->p_ctrl, &g_user_motor_angle1_cfg);

        g_u1_trig_enable_write = MTR_FLG_CLR;
    }
} /* End of function mtr_ics_interrupt_process */


static float rm_motor_speed_get_torque_limit_mnm (
    motor_speed_instance_ctrl_t * p_ctrl)
{
    float f_iq_limit;
    float f_torque_constant;
    float f_torque_limit_nm;

    motor_speed_extended_cfg_t * p_extended_cfg =
        (motor_speed_extended_cfg_t *) p_ctrl->p_cfg->p_extend;

    /* Internal maximum Iq used by sensorless control */
    f_iq_limit = p_extended_cfg->f_iq_limit * MTR_SQRT_3;

    /* Torque constant [Nm/A] */
    f_torque_constant = (float) p_extended_cfg->mtr_param.u2_mtr_pp * p_extended_cfg->mtr_param.f4_mtr_m;

    /* Maximum torque [Nm] */
    f_torque_limit_nm =
        f_torque_constant *
        f_iq_limit *
        MOTOR_SPEED_TORQUE_LIMIT_MARGIN;

    /* Return [mNm] */
    return f_torque_limit_nm * 1000.0F;
}
