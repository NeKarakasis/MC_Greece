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
* File Name   : r_pfc_manager_protection.c
* Description : The protection function
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <math.h>

/* Project headers */
#include "r_pfc_common.h"
#include "r_pfc_manager.h"
#include "r_pfc_manager_protection.h"

/***********************************************************************************************************************
* Function Name : pfc_manager_ac_over_voltage_error_check
* Description   : Checks over-voltage error for the input AC voltage
* Arguments     : f4_vac               - The actual Vac value [V]
                  f4_overvoltage_limit - The threshold voltage[V] of Vac over-voltage error
* Return Value  : The flag of Vac over-voltage error
***********************************************************************************************************************/
static inline uint16_t pfc_manager_ac_over_voltage_error_check(float f4_vac, float f4_overvoltage_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = PFC_MANAGER_ERROR_NONE;

    if (f4_vac > f4_overvoltage_limit)
    {
        u2_temp0 = PFC_MANAGER_ERROR_AC_OVER_VOLTAGE;          /* Over voltage error */
    }

    return (u2_temp0);
} /* End of function pfc_manager_ac_over_voltage_error_check */

/***********************************************************************************************************************
* Function Name : pfc_manager_bus_over_voltage_error_check
* Description   : Checks over-voltage error for the bus voltage
* Arguments     : f4_vdc               - The actual Vdc value [V]
                  f4_overvoltage_limit - The threshold voltage[V] of Vdc over-voltage error
* Return Value  : The flag of Vdc over-voltage error
***********************************************************************************************************************/
static inline uint16_t pfc_manager_bus_over_voltage_error_check(float f4_vdc, float f4_overvoltage_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = PFC_MANAGER_ERROR_NONE;

    if (f4_vdc > f4_overvoltage_limit)
    {
        u2_temp0 = PFC_MANAGER_ERROR_BUS_OVER_VOLTAGE;          /* Over voltage error */
    }

    return (u2_temp0);
} /* End of function pfc_manager_bus_over_voltage_error_check */

/***********************************************************************************************************************
* Function Name : pfc_manager_bus_low_voltage_error_check
* Description   : Checks under-voltage error for the bus voltage
* Arguments     : f4_vdc                - The actual Vdc value [V]
                  f4_undervoltage_limit - The threshold voltage[V] of Vdc under-voltage error
* Return Value  : The flag of Vdc under-voltage error
***********************************************************************************************************************/
static inline uint16_t pfc_manager_bus_low_voltage_error_check(float f4_vdc, float f4_undervoltage_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = PFC_MANAGER_ERROR_NONE;

    if (f4_vdc < f4_undervoltage_limit)
    {
        u2_temp0 = PFC_MANAGER_ERROR_BUS_LOW_VOLTAGE;           /* Under voltage error */
    }

    return (u2_temp0);
} /* End of function pfc_manager_bus_low_voltage_error_check */

/***********************************************************************************************************************
* Function Name : pfc_manager_over_current_error_check
* Description   : Checks over-current error for the PFC current
* Arguments     : f4_ichp              - The actual PFC current value [A]
*                 f4_overcurrent_limit - The threshold current[A] of PFC over-current error
* Return Value  : The flag of PFC over-current error
***********************************************************************************************************************/
static inline uint16_t pfc_manager_over_current_error_check(float f4_ichp, float f4_overcurrent_limit)
{
    uint16_t u2_temp0;

    u2_temp0 = PFC_MANAGER_ERROR_NONE;                      /* Initialize */

    if (f4_ichp > f4_overcurrent_limit)
    {
        u2_temp0 = PFC_MANAGER_ERROR_OVER_CURRENT_SW;          /* Over current error */
    }

    return (u2_temp0);
} /* End of function pfc_manager_over_current_error_check */

/***********************************************************************************************************************
* Function Name : pfc_manager_over_heating_error_check
* Description   : Checks over heating error
* Arguments     : p_st_pfc_manager         - The pointer to the PFC management data structure
*                 u1_overheat_detect_level - Port level of over heating error
* Return Value  : The flag of over heating error
***********************************************************************************************************************/
static inline uint16_t pfc_manager_over_heating_error_check(st_pfc_manager_t *p_st_pfc_manager, uint8_t u1_overheat_detect_level)
{
    uint8_t  u1_temp0;
    uint16_t u2_temp0;

    u2_temp0 = PFC_MANAGER_ERROR_NONE;                      /* Initialize */

    u1_temp0 = R_PFC_DRIVER_OverheatCheck(p_st_pfc_manager->p_st_pfc_driver);          /* Get port level */

    if (u1_temp0 == u1_overheat_detect_level)
    {
        u2_temp0 = PFC_MANAGER_ERROR_OVER_HEATING;          /* Over heating error */
    }

    return (u2_temp0);
} /* End of function pfc_manager_over_heating_error_check */

/***********************************************************************************************************************
* Function Name : pfc_manager_error_check
* Description   : Checks the errors
* Arguments     : p_st_pfc_manager - The pointer to the PFC management data structure
* Return Value  : None
***********************************************************************************************************************/
void pfc_manager_error_check(st_pfc_manager_t *p_st_pfc_manager)
{
    uint16_t u2_error_flags = 0;

    /*===================================*/
    /* Input AC over voltage error check */
    /*===================================*/
    u2_error_flags |= pfc_manager_ac_over_voltage_error_check(p_st_pfc_manager->f4_vac_ad_lpf,
                                                              p_st_pfc_manager->f4_ac_overvoltage_limit);

    /*===================================*/
    /* Bus over voltage error check      */
    /*===================================*/
    u2_error_flags |= pfc_manager_bus_over_voltage_error_check(p_st_pfc_manager->f4_vdc_ad_lpf,
                                                               p_st_pfc_manager->f4_bus_overvoltage_limit);

    /*===================================*/
    /* Bus under voltage error check     */
    /*===================================*/
    if (PFC_FLG_SET == p_st_pfc_manager->u1_relay_ctrl_on_flg)
    {
        u2_error_flags |= pfc_manager_bus_low_voltage_error_check(p_st_pfc_manager->f4_vdc_ad_lpf,
                                                                  p_st_pfc_manager->f4_bus_undervoltage_limit);
    }

    /*===================================*/
    /* PFC over current[SW]              */
    /*===================================*/
    u2_error_flags |= pfc_manager_over_current_error_check(p_st_pfc_manager->f4_ichp_ad,
                                                           p_st_pfc_manager->f4_overcurrent_limit);

    /*===================================*/
    /* PFC over heating error check      */
    /*===================================*/
    u2_error_flags |= pfc_manager_over_heating_error_check(p_st_pfc_manager,
                                                           p_st_pfc_manager->u1_overheat_detect_level);

    if (PFC_MANAGER_ERROR_NONE != u2_error_flags)
    {
        pfc_manager_error_process(p_st_pfc_manager, u2_error_flags);     /* Execute error event */
    }
} /* End of function pfc_manager_error_check */
