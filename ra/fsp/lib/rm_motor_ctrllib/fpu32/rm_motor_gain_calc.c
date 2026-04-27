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
#include "rm_motor_pi_control.h"
#include "rm_motor_gain_calc.h"

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief      Calculates PI control gains for motor speed control loop.
 * @details    Uses motor physical parameters and control design values to compute proportional (Kp) and integral (Ki)
 *             gains for the speed PI controller.
 * @param[in]  p_motor              Pointer to motor physical parameter structure.
 * @param[out] p_pi_speed           Pointer to PI controller parameter structure to be updated.
 * @param[in]  f4_speed_omega       Desired speed loop natural frequency [rad/s].
 * @param[in]  f4_speed_zeta        Desired speed loop damping factor.
 * @param[in]  f4_speed_ctrl_period Speed loop control period [s].
 * @return     None
 ***********************************************************************************************************************/
void rm_motor_speed_pi_gain_calc (motor_parameter_ramp_t * p_motor,
                                  pi_ctrl_t              * p_pi_speed,
                                  float                    f4_speed_omega,
                                  float                    f4_speed_zeta,
                                  float                    f4_speed_ctrl_period)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;

    /* Speed PI gain */
    f4_temp0          = MOTOR_FUNDLIB_MULTIPLE_2 * f4_speed_zeta;
    f4_temp1          = MOTOR_FUNDLIB_TWOPI * f4_speed_omega;
    f4_temp2          = (f4_temp0 * f4_temp1) * p_motor->f4_mtr_j;
    f4_temp0          = (f4_temp1 * f4_temp1) * p_motor->f4_mtr_j;
    f4_temp0          = f4_temp0 * f4_speed_ctrl_period;
    f4_temp1          = (float) (p_motor->u2_mtr_pp) * p_motor->f4_mtr_m;
    p_pi_speed->f4_kp = f4_temp2 / f4_temp1;
    p_pi_speed->f4_ki = f4_temp0 / f4_temp1;
}                                      /* End of function rm_motor_speed_pi_gain_calc */
