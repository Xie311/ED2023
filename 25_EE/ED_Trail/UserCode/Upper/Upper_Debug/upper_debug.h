#ifndef __UPPER_DEBUG_H
#define __UPPER_DEBUG_H

#include "Upperstart.h"

void Upper_Debug_TaskStart(void);
void Upper_Debug_Task(void *argument);
void Upper_OLED_TaskStart(void);
void Upper_OLED_Task(void *argument);
void Upper_Reset_TaskStart(void);
void Upper_Reset_Task(void *argument);
#endif