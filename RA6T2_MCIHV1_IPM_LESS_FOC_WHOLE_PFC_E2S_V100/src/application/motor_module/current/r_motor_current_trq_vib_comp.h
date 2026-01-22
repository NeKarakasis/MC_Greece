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
* File Name   : r_motor_current_trq_vib_comp.h
* Description : Definitions of torque vibration compensation
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*           29.02.2024 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_TRQVIB_COMP_H
#define R_MOTOR_CURRENT_TRQVIB_COMP_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define TRQVIB_COMP_ARY_SIZE                (256)

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    uint8_t u1_flag_trqvib_comp_learning;               /* Flags whether learning of Torque Vibration Compensation */
    uint16_t u2_motor_pp;                               /* Motor poles */
    uint16_t u2_k2_timelead;                            /* Output phase index of Torque Vibration Compensation */
    int16_t s2_last_index;                              /* Last current index of Repetitive table */
    int16_t s2_last_index_comp;                         /* Last current index of Compensation */
    float f4_angle_rad_machine;                         /* Motor angle (mechanical) [rad] */
    float f4_angle_electric_last;                       /* Motor angle (electrical) [rad] */
    float f4_repetitive_table[TRQVIB_COMP_ARY_SIZE];    /* Repetitive table */
    float f4_last_spd_ref;                              /* Last Reference speed */
    float f4_last_spd_det;                              /* Last Estimated speed */
    float f4_last_spd_comp;                             /* Last Current Compensation */
    float f4_output_gain;                               /* Gain of Torque Vibration Compensation */
    float f4_input_weight2;                             /* Input weight 2 of Repetitive table */
    float f4_input_weight1;                             /* Input weight 1 of Repetitive table */
    float f4_input_weight0;                             /* Input weight 0 of Repetitive table */

    /* Tracking Filter */
    float f4_tf_lpf_buf_x;                              /* Low Pass Filter buffer X pole of tracking filter */
    float f4_tf_lpf_buf_y;                              /* Low Pass Filter buffer Y pole of tracking filter */
    float f4_tf_lpf_time;                               /* Low Pass Filter time of tracking filter */
    float f4_tf_output;                                 /* Output value of tracking filter */
} st_trqvib_comp_t;

typedef struct
{
    uint8_t u1_flag_trqvib_comp_learning;               /* Flags whether learning of Torque Vibration Compensation */
    uint16_t u2_motor_pp;                               /* Motor poles */
    uint16_t u2_k2_timelead;                            /* Output phase index of Torque Vibration Compensation */
    float f4_output_gain;                               /* Gain of Torque Vibration Compensation */
    float f4_input_weight2;                             /* Input weight 2 of Repetitive table */
    float f4_input_weight1;                             /* Input weight 1 of Repetitive table */
    float f4_input_weight0;                             /* Input weight 0 of Repetitive table */

    /* Tracking Filter */
    float f4_tf_lpf_time;                               /* Low Pass Filter time of tracking filter */
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
                                                     float f4_angle_electric,
                                                     float f4_speed_ref_rad,
                                                     float f4_speed_det_rad);
void motor_current_trq_vibration_compensation_main(st_trqvib_comp_t *p_st_trqvib_comp,
                                                   float f4_speed_ref_rad,
                                                   float f4_speed_det_rad);
void motor_current_trq_vibration_compensation_trackingfilter(st_trqvib_comp_t *p_st_trqvib_comp,
                                                             float f4_input_val,
                                                             float * p_f4_output_val,
                                                             float f4_angle);
float motor_current_trq_vibration_compensation_get_m_angle(st_trqvib_comp_t * st_trqvib_comp,
                                                           float f4_angle_electric);

#endif /* R_MOTOR_CURRENT_TRQVIB_COMP_H */
