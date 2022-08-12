#ifndef INC_MODBUS_H
#define INC_MODBUS_H

/* ---------------------------------------------------------------------------*/
/* -------------------------------- Includes ---------------------------------*/
/* ---------------------------------------------------------------------------*/
// No implementation here
#define LIGHTMODBUS_SLAVE_FULL
#include "lightmodbus.h"

// Library include
#include "hsm.h"        // Lib UML-State-Machine-in-C

//Platform include
#include "stm32l4xx_hal_conf.h"

/* ---------------------------------------------------------------------------*/
/* -------------------------------- Defines ----------------------------------*/
/* ---------------------------------------------------------------------------*/
#define NB_STATE         4u
#define NB_MODBUS_SLAVE  1u

// Number of registers (uint16_t)
#define REG_COUNT     58

// Maximum Data response (all registers at once)
#define DATA_BUFFER   REG_COUNT*2
#define FRAME_SIZE    5   // Frame bytes requiere in addition to Data buffer bytes
#define MAX_RESPONSE  DATA_BUFFER + FRAME_SIZE 

// Fixed Timeout when baudrate > 19200
#define T1_5 750  //us
#define T2   1000 //us
#define T3_5 1750 //us
/* ---------------------------------------------------------------------------*/
/* -------------------------------- Macros -----------------------------------*/
/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/* -------------------------------- Typedef ----------------------------------*/
/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/* ------------------------------- Structures --------------------------------*/
/* ---------------------------------------------------------------------------*/
/**
 * \brief Received data buffer
 */
struct rx
{
    uint8_t data[DATA_BUFFER]; //!< Received data
    uint16_t length;           //!< Length of the request
};


/**
 * \brief Modbus device object
 * 
 */
struct modbus_device
{
    state_machine_t     Machine;
    state_t             States[NB_STATE];
    ModbusSlave         slave[NB_MODBUS_SLAVE];
    UART_HandleTypeDef* uart;
    TIM_HandleTypeDef*  timer;
    struct rx           rxBuff;
    uint16_t            t1_5;
    uint16_t            t2;
};

/* ---------------------------------------------------------------------------*/
/* ------------------------------- Prototype ---------------------------------*/
/* ---------------------------------------------------------------------------*/
/**
 * \brief allocator for response memory space
 */
ModbusError staticAllocator(ModbusBuffer *buffer,
                            uint16_t size,
                            void *context);

/**
 * \brief Callback call by lightmodbus default function to access register
 */
ModbusError regCallback(const ModbusSlave *slave,
                        const ModbusRegisterCallbackArgs *args,
                        ModbusRegisterCallbackResult *result);

/**
 * \brief 
 */
ModbusErrorInfo HandleRequest(ModbusSlave   *buffer,
                              const uint8_t *data, 
                              uint16_t      length);

/**
 * \brief init function
 */
void ModbusDevice_Init(struct modbus_device *device);

/**
 * \brief runtime function
 */
void ModbusDevice_Runtime(struct modbus_device *device);

#endif //INC_MODBUS_H