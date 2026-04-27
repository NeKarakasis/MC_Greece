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
#include "rm_motor_mtpa.h"
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Determines whether to use Maximum Torque Per Ampere (MTPA) or Flux-Weakening control based on motor speed and current references.
 *
 * This function selects between MTPA and flux-weakening control modes by comparing the motor's electrical speed with the calculated
 * flux-weakening threshold and the d-axis current references. It outputs the appropriate d-axis current reference for the selected mode.
 *
 * @param[in]  p_mtr           Pointer to the motor parameter structure.
 * @param[in]  f4_speed_rad    Electrical speed of the motor in [rad/s].
 * @param[in]  f4_va_max       Maximum magnitude of the voltage vector.
 * @param[in]  f4_idref_mtpa   Reference d-axis current for MTPA control.
 * @param[in]  f4_idref_weak   Reference d-axis current for Flux-weakening control.
 * @param[out] p_f4_id_ref     Pointer to the d-axis current reference output.
 *
 * @retval     0               Always returns 0.
 ***********************************************************************************************************************/
uint16_t rm_motor_mtpa_wekn_judge (const motor_parameter_ramp_t * p_mtr,
                                   float                          f4_speed_rad,
                                   float                          f4_va_max,
                                   float                          f4_idref_mtpa,
                                   float                          f4_idref_weak,
                                   float                        * p_f4_id_ref)
{
    uint16_t u2_output_type = 0;
    float    f4_flux_pm     = p_mtr->f4_mtr_m;

    /* Calculate Max Ta and d-pole current command */
    float f4_wc = f4_va_max / f4_flux_pm;

    if (f4_speed_rad < f4_wc)
    {
        /* MTPA */

        /* Check rated speed or more / less */
        if (f4_idref_mtpa > f4_idref_weak)
        {
            /* Select Flux-weakening */
            u2_output_type = 0;
        }
        else
        {
            /* Select MTPA */
            u2_output_type = 1;
        }
    }
    else
    {
        /* Flux-weakening */
        u2_output_type = 0;
    }

    /* Output Id */
    switch (u2_output_type)
    {
        case 0:
        {
            *p_f4_id_ref = f4_idref_weak;
            break;
        }

        case 1:
        {
            *p_f4_id_ref = f4_idref_mtpa;
            break;
        }
    }

    return 0;
}                                      /* End of function rm_motor_mtpa_wekn_judge */

/*******************************************************************************************************************//**
 * Calculates the Maximum Torque Per Ampere (MTPA) current reference for the motor.
 *
 * This function computes the optimal d-axis current (Idref) for MTPA control based on motor parameters and
 * the provided q-axis current reference (Iqref). If the maximum torque per ampere is positive, it applies
 * the MTPA algorithm; otherwise, it sets the d-axis current to zero.
 *
 * @param[in]  p_mtr           Pointer to the motor parameter structure.
 * @param[in]  p_f4_id_ref     Pointer to the d-axis current reference variable (output).
 * @param[in]  p_f4_iq_ref     Pointer to the q-axis current reference variable (input).
 *
 * @retval     0               Always returns 0.
 **********************************************************************************************************************/
uint16_t rm_motor_mtpa_run (const motor_parameter_ramp_t * p_mtr, float * p_f4_id_ref, float * p_f4_iq_ref)
{
    float f4_idref;
    float f4_iqref;
    float f4_max_ta;
    float f4_flux_pm = p_mtr->f4_mtr_m;
    float f4_ld      = p_mtr->f4_mtr_ld;
    float f4_lq      = p_mtr->f4_mtr_lq;

    /* Calculate Max Ta and d-pole current command */

    f4_max_ta = ((f4_flux_pm * 0.5F) / (f4_lq - f4_ld));
    f4_iqref  = *p_f4_iq_ref;

    /* Id */
    if (f4_max_ta > 0)
    {
        /* Maximum Torque Per Ampere (MTPA) Control */
        f4_idref = f4_max_ta - sqrtf((f4_max_ta * f4_max_ta) + (f4_iqref * f4_iqref));
    }
    else
    {
        /* Id = 0 control */
        f4_idref = 0.0F;
    }

    *p_f4_id_ref = f4_idref;

    return 0;
}                                      /* End of function rm_motor_mtpa_run */
