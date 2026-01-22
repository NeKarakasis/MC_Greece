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
* File Name   : r_app_mcu.c
* Description : MCU depend application layer functions
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_app_mcu.h"
#include "r_motor_driver_hal.h"

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static uint8_t  s_u1_cnt_ics = 0;               /* Counter for period of calling "scope_watchpoint" */
static void     r_app_motor_timer_timing_adjust(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);

/***********************************************************************************************************************
* Function Name : r_app_hal_init
* Description   : Initialize and Set up peripheral function
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_hal_init(void)
{
    uint32_t u4_port_config;
    adc_status_t status;

    //R_POEG_Open(g_poeg3.p_ctrl, g_poeg3.p_cfg);



    u4_port_config = (((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_14, u4_port_config);

    /* GPT Output Disable to prevent gate on */
    R_Config_MOTOR_StopTimerCtrl();

    /* Start GPT */
    R_GPT_THREE_PHASE_Open(&g_three_phase0_ctrl, &g_three_phase0_cfg); /* Motor control */
    R_GPT_Open(&g_timer_gpt1_ctrl, &g_timer_gpt1_cfg);                 /* PFC control */

    R_GPT_THREE_PHASE_Stop(&g_three_phase0_ctrl);
    R_GPT_THREE_PHASE_Reset(&g_three_phase0_ctrl);

    R_GPT_Stop(&g_timer_gpt1_ctrl);
    R_GPT_Reset(&g_timer_gpt1_ctrl);

    /* Adjust PWM timing of the motor control and PFC control */
    r_app_motor_timer_timing_adjust(&g_timer_gpt4_ctrl, g_timer_gpt4.p_cfg->period_counts);
    r_app_motor_timer_timing_adjust(&g_timer_gpt5_ctrl, g_timer_gpt5.p_cfg->period_counts);
    r_app_motor_timer_timing_adjust(&g_timer_gpt6_ctrl, g_timer_gpt6.p_cfg->period_counts);

    R_GPT_Start(&g_timer_gpt1_ctrl);
    R_GPT_THREE_PHASE_Start(&g_three_phase0_ctrl);


    R_Config_MC_PWM_PFC_DUTY_StartTimerCtrl();

    /* Start AGT (speed control timer) */
    R_AGT_Open(&g_agt0_ctrl, &g_agt0_cfg);
    R_AGT_Stop(&g_agt0_ctrl);
    R_AGT_Reset(&g_agt0_ctrl);
    R_AGT_Start(&g_agt0_ctrl);

    /* Start AGT (system manager control timer) */
    R_AGT_Open(&g_agt1_ctrl, &g_agt1_cfg);
    R_AGT_Stop(&g_agt1_ctrl);
    R_AGT_Reset(&g_agt1_ctrl);
    R_AGT_Start(&g_agt1_ctrl);

    /* Start ADC module */
    R_ADC_B_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_B_ScanCfg(&g_adc0_ctrl, &g_adc0_scan_cfg);

    /* Wait for calibration to complete */
    R_ADC_B_Calibrate(&g_adc0_ctrl, NULL);
    status.state = ADC_STATE_CALIBRATION_IN_PROGRESS;

    while (ADC_STATE_IDLE != status.state)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        R_ADC_B_StatusGet(&g_adc0_ctrl, &status);
    }

    R_ADC_B_ScanStart(&g_adc0_ctrl);

    /* Watchdog timer start */
    R_IWDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);
    R_IWDT_Refresh(&g_wdt0_ctrl);
} /* End of function r_app_hal_init */

/***********************************************************************************************************************
* Function Name : r_app_rmw_hw_init
* Description   : Initialize rmw module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_hw_init(void)
{
    ics2_init(ICS_SCI9_P605_P606, ICS_BRR, ICS_INT_MODE);
} /* End of function r_app_rmw_hw_init */

/***********************************************************************************************************************
* Function Name : r_app_main_loop
* Description   : Main loop of motor control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_main_loop(void)
{
    poeg_status_t st_status;
    uint32_t u4_status;

    R_POEG_StatusGet(g_poeg3.p_ctrl, &st_status);
    u4_status = st_status.state & POEG_STATE_PIN_DISABLE_REQUEST_ACTIVE;
    if (POEG_STATE_PIN_DISABLE_REQUEST_ACTIVE != u4_status)
    {
        R_BSP_IrqEnable(g_poeg3_ctrl.p_cfg->irq);
    }

    /* Watchdog timer refresh */
    R_IWDT_Refresh(&g_wdt0_ctrl);
} /* End of function r_app_main_loop */

/***********************************************************************************************************************
* Function Name : r_app_pwm_highz_reset
* Description   : Reset poeg flag
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_pwm_highz_reset(void)
{
    R_BSP_IrqDisable(g_poeg3.p_cfg->irq);
    R_POEG_Reset(g_poeg3.p_ctrl);
} /* End of function r_app_pwm_highz_reset */

/***********************************************************************************************************************
* Function Name : r_app_motor_timer_timing_adjust
* Description   : Set GPT carrier period count
* Arguments     : p_ctrl           - gpt instance
*                 u4_period_counts - Period counts
* Return Value  : None
***********************************************************************************************************************/
static void r_app_motor_timer_timing_adjust (timer_ctrl_t * const p_ctrl, uint32_t const u4_period_counts)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;

    /* Set the period */
    p_instance_ctrl->p_reg->GTPBR = u4_period_counts;                 /* Set period to buffer register */
    p_instance_ctrl->p_reg->GTPR = (uint32_t)((float)u4_period_counts * 0.375f);
                                                                     /* Set staggered period to setting register */

    /* Set the duty cycle */
    p_instance_ctrl->p_reg->GTCCR[0] = (uint32_t)((float)u4_period_counts * 0.375f * 0.5f);
                                                                     /* Set duty cycle to compare register */
    p_instance_ctrl->p_reg->GTCCR[1] = (uint32_t)((float)u4_period_counts * 0.375f * 0.5f);
                                                                     /* Set duty cycle to compare register */
} /* End of function r_app_motor_timer_timing_adjust */

/***********************************************************************************************************************
* Function Name : r_app_rmw_watchpoint
* Description   : Call ICS
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_app_rmw_watchpoint(void)
{
    s_u1_cnt_ics++;

    /* Decimation of ICS call */
    if (ICS_DECIMATION < s_u1_cnt_ics)
    {
        s_u1_cnt_ics = 0;

        /* Call ICS */
        ics2_watchpoint();
    }
} /* End of function r_app_rmw_watchpoint */
