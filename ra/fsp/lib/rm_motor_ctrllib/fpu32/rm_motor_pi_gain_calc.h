/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_PI_GAIN_H
#define RM_MOTOR_LIB_PI_GAIN_H

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/
void rm_motor_pi_gain_calc(motor_parameter_ramp_t * st_motor,
                           motor_design_param_t   * st_design_params,
                           pi_ctrl_t              * st_pi_id,
                           pi_ctrl_t              * st_pi_iq,
                           float                    f4_ctrl_period);

#endif                                 /* RM_MOTOR_LIB_PI_GAIN_H */
