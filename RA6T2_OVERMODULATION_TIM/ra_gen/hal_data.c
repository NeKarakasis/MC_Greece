/* generated HAL source file - do not edit */
#include "hal_data.h"

gpt_instance_ctrl_t g_timer2_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer2_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW)
    .trough_irq             = VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 180,
  .dead_time_count_down = 240,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED, };
#endif
const gpt_extended_cfg_t g_timer2_extend =
        { .gtioca =
        { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtiocb =
          { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_HIGH },
          .start_source = (gpt_source_t) (GPT_SOURCE_NONE), .stop_source = (gpt_source_t) (GPT_SOURCE_NONE), .clear_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .count_up_source = (gpt_source_t) (GPT_SOURCE_NONE), .count_down_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .capture_b_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_ipl = (BSP_IRQ_DISABLED), .capture_b_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_c_ipl = (BSP_IRQ_DISABLED), .compare_match_d_ipl = (BSP_IRQ_DISABLED), .compare_match_e_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_f_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A)
    .capture_a_irq         = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B)
    .capture_b_irq         = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_COMPARE_C)
    .compare_match_c_irq   = VECTOR_NUMBER_GPT6_COMPARE_C,
#else
          .compare_match_c_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_COMPARE_D)
    .compare_match_d_irq   = VECTOR_NUMBER_GPT6_COMPARE_D,
#else
          .compare_match_d_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_COMPARE_E)
    .compare_match_e_irq   = VECTOR_NUMBER_GPT6_COMPARE_E,
#else
          .compare_match_e_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_COMPARE_F)
    .compare_match_f_irq   = VECTOR_NUMBER_GPT6_COMPARE_F,
#else
          .compare_match_f_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { (uint32_t) 0x0, /* CMP_A */
            (uint32_t) 0x0, /* CMP_B */
            (uint32_t) 0x0, /* CMP_C */
            (uint32_t) 0x0, /* CMP_D */
            (uint32_t) 0x0, /* CMP_E */
            (uint32_t) 0x0, /* CMP_F */},
          .compare_match_status = ((0U << 5U) | (0U << 4U) | (0U << 3U) | (0U << 2U) | (0U << 1U) | 0U), .capture_filter_gtioca =
                  GPT_CAPTURE_FILTER_NONE,
          .capture_filter_gtiocb = GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer2_pwm_extend,
#else
    .p_pwm_cfg             = NULL,
#endif
#if 1
          .gtior_setting.gtior_b.gtioa = (0U << 4U) | (0U << 2U) | (3U << 0U),
          .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW, .gtior_setting.gtior_b.oahld = 0U, .gtior_setting.gtior_b.oae =
                  (uint32_t) true,
          .gtior_setting.gtior_b.oadf = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED, .gtior_setting.gtior_b.nfaen =
                  ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
          .gtior_setting.gtior_b.nfcsa = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U), .gtior_setting.gtior_b.gtiob = (1U
                  << 4U) | (0U << 2U) | (3U << 0U),
          .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH, .gtior_setting.gtior_b.obhld = 0U, .gtior_setting.gtior_b.obe =
                  (uint32_t) true,
          .gtior_setting.gtior_b.obdf = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED, .gtior_setting.gtior_b.nfben =
                  ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
          .gtior_setting.gtior_b.nfcsb = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer2_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.0001 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x1770,
  .duty_cycle_counts = 0xbb8, .source_div = (timer_source_div_t) 0, .channel = 6, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer2_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer2 =
{ .p_ctrl = &g_timer2_ctrl, .p_cfg = &g_timer2_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer1_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer1_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW)
    .trough_irq             = VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 180,
  .dead_time_count_down = 240,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED, };
#endif
const gpt_extended_cfg_t g_timer1_extend =
        { .gtioca =
        { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtiocb =
          { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_HIGH },
          .start_source = (gpt_source_t) (GPT_SOURCE_NONE), .stop_source = (gpt_source_t) (GPT_SOURCE_NONE), .clear_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .count_up_source = (gpt_source_t) (GPT_SOURCE_NONE), .count_down_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .capture_b_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_ipl = (BSP_IRQ_DISABLED), .capture_b_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_c_ipl = (BSP_IRQ_DISABLED), .compare_match_d_ipl = (BSP_IRQ_DISABLED), .compare_match_e_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_f_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A)
    .capture_a_irq         = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B)
    .capture_b_irq         = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_COMPARE_C)
    .compare_match_c_irq   = VECTOR_NUMBER_GPT5_COMPARE_C,
#else
          .compare_match_c_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_COMPARE_D)
    .compare_match_d_irq   = VECTOR_NUMBER_GPT5_COMPARE_D,
#else
          .compare_match_d_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_COMPARE_E)
    .compare_match_e_irq   = VECTOR_NUMBER_GPT5_COMPARE_E,
#else
          .compare_match_e_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_COMPARE_F)
    .compare_match_f_irq   = VECTOR_NUMBER_GPT5_COMPARE_F,
#else
          .compare_match_f_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { (uint32_t) 0x0, /* CMP_A */
            (uint32_t) 0x0, /* CMP_B */
            (uint32_t) 0x0, /* CMP_C */
            (uint32_t) 0x0, /* CMP_D */
            (uint32_t) 0x0, /* CMP_E */
            (uint32_t) 0x0, /* CMP_F */},
          .compare_match_status = ((0U << 5U) | (0U << 4U) | (0U << 3U) | (0U << 2U) | (0U << 1U) | 0U), .capture_filter_gtioca =
                  GPT_CAPTURE_FILTER_NONE,
          .capture_filter_gtiocb = GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer1_pwm_extend,
