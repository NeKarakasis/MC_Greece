/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = gpt_counter_overflow_isr, /* GPT4 COUNTER OVERFLOW (Overflow) */
            [1] = gpt_counter_underflow_isr, /* GPT4 COUNTER UNDERFLOW (Underflow) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_GPT4_COUNTER_OVERFLOW,GROUP0), /* GPT4 COUNTER OVERFLOW (Overflow) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_GPT4_COUNTER_UNDERFLOW,GROUP1), /* GPT4 COUNTER UNDERFLOW (Underflow) */
        };
        #endif
        #endif
