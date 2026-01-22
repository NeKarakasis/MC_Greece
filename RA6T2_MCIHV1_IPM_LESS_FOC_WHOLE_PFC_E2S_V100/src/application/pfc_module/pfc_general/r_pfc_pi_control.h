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
* File Name   : r_pfc_pi_control.h
* Description : Definitions of general purpose PI control modules
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_PI_CONTROL_H
#define R_PFC_PI_CONTROL_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    float f4_err;         /* Error */
    float f4_kp;          /* Gain of proportional term */
    float f4_ki;          /* Gain of integral term */
    float f4_refp;        /* Buffer of proportional term */
    float f4_refi;        /* Buffer of integral term */
    float f4_ilimit;      /* Limit of integral term */
} st_pfc_pi_ctrl_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
float pfc_pi_ctrl(st_pfc_pi_ctrl_t *p_st_pi_ctrl);
void pfc_pi_ctrl_reset(st_pfc_pi_ctrl_t *p_st_pi_ctrl);
void pfc_pi_ctrl_kp_set(st_pfc_pi_ctrl_t *p_st_pi_ctrl, float f4_kp);
void pfc_pi_ctrl_ki_set(st_pfc_pi_ctrl_t *p_st_pi_ctrl, float f4_ki);
void pfc_pi_ctrl_integrator_set(st_pfc_pi_ctrl_t *p_st_pi_ctrl, float f4_integrator);
void pfc_pi_ctrl_integral_limit_set(st_pfc_pi_ctrl_t *p_st_pi_ctrl, float f4_ilimit);
float pfc_pi_ctrl_proportion_get(st_pfc_pi_ctrl_t *p_st_pi_ctrl);
float pfc_pi_ctrl_integrator_get(st_pfc_pi_ctrl_t *p_st_pi_ctrl);

#endif /* R_PFC_PI_CONTROL_H */
