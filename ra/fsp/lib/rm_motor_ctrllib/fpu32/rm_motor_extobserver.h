/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef R_MOTOR_SPEED_EXTOBSERVER_H
#define R_MOTOR_SPEED_EXTOBSERVER_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Global structure
 ***********************************************************************************************************************/

/* Main structure */
typedef struct st_motor_extobserver
{
    float              f4_gain_distb_est;      /* The gain of disturbance estimator */
    float              f4_gain_speed_est;      /* The gain of speed estimator */
    float              f4_estimated_distb;     /* The estimated disturbance */
    float              f4_estimated_speed;     /* The estimated speed */
    float              f4_inertia_kgms2;       /* The total inertia of system */
    float              f4_natural_freq_hz;     /* The natural frequency [Hz] */
    float              f4_ctrl_period_s;       /* The interval of estimator execution */
    float              f4_nominal_cur_peak;    /* Nominal current peak*/
    float              f4_nominal_cur;         /* Nominal current */
    float              f4_outputlim_start_rad; /*  */
    float              f4_outputlim_end_rad;   /*  */
    motor_common_lpf_t st_lpf;                 /* The LPF*/
} motor_extobserver_t;

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

void rm_motor_extobserver_init(motor_extobserver_t * p_st_extobs,
                               float                 f4_natural_freq_hz,
                               float                 f4_ctrl_period_s,
                               float                 f4_inertia_kgms2,
                               float                 f4_outputlim_start,
                               float                 f4_outputlim_end,
                               float                 f4_nominal_current);
void  rm_motor_extobserver_reset(motor_extobserver_t * p_st_extobs);
void  rm_motor_extobserver_start(motor_extobserver_t * p_st_extobs, float f4_torque_cmd_nm, float f4_speed_rad);
float rm_motor_extobserver_mech_speed_get(motor_extobserver_t * p_st_extobs);
float rm_motor_extobserver_disturbance_get(motor_extobserver_t * p_st_extobs);
void  rm_motor_extobserver_natural_freq_set(motor_extobserver_t * p_st_extobs,
                                            float                 f4_natural_freq_hz,
                                            float                 f4_ctrl_period_s);
void  rm_motor_extobserver_init_speed_set(motor_extobserver_t * p_st_extobs, float f4_speed_rad);
void  rm_motor_extobserver_inertia_set(motor_extobserver_t * p_st_extobs, float f4_inertia_kgms2);
void  rm_motor_extobserver_current_limit_set(motor_extobserver_t * p_st_extobs, float f4_nominal_current);
float rm_motor_extobserver_disturbance_current_limit(motor_extobserver_t * p_st_extobs,
                                                     float                 f4_speed_rad,
                                                     float                 f4_disturb_current);

#endif                                 /* R_MOTOR_SPEED_EXTOBSERVER_H */
