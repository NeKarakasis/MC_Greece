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
* File Name   : r_motor_current_api.h
* Description : Definitions of current control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_API_H
#define R_MOTOR_CURRENT_API_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_pi_control.h"
#include "r_motor_current_modulation.h"
#include "r_motor_current_volt_err_comp.h"
#include "r_motor_current_bemf_observer.h"
#include "r_motor_current_stall_detection.h"
#include "r_motor_current_trq_vib_comp.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Id reference status */
#define     CURRENT_STATE_ID_ZERO_CONST         (0)
#define     CURRENT_STATE_ID_INPUT              (1)
#define     CURRENT_STATE_ID_MANUAL             (2)
#define     CURRENT_STATE_ID_UP                 (3)
#define     CURRENT_STATE_ID_DOWN               (4)
/* Iq reference status */
#define     CURRENT_STATE_IQ_ZERO_CONST         (0)
#define     CURRENT_STATE_IQ_SPEED_PI_OUTPUT    (1)
#define     CURRENT_STATE_IQ_AUTO_ADJ           (2)
#define     CURRENT_STATE_IQ_DOWN               (3)

#define     CURRENT_VERR_COMP_LIMIT             (MOTOR_MCU_CFG_CARRIER_FREQ * INVERTER_CFG_DEADTIME / 1000.0f)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
typedef struct
{
    uint8_t              u1_flag_volt_err_comp_use;         /* Voltage error compensation use flag */
    uint8_t              u1_flag_stall_detection_use;       /* Stall Detection use flag */
    uint8_t              u1_flag_trq_vibration_comp_use;    /* Torque vibration compensation use flag */
    uint8_t              u1_flag_trq_vibration_comp_mode;   /* Generation method of Compensation signal, LUT: 0, PAT: 1 */
    uint8_t              u1_target_2f;                      /* Wheter to include 2F for Target Vibration Component */
    uint16_t             u2_offset_calc_time;               /* Offset calculation time */
    uint16_t             u2_charge_bootstrap_time;          /* Charge time for bootstrap circuit */
    float                f4_ctrl_period;                    /* The control period */
    float                f4_current_omega_hz;               /* Current PI control omega */
    float                f4_current_zeta;                   /* Current PI control zeta */
    float                f4_id_up_step;                     /* Reference d-axis current adding value [A] */
    float                f4_id_down_step;                   /* Reference d-axis current subtracting value [A] */
    float                f4_iq_down_step_time_inv;          /* Inverse number of time to decrease iq before open-loop */
    float                f4_ol_ref_id;                      /* d-axis current command value in open loop mode [A] */
    float                f4_id_hpf_time;                    /* Detection frequency range of Stall Detection for d axis */
    float                f4_iq_hpf_time;                    /* Detection frequency range of Stall Detection for q axis */
    float                f4_threshold_level;                /* Threshold Id of Stall Detection */
    float                f4_threshold_time;                 /* Threshold LPF time of Stall Detection */
    float                f4_timelead_1f;                    /* Output phase of Torque Vibration Compensation for 1f */
    float                f4_timelead_2f;                    /* Output phase of Torque Vibration Compensation for 2f */
    float                f4_tf_lpf_omega;                   /* natural frequency for LPF in TF [Hz] */
    float                f4_output_gain_1f;                 /* Gain of Torque Vibration Compensation for 1f */
    float                f4_output_gain_2f;                 /* Gain of Torque Vibration Compensation for 2f */
    float                f4_input_weight2;                  /* Input weight 2 of Repetitive table */
    float                f4_input_weight1;                  /* Input weight 1 of Repetitive table */
    float                f4_input_weight0;                  /* Input weight 0 of Repetitive table */
    float                f4_suppression_th_1f;              /* The threshold of learning off for 1f : The ratio of amplitude before and after suppression */
    float                f4_suppression_th_2f;              /* The threshold of learning off for 2f : The ratio of amplitude before and after suppression */
    float                f4_abnormal_output_th_1f;          /* The threshold of learning off for 1f : The output abnormality of Tracking filter */
    float                f4_abnormal_output_th_2f;          /* The threshold of learning off for 2f : The output abnormality of Tracking filter */
    st_motor_parameter_t st_motor;                          /* Motor parameter structure */
} st_current_cfg_t;

typedef struct
{
    uint8_t u1_state_id_ref;                                /* The d-axis current command status */
    uint8_t u1_state_iq_ref;                                /* The q-axis current command status */
    float   f4_iu_ad;                                       /* U-phase current value */
    float   f4_iv_ad;                                       /* V-phase current value */
    float   f4_iw_ad;                                       /* W-phase current value */
    float   f4_vdc_ad;                                      /* Bus voltage [V] */
    float   f4_id_ref;                                      /* The reference d-axis current value [A] */
    float   f4_iq_ref;                                      /* The reference q-axis current value [A] */
    float   f4_ol_speed_rad;                                /* Speed value in open loop mode [rad/s] */
    float   f4_ref_speed_rad_ctrl;                          /* command speed value for speed PI control [rad/s] */
} st_current_input_t;

