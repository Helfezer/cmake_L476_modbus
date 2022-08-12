#include "state.h"
#include "main.h"

// ============================================================================
//
state_machine_result_t idle_handler(state_machine_t* const StateMachine)
{
    struct modbus_device *device = getDevice();

    switch(StateMachine->Event)
    {
        case RX:
            return switch_state(StateMachine, &device->States[STATE_RECEIVE]);

        case TIMEOUT:
        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t idle_entry_handler(state_machine_t* const StateMachine)
{
    struct modbus_device *device = getDevice();

    modbusSlaveDestroy(&device->slave[0]);
    HAL_UART_Receive_IT(device->uart, device->uartRxStr, 1);
    
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t idle_exit_handler(state_machine_t* const StateMachine)
{
    const state_t* newState = StateMachine->State;
    struct modbus_device *device = getDevice();

    if(newState == &(device->States[STATE_RECEIVE]))
    {
        platform_stop_timer(device->timer);

        platform_set_timer(device->timer,
                           device->t1_5);

        platform_start_timer(device->timer);

        HAL_UART_Receive_IT(device->uart, device->uartRxStr, 1);
        return EVENT_HANDLED;
    }
    return EVENT_UN_HANDLED;
}