/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_SPDGAINCALC_H
#define RM_MOTOR_LIB_SPDGAINCALC_H

#include "rm_motor_pi_control.h"

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

void rm_motor_speed_pi_gain_calc(motor_parameter_ramp_t * p_motor,
                                 pi_ctrl_t              * p_pi_speed,
                                 float                    f4_speed_omega,
                                 float                    f4_speed_zeta,
                                 float                    f4_speed_ctrl_period);

#endif                                 /* RM_MOTOR_LIB_SPDGAINCALC_H */
