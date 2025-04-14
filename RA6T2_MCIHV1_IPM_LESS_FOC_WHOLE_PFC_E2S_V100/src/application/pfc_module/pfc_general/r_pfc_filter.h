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
* File Name   : r_pfc_filter.h
* Description : Definitions of processes of filters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_FILTER_H
#define R_PFC_FILTER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
/* Second order LPF structure */
typedef struct
{
    float f4_pre_output;      /* Previous value of output */
    float f4_pre2_output;     /* Before last value of output */
    float f4_pre_input;       /* Previous value of input */
    float f4_pre2_input;      /* Before last value of input */
    float f4_omega_t;         /* Calculate value */
    float f4_omega2_t;        /* Calculate value */
    float f4_omega2_t2;       /* Calculate value */
    float f4_gain_ka;         /* LPF gain */
    float f4_gain_kb;         /* LPF gain */
    float f4_gain_kc;         /* LPF gain */
} st_pfc_2nd_order_lpf_t;

/* First order LPF structure */
typedef struct
{
    float f4_pre_output;      /* Previous value of output */
    float f4_pre_input;       /* Previous value of input */
    float f4_omega_t;         /* Calculate value */
    float f4_gain_ka;         /* LPF gain */
    float f4_gain_kb;         /* LPF gain */
} st_pfc_1st_order_lpf_t;

/* Notch filter structure */
typedef struct
{
    float f4_in_data[3];      /* Input value */
    float f4_out_data[3];     /* Output value */
    float f4_coef_n[3];       /* Coefficients of the numerator polynomial */
    float f4_coef_d[3];       /* Coefficients of the denominator polynomial */
    float f4_omega_t;         /* Calculate value */
} st_pfc_notch_fil_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
float pfc_filter_lpff(float f4_lpf_input, float f4_pre_lpf_output, float f4_lpf_k);
float pfc_filter_limitf(float f4_value, float f4_max, float f4_min);
int16_t pfc_filter_limit(int16_t s2_value, int16_t s2_max, int16_t s2_min);
float pfc_filter_upper_limitf(float f4_value, float f4_max);
int16_t pfc_filter_upper_limit(int16_t s2_value, int16_t s2_max);
float pfc_filter_lower_limitf(float f4_value, float f4_min);
int16_t pfc_filter_lower_limit(int16_t s2_value, int16_t s2_min);
float pfc_filter_limitf_abs(float f4_value, float f4_limit_value);
int16_t pfc_filter_limit_abs(int16_t s2_value, int16_t s2_limit_value);
void pfc_filter_second_order_lpff_init(st_pfc_2nd_order_lpf_t *p_st_lpf);
void pfc_filter_second_order_lpff_reset(st_pfc_2nd_order_lpf_t *p_st_lpf);
void pfc_filter_second_order_lpff_gain_calc(st_pfc_2nd_order_lpf_t *p_st_lpf, float f4_omega,
                                        float f4_zeta, float f4_ctrl_period);
float pfc_filter_second_order_lpff(st_pfc_2nd_order_lpf_t *p_st_lpf, float f4_input);
void pfc_filter_first_order_lpff_init(st_pfc_1st_order_lpf_t *p_st_lpf);
void pfc_filter_first_order_lpff_reset(st_pfc_1st_order_lpf_t *p_st_lpf);
void pfc_filter_first_order_lpff_gain_calc(st_pfc_1st_order_lpf_t *p_st_lpf, float f4_omega, float f4_ctrl_period);
float pfc_filter_first_order_lpff(st_pfc_1st_order_lpf_t *p_st_lpf, float f4_input);
void pfc_filter_notch_init(st_pfc_notch_fil_t *p_st_notch_fil);
void pfc_filter_notch_reset(st_pfc_notch_fil_t *p_st_notch_fil);
void pfc_filter_notch_coef_calc(st_pfc_notch_fil_t *p_st_notch_fil,
                                float f4_omega,
                                float f4_samp_period,
                                float f4_d,
                                float f4_zeta);
float pfc_filter_notch(st_pfc_notch_fil_t *p_st_notch_fil, float f4_input);

#endif /* R_PFC_FILTER_H */
