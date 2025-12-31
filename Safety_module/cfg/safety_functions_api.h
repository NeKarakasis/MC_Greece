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
#define RAM_TEST_TOTAL_SIZE      (6800U)       // Total RAM size in bytes
#define RAM_TEST_BLOCK_SIZE      (4U)          // Block size in bytes

#define RAM_TEST_NUM_BLOCKS      (RAM_TEST_TOTAL_SIZE / RAM_TEST_BLOCK_SIZE)


/*end of RAM test configuration*/

/*!!!!Start of ROM tests configuration!!!!!
that code should be modified based of the hardware.*/

#define ROM_START_ADDR         (0xFFFF0100UL)
#define ROM_TOTAL_SIZE         (34UL * 1024UL)         /* 16 KB */
#define ROM_BLOCK_SIZE         (1UL * 1024UL)          /* 1 KB */
#define ROM_NUM_BLOCKS         (ROM_TOTAL_SIZE / ROM_BLOCK_SIZE)
#define ROM_TEST_CHUNK_SIZE    (32UL)                  /* 32 bytes per test */

#define CHECKSUM_START_ADDRESS (0xFFFFFF10)				/* Address in flash where expected CRCs are stored */


// basic configuration of FuSa manager
#define AVAILABLE_UNITS_FOR_FUSA	(1066U)		// The available time units for FUSA
#define	ADC_PERIOD_UNITS		(10U)		// The timing period of adc test
#define	RAM_PERIOD_UNITS		(2U)		// The timing period of ram test
#define	ROM_PERIOD_UNITS		(3U)		// The timing period of rom test
#define	MAX_CPU_SLICES_PER_UNIT (3u) 			// The maximum single CPU tests per tick (subject to budget)
#define	ADC_OFFSET_UNITS		(1U)		// The adc offset to avoid timing colisions
#define	RAM_OFFSET_UNITS		(2U)		// The ram offset to avoid ram collisions
#define	ROM_OFFSET_UNITS		(4U)		// The ROM offset to avoid rom collisions
#define MARGIN_BUDGET			(20U)		// The persentage of the margin


// Modes for CPU tests.
typedef enum
{
    CPU_TEST_MODE_PART = 0,   // run all tests assigned to part_index (existing behavior)
    CPU_TEST_MODE_SLICE       // run exactly one test per call (internal cursor)
} cpu_test_mode_t;

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

/* ================= FuSa Test Manager ================= */

typedef enum
{
    FUSA_MGR_TEST_NONE = 0,
    FUSA_MGR_TEST_CPU_SLICE,
    FUSA_MGR_TEST_RAM_AUTO,
    FUSA_MGR_TEST_ROM_AUTO,
    FUSA_MGR_TEST_ADC,
} fusa_mgr_test_t;

typedef enum
{
    FUSA_MGR_POLICY_CPU_FIRST = 0,     // prioritize CPU when no heavy test due
    FUSA_MGR_POLICY_MEMORY_FIRST       // prioritize RAM/ROM coverage
} fusa_mgr_policy_t;

typedef struct
{
     uint32_t budget_units;                // per manager call (your MTU4.TCNT units)
     uint16_t adc_period_ticks;            // 0 disables
     uint16_t ram_period_ticks;            // 0 disables
     uint16_t rom_period_ticks;            // 0 disables

     uint16_t adc_offset_ticks;
     uint16_t ram_offset_ticks;
     uint16_t rom_offset_ticks;
     uint16_t margin_budget;              // the margin of between the budget

    uint8_t  max_cpu_slices_per_tick;     // how many single CPU tests per tick (subject to budget)
    fusa_mgr_policy_t policy;

    // ADC configuration
    st_adc_driver adc_driver;

    // Time source (must be monotonic, same units as budget)
    uint32_t (*get_time_units)(void);
} fusa_mgr_cfg_t;

typedef struct
{
    /* ===== Scheduler state (persistent) ===== */

    uint32_t tick;                          /* increments every FuSa_Manager_Run() call */

    adc_voltage_level_t next_adc_level;     /* ADC level to use next time ADC test runs */


    /* ===== Per-tick report (valid only for the most recent invocation) ===== */

    fusa_mgr_test_t last_heavy_test;        /* NONE / ADC / RAM_AUTO / ROM_AUTO */
    uint8_t         last_cpu_slices;        /* number of CPU SLICE executions in this tick */
    uint32_t        last_total_cost_units;  /* total elapsed units for the whole manager call */
    uint8_t         over_budget;            /* 1 if last_total_cost_units > budget_units, else 0 */


    /* ===== Persistent “ever” info (optional but recommended for debugging/audits) ===== */

    fusa_mgr_test_t last_heavy_test_ever;   /* most recent heavy test that ever ran */
    uint32_t        last_heavy_test_tick;   /* tick number when last_heavy_test_ever occurred */

} fusa_mgr_state_t;

extern fusa_mgr_cfg_t g_cfg;
extern fusa_mgr_state_t g_st;

/* Initialize manager state (call once at boot, after basic MCU init) */
void FuSa_Manager_Init(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st);

/* Call periodically (e.g., in safety task). Runs tests within budget. */
void FuSa_Manager_Run(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st);

/* Initialaze the FuSa manager and run all the tests for FuSa at the StartUp */
void FuSa_Startup_FullSelfTest_Init_manager(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st);



extern void		safety_CPU_test(cpu_test_mode_t mode, uint32_t part_index);	 /* Test CPU for safety */
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
