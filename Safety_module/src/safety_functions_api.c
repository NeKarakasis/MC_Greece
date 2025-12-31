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
void 	safety_CPU_test(cpu_test_mode_t mode, uint32_t part_index);										/* Test CPU for safety */
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

fusa_mgr_cfg_t g_cfg;
fusa_mgr_state_t g_st;

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
int32_t duration_cpu_sf_test[CPU_DIAG_SIZE + 1] = {0,0,0,0,0,0,0,0,0,0,0,0};

/***********************************************************************************************************************
* Function Name : safety_CPU_test
* Description   : Executes CPU self-diagnostic tests and records durations. The groups is defined in the
* configuration api file.
* Arguments     : the group of the tests to be tested
* Return Value  : None
***********************************************************************************************************************/
void safety_CPU_test(cpu_test_mode_t mode, uint32_t part_index)
{
    const uint32_t forceFail = 1U;
    static uint32_t slice_i = 0U;   // persists across calls for SLICE mode

    if (CPU_DIAG_TABLE_SIZE == 0U)
    {
        SafetyErrorHandler(SAFETY_CPU_ERROR);
        return;
    }

    if (mode == CPU_TEST_MODE_SLICE)
    {
        /* Optional strictness: enforce part_index == 0 to avoid misuse */
        if (part_index != 0U)
        {
            SafetyErrorHandler(SAFETY_CPU_ERROR);
            return;
        }

        if (slice_i >= CPU_DIAG_TABLE_SIZE)
        {
            slice_i = 0U;
        }

        int32_t result = 0;

        MTU.TRWERA.BIT.RWE = 1U;
        int32_t start_time = MTU4.TCNT;

        R_CPU_Diag(CPU_index_Table[slice_i], forceFail, &result);

        if (result != 1)
        {
            SafetyErrorHandler(SAFETY_CPU_ERROR);
        }

        duration_cpu_sf_test[CPU_index_Table[slice_i]] =start_time -  MTU4.TCNT ;
        MTU.TRWERA.BIT.RWE = 0U;

        slice_i++;
        return;
    }

    /* Default / PART mode: existing behavior */
    if (mode != CPU_TEST_MODE_PART)
    {
        SafetyErrorHandler(SAFETY_CPU_ERROR);
        return;
    }

    if (part_index >= TOTAL_CPU_TEST_PARTS)
    {
        SafetyErrorHandler(SAFETY_CPU_ERROR);
        return;
    }

    /* Ceiling division as you already do */
    uint32_t items_per_part = (CPU_DIAG_TABLE_SIZE + TOTAL_CPU_TEST_PARTS - 1U) / TOTAL_CPU_TEST_PARTS;

    uint32_t start_index = part_index * items_per_part;
    uint32_t end_index = start_index + items_per_part;
    if (end_index > CPU_DIAG_TABLE_SIZE)
    {
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
            SafetyErrorHandler(SAFETY_CPU_ERROR);
        }

        duration_cpu_sf_test[CPU_index_Table[i]] = MTU4.TCNT - start_time;
        MTU.TRWERA.BIT.RWE = 0U;
    }
}


