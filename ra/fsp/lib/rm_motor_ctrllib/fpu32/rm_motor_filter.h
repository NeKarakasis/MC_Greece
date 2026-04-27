/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_FILTER_H
#define RM_MOTOR_LIB_FILTER_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Structure definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Global function definitions
 ***********************************************************************************************************************/
float   rm_motor_filter_lpff(float f4_lpf_input, float f4_pre_lpf_output, float f4_lpf_k);
float   rm_motor_filter_limitf(float f4_value, float f4_max, float f4_min);
int16_t rm_motor_filter_limit(int16_t s2_value, int16_t s2_max, int16_t s2_min);
float   rm_motor_filter_upper_limitf(float f4_value, float f4_max);
int16_t rm_motor_filter_upper_limit(int16_t s2_value, int16_t s2_max);
float   rm_motor_filter_lower_limitf(float f4_value, float f4_min);
int16_t rm_motor_filter_lower_limit(int16_t s2_value, int16_t s2_min);
float   rm_motor_filter_limitf_abs(float f4_value, float f4_limit_value);
int16_t rm_motor_filter_limit_abs(int16_t s2_value, int16_t s2_limit_value);
void    rm_motor_filter_second_order_lpff_init(motor_fundlib_2nd_lpf_t * p_st_lpf);
void    rm_motor_filter_second_order_lpff_reset(motor_fundlib_2nd_lpf_t * p_st_lpf);
void    rm_motor_filter_second_order_lpff_gain_calc(motor_fundlib_2nd_lpf_t * p_st_lpf,
                                                    float                     f4_omega,
                                                    float                     f4_zeta,
                                                    float                     f4_ctrl_period);
float rm_motor_filter_second_order_lpff(motor_fundlib_2nd_lpf_t * p_st_lpf, float f4_input);
void  rm_motor_filter_first_order_lpff_init(motor_common_lpf_t * p_st_lpf);
void  rm_motor_filter_first_order_lpff_reset(motor_common_lpf_t * p_st_lpf);
void  rm_motor_filter_first_order_lpff_gain_calc(motor_common_lpf_t * p_st_lpf, float f4_omega, float f4_ctrl_period);
float rm_motor_filter_first_order_lpff(motor_common_lpf_t * p_st_lpf, float f4_input);

#endif                                 /* RM_MOTOR_LIB_FILTER_H */
