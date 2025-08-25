#include <cfg/safety_functions_api.h>
#include <safety_error_codes.h>
#include "r_smc_entry.h"
#include "safety_functions_api.h"
#include "r_cpu_diag.h"
#include "r_ram_diag.h"
#include "r_adc_diag.h"
#include "r_rom_diag.h"
#include "r_clock_mon.h"
#include "r_vol_mon.h"
#include "r_pc_mon.h"



// the interrupt functions based of the RX13T interrupt vector.
#pragma interrupt r_Config_CAC_OSCILATION_interrupt(vect = VECT_CAC_FERRF)
#pragma interrupt r_Config_CAC_OVERFLOW_interrupt(vect = VECT_CAC_OVFF)
#pragma interrupt r_Config_LVD_LVD1_interrupt(vect = VECT_LVD_LVD1)


// FuSa functions, those functions will be called in the code when it is necessery
void	safety_CPU_test(uint32_t part_index);											/* Test CPU for safety */
void 	ram_test_sample(ram_test_mode_t mode, uint32_t block_index);					/* Test RAM for safety */
void	rom_test_sample(rom_test_mode_t mode, uint32_t manual_start, uint32_t manual_end); 		/* Test ROM for safety */													/* Test RAM for safety */
void 	adc_test_sample(st_adc_driver voltage);	/* The ADC for safety */
void 	FuSa_clock_monitor(void);				/*Initiate and start the clock monitor*/
void 	IWDT_Reset_out_chk(void);				/*Check after reset if it came from PC fault*/
void	FuSa_PC_init(void);						/*Initiate the PC monitor peripheral*/
void	FuSa_Voltage_init(void);				/*Initiate the Voltage monitor peripheral*/
void 	FuSa_PC_refresh(void);					/*Refresh the watchdog timer*/
void 	rom_test_init(void);


// variables for PC test depending of the HW
static const uint32_t CPU_index_Table[CPU_DIAG_TABLE_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12 };


/***********************************************************************************************************************
* Function Name : SafetyErrorHandler
* Description   : Handles safety errors by entering an infinite loop.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void SafetyErrorHandler(SafetyErrorCode error_code)
{
	SafetyErrorCode error = error_code;
	while(1);
}
int32_t duration_cpu_sf_test[CPU_DIAG_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0};

/***********************************************************************************************************************
* Function Name : safety_CPU_test
* Description   : Executes CPU self-diagnostic tests and records durations. The groups is defined in the
* configuration api file.
* Arguments     : the group of the tests to be tested
* Return Value  : None
***********************************************************************************************************************/
void safety_CPU_test(uint32_t part_index)
{
    const uint32_t forceFail = 1;

    if (part_index >= TOTAL_CPU_TEST_PARTS || CPU_DIAG_TABLE_SIZE == 0)
    {
        SafetyErrorHandler(SAFETY_CPU_ERROR); // Invalid input
        return;
    }

    // Ceiling division
    uint32_t items_per_part = (CPU_DIAG_TABLE_SIZE + TOTAL_CPU_TEST_PARTS - 1) / TOTAL_CPU_TEST_PARTS;

    // Determine range for this part
    uint32_t start_index = part_index * items_per_part;
    uint32_t end_index = start_index + items_per_part;
    if (end_index > CPU_DIAG_TABLE_SIZE) {
        end_index = CPU_DIAG_TABLE_SIZE;
    }

    for (uint32_t i = start_index; i < end_index; i++)
    {
        int32_t result = 0;
        MTU.TRWERA.BIT.RWE = 1U;
        int32_t start_time = MTU4.TCNT;

        R_CPU_Diag(CPU_index_Table[i], forceFail, &result);

        if (result != 1)
        {
            SafetyErrorHandler(SAFETY_CPU_ERROR); // Handle failure
        }

        duration_cpu_sf_test[CPU_index_Table[i]] = MTU4.TCNT - start_time;
        MTU.TRWERA.BIT.RWE = 0U;
    }
}

