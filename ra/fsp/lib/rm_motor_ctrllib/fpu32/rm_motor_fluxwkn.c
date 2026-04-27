/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include <math.h>
#include "rm_motor_fluxwkn.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/
void rm_motor_fluxwkn_init(motor_flux_weakening_t       * p_fluxwkn,
                           float                          f4_ia_max,
                           float                          f4_va_max,
                           const motor_parameter_ramp_t * p_mtr);
void     rm_motor_fluxwkn_reset(motor_flux_weakening_t * p_fluxwkn);
void     rm_motor_fluxwkn_set_motor(motor_flux_weakening_t * p_fluxwkn, const motor_parameter_ramp_t * p_mtr);
void     rm_motor_fluxwkn_set_vamax(motor_flux_weakening_t * p_fluxwkn, float f4_va_max);
void     rm_motor_fluxwkn_set_iamax(motor_flux_weakening_t * p_fluxwkn, float f4_ia_max);
uint16_t rm_motor_fluxwkn_run(motor_flux_weakening_t       * p_fluxwkn,
                              const motor_parameter_ramp_t * p_mtr,
                              float                          f4_speed_rad,
                              float                          p_f4_id,
                              float                          p_f4_iq,
                              float                        * p_f4_id_ref,
                              float                        * p_f4_iq_ref);
uint8_t rm_motor_fluxwkn_check_bypass(motor_flux_weakening_t * p_fluxwkn);

/*******************************************************************************************************************//**
 * @brief Calculates the missing component of a 2D vector given its magnitude and one component.
 *
 * Given the magnitude (absolute value) of a 2D vector and one of its components, computes the value
 * of the other component using the Pythagorean theorem. If the calculation results in a negative value
 * under the square root (due to rounding errors or invalid input), returns zero.
 *
 * Formula: sqrt(f4_abs^2 - f4_vec1^2)
 *
 * @param[in]  f4_abs   Magnitude (absolute value) of the vector (float)
 * @param[in]  f4_vec1  Known component of the vector (float)
 *
 * @retval     The value of the other component of the vector (float)
 **********************************************************************************************************************/
static inline float rm_motor_fluxwkn_get_vec (float f4_abs, float f4_vec1)
{
    float res;
    res = (f4_abs * f4_abs) - (f4_vec1 * f4_vec1);
    if (res < 0.0F)
    {
        res = 0.0F;
    }
    else
    {
        res = sqrtf(res);
    }

    return res;
}                                      /* End of function rm_motor_fluxwkn_get_vec */

/*******************************************************************************************************************//**
 * @brief Calculates the magnitude (absolute value) of a 2D vector from its components.
 *
 * Computes sqrt(f4_vec1^2 + f4_vec2^2). Used to determine the length of a vector in 2D space.
 *
 * @param[in]  f4_vec1   First component of the vector (float)
 * @param[in]  f4_vec2   Second component of the vector (float)
 *
 * @retval     Magnitude of the vector as a float
 **********************************************************************************************************************/
static inline float rm_motor_fluxwkn_vec_abs (float f4_vec1, float f4_vec2)
{
    return sqrtf((f4_vec1 * f4_vec1) + (f4_vec2 * f4_vec2));
}                                      /* End of function rm_motor_fluxwkn_vec_abs */

/*******************************************************************************************************************//**
 * @brief Sets the maximum available voltage ratio for the flux-weakening module.
 *
 * Updates the voltage ratio parameter for the flux-weakening module, ensuring the value is within valid bounds.
 * If the provided ratio is outside the allowed range, the module status is set to indicate an invalid ratio.
 *
 * @param[in,out]  p_fluxwkn    Pointer to the flux-weakening module structure
 * @param[in]      f4_vfw_ratio Maximum ratio of voltage available for flux-weakening operation (range: [VFWRATIO_MIN, 1.0])
 *
 * @retval         None
 **********************************************************************************************************************/
