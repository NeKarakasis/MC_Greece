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
* File Name   : r_motor_filter.h
* Description : Definitions of processes of filters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 16.06.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_FILTER_H
#define R_MOTOR_FILTER_H

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
}st_2nd_order_lpf_t;

/* First order LPF structure */
typedef struct
{
    float f4_pre_output;      /* Previous value of output */
    float f4_pre_input;       /* Previous value of input */
    float f4_omega_t;         /* Calculate value */
    float f4_gain_ka;         /* LPF gain */
    float f4_gain_kb;         /* LPF gain */
}st_1st_order_lpf_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : motor_filter_lpff
* Description   : LPF process
* Arguments     : f4_lpf_input      - LPF input value
                  f4_pre_lpf_output - Previous LPF output value
                  f4_lpf_k          - LPF parameter
* Return Value  : LPF output value
***********************************************************************************************************************/
float motor_filter_lpff(float f4_lpf_input, float f4_pre_lpf_output, float f4_lpf_k);

/***********************************************************************************************************************
* Function Name : motor_filter_limitf
* Description   : Limit with maximum limit and minimum limit
* Arguments     : f4_value - Target value
                  f4_max   - Maximum limit
                  f4_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_limitf(float f4_value, float f4_max, float f4_min);

/***********************************************************************************************************************
* Function Name : motor_filter_limit
* Description   : Limit with maximum limit and minimum limit
* Arguments     : s2_value - Target value
                  s2_max   - Maximum limit
                  s2_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_limit(int16_t s2_value, int16_t s2_max, int16_t s2_min);

/***********************************************************************************************************************
* Function Name : motor_filter_upper_limitf
* Description   : Limit with maximum limit
* Arguments     : f4_value - Target value
                  f4_max   - Maximum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_upper_limitf(float f4_value, float f4_max);

/***********************************************************************************************************************
* Function Name : motor_filter_upper_limit
* Description   : Limit with maximum limit
* Arguments     : s2_value - Target value
                  s2_max   - Maximum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_upper_limit(int16_t s2_value, int16_t s2_max);

/***********************************************************************************************************************
* Function Name : motor_filter_lower_limitf
* Description   : Limit with minimum limit
* Arguments     : f4_value - Target value
                  f4_min   - minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_lower_limitf(float f4_value, float f4_min);

/***********************************************************************************************************************
* Function Name : motor_filter_lower_limit
* Description   : Limit with minimum limit
* Arguments     : s2_value - Target value
                  s2_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_lower_limit(int16_t s2_value, int16_t s2_min);

/***********************************************************************************************************************
* Function Name : motor_filter_limitf_abs
* Description   : Limit with absolute value
* Arguments     : f4_value       - Target value
                  f4_limit_value - Limit
* Return Value  : Limited value
***********************************************************************************************************************/
float motor_filter_limitf_abs(float f4_value, float f4_limit_value);

/***********************************************************************************************************************
* Function Name : motor_filter_limit_abs
* Description   : Limit with absolute value
* Arguments     : s2_value       - Target value
                  s2_limit_value - Limit
* Return Value  : Limited value
***********************************************************************************************************************/
int16_t motor_filter_limit_abs(int16_t s2_value, int16_t s2_limit_value);

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_init
* Description   : Initialize Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_init(st_2nd_order_lpf_t *p_st_lpf);

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_reset
* Description   : Reset Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_reset(st_2nd_order_lpf_t *p_st_lpf);

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_gain_calc
* Description   : Gain Calculation for Second Order LPF
* Arguments     : p_st_lpf        - Second order LPF structure (pointer)
*                 f4_omega        - Natural frequency
*                 f4_zeta         - Damping ratio
*                 f4_ctrl_period  - Control period
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_gain_calc(st_2nd_order_lpf_t *p_st_lpf, float f4_omega,
                                        float f4_zeta, float f4_ctrl_period);

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff
* Description   : Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
                : f4_input - Input value
* Return Value  : Filtered value
***********************************************************************************************************************/
float motor_filter_second_order_lpff(st_2nd_order_lpf_t *p_st_lpf, float f4_input);

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_init
* Description   : Initialize First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_init(st_1st_order_lpf_t *p_st_lpf);

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_reset
* Description   : Reset First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_reset(st_1st_order_lpf_t *p_st_lpf);

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_gain_calc
* Description   : Gain Calculation for First Order LPF
* Arguments     : p_st_lpf        - First order LPF structure (pointer)
*                 f4_omega        - Natural frequency
*                 f4_ctrl_period  - Control period
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_gain_calc(st_1st_order_lpf_t *p_st_lpf, float f4_omega, float f4_ctrl_period);

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff
* Description   : First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
                : f4_input - Input value
* Return Value  : Filtered value
***********************************************************************************************************************/
float motor_filter_first_order_lpff(st_1st_order_lpf_t *p_st_lpf, float f4_input);
#endif /* R_MOTOR_FILTER_H */
