#include <assert.h>
#include "modbus.h"
#include "state.h"

extern uint8_t uartRxStr[8];
extern state_machine_t * const State_Machines[];
uint16_t regs[REG_COUNT] = {0};

/******************************************************************************/
/***************************** Private functions ******************************/
/******************************************************************************/

// ============================================================================
//
/**
 * \brief Calculate timeout based on baudrate to match Modbus standard
 * \param[in, out] modbus pointer to the modbus_slave object to configure 
 * \details Standard definition:
 *          "Modbus states that a baud rate higher than 19200 must use a fixed
 *          750 us for inter character time out and 1.75 ms for a frame delay.
 *          For baud rates below 19200 the timeing is more critical and has to
 *          be calculated. 
 *          For example: 9600 baud in a 10 bit packet is 960 characters per 
 *          second In milliseconds this will be 960characters per 1000ms. 
 *          So for 1 character 1000ms/960characters is 1.04167ms per character 
 *          and finaly modbus states an intercharacter must be 
 *          1.5T or 1.5 times longer than a normal character and thus 
 *          1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T."
 */
void ConfigureModbusTimeout(struct modbus_device *device)
{
    if(device->uart->Init.BaudRate > 19200)
    {
        device->t1_5 = T1_5;
        device->t2   = T2;
    }
    else
    {
        float T = 11.0f / (float)device->uart->Init.BaudRate; //result in sec
        device->t1_5 = 1.5 * T * 1000000; //us
        device->t2  = 2 * T * 1000000;   //us
    }
}

/******************************************************************************/
/****************************** Public functions ******************************/
/******************************************************************************/

// ============================================================================
//
void ModbusDevice_Init(struct modbus_device *device)
{
    //! 1- Uart configuration: start callback
    //uart initialisation done in main
    HAL_UART_Receive_IT(device->uart, uartRxStr, 1);

    //! 2- Timer configuration
    //tier intialisation done in main
    ConfigureModbusTimeout(device);

    //! 3- State Machine configuration
    device->States[STATE_IDLE].Handler = idle_handler;
    device->States[STATE_IDLE].Entry   = idle_entry_handler;
    device->States[STATE_IDLE].Exit    = idle_exit_handler;

    device->States[STATE_RECEIVE].Handler = receive_handler;
    device->States[STATE_RECEIVE].Entry   = receive_entry_handler;
    device->States[STATE_RECEIVE].Exit    = receive_exit_handler;

    device->States[STATE_WAIT].Handler = wait_handler;
    device->States[STATE_WAIT].Entry   = wait_entry_handler;
    device->States[STATE_WAIT].Exit    = wait_exit_handler;

    device->States[STATE_PARSE].Handler = parse_handler;
    device->States[STATE_PARSE].Entry   = parse_entry_handler;
    device->States[STATE_PARSE].Exit    = parse_exit_handler;

    device->Machine.State = &device->States[STATE_IDLE];
    device->Machine.Event = 0;

    //! 4- Modbus initialization
    ModbusErrorInfo err;

    err = modbusSlaveInit(&device->slave[0],
                          regCallback,
                          NULL,
                          staticAllocator,
                          modbusSlaveDefaultFunctions,
                          modbusSlaveDefaultFunctionCount);

    assert(modbusIsOk(err));
}

// ============================================================================
//
void ModbusDevice_Runtime(struct modbus_device *device)
{
    if(dispatch_event(State_Machines, 1) == EVENT_UN_HANDLED)
    {

    }
}