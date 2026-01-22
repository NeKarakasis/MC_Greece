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
* File Name   : r_motor_inverter_cfg.h
* Description : Definitions of inverter board parameters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_INVERTER_CFG_H
#define R_MOTOR_INVERTER_CFG_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/*
 * Hardware system configurations
 */
#define MOTOR_COMMON_CFG_BOARD_HVI        /* High Voltage Inverter (MCI-HV-1) */

/***********************************************************************************************************************
* Target Inverter Define
***********************************************************************************************************************/
/* Target Inverter definitions */
#ifdef MOTOR_COMMON_CFG_BOARD_HVI

#define     INVERTER_CFG_SHUNT_RESIST           (0.2f)               /* Resistance of shunt resistor [ohm] */
#define     INVERTER_CFG_DEADTIME               (2.0f)                /* Deadtime [us] */
#define     INVERTER_CFG_VOLTAGE_GAIN           (201.0f)            /* Voltage measurement gain */
#define     INVERTER_CFG_CURRENT_AMP_GAIN       (2.00f)               /* Current amplifier gain */
#define     INVERTER_CFG_CURRENT_LIMIT          (21.2f)               /* Current limit[A] for the inverter board */
#define     INVERTER_CFG_OVERVOLTAGE_LIMIT      (450.0f)              /* Over voltage limit [V] */
#define     INVERTER_CFG_UNDERVOLTAGE_LIMIT     (100.0f)              /* Under voltage limit [V] */
#define     INVERTER_CFG_INPUT_V                (375.0f)              /* Nominal Vdc [V] */
#define     INVERTER_CFG_ADC_REF_VOLTAGE        (3.3f)                /* ADC reference voltage [V] */

#endif /* MOTOR_COMMON_CFG_BOARD_HVI */

/* Voltage error compensation User set value. */
#define     INVERTER_CFG_COMP_V0                (0.79f)
#define     INVERTER_CFG_COMP_V1                (2.25f)
#define     INVERTER_CFG_COMP_V2                (5.4f)
#define     INVERTER_CFG_COMP_V3                (8.78f)
#define     INVERTER_CFG_COMP_V4                (12.2f)

#define     INVERTER_CFG_COMP_I0                (0.124f)
#define     INVERTER_CFG_COMP_I1                (0.22f)
#define     INVERTER_CFG_COMP_I2                (0.48f)
#define     INVERTER_CFG_COMP_I3                (0.75f)
#define     INVERTER_CFG_COMP_I4                (0.99f)

#endif /* R_MOTOR_INVERTER_CFG_H */
