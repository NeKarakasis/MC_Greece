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
* File Name   : r_app_rmw.c
* Description : Processes of a user interface (tool)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <machine.h>

/* Main associated header file */
#include "r_app_rmw.h"
#include "r_app_main.h"
#include "r_app_board_ui_ctrl.h"
#include "r_motor_module_cfg.h"
#include "r_motor_sensorless_vector_api.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
uint16_t    g_u2_conf_hw   = 0x0101;        /* 0000000100000001b */
uint16_t    g_u2_conf_sw   = 0x0000;        /* 0000000000000000b */
uint16_t    g_u2_conf_tool = 0x0200;        /* 0000001000000000b */
uint8_t     gui_u1_active_gui;
uint16_t    g_u2_conf_sw_ver;

/***********************************************************************************************************************
* Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
st_rmw_param_buffer_t   g_st_rmw_input_buffer;      /* Structure for ICS input */
uint8_t     g_u1_update_param_flag = 0;             /* Update commands and configurations flag */

/* Operation variables */
uint8_t     com_u1_system_mode;             /* System mode */
uint8_t     g_u1_system_mode;               /* System mode */
uint8_t     com_u1_enable_write;            /* ICS write enable flag */
uint8_t     g_u1_enable_write;              /* ICS write enable flag */
uint8_t     com_u1_sw_userif;               /* User interface switch */
uint8_t     g_u1_sw_userif;                 /* User interface switch */

/* Offset parameters */
uint16_t    com_u2_offset_calc_time;        /* current offset calculation time */

/* Motor parameters */
uint16_t    com_u2_mtr_pp;                  /* pole pairs */
float       com_f4_mtr_r;                   /* resistance [ohm] */
float       com_f4_mtr_ld;                  /* d-axis inductance [H] */
float       com_f4_mtr_lq;                  /* q-axis inductance [H] */
float       com_f4_mtr_m;                   /* permanent magnetic flux [Wb] */
float       com_f4_mtr_j;                   /* rotor inertia [kgm^2] */
float       com_f4_nominal_current_rms;     /* The nominal current[Arms] */
float       com_f4_max_speed_rpm;           /* maximum speed [rpm] (mechanical) */

/* Sensorless parameter */
uint8_t     com_u1_ctrl_loop_mode;          /* loop mode select */
float       com_f4_ol_ref_id;               /* id reference when open loop [A] */
float       com_f4_id_up_time;              /* time to increase id */
float       com_f4_id_down_time;            /* time to decrease id */
float       com_f4_id_down_speed_rpm;       /* The speed threshold[rpm] to ramp down the d-axis current */
float       com_f4_id_up_speed_rpm;         /* The speed threshold[rpm] to ramp up the d-axis current */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
int16_t     com_s2_difference_minimum;      /* Minimum difference of PWM duty */
int16_t     com_s2_adjust_adc_delay;        /* Adjustment delay for A/D conversion */
#endif

/* Control parameters */
/* Current control */
float       com_f4_current_omega_hz;        /* natural frequency for current loop [Hz] */
float       com_f4_current_zeta;            /* damping ratio for current loop */

/* Speed control */
float       com_f4_speed_omega_hz;          /* natural frequency for speed loop [Hz] */
float       com_f4_speed_zeta;              /* damping ratio for speed loop */
float       com_f4_speed_lpf_hz;            /* Natural frequency for speed LPF [Hz] */
float       com_f4_ref_speed_rpm;           /* motor speed reference [rpm] (mechanical) */
float       com_f4_speed_rate_limit_rpm;    /* limit of speed change [rpm/s] */
float       com_f4_overspeed_limit_rpm;     /* over speed limit [rpm] (mechanical) */

/* Optional functions */
/* Voltage error compensation */
uint8_t     com_u1_flag_volt_err_comp_use;  /* Flags whether use voltage error compensation */

/* Flux-weakening control */
uint8_t     com_u1_flag_fluxwkn_use;        /* Flags whether use flux-weakening */

