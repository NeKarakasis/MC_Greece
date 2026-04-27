/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "rm_motor_fundlib.h"
#include "rm_motor_filter.h"
#include "rm_motor_extobserver.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/

/***********************************************************************************************************************
 * Private (static) variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Global functions definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief Initializes the extended observer with given parameters.
 *
 * This function sets up the extended observer structure with the specified natural frequency, control period,
 * motor inertia, output limits, and nominal current. It initializes the estimated speed and disturbance,
 * calculates internal limits and ratings, and sets up the natural frequency and low-pass filter for observer calculations.
 *
 * @param[in] p_st_extobs        Pointer to the extended observer structure.
 * @param[in] f4_natural_freq_hz Natural frequency [Hz] that determines gain of the observer.
 * @param[in] f4_ctrl_period_s   Sample period [s].
 * @param[in] f4_inertia_kgms2   Inertia moment [kgm/s^2].
 * @param[in] f4_outputlim_start Output limit start [RPM].
 * @param[in] f4_outputlim_end   Output limit end [RPM].
 * @param[in] f4_nominal_current Nominal current value.
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_init (motor_extobserver_t * p_st_extobs,
                                float                 f4_natural_freq_hz,
                                float                 f4_ctrl_period_s,
                                float                 f4_inertia_kgms2,
                                float                 f4_outputlim_start,
                                float                 f4_outputlim_end,
                                float                 f4_nominal_current)
{
    p_st_extobs->f4_estimated_speed = 0.0F;
    p_st_extobs->f4_estimated_distb = 0.0F;
    p_st_extobs->f4_inertia_kgms2   = f4_inertia_kgms2;
    p_st_extobs->f4_ctrl_period_s   = f4_ctrl_period_s;

    p_st_extobs->f4_outputlim_start_rad = f4_outputlim_start * MOTOR_FUNDLIB_RPM2RAD;
    p_st_extobs->f4_outputlim_end_rad   = f4_outputlim_end * MOTOR_FUNDLIB_RPM2RAD;

    p_st_extobs->f4_nominal_cur      = f4_nominal_current;
    p_st_extobs->f4_nominal_cur_peak = f4_nominal_current * MOTOR_FUNDLIB_SQRT_3; /* Q-axis rated current limiter value calculation */

    rm_motor_extobserver_natural_freq_set(p_st_extobs, f4_natural_freq_hz, f4_ctrl_period_s);
    rm_motor_filter_first_order_lpff_init(&p_st_extobs->st_lpf);
} /* End of function rm_motor_extobserver_init */

/***********************************************************************************************************************
 * @brief Resets the extended observer state.
 *
 * This function resets the estimated speed and disturbance values of the extended observer,
 * and reinitializes the internal low-pass filter. It does not modify any configuration parameters.
 *
 * @param[in] p_st_extobs Pointer to the extended observer structure.
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_reset (motor_extobserver_t * p_st_extobs)
{
    p_st_extobs->f4_estimated_speed = 0.0F;
    p_st_extobs->f4_estimated_distb = 0.0F;
    rm_motor_filter_first_order_lpff_init(&p_st_extobs->st_lpf);
}                                      /* End of function rm_motor_extobserver_reset */

/***********************************************************************************************************************
 * @brief Executes one observer update step for disturbance and speed estimation.
 *
 * This function updates the internal state of the extended observer by estimating the disturbance torque
 * and mechanical speed based on the input torque command and rotor speed. It applies a first-order low-pass
 * filter to the speed error, uses observer gains to update disturbance and speed estimates, and integrates
 * these over the control period. This function should be called at each control tick.
 *
 * @param[in] p_st_extobs      Pointer to the extended observer structure.
 * @param[in] f4_torque_cmd_nm Torque command [Nm]; use actual torque if Iq saturates.
 * @param[in] f4_speed_rad     Rotor speed [rad/s].
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_start (motor_extobserver_t * p_st_extobs, float f4_torque_cmd_nm, float f4_speed_rad)
{
    float f4_temp1;
    float f4_temp2;
    float f4_ctrl_period_s = p_st_extobs->f4_ctrl_period_s;
    float f4_speed_err;

    /* Calculate position error */
    f4_speed_err = f4_speed_rad - p_st_extobs->f4_estimated_speed;

    /* Calculate disturbance torque*/
    f4_temp1 = rm_motor_filter_first_order_lpff(&p_st_extobs->st_lpf, f4_speed_err);
    f4_temp2 = f4_temp1 * p_st_extobs->f4_gain_distb_est;
    p_st_extobs->f4_estimated_distb += (f4_temp2 * f4_ctrl_period_s);

    /* Calculate estimated speed*/
    f4_temp2  = f4_temp1 * p_st_extobs->f4_gain_speed_est;
    f4_temp2 += ((1.0F / p_st_extobs->f4_inertia_kgms2) * (f4_torque_cmd_nm - p_st_extobs->f4_estimated_distb));
    p_st_extobs->f4_estimated_speed += (f4_temp2 * f4_ctrl_period_s);
}                                      /* End of function rm_motor_extobserver_start */

