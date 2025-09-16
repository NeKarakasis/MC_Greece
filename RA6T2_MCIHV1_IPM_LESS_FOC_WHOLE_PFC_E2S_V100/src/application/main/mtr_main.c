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
* File Name   : mtr_main.c
* Description : The main function and the processes of motor control application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "mtr_main.h"
#include "hal_data.h"
#include "ics2_RA6T2.h"
#include "r_motor_common.h"
#include "r_mtr_ics.h"
#include "r_motor_sensorless_vector_api.h"
#include "r_motor_sensorless_vector_statemachine.h"
#include "r_system_manager_api.h"
#include "r_motor_driver_fsp.h"



/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static void     motor_fsp_init(void);
static void     software_init(void);      /* Software initialize */
static void     gpt_periodset(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);

#if 0
static void     led1_on(void);
static void     led2_on(void);
static void     led3_on(void);
static void     led1_off(void);
static void     led2_off(void);
static void     led3_off(void);
#endif

/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control & PFC Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init(void)
{
    /* Create a System manager Instance */
    R_SYSTEM_MANAGER_Open();

    /* Initialize FSP */
    motor_fsp_init();

    /* Initialize private global variables */
    software_init();

    /* Initialize ICS(RMW) interface */
    ics2_init(ICS_SCI9_P605_P606, ICS_BRR, ICS_INT_MODE);
} /* End of function mtr_init() */

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Main routine for Motor Control & PFC Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main(void)
{
    poeg_status_t st_status;
    uint32_t u4_status;

    /*============================*/
    /*        Get ICS value       */
    /*============================*/
    r_app_rmw_copy_com_to_buffer();

    /*============================*/
    /*        Execute event       */
    /*============================*/

    /* Main process for ICS UI */
    r_app_rmw_ui_mainloop();

    R_POEG_StatusGet(g_poeg3.p_ctrl, &st_status);
    u4_status = st_status.state & POEG_STATE_PIN_DISABLE_REQUEST_ACTIVE;
    if (POEG_STATE_PIN_DISABLE_REQUEST_ACTIVE != u4_status)
    {
        R_BSP_IrqEnable(g_poeg3_ctrl.p_cfg->irq);
    }
} /* End of function mtr_main */

/***********************************************************************************************************************
* Function Name : software_init
* Description   : Initialize private global variables
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void software_init(void)
{
    g_u1_system_mode = STATEMACHINE_EVENT_STOP;

    /* ICS variables initialization */
    com_u1_system_mode = STATEMACHINE_EVENT_STOP;

    r_app_rmw_ui_init();
} /* End of function software_init */

/***********************************************************************************************************************
* Function Name : motor_fsp_init
* Description   : Initialize Motor & PFC FSP modules
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void motor_fsp_init(void)
{
    uint32_t u4_port_config;
    adc_status_t status;

    R_POEG_Open(g_poeg3.p_ctrl, g_poeg3.p_cfg);


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
    R_Config_MC_PWM_PFC_DUTY_StartTimerCtrl();

    /* Adjust PWM timing of the motor control and PFC control */
    gpt_periodset(&g_timer_gpt4_ctrl, g_timer_gpt4.p_cfg->period_counts);
    gpt_periodset(&g_timer_gpt5_ctrl, g_timer_gpt5.p_cfg->period_counts);
    gpt_periodset(&g_timer_gpt6_ctrl, g_timer_gpt6.p_cfg->period_counts);

    R_GPT_Start(&g_timer_gpt1_ctrl);
    R_GPT_THREE_PHASE_Start(&g_three_phase0_ctrl);

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
} /* End of function motor_fsp_init */

