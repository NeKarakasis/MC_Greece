/*
 * safety_cfg.h
 *
 *  Created on: 20 Ιουν 2025
 *      Author: a5155223
 */
#ifndef SAFETY_MODULE_CFG_SAFETY_FUNCTIONS_API_H_
#define SAFETY_MODULE_CFG_SAFETY_FUNCTIONS_API_H_
#include "stdint.h"


#define CPU_DIAG_SIZE				(12)		/* Number of the size of CPU_DIAG_Table 0-9,11,12*/
#define EVOLTAGE	6							/* The voltage trigger for voltage monitor fault triggering. 6 -> 2.9V*/

/*!!!!Start of CPU tests configuration!!!!!
that code should be modified based of the hardware.*/

// RX13T-specific CPU test indices
#define CPU_DIAG_TABLE_SIZE 12
#define TOTAL_CPU_TEST_PARTS 2

extern const uint32_t CPU_index_Table[CPU_DIAG_TABLE_SIZE];


/*!!!!Start of RAM tests configuration!!!!!
that code should be modified based of the hardware.*/

#define RAM_MAIN_BUFFER_ADDRESS  0x00000400
#define RAM_RESULT_ADDRESS_1 0x00000420
#define RAM_RESULT2_ADDRESS_2 0x00000450


#define RAM_TEST_START_ADDRESS   (0x00000400U)
#define RAM_TEST_TOTAL_SIZE      (1500U)       // Total RAM size in bytes
#define RAM_TEST_BLOCK_SIZE      (4U)          // Block size in bytes

#define RAM_TEST_NUM_BLOCKS      (RAM_TEST_TOTAL_SIZE / RAM_TEST_BLOCK_SIZE)


/*end of RAM test configuration*/

/*!!!!Start of ROM tests configuration!!!!!
that code should be modified based of the hardware.*/

#define ROM_START_ADDR         (0xFFFF0100UL)
#define ROM_TOTAL_SIZE         (31UL * 1024UL)         /* 16 KB */
#define ROM_BLOCK_SIZE         (1UL * 1024UL)          /* 1 KB */
#define ROM_NUM_BLOCKS         (ROM_TOTAL_SIZE / ROM_BLOCK_SIZE)
#define ROM_TEST_CHUNK_SIZE    (32UL)                  /* 32 bytes per test */

#define CHECKSUM_START_ADDRESS (0xFFFFFF10)				/* Address in flash where expected CRCs are stored */


// Modes for RAM tests.
typedef enum {
    RAM_TEST_MODE_AUTO = 0,// every is managed internally
	RAM_TEST_MODE_MANUAL // start and stop address should be input. Internally will be managed all the other
} ram_test_mode_t;

// voltage levels for voltage FuSa tests
typedef enum
{
    ADC_VOLTAGE_LEVEL_0 = 1,      // 0V
    ADC_VOLTAGE_LEVEL_HALF,       // 1/2 Vref
    ADC_VOLTAGE_LEVEL_FULL        // Full Vref
} adc_voltage_level_t;

typedef struct
{
	adc_voltage_level_t typeOfTest;
	uint8_t voltageUnit;
	void (*ReInitialazationADC)(void);
	void (*StartADC)(void);
}st_adc_driver;

// Modes for ROM tests
typedef enum
{
    ROM_TEST_MODE_AUTO = 0, // start and stop address should be input. Internally will be managed all the other
    ROM_TEST_MODE_MANUAL //// every is managed internally
} rom_test_mode_t;


extern void		safety_CPU_test(uint32_t part_index);	/* Test CPU for safety */
extern void 	ram_test_sample(ram_test_mode_t mode, uint32_t block_index);					/* Test RAM for safety */
extern void		rom_test_sample(rom_test_mode_t mode, uint32_t manual_start, uint32_t manual_end);					/* Test RAM for safety */
extern void 	adc_test_sample(st_adc_driver voltage);					/* The ADC for safety */
extern void 	FuSa_clock_monitor(void);				/*Initiate and start the clock monitor*/
extern void 	IWDT_Reset_out_chk(void);				/*Check after reset if it came from PC fault*/
extern void		FuSa_PC_init(void);						/*Initiate the PC monitor peripheral*/
extern void 	FuSa_PC_refresh(void);					/* Refresh the PC counter */
extern void		FuSa_Voltage_init(void);				/*Initiate the Voltage monitor peripheral*/
extern void 	rom_test_init(void);

#endif /* SAFETY_MODULE_CFG_SAFETY_FUNCTIONS_API_H_ */