/*******************************************************************************************************************//**
 * @brief Gets the estimated mechanical speed
 *
 * @param[in]  p_st_extobs - The pointer to the extended observer structure
 *
 * @retval The estimated mechanical speed[rad/s]
 **********************************************************************************************************************/
float rm_motor_extobserver_mech_speed_get (motor_extobserver_t * p_st_extobs)
{
    return p_st_extobs->f4_estimated_speed;
}                                      /* End of function rm_motor_extobserver_mech_speed_get */

/***********************************************************************************************************************
 * @brief Gets the estimated disturbance torque.
 *
 * This function returns the current estimated disturbance torque value from the extended observer structure.
 *
 * @param[in]  p_st_extobs - Pointer to the extended observer structure.
 *
 * @retval The estimated disturbance torque [Nm].
 ***********************************************************************************************************************/
float rm_motor_extobserver_disturbance_get (motor_extobserver_t * p_st_extobs)
{
    return p_st_extobs->f4_estimated_distb;
}                                      /* End of function rm_motor_extobserver_disturbance_get */

/***********************************************************************************************************************
 * @brief Sets the natural frequency of the extended observer estimators.
 *
 * This function configures the internal observer gains based on the specified natural frequency and control period.
 * It calculates the gain values for disturbance and speed estimation using the provided parameters, updating the
 * observer structure accordingly. Additionally, it initializes the low-pass filter used for observer calculations.
 *
 * @param[in] p_st_extobs        Pointer to the extended observer structure.
 * @param[in] f4_natural_freq_hz Natural frequency [Hz] to set for the observer.
 * @param[in] f4_ctrl_period_s   Control period [s].
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_natural_freq_set (motor_extobserver_t * p_st_extobs,
                                            float                 f4_natural_freq_hz,
                                            float                 f4_ctrl_period_s)
{
    float f4_omega_rad_s  = f4_natural_freq_hz * MOTOR_FUNDLIB_TWOPI;
    float f4_omega_mult_3 = 3.0F * f4_omega_rad_s;
    float f4_omega_cubic  = f4_omega_rad_s * f4_omega_rad_s * f4_omega_rad_s;
    float f4_gain_temp_distb;
    float f4_gain_temp_speed;

    p_st_extobs->f4_ctrl_period_s   = f4_ctrl_period_s;
    p_st_extobs->f4_natural_freq_hz = f4_natural_freq_hz;

    /* Calculate estimation gains */
    f4_gain_temp_distb             = (-1.0F) * f4_omega_cubic * p_st_extobs->f4_inertia_kgms2;
    f4_gain_temp_speed             = 3.0F * f4_omega_rad_s * f4_omega_rad_s;
    p_st_extobs->f4_gain_distb_est = f4_gain_temp_distb / f4_omega_mult_3;
    p_st_extobs->f4_gain_speed_est = f4_gain_temp_speed / f4_omega_mult_3;

    rm_motor_filter_first_order_lpff_gain_calc(&p_st_extobs->st_lpf, f4_omega_mult_3, f4_ctrl_period_s);
}                                      /* End of function rm_motor_extobserver_natural_freq_set */

/***********************************************************************************************************************
 * @brief Sets the initial estimated mechanical speed for the extended observer.
 *
 * This function initializes the observer's estimated speed value with the provided mechanical speed in radians per second.
 * It should be called to synchronize the observer's internal speed estimate before invoking rm_motor_extobserver_start.
 *
 * @param[in] p_st_extobs  Pointer to the extended observer structure.
 * @param[in] f4_speed_rad Initial mechanical speed [rad/s].
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_init_speed_set (motor_extobserver_t * p_st_extobs, float f4_speed_rad)
{
    p_st_extobs->f4_estimated_speed = f4_speed_rad;
}                                      /* End of function rm_motor_extobserver_init_speed_set */