static void rm_motor_fluxwkn_set_v_fwratio (motor_flux_weakening_t * p_fluxwkn, float f4_vfw_ratio)
{
    if ((f4_vfw_ratio >= MOTOR_FLUXWKN_DEF_VFWRATIO_MIN) && (f4_vfw_ratio <= 1.0F))
    {
        p_fluxwkn->f4_vfw_ratio = f4_vfw_ratio;

        if (MOTOR_FLUXWKN_STATE_INVALID_VFWRATIO == p_fluxwkn->u2_fw_status)
        {
            p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_INVALID_VFWRATIO;
    }
}                                      /* End of function rm_motor_fluxwkn_set_v_fwratio */

/*******************************************************************************************************************//**
 * @brief Checks if the flux-weakening module status indicates any error (including invalid parameter error).
 *
 * Evaluates the status of the flux-weakening module to determine if any error condition is present,
 * including invalid parameter errors and runtime errors.
 *
 * @param[in]  p_fluxwkn   Pointer to the flux-weakening module structure
 *
 * @retval     1 if status indicates error, 0 otherwise
 **********************************************************************************************************************/
static uint8_t rm_motor_fluxwkn_is_error (motor_flux_weakening_t * p_fluxwkn)
{
    return (p_fluxwkn->u2_fw_status & MOTOR_FLUXWKN_CHK_MASK) != 0;
}                                      /* End of function rm_motor_fluxwkn_is_error */

/*******************************************************************************************************************//**
 * @brief Checks if the flux-weakening module status indicates an invalid parameter error.
 *
 * Evaluates the status of the flux-weakening module to determine if an invalid parameter error is present.
 *
 * @param[in]  p_fluxwkn   Pointer to the flux-weakening module structure
 *
 * @retval     1 if status indicates invalid parameter error, 0 otherwise
 **********************************************************************************************************************/
static uint8_t rm_motor_fluxwkn_is_invalid_parameter (motor_flux_weakening_t * p_fluxwkn)
{
    return (p_fluxwkn->u2_fw_status & MOTOR_FLUXWKN_CHK_MASK) == MOTOR_FLUXWKN_INVALID_CHK;
}                                      /* End of function rm_motor_fluxwkn_is_invalid_parameter */

/*******************************************************************************************************************//**
 * @brief Initializes the flux-weakening module.
 *
 * Sets up the flux-weakening module with motor parameters, current and voltage limits, and the default voltage margin.
 * All other parameters are initialized to their default values.
 *
 * @param[in]  p_fluxwkn   Pointer to flux-weakening module structure
 * @param[in]  f4_ia_max   Maximum magnitude of current vector
 * @param[in]  f4_va_max   Maximum magnitude of voltage vector
 * @param[in]  p_mtr       Pointer to motor parameter data structure
 *
 * @retval     None
 **********************************************************************************************************************/
void rm_motor_fluxwkn_init (motor_flux_weakening_t       * p_fluxwkn,
                            float                          f4_ia_max,
                            float                          f4_va_max,
                            const motor_parameter_ramp_t * p_mtr)
{
    /* Set up motor */
    rm_motor_fluxwkn_set_motor(p_fluxwkn, p_mtr);

    /* Set up circle limitation of current vector and voltage vector */
    rm_motor_fluxwkn_set_vamax(p_fluxwkn, f4_va_max);
    rm_motor_fluxwkn_set_iamax(p_fluxwkn, f4_ia_max);

    /* Set up flux-weakening voltage margin */
    rm_motor_fluxwkn_set_v_fwratio(p_fluxwkn, MOTOR_FLUXWKN_DEF_VFWRATIO);
}                                      /* End of function rm_motor_fluxwkn_init */

/*******************************************************************************************************************//**
 * @brief Resets the flux-weakening module status.
 *
 * Resets the module status to BYPASSED if all parameters are valid; otherwise, leaves the status unchanged.
 *
 * @param[in]  p_fluxwkn   Pointer to the flux-weakening module structure
 *
 * @retval     None
 **********************************************************************************************************************/
void rm_motor_fluxwkn_reset (motor_flux_weakening_t * p_fluxwkn)
{
    /* Reset only when parameters are valid */
    if (0 == rm_motor_fluxwkn_is_invalid_parameter(p_fluxwkn))
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
    }
}                                      /* End of function rm_motor_fluxwkn_reset */

/*******************************************************************************************************************//**
 * @brief Executes the flux-weakening control algorithm.
 *
 * Calculates optimal d-axis and q-axis current references for flux-weakening operation,
 * enforcing current and voltage limits. Returns the module status.
 *
 * @param[in]   p_fluxwkn     Pointer to the flux-weakening module structure
 * @param[in]   p_mtr         Pointer to motor parameter structure
 * @param[in]   f4_speed_mech Mechanical Rotor speed in radians per second [rad/s]
 * @param[in]   p_f4_id       Measured d-axis current
 * @param[in]   p_f4_iq       Measured q-axis current
 * @param[out]  p_f4_id_ref   Pointer to d-axis current reference output
 * @param[out]  p_f4_iq_ref   Pointer to q-axis current reference output
 *
 * @retval      Module status as uint16_t
 **********************************************************************************************************************/
