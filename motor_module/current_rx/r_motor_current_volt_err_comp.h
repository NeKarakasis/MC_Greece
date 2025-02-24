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
* File Name   : r_motor_current_volt_err_comp.h
* Description : Definitions of the inverter voltage error compensation
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*           11.06.2021 1.00     First Release
***********************************************************************************************************************/

/* guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_VOLT_ERR_COMP_H
#define R_MOTOR_CURRENT_VOLT_ERR_COMP_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define VERR_COMP_ARY_SIZE                (5)
/* default setting */
#define VERR_COMP_TBL_COMP_V0             (0.0f)
#define VERR_COMP_TBL_COMP_V1             (0.0f)
#define VERR_COMP_TBL_COMP_V2             (0.0f)
#define VERR_COMP_TBL_COMP_V3             (0.0f)
#define VERR_COMP_TBL_COMP_V4             (0.0f)

#define VERR_COMP_TBL_COMP_I0             (1.0f)
#define VERR_COMP_TBL_COMP_I1             (2.0f)
#define VERR_COMP_TBL_COMP_I2             (3.0f)
#define VERR_COMP_TBL_COMP_I3             (4.0f)
#define VERR_COMP_TBL_COMP_I4             (5.0f)

#define VERR_COMP_DISABLE                 (0)
#define VERR_COMP_ENABLE                  (1)
#define VERR_COMP_REF_VOLTAGE             (24.0f)

/* Definition for two-phase and three-phase branching.(will be moved to cfg later) */
#define VERROR_COMP_USE_UVW (0)  /* three-phase */
#define VERROR_COMP_USE_AB  (1)  /* two-phase   */
#define VERROR_COMP_USE_OFF (2)  /* no use*/

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    float f4_comp_v [VERR_COMP_ARY_SIZE];
    float f4_comp_i [VERR_COMP_ARY_SIZE];
    float f4_slope [VERR_COMP_ARY_SIZE +1];
    float f4_intcept[VERR_COMP_ARY_SIZE +1];
    float f4_volt_comp_array[3];
    float f4_vdc;
    float f4_volt_comp_limit;
    float f4_volt_comp_limit_ratio;
    uint8_t   u1_volt_err_comp_enable;
    uint8_t   u1_volt_comp_use_motor_type;  /* Flag for defining the type of motor used.  */
} st_volt_comp_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : motor_current_volt_err_comp_init
* Description   : Initializes voltage error compensation module
* Arguments     : p_st_volt_comp - The pointer to voltage error compensation structure
*               : u1_volt_comp_use_motor_type - Two-phase / three-phase motor selection flag
* Return Value  : none
***********************************************************************************************************************/
void motor_current_volt_err_comp_init(st_volt_comp_t *p_st_volt_comp, uint8_t u1_volt_comp_use_motor_type);

/***********************************************************************************************************************
* Function Name : motor_current_volt_err_comp_reset
* Description   : Resets compensation voltage error value
* Arguments     : p_st_volt_comp - The pointer to voltage error compensation structure
* Return Value  : none
***********************************************************************************************************************/
void motor_current_volt_err_comp_reset(st_volt_comp_t *p_st_volt_comp);

/***********************************************************************************************************************
* Function Name : motor_current_volt_err_comp_main
* Description   : Compensates voltage error
* Arguments     : p_st_volt_comp - The pointer to voltage error compensation structure
*                 p_f4_v_array - The pointer to the compensation amount of the motor two or three phase voltage
*                 p_f4_i_array - The pointer to the motor three phase current
*                 f4_vdc       - Inverter bus voltage
* Return Value  : none
***********************************************************************************************************************/
void motor_current_volt_err_comp_main(st_volt_comp_t *p_st_volt_comp,
                                      float *p_f4_v_array,
                                      float *p_f4_i_array,
                                      float f4_vdc);

/***********************************************************************************************************************
* Function Name : motor_current_volt_err_comp_table_set
* Description   : Set voltage error compensation table
* Arguments     : p_st_volt_comp   - The pointer to voltage error compensation structure
*                 f4_current_table - The pointer to the current table
*                 f4_volterr_table - The pointer to the voltage error table
*                 f4_ref_vdc       - Inverter bus voltage for the table
* Return Value  : none
***********************************************************************************************************************/
void motor_current_volt_err_comp_table_set(st_volt_comp_t *p_st_volt_comp,
                                       const float *f4_current_table,
                                       const float *f4_volterr_table,
                                       float f4_ref_vdc);

/***********************************************************************************************************************
* Function Name : motor_current_volt_err_vlimit_set
* Description   : Set voltage error compensation limit
* Arguments     : p_st_volt_comp - The pointer to voltage error compensation structure
*                 f4_vlimit_ratio - The ratio to Vdc, determine the maximum voltage for compensation
* Return Value  : none
***********************************************************************************************************************/
void motor_current_volt_err_vlimit_set(st_volt_comp_t *p_st_volt_comp, float f4_vlimit_ratio);

#endif /* R_MOTOR_CURRENT_VOLT_ERR_COMP_H */
