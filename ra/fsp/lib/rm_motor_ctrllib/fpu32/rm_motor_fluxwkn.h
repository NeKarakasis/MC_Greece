/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_FLUXWEAK_H
#define RM_MOTOR_LIB_FLUXWEAK_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define MOTOR_FLUXWKN_DEF_VFWRATIO              (0.95F)  /* Defines default flux-weakening voltage ratio */
#define MOTOR_FLUXWKN_DEF_VFWRATIO_MIN          (0.5F)   /* Defines default minimum flux-weakening voltage ratio */

/*
 * Flux-weakening-states
 */
#define MOTOR_FLUXWKN_STATE_BYPASSED            (0x0000) /* Flux-weakening control is not necessary, module is bypassed */
#define MOTOR_FLUXWKN_STATE_FLUXWKN             (0x0001) /* Normal flux-weakening state */
#define MOTOR_FLUXWKN_STATE_IDSAT               (0x0002) /* D-axis current has already saturated */
#define MOTOR_FLUXWKN_STATE_ERROR               (0x8000) /* Flux-weakening general/runtime error */
#define MOTOR_FLUXWKN_STATE_INVALID_MOTOR       (0x9001) /* Motor pointer is null or either of Ld, Lq, flux is below 0 */
#define MOTOR_FLUXWKN_STATE_INVALID_IAMAX       (0x9002) /* Invalid maximum Ia value (Iamax < 0.0) */
#define MOTOR_FLUXWKN_STATE_INVALID_VAMAX       (0x9003) /* Invalid maximum Va value (Vamax < 0.0)  */
#define MOTOR_FLUXWKN_STATE_INVALID_VFWRATIO    (0x9004) /* Invalid flux-weakening maximum voltage ratio, that f4_vfw_ratio
                                                          * is out of range [VFWRATIO_MIN, 1.0]  */
#define MOTOR_FLUXWKN_CHK_MASK                  (0xF000) /* Mask value to check error */
#define MOTOR_FLUXWKN_INVALID_CHK               (0x9000) /* Value to check Invalidate */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

typedef struct st_motor_flux_weakening
{
    /* User configurations */
    float f4_ia_max;                   ///< The maximum magnitude of current vector
    float f4_va_max;                   ///< The maximum magnitude of voltage vector
    float f4_vfw_ratio;                ///< The maximum ratio of voltage used by flux-weakening module

    /* Internal data */
    float    f4_id_demag;              ///< The demagnetization current derived by (-flux/Ld)
    float    f4_id_min;                ///< The minimum Id output, min(-Ia,Id_demag)
    float    f4_v_fw;                  ///< The target abs. limit value of voltage vector due to the total flux
    uint16_t u2_fw_status;             ///< The status of flux-weakening
} motor_flux_weakening_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

void rm_motor_fluxwkn_init(motor_flux_weakening_t       * p_fluxwkn,
                           float                          f4_ia_max,
                           float                          f4_va_max,
                           const motor_parameter_ramp_t * p_mtr);
void     rm_motor_fluxwkn_reset(motor_flux_weakening_t * p_fluxwkn);
void     rm_motor_fluxwkn_set_motor(motor_flux_weakening_t * p_fluxwkn, const motor_parameter_ramp_t * p_mtr);
void     rm_motor_fluxwkn_set_vamax(motor_flux_weakening_t * p_fluxwkn, float f4_va_max);
void     rm_motor_fluxwkn_set_iamax(motor_flux_weakening_t * p_fluxwkn, float f4_ia_max);
uint16_t rm_motor_fluxwkn_run(motor_flux_weakening_t       * p_fluxwkn,
                              const motor_parameter_ramp_t * p_mtr,
                              float                          f4_speed_rad,
                              float                          p_f4_id,
                              float                          p_f4_iq,
                              float                        * p_f4_id_ref,
                              float                        * p_f4_iq_ref);
uint8_t rm_motor_fluxwkn_check_bypass(motor_flux_weakening_t * p_fluxwkn);

#endif                                 /* RM_MOTOR_LIB_FLUXWEAK_H */
