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
* File Name   : r_motor_speed_extobserver.c
* Description : Extended observer which estimates speed and disturbance torque from torque command and measured speed
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 20.12.2022 1.00
***********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "r_motor_speed_extobserver.h"
#include "r_motor_common.h"

/***********************************************************************************************************************
* Private (static) variables and functions
***********************************************************************************************************************/
static void  motor_speed_extobserver_lpf_gain_calc(st_extobs_lpf_t * p_st_extobs_lpf,
                                                   float f4_omega,
                                                   float f4_ctrl_period);
static float motor_speed_extobserver_lpf(st_extobs_lpf_t * p_st_extobs_lpf, float f4_input);
static void  motor_speed_extobserver_lpf_init(st_extobs_lpf_t * p_st_extobs_lpf);
static void  motor_speed_extobserver_lpf_reset(st_extobs_lpf_t * p_st_extobs_lpf);

/***********************************************************************************************************************
* Global functions definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_init
* Description   : Initializes the extended observer with given parameters
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_natural_freq_hz - The natural frequency [Hz] that determine gain of the observer
*                 f4_ctrl_period_s   - The sample period [s]
*                 f4_inertia_kgms2   - The inertia moment [kgm/s^2]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_init(st_extobs_t * p_st_extobs,
                                  float f4_natural_freq_hz,
                                  float f4_ctrl_period_s,
                                  float f4_inertia_kgms2,
                                  float f4_outputlim_start,
                                  float f4_outputlim_end,
                                  float f4_nominal_current)
{
    p_st_extobs->f4_estimated_speed = 0.0f;
    p_st_extobs->f4_estimated_distb = 0.0f;
    p_st_extobs->f4_inertia_kgms2   = f4_inertia_kgms2;
    p_st_extobs->f4_ctrl_period_s   = f4_ctrl_period_s;

    p_st_extobs->f4_outputlim_start_rad = f4_outputlim_start * MTR_RPM2RAD;
    p_st_extobs->f4_outputlim_end_rad = f4_outputlim_end * MTR_RPM2RAD;

    p_st_extobs->f4_nominal_cur = f4_nominal_current;
    p_st_extobs->f4_nominal_cur_peak = f4_nominal_current * MTR_SQRT_3; /* Q-axis rated current limiter value calculation */

    motor_speed_extobserver_natural_freq_set(p_st_extobs, f4_natural_freq_hz, f4_ctrl_period_s);
    motor_speed_extobserver_lpf_init(&p_st_extobs->st_lpf);
} /* End of function motor_speed_extobserver_init */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_reset
* Description   : Resets the extended observer without reseting the parameters
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_reset(st_extobs_t * p_st_extobs)
{
    p_st_extobs->f4_estimated_speed = 0.0f;
    p_st_extobs->f4_estimated_distb = 0.0f;
    motor_speed_extobserver_lpf_reset(&p_st_extobs->st_lpf);
} /* End of function motor_speed_extobserver_reset */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_start
* Description   : Executes the extended observer tick processes
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_torque_cmd_nm - The torque command [Nm], the actual torque output should be used if the Iq saturate
*                 f4_speed_rad     - The rotor speed [rad/s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_start(st_extobs_t * p_st_extobs, float f4_torque_cmd_nm, float f4_speed_rad)
{
    float f4_temp1;
    float f4_temp2;
    float f4_ctrl_period_s = p_st_extobs->f4_ctrl_period_s;
    float f4_speed_err;

    /* Calculate position error */
    f4_speed_err = f4_speed_rad - p_st_extobs->f4_estimated_speed;

    /* Calculate disturbance torque*/
    f4_temp1 = motor_speed_extobserver_lpf(&p_st_extobs->st_lpf, f4_speed_err);
    f4_temp2 = f4_temp1 * p_st_extobs->f4_gain_distb_est;
    p_st_extobs->f4_estimated_distb += (f4_temp2 * f4_ctrl_period_s);

    /* Calculate estimated speed*/
    f4_temp2 = f4_temp1 * p_st_extobs->f4_gain_speed_est;
    f4_temp2 += ((1.0f / p_st_extobs->f4_inertia_kgms2) * (f4_torque_cmd_nm - p_st_extobs->f4_estimated_distb));
    p_st_extobs->f4_estimated_speed += (f4_temp2 * f4_ctrl_period_s);
} /* End of function motor_speed_extobserver_start */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_mech_speed_get
* Description   : Gets the estimated mechanical speed
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : The estimated mechanical speed[rad/s]
***********************************************************************************************************************/
float motor_speed_extobserver_mech_speed_get(st_extobs_t * p_st_extobs)
{
    return p_st_extobs->f4_estimated_speed;
} /* End of function motor_speed_extobserver_mech_speed_get */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_disturbance_get
* Description   : Gets the estimated torque
* Arguments     : p_st_extobs - The pointer to the extended observer structure
* Return Value  : The estimated torque[Nm]
***********************************************************************************************************************/
float motor_speed_extobserver_disturbance_get(st_extobs_t * p_st_extobs)
{
    return p_st_extobs->f4_estimated_distb;
} /* End of function motor_speed_extobserver_disturbance_get */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_natural_freq_set
* Description   : Sets the natural frequency of estimators
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_natural_freq_hz - The natural frequency [Hz] to set
*                 f4_ctrl_period_s   - The control period [s]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_natural_freq_set(st_extobs_t * p_st_extobs,
                                              float f4_natural_freq_hz,
                                              float f4_ctrl_period_s)
{
    float f4_omega_rad_s  = f4_natural_freq_hz * MTR_TWOPI;
    float f4_omega_mult_3 = 3.0f * f4_omega_rad_s;
    float f4_omega_cubic  = f4_omega_rad_s * f4_omega_rad_s * f4_omega_rad_s;
    float f4_gain_temp_distb;
    float f4_gain_temp_speed;

    p_st_extobs->f4_ctrl_period_s   = f4_ctrl_period_s;
    p_st_extobs->f4_natural_freq_hz = f4_natural_freq_hz;

    /* Calculate estimation gains */
    f4_gain_temp_distb              = (-1.0f) * f4_omega_cubic * p_st_extobs->f4_inertia_kgms2;
    f4_gain_temp_speed              = 3.0f * f4_omega_rad_s * f4_omega_rad_s;
    p_st_extobs->f4_gain_distb_est  = f4_gain_temp_distb / f4_omega_mult_3;
    p_st_extobs->f4_gain_speed_est  = f4_gain_temp_speed / f4_omega_mult_3;

    motor_speed_extobserver_lpf_gain_calc(&p_st_extobs->st_lpf, f4_omega_mult_3, f4_ctrl_period_s);
} /* End of function motor_speed_extobserver_natural_freq_set */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_init_speed_set
* Description   : Sets the initial speed (estimated speed), use this function to synchronize the speed before
*                 calling motor_speed_extobserver_start function
* Arguments     : p_st_extobs  - The pointer to the extended observer structure
*                 f4_speed_rad - The mechanical speed [rad/sec]
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_init_speed_set(st_extobs_t * p_st_extobs, float f4_speed_rad)
{
    p_st_extobs->f4_estimated_speed = f4_speed_rad;
} /* End of function motor_speed_extobserver_init_speed_set */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_inertia_set
* Description   : Sets the inertia
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_inertia_kgms2 - The motor inertia [kgms2] to set
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_inertia_set(st_extobs_t * p_st_extobs, float f4_inertia_kgms2)
{
    p_st_extobs->f4_inertia_kgms2 = f4_inertia_kgms2;
} /* End of function motor_speed_extobserver_inertia_set */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_current_limit_set
* Description   : Sets the nominal current and nominal current peak
* Arguments     : p_st_extobs        - The pointer to the extended observer structure
*                 f4_nominal_current - Nominal Current
* Return Value  : None
***********************************************************************************************************************/
void motor_speed_extobserver_current_limit_set(st_extobs_t * p_st_extobs, float f4_nominal_current)
{
    p_st_extobs->f4_nominal_cur = f4_nominal_current;
    p_st_extobs->f4_nominal_cur_peak = f4_nominal_current * MTR_SQRT_3; /* Q-axis rated current limiter value calculation */
} /* End of function motor_speed_extobserver_current_limit_set */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_lpf_gain_calc
* Description   : Gain Calculation for LPF
* Arguments     : p_st_extobs_lpf - LPF structure (pointer)
*                 f4_omega        - Natural frequency
*                 f4_ctrl_period  - Control period
* Return Value  : None
***********************************************************************************************************************/
static void motor_speed_extobserver_lpf_gain_calc(st_extobs_lpf_t * p_st_extobs_lpf,
                                                  float f4_omega,
                                                  float f4_ctrl_period)
{
    p_st_extobs_lpf->f4_omega_t = f4_omega * f4_ctrl_period;
    p_st_extobs_lpf->f4_gain_ka = (2.0f - p_st_extobs_lpf->f4_omega_t) / (p_st_extobs_lpf->f4_omega_t + 2.0f);
    p_st_extobs_lpf->f4_gain_kb = p_st_extobs_lpf->f4_omega_t / (p_st_extobs_lpf->f4_omega_t + 2.0f);
} /* End of function motor_speed_extobserver_lpf_gain_calc */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_lpf
* Description   :Low-pass filter using bilinear transformation
* Arguments     : p_st_extobs_lpf - LPF structure (pointer)
                : f4_input        - Input value
* Return Value  : Filtered value
***********************************************************************************************************************/
static float motor_speed_extobserver_lpf(st_extobs_lpf_t * p_st_extobs_lpf, float f4_input)
{
    float f4_temp0;

    f4_temp0 = (p_st_extobs_lpf->f4_gain_ka * p_st_extobs_lpf->f4_pre_output);
    f4_temp0 += (p_st_extobs_lpf->f4_gain_kb * (f4_input + p_st_extobs_lpf->f4_pre_input));

    p_st_extobs_lpf->f4_pre_input  = f4_input;
    p_st_extobs_lpf->f4_pre_output = f4_temp0;

    return (f4_temp0);
} /* End of function motor_speed_extobserver_lpf */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_lpf_init
* Description   : Initialize  variables of LPF
* Arguments     : p_st_extobs_lpf - First order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
static void motor_speed_extobserver_lpf_init(st_extobs_lpf_t * p_st_extobs_lpf)
{
    p_st_extobs_lpf->f4_pre_output   = 0.0f;
    p_st_extobs_lpf->f4_pre_input    = 0.0f;
} /* End of function motor_speed_extobserver_lpf_init */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_lpf_reset
* Description   : Reset variables of LPF
* Arguments     : p_st_extobs_lpf -  LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
static void motor_speed_extobserver_lpf_reset(st_extobs_lpf_t * p_st_extobs_lpf)
{
    p_st_extobs_lpf->f4_pre_output   = 0.0f;
    p_st_extobs_lpf->f4_pre_input    = 0.0f;
} /* End of function motor_speed_extobserver_lpf_reset */

/***********************************************************************************************************************
* Function Name : motor_speed_extobserver_disturbance_current_limit
* Description   : Disturbance current limit
* Arguments     : p_st_extobs      - The pointer to the extended observer structure
*                 f4_speed_rad     - The rotor speed [rad/s]
*                 f4_disturb_current - The distrubance current [A], ASR feed forward
* Return Value  : limited distrubance current [A]
***********************************************************************************************************************/
float motor_speed_extobserver_disturbance_current_limit(st_extobs_t * p_st_extobs,
                                                        float f4_speed_rad,
                                                        float f4_disturb_current)
{
    float f4_limited_disturb_cur = 0.0f;

    if( fabsf(f4_speed_rad) < p_st_extobs->f4_outputlim_start_rad)
    {
        /* current limiter */
        if( f4_disturb_current > p_st_extobs->f4_nominal_cur_peak)
        {
            f4_limited_disturb_cur = p_st_extobs->f4_nominal_cur_peak;
        }
        else if( f4_disturb_current < -p_st_extobs->f4_nominal_cur_peak)
        {
            f4_limited_disturb_cur = -p_st_extobs->f4_nominal_cur_peak;
        }
        else
        {
            f4_limited_disturb_cur = f4_disturb_current;
        }
    }
    else if( (fabsf(f4_speed_rad) >= p_st_extobs->f4_outputlim_start_rad) &&
            (fabsf(f4_speed_rad) <= p_st_extobs->f4_outputlim_end_rad) )
    {
        float f4_nominal_cur_limit = p_st_extobs->f4_nominal_cur *
                                     ( 1.0f - (fabsf(f4_speed_rad) - p_st_extobs->f4_outputlim_start_rad) /
                                     (p_st_extobs->f4_outputlim_end_rad - p_st_extobs->f4_outputlim_start_rad));

        /* current limiter */
        if( f4_disturb_current > f4_nominal_cur_limit)
        {
            f4_limited_disturb_cur = f4_nominal_cur_limit;
        }
        else if( f4_disturb_current < -f4_nominal_cur_limit)
        {
            f4_limited_disturb_cur = -f4_nominal_cur_limit;
        }
        else
        {
            f4_limited_disturb_cur = f4_disturb_current;
        }
    }
    else
    {
        f4_limited_disturb_cur = 0.0f;
    }
    return f4_limited_disturb_cur;
} /* end of motor_speed_extobserver_disturbance_current_limit */
