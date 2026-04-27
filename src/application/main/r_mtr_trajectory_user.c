/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* 
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#ifdef MOTOR_CONTROL_TRAJECTORY_ENABLED
#include "rm_motor_trajectory_linear.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
 /***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
  void rm_motor_trajectory_init(void);
 
 /***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Global variabless
 **********************************************************************************************************************/
static motor_trajectory_command_status_t cmd_status;

/*******************************************************************************************************************//**
 * @brief Initialize trajectory module with proper configuration
 *
 * @return None
 **********************************************************************************************************************/
void rm_motor_trajectory_init(void)
{
    /* Define linear trajectory parameters.
     *
     * By using the MOTOR_TRAJECTORY_LINEAR_PARAMS_INIT macro, the base structure
     * fields such as type identifier and version information are automatically set.
     * 
     * Parameter meanings:
     *   - 1st argument (0.0F):    Target speed [rpm]
     *   - 2nd argument (500.0F):  Acceleration/Deceleration rate [rpm/s] (slew_rate)
     * 
     * Note: Defined as static const to keep it in memory after function exit.
     *       This is necessary because target.p_params continues to reference this structure.
     * */
    static const motor_trajectory_linear_params_t linear_params = 
        MOTOR_TRAJECTORY_LINEAR_PARAMS_INIT(0.0F, 500.0F);
    
    /* Configure the target structure used by the trajectory control API's common interface.
     *
     *   This design allows adding different trajectory types (trapezoidal, position control, etc.)
     *   in the future while using the same API interface. The implementation can safely cast
     *   the base pointer to the specific type after validating the type_id field.
     * 
     * Type safety mechanism:
     *   The targetSet function will check type_id to ensure the pointer is cast to the
     *   correct trajectory parameter type, preventing undefined behavior from incorrect casts.
     */
    motor_trajectory_target_t target = {
        .p_params = &linear_params.base
    };

    /* Disable bypass mode to enable trajectory control.
     * 
     * Bypass mode options:
     *   - false: Enable trajectory control (acceleration/deceleration control active)
     *            The module will smoothly ramp the speed from current to target value
     *            using the configured slew rates.
     *   - true:  Disable trajectory control (target value output directly)
     *            The target speed is applied immediately without ramping.
     *            Useful for debugging or emergency situations.
     * */
    g_motor0_trajectory_linear.p_api->bypassModeSet(g_motor0_trajectory_linear.p_ctrl, false);

    /* Set the initial target to the trajectory control module.
     * 
     * Arguments:
     *   - p_ctrl:       Control structure of the trajectory control instance
     *   - &target:      Target parameters to set (defined in Step 2)
     *   - &cmd_status:  Command acceptance status is returned here
     * 
     * cmd_status return values:
     *   - MOTOR_TRAJECTORY_CMD_ACCEPTED_IMMEDIATE: 
     *       Command accepted and execution started immediately.
     *       This occurs when the trajectory module is idle.
     *   
     *   - MOTOR_TRAJECTORY_CMD_QUEUED: 
     *       Command queued for later execution.
     *       This occurs when a previous command is still in progress.
     *       The queued command will start automatically when current one completes.
     *   
     *   - MOTOR_TRAJECTORY_CMD_REJECTED: 
     *       Command rejected (queue is full or invalid parameters).
     *       Check the queue status or parameter validity.
     * 
     * Execution behavior:
     *   After this function call, the motor will start accelerating from its current speed
     * */
    g_motor0_trajectory_linear.p_api->targetSet(g_motor0_trajectory_linear.p_ctrl,
                                                &target,
                                                &cmd_status);
}
#endif
