#include "USART_Init.h"
void Gantry_usart_init()
{
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart1, Rxbuffer[0], sizeof(Rxbuffer[0]));

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart2, Rxbuffer[1], sizeof(Rxbuffer[1]));

    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart3, Rxbuffer[2], sizeof(Rxbuffer[2]));

    __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart4, Rxbuffer[3], sizeof(Rxbuffer[3]));

    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart6, Rxbuffer[4], sizeof(Rxbuffer[4]));
}