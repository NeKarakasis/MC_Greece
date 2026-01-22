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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name   : r_motor_current.h
* Description : Definitions of FOC current control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.06.2021 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_H
#define R_MOTOR_CURRENT_H

/**********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"
#include "r_motor_pi_control.h"
#include "r_motor_current_api.h"

/**********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/**********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void motor_current_decoupling(st_current_control_t *p_st_cc,
                              float f4_speed_rad,
                              const st_motor_parameter_t *p_st_motor);
void motor_current_pi_control(st_current_control_t *p_st_cc);
void motor_current_rotor_angle_init(st_rotor_angle_t *p_st_angle);
void motor_current_rotor_angle_reset(st_rotor_angle_t *p_st_angle);
void motor_current_rotor_angle_update(st_rotor_angle_t *p_st_angle, float f4_angle_rad);
void motor_current_transform_uvw_dq_abs(const st_rotor_angle_t *p_st_angle, const float *f4_uvw, float *f4_dq);
void motor_current_transform_dq_uvw_abs(const st_rotor_angle_t *p_st_angle, const float *f4_dq, float *f4_uvw);
void motor_current_volt_limit(st_current_control_t *p_st_cc);
float motor_current_reference_iq_set(st_current_control_t *p_st_cc);
float motor_current_reference_id_set(st_current_control_t *p_st_cc);
void motor_current_angle_speed_detect(st_current_control_t *p_st_cc);
float motor_current_angle_get(st_rotor_angle_t *p_st_angle);
void motor_current_pi_gain_calc(st_current_control_t *p_st_cc, float f4_omega_hz, float f4_zeta);
float motor_current_control_smpldly_comp(st_current_control_t *p_st_cc, float f4_angle_rad, float f4_speed_rad);


#endif /* R_MOTOR_CURRENT_H */
