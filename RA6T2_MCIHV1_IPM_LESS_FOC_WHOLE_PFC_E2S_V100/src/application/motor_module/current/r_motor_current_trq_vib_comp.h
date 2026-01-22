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
* File Name   : r_motor_current_trq_vib_comp.h
* Description : Definitions of torque vibration compensation
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_TRQVIB_COMP_H
#define R_MOTOR_CURRENT_TRQVIB_COMP_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_module_cfg.h"
/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Polynomial Apploximation order for PAT */
#define N_DIM                               (5)
/* Table size of Repetitive table to Generate Compensation signal by LUT */
#define TRQVIB_COMP_ARY_SIZE                (256)

/* Generation method of Compensation signal, LUT: 0, PAT: 1 */
#define TRQCOMP_MODE_LUT                    (0)
#define TRQCOMP_MODE_PAT                    (1)

/* Torque Compensation state */
#define TRQCOMP_STATE_IDLE                  (0U)
#define TRQCOMP_STATE_1F_STANDBY            (1U)
#define TRQCOMP_STATE_1F_LEARNING           (2U)
#define TRQCOMP_STATE_2F_STANDBY            (3U)
#define TRQCOMP_STATE_2F_LEARNING           (4U)
#define TRQCOMP_STATE_COMPLETE              (5U)

/* Torque Compensation action */
#define TRQCOMP_ACTION_RESET                (0U)
#define TRQCOMP_ACTION_START                (1U)
#define TRQCOMP_ACTION_LEARNING_ON          (2U)
#define TRQCOMP_ACTION_LEARNING_OFF         (3U)

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    uint8_t u1_target_learning_done;                    /* Flag whether learning is done */
    uint8_t u1_target_output_enable;                    /* Compensation signal output enable flag */
    int16_t s2_last_index;                              /* Last current index of Repetitive table */
    uint16_t u2_motor_pp;                               /* Motor pole paires */
    float f4_angle_rad_machine;                         /* Motor angle (mechanical) [rad] */
    float f4_angle_electric_last;                       /* Motor angle (electrical) [rad] */
    float f4_last_spd_comp;                             /* Last Compensation signal [A] */
    float f4_output_gain;                               /* Gain for input to the repetitive control */
    float f4_harmonic_order;                            /* The harmonic order versus the motor's rotational frequency */
    float f4_suppression_th;                            /* The threshold of learning off : The ratio of amplitude before and after suppression */
    float f4_abnormal_output_th;                        /* The threshold of learning off : The output abnormality of Tracking filter */

    float f4_fitting_coeff[N_DIM+1];                    /* Polynomial approximation coefficients */
    float f4_k2_timelead;                               /* Output phase lead index for generating Compensation signal by PAT */
    float f4_last_index_comp;                           /* Last table index for generating Compensation signal by PAT */

    float f4_repetitive_table[TRQVIB_COMP_ARY_SIZE];    /* Repetitive table for LUT */
    uint16_t u2_k2_timelead;                            /* Output phase lead index for generating Compensation signal by LUT */
    int16_t s2_last_index_comp;                         /* Last table index for generating Compensation signal by LUT */

} st_trqvib_comp_target_t;

