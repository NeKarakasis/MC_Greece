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
*         : 19.01.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "mtr_main.h"
#include "hal_data.h"
#include "r_mtr_rmw.h"
#include "r_mtr_rmw_display_cfg.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
float       g_f4_speed_ref = 0.0F;
float       g_f4_speed_ref1 = 0.0F;
uint8_t     g_u1_motor_status;          /* Motor status of motor #1 */
uint8_t     g_u1_motor_status1;         /* Motor status of motor #2 */
uint8_t     com_u1_sw_userif;           /* User interface switch */
uint8_t     g_u1_sw_userif;             /* User interface switch */
uint8_t     com_u1_both_mode_system;
uint8_t     com_u1_mode_system;         /* System mode for motor #1 */
uint8_t     com_u1_mode_system1;        /* System mode for motor #2 */
uint8_t     g_u1_mode_system;           /* System mode for motor #1 */
uint8_t     g_u1_mode_system1;          /* System mode for motor #2 */
uint16_t    g_u2_max_speed_rpm;
uint16_t    g_u2_max_speed_rpm1;
uint8_t     g_u1_stop_req;
uint8_t     g_u1_stop_req1;
uint16_t    g_u2_chk_error;
uint16_t    g_u2_chk_error1;
uint16_t    g_u2_vr1_ad;
uint16_t    g_u2_vr2_ad;
uint16_t    g_u2_conf_hw;
uint16_t    g_u2_conf_sw;
uint16_t    g_u2_conf_tool;
uint8_t     g_u1_conf_motor_type_len;
uint8_t     g_u1_conf_control_len;
uint8_t     g_u1_conf_inverter_len;
uint8_t     g_u1_conf_motor_type[CONF_MOTOR_TYPE_LEN];
uint8_t     g_u1_conf_control[CONF_CONTROL_LEN];
uint8_t     g_u1_conf_inverter[CONF_INVERTER_LEN];
uint8_t     g_u1_reset_req;             /* Reset request flag for motor #1 */
uint8_t     g_u1_reset_req1;            /* Reset request flag for motor #2 */
uint8_t     g_u1_sw_cnt;                /* Counter to remove chattering for SW1_1 */
uint8_t     g_u1_sw_cnt1;               /* Counter to remove chattering for SW1_2 */

/* Motor #1 */
const motor_instance_t  *p_motor_instance;
motor_cfg_t g_user_motor_cfg;
motor_sensorless_instance_ctrl_t *p_user_motor_instance_ctrl;
motor_sensorless_extended_cfg_t g_user_motor_sensorless_extended_cfg;
motor_speed_cfg_t g_user_motor_speed_cfg;
motor_speed_extended_cfg_t g_user_motor_speed_extended_cfg;
motor_current_cfg_t g_user_motor_current_cfg;
motor_current_extended_cfg_t g_user_motor_current_extended_cfg;
motor_angle_cfg_t g_user_motor_angle_cfg;
motor_estimate_extended_cfg_t g_user_motor_estimate_extended_cfg;
motor_driver_cfg_t g_user_motor_driver_cfg;
motor_driver_extended_cfg_t g_user_motor_driver_extended_cfg;
motor_current_motor_parameter_t g_user_motor_current_motor_parameter;
motor_current_design_parameter_t g_user_motor_current_design_parameter;

/* Motor #2 */
const motor_instance_t  *p_motor1_instance;
motor_cfg_t g_user_motor1_cfg;
motor_sensorless_instance_ctrl_t *p_user_motor1_instance_ctrl;
motor_sensorless_extended_cfg_t g_user_motor_sensorless1_extended_cfg;
motor_speed_cfg_t g_user_motor_speed1_cfg;
motor_speed_extended_cfg_t g_user_motor_speed1_extended_cfg;
motor_current_cfg_t g_user_motor_current1_cfg;
motor_current_extended_cfg_t g_user_motor_current1_extended_cfg;
motor_angle_cfg_t g_user_motor_angle1_cfg;
motor_estimate_extended_cfg_t g_user_motor_estimate1_extended_cfg;
motor_driver_cfg_t g_user_motor_driver1_cfg;
motor_driver_extended_cfg_t g_user_motor_driver1_extended_cfg;
motor_current_motor_parameter_t g_user_motor_current1_motor_parameter;
motor_current_design_parameter_t g_user_motor_current1_design_parameter;

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static void     motor_fsp_init (void);
static void     mtr_board_mode_control (uint8_t u1_id);
static void     mtr_ics_mode_control (uint8_t u1_id);
static void     mtr_board_led_control (uint8_t u1_motor_status, uint8_t u1_id);
static uint8_t  mtr_remove_sw_chattering (uint8_t u1_sw, uint8_t u1_on_off, uint8_t u1_id);
static void     mtr_set_reference (uint8_t u1_id);
static void     board_ui (void);                    /* Board user interface */
static void     ics_ui (void);                      /* ICS (Analyzer) user interface */
static void     software_init (uint8_t u1_id);      /* Software initialize */
static uint16_t get_vr1 (void);
static uint16_t get_vr2 (void);
static uint8_t  get_sw1 (uint8_t u1_id);
static uint8_t  get_sw2 (uint8_t u1_id);
static void     led1_on (uint8_t u1_id);
static void     led2_on (uint8_t u1_id);
static void     led3_on (uint8_t u1_id);
static void     led1_off (uint8_t u1_id);
static void     led2_off (uint8_t u1_id);
static void     led3_off (uint8_t u1_id);
static void     gpt_periodset (timer_ctrl_t * const p_ctrl, uint32_t const period_counts);