/***********************************************************************************************************************
 * @brief Sets the motor inertia value for the extended observer.
 *
 * This function updates the inertia parameter of the observer structure with the specified value.
 * The inertia value is used for calculation of speed and disturbance estimates during observer operation.
 *
 * @param[in]  p_st_extobs      Pointer to the extended observer structure.
 * @param[in]  f4_inertia_kgms2 Motor inertia value [kg/m2] to set.
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_inertia_set (motor_extobserver_t * p_st_extobs, float f4_inertia_kgms2)
{
    p_st_extobs->f4_inertia_kgms2 = f4_inertia_kgms2;
}                                      /* End of function rm_motor_extobserver_inertia_set */

/***********************************************************************************************************************
 * @brief Sets the nominal current limits for the extended observer.
 *
 * This function sets the nominal current and calculates the Q-axis rated current peak limit
 * for the extended observer structure. The peak limit is computed as nominal current multiplied
 * by the square root of 3, and both values are stored for use in current limiting logic.
 *
 * @param[in] p_st_extobs        Pointer to the extended observer structure.
 * @param[in] f4_nominal_current Nominal current value to set [A].
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_extobserver_current_limit_set (motor_extobserver_t * p_st_extobs, float f4_nominal_current)
{
    p_st_extobs->f4_nominal_cur      = f4_nominal_current;
    p_st_extobs->f4_nominal_cur_peak = f4_nominal_current * MOTOR_FUNDLIB_SQRT_3; /* Q-axis rated current limiter value calculation */
} /* End of function rm_motor_extobserver_current_limit_set */

/***********************************************************************************************************************
 * @brief Limits the disturbance current based on rotor speed and observer parameters.
 *
 * This function applies current limiting logic to the input disturbance current according to the rotor speed and
 * the extended observer's configuration. At low speed, the current is limited to the peak nominal value.
 * Between low and high speed thresholds, the limit is linearly reduced. Above the high speed threshold,
 * the limited current is set to zero. This is typically used to protect the motor and power electronics.
 *
 * @param[in]  p_st_extobs        Pointer to the extended observer structure.
 * @param[in]  f4_speed_rad       Rotor speed [rad/s].
 * @param[in]  f4_disturb_current Disturbance current [A], e.g., ASR feed forward.
 *
 * @retval     Limited disturbance current [A].
 ***********************************************************************************************************************/
float rm_motor_extobserver_disturbance_current_limit (motor_extobserver_t * p_st_extobs,
                                                      float                 f4_speed_rad,
                                                      float                 f4_disturb_current)
{
    float f4_limited_disturb_cur = 0.0F;

    if (fabsf(f4_speed_rad) < p_st_extobs->f4_outputlim_start_rad)
    {
        /* current limiter */
        if (f4_disturb_current > p_st_extobs->f4_nominal_cur_peak)
        {
            f4_limited_disturb_cur = p_st_extobs->f4_nominal_cur_peak;
        }
        else if (f4_disturb_current < -p_st_extobs->f4_nominal_cur_peak)
        {
            f4_limited_disturb_cur = -p_st_extobs->f4_nominal_cur_peak;
        }
        else
        {
            f4_limited_disturb_cur = f4_disturb_current;
        }
    }
    else if ((fabsf(f4_speed_rad) >= p_st_extobs->f4_outputlim_start_rad) &&
             (fabsf(f4_speed_rad) <= p_st_extobs->f4_outputlim_end_rad))
    {
        float f4_nominal_cur_limit = p_st_extobs->f4_nominal_cur *
                                     (1.0F - (fabsf(f4_speed_rad) - p_st_extobs->f4_outputlim_start_rad) /
                                      (p_st_extobs->f4_outputlim_end_rad - p_st_extobs->f4_outputlim_start_rad));

        /* current limiter */
        if (f4_disturb_current > f4_nominal_cur_limit)
        {
            f4_limited_disturb_cur = f4_nominal_cur_limit;
        }
        else if (f4_disturb_current < -f4_nominal_cur_limit)
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
        f4_limited_disturb_cur = 0.0F;
    }

    return f4_limited_disturb_cur;
}                                      /* end of rm_motor_extobserver_disturbance_current_limit */