typedef struct
{
    uint8_t u1_flag_offset_calc;                            /* Current offset flag */
    uint8_t u1_flag_charge_bootstrap;                       /* Charge flag of bootstrap circuit */
    float   f4_modu;                                        /* U-phase modulation value */
    float   f4_modv;                                        /* V-phase modulation value */
    float   f4_modw;                                        /* W-phase modulation value */
    float   f4_neutral_duty;                                /* Duty at the time of offset measurement */
    float   f4_va_max;                                      /* Maximum voltage on the dq axis */
    float   f4_ref_id_ctrl;                                 /* The reference d-axis current value [A] */
    float   f4_speed_rad;                                   /* Speed value [rad/s] */
    float   f4_ed;                                          /* d-axis BEMF */
    float   f4_eq;                                          /* q-axis BEMF */
    float   f4_phase_err_rad;                               /* Phase error between the real motor axis and the controlled axis */
} st_current_output_t;

typedef struct
{
    float f4_rotor_angle_rad;                               /* Rotor angle */
    float f4_sin;                                           /* sin */
    float f4_cos;                                           /* cos */
} st_rotor_angle_t;

/* Motor current control parameter structure */
typedef struct
{
    uint8_t               u1_active;                        /* Flag to set active/inactive the speed control */
    uint8_t               u1_flag_volt_err_comp_use;        /* Voltage error compensation use flag */
    uint8_t               u1_flag_stall_detection_use;      /* Stall Detection use flag */
    uint8_t               u1_flag_trq_vibration_comp_use;   /* Torque vibration compensation use flag */
    uint8_t               u1_flag_trq_vibration_comp_mode;  /* Generation method of Compensation signal, LUT: 0, PAT: 1 */
    uint8_t               u1_state_id_ref;                  /* The d-axis current command status */
    uint8_t               u1_state_iq_ref;                  /* The q-axis current command status */
    uint8_t               u1_flag_offset_calc;              /* q-axis current command value management flag */
    uint8_t               u1_flag_charge_bootstrap;         /* Charge flag of bootstrap circuit */
    uint16_t              u2_offset_calc_time;              /* Calculation time for current offset */
    uint16_t              u2_crnt_offset_cnt;               /* The number of samples to be acquired in offset measurement */
    uint16_t              u2_charge_bootstrap_time;         /* Charge time for bootstrap circuit */
    uint16_t              u2_charge_bootstrap_cnt;          /* The number of samples for charging bootstrap circuit */
    float                 f4_ctrl_period;                   /* The control period */
    float                 f4_refu;                          /* U-phase reference voltage value [V] */
    float                 f4_refv;                          /* V-phase reference voltage value [V] */
    float                 f4_refw;                          /* W-phase reference voltage value [V] */
    float                 f4_vd_ref;                        /* The reference d-axis voltage value [V] */
    float                 f4_vq_ref;                        /* The reference q-axis voltage value [V] */
    float                 f4_id_ref;                        /* The reference d-axis current value [A] */
    float                 f4_iq_ref;                        /* The reference q-axis current value [A] */
    float                 f4_id_ad;                         /* The d-axis current value [A] */
    float                 f4_iq_ad;                         /* The q-axis current value [A] */
    float                 f4_lim_iq;                        /* The speed PI control output limit value [A}*/
    float                 f4_offset_iu;                     /* U-phase current offset value [A] */
    float                 f4_offset_iw;                     /* W-phase current offset value [A] */
    float                 f4_offset_iv;
    float                 f4_sum_iu_ad;                     /* U-phase current summation value for offset calculation [A] */
    float                 f4_sum_iw_ad;                     /* W-phase current summation value for offset calculation [A] */
    float                 f4_sum_iv_ad;


    float         f4_iu_ref_ad;
    float         f4_iv_ref_ad;
    float         f4_iw_ref_ad;


    float                 f4_vdc_ad;                        /* Bus voltage [V] */
    float                 f4_iu_ad;                         /* U-phase current value [A] */
    float                 f4_iv_ad;                         /* V-phase current value [A] */
    float                 f4_iw_ad;                         /* W-phase current value [A] */
    float                 f4_modu;                          /* U-phase duty cycle */
    float                 f4_modv;                          /* V-phase duty cycle */
    float                 f4_modw;                          /* W-phase duty cycle */
    float                 f4_speed_rad;                     /* Speed value(electric angle) [rad/s] */
    float                 f4_ref_id_ctrl;                   /* The reference d-axis current value [A] */
    float                 f4_ref_iq_ctrl;                   /* The reference q-axis current value [A] */
    float                 f4_id_up_step;                    /* Reference d-axis current adding value [A] */
    float                 f4_id_down_step;                  /* Reference d-axis current subtracting value [A] */
    float                 f4_iq_down_step;                  /* Reference q-axis current subtracting value [A] */
    float                 f4_iq_down_step_time_inv;         /* Inverse number of time to decrease iq before open-loop */
    float                 f4_ol_ref_id;                     /* d-axis current command value in open loop mode [A] */
    float                 f4_va_max;                        /* The maximum magnitude of voltage vector */
    float                 f4_ed;                            /* d-axis BEMF */
    float                 f4_eq;                            /* q-axis BEMF */
    float                 f4_phase_err_rad;                 /* Phase error between the real motor axis and the controlled axis */
    float                 f4_ol_speed_rad;                  /* Speed value(electric angle) in open loop mode [rad/s] */
    float                 f4_ref_speed_rad_ctrl;            /* command speed value(machine angle) for speed PI control [rad/s] */
    st_mod_t              st_mod;                           /* Modulation structure */
    st_volt_comp_t        st_volt_comp;                     /* Compensates voltage error structure */
    st_bemf_observer_t    st_bemf_observer;                 /* BEMF observer structure */
    st_pll_est_t          st_pll_est;                       /* PLL estimator structure */
    st_pi_ctrl_t          st_pi_id;                         /* The d-axis current PI controller */
    st_pi_ctrl_t          st_pi_iq;                         /* The q-axis current PI controller */
    st_rotor_angle_t      st_rotor_angle;                   /* Rotor angle structure */
    st_rotor_angle_t      st_rotor_angle_phasecomp;         /* Rotor angle structure applied phase compensation */
    st_motor_parameter_t  st_motor;                         /* Motor parameter structure */
    st_stall_detect_t     st_stalldet;                      /* Stall Detection structure */
    st_trqvib_comp_t      st_trqvib_comp;                   /* Torque Vibration compensation structure */
} st_current_control_t;

