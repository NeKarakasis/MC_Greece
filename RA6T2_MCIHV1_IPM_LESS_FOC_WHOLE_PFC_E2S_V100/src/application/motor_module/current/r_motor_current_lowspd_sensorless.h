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
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_current_lowspd_sensorless.h
* Description : Definitions of low speed sonsorless
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 20.12.2022 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_LOWSPD_SENSORLESS_H
#define R_MOTOR_CURRENT_LOWSPD_SENSORLESS_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define     MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE   (8)         /* Table size of pulse injection current pattern */
#define     MOTOR_SENSORLESS_VECTOR_THRESHOLD_HIGHSPEED  55.0//41.87 //400rpm //(54.9779f)  /* 525 r/min -> 525/60 * 2PI =   rad/s */
#define     MOTOR_SENSORLESS_VECTOR_THRESHOLD_LOWSPEED  50.0// 31.41 //300rpm //(49.7419f)  /* 475 r/min -> 475/60 * 2PI =   rad/s */
#define	    MOTOR_SENSORLESS_VECTOR_PF_START_CNT	     (100)       /* Calculation start count of PF(count of CurrentCycles) [2ms] */


/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/

typedef struct
{
    float                   f4_ctrl_period;                   /* Control Period */
    float                   f4_pulse_inj_interval;            /* Pulse Injection Interval for */
                                                              /* position estimator[cnt/pwm interval] */
    float                   f4_ld_pulse;                      /* Ld with pulse interval*/
    float                   f4_lq_pulse;                      /* Ld with pulse interval*/

    /* Pulse Injection */
    float                   f4_volt_pulse_inj;                /* PLL Speed estimate loop */
    float                   f4_vd_ref_pulse_inj;              /* Injection Vd ref*/
    uint8_t                 u1_pulse_freq;                    /* Pulse Injection Frequency [cnt] */
    uint8_t                 u1_pulse_cnt;                     /* Pulse Injection counter */
    uint8_t                 u1_pulse_sign;                    /* Pulse Injection sign */

    /* PF */
    float                   f4_PF;                            /* PF */
    float                   f4_array_id_p[MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE];
    float                   f4_array_id_n[MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE];
    float                   f4_array_iq_p[MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE];
    float                   f4_array_iq_n[MOTOR_SENSORLESS_VECTOR_CURRENT_TABLE_SIZE];
    float                   f4_id_peak_p;
    float                   f4_id_peak_n;
    float                   f4_id_ad_ave;                     /* Output of Idc_FB */
    float                   f4_iq_ad_ave;                     /* Output of Iqc_FB */
    float                   f4_delt_id;                       /* Delta Idc */
    float                   f4_delt_iq;                       /* Delta Iqc */
    float                   f4_delta_ang;                     /* delta theta (angle)*/
    uint8_t                 u1_PF_start_cnt;                  /* PF Start counter */

    /* Sequence change */
    uint8_t                 u1_cur_chg_cnt;                   /* State change counter */
    uint8_t                 u1_div_ang_cnt;                   /* Magnetic pole position estimation judgment counter */

    /* PF */
    uint8_t                 u1_pf_calculated;
    uint8_t                 u1_posest_calculated;             /* Magnetic pole position estimation completion flag */

    float                   f4_pf_last_idc_plus;
    float                   f4_pf_last_idc_minus;

    /* High/low speed switching speed threshold */
    float                   f4_highspd_threshold;             /* Slow-to-fast switching speed threshold [r/min] */
    float                   f4_lowspd_threshold;              /* Fast to slow switching speed threshold [r/min] */

    /* 1ms task */
    uint16_t                u2_ang_est_count;
    float                   f4_div_ang;                       /* diverse of theta */
    float                   f4_last_ang;                      /* angle at last sample */
} st_lowspd_sensorless_t;


/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_init
* Description  : Initializes lowspeed sensorless module
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                f4_ld_pulse           - D-axis inductance
*                f4_lq_pulse           - Q-axis inductance
*                f4_volt_puls_inj      - Pulse injection frequency 1/f at the boot mode
*                f4_ctrl_period        - Control Period
*                u1_puls_inj_freq_boot - Pulse injection current pattern
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_init(st_lowspd_sensorless_t *p_st_lowspd,
                                         float f4_ld_pulse,
                                         float f4_lq_pulse,
                                         float f4_volt_puls_inj,
                                         float f4_ctrl_period,
                                         uint8_t u1_puls_inj_freq_boot);


/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_save_current
* Description  : save current data
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_id - d-axis detected current
*                 f4_iq - q-axis detected current
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_save_current(st_lowspd_sensorless_t *p_st_lowspd, float f4_id, float f4_iq);

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_current
* Description  : calcurate current avarage and delta id / iq
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_vd_ref - d-axis voltage reference
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_current(st_lowspd_sensorless_t *p_st_lowspd, float f4_vd_ref);


/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_dtheta
* Description  : calcurate delta theta
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_dtheta(st_lowspd_sensorless_t *p_st_lowspd);

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_vd_pulse_injection
* Description  : calcurate delta theta
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
*                 f4_vd_ref - d-axis voltage reference
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_vd_pulse_injection(st_lowspd_sensorless_t *p_st_lowspd, float f4_vd_ref);

/***********************************************************************************************************************
* Function Name: motor_current_lowspdsensorless_calc_pf
* Description  : calcurate delta pf
* Arguments    : p_st_lowspd -
*                    Pointer to the sensorless data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_lowspdsensorless_calc_pf(st_lowspd_sensorless_t *p_st_lowspd);

#endif /* R_MOTOR_CURRENT_LOWSPD_SENSORLESS_H */