static uint8_t  u1_init_flag = 0U;

#define     MOTOR_STATUS_STOP       (0)
#define     MOTOR_STATUS_RUN        (1)
#define     MOTOR_STATUS_ERROR      (2)
#define     MOTOR_CTRL_EVENT_STOP   (0)
#define     MOTOR_CTRL_EVENT_RUN    (1)
#define     MOTOR_CTRL_EVENT_ERROR  (2)
#define     MOTOR_CTRL_EVENT_RESET  (3)

/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init(void)
{
    int i;
    uint8_t u1_conf_motor_type[] = CONF_MOTOR_TYPE;
    uint8_t u1_conf_control[] = CONF_CONTROL;
    uint8_t u1_conf_inverter[] = CONF_INVERTER;
    g_u1_conf_motor_type_len = CONF_MOTOR_TYPE_LEN;
    g_u1_conf_control_len    = CONF_CONTROL_LEN;
    g_u1_conf_inverter_len   = CONF_INVERTER_LEN;
    for (i = 0; i < g_u1_conf_motor_type_len; i++)
    {
        g_u1_conf_motor_type[i] = u1_conf_motor_type[i];
    }
    for (i = 0; i < g_u1_conf_control_len; i++)
    {
        g_u1_conf_control[i] = u1_conf_control[i];
    }
    for (i = 0; i < g_u1_conf_inverter_len; i++)
    {
        g_u1_conf_inverter[i] = u1_conf_inverter[i];
    }
    g_u2_conf_hw = 0x0008;                        /* 0000000000001000b */
    g_u2_conf_sw = 0x0000;                        /* 0000000000000000b */
    g_u2_conf_tool = 0x0300;                      /* 0000011000000000b */

    ics2_init(ICS_BRR, ICS_INT_MODE);

    /* Set motor instance pointer */
    p_motor_instance  = &g_motor_sensorless0;
    p_motor1_instance = &g_motor_sensorless1;

    /* Start instances & set user parameters */
    motor_fsp_init();

    /* Initialize private global variables */
    software_init(MTR_ID_0);
    software_init(MTR_ID_1);
    g_u1_sw_userif   = CONFIG_DEFAULT_UI;
    com_u1_sw_userif = CONFIG_DEFAULT_UI;

    /* Set the flag of finsh intialization */
    u1_init_flag = 1U;

    /* Execute reset event */
    p_motor_instance->p_api->reset(p_motor_instance->p_ctrl);
    p_motor1_instance->p_api->reset(p_motor1_instance->p_ctrl);
} /* End of function mtr_init() */

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Main routine for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main(void)
{
    /*** select user interfaces ***/
    if (g_u1_sw_userif != com_u1_sw_userif)
    {
        g_u1_sw_userif = com_u1_sw_userif;
        if (ICS_UI == g_u1_sw_userif)
        {
            g_u1_mode_system = g_u1_motor_status;
            g_u1_mode_system1 = g_u1_motor_status1;
        }
    }

    if (BOARD_UI == g_u1_sw_userif)
    {
        board_ui();                           /* User interface control routine */
    }
    else if (ICS_UI == g_u1_sw_userif)
    {
        ics_ui();                             /* User interface using ICS */
    }
    else
    {
        /* Do Nothing */
    }
} /* End of function mtr_main() */

/***********************************************************************************************************************
* Function Name : board_ui
* Description   : User interface using board UI
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void board_ui(void)
{
    /***** MODE control *****/
    mtr_board_mode_control(MTR_ID_0);
    mtr_board_mode_control(MTR_ID_1);

    /***** LED control *****/
    mtr_board_led_control(g_u1_motor_status, MTR_ID_0);
    mtr_board_led_control(g_u1_motor_status1, MTR_ID_1);
} /* End of function board_ui */