uint16_t rm_motor_fluxwkn_run (motor_flux_weakening_t       * p_fluxwkn,
                               const motor_parameter_ramp_t * p_mtr,
                               float                          f4_speed_mech,
                               float                          p_f4_id,
                               float                          p_f4_iq,
                               float                        * p_f4_id_ref,
                               float                        * p_f4_iq_ref)
{
    float f4_idref;
    float f4_iqref;
    float f4_v_om;
    float f4_id_est;
    float f4_iq_max_abs;
    float f4_flux_abs;
    float f4_flux_q;
    float f4_flux_d;
    float f4_temp;
    float f4_flux_pm  = p_mtr->f4_mtr_m;
    float f4_ld       = p_mtr->f4_mtr_ld;
    float f4_lq       = p_mtr->f4_mtr_lq;
    float f4_r        = p_mtr->f4_mtr_r;
    float f4_speed_el = f4_speed_mech * p_mtr->u2_mtr_pp;

    /* Check if any error have occurred */
    if (rm_motor_fluxwkn_is_error(p_fluxwkn))
    {
        return p_fluxwkn->u2_fw_status;
    }

    /* Calculate Voltage vector limit exclude resistance voltage vector */
    f4_temp = f4_r * rm_motor_fluxwkn_vec_abs(p_f4_id, p_f4_iq);
    f4_v_om = (p_fluxwkn->f4_va_max - f4_temp) * p_fluxwkn->f4_vfw_ratio;

    /* Store the Vom for monitoring */
    p_fluxwkn->f4_v_fw = f4_v_om;

    /* Calculate d-axis value of the intersection between voltage limit circle and current limit circle */
    f4_temp   = (f4_v_om * f4_v_om) / ((f4_speed_el * f4_speed_el) * (f4_ld * f4_ld));
    f4_temp  -= (p_fluxwkn->f4_id_demag * p_fluxwkn->f4_id_demag);
    f4_temp  -= (p_fluxwkn->f4_ia_max * p_fluxwkn->f4_ia_max);
    f4_id_est = f4_temp / (-(MOTOR_FUNDLIB_MULTIPLE_2) *p_fluxwkn->f4_id_demag);

    /* Calculate q-axis current limit */
    if (f4_id_est > 0.0F)
    {
        /* If id_est > 0, flux-weakening is not necessary at max torque, maximum Iq is only limited by current limit
         * and maximum iq is determined by ia_max */
        f4_iq_max_abs = p_fluxwkn->f4_ia_max;
    }
    else if (f4_id_est < p_fluxwkn->f4_id_demag)
    {
        /* Since more current on minus d-axis dose not benefit, maximum Iq is only limited by the voltage limit circle
         */
        f4_iq_max_abs = fabsf(f4_v_om / (f4_lq * f4_speed_el));
    }
    else
    {
        /* Normal flux-weakening, at max torque */
        f4_iq_max_abs = rm_motor_fluxwkn_get_vec(p_fluxwkn->f4_ia_max, f4_id_est);
    }

    /* Limit Iq reference */
    f4_iqref = *p_f4_iq_ref;
    if (f4_iqref > f4_iq_max_abs)
    {
        f4_iqref = f4_iq_max_abs;
    }
    else if (f4_iqref < (-f4_iq_max_abs))
    {
        f4_iqref = -f4_iq_max_abs;
    }
    else
    {
        /* Do nothing */
    }

    /* Calculate Id reference */
    f4_flux_abs = f4_v_om / f4_speed_el;
    f4_flux_d   = f4_lq * f4_iqref;
    f4_flux_q   = rm_motor_fluxwkn_get_vec(f4_flux_abs, f4_flux_d);
    f4_idref    = (f4_flux_q - f4_flux_pm) / (f4_ld);

    if (f4_idref <= p_fluxwkn->f4_id_min)
    {
        f4_idref                = p_fluxwkn->f4_id_min;
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_IDSAT;
    }
    else if (f4_idref >= 0.0F)
    {
        f4_idref                = 0.0F;
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
    }
    else
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_FLUXWKN;
    }

    *p_f4_id_ref = f4_idref;
    *p_f4_iq_ref = f4_iqref;

    return p_fluxwkn->u2_fw_status;
}                                      /* End of function rm_motor_fluxwkn_run */

/*******************************************************************************************************************//**
 * @brief Sets the motor parameters for the flux-weakening module.
 *
 * Assigns the provided motor parameter structure to the flux-weakening module and computes
 * the demagnetizing current based on the motor's flux and inductance values, if all parameters are valid.
 * Updates the module status accordingly.
 *
 * @param[in,out] p_fluxwkn Pointer to the flux-weakening module structure.
 * @param[in]     p_mtr     Pointer to the motor parameter structure.
 *
 * @retval None
 ***********************************************************************************************************************/
