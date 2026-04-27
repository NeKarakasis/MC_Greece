/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_SHARED_POS_SPEED_H
#define RM_MOTOR_SHARED_POS_SPEED_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_mc_lib.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Structure of Position&Speed specific Interfaces*/

/* Structure of shared data from Position&Speed to inner loop */
typedef struct st_motor_shared_pos_speed_to_inner
{
    float   i_d_ref_ow;                ///< d-axis current reference for open-loop control
    float   i_q_ref_ow;                ///< q-axis current reference for open-loop control
    float   v_d_ref_inj;               ///< d-axis voltage reference for HFI (High-Frequency Injection)
    float   v_q_ref_inj;               ///< q-axis voltage reference for HFI, typically set to 0
    float   v_u_ref_ow;                ///< U-phase voltage reference for IPD (Initial Position Detection)
    float   v_v_ref_ow;                ///< V-phase voltage reference for IPD
    float   v_w_ref_ow;                ///< W-phase voltage reference for IPD
    uint8_t mode_sensor;               ///< sensor mode specification for Motor Control (MC)
} motor_shared_pos_speed_to_inner_t;

/* Structure of pointers for outer loop to MCSensor data*/
typedef struct st_motor_shared_outer_to_position_speed
{
    float   speed_mech_ref_ctrl;       ///< Reference speed of outer
    uint8_t speed_pi_status_flag;      ///< pi status flag
} motor_shared_outer_to_position_speed_t;

/* Structure of pointers for inner loop to MCSensor data*/
typedef struct st_motor_shared_inner_to_position_speed_params
{
    float i_u;                               ///< Motor phase u current [A]
    float i_v;                               ///< Motor phase v current [A]
    float i_w;                               ///< Motor phase w current [A]
    float inner_control_period;              ///< Control Period (Inner-loop)[sec]
    float outer_control_period;              ///< Control Period (Outer-loop)[sec]
    motor_parameter_ramp_t * p_motor_params; ///< Motor parameter Pointer
} motor_shared_inner_to_position_speed_t;

/* Structure of pointers for inner loop and MCSensor data*/
typedef struct st_motor_shared_sensor_data_exchange
{
    motor_shared_pos_speed_to_inner_t     ** pp_out_extra_data; ///< pointers for MCSensor data to inner loop
    motor_shared_inner_to_position_speed_t * p_in_extra_data;   ///< pointers for inner loop to MCSensor data
} motor_shared_sensor_data_exchange_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

#endif                                 // RM_MOTOR_SHARED_POS_SPEED_H
