/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_MTPA_H
#define RM_MOTOR_LIB_MTPA_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define MOTOR_MTPA_LDLQ_RATIO    1.01F ///< MTPA Ld/Lq arrowable threshold ratio. 1.00 = 100%,
///< typical IPMSM ratio is greater than 1.2

/***********************************************************************************************************************
 * Structure definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

uint16_t rm_motor_mtpa_run(const motor_parameter_ramp_t * p_mtr, float * p_f4_id_ref, float * p_f4_iq_ref);
uint16_t rm_motor_mtpa_wekn_judge(const motor_parameter_ramp_t * p_mtr,
                                  float                          f4_speed_rad,
                                  float                          f4_va_max,
                                  float                          f4_idref_mtpa,
                                  float                          f4_idref_weak,
                                  float                        * p_f4_id_ref);

#endif                                 /* RM_MOTOR_LIB_MTPA_H */
