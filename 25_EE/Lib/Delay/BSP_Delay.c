//
// Created by Doge60 on 2023/11/1.
//

// 说明：使用该库时，请先在CubeMX中选择一个定时器
// 并使【系统时钟频率】/【预分频值】=1000000Hz/1MHz
// ARR按自己需要设，其值即为最大计时时间（可以赋给us、ms形参的最大值）
// 然后去头文件里把TIM_Select宏常量改成自己选的定时器编号
// （ps:这里假设您已经通过了初始电控培训，并知道在CubeMX里配置stm32的基本操作^_^）

#include "BSP_Delay.h"

void delay_us(uint16_t us)
{
    uint16_t counter = 0;
    __HAL_TIM_SetCounter(TIM_Select, counter);
    HAL_TIM_Base_Start(TIM_Select);

    while (counter != us) {
        counter = __HAL_TIM_GetCounter(TIM_Select);
    }
    HAL_TIM_Base_Stop(TIM_Select);
}

void delay_ms(uint16_t ms)
{
    for (int i = 0; i < ms; i++) {
        delay_us(1000);
    }
}