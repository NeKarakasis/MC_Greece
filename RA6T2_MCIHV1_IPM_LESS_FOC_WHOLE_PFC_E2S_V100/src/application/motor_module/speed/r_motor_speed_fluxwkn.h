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
* File Name    : r_motor_speed_fluxwkn.h
* Description  : Definitions for flux weakening module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_SPEED_FLUXWKN_H
#define R_MOTOR_SPEED_FLUXWKN_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define FLUXWKN_DEF_VFWRATIO            (0.95f)     /* Defines default flux-weakening voltage ratio */
#define FLUXWKN_DEF_VFWRATIO_MIN        (0.5f)      /* Defines default minimum flux-weakening voltage ratio */

/**
 * Flux-weakening-states
 */
#define FLUXWKN_STATE_BYPASSED          (0x0000)    /* Flux-weakening control is not necessary, module is bypassed */
#define FLUXWKN_STATE_FLUXWKN           (0x0001)    /* Normal flux-weakening state */
#define FLUXWKN_STATE_IDSAT             (0x0002)    /* D-axis current has already saturated */
#define FLUXWKN_STATE_ERROR             (0x8000)    /* Flux-weakening general/runtime error */
#define FLUXWKN_STATE_INVALID_MOTOR     (0x9001)    /* Motor pointer is null or either of Ld, Lq, flux is below 0 */
#define FLUXWKN_STATE_INVALID_IAMAX     (0x9002)    /* Invalid maximum Ia value (Iamax < 0.0) */
#define FLUXWKN_STATE_INVALID_VAMAX     (0x9003)    /* Invalid maximum Va value (Vamax < 0.0)  */
#define FLUXWKN_STATE_INVALID_VFWRATIO  (0x9004)    /* Invalid flux-weakening maximum voltage ratio, that f4_vfw_ratio
                                                     * is out of range [VFWRATIO_MIN, 1.0]  */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/**
 * Flux-weakening data structure
 */
typedef struct
{
    /* User configurations */
    const st_motor_parameter_t *p_motor;  /* The pointer to the motor parameter data structure */
    float               f4_ia_max;        /* The maximum magnitude of current vector */
    float               f4_va_max;        /* The maximum magnitude of voltage vector */
    float               f4_vfw_ratio;     /* The maximum ratio of voltage used by flux-weakening module */

    /* Internal data */
    float               f4_id_demag;      /* The demagnetization current derived by (-flux/Ld) */
    float               f4_id_min;        /* The minimum Id output, min(-Ia,Id_demag) */
    float               f4_v_fw;          /* The target abs. limit value of voltage vector due to the total flux */
    uint16_t            u2_fw_status;     /* The status of flux-weakening */
} st_fluxwkn_t;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void motor_speed_flux_weakn_init(st_fluxwkn_t * p_st_fluxwkn,
                                 float f4_ia_max,
                                 float f4_va_max,
                                 float f4_vfw_ratio,
                                 const st_motor_parameter_t *p_st_motor);
void motor_speed_flux_weakn_reset(st_fluxwkn_t * p_st_fluxwkn);
uint16_t motor_speed_flux_weakn_start(st_fluxwkn_t * p_st_fluxwkn,
                                      float f4_speed_rad,
                                      const float *p_f4_idq,
                                      float *p_f4_idq_ref);
void motor_speed_flux_weakn_motor_set(st_fluxwkn_t * p_st_fluxwkn, const st_motor_parameter_t * p_st_motor);
void motor_speed_flux_weakn_iamax_set(st_fluxwkn_t * p_st_fluxwkn, float f4_ia_max);
void motor_speed_flux_weakn_vamax_set(st_fluxwkn_t * p_st_fluxwkn, float f4_va_max);
void motor_speed_flux_weakn_vfw_ratio_set(st_fluxwkn_t * p_st_fluxwkn, float f4_vfw_ratio);
float motor_speed_flux_weakn_iamax_get(st_fluxwkn_t * p_st_fluxwkn);
float motor_speed_flux_weakn_vamax_get(st_fluxwkn_t * p_st_fluxwkn);
float motor_speed_flux_weakn_vfw_ratio_get(st_fluxwkn_t * p_st_fluxwkn);
uint16_t motor_speed_flux_weakn_status_get(st_fluxwkn_t * p_st_fluxwkn);
uint8_t motor_speed_flux_weakn_error_check(st_fluxwkn_t * p_st_fluxwkn);
uint8_t motor_speed_flux_weakn_invalid_parameter_check(st_fluxwkn_t * p_st_fluxwkn);
uint8_t motor_speed_flux_weakn_run_time_error_check(st_fluxwkn_t * p_st_fluxwkn);

#endif /* R_MOTOR_SPEED_FLUXWKN_H */