typedef struct
{
    float f4_e_obs_omega_hz;                                /* Natural frequency for BEMF observer [Hz] */
    float f4_e_obs_zeta;                                    /* Damping ratio for BEMF observer */
    float f4_pll_est_omega_hz;                              /* Natural frequency for rotor position Phase-Locked Loop [Hz] */
    float f4_pll_est_zeta;                                  /* Damping ratio for rotor position Phase-Locked Loop */
} st_bemf_observer_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_current_control_t g_st_cc;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_MOTOR_CURRENT_Open(void);
void R_MOTOR_CURRENT_Close(void);
void R_MOTOR_CURRENT_Reset(st_current_control_t *p_st_cc);
void R_MOTOR_CURRENT_Run(st_current_control_t *p_st_cc);
void R_MOTOR_CURRENT_ParameterSet(st_current_control_t *p_st_cc, const st_current_input_t * p_st_current_input);
void R_MOTOR_CURRENT_ParameterGet(st_current_control_t *p_st_cc, st_current_output_t * p_st_current_output);
void R_MOTOR_CURRENT_ParameterUpdate(st_current_control_t *p_st_cc, const st_current_cfg_t *p_st_current_cfg);
void R_MOTOR_CURRENT_CurrentCyclic(st_current_control_t *p_st_cc);
void R_MOTOR_CURRENT_OffsetCalibration(st_current_control_t *p_st_cc);
void R_MOTOR_CURRENT_CurrentOffsetRemove(st_current_control_t *p_st_cc, float *p_f4_iu, float *p_f4_iv, float *p_f4_iw);
void R_MOTOR_CURRENT_VoltErrCompParamSet(st_current_control_t *p_st_cc,
                                         const float *crnt_array,
                                         const float *volterr_array,
                                         float ref_voltage);
void R_MOTOR_CURRENT_BEMFObserverParameterUpdate(st_current_control_t * p_st_cc,
                                                 st_bemf_observer_cfg_t * p_st_bemf_obs_cfg);
void R_MOTOR_CURRENT_HuntingSuppress(st_current_control_t * p_st_cc, float f4_ref_speed_rad_ctrl);
void R_MOTOR_CURRENT_PLLSpeedSet(st_current_control_t * p_st_cc, float f4_speed_ini_est);
void R_MOTOR_CURRENT_RotorAngleSet(st_current_control_t * p_st_cc, float f4_angle_rad);
void R_MOTOR_CURRENT_RefstateSet(st_current_control_t *p_st_cc, uint8_t u1_state_id_ref, uint8_t u1_state_iq_ref);
void R_MOTOR_CURRENT_BEMFObserverParameterSet(st_current_control_t * p_st_cc, float f4_d_est);
void R_MOTOR_CURRENT_ChargeBootstrap(st_current_control_t *p_st_cc);

#endif /* R_MOTOR_CURRENT_API_H */
