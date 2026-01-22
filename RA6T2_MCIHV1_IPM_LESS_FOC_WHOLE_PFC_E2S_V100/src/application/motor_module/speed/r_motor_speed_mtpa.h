/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_motor_speed_mtpa.h
* Description  : Definitions for MTPA module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_MTPA_H
#define R_MOTOR_SPEED_MTPA_H

/***********************************************************************************************************************
* Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
/**
 * MTPA data structure
 */
typedef struct
{
    /* User configurations */
    const st_motor_parameter_t *p_motor;  /* The pointer to the motor parameter data structure */
} st_mtpa_t;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void motor_speed_mtpa_init(st_mtpa_t * p_mtpa, const st_motor_parameter_t * p_mtr);
uint16_t motor_speed_mtpa_run(st_mtpa_t * p_mtpa, float * p_f4_idq_ref);
void motor_speed_mtpa_motor_param_set(st_mtpa_t * p_st_mtpa, const st_motor_parameter_t * p_st_motor);
uint16_t motor_speed_mtpa_wekn_judge(st_mtpa_t * p_mtpa,
                                     float f4_speed_rad,
                                     float f4_va_max,
                                     float f4_idref_mtpa,
                                     float f4_idref_weak,
                                     float * p_f4_idq_ref);

#endif /* R_MOTOR_SPEED_MTPA_H */
