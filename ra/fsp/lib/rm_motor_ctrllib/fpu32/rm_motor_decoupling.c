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
#include "rm_motor_decoupling.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Decoupling control to cancel interference voltage caused by the magnet and inductance.
 *
 * This function applies decoupling compensation to the d-axis and q-axis reference voltages
 * in Field Oriented Control (FOC) for motors. It calculates and subtracts the cross-coupling
 * voltage terms caused by motor inductances and permanent magnet flux linkage.
 *
 * @param[in]  i_d         d-axis current (A)
 * @param[in]  i_q         q-axis current (A)
 * @param[in]  speed_el    Electrical angular speed (rad/s)
 * @param[out] v_d_ref     Pointer to d-axis reference voltage (V)
 * @param[out] v_q_ref     Pointer to q-axis reference voltage (V)
 * @param[in]  p_mtr       Pointer to motor parameter structure
 *
 * @return     None
 **********************************************************************************************************************/
void rm_motor_decoupling (float                          i_d,
                          float                          i_q,
                          float                          speed_el,
                          float                        * v_d_ref,
                          float                        * v_q_ref,
                          const motor_parameter_ramp_t * p_mtr)
{
    float f4_temp0 = 0.0F;

    f4_temp0  = p_mtr->f4_mtr_lq * i_q;                          /* Lq * Iq */
    f4_temp0  = (speed_el * f4_temp0) - (p_mtr->f4_mtr_r * i_d); /* Speed * Lq * Iq - R * Id */
    *v_d_ref -= f4_temp0;                                        /* Vd - Speed * Lq * Iq + R * Id */

    f4_temp0  = p_mtr->f4_mtr_ld * i_d;                          /* Ld * id */
    f4_temp0  = f4_temp0 + p_mtr->f4_mtr_m;                      /* Ld * Id + Flux */
    f4_temp0  = speed_el * f4_temp0 + (p_mtr->f4_mtr_r * i_q);   /* Speed * (Ld * id + Flux) + R * Iq */
    *v_q_ref += f4_temp0;                                        /* Vq + Speed*(Ld * id + Flux) + R * Iq */
} /* End of function rm_motor_decoupling */
