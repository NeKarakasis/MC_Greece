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
/**********************************************************************************************************************
* File Name   : r_pfc_ctrl.h
* Description : Definitions of PFC control
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_CTRL_H
#define R_PFC_CTRL_H

/**********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_pfc_ctrl_api.h"

/**********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/**********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
float pfc_ctrl_power_limit(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_vrms);
float pfc_ctrl_voltage_pi_control(st_pfc_ctrl_t *p_st_pfc_ctrl);
void pfc_ctrl_avr_anti_windup_control(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out, float f4_max, float f4_min);
float pfc_ctrl_avr_ff_comp(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out);
float pfc_ctrl_current_pi_control(st_pfc_ctrl_t *p_st_pfc_ctrl);
float pfc_ctrl_acr_ff_comp(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out);
void pfc_ctrl_acr_anti_windup_control(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out, float f4_max, float f4_min);
float pfc_ctrl_duty_limit(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_duty, float f4_max, float f4_min);

#endif /* R_PFC_CTRL_H */
