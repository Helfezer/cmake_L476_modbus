#include "main.h"
#include "platform.h"

uint8_t uartRxStr[8] = {0};

/******************************************************************************/
/*                             Callbacks                                      */
/******************************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // if(huart->Instance == huart5.Instance)
  // {
  //   if (trampoline->slave.rxBuff.length < DATA_BUFFER)
  //   {
	// 	trampoline->slave.rxBuff.data[trampoline->slave.rxBuff.length++] = uart_host_link->c;
  //   }

  //   // Do not write new event value, when the `Event` field in the `state_machine_t` is not zero
  //   trampoline->slave.Machine.Event = RX;
  //   // Trampoline_RunTime();
  // }
  
  // if(huart->Instance == huart3.Instance)
  // {
  //   if (trampoline->slave.rxBuff.length < DATA_BUFFER)
  //   {
	// 	trampoline->slave.rxBuff.data[trampoline->slave.rxBuff.length++] = uart_host_link->c;
  //   }

  //   // Do not write new event value, when the `Event` field in the `state_machine_t` is not zero
  //   trampoline->slave.Machine.Event = RX;
  //   // Trampoline_RunTime();
  // }
}

static uint8_t test = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  HAL_GPIO_TogglePin(debug_pin_GPIO_Port, debug_pin_Pin);

  platform_stop_timer(htim);

  if (test == 0)
  {
    platform_set_timer(htim, 2000);
    test++;
  }
  else
  {
    platform_set_timer(htim, 1000);
    test = 0;
  }

  platform_start_timer(htim);

    // // Do not write new event value, when the `Event` field in the `state_machine_t` is not zero
    // if(trampoline->slave.Machine.Event == 0)
    // {
    //     trampoline->slave.Machine.Event = TIMEOUT;
    // }
    // else
    // {
    //     //If this occur a queue for event will be require
    // }

    // // Trampoline_RunTime();
}
