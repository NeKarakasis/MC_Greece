/* generated HAL source file - do not edit */
#include "hal_data.h"

motor_estimate_instance_ctrl_t g_motor_angle1_ctrl;
const motor_estimate_extended_cfg_t g_motor_angle1_extend =
{ .openloop_damping = MOTOR_ESTIMATE_OPENLOOP_DAMPING_ENABLE,
  .f_e_obs_omega = 1000.0F,
  .f_e_obs_zeta = 1.0F,
  .f_pll_est_omega = 20.0F,
  .f_pll_est_zeta = 1.0F,
  .f4_ctrl_period = 0.00005F,
  .st_motor_params.u2_mtr_pp = 4,
  .st_motor_params.f4_mtr_r = 1.3F,
  .st_motor_params.f4_mtr_ld = 0.0013F,
  .st_motor_params.f4_mtr_lq = 0.0013F,
  .st_motor_params.f4_mtr_m = 0.01119F,
  .st_motor_params.f4_mtr_j = 0.000003666F,
  .st_motor_params.f4_mtr_nominal_current = 1.67F, };

const motor_angle_cfg_t g_motor_angle1_cfg =
{ .p_context = NULL, .p_extend = &g_motor_angle1_extend, };

/* Instance structure to use Motor Angle module. */
const motor_angle_instance_t g_motor_angle1 =
{ .p_ctrl = &g_motor_angle1_ctrl, .p_cfg = &g_motor_angle1_cfg, .p_api = &g_motor_angle_on_motor_estimate };
gpt_instance_ctrl_t g_timer4_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer4_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT4_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT4_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG1,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
#endif
const gpt_extended_cfg_t g_timer4_extend =
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
#if defined(VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT4_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer4_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer4_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 4, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer4_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT4_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT4_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer4 =
{ .p_ctrl = &g_timer4_ctrl, .p_cfg = &g_timer4_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer5_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer5_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG1,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
#endif
const gpt_extended_cfg_t g_timer5_extend =
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
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer5_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer5_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 6, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer5_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer5 =
{ .p_ctrl = &g_timer5_ctrl, .p_cfg = &g_timer5_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer3_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer3_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG1,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_DOWN_COUNT_START_ADC_A | GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
#endif
const gpt_extended_cfg_t g_timer3_extend =
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
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer3_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer3_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 5, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer3_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer3 =
{ .p_ctrl = &g_timer3_ctrl, .p_cfg = &g_timer3_cfg, .p_api = &g_timer_on_gpt };
gpt_three_phase_instance_ctrl_t g_three_phase1_ctrl;
const three_phase_cfg_t g_three_phase1_cfg =
{ .buffer_mode = (THREE_PHASE_BUFFER_MODE_SINGLE), .p_timer_instance =
{ &g_timer3, &g_timer5, &g_timer4 },
  .callback_ch = THREE_PHASE_CHANNEL_U, .channel_mask = (1 << 5) | (1 << 6) | (1 << 4), .p_context = NULL, .p_extend =
          NULL, };
/* Instance structure to use this module. */
const three_phase_instance_t g_three_phase1 =
{ .p_ctrl = &g_three_phase1_ctrl, .p_cfg = &g_three_phase1_cfg, .p_api = &g_gpt_three_phase_on_gpt_three_phase };
/* Motor Driver Configuration */
motor_driver_instance_ctrl_t g_motor_driver1_ctrl;

const motor_driver_extended_cfg_t g_motor_driver1_extend =
{ .u2_pwm_timer_freq = 250,
  .pwm_carrier_freq = 1.0F / (50 / 1000.0F),
  .u2_deadtime = 500,
  .f_current_range = 16.5F,
  .f_vdc_range = 73.51F,
  .f_ad_resolution = 0xFFF,
  .f_ad_current_offset = 0x7FF,
  .f_ad_voltage_conversion = 1.0F,
  .u2_offset_calc_count = 500,
  .modulation_method = MOTOR_DRIVER_MODULATION_METHOD_SVPWM,
  .port_up = (bsp_io_port_pin_t) BSP_IO_PORT_02_PIN_03,
  .port_un = (bsp_io_port_pin_t) BSP_IO_PORT_02_PIN_02,
  .port_vp = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_01,
  .port_vn = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_00,
  .port_wp = (bsp_io_port_pin_t) BSP_IO_PORT_02_PIN_05,
  .port_wn = (bsp_io_port_pin_t) BSP_IO_PORT_02_PIN_04,
  .f_ad_current_adjust = 20.0F,
  .s4_difference_minimum = 300,
  .s4_adjust_adc_delay = 240,
  .trigger_phase = MOTOR_DRIVER_PHASE_U_PHASE,
  .adc_group = (adc_group_mask_t) 2,
  .mod_param.f4_vdc = 24.0F,
  .mod_param.f4_1_div_vdc = 1.0F / 24.0F,
  .mod_param.f4_voltage_error_ratio = ((float) 500 / (float) 250) / (float) 50,
  .mod_param.f4_max_duty = 0.9375F,
  .mod_param.f4_min_duty = ((float) 500 / (float) 250) / (float) 50,
  .mod_param.f4_neutral_duty = 0.5F,
  .mod_param.u1_sat_flag = 0U,

  .iu_ad_unit = 0,
  .iv_ad_unit = 0,
  .iw_ad_unit = 0,
  .vdc_ad_unit = 0,
  .sin_ad_unit = 0,
  .cos_ad_unit = 0,

  .interrupt_adc = MOTOR_DRIVER_SELECT_ADC_INSTANCE_FIRST,

#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_motor_adc_shared0)
  .p_shared_cfg = NULL,
#else
  .p_shared_cfg = &g_motor_adc_shared0_extend_cfg,
#endif
#undef RA_NOT_DEFINED

        };

const motor_driver_cfg_t g_motor_driver1_cfg =
{
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_adc_instance = NULL,
#else
  .p_adc_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .iu_ad_ch = (adc_channel_t) 2,
  .iv_ad_ch = (adc_channel_t) 0, .iw_ad_ch = (adc_channel_t) 4, .vdc_ad_ch = (adc_channel_t) 18, .sin_ad_ch =
          (adc_channel_t) 27,
  .cos_ad_ch = (adc_channel_t) 28,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_adc2_instance = NULL,
#else
  .p_adc2_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .shunt = MOTOR_DRIVER_SHUNT_TYPE_2_SHUNT,
  .p_three_phase_instance = &g_three_phase1, .p_callback = rm_motor_current_cyclic, .p_context =
          (void*) &g_motor_current1,
  .p_extend = &g_motor_driver1_extend, };

/* Instance structure to use Motor Driver module. */
const motor_driver_instance_t g_motor_driver1 =
{ .p_ctrl = &g_motor_driver1_ctrl, .p_cfg = &g_motor_driver1_cfg, .p_api = &g_motor_driver_on_motor_driver, };
/* Motor Current Configuration */
motor_current_instance_ctrl_t g_motor_current1_ctrl;

motor_current_design_parameter_t g_motor_current1_design =
{ .f_current_omega = 300.0F, .f_current_zeta = 1.0F, };

motor_current_motor_parameter_t g_motor_current1_motor =
{ .u2_mtr_pp = 4, .f4_mtr_r = 1.3F, .f4_mtr_ld = 0.0013F, .f4_mtr_lq = 0.0013F, .f4_mtr_m = 0.01119F, .f4_mtr_j =
          0.000003666F, };

const motor_current_extended_cfg_t g_motor_current1_extend =
{ .f_comp_v[0] = 0.477F,
  .f_comp_v[1] = 0.742F,
  .f_comp_v[2] = 0.892F,
  .f_comp_v[3] = 0.979F,
  .f_comp_v[4] = 1.009F,
  .f_comp_i[0] = 0.021F,
  .f_comp_i[1] = 0.034F,
  .f_comp_i[2] = 0.064F,
  .f_comp_i[3] = 0.158F,
  .f_comp_i[4] = 0.400F,
  .vcomp_enable = MOTOR_CURRENT_VOLTAGE_COMPENSATION_SELECT_ENABLE,
  .f_current_ctrl_period = (0 + 1.0F) / 20.0F,
  .f_ilimit = 24.0F / 2.0F,
  .p_motor_parameter = &g_motor_current1_motor,
  .p_design_parameter = &g_motor_current1_design,
  .u1_control_type = MOTOR_CURRENT_CONTROL_TYPE_SENSORLESS,
  .shunt = MOTOR_CURRENT_SHUNT_TYPE_2_SHUNT,
  .u1_sample_delay_comp_enable = MOTOR_CURRENT_SAMPLE_DELAY_COMPENSATION_ENABLE,
  .f_period_magnitude_value = 1.5F, };

const motor_current_cfg_t g_motor_current1_cfg =
{ .p_motor_driver_instance = &g_motor_driver1, .p_motor_angle_instance = &g_motor_angle1, .p_callback =
          rm_motor_sensorless_current_callback,
  .p_context = (void*) &g_motor_sensorless1, .p_extend = &g_motor_current1_extend, };

/* Instance structure to use motor current control module. */
const motor_current_instance_t g_motor_current1 =
{ .p_ctrl = &g_motor_current1_ctrl, .p_cfg = &g_motor_current1_cfg, .p_api = &g_motor_current_on_motor_current, };
agt_instance_ctrl_t g_timer7_ctrl;
const agt_extended_cfg_t g_timer7_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING,
  .counter_bit_width = AGT_COUNTER_BIT_WIDTH_16, };
const timer_cfg_t g_timer7_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.0005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x7a12,
  .duty_cycle_counts = 0x3d09, .source_div = (timer_source_div_t) 0, .channel = 1, .p_callback = rm_motor_speed_cyclic,
  /** If NULL then do not add & */
#if defined(g_motor_speed1)
    .p_context           = g_motor_speed1,
#else
  .p_context = (void*) &g_motor_speed1,
#endif
  .p_extend = &g_timer7_extend,
  .cycle_end_ipl = (10),
#if defined(VECTOR_NUMBER_AGT1_INT)
    .cycle_end_irq       = VECTOR_NUMBER_AGT1_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer7 =
{ .p_ctrl = &g_timer7_ctrl, .p_cfg = &g_timer7_cfg, .p_api = &g_timer_on_agt };
/* Motor Speed Configuration */
motor_speed_instance_ctrl_t g_motor_speed1_ctrl;

const motor_speed_extended_cfg_t g_motor_speed1_extend =
{ .f_speed_ctrl_period = 0.0005F,
  .f_limit_speed_change = 0.5F * 4,
  .f_maximum_speed_rpm = 2400.0F * 4,
  .f_omega_t = 10.0F,
  .f_iq_limit = 1.67F,
  .f_ol_fb_speed_limit_rate = 0.2F,
  .f_natural_frequency = 100.0F,
  .u1_openloop_damping = MOTOR_SPEED_OPENLOOP_DAMPING_ENABLE,
  .u1_flux_weakening = MOTOR_SPEED_FLUX_WEAKEN_DISABLE,
  .u1_less_switch = MOTOR_SPEED_LESS_SWITCH_ENABLE,
  .u1_observer_swtich = MOTOR_SPEED_OBSERVER_SWITCH_DISABLE,
  .observer_select = MOTOR_SPEED_OBSERVER_SELECT_NORMAL,
  .u1_ctrl_method = MOTOR_SPEED_METHOD_PID,
  .ol_param.f4_ol_id_up_step = 0.3F / 256.0F,
  .ol_param.f4_ol_id_down_step = 0.3F / 50.0F,
  .ol_param.f4_ol_iq_down_step_ratio = 1.0F,
  .ol_param.f4_ol_id_ref = 0.3F,
  .ol_param.f4_id_down_speed_rpm = 500.0F * 4,
  .ol_param.f4_id_up_speed_rpm = 400.0F * 4,
  .ol_param.f4_opl2less_sw_time = 0.025F,
  .ol_param.f4_switch_phase_err_rad = 10.0F,
  .ol_sub_param.f_damp_comp_gain = 0.0F,
  .ol_sub_param.f_fb_speed_limit_rate = 0.0F,
  .d_param.f_speed_omega = 3.0F,
  .d_param.f_speed_zeta = 1.0F,
  .d_param.f_ed_hpf_omega = 2.5F,
  .d_param.f_ol_damping_zeta = 1.0F,
  .d_param.f_phase_err_lpf_cut_freq = 10.0F,
  .d_param.f_observer_omega = 200.0F,
  .d_param.f_observer_zeta = 1.0F,
  .control_type = MOTOR_SPEED_CONTROL_TYPE_SENSORLESS,
  .mtr_param.u2_mtr_pp = 4,
  .mtr_param.f4_mtr_r = 1.3F,
  .mtr_param.f4_mtr_ld = 0.0013F,
  .mtr_param.f4_mtr_lq = 0.0013F,
  .mtr_param.f4_mtr_m = 0.01119F,
  .mtr_param.f4_mtr_j = 0.000003666F, };

const motor_speed_cfg_t g_motor_speed1_cfg =
{ .st_input = &(g_motor_sensorless1_ctrl.st_speed_input),
  .st_output = &(g_motor_sensorless1_ctrl.st_speed_output),
  .p_callback = rm_motor_sensorless_speed_callback,
  .p_context = (void*) &g_motor_sensorless1,
  .p_timer_instance = &g_timer7,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_position_instance = NULL,
#else
  .p_position_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .p_extend = &g_motor_speed1_extend, };

/* Instance structure to use motor speed module. */
const motor_speed_instance_t g_motor_speed1 =
{ .p_ctrl = &g_motor_speed1_ctrl, .p_cfg = &g_motor_speed1_cfg, .p_api = &g_motor_speed_on_motor_speed, };
/* Motor sensorless control configuration */
motor_sensorless_instance_ctrl_t g_motor_sensorless1_ctrl;

const motor_sensorless_extended_cfg_t g_motor_sensorless1_extend =
{ .f_overcurrent_limit = 1.67F * 1.414213562F * 1.5F,
  .f_overvoltage_limit = 60.0F,
  .f_overspeed_limit = 4500.0F,
  .f_lowvoltage_limit = 8.0F, };

const motor_cfg_t g_motor_sensorless1_cfg =
{ .p_callback = mtr_callback_event1,
  .p_context = (void*) NULL,
  .p_motor_speed_instance = &g_motor_speed1,
  .p_motor_current_instance = &g_motor_current1,
  .p_extend = &g_motor_sensorless1_extend, };

/* Instance structure to use motor sensorless control module. */
const motor_instance_t g_motor_sensorless1 =
{ .p_ctrl = &g_motor_sensorless1_ctrl, .p_cfg = &g_motor_sensorless1_cfg, .p_api = &g_motor_on_sensorless, };
poeg_instance_ctrl_t g_poeg1_ctrl;
const poeg_cfg_t g_poeg1_cfg =
{ .trigger = (poeg_trigger_t) (POEG_TRIGGER_PIN | POEG_TRIGGER_SOFTWARE),
  .polarity = POEG_GTETRG_POLARITY_ACTIVE_LOW,
  .noise_filter = POEG_GTETRG_NOISE_FILTER_CLK_SOURCE_DIV_32,
  .channel = 1,
  .ipl = (0),
  .p_callback = g_poe_overcurrent1,
  .p_context = NULL,
#if defined(VECTOR_NUMBER_POEG1_EVENT)
    .irq       = VECTOR_NUMBER_POEG1_EVENT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const poeg_instance_t g_poeg1 =
{ .p_ctrl = &g_poeg1_ctrl, .p_cfg = &g_poeg1_cfg, .p_api = &g_poeg_on_poeg };
motor_estimate_instance_ctrl_t g_motor_angle0_ctrl;
const motor_estimate_extended_cfg_t g_motor_angle0_extend =
{ .openloop_damping = MOTOR_ESTIMATE_OPENLOOP_DAMPING_ENABLE,
  .f_e_obs_omega = 1000.0F,
  .f_e_obs_zeta = 1.0F,
  .f_pll_est_omega = 20.0F,
  .f_pll_est_zeta = 1.0F,
  .f4_ctrl_period = 0.00005F,
  .st_motor_params.u2_mtr_pp = 4,
  .st_motor_params.f4_mtr_r = 1.3F,
  .st_motor_params.f4_mtr_ld = 0.0013F,
  .st_motor_params.f4_mtr_lq = 0.0013F,
  .st_motor_params.f4_mtr_m = 0.01119F,
  .st_motor_params.f4_mtr_j = 0.000003666F,
  .st_motor_params.f4_mtr_nominal_current = 1.67F, };

const motor_angle_cfg_t g_motor_angle0_cfg =
{ .p_context = NULL, .p_extend = &g_motor_angle0_extend, };

/* Instance structure to use Motor Angle module. */
const motor_angle_instance_t g_motor_angle0 =
{ .p_ctrl = &g_motor_angle0_ctrl, .p_cfg = &g_motor_angle0_cfg, .p_api = &g_motor_angle_on_motor_estimate };
gpt_instance_ctrl_t g_timer1_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer1_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT9_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT9_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
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
#if defined(VECTOR_NUMBER_GPT9_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT9_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT9_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT9_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer1_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer1_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 9, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer1_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT9_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT9_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer1 =
{ .p_ctrl = &g_timer1_ctrl, .p_cfg = &g_timer1_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer2_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer2_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT7_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT7_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
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
#if defined(VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer2_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer2_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 7, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer2_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT7_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT7_COUNTER_OVERFLOW,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer2 =
{ .p_ctrl = &g_timer2_ctrl, .p_cfg = &g_timer2_cfg, .p_api = &g_timer_on_gpt };
gpt_instance_ctrl_t g_timer0_ctrl;
#if 1
const gpt_extended_pwm_cfg_t g_timer0_pwm_extend =
{ .trough_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT8_COUNTER_UNDERFLOW,
#else
  .trough_irq = FSP_INVALID_VECTOR,
#endif
  .poeg_link = GPT_POEG_LINK_POEG0,
  .output_disable = (gpt_output_disable_t) (GPT_OUTPUT_DISABLE_NONE),
  .adc_trigger = (gpt_adc_trigger_t) (GPT_ADC_TRIGGER_DOWN_COUNT_START_ADC_A | GPT_ADC_TRIGGER_NONE),
  .dead_time_count_up = 500,
  .dead_time_count_down = 500,
  .adc_a_compare_match = 0,
  .adc_b_compare_match = 0,
  .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
  .interrupt_skip_count = GPT_INTERRUPT_SKIP_COUNT_0,
  .interrupt_skip_adc = GPT_INTERRUPT_SKIP_ADC_NONE,
  .gtioca_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z,
  .gtiocb_disable_setting = GPT_GTIOC_DISABLE_SET_HI_Z, };
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
#if defined(VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_A,
#else
          .capture_a_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_B,
#else
          .capture_b_irq = FSP_INVALID_VECTOR,
#endif
          .compare_match_value =
          { /* CMP_A */(uint32_t) 0x0, /* CMP_B */(uint32_t) 0x0 },
          .compare_match_status = (0U << 1U) | 0U, .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE, .capture_filter_gtiocb =
                  GPT_CAPTURE_FILTER_NONE,
#if 1
          .p_pwm_cfg = &g_timer0_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_HIGH,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_SET_HI_Z,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
          .gtior_setting.gtior = 0U,
#endif

          .gtioca_polarity = GPT_GTIOC_POLARITY_NORMAL,
          .gtiocb_polarity = GPT_GTIOC_POLARITY_NORMAL, };

const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM,
/* Actual period: 0.00005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x186a,
  .duty_cycle_counts = 0xc35, .source_div = (timer_source_div_t) 0, .channel = 8, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer0_extend,
  .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT8_COUNTER_OVERFLOW,
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
{ &g_timer0, &g_timer2, &g_timer1 },
  .callback_ch = THREE_PHASE_CHANNEL_U, .channel_mask = (1 << 8) | (1 << 7) | (1 << 9), .p_context = NULL, .p_extend =
          NULL, };
/* Instance structure to use this module. */
const three_phase_instance_t g_three_phase0 =
{ .p_ctrl = &g_three_phase0_ctrl, .p_cfg = &g_three_phase0_cfg, .p_api = &g_gpt_three_phase_on_gpt_three_phase };
/* Motor Driver Configuration */
motor_driver_instance_ctrl_t g_motor_driver0_ctrl;

const motor_driver_extended_cfg_t g_motor_driver0_extend =
{ .u2_pwm_timer_freq = 250,
  .pwm_carrier_freq = 1.0F / (50 / 1000.0F),
  .u2_deadtime = 500,
  .f_current_range = 16.5F,
  .f_vdc_range = 73.51F,
  .f_ad_resolution = 0xFFF,
  .f_ad_current_offset = 0x7FF,
  .f_ad_voltage_conversion = 1.0F,
  .u2_offset_calc_count = 500,
  .modulation_method = MOTOR_DRIVER_MODULATION_METHOD_SVPWM,
  .port_up = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05,
  .port_un = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_04,
  .port_vp = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03,
  .port_vn = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02,
  .port_wp = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_12,
  .port_wn = (bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_13,
  .f_ad_current_adjust = 20.0F,
  .s4_difference_minimum = 300,
  .s4_adjust_adc_delay = 240,
  .trigger_phase = MOTOR_DRIVER_PHASE_U_PHASE,
  .adc_group = (adc_group_mask_t) 0,
  .mod_param.f4_vdc = 24.0F,
  .mod_param.f4_1_div_vdc = 1.0F / 24.0F,
  .mod_param.f4_voltage_error_ratio = ((float) 500 / (float) 250) / (float) 50,
  .mod_param.f4_max_duty = 0.9375F,
  .mod_param.f4_min_duty = ((float) 500 / (float) 250) / (float) 50,
  .mod_param.f4_neutral_duty = 0.5F,
  .mod_param.u1_sat_flag = 0U,

  .iu_ad_unit = 0,
  .iv_ad_unit = 0,
  .iw_ad_unit = 0,
  .vdc_ad_unit = 0,
  .sin_ad_unit = 0,
  .cos_ad_unit = 0,

  .interrupt_adc = MOTOR_DRIVER_SELECT_ADC_INSTANCE_FIRST,

#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_motor_adc_shared0)
  .p_shared_cfg = NULL,
#else
  .p_shared_cfg = &g_motor_adc_shared0_extend_cfg,
#endif
#undef RA_NOT_DEFINED

        };

const motor_driver_cfg_t g_motor_driver0_cfg =
{
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_adc_instance = NULL,
#else
  .p_adc_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .iu_ad_ch = (adc_channel_t) 6,
  .iv_ad_ch = (adc_channel_t) 8, .iw_ad_ch = (adc_channel_t) 10, .vdc_ad_ch = (adc_channel_t) 7, .sin_ad_ch =
          (adc_channel_t) 20,
  .cos_ad_ch = (adc_channel_t) 19,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_adc2_instance = NULL,
#else
  .p_adc2_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .shunt = MOTOR_DRIVER_SHUNT_TYPE_2_SHUNT,
  .p_three_phase_instance = &g_three_phase0, .p_callback = rm_motor_current_cyclic, .p_context =
          (void*) &g_motor_current0,
  .p_extend = &g_motor_driver0_extend, };

/* Instance structure to use Motor Driver module. */
const motor_driver_instance_t g_motor_driver0 =
{ .p_ctrl = &g_motor_driver0_ctrl, .p_cfg = &g_motor_driver0_cfg, .p_api = &g_motor_driver_on_motor_driver, };
/* Motor Current Configuration */
motor_current_instance_ctrl_t g_motor_current0_ctrl;

motor_current_design_parameter_t g_motor_current0_design =
{ .f_current_omega = 300.0F, .f_current_zeta = 1.0F, };

motor_current_motor_parameter_t g_motor_current0_motor =
{ .u2_mtr_pp = 4, .f4_mtr_r = 1.3F, .f4_mtr_ld = 0.0013F, .f4_mtr_lq = 0.0013F, .f4_mtr_m = 0.01119F, .f4_mtr_j =
          0.000003666F, };

const motor_current_extended_cfg_t g_motor_current0_extend =
{ .f_comp_v[0] = 0.477F,
  .f_comp_v[1] = 0.742F,
  .f_comp_v[2] = 0.892F,
  .f_comp_v[3] = 0.979F,
  .f_comp_v[4] = 1.009F,
  .f_comp_i[0] = 0.021F,
  .f_comp_i[1] = 0.034F,
  .f_comp_i[2] = 0.064F,
  .f_comp_i[3] = 0.158F,
  .f_comp_i[4] = 0.400F,
  .vcomp_enable = MOTOR_CURRENT_VOLTAGE_COMPENSATION_SELECT_ENABLE,
  .f_current_ctrl_period = (0 + 1.0F) / 20.0F,
  .f_ilimit = 24.0F / 2.0F,
  .p_motor_parameter = &g_motor_current0_motor,
  .p_design_parameter = &g_motor_current0_design,
  .u1_control_type = MOTOR_CURRENT_CONTROL_TYPE_SENSORLESS,
  .shunt = MOTOR_CURRENT_SHUNT_TYPE_2_SHUNT,
  .u1_sample_delay_comp_enable = MOTOR_CURRENT_SAMPLE_DELAY_COMPENSATION_ENABLE,
  .f_period_magnitude_value = 1.5F, };

const motor_current_cfg_t g_motor_current0_cfg =
{ .p_motor_driver_instance = &g_motor_driver0, .p_motor_angle_instance = &g_motor_angle0, .p_callback =
          rm_motor_sensorless_current_callback,
  .p_context = (void*) &g_motor_sensorless0, .p_extend = &g_motor_current0_extend, };

/* Instance structure to use motor current control module. */
const motor_current_instance_t g_motor_current0 =
{ .p_ctrl = &g_motor_current0_ctrl, .p_cfg = &g_motor_current0_cfg, .p_api = &g_motor_current_on_motor_current, };
agt_instance_ctrl_t g_timer6_ctrl;
const agt_extended_cfg_t g_timer6_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING,
  .counter_bit_width = AGT_COUNTER_BIT_WIDTH_16, };
const timer_cfg_t g_timer6_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.0005 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x7a12,
  .duty_cycle_counts = 0x3d09, .source_div = (timer_source_div_t) 0, .channel = 0, .p_callback = rm_motor_speed_cyclic,
  /** If NULL then do not add & */
#if defined(g_motor_speed0)
    .p_context           = g_motor_speed0,
#else
  .p_context = (void*) &g_motor_speed0,
#endif
  .p_extend = &g_timer6_extend,
  .cycle_end_ipl = (10),
#if defined(VECTOR_NUMBER_AGT0_INT)
    .cycle_end_irq       = VECTOR_NUMBER_AGT0_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer6 =
{ .p_ctrl = &g_timer6_ctrl, .p_cfg = &g_timer6_cfg, .p_api = &g_timer_on_agt };
/* Motor Speed Configuration */
motor_speed_instance_ctrl_t g_motor_speed0_ctrl;

const motor_speed_extended_cfg_t g_motor_speed0_extend =
{ .f_speed_ctrl_period = 0.0005F,
  .f_limit_speed_change = 0.5F * 4,
  .f_maximum_speed_rpm = 2400.0F * 4,
  .f_omega_t = 10.0F,
  .f_iq_limit = 1.67F,
  .f_ol_fb_speed_limit_rate = 0.2F,
  .f_natural_frequency = 100.0F,
  .u1_openloop_damping = MOTOR_SPEED_OPENLOOP_DAMPING_ENABLE,
  .u1_flux_weakening = MOTOR_SPEED_FLUX_WEAKEN_DISABLE,
  .u1_less_switch = MOTOR_SPEED_LESS_SWITCH_ENABLE,
  .u1_observer_swtich = MOTOR_SPEED_OBSERVER_SWITCH_DISABLE,
  .observer_select = MOTOR_SPEED_OBSERVER_SELECT_NORMAL,
  .u1_ctrl_method = MOTOR_SPEED_METHOD_PID,
  .ol_param.f4_ol_id_up_step = 0.3F / 256.0F,
  .ol_param.f4_ol_id_down_step = 0.3F / 50.0F,
  .ol_param.f4_ol_iq_down_step_ratio = 1.0F,
  .ol_param.f4_ol_id_ref = 0.3F,
  .ol_param.f4_id_down_speed_rpm = 500.0F * 4,
  .ol_param.f4_id_up_speed_rpm = 400.0F * 4,
  .ol_param.f4_opl2less_sw_time = 0.025F,
  .ol_param.f4_switch_phase_err_rad = 10.0F,
  .ol_sub_param.f_damp_comp_gain = 0.0F,
  .ol_sub_param.f_fb_speed_limit_rate = 0.0F,
  .d_param.f_speed_omega = 3.0F,
  .d_param.f_speed_zeta = 1.0F,
  .d_param.f_ed_hpf_omega = 2.5F,
  .d_param.f_ol_damping_zeta = 1.0F,
  .d_param.f_phase_err_lpf_cut_freq = 10.0F,
  .d_param.f_observer_omega = 200.0F,
  .d_param.f_observer_zeta = 1.0F,
  .control_type = MOTOR_SPEED_CONTROL_TYPE_SENSORLESS,
  .mtr_param.u2_mtr_pp = 4,
  .mtr_param.f4_mtr_r = 1.3F,
  .mtr_param.f4_mtr_ld = 0.0013F,
  .mtr_param.f4_mtr_lq = 0.0013F,
  .mtr_param.f4_mtr_m = 0.01119F,
  .mtr_param.f4_mtr_j = 0.000003666F, };

const motor_speed_cfg_t g_motor_speed0_cfg =
{ .st_input = &(g_motor_sensorless0_ctrl.st_speed_input),
  .st_output = &(g_motor_sensorless0_ctrl.st_speed_output),
  .p_callback = rm_motor_sensorless_speed_callback,
  .p_context = (void*) &g_motor_sensorless0,
  .p_timer_instance = &g_timer6,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_position_instance = NULL,
#else
  .p_position_instance = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .p_extend = &g_motor_speed0_extend, };

/* Instance structure to use motor speed module. */
const motor_speed_instance_t g_motor_speed0 =
{ .p_ctrl = &g_motor_speed0_ctrl, .p_cfg = &g_motor_speed0_cfg, .p_api = &g_motor_speed_on_motor_speed, };
/* Motor sensorless control configuration */
motor_sensorless_instance_ctrl_t g_motor_sensorless0_ctrl;

const motor_sensorless_extended_cfg_t g_motor_sensorless0_extend =
{ .f_overcurrent_limit = 1.67F * 1.414213562F * 1.5F,
  .f_overvoltage_limit = 60.0F,
  .f_overspeed_limit = 4500.0F,
  .f_lowvoltage_limit = 8.0F, };

const motor_cfg_t g_motor_sensorless0_cfg =
{ .p_callback = mtr_callback_event0,
  .p_context = (void*) NULL,
  .p_motor_speed_instance = &g_motor_speed0,
  .p_motor_current_instance = &g_motor_current0,
  .p_extend = &g_motor_sensorless0_extend, };

/* Instance structure to use motor sensorless control module. */
const motor_instance_t g_motor_sensorless0 =
{ .p_ctrl = &g_motor_sensorless0_ctrl, .p_cfg = &g_motor_sensorless0_cfg, .p_api = &g_motor_on_sensorless, };
poeg_instance_ctrl_t g_poeg0_ctrl;
const poeg_cfg_t g_poeg0_cfg =
{ .trigger = (poeg_trigger_t) (POEG_TRIGGER_PIN | POEG_TRIGGER_SOFTWARE),
  .polarity = POEG_GTETRG_POLARITY_ACTIVE_LOW,
  .noise_filter = POEG_GTETRG_NOISE_FILTER_CLK_SOURCE_DIV_32,
  .channel = 0,
  .ipl = (0),
  .p_callback = g_poe_overcurrent0,
  .p_context = NULL,
#if defined(VECTOR_NUMBER_POEG0_EVENT)
    .irq       = VECTOR_NUMBER_POEG0_EVENT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const poeg_instance_t g_poeg0 =
{ .p_ctrl = &g_poeg0_ctrl, .p_cfg = &g_poeg0_cfg, .p_api = &g_poeg_on_poeg };
void g_hal_init(void)
{
    g_common_init ();
}
