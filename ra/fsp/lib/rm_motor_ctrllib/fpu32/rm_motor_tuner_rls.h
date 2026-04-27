/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef R_MOTOR_TUNER_PM_RLS_H
#define R_MOTOR_TUNER_PM_RLS_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* 3rd order Recursive Least Squares(RLS) filter */
typedef struct st_motor_tuner_rls3
{
    float p[3][3];
    float y_est;
    float err;
    float k[3];
    float w[3];
    float k_forget;
} motor_tuner_rls3_t;

/* 2nd order RLS */
typedef struct st_motor_tuner_rls
{
    float y;
    float u;
    float a;
    float b11;
    float b12;
    float b21;
    float b22;
    float p11;
    float p12;
    float p21;
    float p22;
    float y_pre;
    float u_pre;
    float p11_pre;
    float p12_pre;
    float p21_pre;
    float p22_pre;
    float k_forget;
    float k1;
    float k2;
    float pu_1;
    float pu_2;
    float a_est;
    float b_est;
    float a_est_pre;
    float b_est_pre;
    float v;
    float y_est;
    float err_est;
} motor_tuner_rls_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
void rm_motor_tuner_rls_init(motor_tuner_rls_t * p_rls, float ini_p, float ini_k_foget);
void rm_motor_tuner_rls_exec(motor_tuner_rls_t * p_rls, float u, float y, float * a_est, float * b_est);
void rm_motor_tuner_rls_set_init_ab(motor_tuner_rls_t * p_rls, float a_init, float b_init);
void rm_motor_tuner_rls2_init(motor_tuner_rls_t * p_rls,
                              float               ini_p_a,
                              float               int_p_b,
                              float               ini_k_foget,
                              float               a_init,
                              float               b_init);
void rm_motor_tuner_rls2_exec(motor_tuner_rls_t * p_rls, float a_in, float b_in, float y, float * a_est, float * b_est);
void rm_motor_tuner_rls3_init(motor_tuner_rls3_t * rls3,
                              float                ini_p_1,
                              float                int_p_2,
                              float                int_p_3,
                              float                ini_k_forget,
                              float                in_1_init,
                              float                in_2_init,
                              float                in_3_init);

void rm_motor_tuner_rls3_exec(motor_tuner_rls3_t * rls3, float * in, float y, float * weight);

#endif                                 /* R_MOTOR_TUNER_PM_RLS_H */
