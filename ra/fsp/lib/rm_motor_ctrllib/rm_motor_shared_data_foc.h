/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_SHARED_DATA_FOC_H
#define RM_MOTOR_SHARED_DATA_FOC_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_shared_pos_speed_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_SENSORLESS_FLG_CLR    (0)
#define MOTOR_SENSORLESS_FLG_SET    (1)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Structure of shared variables from inner to outer loop */
typedef struct st_motor_shared_inner_to_outer_foc
{
    float   i_d;                       ///< D-axis current [A]
    float   i_q;                       ///< Q-axis current [A]
    float   v_ampl_max;
    float   v_d_ref;                   ///< Reference d-axis voltage [V]
    float   v_q_ref;                   ///< Reference q-axis voltage [V]
    uint8_t current_ref_update_flag;   ///< Flag to set d/q-axis current reference
} motor_shared_inner_to_outer_foc_t;

/* Structure of shared variables from outer to inner loop */
typedef struct st_motor_shared_outer_to_inner_foc
{
    float i_d_ref;                                               ///< D-axis current reference [A] for calculation
    float i_q_ref;                                               ///< Q-axis current reference [A] for calculation
    motor_shared_outer_to_position_speed_t pos_speed_input_data; ///< struct of data used by position&speed module
} motor_shared_outer_to_inner_foc_t;

typedef struct st_motor_shared_buffer_foc
{
    /* Outer control <=> Inner control data interface */
    motor_shared_inner_to_outer_foc_t shared_inner_to_outer_A;
    motor_shared_inner_to_outer_foc_t shared_inner_to_outer_B;
    motor_shared_outer_to_inner_foc_t shared_outer_to_inner_A;
    motor_shared_outer_to_inner_foc_t shared_outer_to_inner_B; // copy is needed for inner loop interrupting outer loop
    void ** pp_shared_outer_to_inner_active;                   // pointer to pointer active buffer
    void ** pp_shared_inner_to_outer_active;                   // pointer to pointer active buffer
} motor_shared_buffer_foc_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

#endif                                 // RM_MOTOR_SHARED_DATA_FOC_H
