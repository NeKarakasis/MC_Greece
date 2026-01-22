/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_app_mcu.h
* Description : Header for MCU depend application layer functions
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
 *********************************************************************************************************************/
#ifndef R_APP_MCU_H
#define R_APP_MCU_H
/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "ICS2_RA6T2.h"
#include "hal_data.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define     ICS_DECIMATION               (3)                  /* ICS watch skipping number */

/* For ICS */
#define     ICS_BRR                      (1)                 /* Baudrate select */
#define     ICS_INT_MODE                 (1)                  /* Mode select */
#define     CONF_MOTOR_TYPE              ("Brushless DC Motor")
#define     CONF_CONTROL                 ("Sensorless vector control (Speed control)")
#define     CONF_INVERTER                ("MCI-HV-1")
#define     CONF_MOTOR_TYPE_LEN          (18)
#define     CONF_CONTROL_LEN             (41)
#define     CONF_INVERTER_LEN            (8)

/* design parameter */
#define     APP_CFG_FREQ_BAND_LIMIT      (3.0f)               /* Motor control natural frequency limit */
#define     APP_CFG_MAX_CURRENT_OMEGA    (1000.0f)            /* Max natural frequency of current loop [Hz] */
#define     APP_CFG_MIN_OMEGA            (1.0f)               /* Min natural frequency of control loop [Hz] */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void r_app_rmw_hw_init(void);
void r_app_hal_init(void);
void r_app_main_loop(void);
void r_app_pwm_highz_reset(void);
void r_app_rmw_watchpoint(void);

#endif /* R_APP_MCU_H */
