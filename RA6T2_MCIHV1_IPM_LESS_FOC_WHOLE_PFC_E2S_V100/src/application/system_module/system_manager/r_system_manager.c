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
* File Name   : r_system_manager.c
* Description : The processes of system manager
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_system_manager_api.h"
#include "r_system_manager.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
uint16_t g_u2_pfc_status_pre;    /* Previously obtained value */
uint8_t  g_u1_relay_flg = MTR_FLG_CLR;

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : system_manager_pfc_error_process
* Description   : Check PFC module error and set error to motor module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void system_manager_pfc_error_process(void)
{
    uint16_t u2_pfc_status;

    /* Get PFC status */
    u2_pfc_status = R_SYSTEM_MANAGER_PfcStatusGet();

    if (u2_pfc_status != g_u2_pfc_status_pre)
    {
        if (PFC_MODE_PFC_ERROR == u2_pfc_status)
        {
            g_st_sensorless_vector.u2_error_status |= MOTOR_SENSORLESS_VECTOR_ERROR_PFC;
            R_MOTOR_SENSORLESS_VECTOR_ErrorSet(&g_st_sensorless_vector);
        }
    }

    g_u2_pfc_status_pre = u2_pfc_status;
} /* End of function system_manager_pfc_error_process */

/***********************************************************************************************************************
* Function Name : system_manager_pfc_relay_status_judgment
* Description   : Detect relay switching
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void system_manager_pfc_relay_status_judgment(void)
{
    uint8_t u1_relay_flg;

    u1_relay_flg = g_st_pfc_manager.u1_relay_ctrl_on_flg;

    if ((MTR_FLG_CLR == g_u1_relay_flg) && (MTR_FLG_SET == u1_relay_flg))
    {
        /* Relay OFF -> ON */
        g_st_sensorless_vector.u1_relay_first_on = MTR_FLG_SET;
    }
    else if ((MTR_FLG_SET == g_u1_relay_flg) && (MTR_FLG_CLR == u1_relay_flg))
    {
        /* Relay ON -> OFF */
        if (STATEMACHINE_STATE_RUN == R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector))
        {
            R_MOTOR_SENSORLESS_VECTOR_MotorStop(&g_st_sensorless_vector);
        }
    }
    else
    {
        /* Do nothing */
    }

    g_u1_relay_flg = u1_relay_flg;
} /* End of function system_manager_pfc_relay_status_judgment */
