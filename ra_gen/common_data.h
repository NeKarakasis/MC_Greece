/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_adc_b.h"
#include "r_adc_api.h"
#include "rm_motor_driver.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
/** ADC on ADC_B instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC_B instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_b_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_b_scan_cfg_t g_adc0_scan_cfg;

#ifndef rm_motor_driver_shared_cyclic
void rm_motor_driver_shared_cyclic(adc_callback_args_t *p_args);
#endif
/** Motor Driver shared ADC module. */
extern motor_driver_shared_instance_ctrl_t g_motor_adc_shared0_instance_ctrl;
extern const motor_driver_extended_shared_cfg_t g_motor_adc_shared0_extend_cfg;
#define IOPORT_CFG_NAME g_bsp_pin_cfg
#define IOPORT_CFG_OPEN R_IOPORT_Open
#define IOPORT_CFG_CTRL g_ioport_ctrl

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
