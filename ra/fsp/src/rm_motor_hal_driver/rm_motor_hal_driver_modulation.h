/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup MOTOR_HAL_DRIVER
 * @{
 **********************************************************************************************************************/

#ifndef RM_MOTOR_HAL_DRIVER_MODULATION_H
#define RM_MOTOR_HAL_DRIVER_MODULATION_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>

#ifndef CCAL_FLOAT
 #define CCAL_FLOAT
#endif
#include "rm_motor_shared_ccal_types.h"
#include "rm_motor_shared_ccal_macros.h"
#include "rm_motor_hal_driver.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_run
 * Description  : Calculates duty cycle from input 3-phase voltage (bipolar)
 * Arguments    : p_ctrl -
 *                    Pointer to the motor driver module valuables
 *              : p_v_in -
 *                    Pointer to the 3-phase input voltage
 *              : p_duty_out -
 *                    Where to store the 3-phase output duty cycle
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_run(motor_hal_driver_modulation_t * p_mod,
                                        motor_hal_driver_modulation_t * p_cfg_mod,
                                        const ccal_num_t              * p_v_in,
                                        ccal_num_t                    * p_duty_out,
                                        uint8_t                       * p_maxvoltage_flag,
                                        uint8_t                         modulation_method);

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_set_max_duty
 * Description  : Sets the maximum duty cycle
 * Arguments    : p_mod -
 *                    Pointer to the modulation data structure
 *                max_duty -
 *                    Maximum duty cycle to set
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_set_max_duty(motor_hal_driver_modulation_t * p_mod, ccal_num_t max_duty);

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_set_min_duty
 * Description  : Sets the minimum duty cycle
 * Arguments    : p_mod -
 *                    Pointer to the modulation data structure
 *                min_duty -
 *                    Minimum duty cycle to set
 * Return Value : None
 **********************************************************************************************************************/
void rm_motor_hal_driver_modulation_set_min_duty(motor_hal_driver_modulation_t * p_mod, ccal_num_t min_duty);

/***********************************************************************************************************************
 * Function Name: rm_motor_hal_driver_modulation_get_vamax
 * Description  : Gets the maximum magnitude of voltage vector
 * Arguments    : p_mod -
 *                    The pointer to the modulation data structure
 *                u1_method -
 *                    Modulation method (SPWM, SVPWM, DPWM)
 * Return Value : The maximum magnitude of voltage vector
 **********************************************************************************************************************/
ccal_num_t rm_motor_hal_driver_modulation_get_vamax(motor_hal_driver_modulation_t * p_mod, uint8_t u1_method);

#endif                                 // RM_MOTOR_HAL_DRIVER_MODULATION_H

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_MOTOR_HAL_DRIVER_MODULATION)
 **********************************************************************************************************************/
