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
* File Name   : r_mtr_ics.c
* Description : Processes of a user interface (tool)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/

#include <stdint.h>
#include "mtr_main.h"
#include "r_mtr_ics.h"
#include "ics2_RA6T2.h"
#include "hal_data.h"
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_sensorless_vector_api.h"
#include "r_system_manager_api.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
uint16_t    g_u2_conf_hw;
uint16_t    g_u2_conf_sw;
uint16_t    g_u2_conf_tool;
uint8_t     gui_u1_active_gui;
uint16_t    g_u2_conf_sw_ver;
uint8_t     g_u1_conf_motor_type_len;
uint8_t     g_u1_conf_control_len;
uint8_t     g_u1_conf_inverter_len;
uint8_t     g_u1_conf_motor_type[CONF_MOTOR_TYPE_LEN];
uint8_t     g_u1_conf_control[CONF_CONTROL_LEN];
uint8_t     g_u1_conf_inverter[CONF_INVERTER_LEN];

/***********************************************************************************************************************
* Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
st_rmw_param_buffer_t   g_st_rmw_input_buffer;  /* Structure for ICS input */
uint8_t     g_u1_update_param_flag = 0;         /* Update commands and configurations flag */

/* Operation variables */
uint8_t     com_u1_system_mode;                 /* System mode */
uint8_t     g_u1_system_mode;                   /* System mode */
uint8_t     com_u1_enable_write;                /* ICS write enable flag */
uint8_t     g_u1_enable_write;                  /* ICS write enable flag */

/* Offset parameters */
uint16_t    com_u2_offset_calc_time;            /* current offset calculation time */

/* Motor parameters */
uint16_t    com_u2_mtr_pp;                      /* pole pairs */
float       com_f4_mtr_r;                       /* resistance [ohm] */
float       com_f4_mtr_ld;                      /* d-axis inductance [H] */
float       com_f4_mtr_lq;                      /* q-axis inductance [H] */
float       com_f4_mtr_m;                       /* permanent magnetic flux [Wb] */
float       com_f4_mtr_j;                       /* rotor inertia [kgm^2] */
float       com_f4_nominal_current_rms;         /* The nominal current[Arms] */
float       com_f4_max_speed_rpm;               /* maximum speed [rpm] (mechanical) */

/* Sensorless parameter */
uint8_t     com_u1_ctrl_loop_mode;              /* loop mode select */
/* Current control */
float       com_f4_current_omega_hz;            /* natural frequency for current loop [Hz] */
float       com_f4_current_zeta;                /* damping ratio for current loop */

/* Speed control */
float       com_f4_speed_omega_hz;              /* natural frequency for speed loop [Hz] */
float       com_f4_speed_zeta;                  /* damping ratio for speed loop */
float       com_f4_speed_lpf_hz;                /* Natural frequency for speed LPF [Hz] */
float       com_f4_ref_speed_rpm;               /* motor speed reference [rpm] (mechanical) */
float       com_f4_speed_rate_limit_rpm;        /* limit of speed change [rpm/s] */
float       com_f4_overspeed_limit_rpm;         /* over speed limit [rpm] (mechanical) */
float       com_f4_speed_rate_limit_rpm1;        /* limit of speed change [rpm/s] */
float       com_f4_speed_rate_limit_rpm2;
float       com_f4_speed_rate_limit_rpm3;         /* over speed limit [rpm] (mechanical) */



/* Optional functions */
/* Voltage error compensation */
uint8_t     com_u1_flag_volt_err_comp_use;      /* Flags whether use voltage error compensation */

/* Flux-weakening control */
uint8_t     com_u1_flag_fluxwkn_use;            /* Flags whether use flux-weakening */

/* MTPA control */
uint8_t     com_u1_flag_mtpa_use;               /* Flags whether use MTPA */

/* Flying Start control */
uint8_t     com_u1_flag_flying_start_use;       /* Flags whether use flying start */

/* Stall Detection control */
uint8_t     com_u1_flag_stall_detection_use;    /* Flags whether use stall Detection */

/* Torque Vibration Compensation control */
uint8_t     com_u1_flag_trq_vibration_comp_use; /* Flags whether use torque vibration compensation */

