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
* File Name   : r_pfc_manager_api.c
* Description : The processes of accessing driver
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <math.h>
#include <r_pfc_manager_api.h>

/* Main associated header file */
#include "r_pfc_manager.h"

/* Module headers */
/* Submodule headers */
#include "r_pfc_manager_protection.h"

/* General library headers */
#include "r_pfc_common.h"

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
st_pfc_manager_t g_st_pfc_manager;

/***********************************************************************************************************************
* Function Name : R_PFC_MANAGER_Open
* Description   : Open PFC manager module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_MANAGER_Open(void)
{
    R_PFC_CTRL_Open();
    R_PFC_DRIVER_Open();

    /* Store the address of the module instance in a structure and connect the module to be used */
    g_st_pfc_manager.p_st_pfc_ctrl   = &g_st_pfc_ctrl;
    g_st_pfc_manager.p_st_pfc_driver = &g_st_pfc_driver;

    pfc_manager_default_init(&g_st_pfc_manager);
} /* End of function R_PFC_MANAGER_Open */

/***********************************************************************************************************************
* Function Name : R_PFC_MANAGER_Close
* Description   : Close PFC manager module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_MANAGER_Close(void)
{
    R_PFC_CTRL_Close();
    R_PFC_DRIVER_Close();
    pfc_manager_reset(&g_st_pfc_manager);
} /* End of function R_PFC_MANAGER_Close */

/**********************************************************************************************************************
* Function Name : R_PFC_MANAGER_Reset
* Description   : Reset PFC drive modules, configurations will not be reset
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_MANAGER_Reset(st_pfc_manager_t * p_st_pfc_manager)
{
    pfc_manager_reset(p_st_pfc_manager);
} /* End of function R_PFC_MANAGER_Reset */

/**********************************************************************************************************************
 * Function Name: R_PFC_MANAGER_ErrorCancel
 * Description  : Cancel error
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
 * Return Value : None
 *********************************************************************************************************************/
void R_PFC_MANAGER_ErrorCancel(st_pfc_manager_t * p_st_pfc_manager)
{
    if (p_st_pfc_manager->u2_run_mode == PFC_MODE_PFC_ERROR)
    {
        p_st_pfc_manager->u1_error_cancel_flg = PFC_FLG_SET;
    }
} /* End of function R_PFC_MANAGER_ErrorCancel */

/***********************************************************************************************************************
* Function Name : R_PFC_MANAGER_StatusGet
* Description   : Get status of PFC control system
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : PFC status
***********************************************************************************************************************/
uint16_t R_PFC_MANAGER_StatusGet(st_pfc_manager_t * p_st_pfc_manager)
{
    return (p_st_pfc_manager->u2_run_mode);
} /* End of function R_PFC_MANAGER_StatusGet */