/* BEMF observer */
float       com_f4_e_obs_omega_hz;          /* Natural frequency for BEMF observer [Hz] */
float       com_f4_e_obs_zeta;              /* Damping ratio for BEMF observer */
float       com_f4_pll_est_omega_hz;        /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
float       com_f4_pll_est_zeta;            /* Damping ratio for rotor position Phase-Locked Loop */

/* Sensor-less switch control */
uint8_t     com_u1_flag_less_switch_use;    /* Flags whether use sensor-less switch control*/
float       com_f4_switch_phase_err_deg;    /* Phase error to decide sensor-less switch timing [deg] */
float       com_f4_opl2less_sw_time;        /* Time to switch open-loop to sensor-less [s] */
float       com_f4_phase_err_lpf_cut_freq;  /* Cut off frequency[Hz] of phase error LPF */

/* Open-loop damping control */
uint8_t     com_u1_flag_openloop_damping_use;   /* Flags whether use open-loop damping control */
float       com_f4_ed_hpf_omega;                /* HPF cutoff frequency for ed [Hz] */
float       com_f4_ol_damping_zeta;             /* Damping ratio of open-loop damping control */
float       com_f4_ol_damping_fb_limit_rate;    /* Rate of reference speed for feedback speed limiter */

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
static uint8_t  s_u1_cnt_ics = 0;           /* Counter for period of calling "scope_watchpoint" */
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
    /* Operation variables */
    com_u1_system_mode  = STATEMACHINE_EVENT_STOP;
    g_u1_system_mode    = STATEMACHINE_EVENT_STOP;
    com_u1_enable_write = 0;
    g_u1_enable_write   = 0;
    com_u1_sw_userif    = APP_CFG_USE_UI;
    g_u1_sw_userif      = APP_CFG_USE_UI;

    /* Offset parameters */
    com_u2_offset_calc_time = CURRENT_CFG_OFFSET_CALC_TIME;

    /* Motor parameters */
    com_u2_mtr_pp              = MOTOR_CFG_POLE_PAIRS(0);
    com_f4_mtr_r               = MOTOR_CFG_RESISTANCE(0);
    com_f4_mtr_ld              = MOTOR_CFG_D_INDUCTANCE(0);
    com_f4_mtr_lq              = MOTOR_CFG_Q_INDUCTANCE(0);
    com_f4_mtr_m               = MOTOR_CFG_MAGNETIC_FLUX(0);
    com_f4_mtr_j               = MOTOR_CFG_ROTOR_INERTIA(0);
    com_f4_nominal_current_rms = MOTOR_CFG_NOMINAL_CURRENT_RMS(0);
    com_f4_max_speed_rpm       = MOTOR_CFG_MAX_SPEED_RPM(0);

    /* Sensorless parameter */
    com_u1_ctrl_loop_mode    = MOTOR_COMMON_CFG_LOOP_MODE;
    com_f4_ol_ref_id         = CIRC_CURRENT_CFG_REF_ID_OPENLOOP; // just for compiler reasons
    com_f4_id_up_time        = CURRENT_CFG_ID_UP_STEP_TIME;
    com_f4_id_down_time      = CURRENT_CFG_ID_DOWN_STEP_TIME;
    com_f4_id_down_speed_rpm = SENSORLESS_VECTOR_ID_DOWN_SPEED_RPM(0);
    com_f4_id_up_speed_rpm   = SENSORLESS_VECTOR_ID_UP_SPEED_RPM;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    com_s2_difference_minimum = CURRENT_CFG_MIN_DIFFERENCE_DUTY;
    com_s2_adjust_adc_delay   = CURRENT_CFG_ADJUST_ADC_DELAY;