/* BEMF observer */
float       com_f4_e_obs_omega_hz;              /* Natural frequency for BEMF observer [Hz] */
float       com_f4_e_obs_zeta;                  /* Damping ratio for BEMF observer */
float       com_f4_pll_est_omega_hz;            /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
float       com_f4_pll_est_zeta;                /* Damping ratio for rotor position Phase-Locked Loop */

float       com_f4_pll_estlow_omega_hz;     /* Natural frequency for rotor position Phase-Locked Loop [Hz] (lowspd) */
float       com_f4_pll_estlow_zeta;         /* Damping ratio for rotor position Phase-Locked Loop (lowspd) */
/* Extended observe */
uint8_t     com_u1_flag_extobserver_use;    /* Flags whether use ext observer*/
float       com_f4_extobs_omega;            /* natural frequency for ext observer [Hz] */
/* low speed */
float       com_f4_spd_low_to_high_threshold; /* Slow-to-fast switching speed threshold [r/min] */
float       com_f4_spd_high_to_low_threshold; /* Fast to slow switching speed threshold [r/min] */

/* Stall detection */
float       com_f4_id_hpf_time;                 /* d-axis HPF time constant [s] */
float       com_f4_iq_hpf_time;                 /* d-axis HPF time constant [s] */
float       com_f4_threshold_level;             /* stall detection threshold [A] */
float       com_f4_threshold_time;              /* detection time [s] */

/* Torque vibration suppression */
uint8_t     com_u1_flag_trqvib_comp_learning;   /* Flags whether learning of Torque Vibration Compensation */
float       com_f4_timelead;                    /* Output phase index of Torque Vibration Compensation */
float       com_f4_tf_lpf_time;                 /* Tracking filter internal LPF constant */
float       com_f4_output_gain;                 /* Output gain (speed deviation/vibration torque conversion factor) */
float       com_f4_input_weight2;               /* coefficient for moving average filter */
float       com_f4_input_weight1;               /* coefficient for moving average filter */
float       com_f4_input_weight0;               /* coefficient for moving average filter */

/* Flying start */
float       com_f4_restart_speed;               /* Restart judgment speed reference value */
float       com_f4_off_time;                    /* Short-circuit OFF time [s] */
float       com_f4_over_time;                   /* Detection excess time */
float       com_f4_active_brake_time;           /* Active braking time */
float       com_f4_on_current_th;               /* Current threshold of Short-circuit ON */

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
static uint8_t  s_u1_cnt_ics = 0;               /* Counter for period of calling "scope_watchpoint" */
static void r_app_rmw_system_mode(void);
static void r_app_rmw_check_com_input(void);
static void r_app_rmw_update_params(void);
static void r_app_rmw_update_command(void);

