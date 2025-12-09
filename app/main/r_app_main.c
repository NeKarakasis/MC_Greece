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
* File Name   : r_app_main.c
* Description : The main function and the processes of application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.10.2021 1.00
*         : 12.05.2022 1.10
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_app_control_cfg.h"
#include "r_app_main.h"
#include "r_app_rmw.h"
#include "r_app_board_ui.h"
#include "r_motor_sensorless_vector_api.h"
#include "safety_functions_api.h"
#include "pin.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
/*** DTC table for ICS ***/
#pragma section DTCTBL
uint32_t g_dtc_table[256];
#pragma section
/*************************/
static e_motor_id_t g_current_motor_id = CIRCULATION_MOTOR;      /* what the stack is currently opened for */
static volatile e_motor_id_t g_requested_motor_id = CIRCULATION_MOTOR; /* what the UI wants */
static volatile bool g_req_motor_change = false;          /* set true when UI asks to switch */
static void app_handle_motor_selection(void);			  /* Handles the motor selection */
void app_request_motor(e_motor_id_t new_id); 			   /* Handles the request for motor selection */
e_motor_id_t requested_motor;									// temperature variable to request new motor to run


/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static void     r_app_main_ui_mainloop(void);           /* User interface control routine */
static void     r_app_main_init_motor_ctrl(void);       /* Initialize motor control instance */
static void     r_app_main_start_motor_ctrl(void);      /* Start motor control */
static void 	app_set_motor_output_path(e_motor_id_t id); /* Implementation to toggle the output relay */

#define USE_UI 0
#define STARTUP_MOTOR_CONTROL_DELAY		1
#define VDC_VOLTAGE_STABLE_WAIT_COUNT	(0x325AA0U)  /* motor control delays 500ms for stabilization of VDC and analog inputs */

/***********************************************************************************************************************
* Function Name : main
* Description   : Initialization and main routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
volatile static int32_t adc_cmt_counts[4] = {0,0,0,0};
volatile static uint32_t d_count = 0;
static float g_manual_speed = 0;
static int g_manual_state_cmd = 0;
static int temp_g_manual_state_cmd = 0;
void main(void)
{
clrpsw_i();                                       /* Disable interrupt */

    /* LED off */
    //r_app_board_ui_led_control(STATEMACHINE_STATE_STOP);

    /* Initialize ICS */
#if USE_UI
    ics2_init((void*)g_dtc_table, APP_CFG_SCI_CH_SELECT, ICS_INT_LEVEL, ICS_BRR, ICS_INT_MODE);
#endif
#if STARTUP_MOTOR_CONTROL_DELAY
    for (d_count = 0U; d_count < VDC_VOLTAGE_STABLE_WAIT_COUNT; d_count++)
    {
        nop();
    }
#endif
    /* Initialize open motor control instance */
    r_app_main_init_motor_ctrl();
#if USE_UI
    /* Initialize RMW communication support for motor control
     * MUST be called after motor instance being configured */
    r_app_rmw_ui_init();
#endif

setpsw_i();                                       /* Enable interrupt */

    /* Start peripheral functions */
    r_app_main_start_motor_ctrl();