/***********************************************************************************************************************
* Function Name : ics_ui
* Description   : User interface using ICS
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void ics_ui(void)
{
    /***** MODE control *****/
    mtr_ics_mode_control(MTR_ID_0);
    mtr_ics_mode_control(MTR_ID_1);

    /***** LED control *****/
    mtr_board_led_control(g_u1_motor_status, MTR_ID_0);
    mtr_board_led_control(g_u1_motor_status1, MTR_ID_1);
} /* End of function ics_ui */

/***********************************************************************************************************************
* Function Name : software_init
* Description   : Initialize private global variables
* Arguments     : u1_id - Motor identifical number
* Return Value  : None
***********************************************************************************************************************/
static void software_init(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            g_u1_motor_status       = MOTOR_STATUS_STOP;
            g_u2_max_speed_rpm      = MTR_MAX_SPEED_RPM;
            g_u1_mode_system        = MOTOR_CTRL_EVENT_STOP;
            g_u1_stop_req           = MTR_FLG_SET;
            g_u1_reset_req          = SW_OFF;
            com_u1_mode_system      = MOTOR_CTRL_EVENT_STOP;
        break;

        case MTR_ID_1:
            g_u1_motor_status1      = MOTOR_STATUS_STOP;
            g_u2_max_speed_rpm1     = MTR_MAX_SPEED_RPM1;
            g_u1_mode_system1       = MOTOR_CTRL_EVENT_STOP;
            g_u1_stop_req1          = MTR_FLG_SET;
            g_u1_reset_req1         = SW_OFF;
            com_u1_mode_system1     = MOTOR_CTRL_EVENT_STOP;
        break;

        default:
            /* Do Nothing */
        break;
    }

    /* ICS variables initialization */
    mtr_ics_variables_init(u1_id);
} /* End of function software_init */

/***********************************************************************************************************************
* Function Name : g_poe_overcurrent0
* Description   : POEG3 Interrupt callback function
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void g_poe_overcurrent0(poeg_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        R_POEG_Reset(g_poeg0.p_ctrl);
        p_motor_instance->p_api->errorSet(p_motor_instance->p_ctrl, MOTOR_ERROR_OVER_CURRENT_HW);
        g_u2_chk_error |= MOTOR_ERROR_OVER_CURRENT_HW;
    }
} /* End of function g_poe_overcurrent */

/***********************************************************************************************************************
* Function Name : g_poe_overcurrent1
* Description   : POEG1 Interrupt callback function
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void g_poe_overcurrent1(poeg_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        R_POEG_Reset(g_poeg1.p_ctrl);
        p_motor1_instance->p_api->errorSet(p_motor1_instance->p_ctrl, MOTOR_ERROR_OVER_CURRENT_HW);
        g_u2_chk_error1 |= MOTOR_ERROR_OVER_CURRENT_HW;
    }
} /* End of function g_poe_overcurrent1 */