/***********************************************************************************************************************
* Function Name : gpt_periodset
* Description   : Set GPT carrier period count
* Arguments     : p_ctrl - gpt instance
*                 period_counts - Period counts
* Return Value  : None
***********************************************************************************************************************/
static void gpt_periodset (timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;

    /* Set the period */
    p_instance_ctrl->p_reg->GTPBR = period_counts;                   /* Set period to buffer register */
    p_instance_ctrl->p_reg->GTPR = (uint32_t)((float)period_counts * 0.375f);
                                                                     /* Set staggered period to setting register */

    /* Set the duty cycle */
    p_instance_ctrl->p_reg->GTCCR[0] = (uint32_t)((float)period_counts * 0.375f * 0.5f);
                                                                     /* Set duty cycle to compare register */
    p_instance_ctrl->p_reg->GTCCR[1] = (uint32_t)((float)period_counts * 0.375f * 0.5f);
                                                                     /* Set duty cycle to compare register */
} /* End of function gpt_periodset */

/***********************************************************************************************************************
* Function Name : callback_gpt_adc_cyclic
* Description   : A/D conversion completion interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_gpt_adc_cyclic(adc_callback_args_t *p_args)
{
    if (ADC_EVENT_SCAN_COMPLETE == p_args->event)
    {
        switch (p_args->group_mask)
        {
            case ADC_GROUP_MASK_0:
                /* Motor control cyclic */
                R_SYSTEM_MANAGER_MotorCurrentInterrupt();
                r_app_rmw_interrupt_handler();
            break;

            case ADC_GROUP_MASK_1:
                /* PFC control cyclic */
               // R_SYSTEM_MANAGER_PfcCurrentInterrupt();

            break;

            default:
                /* Do nothing */
            break;
        }
    }
} /* End of function callback_gpt_adc_cyclic */


/* Callback function */
void pfc_pwm_cyclic(timer_callback_args_t *p_args)
{
	/* TODO: add your own code here */
	if( TIMER_EVENT_CREST == p_args->event)
	{
		R_Config_MC_PWM_PFC_DUTY_UpdDuty();
	}
}


/***********************************************************************************************************************
* Function Name : callback_agt_motor_speed_cyclic
* Description   : Speed control cycle interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_agt_motor_speed_cyclic(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        R_SYSTEM_MANAGER_MotorSpeedInterrupt();
    }
} /* End of function callback_agt_motor_speed_cyclic */

/***********************************************************************************************************************
* Function Name : callback_agt_system_manager_cyclic
* Description   : System manager control cycle interrupt.
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_agt_system_manager_cyclic(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        R_SYSTEM_MANAGER_ControlInterrupt();
    }
} /* End of function callback_agt_system_manager_cyclic */

/***********************************************************************************************************************
* Function Name : callback_poe_overcurrent
* Description   : POEG3 Interrupt callback function
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void callback_poe_overcurrent(poeg_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        R_BSP_IrqDisable(g_poeg3.p_cfg->irq);
        R_POEG_Reset(g_poeg3.p_ctrl);
        R_SYSTEM_MANAGER_MotorOverCurrentInterrupt();
    }
} /* End of function callback_poe_overcurrent */

#if 0
/***********************************************************************************************************************
* Function Name : led1_on
* Description   : Turn on LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_on(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1, MTR_LED_ON);
} /* End of function led1_on */

/***********************************************************************************************************************
* Function Name : led2_on
* Description   : Turn on LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_on(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2, MTR_LED_ON);
} /* End of function led2_on */

/***********************************************************************************************************************
* Function Name : led3_on
* Description   : Turn on LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_on(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED3, MTR_LED_ON);
} /* End of function led3_on */

/***********************************************************************************************************************
* Function Name : led1_off
* Description   : Turn off LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_off(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1, MTR_LED_OFF);
} /* End of function led1_off */

/***********************************************************************************************************************
* Function Name : led2_off
* Description   : Turn off LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_off(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2, MTR_LED_OFF);
} /* End of function led2_off */

/***********************************************************************************************************************
* Function Name : led3_off
* Description   : Turn off LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_off(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED3, MTR_LED_OFF);
} /* End of function led3_off */
#endif
