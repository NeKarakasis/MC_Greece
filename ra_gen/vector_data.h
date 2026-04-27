/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (16)
        #endif
        /* ISR prototypes */
        void adc_b_limclpi_isr(void);
        void adc_b_err0_isr(void);
        void adc_b_err1_isr(void);
        void adc_b_resovf0_isr(void);
        void adc_b_resovf1_isr(void);
        void adc_b_calend0_isr(void);
        void adc_b_calend1_isr(void);
        void adc_b_adi0_isr(void);
        void adc_b_fifoovf_isr(void);
        void adc_b_fiforeq0_isr(void);
        void adc_b_fiforeq1_isr(void);
        void adc_b_fiforeq2_isr(void);
        void adc_b_fiforeq3_isr(void);
        void adc_b_fiforeq4_isr(void);
        void agt_int_isr(void);
        void poeg_event_isr(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_ADC_LIMCLPI ((IRQn_Type) 0) /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
        #define ADC_LIMCLPI_IRQn          ((IRQn_Type) 0) /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
        #define VECTOR_NUMBER_ADC_ERR0 ((IRQn_Type) 1) /* ADC ERR0 (A/D converter unit 0 Error) */
        #define ADC_ERR0_IRQn          ((IRQn_Type) 1) /* ADC ERR0 (A/D converter unit 0 Error) */
        #define VECTOR_NUMBER_ADC_ERR1 ((IRQn_Type) 2) /* ADC ERR1 (A/D converter unit 1 Error) */
        #define ADC_ERR1_IRQn          ((IRQn_Type) 2) /* ADC ERR1 (A/D converter unit 1 Error) */
        #define VECTOR_NUMBER_ADC_RESOVF0 ((IRQn_Type) 3) /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
        #define ADC_RESOVF0_IRQn          ((IRQn_Type) 3) /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
        #define VECTOR_NUMBER_ADC_RESOVF1 ((IRQn_Type) 4) /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
        #define ADC_RESOVF1_IRQn          ((IRQn_Type) 4) /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
        #define VECTOR_NUMBER_ADC_CALEND0 ((IRQn_Type) 5) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
        #define ADC_CALEND0_IRQn          ((IRQn_Type) 5) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
        #define VECTOR_NUMBER_ADC_CALEND1 ((IRQn_Type) 6) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
        #define ADC_CALEND1_IRQn          ((IRQn_Type) 6) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
        #define VECTOR_NUMBER_ADC_ADI0 ((IRQn_Type) 7) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
        #define ADC_ADI0_IRQn          ((IRQn_Type) 7) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
        #define VECTOR_NUMBER_ADC_FIFOOVF ((IRQn_Type) 8) /* ADC FIFOOVF (FIFO data overflow) */
        #define ADC_FIFOOVF_IRQn          ((IRQn_Type) 8) /* ADC FIFOOVF (FIFO data overflow) */
        #define VECTOR_NUMBER_ADC_FIFOREQ0 ((IRQn_Type) 9) /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
        #define ADC_FIFOREQ0_IRQn          ((IRQn_Type) 9) /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ1 ((IRQn_Type) 10) /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
        #define ADC_FIFOREQ1_IRQn          ((IRQn_Type) 10) /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ2 ((IRQn_Type) 11) /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
        #define ADC_FIFOREQ2_IRQn          ((IRQn_Type) 11) /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ3 ((IRQn_Type) 12) /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
        #define ADC_FIFOREQ3_IRQn          ((IRQn_Type) 12) /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ4 ((IRQn_Type) 13) /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
        #define ADC_FIFOREQ4_IRQn          ((IRQn_Type) 13) /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
        #define VECTOR_NUMBER_AGT0_INT ((IRQn_Type) 14) /* AGT0 INT (AGT interrupt) */
        #define AGT0_INT_IRQn          ((IRQn_Type) 14) /* AGT0 INT (AGT interrupt) */
        #define VECTOR_NUMBER_POEG3_EVENT ((IRQn_Type) 15) /* POEG3 EVENT (Port Output disable 3 interrupt) */
        #define POEG3_EVENT_IRQn          ((IRQn_Type) 15) /* POEG3 EVENT (Port Output disable 3 interrupt) */
        /* The number of entries required for the ICU vector table. */
        #define BSP_ICU_VECTOR_NUM_ENTRIES (16)

        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */