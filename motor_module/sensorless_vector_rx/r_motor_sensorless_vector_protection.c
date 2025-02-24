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
* File Name   : r_motor_sensorless_vector_protection.c
* Description : The protection function
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <math.h>
#include <machine.h>

/* Project headers */
#include "r_smc_entry.h"
#include "r_motor_common.h"
#include "r_motor_sensorless_vector_manager.h"
#include "r_motor_sensorless_vector_protection.h"
#include "r_motor_current_api.h"
#include "r_motor_driver.h"

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_over_speed_error_check
* Description   : Checks over-speed error
* Arguments     : f4_speed_rad       - The electrical speed[rad/s]
*                 f4_speed_limit_rad - The speed[rad/s] threshold of the over-speed error, should be a positive value
* Return Value  : The over-speed error flag
***********************************************************************************************************************/
static inline uint16_t motor_sensorless_vector_over_speed_error_check(float f4_speed_rad, float f4_overspeed_limit_rad)
{
    float f4_temp0;
    uint16_t u2_temp0;

    u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_NONE;

    f4_temp0 = fabsf(f4_speed_rad);
    if (f4_temp0 > f4_overspeed_limit_rad)
    {
        u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_OVER_SPEED;            /* Over speed error */
    }

    return (u2_temp0);
} /* End of function motor_sensorless_vector_over_speed_error_check */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_over_voltage_error_check
* Description   : Checks over-voltage error
* Arguments     : f4_vdc               - The actual Vdc value [V]
                  f4_overvoltage_limit - The threshold voltage[V] of over-voltage error
* Return Value  : The flag of over-voltage error
***********************************************************************************************************************/
static inline uint16_t motor_sensorless_vector_over_voltage_error_check(float f4_vdc, float f4_overvoltage_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_NONE;

    if (f4_vdc > f4_overvoltage_limit)
    {
        u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_OVER_VOLTAGE;          /* Over voltage error */
    }

    return (u2_temp0);
} /* End of function motor_sensorless_vector_over_voltage_error_check */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_low_voltage_error_check
* Description   : Checks under-voltage error
* Arguments     : f4_vdc - The actual Vdc value [V]
                  f4_undervoltage_limit - The threshold voltage[V] of under-voltage error
* Return Value  : The flag of under-voltage error
***********************************************************************************************************************/
static inline uint16_t motor_sensorless_vector_low_voltage_error_check(float f4_vdc, float f4_undervoltage_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_NONE;

    if (f4_vdc < f4_undervoltage_limit)
    {
        u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_LOW_VOLTAGE;           /* Under voltage error */
    }

    return (u2_temp0);
} /* End of function motor_sensorless_vector_low_voltage_error_check */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_over_current_error_check
* Description   : Checks over-current error
* Arguments     : f4_iu                - The actual U-phase current value [A]
*                 f4_iv                - The actual V-phase current value [A]
*                 f4_iw                - The actual W-phase current value [A]
*                 f4_overcurrent_limit - The threshold current[A] of over-current error
* Return Value  : The flag of over-current error
***********************************************************************************************************************/
static inline uint16_t motor_sensorless_vector_over_current_error_check(float f4_iu,
                                                                        float f4_iv,
                                                                        float f4_iw,
                                                                        float f4_overcurrent_limit)
{
    float f4_temp0;
    uint16_t u2_temp0;

    u2_temp0 = MOTOR_SENSORLESS_VECTOR_ERROR_NONE;                      /* Initialize */

    f4_temp0 = fabsf(f4_iu);
    u2_temp0 |= (f4_temp0 > f4_overcurrent_limit);
    f4_temp0 = fabsf(f4_iv);
    u2_temp0 |= (f4_temp0 > f4_overcurrent_limit);
    f4_temp0 = fabsf(f4_iw);
    u2_temp0 |= (f4_temp0 > f4_overcurrent_limit);

    u2_temp0 *= MOTOR_SENSORLESS_VECTOR_ERROR_OVER_CURRENT_SW;

    return (u2_temp0);
} /* End of function motor_sensorless_vector_over_current_error_check */

/***********************************************************************************************************************
* Function Name : motor_sensorless_vector_error_check
* Description   : Checks the errors
* Arguments     : p_st_sensorless_vector - The pointer to the motor control management data structure
* Return Value  : None
***********************************************************************************************************************/
void motor_sensorless_vector_error_check(st_sensorless_vector_control_t *p_st_sensorless_vector)
{
    uint16_t u2_error_flags = 0;

    /*==================================*/
    /* Over current error check         */
    /*==================================*/
    u2_error_flags |= motor_sensorless_vector_over_current_error_check(p_st_sensorless_vector->f4_iu_ad,
                                                                       p_st_sensorless_vector->f4_iv_ad,
                                                                       p_st_sensorless_vector->f4_iw_ad,
                                                                       p_st_sensorless_vector->f4_overcurrent_limit);

    /*==================================*/
    /* Over voltage error check         */
    /*==================================*/
    u2_error_flags |= motor_sensorless_vector_over_voltage_error_check(p_st_sensorless_vector->f4_vdc_ad,
                                                                       p_st_sensorless_vector->f4_overvoltage_limit);

    /*===================================*/
    /* Under voltage error check         */
    /*===================================*/
    u2_error_flags |= motor_sensorless_vector_low_voltage_error_check(p_st_sensorless_vector->f4_vdc_ad,
                                                                      p_st_sensorless_vector->f4_undervoltage_limit);

    /*===================================*/
    /* Over speed error check            */
    /*===================================*/
    u2_error_flags |=
            motor_sensorless_vector_over_speed_error_check(p_st_sensorless_vector->st_current_output.f4_speed_rad,
                                                           p_st_sensorless_vector->f4_overspeed_limit_rad);

    if (MOTOR_SENSORLESS_VECTOR_ERROR_NONE != u2_error_flags)
    {
        motor_sensorless_vector_error_process(p_st_sensorless_vector, u2_error_flags);     /* Execute error event */
    }
} /* End of function motor_sensorless_vector_error_check */
