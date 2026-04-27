/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_poeg.h"
#include "r_poeg_api.h"
#include "rm_motor_trajectory_linear.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "rm_motor_sensor_bemf.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_gpt_three_phase.h"
            #include "r_three_phase_api.h"
#include "r_adc_b.h"
                      #include "r_adc_api.h"
#include "rm_motor_hal_driver.h"
#include "rm_motor_pm_foc.h"
#include "rm_motor_control.h"
FSP_HEADER
/** POEG Instance. */
extern const poeg_instance_t g_poeg0;

/** Access the POEG instance using these structures when calling API functions directly (::p_api is not used). */
extern poeg_instance_ctrl_t g_poeg0_ctrl;
extern const poeg_cfg_t g_poeg0_cfg;

#ifndef g_poe_overcurrent
void g_poe_overcurrent(poeg_callback_args_t * p_args);
#endif
/* Trajectory Linear Ramp Instance */
        
        extern const motor_trajectory_instance_t g_motor0_trajectory_linear;
        extern motor_trajectory_linear_ctrl_t g_motor0_trajectory_linear_ctrl;
        extern const motor_trajectory_cfg_t g_motor0_trajectory_linear_cfg;
        #ifndef NULL
        void NULL(void * p_args);
        #endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer3;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer3_ctrl;
extern const timer_cfg_t g_timer3_cfg;

#ifndef rm_motor_outer_cyclic
void rm_motor_outer_cyclic(timer_callback_args_t * p_args);
#endif
/** Motor Sensor BEMF Instance */
        extern const motor_sensor_instance_t g_motor0_sensor_bemf;

        /** Access the Motor Sensor BEMF instance using these structures
            when calling API functions directly (::p_api is not used). */
        extern motor_sensor_bemf_instance_ctrl_t g_motor0_sensor_bemf_ctrl;
        extern const motor_sensor_bemf_extended_cfg_t g_motor0_sensor_bemf_extend;
        extern const motor_sensor_cfg_t g_motor0_sensor_bemf_cfg;

        /* Validator references: MOTOR_MCSENSOR_OPENLOOP_DAMPING_ENABLE MOTOR_MCSENSOR_LESS_SWITCH_ENABLE 256 500 */
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer2_ctrl;
extern const timer_cfg_t g_timer2_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer1_ctrl;
extern const timer_cfg_t g_timer1_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
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

                    #ifndef NULL
                    void NULL(adc_callback_args_t * p_args);
                    #endif
/** Motor Hal Driver Instance. */
extern const motor_hal_driver_instance_t g_motor0_hal_driver;

/** Access the Motor Hal instance using these structures
 ** when calling API functions directly (::p_api is not used). */
extern motor_hal_driver_instance_ctrl_t g_motor0_hal_driver_ctrl;
extern const motor_hal_driver_extended_cfg_t g_motor0_hal_driver_extend;
extern const motor_hal_driver_cfg_t g_motor0_hal_driver_cfg;

#ifndef rm_motor_inner_cyclic
void rm_motor_inner_cyclic(motor_hal_driver_callback_args_t * p_args);
#endif
/* rm_motor_pm_foc */
/* rm_foc inner */
extern motor_pm_foc_inner_instance_ctrl_t g_motor0_pm_foc_inner_ctrl;
extern const motor_pm_foc_inner_cfg_t g_motor0_pm_foc_inner_cfg;

/* rm_foc outer*/
extern motor_pm_foc_outer_instance_ctrl_t g_motor0_pm_foc_outer_ctrl;
extern const motor_pm_foc_outer_cfg_t g_motor0_pm_foc_outer_cfg;

/* rm_foc */;
extern motor_pm_foc_instance_ctrl_t  g_motor0_pm_foc_ctrl;
extern const motor_pm_foc_extended_cfg_t g_motor0_pm_foc_extend;
extern const motor_algorithm_cfg_t  g_motor0_pm_foc_cfg;
extern const motor_algorithm_instance_t  g_motor0_pm_foc;
extern motor_shared_buffer_foc_t g_motor0_pm_foc_shared_data;

#ifndef rm_motor_control_callback
void rm_motor_control_callback(motor_algorithm_callback_args_t *p_args);
#endif
/** Access the motor sensorless control instance using these structures
 when calling API functions directly (::p_api is not used). */
extern motor_control_instance_ctrl_t g_motor0_control_ctrl;
extern const motor_cfg_t g_motor0_control_cfg;
extern const motor_control_extended_cfg_t g_motor0_control_extended_cfg;

#ifndef mtr_callback_event
void mtr_callback_event(motor_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