/***********************************************************************************************************************
* Function Name : motor_fsp_init
* Description   : Initialize Motor FSP modules
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void motor_fsp_init(void)
{
    /* Open motor instance */
    p_motor_instance->p_api->open(p_motor_instance->p_ctrl, p_motor_instance->p_cfg);
    p_motor1_instance->p_api->open(p_motor1_instance->p_ctrl, p_motor1_instance->p_cfg);

    /* Open POEG instance */
    R_POEG_Open(g_poeg0.p_ctrl, g_poeg0.p_cfg);
    R_POEG_Open(g_poeg1.p_ctrl, g_poeg1.p_cfg);

    /* Adjustment of PWM carrier timing */
    /* Refer Motor1 PWM GPT timers */
    motor_current_instance_t const *p_motor1_current_instance = p_motor1_instance->p_cfg->p_motor_current_instance;
    motor_driver_instance_t  const *p_motor1_driver_instance
        = p_motor1_current_instance->p_cfg->p_motor_driver_instance;
    three_phase_instance_t const *p_motor1_3phase_instance = p_motor1_driver_instance->p_cfg->p_three_phase_instance;
    timer_instance_t const *p_u_phase_gpt = p_motor1_3phase_instance->p_cfg->p_timer_instance[0];
    timer_instance_t const *p_v_phase_gpt = p_motor1_3phase_instance->p_cfg->p_timer_instance[1];
    timer_instance_t const *p_w_phase_gpt = p_motor1_3phase_instance->p_cfg->p_timer_instance[2];

    R_GPT_THREE_PHASE_Stop(g_three_phase0.p_ctrl);
    R_GPT_THREE_PHASE_Stop(g_three_phase1.p_ctrl);
    R_GPT_THREE_PHASE_Reset(g_three_phase0.p_ctrl);
    R_GPT_THREE_PHASE_Reset(g_three_phase1.p_ctrl);
    gpt_periodset(p_u_phase_gpt->p_ctrl,p_u_phase_gpt->p_cfg->period_counts);
    gpt_periodset(p_v_phase_gpt->p_ctrl,p_v_phase_gpt->p_cfg->period_counts);
    gpt_periodset(p_w_phase_gpt->p_ctrl,p_w_phase_gpt->p_cfg->period_counts);
    R_GPT_THREE_PHASE_Start(g_three_phase0.p_ctrl);
    R_GPT_THREE_PHASE_Start(g_three_phase1.p_ctrl);

    /* Set user configuration */
    /* Motor #1 */
    g_user_motor_cfg = *(p_motor_instance->p_cfg);
    g_user_motor_sensorless_extended_cfg = *(motor_sensorless_extended_cfg_t *)g_user_motor_cfg.p_extend;
    g_user_motor_cfg.p_extend = &g_user_motor_sensorless_extended_cfg;
    p_user_motor_instance_ctrl = p_motor_instance->p_ctrl;
    p_user_motor_instance_ctrl->p_cfg = &g_user_motor_cfg;

    g_user_motor_speed_cfg = *(g_user_motor_cfg.p_motor_speed_instance->p_cfg);
    g_user_motor_speed_extended_cfg = *(motor_speed_extended_cfg_t *)g_user_motor_speed_cfg.p_extend;
    g_user_motor_speed_cfg.p_extend = &g_user_motor_speed_extended_cfg;

    g_user_motor_current_cfg = *(g_user_motor_cfg.p_motor_current_instance->p_cfg);
    g_user_motor_current_extended_cfg = *(motor_current_extended_cfg_t *)g_user_motor_current_cfg.p_extend;
    g_user_motor_current_motor_parameter = *(g_user_motor_current_extended_cfg.p_motor_parameter);
    g_user_motor_current_design_parameter = *(g_user_motor_current_extended_cfg.p_design_parameter);
    g_user_motor_current_extended_cfg.p_motor_parameter = &g_user_motor_current_motor_parameter;
    g_user_motor_current_extended_cfg.p_design_parameter = &g_user_motor_current_design_parameter;
    g_user_motor_current_cfg.p_extend = &g_user_motor_current_extended_cfg;

    g_user_motor_angle_cfg = *(g_user_motor_cfg.p_motor_current_instance->p_cfg->p_motor_angle_instance->p_cfg);
    g_user_motor_estimate_extended_cfg = *(motor_estimate_extended_cfg_t *)g_user_motor_angle_cfg.p_extend;
    g_user_motor_angle_cfg.p_extend = &g_user_motor_estimate_extended_cfg;

    g_user_motor_driver_cfg = *(g_user_motor_cfg.p_motor_current_instance->p_cfg->p_motor_driver_instance->p_cfg);
    g_user_motor_driver_extended_cfg = *(motor_driver_extended_cfg_t *)g_user_motor_driver_cfg.p_extend;
    g_user_motor_driver_cfg.p_extend = &g_user_motor_driver_extended_cfg;

    /* Motor #2 */
    g_user_motor1_cfg = *(p_motor1_instance->p_cfg);
    g_user_motor_sensorless1_extended_cfg = *(motor_sensorless_extended_cfg_t *)g_user_motor1_cfg.p_extend;
    g_user_motor1_cfg.p_extend = &g_user_motor_sensorless1_extended_cfg;
    p_user_motor1_instance_ctrl = p_motor1_instance->p_ctrl;
    p_user_motor1_instance_ctrl->p_cfg = &g_user_motor1_cfg;

    g_user_motor_speed1_cfg = *(g_user_motor1_cfg.p_motor_speed_instance->p_cfg);
    g_user_motor_speed1_extended_cfg = *(motor_speed_extended_cfg_t *)g_user_motor_speed1_cfg.p_extend;
    g_user_motor_speed1_cfg.p_extend = &g_user_motor_speed1_extended_cfg;

    g_user_motor_current1_cfg = *(g_user_motor1_cfg.p_motor_current_instance->p_cfg);
    g_user_motor_current1_extended_cfg = *(motor_current_extended_cfg_t *)g_user_motor_current1_cfg.p_extend;
    g_user_motor_current1_motor_parameter = *(g_user_motor_current1_extended_cfg.p_motor_parameter);
    g_user_motor_current1_design_parameter = *(g_user_motor_current1_extended_cfg.p_design_parameter);
    g_user_motor_current1_extended_cfg.p_motor_parameter = &g_user_motor_current1_motor_parameter;
    g_user_motor_current1_extended_cfg.p_design_parameter = &g_user_motor_current1_design_parameter;
    g_user_motor_current1_cfg.p_extend = &g_user_motor_current1_extended_cfg;

    g_user_motor_angle1_cfg = *(g_user_motor1_cfg.p_motor_current_instance->p_cfg->p_motor_angle_instance->p_cfg);
    g_user_motor_estimate1_extended_cfg = *(motor_estimate_extended_cfg_t *)g_user_motor_angle1_cfg.p_extend;
    g_user_motor_angle1_cfg.p_extend = &g_user_motor_estimate1_extended_cfg;

    g_user_motor_driver1_cfg = *(g_user_motor1_cfg.p_motor_current_instance->p_cfg->p_motor_driver_instance->p_cfg);
    g_user_motor_driver1_extended_cfg = *(motor_driver_extended_cfg_t *)g_user_motor_driver1_cfg.p_extend;
    g_user_motor_driver1_cfg.p_extend = &g_user_motor_driver1_extended_cfg;

} /* End of function motor_fsp_init */