/***********************************************************************************************************************
* Function Name : r_app_rmw_ui_init
* Description   : Initialize variables for Analyzer interface
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_ui_init(void)
{
    /* ICS Communication variables, Do not delete these variables. */
    int i;
    uint8_t u1_conf_motor_type[] = CONF_MOTOR_TYPE;
    uint8_t u1_conf_control[]    = CONF_CONTROL;
    uint8_t u1_conf_inverter[]   = CONF_INVERTER;
    g_u1_conf_motor_type_len = CONF_MOTOR_TYPE_LEN;
    g_u1_conf_control_len    = CONF_CONTROL_LEN;
    g_u1_conf_inverter_len   = CONF_INVERTER_LEN;
    for (i = 0; i < g_u1_conf_motor_type_len; i++)
    {
        g_u1_conf_motor_type[i] = u1_conf_motor_type[i];
    }
    for (i = 0; i < g_u1_conf_control_len; i++)
    {
        g_u1_conf_control[i] = u1_conf_control[i];
    }
    for (i = 0; i < g_u1_conf_inverter_len; i++)
    {
        g_u1_conf_inverter[i] = u1_conf_inverter[i];
    }
    g_u2_conf_hw   = 0x0008;        /* 0000000000001000b */
    g_u2_conf_sw   = 0x0000;        /* 0000000000000000b */
    g_u2_conf_tool = 0x0300;        /* 0000011000000000b */

    /* Operation variables */
    com_u1_system_mode  = STATEMACHINE_EVENT_STOP;
    g_u1_system_mode    = STATEMACHINE_EVENT_STOP;
    com_u1_enable_write = 0;
    g_u1_enable_write   = 0;

    /* Offset parameters */
    com_u2_offset_calc_time = CURRENT_CFG_OFFSET_CALC_TIME;

    /* Motor parameters */
    com_u2_mtr_pp              = MOTOR_CFG_POLE_PAIRS;
    com_f4_mtr_r               = MOTOR_CFG_RESISTANCE;
    com_f4_mtr_ld              = MOTOR_CFG_D_INDUCTANCE;
    com_f4_mtr_lq              = MOTOR_CFG_Q_INDUCTANCE;
    com_f4_mtr_m               = MOTOR_CFG_MAGNETIC_FLUX;
    com_f4_mtr_j               = MOTOR_CFG_ROTOR_INERTIA;
    com_f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS;
    com_f4_max_speed_rpm       = MOTOR_CFG_MAX_SPEED_RPM;

    /* Sensorless parameter */
    com_u1_ctrl_loop_mode    = MOTOR_COMMON_CFG_LOOP_MODE;

    /* Control parameters */
    /* Current control */
    com_f4_current_omega_hz = CURRENT_CFG_OMEGA;
    com_f4_current_zeta     = CURRENT_CFG_ZETA;

    /* Speed control */
    com_f4_speed_omega_hz       = SPEED_CFG_OMEGA;
    com_f4_speed_zeta           = SPEED_CFG_ZETA;
    com_f4_speed_lpf_hz         = SPEED_CFG_LPF_OMEGA;
    com_f4_ref_speed_rpm        = 0.0f;
    com_f4_speed_rate_limit_rpm = SPEED_CFG_RATE_LIMIT_RPM;
    com_f4_overspeed_limit_rpm  = SPEED_CFG_SPEED_LIMIT_RPM;
    com_f4_speed_rate_limit_rpm1 = SPEED_CFG_RATE_LIMIT_RPM1;
    com_f4_speed_rate_limit_rpm2 = SPEED_CFG_RATE_LIMIT_RPM2;
    com_f4_speed_rate_limit_rpm3  =  SPEED_CFG_RATE_LIMIT_RPM3;

    /* Optional functions */
    /* Voltage error compensation */
    com_u1_flag_volt_err_comp_use = CURRENT_CFG_VOLT_ERR_COMP;

    /* Flux-weakening control */
    com_u1_flag_fluxwkn_use = SPEED_CFG_FLUX_WEAKENING;

    /* MTPA control */
    com_u1_flag_mtpa_use = SPEED_CFG_MTPA;

    /* Flying Start control */
    com_u1_flag_flying_start_use = CURRENT_CFG_FLYING_START;

    /* Stall Detection control */
    com_u1_flag_stall_detection_use = CURRENT_CFG_STALL_DETECTION;

    /* Torque Vibration Compensation control */
    com_u1_flag_trq_vibration_comp_use = MTR_FLG_CLR;

    /* BEMF observer */
    com_f4_e_obs_omega_hz   = CURRENT_CFG_E_OBS_OMEGA;
    com_f4_e_obs_zeta       = CURRENT_CFG_E_OBS_ZETA;
    com_f4_pll_est_omega_hz = CURRENT_CFG_PLL_EST_OMEGA;
    com_f4_pll_est_zeta     = CURRENT_CFG_PLL_EST_ZETA;
    com_f4_pll_estlow_omega_hz = CURRENT_CFG_PLL_ESTLOW_OMEGA; /* low speed sensorless */
    com_f4_pll_estlow_zeta     = CURRENT_CFG_PLL_ESTLOW_ZETA; /* low speed sensorless */

    /* Speed observer */
    com_u1_flag_extobserver_use = SPEED_CFG_OBSERVER;
    com_f4_extobs_omega         = SPEED_CFG_SOB_OMEGA;

    /* low speed */
    com_f4_spd_low_to_high_threshold = MOTOR_SENSORLESS_VECTOR_THRESHOLD_HIGHSPEED * MTR_RAD2RPM; /* Slow-to-fast switching speed threshold [r/min] */
    com_f4_spd_high_to_low_threshold = MOTOR_SENSORLESS_VECTOR_THRESHOLD_LOWSPEED * MTR_RAD2RPM;  /* Fast to slow switching speed threshold [r/min] */

    /* Stall detection */
    com_f4_id_hpf_time     = CURRENT_CFG_STALL_D_HPF_GAIN;
    com_f4_iq_hpf_time     = CURRENT_CFG_STALL_Q_HPF_GAIN;
    com_f4_threshold_level = CURRENT_CFG_STALL_THRESHOLD_LEVEL;
    com_f4_threshold_time  = CURRENT_CFG_STALL_THRESHOLD_TIME;

    /* Torque vibration suppression */
    com_u1_flag_trqvib_comp_learning = MTR_FLG_CLR;
    com_f4_timelead                  = CURRENT_CFG_TRQVIB_TIMELEAP;
    com_f4_tf_lpf_time               = CURRENT_CFG_TRQVIB_LPF_GAIN;
    com_f4_output_gain               = CURRENT_CFG_TRQVIB_OUTPUT_GAIN;
    com_f4_input_weight2             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_2;
    com_f4_input_weight1             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_1;
    com_f4_input_weight0             = CURRENT_CFG_TRQVIB_INPUT_WEIGHT_0;

    /* Flying start */
    com_f4_restart_speed     = SENSORLESS_VECTOR_FLY_START_RESTART_SPEED_LIMIT;
    com_f4_off_time          = SENSORLESS_VECTOR_FLY_START_OFF_TIME_SEC;
    com_f4_over_time         = SENSORLESS_VECTOR_FLY_START_OVER_TIME_SEC;
    com_f4_active_brake_time = SENSORLESS_VECTOR_FLY_START_ACTIVE_BRAKE_TIME_SEC;
    com_f4_on_current_th     = SENSORLESS_VECTOR_FLY_START_CURRENT_TH;
} /* End of function r_app_rmw_ui_init */