void safety_CPU_test_old(uint32_t part_index)
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
    MTU.TRWERA.BIT.RWE = 1U;
    int32_t mtu_counter = MTU4.TCNT;
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

    R_RAM_Diag(area, index, destructive);

    if ((RramResult1 != RAM_PASS) || (RramResult2 != RAM_PASS))
    {
        SafetyErrorHandler(SAFETY_RAM_ERROR);
    }

    if (mode == RAM_TEST_MODE_AUTO)
	{
	auto_block_index = auto_block_index + 1;
	}
    if (index < 10)
    {
        duration_ram_sf_test[index] = MTU4.TCNT - mtu_counter;
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

static uint8_t due_with_offset(uint32_t tick, uint16_t period, uint16_t offset)
{
    if (period == 0U) return 0U;
    return (uint8_t)(((tick + (uint32_t)offset) % (uint32_t)period) == 0U);
}

uint32_t GetTicksFun(void)
{
	uint32_t tick_units;
	MTU.TRWERA.BIT.RWE = 1U;
	tick_units = MTU4.TCNT;
	MTU.TRWERA.BIT.RWE = 0U;
	return tick_units;
}

static uint16_t mod_diff_u16(uint16_t a, uint16_t b, uint16_t m)
{
    /* returns (a-b) mod m, safe for unsigned */
    if (m == 0U) return 0U;
    return (uint16_t)((a >= b) ? ((a - b) % m) : ((m - ((b - a) % m)) % m));
}

/* Returns 1 if L is permanently blocked by H, else 0 */
static uint8_t permanently_blocked(uint16_t pL, uint16_t oL, uint16_t pH, uint16_t oH)
{
    if ((pL == 0U) || (pH == 0U)) return 0U;          /* disabled tests cannot starve */
    if ((pL % pH) != 0U) return 0U;                   /* not a subset */
    if (mod_diff_u16(oL, oH, pH) != 0U) return 0U;    /* phase mismatch */
    return 1U;
}


void FuSa_Manager_Init(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st)
{
    if ((cfg == 0) || (st == 0))
    {
        SafetyErrorHandler(SAFETY_GENERIC_CONFIGURATION_ERROR); // or a generic config error
        return;
    }
    cfg->budget_units = AVAILABLE_UNITS_FOR_FUSA;
    cfg->adc_period_ticks = ADC_PERIOD_UNITS;
    cfg->ram_period_ticks = RAM_PERIOD_UNITS;
    cfg->rom_period_ticks = ROM_PERIOD_UNITS;
    cfg->adc_offset_ticks = ADC_OFFSET_UNITS;
    cfg->ram_offset_ticks = RAM_OFFSET_UNITS;
    cfg->rom_offset_ticks = ROM_OFFSET_UNITS;
    cfg->max_cpu_slices_per_tick = MAX_CPU_SLICES_PER_UNIT;
    cfg->adc_driver.ReInitialazationADC = R_Config_S12AD0_Create;
    cfg->adc_driver.StartADC = R_Config_S12AD0_Start;
    cfg->get_time_units = GetTicksFun;
    cfg->margin_budget = MARGIN_BUDGET;

    st->tick = 0U;
    st->next_adc_level = ADC_VOLTAGE_LEVEL_0;
    st->last_heavy_test = FUSA_MGR_TEST_NONE;
    st->last_total_cost_units = 0U;
    st->last_cpu_slices = 0U;
    st->over_budget = 0U;

    /* Priority: RAM > ROM > ADC, A TESTING IF THERE IS PERMANENT COLLISION BASED OF THE CONFIGURATION OF PERIOD AND OFFSETS */

    if (permanently_blocked(cfg->rom_period_ticks, cfg->rom_offset_ticks,
                            cfg->ram_period_ticks, cfg->ram_offset_ticks))
    {
        SafetyErrorHandler(SAFETY_GENERIC_CONFIGURATION_ERROR);
    }

    if (permanently_blocked(cfg->adc_period_ticks, cfg->adc_offset_ticks,
                            cfg->ram_period_ticks, cfg->ram_offset_ticks) ||
        permanently_blocked(cfg->adc_period_ticks, cfg->adc_offset_ticks,
                            cfg->rom_period_ticks, cfg->rom_offset_ticks))
    {
        SafetyErrorHandler(SAFETY_GENERIC_CONFIGURATION_ERROR);
    }

}

//fusa_mgr_test_t debugTestTable[5];
uint32_t DebugUsedTicks[5];
fusa_mgr_test_t DebugTable[10];
uint8_t debug_index = 0;
uint32_t debug_duty[10];
void FuSa_Manager_Run(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st)
{
	uint32_t margin_ticks;
	margin_ticks = (uint32_t)(cfg->budget_units*cfg->margin_budget/100);
    if ((cfg == 0) || (st == 0) || (cfg->get_time_units == 0))
    {
        SafetyErrorHandler(SAFETY_GENERIC_CONFIGURATION_ERROR);
        return;
    }
    st->tick++;
    st->last_heavy_test = FUSA_MGR_TEST_NONE;
    st->last_cpu_slices = 0U;
    st->over_budget = 0U;

    uint32_t t0 = cfg->get_time_units();
    uint32_t used;

    /* Decide ONE heavy test per tick (if due) */
    uint8_t did_heavy = 0U;
    if (due_with_offset(st->tick, cfg->adc_period_ticks, cfg->adc_offset_ticks))
    {
        st_adc_driver drv = cfg->adc_driver;
        drv.typeOfTest = st->next_adc_level;

        adc_test_sample(drv);

        /* rotate ADC level */
        if (st->next_adc_level == ADC_VOLTAGE_LEVEL_0)
        	{
        		st->next_adc_level = ADC_VOLTAGE_LEVEL_HALF;
        	}
        else if (st->next_adc_level == ADC_VOLTAGE_LEVEL_HALF)
        	{
        		st->next_adc_level = ADC_VOLTAGE_LEVEL_FULL;
        	}
        else
        	{
        	st->next_adc_level = ADC_VOLTAGE_LEVEL_0;
        	}

        did_heavy = 1U;
        st->last_heavy_test = FUSA_MGR_TEST_ADC;
    }
    else if (due_with_offset(st->tick, cfg->ram_period_ticks, cfg->ram_offset_ticks))
    {
        ram_test_sample(RAM_TEST_MODE_AUTO, 0U);
        did_heavy = 1U;
        st->last_heavy_test = FUSA_MGR_TEST_RAM_AUTO;
    }
    else if (due_with_offset(st->tick, cfg->rom_period_ticks, cfg->rom_offset_ticks))
    {
        rom_test_sample(ROM_TEST_MODE_AUTO, 0U, 0U);
        did_heavy = 1U;
        st->last_heavy_test = FUSA_MGR_TEST_ROM_AUTO;
    }

    /* Update used time AFTER heavy test */
    used = (uint32_t)(t0 - cfg->get_time_units()); // the timer is on down count
    switch (st->last_heavy_test)
    {
    case FUSA_MGR_TEST_ADC:
    	DebugUsedTicks[0] = used;
    	break;
    case FUSA_MGR_TEST_RAM_AUTO:
    	DebugUsedTicks[1] = used;
    	break;
    case FUSA_MGR_TEST_ROM_AUTO:
    	DebugUsedTicks[2] = used;
    	break;
    }
    if (used >= cfg->budget_units)
    {
        st->last_total_cost_units = used;   /* total, not just heavy */
        st->over_budget = 1U;
        return;
    }

    /* CPU slices: conservative if a heavy test ran */
    uint8_t cpu_limit = cfg->max_cpu_slices_per_tick;
    if (did_heavy && cpu_limit > 1U)
    {
        cpu_limit = 1U; /* or 0U if you want maximum conservatism after ADC */
    }

    for (uint8_t i = 0U; i < cpu_limit; i++)
    {
    	used = (uint32_t)(t0 - cfg->get_time_units());
        if (used >= cfg->budget_units - margin_ticks)
        {
            break;
        }
        safety_CPU_test(CPU_TEST_MODE_SLICE, 0U);
        st->last_cpu_slices++;
    }

    /* FINAL: total cost for the whole manager call */
    st->last_total_cost_units = (uint32_t)(t0 - cfg->get_time_units());

    if (st->last_total_cost_units > cfg->budget_units + margin_ticks)
    {
        st->over_budget = 1U;
    		DebugTable[debug_index] = st->last_heavy_test;
    		debug_duty[debug_index] = st->last_total_cost_units ;
    		if (debug_index > 9)
    			debug_index = 0;
    		else
    			debug_index ++;
    }
}

void FuSa_Startup_FullSelfTest_Init_manager(fusa_mgr_cfg_t* cfg, fusa_mgr_state_t* st)
{
    if ((cfg == 0) || (st == 0))
    {
        SafetyErrorHandler(SAFETY_GENERIC_CONFIGURATION_ERROR);
        return;
    }

    /* 1) Init manager (reads cfg, initializes st and underlying monitors) */
    FuSa_Manager_Init(cfg, st);
    /* One-time init of monitors (your existing API calls) */
    rom_test_init();

    /* 2) Run full tests (CPU/RAM/ROM/ADC) ... */
    for (uint32_t p = 0U; p < (uint32_t)TOTAL_CPU_TEST_PARTS; p++)
    {
        safety_CPU_test(CPU_TEST_MODE_PART, p);
    }

    st_adc_driver drv = cfg->adc_driver;
    drv.typeOfTest = ADC_VOLTAGE_LEVEL_0;
    adc_test_sample(drv);
    drv.typeOfTest = ADC_VOLTAGE_LEVEL_HALF;
    adc_test_sample(drv);
    drv.typeOfTest = ADC_VOLTAGE_LEVEL_FULL;
    adc_test_sample(drv);

    for (uint32_t i = 0U; i < (uint32_t)RAM_TEST_NUM_BLOCKS; i++)
    {
        ram_test_sample(RAM_TEST_MODE_AUTO, 0U);
    }

    for (uint32_t i = 0U; i < (uint32_t)(ROM_NUM_BLOCKS * (ROM_BLOCK_SIZE / ROM_TEST_CHUNK_SIZE)); i++)
    {
        rom_test_sample(ROM_TEST_MODE_AUTO, 0U, 0U);
    }
    /* 3) Initialaze all the rest MCU tests ... */
    FuSa_PC_init();
    FuSa_Voltage_init();
    FuSa_clock_monitor();
    IWDT_Reset_out_chk();
}