#else
    .p_pwm_cfg             = NULL,
#endif
#if 1
          .gtior_setting.gtior_b.gtioa = (0U << 4U) | (0U << 2U) | (3U << 0U),
          .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW, .gtior_setting.gtior_b.oahld = 0U, .gtior_setting.gtior_b.oae =
                  (uint32_t) true,
          .gtior_setting.gtior_b.oadf = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED, .gtior_setting.gtior_b.nfaen =
                  ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
          .gtior_setting.gtior_b.nfcsa = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U), .gtior_setting.gtior_b.gtiob = (1U
                  << 4U) | (0U << 2U) | (3U << 0U),
          .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH, .gtior_setting.gtior_b.obhld = 0U, .gtior_setting.gtior_b.obe =
                  (uint32_t) true,
          .gtior_setting.gtior_b.obdf = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED, .gtior_setting.gtior_b.nfben =
                  ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
          .gtior_setting.gtior_b.nfcsb = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.0001 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x1770,
  .duty_cycle_counts = 0xbb8, .source_div = (timer_source_div_t) 0, .channel = 5, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer1_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer1 =
{ .p_ctrl = &g_timer1_ctrl, .p_cfg = &g_timer1_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer0_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer0_pwm_extend =
{ .trough_ipl = (10),
#if defined(VECTOR_NUMBER_GPT4_COUNTER_UNDERFLOW)
    .trough_irq             = VECTOR_NUMBER_GPT4_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 180,
  .dead_time_count_down = 240,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED, };
#endif
const gpt_extended_cfg_t g_timer0_extend =
        { .gtioca =
        { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
          .gtiocb =
          { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_HIGH },
          .start_source = (gpt_source_t) (GPT_SOURCE_NONE), .stop_source = (gpt_source_t) (GPT_SOURCE_NONE), .clear_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .count_up_source = (gpt_source_t) (GPT_SOURCE_NONE), .count_down_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_source =
                  (gpt_source_t) (GPT_SOURCE_NONE),
          .capture_b_source = (gpt_source_t) (GPT_SOURCE_NONE), .capture_a_ipl = (BSP_IRQ_DISABLED), .capture_b_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_c_ipl = (BSP_IRQ_DISABLED), .compare_match_d_ipl = (BSP_IRQ_DISABLED), .compare_match_e_ipl =
                  (BSP_IRQ_DISABLED),
          .compare_match_f_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_A)
    .capture_a_irq         = VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_B)
    .capture_b_irq         = VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_COMPARE_C)
    .compare_match_c_irq   = VECTOR_NUMBER_GPT4_COMPARE_C,
#else
          .compare_match_c_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_COMPARE_D)
    .compare_match_d_irq   = VECTOR_NUMBER_GPT4_COMPARE_D,
#else
          .compare_match_d_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_COMPARE_E)
    .compare_match_e_irq   = VECTOR_NUMBER_GPT4_COMPARE_E,
#else
          .compare_match_e_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_COMPARE_F)
    .compare_match_f_irq   = VECTOR_NUMBER_GPT4_COMPARE_F,
#else
          .compare_match_f_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { (uint32_t) 0x0, /* CMP_A */
            (uint32_t) 0x0, /* CMP_B */
            (uint32_t) 0x0, /* CMP_C */
            (uint32_t) 0x0, /* CMP_D */
            (uint32_t) 0x0, /* CMP_E */
            (uint32_t) 0x0, /* CMP_F */},
          .compare_match_status = ((0U << 5U) | (0U << 4U) | (0U << 3U) | (0U << 2U) | (0U << 1U) | 0U), .capture_filter_gtioca =
                  GPT_CAPTURE_FILTER_NONE,
          .capture_filter_gtiocb = GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer0_pwm_extend,
#else
    .p_pwm_cfg             = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (3U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (1U << 4U) | (0U << 2U) | (3U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.0001 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x1770,
  .duty_cycle_counts = 0xbb8, .source_div = (timer_source_div_t) 0, .channel = 4, .p_callback = gpt_c_t,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer0_extend,
  .cycle_end_ipl = (10),
#if defined(VECTOR_NUMBER_GPT4_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT4_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_gpt };
gpt_three_phase_instance_ctrl_t g_three_phase0_ctrl;
const three_phase_cfg_t g_three_phase0_cfg =
{ .buffer_mode = (THREE_PHASE_BUFFER_MODE_SINGLE), .p_timer_instance =
{ &g_timer0, &g_timer1, &g_timer2 },
  .callback_ch = THREE_PHASE_CHANNEL_U, .channel_mask = (1 << 4) | (1 << 5) | (1 << 6), .p_context = NULL, .p_extend =
          NULL, };
/* Instance structure to use this module. */
const three_phase_instance_t g_three_phase0 =
{ .p_ctrl = &g_three_phase0_ctrl, .p_cfg = &g_three_phase0_cfg, .p_api = &g_gpt_three_phase_on_gpt_three_phase };
void g_hal_init(void)
{
    g_common_init ();
}