/***********************************************************************************************************************
* Function Name : r_app_rmw_ui_mainloop
* Description   : RMW communication main-loop process
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_ui_mainloop(void)
{
    r_app_rmw_system_mode();
} /* End of function r_app_rmw_ui_mainloop */

/***********************************************************************************************************************
* Function Name : r_app_rmw_copy_com_to_buffer
* Description   : Set com variables to "tool" input buffer
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_copy_com_to_buffer(void)
{
    r_app_rmw_check_com_input();

    /***** When com_s2_enable_sw and g_u1_enable_write are same value, rewrite enable. *****/
    if (com_u1_enable_write == g_u1_enable_write)
    {
        /* Offset parameters */
        g_st_rmw_input_buffer.u2_offset_calc_time = com_u2_offset_calc_time;

        /* Motor parameters */
        g_st_rmw_input_buffer.st_motor.u2_mtr_pp              = com_u2_mtr_pp;
        g_st_rmw_input_buffer.st_motor.f4_mtr_r               = com_f4_mtr_r;
        g_st_rmw_input_buffer.st_motor.f4_mtr_ld              = com_f4_mtr_ld;
        g_st_rmw_input_buffer.st_motor.f4_mtr_lq              = com_f4_mtr_lq;
        g_st_rmw_input_buffer.st_motor.f4_mtr_m               = com_f4_mtr_m;
        g_st_rmw_input_buffer.st_motor.f4_mtr_j               = com_f4_mtr_j;
        g_st_rmw_input_buffer.st_motor.f4_nominal_current_rms = com_f4_nominal_current_rms;
        g_st_rmw_input_buffer.f4_max_speed_rpm                = com_f4_max_speed_rpm;

        /* Sensorless parameter */
        g_st_rmw_input_buffer.u1_ctrl_loop_mode    = com_u1_ctrl_loop_mode;
        /* Control parameters */
        /* Current control */
        g_st_rmw_input_buffer.f4_current_omega_hz = com_f4_current_omega_hz;
        g_st_rmw_input_buffer.f4_current_zeta     = com_f4_current_zeta;

        /* Speed control */
        g_st_rmw_input_buffer.f4_speed_omega_hz       = com_f4_speed_omega_hz;
        g_st_rmw_input_buffer.f4_speed_zeta           = com_f4_speed_zeta;
        g_st_rmw_input_buffer.f4_speed_lpf_hz         = com_f4_speed_lpf_hz;
        g_st_rmw_input_buffer.f4_ref_speed_rpm        = com_f4_ref_speed_rpm;
        g_st_rmw_input_buffer.f4_speed_rate_limit_rpm = com_f4_speed_rate_limit_rpm;
        g_st_rmw_input_buffer.f4_overspeed_limit_rpm  = com_f4_overspeed_limit_rpm;

        /* Optional functions */
        /* Voltage error compensation */
        g_st_rmw_input_buffer.u1_flag_volt_err_comp_use = com_u1_flag_volt_err_comp_use;

        /* Flux-weakening control */
        g_st_rmw_input_buffer.u1_flag_fluxwkn_use = com_u1_flag_fluxwkn_use;

        /* MTPA control */
        g_st_rmw_input_buffer.u1_flag_mtpa_use = com_u1_flag_mtpa_use;

        /* Flying Start control */
        g_st_rmw_input_buffer.u1_flag_flying_start_use = com_u1_flag_flying_start_use;

        /* Stall Detection control */
        g_st_rmw_input_buffer.u1_flag_stall_detection_use = com_u1_flag_stall_detection_use;

        /* Torque Vibration Compensation control */
        g_st_rmw_input_buffer.u1_flag_trq_vibration_comp_use = com_u1_flag_trq_vibration_comp_use;

        /* BEMF observer */
        g_st_rmw_input_buffer.f4_e_obs_omega_hz   = com_f4_e_obs_omega_hz;
        g_st_rmw_input_buffer.f4_e_obs_zeta       = com_f4_e_obs_zeta;
        g_st_rmw_input_buffer.f4_pll_est_omega_hz = com_f4_pll_est_omega_hz;
        g_st_rmw_input_buffer.f4_pll_est_zeta     = com_f4_pll_est_zeta;
        g_st_rmw_input_buffer.f4_pll_estlow_omega_hz = com_f4_pll_estlow_omega_hz; /* low speed sensorless */
        g_st_rmw_input_buffer.f4_pll_estlow_zeta     = com_f4_pll_estlow_zeta; /* low speed sensorless */

        /* Speed observer */
        g_st_rmw_input_buffer.u1_flag_extobserver_use = com_u1_flag_extobserver_use;
        g_st_rmw_input_buffer.f4_extobs_omega         = com_f4_extobs_omega;

        /* High/low speed switching speed threshold */
        g_st_rmw_input_buffer.f4_highspd_threshold    = com_f4_spd_low_to_high_threshold;
        g_st_rmw_input_buffer.f4_lowspd_threshold     = com_f4_spd_high_to_low_threshold;

        /* Stall detection */
        g_st_rmw_input_buffer.f4_id_hpf_time     = com_f4_id_hpf_time;
        g_st_rmw_input_buffer.f4_iq_hpf_time     = com_f4_iq_hpf_time;
        g_st_rmw_input_buffer.f4_threshold_level = com_f4_threshold_level;
        g_st_rmw_input_buffer.f4_threshold_time  = com_f4_threshold_time;

        /* Torque vibration suppression */
        g_st_rmw_input_buffer.u1_flag_trqvib_comp_learning = com_u1_flag_trqvib_comp_learning;
        g_st_rmw_input_buffer.f4_timelead                  = com_f4_timelead;
        g_st_rmw_input_buffer.f4_tf_lpf_time               = com_f4_tf_lpf_time;
        g_st_rmw_input_buffer.f4_output_gain               = com_f4_output_gain;
        g_st_rmw_input_buffer.f4_input_weight2             = com_f4_input_weight2;
        g_st_rmw_input_buffer.f4_input_weight1             = com_f4_input_weight1;
        g_st_rmw_input_buffer.f4_input_weight0             = com_f4_input_weight0;

        /* Flying start */
        g_st_rmw_input_buffer.f4_restart_speed     = com_f4_restart_speed;
        g_st_rmw_input_buffer.f4_off_time          = com_f4_off_time;
        g_st_rmw_input_buffer.f4_over_time         = com_f4_over_time;
        g_st_rmw_input_buffer.f4_active_brake_time = com_f4_active_brake_time;
        g_st_rmw_input_buffer.f4_on_current_th     = com_f4_on_current_th;

        /* copy variables com to ICS input */
        g_u1_update_param_flag = 1;
        g_u1_enable_write     ^= 1;                     /* change every time 0 and 1 */
    }
} /* End of function r_app_rmw_copy_com_to_buffer */

