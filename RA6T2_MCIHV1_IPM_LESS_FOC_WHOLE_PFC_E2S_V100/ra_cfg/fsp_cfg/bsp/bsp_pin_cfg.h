/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define SC_MC_DIO_BLDC_FAULT (BSP_IO_PORT_11_PIN_01)
#define MC_DIO_BLDC_BRAKE (BSP_IO_PORT_11_PIN_10)
#define MC_DIO_BLDC_STOP (BSP_IO_PORT_14_PIN_15)

extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA6T2BD3CFP.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