/***********************************************************************************************************************
* Function Name : mtr_callback_event0
* Description   : Callback function of motor control
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void mtr_callback_event0(motor_callback_args_t * p_args)
{
    /* Only valid after the finish of initialization */
    if (1U == u1_init_flag)
    {
        switch (p_args->event)
        {
            case MOTOR_CALLBACK_EVENT_SPEED_FORWARD:
            break;

            case MOTOR_CALLBACK_EVENT_SPEED_BACKWARD:
            {
                mtr_set_reference(MTR_ID_0);
                break;
            }

            case MOTOR_CALLBACK_EVENT_CURRENT_FORWARD:
            {
                if (MOTOR_STATUS_ERROR != g_u1_motor_status)
                {
                    p_motor_instance->p_api->errorCheck(p_motor_instance->p_ctrl, &g_u2_chk_error);
                }
                /* Speed reference (VR1) is sampled here after A/D conversion. */
                g_u2_vr1_ad = get_vr1();
                break;
            }

            case MOTOR_CALLBACK_EVENT_CURRENT_BACKWARD:
            {
                mtr_ics_interrupt_process();
                break;
            }

            default:
            break;
        }
    }
} /* End of function mtr_callback_event0 */

/***********************************************************************************************************************
* Function Name : mtr_callback_event1//mtr_callback_event
* Description   : Callback function of motor control
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void mtr_callback_event1(motor_callback_args_t * p_args)
{
    /* Only valid after the finish of initialization */
    if (1U == u1_init_flag)
    {
        switch (p_args->event)
        {
            case MOTOR_CALLBACK_EVENT_SPEED_FORWARD:
            break;

            case MOTOR_CALLBACK_EVENT_SPEED_BACKWARD:
            {
                mtr_set_reference(MTR_ID_1);
                break;
            }

            case MOTOR_CALLBACK_EVENT_CURRENT_FORWARD:
            {
                if (MOTOR_STATUS_ERROR != g_u1_motor_status1)
                {
                    p_motor1_instance->p_api->errorCheck(p_motor1_instance->p_ctrl, &g_u2_chk_error1);
                }
                /* Speed reference (VR2) is sampled here after A/D conversion. */
                g_u2_vr2_ad = get_vr2();
                break;
            }

            case MOTOR_CALLBACK_EVENT_CURRENT_BACKWARD:
            {
                break;
            }

            default:
            break;
        }
    }
} /* End of function mtr_callback_event1 */