//Disable interrupt
/*
clrpsw_i();
MTU.TRWERA.BIT.RWE = 1U;
int32_t mtu_counter = MTU4.TCNT;

int32_t tempcounter;
uint16_t i = 0;
for (i = 0; i < 2; i++)
{
	safety_CPU_test(i);
}

for (i = 0; i < 64; i++)
{
	ram_test_sample(RAM_TEST_MODE_AUTO,0);
}
rom_test_init();
for (i = 0; i < 258; i++)
{
	rom_test_sample(ROM_TEST_MODE_AUTO,0,0);
}

//tempcounter = CMT1.CMCNT;
R_Config_S12AD0_Stop();
S12AD.ADCSR.BIT.ADST = 0;
//adc_cmt_counts[0] = CMT1.CMCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 1U;
tempcounter = MTU4.TCNT;
//adc_init_sample();
adc_cmt_counts[1] = MTU4.TCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 0U;
tempcounter =CMT1.CMCNT;
st_adc_driver st_voltage_test;
st_voltage_test.typeOfTest = ADC_VOLTAGE_LEVEL_0;
st_voltage_test.voltageUnit = 0;
st_voltage_test.ReInitialazationADC = R_Config_S12AD0_Create;
st_voltage_test.StartADC = R_Config_S12AD0_Start;
adc_test_sample(st_voltage_test);
st_voltage_test.typeOfTest = ADC_VOLTAGE_LEVEL_HALF;
adc_test_sample(st_voltage_test);

//adc_cmt_counts[2] = CMT1.CMCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 1U;
tempcounter =MTU4.TCNT;
S12AD.ADCSR.BIT.ADST = 1;
adc_cmt_counts[3] = MTU4.TCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 0U;

FuSa_clock_monitor();
IWDT_Reset_out_chk();

FuSa_PC_init();

// 2.9V trigger voltage
FuSa_Voltage_init();

setpsw_i();                                        Enable interrupt
*/


    /*** Main routine ***/
    while (1)
    {
        /* User interface */
        r_app_main_ui_mainloop();
        /* Clear watch dog timer */
    }
} /* End of function main */

/***********************************************************************************************************************
* Function Name : r_app_main_ui_mainloop
* Description   : User interface control routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_main_ui_mainloop(void)
{
    uint8_t u1_temp;
    uint8_t u1_motor_status;

    /*============================*/
    /*        Get ICS value       */
    /*============================*/

#if USE_UI
    r_app_rmw_copy_com_to_buffer();
#endif

    /*============================*/
    /*   User interface switch    */
    /*============================*/
    u1_temp = com_u1_sw_userif;
    app_request_motor(requested_motor);
    app_handle_motor_selection();
    g_u1_sw_userif = MAIN_UI_SERIAL;
    if (g_u1_sw_userif != MAIN_UI_SERIAL)
    {
		if (g_u1_sw_userif != u1_temp)
		{
			if (u1_temp >= MAIN_UI_SIZE)
			{
				com_u1_sw_userif = g_u1_sw_userif;
			}
			else
			{
				g_u1_sw_userif = u1_temp;

				if (MAIN_UI_RMW == g_u1_sw_userif)
				{
					/* Get status of motor control system */
					u1_motor_status  = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);
					g_u1_system_mode = u1_motor_status;
				}
				else
				{
					/* Do Nothing */
				}
			}
		}
    }
    /*============================*/
    /*        Execute event       */
    /*============================*/

    if (MAIN_UI_RMW == g_u1_sw_userif)
    {
         /*Main process for ICS UI*/
        r_app_rmw_ui_mainloop();
    }
    else if (MAIN_UI_BOARD == g_u1_sw_userif)
    {
        /* Main process for board UI */
        r_app_board_ui_mainloop();
    }

    else if (MAIN_UI_SERIAL == g_u1_sw_userif)
    {
        switch (g_manual_state_cmd)
            {
                case STATEMACHINE_STATE_STOP:
                    if (temp_g_manual_state_cmd == STATEMACHINE_STATE_RUN)
                    {
                        R_MOTOR_SENSORLESS_VECTOR_MotorStart(&g_st_sensorless_vector); /* Execute active event */
                        g_manual_state_cmd = 1;
                    }
                break;

                case STATEMACHINE_STATE_RUN:

                    if (temp_g_manual_state_cmd == STATEMACHINE_STATE_STOP) /* Check SW1 */
                    {
                        R_MOTOR_SENSORLESS_VECTOR_MotorStop(&g_st_sensorless_vector); /* Execute stop event */
                        g_manual_state_cmd = 0;
                    }
                    u1_temp = R_MOTOR_SENSORLESS_VECTOR_LoopModeStatusGet(&g_st_sensorless_vector);
                    if (u1_temp == MOTOR_LOOP_SPEED)
                    {
                        /*=============================*/
                        /*     Set speed reference     */
                        /*=============================*/

                        R_MOTOR_SENSORLESS_VECTOR_SpeedSet(&g_st_sensorless_vector, g_manual_speed);
                                                                                        /* set speed reference */
                    }
                    else
                    {
                        /* Do nothing */
                    }

            }
    }
    else
    {
         //Do Nothing
    }


    /*============================*/
    /*         LED control        */
    /*============================*/
    u1_motor_status = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);
                                                                        /* Get status of motor control system */
    //r_app_board_ui_led_control(u1_motor_status);
} /* End of function r_app_main_ui_mainloop */

