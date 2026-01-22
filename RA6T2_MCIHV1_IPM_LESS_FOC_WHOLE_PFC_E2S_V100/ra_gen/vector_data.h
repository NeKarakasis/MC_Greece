/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (12)
#endif
/* ISR prototypes */
void adc_b_calend0_isr(void);
void adc_b_calend1_isr(void);
void adc_b_adi0_isr(void);
void adc_b_adi1_isr(void);
void poeg_event_isr(void);
void agt_int_isr(void);
void r_icu_isr(void);
void spi_b_rxi_isr(void);
void spi_b_txi_isr(void);
void spi_b_tei_isr(void);
void spi_b_eri_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_ADC_CALEND0 ((IRQn_Type) 0) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
#define ADC_CALEND0_IRQn          ((IRQn_Type) 0) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
#define VECTOR_NUMBER_ADC_CALEND1 ((IRQn_Type) 1) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
#define ADC_CALEND1_IRQn          ((IRQn_Type) 1) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
#define VECTOR_NUMBER_ADC_ADI0 ((IRQn_Type) 2) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
#define ADC_ADI0_IRQn          ((IRQn_Type) 2) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
#define VECTOR_NUMBER_ADC_ADI1 ((IRQn_Type) 3) /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
#define ADC_ADI1_IRQn          ((IRQn_Type) 3) /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
#define VECTOR_NUMBER_POEG3_EVENT ((IRQn_Type) 4) /* POEG3 EVENT (Port Output disable 3 interrupt) */
#define POEG3_EVENT_IRQn          ((IRQn_Type) 4) /* POEG3 EVENT (Port Output disable 3 interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type) 5) /* AGT0 INT (AGT interrupt) */
#define AGT0_INT_IRQn          ((IRQn_Type) 5) /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ2 ((IRQn_Type) 6) /* ICU IRQ2 (External pin interrupt 2) */
#define ICU_IRQ2_IRQn          ((IRQn_Type) 6) /* ICU IRQ2 (External pin interrupt 2) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type) 7) /* AGT1 INT (AGT interrupt) */
#define AGT1_INT_IRQn          ((IRQn_Type) 7) /* AGT1 INT (AGT interrupt) */
#define VECTOR_NUMBER_SPI1_RXI ((IRQn_Type) 8) /* SPI1 RXI (Receive buffer full) */
#define SPI1_RXI_IRQn          ((IRQn_Type) 8) /* SPI1 RXI (Receive buffer full) */
#define VECTOR_NUMBER_SPI1_TXI ((IRQn_Type) 9) /* SPI1 TXI (Transmit buffer empty) */
#define SPI1_TXI_IRQn          ((IRQn_Type) 9) /* SPI1 TXI (Transmit buffer empty) */
#define VECTOR_NUMBER_SPI1_TEI ((IRQn_Type) 10) /* SPI1 TEI (Transmission complete event) */
#define SPI1_TEI_IRQn          ((IRQn_Type) 10) /* SPI1 TEI (Transmission complete event) */
#define VECTOR_NUMBER_SPI1_ERI ((IRQn_Type) 11) /* SPI1 ERI (Error) */
#define SPI1_ERI_IRQn          ((IRQn_Type) 11) /* SPI1 ERI (Error) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (12)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