/***********************************************************************************************************************
* Function Name : mtr_board_mode_control
* Description   : Motor status control by board UI
* Arguments     : u1_id - Motor identifical number
* Return Value  : None
***********************************************************************************************************************/
static void mtr_board_mode_control(uint8_t u1_id)
{
    uint8_t temp_sw_signal;
    uint8_t *p_motor_status;
    const motor_instance_t  *p_temp_motor_instance;
    uint8_t *p_stop_req;
    uint8_t *p_reset_req;

    if (MTR_ID_0 == u1_id)
    {
        p_motor_status = &g_u1_motor_status;
        p_temp_motor_instance = p_motor_instance;
        p_stop_req = &g_u1_stop_req;
        p_reset_req = &g_u1_reset_req;
    }
    else if (MTR_ID_1 == u1_id)
    {
        p_motor_status = &g_u1_motor_status1;
        p_temp_motor_instance = p_motor1_instance;
        p_stop_req = &g_u1_stop_req1;
        p_reset_req = &g_u1_reset_req1;
    }
    else
    {
        /* Do nothing */
    }

    /* Get status of motor control system */
    p_temp_motor_instance->p_api->statusGet(p_temp_motor_instance->p_ctrl, p_motor_status);

    switch (*p_motor_status)
    {
        case MOTOR_STATUS_STOP:
            temp_sw_signal = get_sw1(u1_id);

            /* Check SW1 */
            if ((MTR_FLG_SET == mtr_remove_sw_chattering(temp_sw_signal, SW1_ON, u1_id))
             && (MTR_FLG_SET != *p_stop_req))
            {
                /* SW1:ON and Stop request:None => Start the motor */
                p_temp_motor_instance->p_api->run(p_temp_motor_instance->p_ctrl);
            }
        break;

        case MOTOR_STATUS_RUN:
            temp_sw_signal = get_sw1(u1_id);

            /* Check SW1 */
            if ((MTR_FLG_SET == mtr_remove_sw_chattering(temp_sw_signal, SW1_OFF, u1_id))
             || (MTR_FLG_CLR != *p_stop_req))
            {
                /* SW1:OFF or Stop request:ON => Stop the motor */
                p_temp_motor_instance->p_api->stop(p_temp_motor_instance->p_ctrl);
            }
        break;

        case MOTOR_STATUS_ERROR:
            if (MTR_FLG_CLR == get_sw1(u1_id))
            {
                /* check SW2 & reset request flag */
                temp_sw_signal = get_sw2(u1_id);

                if ((SW_OFF == *p_reset_req)
                 && (MTR_FLG_SET == mtr_remove_sw_chattering(temp_sw_signal, SW2_ON, u1_id)))
                {
                    *p_reset_req = SW_ON;
                }
                else if ((SW_ON == *p_reset_req)
                      && (MTR_FLG_SET == mtr_remove_sw_chattering(temp_sw_signal, SW2_OFF, u1_id)))
                {
                    *p_reset_req = SW_OFF;
                    p_temp_motor_instance->p_api->reset(p_temp_motor_instance->p_ctrl);
                }
                else
                {
                    /* Do nothing */
                }
            }
        break;

        default:
            /* Do nothing */
        break;
    }
}   /* End of function mtr_board_mode_control */

