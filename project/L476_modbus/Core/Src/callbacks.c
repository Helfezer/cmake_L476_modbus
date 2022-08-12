#include "main.h"
#include "platform.h"

/******************************************************************************/
/*                             Callbacks                                      */
/******************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    struct modbus_device *device = getDevice();
    if(huart->Instance == device->uart->Instance)
    {
        if (device->request.length < DATA_BUFFER)
        {
            device->request.data[device->request.length++] = device->uartRxStr[0];
        }

        // Do not write new event value, when the `Event` field in the `state_machine_t` is not zero
        device->Machine.Event = RX;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    struct modbus_device *device = getDevice();
    HAL_GPIO_TogglePin(debug_pin_GPIO_Port, debug_pin_Pin);

    platform_stop_timer(htim);

    // Do not write new event value, when the `Event` field in the `state_machine_t` is not zero
    if(device->Machine.Event == 0)
    {
        device->Machine.Event = TIMEOUT;
    }
    else
    {
        //If this occur a queue for event will be require
    }
}
