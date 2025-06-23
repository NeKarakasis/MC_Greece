#include <cfg/safety_functions_api.h>
/*
 * safety_functions_api.c
 *
 *  Created on: 20 Ιουν 2025
 *      Author: a5155223
 */
#include "r_smc_entry.h"
#include "safety_functions_api.h"
#include "r_cpu_diag.h"
#include "r_ram_diag.h"
#include "r_adc_diag.h"
#include "r_rom_diag.h"
#include "r_clock_mon.h"
#include "r_vol_mon.h"
#include "r_pc_mon.h"
#include "safety_error_codes.h"



// the interrupt functions based of the RX13T interrupt vector.
#pragma interrupt r_Config_CAC_OSCILATION_interrupt(vect = VECT_CAC_FERRF)
#pragma interrupt r_Config_CAC_OVERFLOW_interrupt(vect = VECT_CAC_OVFF)
#pragma interrupt r_Config_LVD_LVD1_interrupt(vect = VECT_LVD_LVD1)


// FuSa functions, those functions will be called in the code when it is necessery
void	safety_CPU_test(void);					/* Test CPU for safety */
void 	ram_test_sample(void);					/* Test RAM for safety */
void	rom_test_sample(void);					/* Test RAM for safety */
void	adc_init_sample(void);					/* Initialaze for test the ADC for safety*/
void 	adc_test_sample(void);					/* The ADC for safety */
void 	FuSa_clock_monitor(void);				/*Initiate and start the clock monitor*/
void 	IWDT_Reset_out_chk(void);				/*Check after reset if it came from PC fault*/
void	FuSa_PC_init(void);						/*Initiate the PC monitor peripheral*/
void	FuSa_Voltage_init(void);				/*Initiate the Voltage monitor peripheral*/

/***********************************************************************************************************************
* Function Name : SafetyErrorHandler
* Description   : Handles safety errors by entering an infinite loop.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void SafetyErrorHandler(void)
{
	while(1);
}
int32_t duration_cpu_sf_test[CPU_DIAG_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0};

/***********************************************************************************************************************
* Function Name : safety_CPU_test
* Description   : Executes CPU self-diagnostic tests and records durations.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void safety_CPU_test(void)
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

/***********************************************************************************************************************
* Function Name : ram_test_sample
* Description   : Executes RAM diagnostic tests and records durations.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
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

int32_t duration_adc_sf_test[3] = {0,0,0};
int32_t duration_adc_sf_test_cmt[3] = {0,0,0};
/***********************************************************************************************************************
* Function Name : adc_init_sample
* Description   : Initializes the ADC self-diagnostic functionality.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void adc_init_sample(void)
{
	R_ADC_Diag_Init(); /* Call API */
}

/***********************************************************************************************************************
* Function Name : adc_test_sample
* Description   : Performs ADC self-diagnostic test and checks results.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
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

uint16_t calChecksum1 = 0;
uint16_t calChecksum2 = 0;
uint16_t calChecksum3 = 0;
int16_t duration_rom_sf_test[6]={0,0,0,0,0,0};
uint16_t *pExpChecksum = (uint16_t*)CHECKSUM_START_ADDRESS;
uint32_t startAddress[4];
uint32_t endAddress[4];
/***********************************************************************************************************************
* Function Name : rom_test_sample
* Description   : Performs ROM checksum diagnostics and validates results.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
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

/***********************************************************************************************************************
* Function Name : FuSa_clock_monitor
* Description   : Initializes and starts the clock monitor with specific settings.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void FuSa_clock_monitor(void)
{
/* Measurement target clocks: main, Frequency of measurement target clock: 12 MHz, Tolerance limit: 25 % */
uint32_t clk_sel = 1, clock_freq = 32000, tolerance = 16;
R_CLOCK_Mon_Init(clk_sel, clock_freq, tolerance); /* Call API */
}

/***********************************************************************************************************************
* Function Name : r_Config_CAC_OSCILATION_interrupt
* Description   : Callback for clock oscillation fault interrupt.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_Config_CAC_OSCILATION_interrupt()
{
	SafetyErrorHandler();
}

/***********************************************************************************************************************
* Function Name : r_Config_CAC_OVERFLOW_interrupt
* Description   : Callback for clock monitor overflow interrupt.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_Config_CAC_OVERFLOW_interrupt()
{
	SafetyErrorHandler();
}

/***********************************************************************************************************************
* Function Name : r_Config_LVD_LVD1_interrupt
* Description   : Callback for voltage detection interrupt (LVD1).
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_Config_LVD_LVD1_interrupt()
{
	SafetyErrorHandler();

}

/***********************************************************************************************************************
* Function Name : IWDT_Reset_out_chk
* Description   : Checks the cause of reset and handles if PC fault detected.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void IWDT_Reset_out_chk(void) /* The function to check if an error has been detected by this software */
{
	int32_t result = 0;
	R_PC_Mon_Check(&result); /* Call API */
	if (result != 1)
	{
		//SafetyErrorHandler(); /* Failure detection */
	}
}

/***********************************************************************************************************************
* Function Name : FuSa_PC_init
* Description   : Initializes the Program Counter (PC) monitor module.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void FuSa_PC_init(void)
{
	R_PC_Mon_Init(IWDT_CKS_DIV_128, IWDT_WINDOW_START_75, IWDT_WINDOW_END_0, IWDT_ACTION_RST);

}

/***********************************************************************************************************************
* Function Name : FuSa_Voltage_init
* Description   : Initializes the voltage monitoring module.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void FuSa_Voltage_init(void)
{
	uint8_t evoltage = EVOLTAGE;
	R_VOL_Mon_Init(evoltage);
}
