/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_OUTER_H
#define RM_MOTOR_OUTER_H

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
 * Library Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/** Outer functions */
extern void rm_motor_outer_init(motor_pm_foc_instance_ctrl_t * const p_ctrl);
extern void rm_motor_outer_close(motor_pm_foc_instance_ctrl_t * const p_ctrl);
extern void rm_motor_outer_reset(motor_pm_foc_instance_ctrl_t * const p_ctrl);
extern void rm_motor_outer_parameter_update(motor_pm_foc_instance_ctrl_t * const p_ctrl);
extern void rm_motor_outer_error_get(motor_pm_foc_outer_instance_ctrl_t * p_ctrl, uint32_t * p_error_flags);

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MOTOR_OUTER_H
