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
* File Name   : r_mtr_ics.h
* Description : Definitions of user interface using ICS
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MTR_ICS_H
#define R_MTR_ICS_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_sensorless_vector_api.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    /* Offset parameters */
    uint16_t    u2_offset_calc_time;        /* calculation time for current offset */
    /* Charge Bootstrap parameters */
    uint16_t    u2_charge_bootstrap_time;   /* Charge time for bootstrap circuit */

    /* Motor parameters */
    st_motor_parameter_t   st_motor;        /* motor parameters */
    float       f4_max_speed_rpm;           /* maximum speed [rpm] (mechanical) */

    /* Sensorless parameter */
    uint8_t     u1_ctrl_loop_mode;          /* loop mode select */
    float       f4_ol_ref_id;               /* id reference when open loop [A] */
    float       f4_id_up_time;              /* time to increase id */
    float       f4_id_down_time;            /* time to decrease id */
    float       f4_id_down_speed_rpm;       /* The speed threshold[rpm] to ramp down the d-axis current */
    float       f4_id_up_speed_rpm;         /* The speed threshold[rpm] to ramp up the d-axis current */

    /* Control parameters */
    /* Current control */
    float       f4_current_omega_hz;        /* Natural frequency[Hz] for current loop gain design */
    float       f4_current_zeta;            /* Damping ratio for current loop gain design */

    /* Speed control */
    float       f4_speed_omega_hz;          /* Natural frequency[Hz] for speed loop gain design */
    float       f4_speed_zeta;              /* Damping ratio for speed loop gain design */
    float       f4_speed_lpf_hz;            /* Natural frequency for speed LPF [Hz] */
    float       f4_ref_speed_rpm;           /* reference speed [rpm] (mechanical) */
    float       f4_speed_rate_limit_rpm;    /* reference speed max change limit [rpm/s] */
    float       f4_overspeed_limit_rpm;     /* over speed limit [rpm] (mechanical) */

    /* Optional functions */
    uint8_t     u1_flag_volt_err_comp_use;       /* voltage error compensation */
    uint8_t     u1_flag_fluxwkn_use;             /* Flux-weakening control */
    uint8_t     u1_flag_mtpa_use;                /* MTPA control */
    uint8_t     u1_flag_flying_start_use;        /* Flying Start control */
    uint8_t     u1_flag_stall_detection_use;     /* Stall Detection control */
    uint8_t     u1_flag_trq_vibration_comp_use;  /* Torque Vibration Compensation control */
    uint8_t     u1_flag_trq_vibration_comp_mode; /* Generation method of Compensation signal, LUT: 0, PAT: 1 */

    /* BEMF observer */
    float       f4_e_obs_omega_hz;          /* Natural frequency for BEMF observer [Hz] */
    float       f4_e_obs_zeta;              /* Damping ratio for BEMF observer */
    float       f4_pll_est_omega_hz;        /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
    float       f4_pll_est_zeta;            /* Damping ratio for rotor position Phase-Locked Loop */

    /* Sensor-less switch control */
    uint8_t     u1_flag_less_switch_use;    /* Flags whether use sensor-less switch control */
    float       f4_switch_phase_err_deg;    /* Phase error to decide sensor-less switch timing [deg] */
    float       f4_opl2less_sw_time;        /* Time to switch open-loop to sensor-less [s] */
    float       f4_phase_err_lpf_cut_freq;  /* Cut off frequency[Hz] of phase error LPF */

    /* Open-loop damping control */
    uint8_t     u1_flag_openloop_damping_use;   /* Flags whether use open-loop damping control */
    float       f4_ed_hpf_omega;                /* HPF cutoff frequency for ed [Hz] */
    float       f4_ol_damping_zeta;             /* Damping ratio of open-loop damping control */
    float       f4_ol_damping_fb_limit_rate;    /* Rate of reference speed for feedback speed limiter */

    /* Stall detection */
    float       f4_id_hpf_time;             /* d-axis HPF time constant [s] */
    float       f4_iq_hpf_time;             /* d-axis HPF time constant [s] */
    float       f4_threshold_level;         /* stall detection threshold [A] */
    float       f4_threshold_time;          /* detection time [s] */

    /* Torque vibration suppression */
    uint8_t     u1_target_2f;                   /* Wheter to include 2F for Target Vibration Component */
    float       f4_timelead_1f;                 /* Output phase index of Torque Vibration Compensation for 1f */
    float       f4_timelead_2f;                 /* Output phase index of Torque Vibration Compensation for 2f */
    float       f4_tf_lpf_omega;                /* natural frequency for LPF in TF [Hz] */
    float       f4_output_gain_1f;              /* Output gain (speed deviation/vibration torque conversion factor) for 1f */
    float       f4_output_gain_2f;              /* Output gain (speed deviation/vibration torque conversion factor) fpr 2f */
    float       f4_input_weight2;               /* coefficient for moving average filter */
    float       f4_input_weight1;               /* coefficient for moving average filter */
    float       f4_input_weight0;               /* coefficient for moving average filter */
    float       f4_suppression_th_1f;           /* The threshold of learning off for 1f : The ratio of amplitude before and after suppression */
    float       f4_suppression_th_2f;           /* The threshold of learning off for 2f : The ratio of amplitude before and after suppression */
    float       f4_abnormal_output_th_1f;       /* The threshold of learning off for 1f : The output abnormality of Tracking filter */
    float       f4_abnormal_output_th_2f;       /* The threshold of learning off for 2f : The output abnormality of Tracking filter */

    /* Flying start */
    float       f4_restart_speed;           /* Restart judgment speed reference value */
    float       f4_off_time;                /* Short-circuit OFF time [s] */
    float       f4_over_time;               /* Detection excess time (cycle conversion) */
    float       f4_active_brake_time;       /* Active braking time */
    float       f4_on_current_th;           /* Current threshold of Short-circuit ON */
} st_rmw_param_buffer_t;

extern st_rmw_param_buffer_t   g_st_rmw_input_buffer;      /* Structure for ICS input */

/***********************************************************************************************************************
* External global variables
***********************************************************************************************************************/
/* Operation variables */
extern uint8_t      com_u1_system_mode;             /* System mode */
extern uint8_t      g_u1_system_mode;               /* System mode */
extern uint8_t      com_u1_enable_write;            /* ICS write enable flag */
extern uint8_t      g_u1_enable_write;              /* ICS write enable flag */

/* Offset parameters */
extern uint16_t     com_u2_offset_calc_time;        /* current offset calculation time */

/* Charge Bootstrap parameters */
extern uint16_t     com_u2_charge_bootstrap_time;   /* Charge time for bootstrap circuit */

/* Motor parameters */
extern uint16_t     com_u2_mtr_pp;                  /* pole pairs */
extern float        com_f4_mtr_r;                   /* resistance [ohm] */
extern float        com_f4_mtr_ld;                  /* d-axis inductance [H] */
extern float        com_f4_mtr_lq;                  /* q-axis inductance [H] */
extern float        com_f4_mtr_m;                   /* permanent magnetic flux [Wb] */
extern float        com_f4_mtr_j;                   /* rotor inertia [kgm^2] */
extern float        com_f4_nominal_current_rms;     /* The nominal current[Arms] */
extern float        com_f4_max_speed_rpm;           /* maximum speed [rpm] (mechanical) */

/* Sensorless parameter */
extern uint8_t      com_u1_ctrl_loop_mode;          /* loop mode select */
extern float        com_f4_ol_ref_id;               /* id reference when open loop [A] */
extern float        com_f4_id_up_time;              /* time to increase id */
extern float        com_f4_id_down_time;            /* time to decrease id */
extern float        com_f4_id_down_speed_rpm;       /* The speed threshold[rpm] to ramp down the d-axis current */
extern float        com_f4_id_up_speed_rpm;         /* The speed threshold[rpm] to ramp up the d-axis current */

/* Control parameters */
/* Current control */
extern float        com_f4_current_omega_hz;        /* natural frequency for current loop [Hz] */
extern float        com_f4_current_zeta;            /* damping ratio for current loop */

/* Speed control */
extern float        com_f4_speed_omega_hz;          /* natural frequency for speed loop [Hz] */
extern float        com_f4_speed_zeta;              /* damping ratio for speed loop */
extern float        com_f4_speed_lpf_hz;            /* Natural frequency for speed LPF [Hz] */
extern float        com_f4_ref_speed_rpm;           /* motor speed reference [rpm] (mechanical) */
extern float        com_f4_speed_rate_limit_rpm;    /* limit of speed change [rpm/s] */
extern float        com_f4_overspeed_limit_rpm;     /* over speed limit [rpm] (mechanical) */

/* Optional functions */
/* Voltage error compensation */
extern uint8_t      com_u1_flag_volt_err_comp_use;  /* Flags whether use voltage error compensation */

