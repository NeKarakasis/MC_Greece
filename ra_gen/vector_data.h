/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (8)
#endif
/* ISR prototypes */
void poeg_event_isr(void);
void agt_int_isr(void);
void adc_b_calend0_isr(void);
void adc_b_calend1_isr(void);
void adc_b_adi0_isr(void);
void adc_b_adi2_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_POEG0_EVENT ((IRQn_Type) 0) /* POEG0 EVENT (Port Output disable 0 interrupt) */
#define POEG0_EVENT_IRQn          ((IRQn_Type) 0) /* POEG0 EVENT (Port Output disable 0 interrupt) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type) 1) /* AGT0 INT (AGT interrupt) */
#define AGT0_INT_IRQn          ((IRQn_Type) 1) /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_ADC_CALEND0 ((IRQn_Type) 2) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
#define ADC_CALEND0_IRQn          ((IRQn_Type) 2) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
#define VECTOR_NUMBER_ADC_CALEND1 ((IRQn_Type) 3) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
#define ADC_CALEND1_IRQn          ((IRQn_Type) 3) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
#define VECTOR_NUMBER_ADC_ADI0 ((IRQn_Type) 4) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
#define ADC_ADI0_IRQn          ((IRQn_Type) 4) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
#define VECTOR_NUMBER_ADC_ADI2 ((IRQn_Type) 5) /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
#define ADC_ADI2_IRQn          ((IRQn_Type) 5) /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
#define VECTOR_NUMBER_POEG1_EVENT ((IRQn_Type) 6) /* POEG1 EVENT (Port Output disable 1 interrupt) */
#define POEG1_EVENT_IRQn          ((IRQn_Type) 6) /* POEG1 EVENT (Port Output disable 1 interrupt) */
#define VECTOR_NUMBER_AGT1_INT ((IRQn_Type) 7) /* AGT1 INT (AGT interrupt) */
#define AGT1_INT_IRQn          ((IRQn_Type) 7) /* AGT1 INT (AGT interrupt) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (8)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
