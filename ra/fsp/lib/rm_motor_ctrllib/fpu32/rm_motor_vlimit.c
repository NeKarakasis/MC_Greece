/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "rm_motor_fundlib.h"
#include "rm_motor_vlimit.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief        Limit the voltage vector components within the available maximum voltage.
 *               The d-axis voltage (f4_vd_ref) is given higher priority than the q-axis voltage (f4_vq_ref).
 *
 * @param[out]   f4_vd_ref   Pointer to the d-axis voltage reference (will be limited in place).
 * @param[out]   f4_vq_ref   Pointer to the q-axis voltage reference (will be limited in place).
 * @param[in]    f4_va_max   Maximum allowed magnitude of the voltage vector.
 *
 * @details      If |f4_vd_ref| exceeds f4_va_max, it will be clipped to ±f4_va_max and f4_vq_ref is set to zero.
 *               Otherwise, f4_vq_ref is limited so that the vector magnitude does not exceed f4_va_max.
 *
 * @return       None
 **********************************************************************************************************************/
void rm_motor_voltage_limit (float * f4_vd_ref, float * f4_vq_ref, float f4_va_max)
{
    float f4_vq_lim = 0.0F;

    /* The d-axis voltage has high priority than the q-axis */
    /* If absolute value of D-axis voltage exceeds maximum voltage, limit it to the maximum voltage */
    if (*(f4_vd_ref) > f4_va_max)
    {
        *(f4_vd_ref) = f4_va_max;
        *(f4_vq_ref) = 0.0F;
    }
    else if (*(f4_vd_ref) < -f4_va_max)
    {
        *(f4_vd_ref) = -f4_va_max;
        *(f4_vq_ref) = 0.0F;
    }
    else
    {
        f4_vq_lim    = sqrtf((f4_va_max * f4_va_max) - ((*f4_vd_ref) * (*f4_vd_ref)));
        *(f4_vq_ref) = rm_motor_fundlib_limitfabs(*(f4_vq_ref), f4_vq_lim);
    }
}                                      /* End of function rm_motor_voltage_limit */