/***********************************************************************************************************************
* Function Name : r_app_rmw_interrupt_handler
* Description   : Process for ICS(Analyzer)
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_interrupt_handler(void)
{
    s_u1_cnt_ics++;

    /* Decimation of ICS call */
    if (ICS_DECIMATION < s_u1_cnt_ics)
    {
        s_u1_cnt_ics = 0;

        /* Call ICS */
        ics2_watchpoint();
    }

    /* Update commands and configurations when trigger flag is set */
    if (1 == g_u1_update_param_flag)
    {
        r_app_rmw_update_params();
        g_u1_update_param_flag = 0;
    }
    else
    {
        r_app_rmw_update_command();
    }
} /* End of function r_app_rmw_interrupt_handler */

/***********************************************************************************************************************
* Function Name : r_app_rmw_system_mode
* Description   : Process of system mode command
* Arguments     : None
* Return Value  : None
************************************************************************************************************************/
static void r_app_rmw_system_mode(void)
{
    uint8_t  u1_motor_status;
    uint16_t u2_pfc_status;

    /*============================*/
    /*        Get PFC status      */
    /*============================*/
    u2_pfc_status = R_SYSTEM_MANAGER_PfcStatusGet();

    if ((PFC_MODE_PFC_ON == u2_pfc_status) || (PFC_MODE_PFC_ERROR == u2_pfc_status))
    {
        /*============================*/
        /*        Execute event       */
        /*============================*/
        R_SYSTEM_MANAGER_SystemMode();

        /*============================*/
        /*   Reset event function     */
        /*============================*/
        u1_motor_status = R_SYSTEM_MANAGER_MotorStatusGet(); /* Get status of motor control system */

        if (STATEMACHINE_EVENT_RESET == g_u1_system_mode)
        {
            if (STATEMACHINE_STATE_STOP == u1_motor_status)
            {
                r_app_rmw_ui_init();                         /* Initialize private global variables for reset event */
            }
            else if (STATEMACHINE_STATE_ERROR == u1_motor_status)
            {
                g_u1_system_mode   = STATEMACHINE_EVENT_ERROR;
                com_u1_system_mode = STATEMACHINE_EVENT_ERROR;
            }
            else
            {
                /* Do nothing */
            }

            /*============================*/
            /*     PFC reset control      */
            /*============================*/
            if (PFC_MODE_PFC_ERROR == u2_pfc_status)
            {
                R_SYSTEM_MANAGER_PfcErrorCancel();
            }
        }
    }
    else
    {
    	com_u1_system_mode = g_u1_system_mode;
    }
} /* End of function r_app_rmw_system_mode */