typedef struct
{
    uint8_t u1_target_2f;                               /* Wheter to include 2F for Target Vibration Component */
    uint8_t u1_flag_trqvib_comp_learning;               /* Flags whether learning of Torque Vibration Compensation */
    uint16_t u2_trqcomp_state;                          /* The state of Torque Vibration Compensation */
    uint16_t u2_trqcomp_action;                         /* The action of Torque Vibration Compensation */

    float f4_repetitive_table[N_DIM+1];                 /* Repetitive table for PAT */

    float f4_last_spd_ref;                              /* Last Reference speed (mechanical) [rad/s] */
    float f4_last_spd_det;                              /* Last Estimated speed (mechanical) [rad/s] */
    float f4_input_weight2;                             /* Input weight 2 of Repetitive table */
    float f4_input_weight1;                             /* Input weight 1 of Repetitive table */
    float f4_input_weight0;                             /* Input weight 0 of Repetitive table */

    /* Tracking Filter (TF) */
    float f4_tf_lpf_buf_x;                              /* TF Internal Signal X (TIS_X) : Low Pass Filter buffer X pole of tracking filter */
    float f4_tf_lpf_buf_y;                              /* TF Internal Signal Y (TIS_Y) : Low Pass Filter buffer Y pole of tracking filter */
    float f4_tf_lpf_time;                               /* Low Pass Filter Parameter of tracking filter : 2PI * Cutt off frequwncy (Hz) * contol perido (s) */
    float f4_tf_output;                                 /* Target Vibration Component (TVC) [rad/s] : Output value of tracking filter */

    /* For learning off : Suppression Goal */
    float f4_tvc_max_1cycle;                             /* Max value of TVC during 1 period [rad/s] */
    float f4_tvc_min_1cycle;                             /* Min value of TVC during 1 period [rad/s] */
    float f4_tvc_max;                                    /* Global Max value of TVC during learning off [rad/s] */
    float f4_tvc_min;                                    /* Global Min value of TVC during learning off [rad/s] */
    float f4_tvc_amp_standby;                            /* Global Max amplitude of TVC during learning off [rad/s] */
    float f4_tvc_amp_standby_pre;                        /* Previous global Max amplitude of TVC during learning off [rad/s] */
    float f4_tvc_amp_learning;                           /* The Amplitude of TVC during learning on [rad/s] */

    /* For learning off : Output Abnormality of TF */
    float f4_tf_lpf_x_dc;                                /* DC component of TF Internal Signal X (TIS_X_DC) for x-axis */
    float f4_tf_lpf_y_dc;                                /* DC component of TF Internal Signal Y (TIS_Y_DC) for y-axis */
    float f4_tf_lpf_x_ac;                                /* AC component of TF Internal Signal X (TIS_X_AC) for x-axis */
    float f4_tf_lpf_y_ac;                                /* AC component of TF Internal Signal Y (TIS_Y_AC) for y-axis */
    float f4_tf_lpf_x_max_1cycle;                        /* Max value of TIS_X during 1 period */
    float f4_tf_lpf_x_min_1cycle;                        /* Min value of TIS_X during 1 period */
    float f4_tf_lpf_y_max_1cycle;                        /* Max value of TIS_Y during 1 period */
    float f4_tf_lpf_y_min_1cycle;                        /* Min value of TIS_Y during 1 period */

    /* Target Vibration Component (TVC) are as follows */
    /* 1F : Fundamental harmonic component of the motor's rotational frequency */
    /* 2F : Second harmonic component of the motor's rotational frequency */
    st_trqvib_comp_target_t st_trqvib_comp_1f;          /* 1F */
    st_trqvib_comp_target_t st_trqvib_comp_2f;          /* 2F */
} st_trqvib_comp_t;

typedef struct
{   
    uint8_t u1_target_2f;                               /* Wheter to include 2F for Target Vibration Component */
    uint16_t u2_motor_pp;                               /* Motor pole paires */
    float f4_output_gain_1f;                            /* Gain of 1F for input to the repetitive control */
    float f4_output_gain_2f;                            /* Gain of 2F for input to the repetitive control */
    float f4_input_weight2;                             /* Input weight 2 of Repetitive table */
    float f4_input_weight1;                             /* Input weight 1 of Repetitive table */
    float f4_input_weight0;                             /* Input weight 0 of Repetitive table */
    float f4_suppression_th_1f;                         /* The threshold of learning off for 1f : The ratio of amplitude before and after suppression */
    float f4_suppression_th_2f;                         /* The threshold of learning off for 2f : The ratio of amplitude before and after suppression */
    float f4_abnormal_output_th_1f;                     /* The threshold of learning off for 1f : The output abnormality of Tracking filter */
    float f4_abnormal_output_th_2f;                     /* The threshold of learning off for 2f : The output abnormality of Tracking filter */
    float f4_tf_lpf_time;                               /* Low Pass Filter Parameter of tracking filter : 2PI * Cutt off frequwncy (Hz) * contol perido (s) */

    float f4_k2_timelead_1f;                            /* Output phase lead index for generating Compensation signal of 1F by PAT */
    float f4_k2_timelead_2f;                            /* Output phase lead index for generating Compensation signal of 2F dby PAT */

    uint16_t u2_k2_timelead_1f;                         /* Output phase lead index for generating Compensation signal of 1F by LUT */
    uint16_t u2_k2_timelead_2f;                         /* Output phase lead index for generating Compensation signal of 2F by LUT */

} st_trqvib_comp_cfg_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void motor_current_trq_vibration_compensation_init(st_trqvib_comp_t *p_st_trqvib_comp,
                                                   st_trqvib_comp_cfg_t *p_st_trqvib_comp_cfg);
