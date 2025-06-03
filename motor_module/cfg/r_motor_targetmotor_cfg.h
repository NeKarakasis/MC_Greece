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
* File Name   : r_motor_targetmotor_cfg.h
* Description : Definition of the target motor parameters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_TARGETMOTOR_CFG_H
#define R_MOTOR_TARGETMOTOR_CFG_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/*
 * Hardware system configurations
 */
#define MOTOR_COMMON_CFG_MTR_TG55L          /* Default motor parameters : Tsukasa Electric */

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* Target motor parameter definitions */
#ifdef MOTOR_COMMON_CFG_MTR_TG55L

/*
#define     MOTOR_CFG_POLE_PAIRS            (4)              Number of pole pairs
#define     MOTOR_CFG_MAGNETIC_FLUX         (0.01119f)       Permanent magnetic flux [Wb]
#define     MOTOR_CFG_RESISTANCE            (1.3f)           Resistance [ohm]
#define     MOTOR_CFG_D_INDUCTANCE          (0.0013f)        D-axis inductance [H]
#define     MOTOR_CFG_Q_INDUCTANCE          (0.0013f)        Q-axis inductance [H]
#define     MOTOR_CFG_ROTOR_INERTIA         (0.000003666f)     Rotor inertia [kgm^2]
#define     MOTOR_CFG_NOMINAL_CURRENT_RMS   (5.0f)          Nominal current [Arms]
#define     MOTOR_CFG_MAX_SPEED_RPM         (2650.0f)        maximum speed [rpm] (mechanical angle)
*/

#define     MOTOR_CFG_POLE_PAIRS            (4)             /* Number of pole pairs */
#define     MOTOR_CFG_MAGNETIC_FLUX         (0.0086f)      /* Permanent magnetic flux [Wb] */
#define     MOTOR_CFG_RESISTANCE            (34.4f)          /* Resistance [ohm] */
#define     MOTOR_CFG_D_INDUCTANCE          (0.0534f)       /* D-axis inductance [H] */
#define     MOTOR_CFG_Q_INDUCTANCE          (0.0534f)       /* Q-axis inductance [H] */
#define     MOTOR_CFG_ROTOR_INERTIA         (0.0003666f)    /* Rotor inertia [kgm^2] */
#define     MOTOR_CFG_NOMINAL_CURRENT_RMS   (1.0f)         /* Nominal current [Arms] */
#define     MOTOR_CFG_MAX_SPEED_RPM         (5000.0f)       /* maximum speed [rpm] (mechanical angle) */

#endif /* MOTOR_COMMON_CFG_MTR_TG55L */

#endif /* R_MOTOR_TARGETMOTOR_CFG_H */

