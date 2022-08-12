#include "state.h"
#include "main.h"

// ============================================================================
//
state_machine_result_t receive_handler(state_machine_t* const StateMachine)
{
    struct modbus_device * const device = getDevice();
    
    switch(StateMachine->Event)
    {
        case RX:
            return switch_state(StateMachine, &device->States[STATE_RECEIVE]);

        case TIMEOUT:
            return switch_state(StateMachine, &device->States[STATE_WAIT]);

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t receive_entry_handler(state_machine_t* const StateMachine)
{
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t receive_exit_handler(state_machine_t* const StateMachine)
{
    const state_t* newState = StateMachine->State;
    struct modbus_device *device = getDevice();

    if(newState == &device->States[STATE_RECEIVE])
    {
        platform_stop_timer(device->timer); //In case timeout as not occur?

        platform_set_timer(device->timer,   //reloanch 1.5t timeout betweend each char
                           device->t1_5);

        platform_start_timer(device->timer);

        HAL_UART_Receive_IT(device->uart, device->uartRxStr, 1);
        return EVENT_HANDLED;
    }
    else if (newState == &device->States[STATE_WAIT])
    {
        platform_stop_timer(device->timer);

        platform_set_timer(device->timer,   //Loanch 2.0t timeout
                           device->t2);

        platform_start_timer(device->timer);

        HAL_UART_Receive_IT(device->uart, device->uartRxStr, 1);  // reloanch read to detect new char error

        return EVENT_HANDLED;
    }

    return EVENT_UN_HANDLED;
}