void rm_motor_fluxwkn_set_motor (motor_flux_weakening_t * p_fluxwkn, const motor_parameter_ramp_t * p_mtr)
{
    uint8_t u1_is_valid_motor;

    /* Check if any of motor parameter is obviously invalid */
    u1_is_valid_motor = (0 != p_mtr);
    u1_is_valid_motor = u1_is_valid_motor && (p_mtr->f4_mtr_m > 0.0F);
    u1_is_valid_motor = u1_is_valid_motor && (p_mtr->f4_mtr_ld > 0.0F);
    u1_is_valid_motor = u1_is_valid_motor && (p_mtr->f4_mtr_lq > 0.0F);
    u1_is_valid_motor = u1_is_valid_motor && (p_mtr->f4_mtr_r > 0.0F);

    if (u1_is_valid_motor)
    {
        p_fluxwkn->f4_id_demag = -(p_mtr->f4_mtr_m / p_mtr->f4_mtr_ld);

        if (MOTOR_FLUXWKN_STATE_INVALID_MOTOR == p_fluxwkn->u2_fw_status)
        {
            p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_INVALID_MOTOR;
    }
}                                      /* End of function rm_motor_fluxwkn_set_motor */

/*******************************************************************************************************************//**
 * @brief Sets the maximum allowable magnitude of the current vector for the flux-weakening module.
 *
 * This function updates the maximum current (ia_max) used by the flux-weakening algorithm.
 * It also adjusts the minimum allowable d-axis current (id_min) based on ia_max and the demagnetizing current.
 * If the input value is invalid (non-positive), the module status is updated to indicate an error.
 *
 * @param[in,out] p_fluxwkn Pointer to the flux-weakening module structure.
 * @param[in]     f4_ia_max Maximum allowable magnitude of the current vector.
 *
 * @retval None
 ***********************************************************************************************************************/
void rm_motor_fluxwkn_set_iamax (motor_flux_weakening_t * p_fluxwkn, float f4_ia_max)
{
    if (f4_ia_max > 0.0F)
    {
        p_fluxwkn->f4_ia_max = f4_ia_max;

        /* Limit minimum d-axis current with min(-f4_ia_max,f4_id_demag) */
        if (p_fluxwkn->f4_id_demag > (-p_fluxwkn->f4_ia_max))
        {
            p_fluxwkn->f4_id_min = p_fluxwkn->f4_id_demag;
        }
        else
        {
            p_fluxwkn->f4_id_min = -p_fluxwkn->f4_ia_max;
        }

        if (MOTOR_FLUXWKN_STATE_INVALID_IAMAX == p_fluxwkn->u2_fw_status)
        {
            p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_INVALID_IAMAX;
    }
}                                      /* End of function rm_motor_fluxwkn_set_iamax */

/*******************************************************************************************************************//**
 * @brief Sets the maximum allowable magnitude of the voltage vector for the flux-weakening module.
 *
 * This function updates the maximum voltage (va_max) used by the flux-weakening algorithm.
 * If the input value is invalid (non-positive), the module status is updated to indicate an error.
 *
 * @param[in,out] p_fluxwkn Pointer to the flux-weakening module structure.
 * @param[in]     f4_va_max Maximum allowable magnitude of the voltage vector.
 *
 * @retval None
 ***********************************************************************************************************************/
void rm_motor_fluxwkn_set_vamax (motor_flux_weakening_t * p_fluxwkn, float f4_va_max)
{
    if (f4_va_max > 0.0F)
    {
        p_fluxwkn->f4_va_max = f4_va_max;

        if (MOTOR_FLUXWKN_STATE_INVALID_VAMAX == p_fluxwkn->u2_fw_status)
        {
            p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_BYPASSED;
        }
    }
    else
    {
        p_fluxwkn->u2_fw_status = MOTOR_FLUXWKN_STATE_INVALID_VAMAX;
    }
}                                      /* End of function rm_motor_fluxwkn_set_vamax */

/*******************************************************************************************************************//**
 * @brief Checks if the flux-weakening module is in bypass mode.
 *
 * This function examines the status of the flux-weakening module pointed to by p_fluxwkn.
 * If the module is in bypass mode (MOTOR_SPEED_FLUXWKN_STATE_BYPASSED), it returns 0.
 * Otherwise, it returns 1 to indicate the module is not bypassed.
 *
 * @param[in] p_fluxwkn Pointer to the flux-weakening module structure.
 *
 * @return 0 if bypassed, 1 if not bypassed.
 ***********************************************************************************************************************/
uint8_t rm_motor_fluxwkn_check_bypass (motor_flux_weakening_t * p_fluxwkn)
{
    uint8_t ret = 0;
    if (MOTOR_FLUXWKN_STATE_BYPASSED != p_fluxwkn->u2_fw_status)
    {
        ret = 1;
    }

    return ret;
}                                      /* End of function rm_motor_fluxwkn_check_bypass */
