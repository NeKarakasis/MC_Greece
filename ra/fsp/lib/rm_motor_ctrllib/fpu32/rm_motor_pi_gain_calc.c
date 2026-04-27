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
#include "rm_motor_pi_gain_calc.h"

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief      Calculates PI control gains for d-axis and q-axis currents.
 *
 * @param[in]  st_motor         Pointer to the motor parameter structure.
 * @param[in]  st_design_params Pointer to the motor design parameter structure.
 * @param[in]  st_pi_id         Pointer to the PI controller structure for d-axis current.
 * @param[in]  st_pi_iq         Pointer to the PI controller structure for q-axis current.
 * @param[in]  f4_ctrl_period   Control period (in seconds) for current loop.
 *
 * @return     None.
 ***********************************************************************************************************************/
void rm_motor_pi_gain_calc (motor_parameter_ramp_t * st_motor,
                            motor_design_param_t   * st_design_params,
                            pi_ctrl_t              * st_pi_id,
                            pi_ctrl_t              * st_pi_iq,
                            float                    f4_ctrl_period)
{
    float f4_temp0;
    float f4_temp1;

    /* D-axis q-axis current PI gain */
    f4_temp0        = MOTOR_FUNDLIB_MULTIPLE_2 * st_design_params->zeta;
    f4_temp1        = MOTOR_FUNDLIB_TWOPI * st_design_params->omega;
    f4_temp0        = f4_temp0 * f4_temp1;
    f4_temp1        = (f4_temp1 * f4_temp1) * f4_ctrl_period;
    st_pi_id->f4_kp = (f4_temp0 * st_motor->f4_mtr_ld) - st_motor->f4_mtr_r;
    st_pi_id->f4_ki = f4_temp1 * st_motor->f4_mtr_ld;
    st_pi_iq->f4_kp = (f4_temp0 * st_motor->f4_mtr_lq) - st_motor->f4_mtr_r;
    st_pi_iq->f4_ki = f4_temp1 * st_motor->f4_mtr_lq;
}                                      /* End of function rm_motor_pi_gain_calc */
