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
* File Name   : r_pfc_ctrl_api.c
* Description : The PFC control module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_pfc_cfg.h"
#include "r_pfc_ctrl.h"
#include "r_pfc_ctrl_api.h"
#include "r_pfc_filter.h"

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_pfc_ctrl_t g_st_pfc_ctrl;

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_Open
* Description   : Open PFC control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_Open(void)
{
    st_pfc_ctrl_t *p_st_pfc_ctrl;
    p_st_pfc_ctrl = &g_st_pfc_ctrl;

    p_st_pfc_ctrl->f4_vac_ad          = 0.0f;
    p_st_pfc_ctrl->f4_vdc_ad          = 0.0f;
    p_st_pfc_ctrl->f4_ichp_ad         = 0.0f;
    p_st_pfc_ctrl->f4_vac_rms         = 0.0f;
    p_st_pfc_ctrl->f4_ref_vdc_ctrl    = 0.0f;
    p_st_pfc_ctrl->f4_ref_ichp_ctrl   = 0.0f;
    p_st_pfc_ctrl->f4_power_max_limit = PFC_OUT_MAX_POWER * PFC_OUT_POWER_COEF;
    p_st_pfc_ctrl->f4_power_min_limit = PFC_OUT_MIN_POWER * PFC_OUT_POWER_COEF;
    p_st_pfc_ctrl->f4_power_slope     = (p_st_pfc_ctrl->f4_power_max_limit - p_st_pfc_ctrl->f4_power_min_limit)
                                        / (200.0f - 100.0f);    /* (P_max - P_min) / (200Vrms / 100Vrms) */

    p_st_pfc_ctrl->f4_avr_ff_comp_min_limit     = PFC_AVR_FF_COMP_MIN_LIMIT;
    p_st_pfc_ctrl->f4_acr_ff_comp_min_limit     = PFC_ACR_FF_COMP_MIN_LIMIT;
    p_st_pfc_ctrl->u2_duty_ff_softstart_cnt     = 0U;
    p_st_pfc_ctrl->u2_duty_ff_softstart_max_cnt = (uint16_t)PFC_ACR_DUTY_FF_CTRL_CNT_LIMIT;
    p_st_pfc_ctrl->f4_duty_ff_softstart_up_step = 1.0f / PFC_ACR_DUTY_FF_CTRL_CNT_LIMIT;
    p_st_pfc_ctrl->f4_duty_ff_comp              = 0.0f;
    p_st_pfc_ctrl->f4_duty_ff_comp_coef         = PFC_ACR_DUTY_FF_COMP_COEF;
    p_st_pfc_ctrl->f4_duty                      = 0.0f;
    p_st_pfc_ctrl->f4_duty_max                  = PFC_DUTY_MAX;
    p_st_pfc_ctrl->f4_duty_min                  = PFC_DUTY_MIN;
    p_st_pfc_ctrl->f4_avr_pi_output             = 0.0f;
    p_st_pfc_ctrl->f4_acr_pi_output             = 0.0f;
    p_st_pfc_ctrl->u1_duty_lower_limit_flg      = 0U;

    pfc_pi_ctrl_kp_set(&p_st_pfc_ctrl->st_pi_voltage, PFC_AVR_KP);
    pfc_pi_ctrl_ki_set(&p_st_pfc_ctrl->st_pi_voltage, PFC_AVR_KI);
    pfc_pi_ctrl_integral_limit_set(&p_st_pfc_ctrl->st_pi_voltage, PFC_AVR_LIMIT);

    pfc_pi_ctrl_kp_set(&p_st_pfc_ctrl->st_pi_current, PFC_ACR_KP);
    pfc_pi_ctrl_ki_set(&p_st_pfc_ctrl->st_pi_current, PFC_ACR_KI);
    pfc_pi_ctrl_integral_limit_set(&p_st_pfc_ctrl->st_pi_current, PFC_ACR_LIMIT);
} /* End of function R_PFC_CTRL_Open */

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_Close
* Description   : Close PFC control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_Close(void)
{
    st_pfc_ctrl_t *p_st_pfc_ctrl;
    p_st_pfc_ctrl = &g_st_pfc_ctrl;

    p_st_pfc_ctrl->f4_vac_ad        = 0.0f;
    p_st_pfc_ctrl->f4_vdc_ad        = 0.0f;
    p_st_pfc_ctrl->f4_ichp_ad       = 0.0f;
    p_st_pfc_ctrl->f4_vac_rms       = 0.0f;
    p_st_pfc_ctrl->f4_ref_vdc_ctrl  = 0.0f;
    p_st_pfc_ctrl->f4_ref_ichp_ctrl = 0.0f;

    R_PFC_CTRL_Reset(p_st_pfc_ctrl);
} /* End of function R_PFC_CTRL_Close */

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_Reset
* Description   : Reset PFC control module
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_Reset(st_pfc_ctrl_t *p_st_pfc_ctrl)
{
    p_st_pfc_ctrl->f4_ref_ichp_ctrl         = 0.0f;
    p_st_pfc_ctrl->u2_duty_ff_softstart_cnt = 0U;
    p_st_pfc_ctrl->f4_duty_ff_comp          = 0.0f;
    p_st_pfc_ctrl->f4_duty                  = PFC_DUTY_MIN;
    p_st_pfc_ctrl->f4_avr_pi_output         = 0.0f;
    p_st_pfc_ctrl->f4_acr_pi_output         = 0.0f;
    p_st_pfc_ctrl->u1_duty_lower_limit_flg  = 0U;

    pfc_pi_ctrl_reset(&p_st_pfc_ctrl->st_pi_voltage);
    pfc_pi_ctrl_reset(&p_st_pfc_ctrl->st_pi_current);
} /* End of function R_PFC_CTRL_Reset */

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_ParameterSet
* Description   : Set the PFC control parameter
* Arguments     : p_st_pfc_ctrl       - The pointer to the PFC control parameter structure
*                 p_st_pfc_ctrl_input - The pointer to the PFC control input parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_ParameterSet(st_pfc_ctrl_t *p_st_pfc_ctrl, const st_pfc_ctrl_input_t * p_st_pfc_ctrl_input)
{
    p_st_pfc_ctrl->f4_vac_ad       = p_st_pfc_ctrl_input->f4_vac_ad;
    p_st_pfc_ctrl->f4_vdc_ad       = p_st_pfc_ctrl_input->f4_vdc_ad;
    p_st_pfc_ctrl->f4_ichp_ad      = p_st_pfc_ctrl_input->f4_ichp_ad;
    p_st_pfc_ctrl->f4_vac_rms      = p_st_pfc_ctrl_input->f4_vac_rms;
    p_st_pfc_ctrl->f4_ref_vdc_ctrl = p_st_pfc_ctrl_input->f4_ref_vdc_ctrl;
} /* End of function R_PFC_CTRL_ParameterSet */

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_ParameterGet
* Description   : Get the PFC control parameter
* Arguments     : p_st_pfc_ctrl        - The pointer to the PFC control parameter structure
*                 p_st_pfc_ctrl_output - The pointer to the PFC control output parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_ParameterGet(st_pfc_ctrl_t *p_st_pfc_ctrl, st_pfc_ctrl_output_t *p_st_pfc_ctrl_output)
{
    p_st_pfc_ctrl_output->f4_duty                 = p_st_pfc_ctrl->f4_duty;
    p_st_pfc_ctrl_output->u1_duty_lower_limit_flg = p_st_pfc_ctrl->u1_duty_lower_limit_flg;
} /* End of function R_PFC_CTRL_ParameterGet */

