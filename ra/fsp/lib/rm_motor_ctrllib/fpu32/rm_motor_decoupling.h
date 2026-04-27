/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_DECOUPLING_H
#define RM_MOTOR_LIB_DECOUPLING_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definition
 ***********************************************************************************************************************/

void rm_motor_decoupling(float                          i_d,
                         float                          i_q,
                         float                          speed_el,
                         float                        * v_d_ref,
                         float                        * v_q_ref,
                         const motor_parameter_ramp_t * p_mtr);

#endif                                 /* RM_MOTOR_LIB_DECOUPLING_H */