/***********************************************************************************************************************
* Function Name : r_app_main_init_motor_ctrl
* Description   : Open motor control module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_main_init_motor_ctrl(void)
{
    /* Create a Motor Instance */
    R_MOTOR_SENSORLESS_VECTOR_Open();
} /* End of function r_app_main_init_motor_ctrl */

/***********************************************************************************************************************
* Function Name : r_app_main_start_motor_ctrl
* Description   : Start motor control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void r_app_main_start_motor_ctrl(void)
{
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    /* initialize timer setting */
    R_Config_MTU3_MTU4_StopTimerCtrl();
    R_Config_MTU3_MTU4_StartTimerCount();

    /* Start of A/D converter */
    R_Config_MTU3_MTU4_StartAD();
#elif defined(MOTOR_SHUNT_TYPE_2_SHUNT)
    /* initialize timer setting */
    R_Config_MOTOR_StopTimerCtrl();
    R_Config_MOTOR_StartTimerCount();

    /* Start of A/D converter */
	R_Config_S12AD0_Start();
#endif

    /* Start of CMT0 */
    R_Config_CMT0_Start();

    /* Clear POE with Create function before starting POE to avoid miss detection */
    //R_Config_POE_Create();
    //R_Config_POE_Start();
} /* End of function r_app_main_start_motor_ctrl */

static void app_handle_motor_selection(void)
{
    if (!g_req_motor_change)
    {
        return;
    }

    /* Only perform change when the system is STOP */
    if (g_u1_system_mode != STATEMACHINE_EVENT_STOP)
    {
        return; /* defer until STOP */
    }

    /* Double-check underlying MC status is also STOP (or ERROR) */
/*    uint8_t st = R_MOTOR_SENSORLESS_VECTOR_StatusGet(&g_st_sensorless_vector);
    if ((st != STATEMACHINE_STATE_STOP) && (st != STATEMACHINE_STATE_ERROR))
    {
        return;  still stopping → try again next loop
    }*/

    /* Close current instance (idempotent if already closed) */
    R_MOTOR_SENSORLESS_VECTOR_Close();

    /* Switch the physical endpoints first to avoid energizing the wrong motor */
    app_set_motor_output_path(g_requested_motor_id);

    /* Pick the new motor’s parameter set */
    R_MOTOR_SENSORLESS_VECTOR_Set_motor_id(g_requested_motor_id);

    /* Re-open with parameters of the selected motor */
    R_MOTOR_SENSORLESS_VECTOR_Open();

    /* Commit */
    g_current_motor_id  = g_requested_motor_id;
    g_req_motor_change  = false;
}

void app_request_motor(e_motor_id_t new_id)
{
    if (new_id == g_current_motor_id)
    {
        return; /* no-op */
    }
    g_requested_motor_id = new_id;
    g_req_motor_change   = true;
}

static void app_set_motor_output_path(e_motor_id_t id)
{
    /* Drive your relay GPIO(s) here so the inverter endpoints go to the chosen motor */
       if (id == CIRCULATION_MOTOR)
       {
    	   PIN_WRITE(RelayForMotors) = 0;
       }
       else
       {
    	   PIN_WRITE(RelayForMotors) = 1;
       }
}