/***********************************************************************************************************************
* Function Name : ram_test_sample
* Description   : Executes RAM diagnostic tests and records durations.
* Arguments     : ram test mode= automatic or manual, block index for manual mode
* Return Value  : None
***********************************************************************************************************************/
int32_t duration_ram_sf_test[10] = {0,0,0,0,0,0,0,0};

void ram_test_sample(ram_test_mode_t mode, uint32_t block_index)
{
    static uint32_t auto_block_index = 0;  // Persistent across calls
    uint32_t index = 0;

    if (mode == RAM_TEST_MODE_AUTO)
    {
        if (auto_block_index >= RAM_TEST_NUM_BLOCKS)
        {
            auto_block_index = 0; // Wrap around
        }
        index = auto_block_index;
    }
    else if (mode == RAM_TEST_MODE_MANUAL)
    {
        if (block_index >= RAM_TEST_NUM_BLOCKS)
        {
            SafetyErrorHandler(SAFETY_RAM_ERROR); // Invalid block
            return;
        }
        index = block_index;
    }
    else
    {
        SafetyErrorHandler(SAFETY_RAM_ERROR); // Invalid mode
        return;
    }

    uint32_t area = 0;// RAM_TEST_START_ADDRESS + index * RAM_TEST_BLOCK_SIZE;
    uint32_t destructive = (index == 0) ? RAM_MEM_DT : RAM_MEM_NDT;

    MTU.TRWERA.BIT.RWE = 1U;
    int32_t mtu_counter = MTU4.TCNT;

    R_RAM_Diag(area, index, destructive);

    if ((RramResult1 != RAM_PASS) || (RramResult2 != RAM_PASS))
    {
        SafetyErrorHandler(SAFETY_RAM_ERROR);
    }

    if (index < 10)
    {
        duration_ram_sf_test[index] = MTU4.TCNT - mtu_counter;
    }
    if (mode == RAM_TEST_MODE_AUTO)
	{
	auto_block_index = auto_block_index + 1;
	}

    MTU.TRWERA.BIT.RWE = 0U;
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
* Arguments     : input the voltage test
* Return Value  : None
***********************************************************************************************************************/
void adc_test_sample(st_adc_driver voltage)
{

    uint8_t unit = voltage.voltageUnit;
    uint8_t status = 0;
    uint16_t value;

    adc_init_sample();  // Ensure initialization before each test

    value = R_ADC_Diag(unit, (uint8_t)voltage.typeOfTest); // Call API
    status = (value & 0xC000) >> 14;   // Extract self-diagnosis status
    value = value & 0x0FF0;            // Mask to get the valid bits

    switch (status)
    {
        case 1U: // 0V
            if (value != 0x000)
            {
                SafetyErrorHandler(SAFETY_ADC_ERROR);
            }
            break;

        case 2U: // 1/2 Vref
            if ((value != 0x800) && (value != 0x7F0))
            {
                SafetyErrorHandler(SAFETY_ADC_ERROR);
            }
            break;

        case 3U: // Full Vref
            if (value != 0xFF0)
            {
                SafetyErrorHandler(SAFETY_ADC_ERROR);
            }
            break;

        default:
            SafetyErrorHandler(SAFETY_ADC_ERROR);
            break;
    }
    voltage.ReInitialazationADC();
    voltage.StartADC();
}

void rom_test_init(void)
{
    R_ROM_Diag_Init();
}

uint16_t calChecksum1 = 0;
uint16_t calChecksum2 = 0;
uint16_t calChecksum3 = 0;
int16_t duration_rom_sf_test[6]={0,0,0,0,0,0};
uint16_t *pExpChecksum = (uint16_t*)CHECKSUM_START_ADDRESS;
uint32_t startAddress[4];
uint32_t endAddress[4];
uint8_t tempMode[4];
uint8_t count_tests = 0;
uint16_t crc_temp[3] = {0,0,0};
uint16_t accumulated_crc = 0U;
uint16_t chunk_crc = 0U;
volatile uint16_t expected_crc = 0;

/***********************************************************************************************************************
* Function Name : rom_test_sample
* Description   : Performs ROM checksum diagnostics and validates results.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void rom_test_sample(rom_test_mode_t mode, uint32_t manual_start, uint32_t manual_end)
{
    static uint32_t current_block = 0U;
    static uint32_t current_chunk_offset = 0U;
    static uint16_t crc_manual = 0U;

    uint32_t start_addr = 0U;
    uint32_t end_addr = 0U;
    uint32_t mode_flag = 0U;


    if (mode == ROM_TEST_MODE_MANUAL)
    {
        start_addr = manual_start;
        end_addr = manual_end;

        /* Determine mode_flag based on block boundary */
        if (((start_addr - ROM_START_ADDR) % ROM_BLOCK_SIZE) == 0U)
        {
            mode_flag = 0U;
        }
        else
        {
            mode_flag = 1U;
        }

        chunk_crc = R_ROM_Diag(start_addr, end_addr, mode_flag);
        crc_manual = chunk_crc;

        if (start_addr == end_addr)
        {
            uint32_t index;

            if ((start_addr >= ROM_START_ADDR) &&
                ((start_addr - ROM_START_ADDR) < ROM_TOTAL_SIZE))
            {
                index = (start_addr - ROM_START_ADDR) / ROM_BLOCK_SIZE;

                if (index < ROM_NUM_BLOCKS)
                {
                    expected_crc = pExpChecksum[index];

                    if (crc_manual != expected_crc)
                    {
                        SafetyErrorHandler(SAFETY_ROM_ERROR);
                    }
                }
            }

            crc_manual = 0U;
        }
    }
    else
    {
        /* AUTO MODE */
        start_addr = ROM_START_ADDR + (current_block * ROM_BLOCK_SIZE) + current_chunk_offset;
        end_addr   = start_addr + ROM_TEST_CHUNK_SIZE - 1U;
        //startAddress[0] = start_addr;
        //endAddress[0] = end_addr;

        if (current_chunk_offset == 0U)
        {
            mode_flag = 0U;
        }
        else
        {
            mode_flag = 1U;
        }
        chunk_crc = R_ROM_Diag(start_addr, end_addr, mode_flag);
        accumulated_crc = chunk_crc;
        if (count_tests<3)
		{
			startAddress[count_tests] = start_addr;
			endAddress[count_tests] = end_addr;
	        tempMode[count_tests] =mode_flag;
		}
        current_chunk_offset += ROM_TEST_CHUNK_SIZE;
        count_tests++;

        if (current_chunk_offset >= ROM_BLOCK_SIZE)
        {
            expected_crc = pExpChecksum[current_block];

			startAddress[3] = start_addr;
			endAddress[3] = end_addr;
	        tempMode[3] =mode_flag;

            crc_temp[0] = expected_crc;
            crc_temp[1] = chunk_crc;
            if (chunk_crc != expected_crc)
            {
                SafetyErrorHandler(SAFETY_ROM_ERROR);
            }

            current_block++;
            if (current_block >= ROM_NUM_BLOCKS)
            {
                current_block = 0U;
            }

            current_chunk_offset = 0U;
            accumulated_crc = 0U;
            count_tests = 0;

        }
    }
}


/***********************************************************************************************************************

* Function Name : FuSa_clock_monitor
* Description   : Initializes and starts the clock monitor with specific settings.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void FuSa_clock_monitor(void)
{
/* Measurement target clocks: main, Frequency of measurement target clock: 12 MHz, Tolerance limit: 16 % */
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
	SafetyErrorHandler(SAFETY_CLOCK_ERROR);
}

/***********************************************************************************************************************
* Function Name : r_Config_CAC_OVERFLOW_interrupt
* Description   : Callback for clock monitor overflow interrupt.
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_Config_CAC_OVERFLOW_interrupt()
{
	SafetyErrorHandler(SAFETY_CLOCK_ERROR);
}

/***********************************************************************************************************************
* Function Name : r_Config_LVD_LVD1_interrupt
* Description   : Callback for voltage detection interrupt (LVD1).
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void r_Config_LVD_LVD1_interrupt()
{
	SafetyErrorHandler(SAFETY_VOLTAGE_ERROR);

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
		SafetyErrorHandler(SAFETY_PC_ERROR);
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

void FuSa_PC_refresh(void)
{
	R_PC_Mon_Refresh();
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
