#include "modbus.h"

// Include implementation here
#define LIGHTMODBUS_IMPL
// Library configuration
#define LIGHTMODBUS_SLAVE_FULL
#define LIGHTMODBUS_SLAVE
#include "lightmodbus.h"

#include "state.h"
#include <assert.h>

extern state_machine_t * const State_Machines[];
uint16_t regs[REG_COUNT] = {0};

/******************************************************************************/
/***************************** Private functions ******************************/
/******************************************************************************/
// ============================================================================
//
/**
 * \brief allocator for response memory space
 */
ModbusError regCallback(const ModbusSlave *slave,
                        const ModbusRegisterCallbackArgs *args,
                        ModbusRegisterCallbackResult *result)
{
    switch (args->query)
    {
        // All regs can be read
        case MODBUS_REGQ_R_CHECK:
            if (args->index < REG_COUNT)
                result->exceptionCode = MODBUS_EXCEP_NONE;
            else	
                result->exceptionCode = MODBUS_EXCEP_ILLEGAL_ADDRESS;
            break;

        // Read registers
        case MODBUS_REGQ_R:
            switch (args->type)
            {
                case MODBUS_HOLDING_REGISTER: result->value = regs[args->index]; break;
                case MODBUS_INPUT_REGISTER: result->value = regs[args->index]; break;
                case MODBUS_COIL: result->exceptionCode = MODBUS_EXCEP_SLAVE_FAILURE; break;
                case MODBUS_DISCRETE_INPUT: result->exceptionCode = MODBUS_EXCEP_SLAVE_FAILURE; break;
            }
            break;

        // Write registers not possible
        case MODBUS_REGQ_W_CHECK:
        case MODBUS_REGQ_W:
        default:
            result->exceptionCode = MODBUS_EXCEP_SLAVE_FAILURE;
            break;
    }

    return MODBUS_OK;
}

// ============================================================================
//
/**
 * \brief Callback call by lightmodbus default function to access register
 */
ModbusError staticAllocator(ModbusBuffer *buffer,
                            uint16_t size,
                            void *context)
{
    // Array for holding the response frame
    static uint8_t response[MAX_RESPONSE];
    static uint8_t allocated = 0;

    if (size != 0) // Allocation reqest
    {
        if (size <= MAX_RESPONSE && allocated == 0) // Allocation request is within bounds
        {
            buffer->data = response;
            allocated = 1;
            return MODBUS_OK;
        }
        else // Allocation error
        {
            buffer->data = NULL;
            return MODBUS_ERROR_ALLOC;
        }
    }
    else // Free request
    {
        allocated = 0;
        buffer->data = NULL;
        return MODBUS_OK;
    }
}

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

    idle_entry_handler((state_machine_t*)device);
}

// ============================================================================
//
void ModbusDevice_Runtime(struct modbus_device *device)
{
    if(dispatch_event(State_Machines, 1) == EVENT_UN_HANDLED)
    {

    }
}

// ============================================================================
//
ModbusErrorInfo HandleRequest(ModbusSlave   *buffer,
                              const uint8_t *data, 
                              uint16_t      length)
{
    // Attempt to parse the received frame
    ModbusErrorInfo err = modbusParseRequestRTU(
        buffer,
        SLAVE_ADDRESS,
        data,
        length
    );

    // We ignore request/response errors 
    // and only care about the serious stuff
    switch (modbusGetGeneralError(err))
    {
        // We're fine
        case MODBUS_OK:
            break;

        // Since we're only doing static memory allocation
        // we can nicely handle memory allocation errors
        // and respond with a slave failure exception
        case MODBUS_ERROR_ALLOC:
            
            // We must be able to retrieve the function code byte
            if (length < 2)
                break;

            err = modbusBuildExceptionRTU(
                buffer,
                SLAVE_ADDRESS,
                data[1],
                MODBUS_EXCEP_SLAVE_FAILURE);
            
            if (!modbusIsOk(err))
            {
                // Error while handling error. should die.
            }

            break;

        // Oh no.
        default:
            break;
    }

    return err;
}