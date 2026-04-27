/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef R_MOTOR_TUNER_PM_FUNCTION_H
#define R_MOTOR_TUNER_PM_FUNCTION_H

#include <stdint.h>
#include <stdbool.h>

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/
typedef struct st_motor_tuner_dft
{
    float a_sum;
    float b_sum;
    float cnt;
} motor_tuner_dft_t;

typedef struct st_motor_tuner_dftdiff
{
    float a_sum[2];
    float b_sum[2];
    float cnt;
} motor_tuner_dftdiff_t;

typedef struct st_motor_tuner_sinref
{
    float freq;
    float amp;
    float offset;
    float ctrl_period;
    float w_ref;
    float angle_ref;
    float sinref;
} motor_tuner_sinref_t;

/***********************************************************************************************************************
 * Exported global variables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/
void rm_motor_tuner_dft_init(motor_tuner_dft_t * p_dft);
void rm_motor_tuner_dft_sum(motor_tuner_dft_t * p_dft, float angle, float signal);
void rm_motor_tuner_dft_result(motor_tuner_dft_t * p_dft, float * real, float * imag);

void rm_motor_tuner_dftdiff_init(motor_tuner_dftdiff_t * p_dftdiff);
void rm_motor_tuner_dftdiff_sum(motor_tuner_dftdiff_t * p_dftdiff, float angle, float signal_num, float signal_den);
void rm_motor_tuner_dftdiff_result_div(motor_tuner_dftdiff_t * p_dftdiff, float * real, float * imag);
void rm_motor_tuner_dftdiff_get_result_amp(motor_tuner_dftdiff_t * p_dftdiff, float * amp_num, float * amp_den);

void rm_motor_tuner_sinref_init(motor_tuner_sinref_t * p_sinref, float freq, float amp, float offset,
                                float ctrl_period);
float rm_motor_tuner_sinref_generate(motor_tuner_sinref_t * p_sinref);
float rm_motor_tuner_sinref_get_angle(motor_tuner_sinref_t * p_sinref);
void  rm_motor_tuner_sinref_set_amp(motor_tuner_sinref_t * p_sinref, float amp);

void rm_motor_tuner_linear_regression(const float * px, const float * py, int32_t n, float * slope, float * intercept);
bool rm_motor_tuner_is_equalf(float a, float b);

#endif                                 /* R_MOTOR_TUNER_PM_FUNCTION_H */
