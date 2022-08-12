#include "state.h"
#include "main.h"
#include <stdbool.h>

bool rx_occur = false;
// ============================================================================
//
state_machine_result_t wait_handler(state_machine_t* const StateMachine)
{
    struct modbus_device *device = getDevice();

    switch(StateMachine->Event)
    {
        case RX:
            rx_occur = true;

        case TIMEOUT:
            if(rx_occur)
            {
                return switch_state(StateMachine, &device->States[STATE_IDLE]);
            }
            else
            {
                return switch_state(StateMachine, &device->States[STATE_PARSE]);
            }
            
        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t wait_entry_handler(state_machine_t* const StateMachine)
{
    rx_occur = false;
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t wait_exit_handler(state_machine_t* const StateMachine)
{
    const state_t* newState = StateMachine->State;
    struct modbus_device *device = getDevice();

    platform_stop_timer(device->timer);

    if(newState == &device->States[STATE_IDLE])
    {
        //TODO: Timing not respected -> Drop message ?
        device->request.length = 0;
        return EVENT_HANDLED;
    }
    else if (newState == &device->States[STATE_PARSE])
    {
        //disable uart
        return EVENT_HANDLED;
    }

    return EVENT_UN_HANDLED;
}