#endif

    /* Control parameters */
    /* Current control */
    com_f4_current_omega_hz = CURRENT_CFG_OMEGA;
    com_f4_current_zeta     = CURRENT_CFG_ZETA;

    /* Speed control */
    com_f4_speed_omega_hz       = SPEED_CFG_OMEGA(0);
    com_f4_speed_zeta           = SPEED_CFG_ZETA;
    com_f4_speed_lpf_hz         = SPEED_CFG_LPF_OMEGA;
    com_f4_ref_speed_rpm        = 0.0f;
    com_f4_speed_rate_limit_rpm = SPEED_CFG_RATE_LIMIT_RPM;
    com_f4_overspeed_limit_rpm  = SPEED_CFG_SPEED_LIMIT_RPM;

    /* Optional functions */
    /* Voltage error compensation */
    com_u1_flag_volt_err_comp_use = CURRENT_CFG_VOLT_ERR_COMP;

    /* Flux-weakening control */
    com_u1_flag_fluxwkn_use = SPEED_CFG_FLUX_WEAKENING;

    /* BEMF observer */
    com_f4_e_obs_omega_hz   = CURRENT_CFG_E_OBS_OMEGA(0);
    com_f4_e_obs_zeta       = CURRENT_CFG_E_OBS_ZETA;
    com_f4_pll_est_omega_hz = CURRENT_CFG_PLL_EST_OMEGA(0);
    com_f4_pll_est_zeta     = CURRENT_CFG_PLL_EST_ZETA;

    /* Sensor-less switch control */
    com_u1_flag_less_switch_use   = SPEED_CFG_LESS_SWITCH;
    com_f4_switch_phase_err_deg   = SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_DEG;
    com_f4_opl2less_sw_time       = SPEED_OPL2LESS_SWITCH_TIME;
    com_f4_phase_err_lpf_cut_freq = SENSORLESS_VECTOR_OPL2LESS_SWITCH_PHASE_ERR_LPF_CUT_FREQ;

    /* Open-loop damping control */
    com_u1_flag_openloop_damping_use = SPEED_CFG_OPENLOOP_DAMPING;
    com_f4_ed_hpf_omega              = SPEED_OPL_DAMP_ED_HPF_OMEGA;
    com_f4_ol_damping_zeta           = SPEED_OPL_DAMP_ZETA;
    com_f4_ol_damping_fb_limit_rate  = SPEED_OPL_DAMP_FB_SPEED_LIMIT_RATE;
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
        g_st_rmw_input_buffer.f4_ol_ref_id         = com_f4_ol_ref_id;
        g_st_rmw_input_buffer.f4_id_up_time        = com_f4_id_up_time;
        g_st_rmw_input_buffer.f4_id_down_time      = com_f4_id_down_time;
        g_st_rmw_input_buffer.f4_id_down_speed_rpm = com_f4_id_down_speed_rpm;
        g_st_rmw_input_buffer.f4_id_up_speed_rpm   = com_f4_id_up_speed_rpm;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
        g_st_rmw_input_buffer.s2_difference_minimum = com_s2_difference_minimum;
        g_st_rmw_input_buffer.s2_adjust_adc_delay   = com_s2_adjust_adc_delay;
#endif

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

        /* BEMF observer */
        g_st_rmw_input_buffer.f4_e_obs_omega_hz   = com_f4_e_obs_omega_hz;
        g_st_rmw_input_buffer.f4_e_obs_zeta       = com_f4_e_obs_zeta;
        g_st_rmw_input_buffer.f4_pll_est_omega_hz = com_f4_pll_est_omega_hz;
        g_st_rmw_input_buffer.f4_pll_est_zeta     = com_f4_pll_est_zeta;

        /* Sensor-less switch control */
        g_st_rmw_input_buffer.u1_flag_less_switch_use   = com_u1_flag_less_switch_use;
        g_st_rmw_input_buffer.f4_switch_phase_err_deg   = com_f4_switch_phase_err_deg;
        g_st_rmw_input_buffer.f4_opl2less_sw_time       = com_f4_opl2less_sw_time;
        g_st_rmw_input_buffer.f4_phase_err_lpf_cut_freq = com_f4_phase_err_lpf_cut_freq;

        /* Open-loop damping control */
        g_st_rmw_input_buffer.u1_flag_openloop_damping_use = com_u1_flag_openloop_damping_use;
        g_st_rmw_input_buffer.f4_ed_hpf_omega              = com_f4_ed_hpf_omega;
        g_st_rmw_input_buffer.f4_ol_damping_zeta           = com_f4_ol_damping_zeta;
        g_st_rmw_input_buffer.f4_ol_damping_fb_limit_rate  = com_f4_ol_damping_fb_limit_rate;

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
        if (MAIN_UI_RMW == g_u1_sw_userif)
        {
            r_app_rmw_update_command();
        }
    }
} /* End of function r_app_rmw_interrupt_handler */