void motor_current_trq_vibration_compensation_reset (st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_parameter_set(st_trqvib_comp_t *p_st_trqvib_comp,
                                                            const st_trqvib_comp_cfg_t *p_st_trqvib_comp_cfg);
void motor_current_trq_vibration_compensation_preproc(st_trqvib_comp_t *p_st_trqvib_comp,
                                                     const float f4_angle_electric,
                                                     const float f4_speed_ref_rad,
                                                     const float f4_speed_det_rad);
void motor_current_trq_vibration_compensation_main_LUT(st_trqvib_comp_t *p_st_trqvib_comp,
                                                       st_trqvib_comp_target_t *p_st_trqvib_comp_target,
                                                       const float f4_speed_ref_rad,
                                                       const float f4_speed_det_rad);
void motor_current_trq_vibration_compensation_trackingfilter(st_trqvib_comp_t *p_st_trqvib_comp,
                                                             const float f4_input_val,
                                                             float * p_f4_output_val,
                                                             const float f4_angle);
float motor_current_trq_vibration_compensation_get_m_angle(st_trqvib_comp_target_t * p_st_trqvib_comp_target,
                                                           const float f4_angle_electric);
void motor_current_trq_vibration_compensation_LU_generate(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_main_PAT(st_trqvib_comp_t *p_st_trqvib_comp, st_trqvib_comp_target_t *p_st_trqvib_comp_target);
void motor_current_trq_vibration_compensation_learning_management(st_trqvib_comp_t *p_st_trqvib_comp,
                                                                  const float f4_angle_rad_machine_pre, const float f4_angle_rad_machine);
void motor_current_trq_vibration_compensation_tvc_max_min_1cycle_update(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_tvc_max_min_1cycle_reset(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_auto_learning_on(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_check_suppression_progress(st_trqvib_comp_t *p_st_trqvib_comp, const float f4_tf_target_th);
void motor_current_trq_vibration_compensation_tvc_max_min_reset(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_trackingfilter_reset(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_tf_lpf_dc_calc(st_trqvib_comp_t *p_st_trqvib_comp, 
                                                                    const float f4_angle_rad_machine, const float f4_angle_rad_machine_pre);
void motor_current_trq_vibration_compensation_tf_lpf_dc_reset(st_trqvib_comp_t *p_st_trqvib_comp);
void motor_current_trq_vibration_compensation_tf_lpf_ac_calc(st_trqvib_comp_t *p_st_trqvib_comp, 
                                                                    const float f4_angle_rad_machine, const float f4_angle_rad_machine_pre);
void motor_current_trq_vibration_compensation_check_tf_abnormal_output(st_trqvib_comp_t *p_st_trqvib_comp, const float f4_target_th);
void motor_current_trq_vibration_compensation_abnormal_prevention(st_trqvib_comp_t *p_st_trqvib_comp,
                                                                const float f4_angle_rad_machine_pre, const float f4_angle_rad_machine);
void motor_current_trq_vibration_compensation_status_transision(st_trqvib_comp_t *p_st_trqvib_comp, const uint16_t u2_action);

#endif /* R_MOTOR_CURRENT_TRQVIB_COMP_H */
