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
* File Name    : r_motor_current_modulation.h
* Description  : Definitions for modulation module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 11.06.2021 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_MODULATION_H
#define R_MOTOR_CURRENT_MODULATION_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#if defined (MOTOR_TYPE_BLDC)
/* Select SVPWM as default method when MOD_METHOD is undefined */
#ifndef CURRENT_CFG_MODULATION_METHOD
#define CURRENT_CFG_MODULATION_METHOD    (MOD_METHOD_SVPWM)
#endif
#define MOD_DEFAULT_MAX_DUTY        (1.0f)          /* Default maximum duty cycle */
#define MOD_METHOD_SPWM             (0)             /* Sinusoidal pulse-width-modulation */
#define MOD_METHOD_SVPWM            (1)             /* Space vector pulse-width-modulation */

/*
 * Vamax in this module is calculated by the following equation
 *   SVPWM :  Vdc * (MOD_VDC_TO_VAMAX_MULT) * (Max duty - Min duty) * (MOD_SVPWM_MULT)
 *   SPWM  :  Vdc * (MOD_VDC_TO_VAMAX_MULT) * (Max duty - Min duty)
 */
#define MOD_VDC_TO_VAMAX_MULT       (0.6124f)       /* The basic coefficient used to convert Vdc to Vamax */
#define MOD_SVPWM_MULT              (1.155f)        /* The usable voltage is multiplied by sqrt(4/3) when */
                                                    /* using SVPWM */
#elif defined (MOTOR_TYPE_STM)
#define MOD_DEFAULT_MAX_DUTY        (1.0f)          /* Default maximum duty cycle */
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    float f4_vdc;                                   /* Vdc */
    float f4_1_div_vdc;                             /* 1/Vdc */
    float f4_voltage_error_ratio;                   /* The voltage error ratio (VoltageError/Vdc) */
    float f4_max_duty;                              /* Maximum duty cycle (default:1.0) */
    float f4_min_duty;                              /* Minimum duty cycle (default:0.0) */
    float f4_neutral_duty;                          /* Duty cycle that represents 0[V] */
} st_mod_t;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name: motor_current_mod_init
* Description  : Initializes modulation module
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_init(st_mod_t *p_st_mod);

/***********************************************************************************************************************
* Function Name: motor_current_mod_reset
* Description  : Resets modulation module
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_reset(st_mod_t *p_st_mod);

/***********************************************************************************************************************
* Function Name: motor_current_mod_start
* Description  : Calculates duty cycle from input 3-phase voltage (bipolar);
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
*              : p_f4_v_in -
*                    Pointer to the 3-phase input voltage
*              : p_f4_duty_out -
*                    Where to store the 3-phase output duty cycle
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_start(st_mod_t *p_st_mod, const float *p_f4_v_in, float *p_f4_duty_out);

/***********************************************************************************************************************
* Function Name: motor_current_mod_vdc_set
* Description  : Sets the DC bus voltage used in modulation
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
*                f4_vdc -
*                    The DC bus voltage to be used in modulation
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_vdc_set(st_mod_t *p_st_mod, float f4_vdc);

/***********************************************************************************************************************
* Function Name: motor_current_mod_max_duty_set
* Description  : Sets the maximum duty cycle
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
*                f4_max_duty -
*                    Maximum duty cycle to set
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_max_duty_set(st_mod_t *p_st_mod, float f4_max_duty);

/***********************************************************************************************************************
* Function Name: motor_current_mod_min_duty_set
* Description  : Sets the minimum duty cycle
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
*                f4_max_duty -
*                    Minimum duty cycle to set
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_min_duty_set(st_mod_t *p_st_mod, float f4_min_duty);

/***********************************************************************************************************************
* Function Name: motor_current_mod_volt_err_ratio_set
* Description  : Sets the voltage error ratio (VoltageError/Vdc);
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
*                f4_voltage_error_ratio -
*                    The voltage error ratio := (VoltageError/Vdc), can use the (Dead-time*CarrierFreq) approximately
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_volt_err_ratio_set(st_mod_t *p_st_mod, float f4_voltage_error_ratio);

/***********************************************************************************************************************
* Function Name: motor_current_mod_voltage_multiplier_get
* Description  : Gets the voltage multiplier
* Arguments    : p_st_mod -
*                    Pointer to the modulation data structure
* Return Value : Voltage multiplier
***********************************************************************************************************************/
float motor_current_mod_voltage_multiplier_get(st_mod_t *p_st_mod);

/***********************************************************************************************************************
* Function Name: motor_current_mod_max_va_get
* Description  : Gets the maximum magnitude of voltage vector
* Arguments    : p_st_mod -
*                    The pointer to the modulation data structure
* Return Value : The maximum magnitude of voltage vector
***********************************************************************************************************************/
float motor_current_mod_max_va_get(st_mod_t *p_st_mod);

#endif /* R_MOTOR_CURRENT_MODULATION_H */
