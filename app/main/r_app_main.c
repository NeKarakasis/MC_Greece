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
#include "r_cpu_diag.h"
#include "r_ram_diag.h"
#include "r_adc_diag.h"
#include "r_rom_diag.h"


#define CHECKSUM_START_ADDRESS (0xFFFFFF10)
/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
/*** DTC table for ICS ***/
#pragma section DTCTBL
uint32_t g_dtc_table[256];
#pragma section
/*************************/

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static void     r_app_main_ui_mainloop(void);           /* User interface control routine */
static void     r_app_main_init_motor_ctrl(void);       /* Initialize motor control instance */
static void     r_app_main_start_motor_ctrl(void);      /* Start motor control */
static void		safety_CPU_test(void);					/* Test CPU for safety */
static void 	ram_test_sample(void);					/* Test RAM for safety */
static void		rom_test_sample(void);					/* Test RAM for safety */
static void		adc_init_sample(void);					/* Initialaze for test the ADC for safety*/
static void 	adc_test_sample(void);					/* The ADC for safety */

/***********************************************************************************************************************
* Function Name : main
* Description   : Initialization and main routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
volatile static int32_t adc_cmt_counts[4] = {0,0,0,0};
void main(void)
{
clrpsw_i();                                       /* Disable interrupt */

    /* LED off */
    //r_app_board_ui_led_control(STATEMACHINE_STATE_STOP);

    /* Initialize ICS */
    ics2_init((void*)g_dtc_table, APP_CFG_SCI_CH_SELECT, ICS_INT_LEVEL, ICS_BRR, ICS_INT_MODE);

    /* Initialize open motor control instance */
    r_app_main_init_motor_ctrl();

    /* Initialize RMW communication support for motor control
     * MUST be called after motor instance being configured */
    r_app_rmw_ui_init();

setpsw_i();                                       /* Enable interrupt */

    /* Start peripheral functions */
    r_app_main_start_motor_ctrl();

