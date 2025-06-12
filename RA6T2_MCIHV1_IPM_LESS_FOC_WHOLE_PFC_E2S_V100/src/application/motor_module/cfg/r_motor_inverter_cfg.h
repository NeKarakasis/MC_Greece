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
* File Name   : r_motor_inverter_cfg.h
* Description : Definitions of inverter board parameters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
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

#define     INVERTER_CFG_SHUNT_RESIST           (0.01f)               /* Resistance of shunt resistor [ohm] */
#define     INVERTER_CFG_DEADTIME               (2.0f)                /* Deadtime [us] */
#define     INVERTER_CFG_VOLTAGE_GAIN           (174.913f)            /* Voltage measurement gain */
#define     INVERTER_CFG_CURRENT_AMP_GAIN       (4.17f)               /* Current amplifier gain */
#define     INVERTER_CFG_CURRENT_LIMIT          (21.2f)               /* Current limit[A] for the inverter board */
#define     INVERTER_CFG_OVERVOLTAGE_LIMIT      (450.0f)              /* Over voltage limit [V] */
#define     INVERTER_CFG_UNDERVOLTAGE_LIMIT     (100.0f)              /* Under voltage limit [V] */
#define     INVERTER_CFG_INPUT_V                (380.0f)              /* Nominal Vdc [V] */
#define     INVERTER_CFG_ADC_REF_VOLTAGE        (3.3f)                /* ADC reference voltage [V] */

#endif /* MOTOR_COMMON_CFG_BOARD_HVI */

/* Voltage error compensation User set value. */
#define     INVERTER_CFG_COMP_V0                (0.624f)
#define     INVERTER_CFG_COMP_V1                (1.248f)
#define     INVERTER_CFG_COMP_V2                (1.872f)
#define     INVERTER_CFG_COMP_V3                (2.496f)
#define     INVERTER_CFG_COMP_V4                (3.120f)

#define     INVERTER_CFG_COMP_I0                (0.084f)
#define     INVERTER_CFG_COMP_I1                (0.168f)
#define     INVERTER_CFG_COMP_I2                (0.264f)
#define     INVERTER_CFG_COMP_I3                (0.360f)
#define     INVERTER_CFG_COMP_I4                (0.600f)

#endif /* R_MOTOR_INVERTER_CFG_H */