/***********************************************************************************************************************
* Function Name : R_PFC_CTRL_Cyclic
* Description   : The cyclic processing of PFC control
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_CTRL_Cyclic(st_pfc_ctrl_t *p_st_pfc_ctrl)
{
    float f4_power_limit;

    /* PFC output power limit */
    f4_power_limit = pfc_ctrl_power_limit(p_st_pfc_ctrl, p_st_pfc_ctrl->f4_vac_rms);
    pfc_pi_ctrl_integral_limit_set(&p_st_pfc_ctrl->st_pi_voltage, f4_power_limit);

    /* Feedback control (Voltage) */
    p_st_pfc_ctrl->f4_avr_pi_output = pfc_ctrl_voltage_pi_control(p_st_pfc_ctrl);

    /* Anti-windup control for the automatic voltage regulator */
    pfc_ctrl_avr_anti_windup_control(p_st_pfc_ctrl, p_st_pfc_ctrl->f4_avr_pi_output, f4_power_limit, 0.0f);

    /* Voltage feed-forward compensation */
    p_st_pfc_ctrl->f4_ref_ichp_ctrl = pfc_ctrl_avr_ff_comp(p_st_pfc_ctrl, p_st_pfc_ctrl->f4_avr_pi_output);

    /* Feedback control (Current) */
    p_st_pfc_ctrl->f4_acr_pi_output = pfc_ctrl_current_pi_control(p_st_pfc_ctrl);

    /* Duty ratio feed-forward compensation */
    p_st_pfc_ctrl->f4_duty = pfc_ctrl_acr_ff_comp(p_st_pfc_ctrl, p_st_pfc_ctrl->f4_acr_pi_output);

    /* Anti-windup control for the automatic current regulator */
    pfc_ctrl_acr_anti_windup_control(p_st_pfc_ctrl,
                                     p_st_pfc_ctrl->f4_duty,
                                     p_st_pfc_ctrl->f4_duty_max,
                                     p_st_pfc_ctrl->f4_duty_min);

    /* Duty limit */
    p_st_pfc_ctrl->f4_duty = pfc_ctrl_duty_limit(p_st_pfc_ctrl,
                                                 p_st_pfc_ctrl->f4_duty,
                                                 p_st_pfc_ctrl->f4_duty_max,
                                                 p_st_pfc_ctrl->f4_duty_min);
} /* End of function R_PFC_CTRL_Cyclic */
