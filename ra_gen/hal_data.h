/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "rm_motor_estimate.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_gpt_three_phase.h"
#include "r_three_phase_api.h"
#include "rm_motor_driver.h"
#include "rm_motor_current.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "rm_motor_speed.h"
#include "rm_motor_sensorless.h"
#include "r_poeg.h"
#include "r_poeg_api.h"
FSP_HEADER
/** Motor Angle Estimation Instance */
extern const motor_angle_instance_t g_motor_angle1;

/** Access the Motor Angle Estimation instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_estimate_instance_ctrl_t g_motor_angle1_ctrl;
extern const motor_angle_cfg_t g_motor_angle1_cfg;
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer4;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer4_ctrl;
extern const timer_cfg_t g_timer4_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer5;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer5_ctrl;
extern const timer_cfg_t g_timer5_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer3;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer3_ctrl;
extern const timer_cfg_t g_timer3_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** GPT Three-Phase Instance. */
extern const three_phase_instance_t g_three_phase1;

/** Access the GPT Three-Phase instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_three_phase_instance_ctrl_t g_three_phase1_ctrl;
extern const three_phase_cfg_t g_three_phase1_cfg;
/** Motor Driver Instance. */
extern const motor_driver_instance_t g_motor_driver1;

/** Access the Motor Driver instance using these structures
 ** when calling API functions directly (::p_api is not used). */
extern motor_driver_instance_ctrl_t g_motor_driver1_ctrl;
extern const motor_driver_extended_cfg_t g_motor_driver1_extend;
extern const motor_driver_cfg_t g_motor_driver1_cfg;

#ifndef rm_motor_current_cyclic
void rm_motor_current_cyclic(motor_driver_callback_args_t *p_args);
#endif
/** Motor Current Instance. */
extern const motor_current_instance_t g_motor_current1;

/** Access the Motor Current instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_current_instance_ctrl_t g_motor_current1_ctrl;
extern const motor_current_extended_cfg_t g_motor_current1_extend;
extern const motor_current_cfg_t g_motor_current1_cfg;

#ifndef rm_motor_sensorless_current_callback
void rm_motor_sensorless_current_callback(motor_current_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer7;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer7_ctrl;
extern const timer_cfg_t g_timer7_cfg;

#ifndef rm_motor_speed_cyclic
void rm_motor_speed_cyclic(timer_callback_args_t *p_args);
#endif
/** Motor Speed Instance. */
extern const motor_speed_instance_t g_motor_speed1;

/** Access the Motor Speed instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_speed_instance_ctrl_t g_motor_speed1_ctrl;
extern const motor_speed_extended_cfg_t g_motor_speed1_extend;
extern const motor_speed_cfg_t g_motor_speed1_cfg;

#ifndef rm_motor_sensorless_speed_callback
void rm_motor_sensorless_speed_callback(motor_speed_callback_args_t *p_args);
#endif
/** Motor sensorless vector control instance. */
extern const motor_instance_t g_motor_sensorless1;

/** Access the motor sensorless control instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_sensorless_instance_ctrl_t g_motor_sensorless1_ctrl;
extern const motor_sensorless_extended_cfg_t g_motor_sensorless1_extend;
extern const motor_cfg_t g_motor_sensorless1_cfg;

#ifndef mtr_callback_event1
void mtr_callback_event1(motor_callback_args_t *p_args);
#endif
/** POEG Instance. */
extern const poeg_instance_t g_poeg1;

/** Access the POEG instance using these structures when calling API functions directly (::p_api is not used). */
extern poeg_instance_ctrl_t g_poeg1_ctrl;
extern const poeg_cfg_t g_poeg1_cfg;

#ifndef g_poe_overcurrent1
void g_poe_overcurrent1(poeg_callback_args_t *p_args);
#endif
/** Motor Angle Estimation Instance */
extern const motor_angle_instance_t g_motor_angle0;

/** Access the Motor Angle Estimation instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_estimate_instance_ctrl_t g_motor_angle0_ctrl;
extern const motor_angle_cfg_t g_motor_angle0_cfg;
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer1_ctrl;
extern const timer_cfg_t g_timer1_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer2_ctrl;
extern const timer_cfg_t g_timer2_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
/** GPT Three-Phase Instance. */
extern const three_phase_instance_t g_three_phase0;

/** Access the GPT Three-Phase instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_three_phase_instance_ctrl_t g_three_phase0_ctrl;
extern const three_phase_cfg_t g_three_phase0_cfg;
/** Motor Driver Instance. */
extern const motor_driver_instance_t g_motor_driver0;

/** Access the Motor Driver instance using these structures
 ** when calling API functions directly (::p_api is not used). */
extern motor_driver_instance_ctrl_t g_motor_driver0_ctrl;
extern const motor_driver_extended_cfg_t g_motor_driver0_extend;
extern const motor_driver_cfg_t g_motor_driver0_cfg;

#ifndef rm_motor_current_cyclic
void rm_motor_current_cyclic(motor_driver_callback_args_t *p_args);
#endif
/** Motor Current Instance. */
extern const motor_current_instance_t g_motor_current0;

/** Access the Motor Current instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_current_instance_ctrl_t g_motor_current0_ctrl;
extern const motor_current_extended_cfg_t g_motor_current0_extend;
extern const motor_current_cfg_t g_motor_current0_cfg;

#ifndef rm_motor_sensorless_current_callback
void rm_motor_sensorless_current_callback(motor_current_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer6;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer6_ctrl;
extern const timer_cfg_t g_timer6_cfg;

#ifndef rm_motor_speed_cyclic
void rm_motor_speed_cyclic(timer_callback_args_t *p_args);
#endif
/** Motor Speed Instance. */
extern const motor_speed_instance_t g_motor_speed0;

/** Access the Motor Speed instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_speed_instance_ctrl_t g_motor_speed0_ctrl;
extern const motor_speed_extended_cfg_t g_motor_speed0_extend;
extern const motor_speed_cfg_t g_motor_speed0_cfg;

#ifndef rm_motor_sensorless_speed_callback
void rm_motor_sensorless_speed_callback(motor_speed_callback_args_t *p_args);
#endif
/** Motor sensorless vector control instance. */
extern const motor_instance_t g_motor_sensorless0;

/** Access the motor sensorless control instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_sensorless_instance_ctrl_t g_motor_sensorless0_ctrl;
extern const motor_sensorless_extended_cfg_t g_motor_sensorless0_extend;
extern const motor_cfg_t g_motor_sensorless0_cfg;

#ifndef mtr_callback_event0
void mtr_callback_event0(motor_callback_args_t *p_args);
#endif
/** POEG Instance. */
extern const poeg_instance_t g_poeg0;

/** Access the POEG instance using these structures when calling API functions directly (::p_api is not used). */
extern poeg_instance_ctrl_t g_poeg0_ctrl;
extern const poeg_cfg_t g_poeg0_cfg;

#ifndef g_poe_overcurrent0
void g_poe_overcurrent0(poeg_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
