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
* File Name   : r_motor_driver_fsp.c
* Description : The processes of FSP driver module
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "hal_data.h"
#include "r_motor_common.h"
#include "r_motor_driver.h"
#include "r_motor_driver_fsp.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Global functions definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StartTimerOutput
* Description  : This function starts the PWM output for motor control
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_StartTimerCtrl(void)
{
    uint32_t u4_port_config;

    u4_port_config = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_GPT1);

    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_04, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_05, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_06, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_07, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_08, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_09, u4_port_config);

    R_GPT_OutputEnable(&g_timer_gpt4_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputEnable(&g_timer_gpt5_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputEnable(&g_timer_gpt6_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
} /* End of function R_Config_MOTOR_StartTimerCtrl */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_StopTimerCtrl
* Description  : This function stops the PWM output for motor control
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_StopTimerCtrl(void)
{
    uint32_t u4_port_config;

    R_GPT_OutputDisable(&g_timer_gpt4_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputDisable(&g_timer_gpt5_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    R_GPT_OutputDisable(&g_timer_gpt6_ctrl, GPT_IO_PIN_GTIOCA_AND_GTIOCB);

    u4_port_config = (((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));

    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_04, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_05, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_06, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_07, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_08, u4_port_config);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_09, u4_port_config);
} /* End of function R_Config_MOTOR_StopTimerCtrl */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_UpdDuty
* Description  : This function updates duty cycle
* Arguments    : duty_u - U phase duty register value
*                duty_v - V phase duty register value
*                duty_w - W phase duty register value
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_UpdDuty(uint16_t duty_u, uint16_t duty_v, uint16_t duty_w)
{
    uint32_t u4_duty_cnt_u = duty_u;
    uint32_t u4_duty_cnt_v = duty_v;
    uint32_t u4_duty_cnt_w = duty_w;

    /* Duty set */
    three_phase_duty_cycle_t duty_uvw;

    duty_uvw.duty[0] = u4_duty_cnt_u;
    duty_uvw.duty[1] = u4_duty_cnt_v;
    duty_uvw.duty[2] = u4_duty_cnt_w;

    R_GPT_THREE_PHASE_DutyCycleSet(&g_three_phase0_ctrl, &duty_uvw);
} /* End of function R_Config_MOTOR_UpdDuty */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_UpdZeroDuty
* Description  : This function sets PWM zero duty
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_UpdZeroDuty(void)
{
    /* For GPT4 */
    R_GPT4->GTUDDTYC_b.OADTY  = 0x02;
    R_GPT4->GTUDDTYC_b.OBDTY  = 0x03;
    R_GPT4->GTUDDTYC_b.OADTYR = 0x01;
    R_GPT4->GTUDDTYC_b.OBDTYR = 0x01;

    /* For GPT5 */
    R_GPT5->GTUDDTYC_b.OADTY  = 0x02;
    R_GPT5->GTUDDTYC_b.OBDTY  = 0x03;
    R_GPT5->GTUDDTYC_b.OADTYR = 0x01;
    R_GPT5->GTUDDTYC_b.OBDTYR = 0x01;

    /* For GPT6 */
    R_GPT6->GTUDDTYC_b.OADTY  = 0x02;
    R_GPT6->GTUDDTYC_b.OBDTY  = 0x03;
    R_GPT6->GTUDDTYC_b.OADTYR = 0x01;
    R_GPT6->GTUDDTYC_b.OBDTYR = 0x01;
} /* End of function R_Config_MOTOR_UpdZeroDuty */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_UpdCompareDuty
* Description  : This function sets PWM compare duty
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_UpdCompareDuty(void)
{
    /* For GPT4 */
    R_GPT4->GTUDDTYC_b.OADTY = 0x00;
    R_GPT4->GTUDDTYC_b.OBDTY = 0x00;

    /* For GPT5 */
    R_GPT5->GTUDDTYC_b.OADTY = 0x00;
    R_GPT5->GTUDDTYC_b.OBDTY = 0x00;

    /* For GPT6 */
    R_GPT6->GTUDDTYC_b.OADTY = 0x00;
    R_GPT6->GTUDDTYC_b.OBDTY = 0x00;
} /* End of function R_Config_MOTOR_UpdCompareDuty */

/***********************************************************************************************************************
* Function Name: R_Config_MOTOR_AdcGetConvVal
* Description  : This function gets A/D conversion result for motor control
* Arguments    : mtr_ad_data - Buffer pointer for A/D conversion register value
* Return Value : None
***********************************************************************************************************************/
void R_Config_MOTOR_AdcGetConvVal(r_mtr_adc_tb * mtr_ad_data)
{
    uint16_t u2_ad_conv_iu;
    uint16_t u2_ad_conv_iw;
    uint16_t u2_ad_conv_iv;
    uint16_t u2_ad_conv_vdc;

    /* ADC read from FSP API */
    R_ADC_B_Read(&g_adc0_ctrl, 6, &u2_ad_conv_vdc);
    R_ADC_B_Read(&g_adc0_ctrl, 4, &u2_ad_conv_iu);
    R_ADC_B_Read(&g_adc0_ctrl, 2, &u2_ad_conv_iv);
    R_ADC_B_Read(&g_adc0_ctrl, 0, &u2_ad_conv_iw);

    /* Store ADC read data */
    mtr_ad_data->u2_iu_ad  = (uint16_t)u2_ad_conv_iu;
    mtr_ad_data->u2_iv_ad  = (uint16_t)u2_ad_conv_iv;
    mtr_ad_data->u2_iw_ad  = (uint16_t)u2_ad_conv_iw;
    mtr_ad_data->u2_vdc_ad = (uint16_t)u2_ad_conv_vdc;
    mtr_ad_data->u2_vphase_u_ad = 0xFFFFU;
    mtr_ad_data->u2_vphase_v_ad = 0xFFFFU;
    mtr_ad_data->u2_vphase_w_ad = 0xFFFFU;
} /* End of function R_Config_MOTOR_AdcGetConvVal */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_StartTimerCtrl
* Description  : This function starts the PWM output for PFC control
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_StartTimerCtrl(void)
{
    uint32_t u4_port_config;

    u4_port_config = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_GPT1);
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_14, u4_port_config);

    R_GPT_OutputEnable(&g_timer_gpt1_ctrl, GPT_IO_PIN_GTIOCA);
} /* End of function R_Config_PFC_StartTimerCtrl */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_StopTimerCtrl
* Description  : This function stops the PWM output for PFC control
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_StopTimerCtrl(void)
{
    uint32_t u4_port_config;

    R_GPT_OutputDisable(&g_timer_gpt1_ctrl, GPT_IO_PIN_GTIOCA);

    u4_port_config = (((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_14, u4_port_config);
} /* End of function R_Config_PFC_StopTimerCtrl */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_AdcGetConvVal
* Description  : This function gets A/D conversion result for PFC control
* Arguments    : pfc_ad_data - Buffer pointer for A/D conversion register value
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_AdcGetConvVal(r_pfc_adc_tb * pfc_ad_data)
{
    uint16_t u2_ad_conv_vdc;
    uint16_t u2_ad_conv_vac;
    uint16_t u2_ad_conv_ichp;

    /* ADC read from FSP API */
    R_ADC_B_Read(&g_adc0_ctrl, 6, &u2_ad_conv_vdc);
    R_ADC_B_Read(&g_adc0_ctrl, 28, &u2_ad_conv_vac);
    R_ADC_B_Read(&g_adc0_ctrl, 27, &u2_ad_conv_ichp);

    /* Store ADC read data */
    pfc_ad_data->u2_vdc_ad  = (uint16_t)u2_ad_conv_vdc;
    pfc_ad_data->u2_vac_ad  = (uint16_t)u2_ad_conv_vac;
    pfc_ad_data->u2_ichp_ad = (uint16_t)u2_ad_conv_ichp;
} /* End of function R_Config_PFC_AdcGetConvVal */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_UpdDuty
* Description  : This function stores duty value to GPT
* Arguments    : u2_duty_cnt - The duty register value for the PFC control
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_UpdDuty(uint16_t u2_duty_cnt)
{
    uint32_t u4_duty_cnt_u;

    u4_duty_cnt_u = (uint32_t)u2_duty_cnt;

    if (GPT_MAX_CNT < u4_duty_cnt_u)
    {
        u4_duty_cnt_u = GPT_MAX_CNT;
    }
    else if (GPT_MIN_CNT > u4_duty_cnt_u)
    {
        u4_duty_cnt_u = GPT_MIN_CNT;
    }

    R_GPT_DutyCycleSet(&g_timer_gpt1_ctrl, u4_duty_cnt_u, GPT_IO_PIN_GTIOCA);
} /* End of function R_Config_PFC_UpdDuty */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_RelayOn
* Description  : This function switchs rush current suppression relay on
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_RelayOn(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, PFC_PORT_RELAY, MTR_HIGH);
} /* End of function R_Config_PFC_RelayOn */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_RelayOff
* Description  : This function switchs rush current suppression relay off
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PFC_RelayOff(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, PFC_PORT_RELAY, MTR_LOW);
} /* End of function R_Config_PFC_RelayOff */

/***********************************************************************************************************************
* Function Name: R_Config_PFC_OverheatCheck
* Description  : Get the port level for over heating error detection
* Arguments    : None
* Return Value : Port level
***********************************************************************************************************************/
uint8_t R_Config_PFC_OverheatCheck(void)
{
    uint8_t u1_temp0;

    R_IOPORT_PinRead(&g_ioport_ctrl, PFC_PORT_THERMAL, &u1_temp0);

    return (u1_temp0);
} /* End of function R_Config_PFC_OverheatCheck */
