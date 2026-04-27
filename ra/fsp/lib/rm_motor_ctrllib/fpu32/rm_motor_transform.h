/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_TRANSFORM_H
#define RM_MOTOR_TRANSFORM_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

void rm_motor_transform_uvw_dq_abs(const float f_angle, const float * f_uvw, float * f_dq);
void rm_motor_transform_dq_uvw_abs(const float f_angle, const float * f_dq, float * f_uvw);
void rm_motor_transform_uvw_dq_abs_trigo(const float f_sin, const float f_cos, const float * f_uvw, float * f_dq);
void rm_motor_transform_dq_uvw_abs_trigo(const float f_sin, const float f_cos, const float * f_dq, float * f_uvw);

#endif                                 // RM_MOTOR_TRANSFORM_H
