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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_speed_opl2less.h
* Description : Definitions of sensor-less switch control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_OPL2LESS_H
#define R_MOTOR_SPEED_OPL2LESS_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_speed_opl2less_iq_calc
* Description   : Set Iq reference when sensor-less switch control
* Arguments     : f4_ed - d-axis BEMF
*                 f4_eq - q-axis BEMF
*                 f4_id - d-axis current reference
*                 f4_torque_current - torque current when open-loop mode
*                 f4_phase_err - phase error between the real motor axis and the controlled axis
* Return Value  : Iq reference
***********************************************************************************************************************/
float motor_speed_opl2less_iq_calc(float f4_ed,
                                   float f4_eq,
                                   float f4_id,
                                   float f4_torque_current,
                                   float f4_phase_err);

/***********************************************************************************************************************
* Function Name: motor_speed_opl2less_torque_current_calc
* Description  : Calculates the torque current required for eliminating the phase error
* Arguments    : p_st_motor           - Motor parameters structure(pointer)
*                f4_opl2less_sw_time  - The transition period [s] from open-loop to normal FOC
*                f4_ol_id_ref         - The Id reference in open-loop drive mode
*                f4_phase_err_rad_lpf - The phase error [rad] processed by an LPF
* Return Value : Torque current required [A]
***********************************************************************************************************************/
float motor_speed_opl2less_torque_current_calc(const st_motor_parameter_t * p_st_motor,
                                               float f4_opl2less_sw_time,
                                               float f4_ol_id_ref,
                                               float f4_phase_err_rad_lpf);

#endif /* R_MOTOR_SPEED_OPL2LESS_H */