/***********************************************************************************************************************
* Function Name : mtr_ics_mode_control
* Description   : Motor status control by RMW UI
* Arguments     : u1_id - Motor identifical number
* Return Value  : None
***********************************************************************************************************************/
static void mtr_ics_mode_control(uint8_t u1_id)
{
    uint8_t u1_temp;
    uint8_t *p_com_mode_system;
    uint8_t *p_g_mode_system;
    const motor_instance_t  *p_temp_motor_instance;
    uint8_t *p_motor_status;

    if (MTR_ID_0 == u1_id)
    {
        //p_com_mode_system = &com_u1_mode_system;
        p_g_mode_system = &g_u1_mode_system;
        p_temp_motor_instance = p_motor_instance;
        p_motor_status = &g_u1_motor_status;
    }
    else if (MTR_ID_1 == u1_id)
    {
        //p_com_mode_system = &com_u1_mode_system1;
        p_g_mode_system = &g_u1_mode_system1;
        p_temp_motor_instance = p_motor1_instance;
        p_motor_status = &g_u1_motor_status1;
    }
    else
    {
    }
    p_com_mode_system = &com_u1_both_mode_system;
    /*============================*/
    /*        Execute event       */
    /*============================*/
    u1_temp = *p_com_mode_system;

    if (*p_g_mode_system != u1_temp)
    {
        if (u1_temp > MOTOR_CTRL_EVENT_RESET)
        {
            *p_com_mode_system = *p_g_mode_system;
        }
        else
        {
            *p_g_mode_system = u1_temp;
            switch (u1_temp)
            {
                case MOTOR_CTRL_EVENT_STOP:
                    p_temp_motor_instance->p_api->stop(p_temp_motor_instance->p_ctrl);
                break;

                case MOTOR_CTRL_EVENT_RUN:
                    p_temp_motor_instance->p_api->run(p_temp_motor_instance->p_ctrl);
                break;

                case MOTOR_CTRL_EVENT_RESET:
                    p_temp_motor_instance->p_api->reset(p_temp_motor_instance->p_ctrl);
                break;

                default:
                    /* Do nothing */
                break;
            }
        }
    }

    p_temp_motor_instance->p_api->statusGet(p_temp_motor_instance->p_ctrl, p_motor_status);

    if (MOTOR_CTRL_EVENT_RESET == *p_g_mode_system)
    {
        if (MOTOR_STATUS_STOP == *p_motor_status)
        {
            /* Initialize private global variables for reset event */
            software_init(u1_id);
        }
        else if (MOTOR_STATUS_ERROR == *p_motor_status)
        {
            *p_g_mode_system   = MOTOR_CTRL_EVENT_ERROR;
            *p_com_mode_system = MOTOR_CTRL_EVENT_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    }
}

/***********************************************************************************************************************
* Function Name : mtr_board_led_control
* Description   : Set LED pattern depend on motor status
* Arguments     : u1_motor_status - Motor control status
*               : u1_id - Motor identifical number
* Return Value  : None
***********************************************************************************************************************/
static void mtr_board_led_control(uint8_t u1_motor_status, uint8_t u1_id)
{
    /***** LED control *****/
    switch (u1_motor_status)
    {
        case MOTOR_STATUS_STOP:
        {
            led1_off(u1_id);            /* LED1 off */
            led2_off(u1_id);            /* LED2 off */
            led3_off(u1_id);            /* LED3 off */
            break;
        }
        case MOTOR_STATUS_RUN:
        {
            led1_on(u1_id);             /* LED1 on */
            led2_off(u1_id);            /* LED2 off */
            break;
        }
        case MOTOR_STATUS_ERROR:
        {
            led1_off(u1_id);            /* LED1 off */
            led2_on(u1_id);             /* LED2 on */
            led3_off(u1_id);            /* LED3 off */
            break;
        }

        default:
        {
            led1_on(u1_id);             /* LED1 on */
            led2_on(u1_id);             /* LED2 on */
            led3_on(u1_id);             /* LED3 on */
            break;
        }
    }
} /* End of function mtr_board_led_control */

/***********************************************************************************************************************
* Function Name : mtr_remove_sw_chattering
* Description   : Get switch status with removing chattering
* Arguments     : u1_sw - Board interface switch signal
*                 u1_on_off - Detected status (ON/OFF)
* Return Value  : u1_remove_chattering_flg - Detection result
***********************************************************************************************************************/
static uint8_t mtr_remove_sw_chattering(uint8_t u1_sw, uint8_t u1_on_off, uint8_t u1_id)
{
    uint8_t u1_remove_chattering_flg;

    u1_remove_chattering_flg = MTR_FLG_CLR;

    switch (u1_id)
    {
        case MTR_ID_0:
            if (u1_on_off == u1_sw)
            {
                g_u1_sw_cnt++;
                if (CHATTERING_CNT < g_u1_sw_cnt)
                {
                    u1_remove_chattering_flg = MTR_FLG_SET;
                    g_u1_sw_cnt = 0U;
                }
            }
            else
            {
                g_u1_sw_cnt = 0U;
            }
        break;

        case MTR_ID_1:
            if (u1_on_off == u1_sw)
            {
                g_u1_sw_cnt1++;
                if (CHATTERING_CNT < g_u1_sw_cnt1)
                {
                    u1_remove_chattering_flg = MTR_FLG_SET;
                    g_u1_sw_cnt1 = 0U;
                }
            }
            else
            {
                g_u1_sw_cnt1 = 0U;
            }
        break;

        default:
            /* Do Nothing */
        break;
    }

    return (u1_remove_chattering_flg);
} /* End of function mtr_remove_sw_chattering */

/***********************************************************************************************************************
* Function Name : mtr_set_reference
* Description   : Set reference of speed by VR1
* Arguments     : u1_id - motor identifical number
* Return Value  : None
***********************************************************************************************************************/
static void mtr_set_reference(uint8_t u1_id)
{
    float temp_speed_ref;
    uint16_t *p_vr_ad;
    float vr_scaling;
    uint16_t max_speed;
    uint8_t  *p_stop_req;
    const motor_instance_t  *p_temp_motor_instance;

    if (BOARD_UI == g_u1_sw_userif)
    {
        if (MTR_ID_0 == u1_id)
        {
            p_vr_ad = &g_u2_vr1_ad;
            vr_scaling = VR1_SCALING;
            max_speed = MTR_MAX_SPEED_RPM;
            p_stop_req = &g_u1_stop_req;
            p_temp_motor_instance = p_motor_instance;
        }
        else
        {
            p_vr_ad = &g_u2_vr2_ad;
            vr_scaling = VR2_SCALING;
            max_speed = MTR_MAX_SPEED_RPM1;
            p_stop_req = &g_u1_stop_req1;
            p_temp_motor_instance = p_motor1_instance;
        }

        temp_speed_ref = -(((float)*p_vr_ad - ADJUST_OFFSET) * vr_scaling);

        if (temp_speed_ref >= max_speed)
        {
            temp_speed_ref = max_speed;
        }
        if (temp_speed_ref <= -max_speed)
        {
            temp_speed_ref = -max_speed;
        }

        /*=============================*/
        /*      Set speed reference    */
        /*=============================*/
        p_temp_motor_instance->p_api->speedSet(p_temp_motor_instance->p_ctrl, temp_speed_ref);

        /* Below low limit speed, stop the motor */
        if ((temp_speed_ref > (-STOP_RPM)) && (temp_speed_ref < STOP_RPM))
        {
            *p_stop_req = MTR_FLG_SET;
        }
        else
        {
            *p_stop_req = MTR_FLG_CLR;
        }
    }
}   /* End of function mtr_set_reference */

/***********************************************************************************************************************
* Function Name : get_vr1
* Description   : Get A/D converted value of VR1
* Arguments     : None
* Return Value  : A/D converted value of VR1
***********************************************************************************************************************/
static uint16_t get_vr1(void)
{
    uint16_t ad_data;

    g_adc0.p_api->read(g_adc0.p_ctrl, MTR_ADCH_VR1, &ad_data);

    return (ad_data);
} /* End of function get_vr1 */

/***********************************************************************************************************************
* Function Name : get_vr2
* Description   : Get A/D converted value of VR2
* Arguments     : None
* Return Value  : A/D converted value of VR2
***********************************************************************************************************************/
static uint16_t get_vr2(void)
{
    uint16_t ad_data;

    g_adc0.p_api->read(g_adc0.p_ctrl, MTR_ADCH_VR2, &ad_data);

    return (ad_data);
} /* End of function get_vr2 */

/***********************************************************************************************************************
* Function Name : get_sw1
* Description   : Get state of SW1
* Arguments     : None
* Return Value  : State of SW1
***********************************************************************************************************************/
static uint8_t get_sw1(uint8_t u1_id)
{
    uint8_t tmp_port;

    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinRead(&g_ioport_ctrl, MTR_PORT_SW1_1, &tmp_port);
        break;

        case MTR_ID_1:
            R_IOPORT_PinRead(&g_ioport_ctrl, MTR_PORT_SW1_2, &tmp_port);
        break;

        default:
            /* Do Nothing */
        break;
    }

    return (tmp_port);
} /* End of function get_sw1 */

