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
* File Name   : r_motor_current_stall_detection.h
* Description : Definitions of stall detection
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_STALL_DETECTION_H
#define R_MOTOR_CURRENT_STALL_DETECTION_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
/**
 * Stall detection date structure
 */
typedef struct
{
    float f4_threshold_level;               /* Threshold of Stall Detection */
    float f4_id_hpf_time;                   /* Detection frequency range of Stall Detection for d-axis */
    float f4_iq_hpf_time;                   /* Detection frequency range of Stall Detection for q-axis */
    float f4_threshold_time;                /* Threshold LPF time of Stall Detection */
    float f4_id_current_abs_val;            /* Filtered |Id| value of Stall Detection */
    float f4_iq_current_abs_val;            /* Filtered |Iq| value of Stall Detection */
    uint8_t u1_stall_detected;              /* Stall Detected Flag */
} st_stall_detect_t;

typedef struct
{
    float f4_threshold_level;               /* Threshold of Stall Detection */
    float f4_id_hpf_time;                   /* Detection frequency range of Stall Detection for d-axis */
    float f4_iq_hpf_time;                   /* Detection frequency range of Stall Detection for q-axis */
    float f4_threshold_time;                /* Threshold LPF time of Stall Detection */
} st_stall_detect_cfg_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void motor_current_stall_detection_init(st_stall_detect_t *p_st_stall_det,
                                        const st_stall_detect_cfg_t *p_st_stall_det_cfg);
void motor_current_stall_detection_parameter_set(st_stall_detect_t *p_st_stall_det,
                                                 const st_stall_detect_cfg_t *p_st_stall_det_cfg);
void motor_current_stall_detection_reset(st_stall_detect_t *p_st_stall_det);
void motor_current_stall_detection_main(st_stall_detect_t *p_st_stall_det, float *p_f4_i_array);

#endif /* R_MOTOR_CURRENT_STALL_DETECTION_H */