/*clrpsw_i();
MTU.TRWERA.BIT.RWE = 1U;
int32_t mtu_counter = MTU4.TCNT;
 Disable interrupt

int32_t tempcounter;
safety_CPU_test();
ram_test_sample();
rom_test_sample();
tempcounter = CMT1.CMCNT;
R_Config_S12AD0_Stop();
S12AD.ADCSR.BIT.ADST = 0;
adc_cmt_counts[0] = CMT1.CMCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 1U;
tempcounter =MTU4.TCNT;
adc_init_sample();
adc_cmt_counts[1] = MTU4.TCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 0U;
tempcounter =CMT1.CMCNT;
adc_test_sample();
adc_cmt_counts[2] = CMT1.CMCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 1U;
tempcounter =MTU4.TCNT;
S12AD.ADCSR.BIT.ADST = 1;
R_Config_S12AD0_Create();
R_Config_S12AD0_Start();
adc_cmt_counts[3] = MTU4.TCNT - tempcounter;
MTU.TRWERA.BIT.RWE = 0U;
setpsw_i();*/                                       /* Enable interrupt */


    /*** Main routine ***/
    while (1)
    {
        /* User interface */
        r_app_main_ui_mainloop();

        /* Clear watch dog timer */
        R_Config_IWDT_Restart();
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
    r_app_rmw_copy_com_to_buffer();

    /*============================*/
    /*   User interface switch    */
    /*============================*/
    u1_temp = com_u1_sw_userif;

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

    /*============================*/
    /*        Execute event       */
    /*============================*/
    if (MAIN_UI_RMW == g_u1_sw_userif)
    {
        /* Main process for ICS UI */
        r_app_rmw_ui_mainloop();
    }
    else if (MAIN_UI_BOARD == g_u1_sw_userif)
    {
        /* Main process for board UI */
        r_app_board_ui_mainloop();
    }
    else
    {
        /* Do Nothing */
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
    R_Config_POE_Create();
    R_Config_POE_Start();
} /* End of function r_app_main_start_motor_ctrl */

static void SafetyErrorHandler(void)
{
	while(1);
}
int32_t duration_cpu_sf_test[CPU_DIAG_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0};

static void safety_CPU_test(void)
{
	const uint32_t forceFail = 1; /* Force fail: Disable */
	uint32_t index;
	int32_t result;
	uint32_t CPU_index_Table[CPU_DIAG_SIZE] = {0,1,2,3,4,5,6,7,8,9,11,12};
	/* Switch processor mode to supervisor mode. */
	/* Disabling interrupts */
	for (index = 0; index < CPU_DIAG_SIZE; index++)
	{
		result = 0;
		MTU.TRWERA.BIT.RWE = 1U;
		int32_t mtu_counter = MTU4.TCNT;
			R_CPU_Diag(CPU_index_Table[index], forceFail, &result);
			if (result != 1)
			{
				SafetyErrorHandler(); /* Failure detection */
			}
			duration_cpu_sf_test[index] = MTU4.TCNT - mtu_counter;
			MTU.TRWERA.BIT.RWE = 0U;
	}
/* Enabling interrupts */
}

int32_t duration_ram_sf_test[10] = {0,0,0,0,0,0,0,0};

void ram_test_sample(void)
{
uint32_t area = 0;
uint32_t index;
uint32_t destructive;
/* Disabling ECC */
/* Disabling interrupts */
	for (index = 0; index < numberOfBUT0; index++)
	{
		MTU.TRWERA.BIT.RWE = 1U;
		//int32_t mtu_counter = CMT0.CMCNT;
		int32_t mtu_counter = MTU4.TCNT;
		if (index == 0)
		{ /* Buffer block */
			destructive = RAM_MEM_DT;
		}
		else
		{
			destructive = RAM_MEM_NDT;
		}
	/* Call API */
		R_RAM_Diag(area, index, destructive);
		/* Check API result */
		if ( (RramResult1 != RAM_PASS) || (RramResult2 != RAM_PASS) )
		{
			SafetyErrorHandler(); /* Failure detection */
		}
		if (index < 10)
		{
			duration_ram_sf_test[index] = MTU4.TCNT - mtu_counter;
		}
		MTU.TRWERA.BIT.RWE = 0U;
	}
/* Enabling interrupts */
/* Enabling ECC */
}

#if 1
int32_t duration_adc_sf_test[3] = {0,0,0};
int32_t duration_adc_sf_test_cmt[3] = {0,0,0};

void adc_init_sample(void)
{
	R_ADC_Diag_Init(); /* Call API */
}

void adc_test_sample(void)
	{
	uint8_t unit = 0;
	uint8_t status = 0;
	uint8_t voltage;
	uint16_t value;
		for ( voltage = 1; voltage <= 2; voltage++ )
		{
			MTU.TRWERA.BIT.RWE = 1U;
			int32_t mtu_counter = MTU4.TCNT;
			int32_t cmt_counter = CMT1.CMCNT;
			value = R_ADC_Diag(unit, voltage); /* Call API */
			status = (value & 0xC000) >> 14; /* Get self-diagnosis status */
			value = value & 0x0FF0; /* Extract valid upper 8bit from A/D-converted value*/
			/* Check API result */
			switch ( status )
			{
				case 1U:
					if ( 0x000 != value)
						{
							SafetyErrorHandler(); /* Failure detection */
						}
					break;
				case 2U:
					if (( 0x800 != value ) && ( 0x7F0 != value ))
						{
							SafetyErrorHandler(); /* Failure detection */
						}
					break;
				case 3U:
					if ( 0xFF0 != value )
						{
							SafetyErrorHandler(); /* Failure detection */
						}
					break;
				default:
						SafetyErrorHandler(); /* Failure detection */
			}
			duration_adc_sf_test[voltage - 1] = MTU4.TCNT - mtu_counter;
			duration_adc_sf_test_cmt[voltage - 1] = CMT1.CMCNT - cmt_counter;

			MTU.TRWERA.BIT.RWE = 0U;
		}
}
#endif
uint16_t calChecksum1 = 0;
uint16_t calChecksum2 = 0;
uint16_t calChecksum3 = 0;
int16_t duration_rom_sf_test[6]={0,0,0,0,0,0};
uint16_t *pExpChecksum = (uint16_t*)CHECKSUM_START_ADDRESS;
uint32_t startAddress[4];
uint32_t endAddress[4];

void rom_test_sample(void)
{
	MTU.TRWERA.BIT.RWE = 1U;
	uint32_t start;
	uint32_t end;
	uint32_t mode;
	//uint16_t calChecksum;
	/* Area where the expected CRC checksum values of each CODE block are aggregated. */
	R_ROM_Diag_Init();
	/* Disabling interrupts */
	/* ROM Test: Block0 (4 KByte) */
	start = 0xFFFF0100;
	end = 0xFFFF10FF;
	mode = 0;
	int32_t mtu_counter = CMT1.CMCNT;
	calChecksum1 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[0] = mtu_counter - CMT1.CMCNT;

	start = 0xFFFF1100;
	end = 0xFFFF20FF;
	mtu_counter = CMT1.CMCNT;
	calChecksum2 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[1] = mtu_counter - CMT1.CMCNT;

	if (calChecksum1 != pExpChecksum [0])
	{
		SafetyErrorHandler(); /* Failure detection */
	}
/*	 ROM Test: Block1 (4 KByte, 4 time-wise split)
	start = 0xFFFF1100;
	end = 0xFFFF14FF;
	mode = 0;

	mtu_counter = CMT1.CMCNT;
	calChecksum3 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[2] = mtu_counter - CMT1.CMCNT;

	start = 0xFFFF1500;
	end = 0xFFFF18FF;
	mode = 1;
	mtu_counter = CMT1.CMCNT;
	calChecksum3 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[3] = mtu_counter - CMT1.CMCNT;

	start = 0xFFFF1900;
	end = 0xFFFF1CFF;
	mtu_counter = CMT1.CMCNT;
	calChecksum3 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[4] = mtu_counter - CMT1.CMCNT;

	start = 0xFFFF1D00;
	end = 0xFFFF20FF;
	mtu_counter = CMT1.CMCNT;
	calChecksum3 = R_ROM_Diag(start, end, mode);
	duration_rom_sf_test[5] = mtu_counter - CMT1.CMCNT;*/
	for(int i=  0; i < 64*2; i++)
	{
		start = 0xFFFF1100 + 0x20*i;
		end = start-1 + 0x20;
		if (i == 0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
		mtu_counter = MTU4.TCNT;
		calChecksum3 = R_ROM_Diag(start, end, mode);
		if (i<4)
		{
			duration_rom_sf_test[i+2] = mtu_counter - MTU4.TCNT;
			startAddress[i] = start;
			endAddress[i] = end;
		}


	}
	if (calChecksum3 != pExpChecksum [1])
	{
		SafetyErrorHandler(); /* Failure detection */
	}
/* Enabling interrupts */
	MTU.TRWERA.BIT.RWE = 0U;
}