/***********************************************************************************************************************
* Function Name : get_sw2
* Description   : Get state of SW2
* Arguments     : None
* Return Value  : State of SW2
***********************************************************************************************************************/
static uint8_t get_sw2(uint8_t u1_id)
{
    uint8_t tmp_port;

    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinRead(&g_ioport_ctrl,MTR_PORT_SW2_1, &tmp_port);
        break;

        case MTR_ID_1:
            R_IOPORT_PinRead(&g_ioport_ctrl,MTR_PORT_SW2_2, &tmp_port);
        break;

        default:
            /* Do Nothing */
        break;
    }

    return (tmp_port);
} /* End of function get_sw2 */

/***********************************************************************************************************************
* Function Name : led1_on
* Description   : Turn on LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_on(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1_1, MTR_LED_ON);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1_2, MTR_LED_ON);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led1_on */

/***********************************************************************************************************************
* Function Name : led2_on
* Description   : Turn on LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_on(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2_1, MTR_LED_ON);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2_2, MTR_LED_ON);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led2_on */

/***********************************************************************************************************************
* Function Name : led3_on
* Description   : Turn on LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_on(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED3_1, MTR_LED_ON);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED3_2, MTR_LED_ON);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led3_on */

/***********************************************************************************************************************
* Function Name : led1_off
* Description   : Turn off LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_off(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED1_1 , MTR_LED_OFF);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED1_2 , MTR_LED_OFF);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led1_off */

/***********************************************************************************************************************
* Function Name : led2_off
* Description   : Turn off LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_off(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED2_1 , MTR_LED_OFF);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED2_2 , MTR_LED_OFF);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led2_off */

/***********************************************************************************************************************
* Function Name : led3_off
* Description   : Turn off LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_off(uint8_t u1_id)
{
    switch (u1_id)
    {
        case MTR_ID_0:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED3_1 , MTR_LED_OFF);
        break;

        case MTR_ID_1:
            R_IOPORT_PinWrite(&g_ioport_ctrl,MTR_PORT_LED3_2 , MTR_LED_OFF);
        break;

        default:
            /* Do Nothing */
        break;
    }
} /* End of function led3_off */

/***********************************************************************************************************************
* Function Name : gpt_periodset
* Description   : Set GPT carrier period count
* Arguments     : p_ctrl - gpt instance
*                 period_counts - set period counts
* Return Value  : None
***********************************************************************************************************************/
static void gpt_periodset (timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;

    /* Set period to buffer register */
    p_instance_ctrl->p_reg->GTPBR = period_counts;
    /* Set staggered period to setting register */
    p_instance_ctrl->p_reg->GTPR = (uint32_t)((float)period_counts * 1.5F);
}
