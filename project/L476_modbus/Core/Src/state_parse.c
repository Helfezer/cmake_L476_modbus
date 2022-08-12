#include "state.h"
#include "main.h"

// ============================================================================
//
state_machine_result_t parse_handler(state_machine_t* const StateMachine)
{
    return EVENT_HANDLED;
}

// ============================================================================
//
state_machine_result_t parse_entry_handler(state_machine_t* const StateMachine)
{
    struct modbus_device *device = getDevice();

    ModbusErrorInfo err = HandleRequest(&device->slave[0], 
                                        device->request.data, 
                                        device->request.length);

    // Respond only if the response can be accessed
    // and has non-zero length
    if (modbusIsOk(err) && modbusSlaveGetResponseLength(&device->slave[0]))
    {
        HAL_UART_Transmit(device->uart, 
                          modbusSlaveGetResponse(&device->slave[0]), 
                          modbusSlaveGetResponseLength(&device->slave[0]),
                          1000u);
    }

    return switch_state(StateMachine, &device->States[STATE_IDLE]);
}

// ============================================================================
//
state_machine_result_t parse_exit_handler(state_machine_t* const StateMachine)
{
    struct modbus_device *device = getDevice();
    device->request.length = 0;

    return EVENT_HANDLED;
}