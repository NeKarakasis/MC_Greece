/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_FUNDLIB_H
 #define RM_MOTOR_FUNDLIB_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
 #include <stdint.h>

 #ifdef __cplusplus
extern "C" {
 #endif

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

 #define     MOTOR_FUNDLIB_FLAG_CLEAR    (0) /* For flag clear */
 #define     MOTOR_FUNDLIB_FLAG_SET      (1) /* For flag set */
 #define     MOTOR_FUNDLIB_MULTIPLE_2    (2.0F)
 #define     MOTOR_FUNDLIB_MULTIPLE_3    (3.0F)
 #define     MOTOR_FUNDLIB_MULTIPLE_4    (4.0F)
 #define     MOTOR_FUNDLIB_MULTIPLE_8    (8.0F)
 #define     MOTOR_FUNDLIB_TWOPI         (3.14159265358979F * 2.0F)
 #define     MOTOR_FUNDLIB_TWOPI_60      (MOTOR_FUNDLIB_TWOPI / 60.0F)  /* To translate rpm => rad/s */
 #define     MOTOR_FUNDLIB_DIV_8BIT      (1.0F / 256.0F)
 #define     MOTOR_FUNDLIB_RAD_TRANS     (3.14159265359F / 180.0F)
 #define     MOTOR_FUNDLIB_RPM2RAD       (MOTOR_FUNDLIB_TWOPI / 60.0F)  /* for rpm -> rad/s transration ((2 * pi) / 60) */
 #define     MOTOR_FUNDLIB_RAD2RPM       (60.0F / MOTOR_FUNDLIB_TWOPI)  /* for rad/s -> rpm transration (60 / (2 * pi)) */
 #define     MOTOR_FUNDLIB_DEG2RAD       (MOTOR_FUNDLIB_TWOPI / 360.0F) /* for degree -> rad transration ((2 * pi) / 360) */
 #define     MOTOR_FUNDLIB_RAD2DEG       (360.0F / MOTOR_FUNDLIB_TWOPI) /* for rad -> degree transration (360 / (2 * pi)) */
 #define     MOTOR_FUNDLIB_SQRT_2        (1.41421356F)                  /* Sqrt(2) */
 #define     MOTOR_FUNDLIB_SQRT_3        (1.7320508F)                   /* Sqrt(3) */
 #define     MOTOR_FUNDLIB_DIV_KHZ       (0.001F)

/* Control mode for Vector Control & MCSensor */
 #define     MOTOR_CTRLMODE_DEFAULT      0                              /* PI */
 #define     MOTOR_CTRLMODE_ID_UP_REF    1                              /* Openloop Id rising for initial positioning*/
 #define     MOTOR_CTRLMODE_IDIQ_REF     2                              /* Openloop */
 #define     MOTOR_CTRLMODE_VDVQ_INJ     3                              /* HFI */
 #define     MOTOR_CTRLMODE_VDVQ_REF     4                              /*  */
 #define     MOTOR_CTRLMODE_VUVW_REF     5                              /* IPD */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* First order LPF structure */
typedef struct motor_common_lpf
{
    float f4_pre_output;               ///< Previous value of output
    float f4_pre_input;                ///< Previous value of input
    float f4_omega_t;                  ///< Calculate value
    float f4_gain_ka;                  ///< LPF gain
    float f4_gain_kb;                  ///< LPF gain
} motor_common_lpf_t;

typedef struct st_motor_parameter_ramp
{
    uint16_t u2_mtr_pp;                ///< Pole pairs
    float    f4_mtr_r;                 ///< Resistance [ohm]
    float    f4_mtr_ld;                ///< Inductance for d-axis [H]
    float    f4_mtr_lq;                ///< Inductance for q-axis [H]
    float    f4_mtr_m;                 ///< Magnet flux [Wb]
    float    f4_mtr_j;                 ///< Rotor inertia [kgm^2]
    float    f4_mtr_nominal_current;   ///< Rated current [Arms]
} motor_parameter_ramp_t;

/* Design parameter structure */
typedef struct motor_design_param
{
    float omega;                       ///< Natural frequency[Hz] for loop gain design
    float zeta;                        ///< Damping ratio for loop gain design
} motor_design_param_t;

/* Second order LPF structure */
typedef struct st_motor_fundlib_2nd_lpf
{
    float f4_pre_output;               /* Previous value of output */
    float f4_pre2_output;              /* Before last value of output */
    float f4_pre_input;                /* Previous value of input */
    float f4_pre2_input;               /* Before last value of input */
    float f4_omega_t;                  /* Calculate value */
    float f4_omega2_t;                 /* Calculate value */
    float f4_omega2_t2;                /* Calculate value */
    float f4_gain_ka;                  /* LPF gain */
    float f4_gain_kb;                  /* LPF gain */
    float f4_gain_kc;                  /* LPF gain */
} motor_fundlib_2nd_lpf_t;

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

float   rm_motor_fundlib_limitfabs(float f4_value, float f4_limit_value);
float   rm_motor_fundlib_limitf(float f4_value, float f4_max, float f4_min);
int16_t rm_motor_fundlib_limit(int16_t s2_value, int16_t s2_max, int16_t s2_min);
float   rm_motor_fundlib_upper_limitf(float f4_value, float f4_max);
int16_t rm_motor_fundlib_upper_limit(int16_t s2_value, int16_t s2_max);
float   rm_motor_fundlib_lower_limitf(float f4_value, float f4_min);
int16_t rm_motor_fundlib_lower_limit(int16_t s2_value, int16_t s2_min);
int16_t rm_motor_fundlib_limit_abs(int16_t s2_value, int16_t s2_limit_value);
float   rm_motor_fundamental_limitf_abs(float f4_value, float f4_limit_value);

float rm_motor_sample_delay_comp(float f4_angle_rad, float f4_speed_rad, float f4_ctrl_period, float f4_magnitude);

 #ifdef __cplusplus
}
 #endif

#endif                                 // RM_MOTOR_FUNDLIB_H
