/*
 * safety_cfg.h
 *
 *  Created on: 20 Ιουν 2025
 *      Author: a5155223
 */

#ifndef SAFETY_MODULE_CFG_SAFETY_FUNCTIONS_API_H_
#define SAFETY_MODULE_CFG_SAFETY_CFG_H_

#define CPU_DIAG_SIZE				(12)		/* Number of the size of CPU_DIAG_Table 0-9,11,12*/
#define CHECKSUM_START_ADDRESS (0xFFFFFF10)
#define EVOLTAGE	6							/* The voltage trigger for voltage monitor fault triggering. 6 -> 2.9V*/

extern void		safety_CPU_test(void);					     	/* Test CPU for safety */
extern void 	ram_test_sample(void);					/* Test RAM for safety */
extern void		rom_test_sample(void);					/* Test RAM for safety */
extern void		adc_init_sample(void);					/* Initialaze for test the ADC for safety*/
extern void 	adc_test_sample(void);					/* The ADC for safety */
extern void 	FuSa_clock_monitor(void);				/*Initiate and start the clock monitor*/
extern void 	IWDT_Reset_out_chk(void);				/*Check after reset if it came from PC fault*/

#endif /* SAFETY_MODULE_CFG_SAFETY_FUNCTIONS_API_H_ */