/***********************************************************************************************************************
* Function Name : R_PFC_MANAGER_Main
* Description   : Main routine for PFC Control
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_MANAGER_Main(st_pfc_manager_t * p_st_pfc_manager)
{
    st_pfc_ctrl_input_t st_pfc_ctrl_input;
    float f4_temp0;

    /* AD conversion */
    R_PFC_DRIVER_AnalogGet(p_st_pfc_manager->p_st_pfc_driver,
                           &p_st_pfc_manager->f4_vac_ad,
                           &p_st_pfc_manager->f4_vdc_ad,
                           &p_st_pfc_manager->f4_ichp_ad);

    /* VAC_filter */
    pfc_manager_fil_vac_calc(&p_st_pfc_manager->st_ac_fil, p_st_pfc_manager->f4_vac_ad);

    /* Notch filter */
    f4_temp0 = pfc_filter_notch(&p_st_pfc_manager->st_vdc_notch_fil, p_st_pfc_manager->f4_vdc_ad);

    /* ADC Data filtering */
    p_st_pfc_manager->f4_vac_ad_lpf = pfc_filter_first_order_lpff(&p_st_pfc_manager->st_vac_ad_lpf, p_st_pfc_manager->f4_vac_ad);
    p_st_pfc_manager->f4_vdc_ad_lpf = pfc_filter_first_order_lpff(&p_st_pfc_manager->st_vdc_ad_lpf, f4_temp0);

    /* Error check */
    pfc_manager_error_check(p_st_pfc_manager);

    /* Voltage dip detection */
    pfc_manager_voltage_dip_detection(p_st_pfc_manager);

    /* Relay ON/OFF */
    pfc_manager_relay_ctrl(p_st_pfc_manager);

    /* State machine */
    pfc_manager_state_machine_process(p_st_pfc_manager);

    /* Set Vdc reference value */
    p_st_pfc_manager->f4_ref_vdc_ctrl = pfc_manager_reference_vdc_set(p_st_pfc_manager);

    /* Set PFC control input */
    st_pfc_ctrl_input.f4_vac_ad       = p_st_pfc_manager->f4_vac_ad_lpf;
    st_pfc_ctrl_input.f4_vdc_ad       = p_st_pfc_manager->f4_vdc_ad_lpf;
    st_pfc_ctrl_input.f4_ichp_ad      = p_st_pfc_manager->f4_ichp_ad;
    st_pfc_ctrl_input.f4_vac_rms      = p_st_pfc_manager->st_ac_fil.f4_data_rms;
    st_pfc_ctrl_input.f4_ref_vdc_ctrl = p_st_pfc_manager->f4_ref_vdc_ctrl;
    R_PFC_CTRL_ParameterSet(p_st_pfc_manager->p_st_pfc_ctrl, &st_pfc_ctrl_input);

    /* Control start */
    if ((PFC_FLG_SET == p_st_pfc_manager->u1_pfc_ctrl_enable_flg) && (PFC_FLG_CLR == p_st_pfc_manager->u1_volt_dip_flg))
    {
        if (PFC_FLG_SET == p_st_pfc_manager->st_pfc_ctrl_output.u1_duty_lower_limit_flg)
        {
            R_PFC_DRIVER_PWMControlStop(p_st_pfc_manager->p_st_pfc_driver);    /* PWM output disable */
        }
        else
        {
            R_PFC_DRIVER_PWMControlStart(p_st_pfc_manager->p_st_pfc_driver);    /* PWM output enable */
        }
        R_PFC_CTRL_Cyclic(p_st_pfc_manager->p_st_pfc_ctrl);
        R_PFC_CTRL_ParameterGet(p_st_pfc_manager->p_st_pfc_ctrl, &p_st_pfc_manager->st_pfc_ctrl_output);

        /* PWM reference setting */
        R_PFC_DRIVER_DutySet(p_st_pfc_manager->p_st_pfc_driver, p_st_pfc_manager->st_pfc_ctrl_output.f4_duty);
    }
    else
    {
        R_PFC_DRIVER_PWMControlStop(p_st_pfc_manager->p_st_pfc_driver);    /* PWM output disable */
        pfc_manager_reset(p_st_pfc_manager);
    }
} /* End of function R_PFC_MANAGER_Main */

/***********************************************************************************************************************
* Function Name : R_PFC_MANAGER_OverCurrentInterrupt
* Description   : PFC H/W over-current interrupt processing
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_MANAGER_OverCurrentInterrupt(st_pfc_manager_t * p_st_pfc_manager)
{
    R_PFC_DRIVER_PWMControlStop(p_st_pfc_manager->p_st_pfc_driver);    /* PWM output disable */
    pfc_manager_error_process(p_st_pfc_manager, PFC_MANAGER_ERROR_OVER_CURRENT_HW);
    p_st_pfc_manager->u1_pfc_ctrl_enable_flg = PFC_FLG_CLR;
    p_st_pfc_manager->u2_run_mode = PFC_MODE_PFC_ERROR;
} /* End of function R_PFC_MANAGER_OverCurrentInterrupt */
