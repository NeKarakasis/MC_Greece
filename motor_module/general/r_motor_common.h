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
* File Name   : r_motor_common.h
* Description : This header includes common definitions
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.06.2021 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_COMMON_H
#define R_MOTOR_COMMON_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* General definition */
#define     MTR_TWOPI                (2.0f * 3.1415926535f)         /* 2 * pi */
#define     MTR_TWOPI_INV            (1.0f / MTR_TWOPI)             /* 1 / (2 * pi) */
#define     MTR_RPM2RAD              (MTR_TWOPI / 60.0f)            /* for rpm -> rad/s transration (2 * pi / 60) */
#define     MTR_RAD2RPM              (60.0f / MTR_TWOPI)            /* for rad/s -> rpm transration (60 / 2 * pi) */
#define     MTR_DEG2RAD              (MTR_TWOPI / 360.0f)           /* for degree -> rad transration (2 * pi / 360) */
#define     MTR_RAD2DEG              (360.0f / MTR_TWOPI)           /* for rad -> degree transration (360 / 2 * pi) */
#define     MTR_SQRT_2               (1.41421356f)                  /* Sqrt(2) */
#define     MTR_SQRT_3               (1.7320508f)                   /* Sqrt(3) */

#define     MTR_CW                   (0)                            /* Direction clock wise */
#define     MTR_CCW                  (1)                            /* Direction counter clock wise */

#define     MTR_LED_ON               (0)                            /* Active level of LED */
#define     MTR_LED_OFF              (1)                            /* Inactive level of LED */

#define     MTR_FLG_CLR              (0)                            /* For flag clear */
#define     MTR_FLG_SET              (1)                            /* For flag set */

#define     MTR_ENABLE               (1)                            /* For function enable */
#define     MTR_DISABLE              (0)                            /* For function disable */

/* User macro definition */
#define     MTR_TRUE                 (1)
#define     MTR_FALSE                (0)

#define     MTR_ON                   (1)
#define     MTR_OFF                  (0)

#define     MTR_HIGH                 (1)
#define     MTR_LOW                  (0)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/* Motor parameter structure */
typedef struct
{
    uint16_t    u2_mtr_pp;                        /* Pole pairs */
    float       f4_mtr_r;                         /* Resistance [ohm] */
    float       f4_mtr_ld;                        /* Inductance for d-axis [H] */
    float       f4_mtr_lq;                        /* Inductance for q-axis [H] */
    float       f4_mtr_m;                         /* Magnet flux [Wb] */
    float       f4_mtr_j;                         /* Rotor inertia [kgm^2] */
    float       f4_nominal_current_rms;           /* Nominal current [Arms] */
} st_motor_parameter_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/

#endif /* R_MOTOR_COMMON_H */
