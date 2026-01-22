/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = adc_b_calend0_isr, /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
            [1] = adc_b_calend1_isr, /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
            [2] = adc_b_adi0_isr, /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
            [3] = adc_b_adi1_isr, /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
            [4] = poeg_event_isr, /* POEG3 EVENT (Port Output disable 3 interrupt) */
            [5] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [6] = r_icu_isr, /* ICU IRQ2 (External pin interrupt 2) */
            [7] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
            [8] = spi_b_rxi_isr, /* SPI1 RXI (Receive buffer full) */
            [9] = spi_b_txi_isr, /* SPI1 TXI (Transmit buffer empty) */
            [10] = spi_b_tei_isr, /* SPI1 TEI (Transmission complete event) */
            [11] = spi_b_eri_isr, /* SPI1 ERI (Error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_ADC_CALEND0,GROUP0), /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_ADC_CALEND1,GROUP1), /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI0,GROUP2), /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI1,GROUP3), /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_POEG3_EVENT,GROUP4), /* POEG3 EVENT (Port Output disable 3 interrupt) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP5), /* AGT0 INT (AGT interrupt) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ2,GROUP6), /* ICU IRQ2 (External pin interrupt 2) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_AGT1_INT,GROUP7), /* AGT1 INT (AGT interrupt) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SPI1_RXI,GROUP0), /* SPI1 RXI (Receive buffer full) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TXI,GROUP1), /* SPI1 TXI (Transmit buffer empty) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TEI,GROUP2), /* SPI1 TEI (Transmission complete event) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SPI1_ERI,GROUP3), /* SPI1 ERI (Error) */
        };
        #endif
        #endif
