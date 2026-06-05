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
* File Name   : r_pfc_common.h
* Description : This header includes common definitions
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_COMMON_H
#define R_PFC_COMMON_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* General definition */
#define     PFC_TWOPI                (2.0f * 3.1415926535f)         /* 2 * pi */
#define     PFC_TWOPI_INV            (1.0f / PFC_TWOPI)             /* 1 / (2 * pi) */
#define     PFC_HALF_PI              (0.5f * 3.1415926535f)         /* 0.5 * pi */
#define     PFC_RPM2RAD              (PFC_TWOPI / 60.0f)            /* for rpm -> rad/s transration ((2 * pi) / 60) */
#define     PFC_RAD2RPM              (60.0f / PFC_TWOPI)            /* for rad/s -> rpm transration (60 / (2 * pi)) */
#define     PFC_DEG2RAD              (PFC_TWOPI / 360.0f)           /* for degree -> rad transration ((2 * pi) / 360) */
#define     PFC_RAD2DEG              (360.0f / PFC_TWOPI)           /* for rad -> degree transration (360 / (2 * pi)) */
#define     PFC_SQRT_2               (1.41421356f)                  /* Sqrt(2) */
#define     PFC_SQRT_3               (1.7320508f)                   /* Sqrt(3) */

#define     PFC_LED_ON               (0)                            /* Active level of LED */
#define     PFC_LED_OFF              (1)                            /* Inactive level of LED */

#define     PFC_FLG_CLR              (0)                            /* For flag clear */
#define     PFC_FLG_SET              (1)                            /* For flag set */

#define     PFC_ENABLE               (1)                            /* For function enable */
#define     PFC_DISABLE              (0)                            /* For function disable */

/* User macro definition */
#define     PFC_TRUE                 (1)
#define     PFC_FALSE                (0)

#define     PFC_ON                   (1)
#define     PFC_OFF                  (0)

#define     PFC_HIGH                 (1)
#define     PFC_LOW                  (0)

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/

#endif /* R_PFC_COMMON_H */
