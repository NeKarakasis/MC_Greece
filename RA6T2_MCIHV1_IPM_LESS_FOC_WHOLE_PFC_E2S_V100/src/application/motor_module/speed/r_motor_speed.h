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
* File Name   : r_motor_speed.h
* Description : Definitions of speed control processes
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_H
#define R_MOTOR_SPEED_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_module_cfg.h"
#include "r_motor_speed_api.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void motor_speed_max_speed_set(st_speed_control_t * p_st_sc, float f4_max_speed_rpm);
void motor_speed_rate_limit_set(st_speed_control_t * p_st_sc, float f4_speed_rate_limit_rpm);
float motor_speed_rate_limit_apply(st_speed_control_t * p_st_sc);
float motor_speed_pi_control(st_speed_control_t * p_st_sc, float f4_speed_rad);
float motor_speed_ref_speed_set(st_speed_control_t * p_st_sc);
void motor_speed_flux_weakening(st_speed_control_t * p_st_sc);
void motor_speed_mtpa(st_speed_control_t * p_st_sc);
void motor_speed_pi_gain_calc(st_speed_control_t * p_st_sc, float f4_omega_hz, float f4_zeta);

#endif /* R_MOTOR_SPEED_H */
