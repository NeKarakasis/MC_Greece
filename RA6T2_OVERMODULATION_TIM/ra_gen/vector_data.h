/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (2)
#endif
/* ISR prototypes */
void gpt_counter_overflow_isr(void);
void gpt_counter_underflow_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_GPT4_COUNTER_OVERFLOW ((IRQn_Type) 0) /* GPT4 COUNTER OVERFLOW (Overflow) */
#define GPT4_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 0) /* GPT4 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_GPT4_COUNTER_UNDERFLOW ((IRQn_Type) 1) /* GPT4 COUNTER UNDERFLOW (Underflow) */
#define GPT4_COUNTER_UNDERFLOW_IRQn          ((IRQn_Type) 1) /* GPT4 COUNTER UNDERFLOW (Underflow) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (2)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
