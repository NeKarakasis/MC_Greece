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
* File Name   : r_motor_speed_api.h
* Description : Definitions of speed control API
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_API_H
#define R_MOTOR_SPEED_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_filter.h"
#include "r_motor_pi_control.h"
#include "r_motor_speed_fluxwkn.h"
#include "r_motor_speed_mtpa.h"
#include "r_motor_speed_opl_damp_ctrl.h"
#include "r_motor_speed_opl2less.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Speed reference status */
#define     SPEED_STATE_ZERO_CONST         (0)
#define     SPEED_STATE_POS_CONTROL_OUTPUT (1)
#define     SPEED_STATE_MANUAL             (2)

/* Iq reference status */
#define     CURRENT_SPD_STATE_IQ_ZERO_CONST         (0)
#define     CURRENT_SPD_STATE_IQ_SPEED_PI_OUTPUT    (1)
#define     CURRENT_SPD_STATE_IQ_AUTO_ADJ           (2)
#define     CURRENT_SPD_STATE_IQ_DOWN               (3)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    uint8_t       u1_flag_fluxwkn_use;
    uint8_t       u1_flag_mtpa_use;
    float         f4_max_speed_rpm;
    float         f4_speed_ctrl_period;
    float         f4_speed_rate_limit_rpm;
    float         f4_speed_omega_hz;
    float         f4_speed_zeta;
    float         f4_speed_lpf_hz;
    float         f4_opl2less_sw_time;
    float         f4_ed_hpf_omega;
    float         f4_ol_damping_zeta;
    float         f4_ol_damping_fb_limit_rate;
    float         f4_ol_ref_id;
    float         f4_id_down_speed_rpm;
    st_motor_parameter_t st_motor;
} st_speed_cfg_t;

typedef struct
{
    uint8_t       u1_state_speed_ref;
    float         f4_speed_rad;
    float         f4_va_max;
    uint8_t       u1_state_id_ref;      /* The d-axis current command status */
    uint8_t       u1_state_iq_ref;      /* The q-axis current command status */
} st_speed_input_t;

typedef struct
{
    float         f4_id_ref;
    float         f4_iq_ref;
    float         f4_ref_speed_rad_ctrl;
    float         f4_speed_rad_lpf;
} st_speed_output_t;

typedef struct
{
    uint8_t       u1_state_id_ref;                   /* The d-axis current command status */
    uint8_t       u1_state_iq_ref;                   /* The q-axis current command status */    
    uint8_t       u1_active;                         /* Flag to set active/inactive the speed control */
    uint8_t       u1_state_speed_ref;                /* The speed command status */
    uint8_t       u1_flag_fluxwkn_use;               /* whether use the output from flux-weakening */
    uint8_t       u1_flag_switching;                 /* whether set estimated speed flag */
    uint8_t       u1_flag_mtpa_use;                  /* whether use the output from MTPA */
    float         f4_speed_ctrl_period;              /* speed control period */
    float         f4_ref_speed_rad_ctrl;             /* command speed value for speed PI control[rad/s] */
    float         f4_ref_speed_rad;                  /* reference speed value [rad/s] */
    float         f4_ref_speed_rad_manual;           /* reference speed value of speed control mode [rad/s] */
    float         f4_speed_rad_ctrl;                 /* speed_value for speed loop control [rad/s] */
    float         f4_speed_rad;                      /* speed value [rad/s] */
    float         f4_max_speed_rad;                  /* maximum speed command value [rad/s] */
    float         f4_speed_rate_limit_rad;           /* reference speed max change limit [rad/s] */
    float         f4_id_ref_output;                  /* the reference current vector (array) in format : Idref */
    float         f4_iq_ref_output;                  /* the reference current vector (array) in format : Iqref */
    float         f4_va_max;                         /* The maximum magnitude of voltage vector */
    float         f4_id_ad;                          /* The d-axis current value [A] */
    float         f4_iq_ad;                          /* The q-axis current value [A] */
    float         f4_opl2less_sw_time;               /* Time to switch open-loop to sensor-less [s] */
    float         f4_torque_current;                 /* The torque current (Iq) in sensor-less switching */
    st_motor_parameter_t  st_motor;                  /* Motor parameters structure */
    st_pi_ctrl_t          st_pi_speed;               /* PI control structure */
    st_fluxwkn_t          st_fluxwkn;                /* Flux-weakening structure */
    st_opl_damp_t         st_opl_damp;               /* Open loop damping control structure */
    st_1st_order_lpf_t    st_slpf;                   /* LPF for speed */
    st_mtpa_t             st_mtpa;                   /* MTPA structure */
} st_speed_control_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_speed_control_t g_st_sc;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_MOTOR_SPEED_Open(void);
void R_MOTOR_SPEED_Close(void);
void R_MOTOR_SPEED_Reset(st_speed_control_t * p_st_sc);
void R_MOTOR_SPEED_Run(st_speed_control_t * p_st_sc);
void R_MOTOR_SPEED_ParameterSet(st_speed_control_t * p_st_sc, const st_speed_input_t * p_st_speed_input);
void R_MOTOR_SPEED_ParameterGet(st_speed_control_t * p_st_sc, st_speed_output_t * p_st_speed_output);
void R_MOTOR_SPEED_ParameterUpdate(st_speed_control_t * p_st_sc, const st_speed_cfg_t * p_st_speed_cfg);
void R_MOTOR_SPEED_SpdRefSet(st_speed_control_t * p_st_sc, float f4_ref_speed_rpm);
void R_MOTOR_SPEED_SpeedCyclic(st_speed_control_t * p_st_sc);
float R_MOTOR_SPEED_Opl2lessReferenceIqCalc(st_speed_control_t * p_st_sc,
                                            float f4_ed,
                                            float f4_eq,
                                            float f4_id_ref,
                                            float f4_phase_err);
void R_MOTOR_SPEED_Opl2lessPreprocess(st_speed_control_t * p_st_sc,
                                      float f4_id_ref,
                                      float f4_phase_err_rad_lpf);
float R_MOTOR_SPEED_OplDampCtrl(st_speed_control_t * p_st_sc, float f4_ed);
void R_MOTOR_SPEED_OplDampReset(st_speed_control_t * p_st_sc);
void R_MOTOR_SPEED_HuntingSuppress(st_speed_control_t * p_st_sc, float f4_iq_ref);
void R_MOTOR_SPEED_SwitchingFlagSet(st_speed_control_t * p_st_sc);
void R_MOTOR_SPEED_ControlParamSet(st_speed_control_t * p_st_sc, float f4_speed_rad_ctrl);
void R_MOTOR_SPEED_RefstateSet(st_speed_control_t * p_st_sc,
                               uint8_t u1_state_speed_ref,
                               uint8_t u1_state_id_ref,
                               uint8_t u1_state_iq_ref);

#endif /* R_MOTOR_SPEED_API_H */
