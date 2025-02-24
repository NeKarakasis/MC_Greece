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
* File Name   : r_app_board_ui_ctrl.h
* Description : Definitions of processes of a H/W control layer(CPU and Inverter board dependence)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 17.06.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_APP_BOARD_UI_CTRL_H
#define R_APP_BOARD_UI_CTRL_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_vr1
* Description   : Get A/D converted value of VR1
* Arguments     : None
* Return Value  : A/D converted value of VR1
***********************************************************************************************************************/
//uint16_t r_app_board_ui_get_vr1(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_sw1
* Description   : Get state of SW1
* Arguments     : None
* Return Value  : State of SW1
***********************************************************************************************************************/
//uint8_t r_app_board_ui_get_sw1(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_get_sw2
* Description   : Get state of SW2
* Arguments     : None
* Return Value  : State of SW2
***********************************************************************************************************************/
//uint8_t r_app_board_ui_get_sw2(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led1_on
* Description   : Turn on LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led1_on(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led2_on
* Description   : Turn on LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led2_on(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led3_on
* Description   : Turn on LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led3_on(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led1_off
* Description   : Turn off LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led1_off(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led2_off
* Description   : Turn off LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led2_off(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led3_off
* Description   : Turn off LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
//void r_app_board_ui_led3_off(void);

#endif /* R_APP_BOARD_UI_CTRL_H */
