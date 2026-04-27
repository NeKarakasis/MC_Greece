/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_SPDOBSERVER_H
#define RM_MOTOR_LIB_SPDOBSERVER_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct st_motor_spdobserver
{
    float f4_speed_rad;                ///< speed observer output speed [rad/s] (electrical)
    float f4_ref_torque;               ///< reference torque
    float f4_ref_pre_torque;           ///< previous value of reference torque
    float f4_ref_speed_rad;            ///< reference speed for sob [rad/s] (electrical)
    float f4_ref_pre_speed_rad;        ///< previous value of reference speed for sob [rad/s] (electrical)
    float f4_hpf_k1;                   ///< HPF gain for sob
    float f4_hpf_k2;                   ///< HPF gain for sob
    float f4_hpf_k3;                   ///< HPF gain for sob
    float f4_k1;                       ///< K1 gain for sob
    float f4_k2;                       ///< K2 gain for sob
    float f4_hpf_ref_speed_rad;        ///< HPF output reference speed [rad/s]
    float f4_hpf_ref_pre_speed_rad;    ///< previous value of HPF output reference speed [rad/s]
    float f4_hpf_omega;                ///< natural frequency for speed observer HPF
    motor_fundlib_2nd_lpf_t st_lpf;    ///< second order LPF structure
} motor_spdobserver_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

void  rm_motor_spdobserver_init(motor_spdobserver_t * p_sob);
void  rm_motor_spdobserver_reset(motor_spdobserver_t * p_sob);
float rm_motor_spdobserver(motor_spdobserver_t    * p_sob,
                           motor_parameter_ramp_t * p_motor,
                           float                    f4_iq_ref,
                           float                    f4_speed_rad);
void rm_motor_spdobserver_gain_calc(motor_spdobserver_t * p_sob,
                                    float                 f4_observer_omega,
                                    float                 f4_observer_zeta,
                                    float                 f4_speed_ctrl_period);

#endif                                 /* RM_MOTOR_LIB_SPDOBSERVER_H */