/***********************************************************************************************************************
* Function Name : r_app_rmw_check_com_input
* Description   : Check com variables
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_rmw_check_com_input(void)
{
    float f4_temp0;

    /**************************/
    /*    limit variables     */
    /**************************/
    com_f4_ref_speed_rpm = motor_filter_limitf_abs(com_f4_ref_speed_rpm, com_f4_max_speed_rpm);

    /* current natural frequency limit 1Hz ~ 1000Hz */
    com_f4_current_omega_hz = motor_filter_limitf(com_f4_current_omega_hz,
                                                  APP_CFG_MAX_CURRENT_OMEGA,
                                                  APP_CFG_MIN_OMEGA);

    /* natural frequency & damping ratio for speed control loop */
    f4_temp0 = com_f4_current_omega_hz / APP_CFG_FREQ_BAND_LIMIT;
    if (f4_temp0 < com_f4_speed_omega_hz)
    {
        com_f4_speed_omega_hz = f4_temp0;

        /* speed natural frequency minimum limit 1Hz */
        com_f4_speed_omega_hz = motor_filter_lower_limitf(com_f4_speed_omega_hz, APP_CFG_MIN_OMEGA);
    }
} /* End of function r_app_rmw_check_com_input */

/**********************************************************************************************************************
 * Function Name: r_app_rmw_update_params
 * Description  : Apply parameters in buffer through API
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
static void r_app_rmw_update_params(void)
{
    R_SYSTEM_MANAGER_ParameterUpdate();
} /* End of function r_app_rmw_update_params */

/***********************************************************************************************************************
* Function Name : r_app_rmw_update_command
* Description   : Update commands without copying to buffer or update trigger
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_rmw_update_command(void)
{
    R_SYSTEM_MANAGER_UpdateCommand();
} /* End of function r_app_rmw_update_command */
