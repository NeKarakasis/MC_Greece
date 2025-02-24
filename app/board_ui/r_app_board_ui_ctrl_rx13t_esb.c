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
* File Name   : r_app_board_ui_ctrl_rx13t_esb.ｃ
* Description : Processes of a H/W control layer(CPU and Inverter board dependence)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_app_board_ui_ctrl.h"

/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_vr1
* Description   : Get A/D converted value of VR1
* Arguments     : None
* Return Value  : A/D converted value of VR1
***********************************************************************************************************************/
uint16_t r_app_board_ui_get_vr1(void)
{
    uint16_t ad_data;

    R_Config_S12AD0_Get_ValueResult(ADCHANNEL7, &ad_data);      /* Read from VR1 */

    return (ad_data);
} /* End of function r_app_board_ui_get_vr1 */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_sw1
* Description   : Get state of SW1
* Arguments     : None
* Return Value  : State of SW1
***********************************************************************************************************************/
uint8_t r_app_board_ui_get_sw1(void)
{
    uint8_t tmp_port;

    tmp_port = R_Config_PORT_GetSW1();

    return (tmp_port);
} /* End of function r_app_board_ui_get_sw1 */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_sw2
* Description   : Get state of SW2
* Arguments     : None
* Return Value  : State of SW2
***********************************************************************************************************************/
uint8_t r_app_board_ui_get_sw2(void)
{
    uint8_t tmp_port;

    tmp_port = R_Config_PORT_GetSW2();

    return (tmp_port);
} /* End of function r_app_board_ui_get_sw2 */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led1_on
* Description   : Turn on LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led1_on(void)
{
    R_Config_PORT_Led1_on();
} /* End of function r_app_board_ui_led1_on */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led2_on
* Description   : Turn on LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led2_on(void)
{
    R_Config_PORT_Led2_on();
} /* End of function r_app_board_ui_led2_on */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led3_on
* Description   : Turn on LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led3_on(void)
{
    R_Config_PORT_Led3_on();
} /* End of function r_app_board_ui_led3_on */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led1_off
* Description   : Turn off LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led1_off(void)
{
    R_Config_PORT_Led1_off();
} /* End of function r_app_board_ui_led1_off */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led2_off
* Description   : Turn off LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led2_off(void)
{
    R_Config_PORT_Led2_off();
} /* End of function r_app_board_ui_led2_off */

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led3_off
* Description   : Turn off LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led3_off(void)
{
    R_Config_PORT_Led3_off();
} /* End of function r_app_board_ui_led3_off */
