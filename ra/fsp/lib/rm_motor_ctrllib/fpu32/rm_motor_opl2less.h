/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_OPL2LESS_H
#define RM_MOTOR_LIB_OPL2LESS_H

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

typedef enum  e_motor_mcsensor_less_switch
{
    MOTOR_MCSENSOR_LESS_SWITCH_DISABLE = 0, ///< Disable soft switching between open-loop mode and normal field oriented control mode
    MOTOR_MCSENSOR_LESS_SWITCH_ENABLE  = 1  ///< Enable soft switching between open-loop mode and normal field oriented control mode
} motor_mcsensor_less_switch_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

float rm_motor_opl2less_torque_current_calc(const motor_parameter_ramp_t * p_motor,
                                            float                          f4_opl2less_sw_time,
                                            float                          f4_ol_id_ref,
                                            float                          f4_phase_err_rad_lpf);
float rm_motor_opl2less_iq_calc(float f4_ed, float f4_eq, float f4_id, float f4_torque_current, float f4_phase_err);

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

#endif                                 // RM_MOTOR_LIB_OPL2LESS_H