/* Flux-weakening control */
extern uint8_t      com_u1_flag_fluxwkn_use;        /* Flags whether use flux-weakening */

/* MTPA control */
extern uint8_t      com_u1_flag_mtpa_use;               /* Flags whether use MTPA */

/* Flying Start control */
extern uint8_t      com_u1_flag_flying_start_use;       /* Flags whether use flying start */

/* Stall Detection control */
extern uint8_t      com_u1_flag_stall_detection_use;    /* Flags whether use stall Detection */

/* Torque Vibration Compensation control */
extern uint8_t      com_u1_flag_trq_vibration_comp_use;  /* Flags whether use torque vibration compensation */
extern uint8_t      com_u1_flag_trq_vibration_comp_mode; /* Generation method of Compensation signal, LUT: 0, PAT: 1 */

/* BEMF observer */
extern float        com_f4_e_obs_omega_hz;          /* Natural frequency for BEMF observer [Hz] */
extern float        com_f4_e_obs_zeta;              /* Damping ratio for BEMF observer */
extern float        com_f4_pll_est_omega_hz;        /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
extern float        com_f4_pll_est_zeta;            /* Damping ratio for rotor position Phase-Locked Loop */

/* Sensor-less switch control */
extern uint8_t      com_u1_flag_less_switch_use;    /* Flags whether use sensor-less switch control */
extern float        com_f4_switch_phase_err_deg;    /* Phase error to decide sensor-less switch timing [deg] */
extern float        com_f4_opl2less_sw_time;        /* Time to switch open-loop to sensor-less [s] */
extern float        com_f4_phase_err_lpf_cut_freq;  /* Cut off frequency[Hz] of phase error LPF */

/* Open-loop damping control */
extern uint8_t      com_u1_flag_openloop_damping_use;   /* Flags whether use open-loop damping control */
extern float        com_f4_ed_hpf_omega;                /* HPF cutoff frequency for ed [Hz] */
extern float        com_f4_ol_damping_zeta;             /* Damping ratio of open-loop damping control */
extern float        com_f4_ol_damping_fb_limit_rate;    /* Rate of reference speed for feedback speed limiter */

/* Stall detection */
extern float       com_f4_id_hpf_time;              /* d-axis HPF time constant [s] */
extern float       com_f4_iq_hpf_time;              /* d-axis HPF time constant [s] */
extern float       com_f4_threshold_level;          /* stall detection threshold [A] */
extern float       com_f4_threshold_time;           /* detection time [s] */

/* Torque vibration suppression */
extern float       com_f4_tf_lpf_omega;                 /* natural frequency for LPF in TF [Hz] */
extern float       com_f4_output_gain_1f;               /* Output gain (speed deviation/vibration torque conversion factor) for 1f */
extern float       com_f4_output_gain_2f;               /* Output gain (speed deviation/vibration torque conversion factor) for 2f */
extern float       com_f4_timelead_1f;                  /* Output phase index of Torque Vibration Compensation for 1f */
extern float       com_f4_timelead_2f;                  /* Output phase index of Torque Vibration Compensation for 1f */
extern float       com_f4_input_weight2;                /* coefficient for moving average filter */
extern float       com_f4_input_weight1;                /* coefficient for moving average filter */
extern float       com_f4_input_weight0;                /* coefficient for moving average filter */
extern float       com_f4_suppression_th_1f;            /* The threshold of learning off for 1f : The ratio of amplitude before and after suppression */
extern float       com_f4_suppression_th_2f;            /* The threshold of learning off for 2f : The ratio of amplitude before and after suppression */
extern float       com_f4_abnormal_output_th_1f;        /* The threshold of learning off for 1f : The output abnormality of Tracking filter */
extern float       com_f4_abnormal_output_th_2f;        /* The threshold of learning off for 2f : The output abnormality of Tracking filter */

/* Flying start */
extern float       com_f4_restart_speed;            /* Restart judgment speed reference value */
extern float       com_f4_off_time;                 /* Short-circuit OFF time [s] */
extern float       com_f4_over_time;                /* Detection excess time */
extern float       com_f4_active_brake_time;        /* Active braking time */
extern float       com_f4_on_current_th;            /* Current threshold of Short-circuit ON */

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void r_app_rmw_copy_com_to_buffer(void);
void r_app_rmw_ui_init(void);
void r_app_rmw_ui_mainloop(void);
void r_app_rmw_interrupt_handler(void);

#endif /* R_MTR_ICS_H */
