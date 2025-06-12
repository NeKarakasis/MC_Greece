/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_poeg.h"
#include "r_poeg_api.h"
#include "r_gpt_three_phase.h"
#include "r_three_phase_api.h"
#include "r_adc_b.h"
#include "r_adc_api.h"
FSP_HEADER
/** AGT Timer Instance */
extern const timer_instance_t g_agt1;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_agt1_ctrl;
extern const timer_cfg_t g_agt1_cfg;

#ifndef callback_agt_system_manager_cyclic
void callback_agt_system_manager_cyclic(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer_gpt1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer_gpt1_ctrl;
extern const timer_cfg_t g_timer_gpt1_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_agt0;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_agt0_ctrl;
extern const timer_cfg_t g_agt0_cfg;

#ifndef callback_agt_motor_speed_cyclic
void callback_agt_motor_speed_cyclic(timer_callback_args_t *p_args);
#endif
/** POEG Instance. */
extern const poeg_instance_t g_poeg3;

/** Access the POEG instance using these structures when calling API functions directly (::p_api is not used). */
extern poeg_instance_ctrl_t g_poeg3_ctrl;
extern const poeg_cfg_t g_poeg3_cfg;

#ifndef callback_poe_overcurrent
void callback_poe_overcurrent(poeg_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer_gpt6;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer_gpt6_ctrl;
extern const timer_cfg_t g_timer_gpt6_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer_gpt5;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer_gpt5_ctrl;
extern const timer_cfg_t g_timer_gpt5_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer_gpt4;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer_gpt4_ctrl;
extern const timer_cfg_t g_timer_gpt4_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** GPT Three-Phase Instance. */
extern const three_phase_instance_t g_three_phase0;

/** Access the GPT Three-Phase instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_three_phase_instance_ctrl_t g_three_phase0_ctrl;
extern const three_phase_cfg_t g_three_phase0_cfg;
/** ADC on ADC_B instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC_B instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_b_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_b_scan_cfg_t g_adc0_scan_cfg;

#ifndef callback_gpt_adc_cyclic
void callback_gpt_adc_cyclic(adc_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
