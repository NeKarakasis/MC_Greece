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
* File Name   : r_app_board_ui.h
* Description : Definitions of user interface on inverter board
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 17.06.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_APP_BOARD_UI_H
#define R_APP_BOARD_UI_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_module_cfg.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define BOARD_SW_ON                    (0)                          /* Active level of SW */
#define BOARD_SW_OFF                   (1)                          /* Inactive level of SW */
#define BOARD_CHATTERING_CNT           (10)                         /* Counts to remove chattering */

#define BOARD_AD12BIT_DATA             (MOTOR_MCU_CFG_AD12BIT_DATA) /* A/D 12Bit data */
#define BOARD_VR1_SPEED_DEAD_BAND      (80)                         /* speed dead-band [rpm] */
#define BOARD_VR1_SPEED_MARGIN         (50)                         /* speed margin [rpm] */
#define BOARD_VR1_SCALING_SPEED(ch)        ((MOTOR_CFG_MAX_SPEED_RPM(ch) + BOARD_VR1_SPEED_MARGIN) / (BOARD_AD12BIT_DATA/2 + 1))
                                                                    /* scaling factor for speed reference (A/D) */
                                                                    /* (MAX_SPEED + margin) / (A/D(12bit)/2+1) */
#define BOARD_ADJUST_OFFSET            (MOTOR_MCU_CFG_ADC_OFFSET)   /* Adjusting offset for reference */

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : r_app_board_ui_mainloop
* Description   : User interface using board UI
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_mainloop(void);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_led_control
* Description   : Set LED pattern depend on motor status
* Arguments     : u1_motor_status - Motor control status
* Return Value  : None
***********************************************************************************************************************/
void r_app_board_ui_led_control(uint8_t u1_motor_status);

/***********************************************************************************************************************
* Function Name : r_app_board_ui_remove_sw_chattering
* Description   : Get switch status with removing chattering
* Arguments     : u1_sw - Board interface switch signal
*                 u1_on_off - Detected status (ON/OFF)
* Return Value  : u1_remove_chattering_flg - Detection result
***********************************************************************************************************************/
uint8_t r_app_board_ui_remove_sw_chattering(uint8_t u1_sw, uint8_t u1_on_off);

#endif /* R_APP_BOARD_UI_H */
