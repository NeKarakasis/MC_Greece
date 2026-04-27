/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_INNER_H
#define RM_MOTOR_INNER_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_mc_lib.h"
#include "rm_motor_algorithm_api.h"
#include "rm_motor_pm_foc.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

extern void rm_motor_inner_init(motor_pm_foc_instance_ctrl_t * const p_ctrl);
extern void rm_motor_inner_close(motor_pm_foc_inner_instance_ctrl_t * const p_ctrl);
extern void rm_motor_inner_reset(motor_pm_foc_inner_instance_ctrl_t * const p_ctrl);
extern void rm_motor_inner_parameter_update(motor_pm_foc_instance_ctrl_t * const p_ctrl,
                                            motor_algorithm_cfg_t const * const  p_cfg);
extern void rm_motor_inner_speed_get(motor_pm_foc_instance_ctrl_t * const p_ctrl,
                                     motor_algorithm_cfg_t const * const  p_cfg,
                                     float * const                        p_speed_rpm);
extern void rm_motor_inner_error_get(motor_pm_foc_inner_instance_ctrl_t * p_ctrl, uint32_t * p_error_flags);
extern void rm_motor_inner_outer_ref_ctrl_mode_get(motor_pm_foc_inner_instance_ctrl_t * p_ctrl,
                                                   motor_mode_inner_to_outer_t        * p_inner_mode_data);

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_INNER_H