/***********************************************************************************************************************
* Function Name : r_app_rmw_system_mode
* Description   : Process system mode command
* Arguments     : None
* Return Value  : None
************************************************************************************************************************/
static void r_app_rmw_system_mode(void)
{
    uint8_t u1_temp;
    uint8_t u1_motor_status;

    /*============================*/
    /*        Execute event       */
    /*============================*/
    u1_temp = com_u1_system_mode;

    if (g_u1_system_mode != u1_temp)
    {
        if (u1_temp >= STATEMACHINE_EVENT_SIZE)
        {
            com_u1_system_mode = g_u1_system_mode;
        }
        else
        {
            g_u1_system_mode = u1_temp;
            switch (g_u1_system_mode)
            {
                case STATEMACHINE_EVENT_STOP:
                    R_MOTOR_SENSORLESS_VECTOR_MotorStop(&g_st_sensorless_vector);
                break;

                case STATEMACHINE_EVENT_RUN:
                    R_MOTOR_SENSORLESS_VECTOR_MotorStart(&g_st_sensorless_vector);
                break;

                case STATEMACHINE_EVENT_RESET:
                    R_MOTOR_SENSORLESS_VECTOR_MotorReset(&g_st_sensorless_vector);
                break;

                default:
                    /* Do nothing */
                break;
            }
        }
    }

    u1_motor_status = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);
                                                                        /* Get status of motor control system */

    if (STATEMACHINE_EVENT_RESET == g_u1_system_mode)
    {
        if (STATEMACHINE_STATE_STOP == u1_motor_status)
        {
            r_app_rmw_ui_init();                            /* Initialize private global variables for reset event */
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
    st_rmw_param_buffer_t *p_st_rmw_input = &g_st_rmw_input_buffer;

    st_current_cfg_t           st_cur_cfg;
    st_bemf_observer_cfg_t     st_bemf_obs_cfg;
    st_speed_cfg_t             st_spd_cfg;
    st_sensorless_vector_cfg_t st_sensorless_vector_cfg;

    /* FOC configuration set */
    st_sensorless_vector_cfg.u1_flag_less_switch_use      = p_st_rmw_input->u1_flag_less_switch_use;
    st_sensorless_vector_cfg.u1_flag_openloop_damping_use = p_st_rmw_input->u1_flag_openloop_damping_use;
    st_sensorless_vector_cfg.f4_overspeed_limit_rpm       = p_st_rmw_input->f4_overspeed_limit_rpm;
    st_sensorless_vector_cfg.f4_switch_phase_err_deg      = p_st_rmw_input->f4_switch_phase_err_deg;
    st_sensorless_vector_cfg.f4_phase_err_lpf_cut_freq    = p_st_rmw_input->f4_phase_err_lpf_cut_freq;
    st_sensorless_vector_cfg.f4_id_down_speed_rpm         = p_st_rmw_input->f4_id_down_speed_rpm;
    st_sensorless_vector_cfg.f4_id_up_speed_rpm           = p_st_rmw_input->f4_id_up_speed_rpm;
    st_sensorless_vector_cfg.f4_ctrl_period               = SPEED_CFG_CTRL_PERIOD;
    st_sensorless_vector_cfg.st_motor                     = p_st_rmw_input->st_motor;
    R_MOTOR_SENSORLESS_VECTOR_ParameterUpdate(&g_st_sensorless_vector, &st_sensorless_vector_cfg);

    /* Commands */
    R_MOTOR_SENSORLESS_VECTOR_CtrlTypeSet(&g_st_sensorless_vector, p_st_rmw_input->u1_ctrl_loop_mode);
    R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, p_st_rmw_input->f4_ref_speed_rpm);

    /* Current configuration set */
    st_cur_cfg.u1_flag_volt_err_comp_use = p_st_rmw_input->u1_flag_volt_err_comp_use;
    st_cur_cfg.u2_offset_calc_time       = p_st_rmw_input->u2_offset_calc_time;
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    st_cur_cfg.s2_difference_minimum     = p_st_rmw_input->s2_difference_minimum;
    st_cur_cfg.s2_adjust_adc_delay       = p_st_rmw_input->s2_adjust_adc_delay;
#endif
    st_cur_cfg.f4_ctrl_period            = MOTOR_COMMON_CTRL_PERIOD;
    st_cur_cfg.f4_current_omega_hz       = p_st_rmw_input->f4_current_omega_hz;
    st_cur_cfg.f4_current_zeta           = p_st_rmw_input->f4_current_zeta;
    st_cur_cfg.f4_id_up_step             = p_st_rmw_input->f4_ol_ref_id / p_st_rmw_input->f4_id_up_time;
    st_cur_cfg.f4_id_down_step           = p_st_rmw_input->f4_ol_ref_id / p_st_rmw_input->f4_id_down_time;
    st_cur_cfg.f4_iq_down_step_time_inv  = 1.0f / p_st_rmw_input->f4_id_up_time;
    st_cur_cfg.f4_ol_ref_id              = p_st_rmw_input->f4_ol_ref_id;
    st_cur_cfg.st_motor                  = p_st_rmw_input->st_motor;
    R_MOTOR_CURRENT_ParameterUpdate(&g_st_cc, &st_cur_cfg);

    /* BEMF observer */
    st_bemf_obs_cfg.f4_e_obs_omega_hz = p_st_rmw_input->f4_e_obs_omega_hz;
    st_bemf_obs_cfg.f4_e_obs_zeta = p_st_rmw_input->f4_e_obs_zeta;
    st_bemf_obs_cfg.f4_pll_est_omega_hz = p_st_rmw_input->f4_pll_est_omega_hz;
    st_bemf_obs_cfg.f4_pll_est_zeta = p_st_rmw_input->f4_pll_est_zeta;
    R_MOTOR_CURRENT_BEMFObserverParameterUpdate(&g_st_cc, &st_bemf_obs_cfg);

    /* Speed configuration set */
    st_spd_cfg.u1_flag_fluxwkn_use         = p_st_rmw_input->u1_flag_fluxwkn_use;
    st_spd_cfg.f4_speed_ctrl_period        = SPEED_CFG_CTRL_PERIOD;
    st_spd_cfg.f4_max_speed_rpm            = p_st_rmw_input->f4_max_speed_rpm;
    st_spd_cfg.f4_speed_rate_limit_rpm     = p_st_rmw_input->f4_speed_rate_limit_rpm;
    st_spd_cfg.f4_speed_omega_hz           = p_st_rmw_input->f4_speed_omega_hz;
    st_spd_cfg.f4_speed_zeta               = p_st_rmw_input->f4_speed_zeta;
    st_spd_cfg.f4_speed_lpf_hz             = p_st_rmw_input->f4_speed_lpf_hz;
    st_spd_cfg.f4_opl2less_sw_time         = p_st_rmw_input->f4_opl2less_sw_time;
    st_spd_cfg.f4_ed_hpf_omega             = p_st_rmw_input->f4_ed_hpf_omega;
    st_spd_cfg.f4_ol_damping_zeta          = p_st_rmw_input->f4_ol_damping_zeta;
    st_spd_cfg.f4_ol_damping_fb_limit_rate = p_st_rmw_input->f4_ol_damping_fb_limit_rate;
    st_spd_cfg.f4_ol_ref_id                = p_st_rmw_input->f4_ol_ref_id;
    st_spd_cfg.f4_id_down_speed_rpm        = p_st_rmw_input->f4_id_down_speed_rpm;
    st_spd_cfg.st_motor                    = p_st_rmw_input->st_motor;
    R_MOTOR_SPEED_ParameterUpdate(&g_st_sc, &st_spd_cfg);
} /* End of function r_app_rmw_update_params */

/***********************************************************************************************************************
* Function Name : r_app_rmw_update_command
* Description   : Update commands without copying to buffer or update trigger
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_rmw_update_command(void)
{
    if (MOTOR_LOOP_SPEED == g_st_sensorless_vector.u1_ctrl_loop_mode)
    {
        R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, com_f4_ref_speed_rpm);
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function r_app_rmw_update_command */
