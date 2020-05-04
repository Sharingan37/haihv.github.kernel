/*
*Name:gpio.h
*Purpose: 
*Author: ThangLMb
*Date: 06/08/2019
*Purpose: Control Gpio qcs
*/

#ifndef GPIO_H__
#define GPIO_H__

#include <linux/types.h>
#include <linux/gpio.h>
#include "LC898201Typedef.h"
#define GPIO_IN  0
#define GPIO_OUT 1

#define GPIO_LOW  0
#define GPIO_HIGH 1

#define RESET_PIN 112
#define IRCUT_PIN 107
#define ENABLE_IRCUT_PIN 110


#define BUFFER_MAX 5
#define DIRECTION_MAX 35
#define VALUE_MAX 30

int GPIOExport(INT pin, const char *label);
INT GPIOUnexport(INT pin);
INT  GPIODirection(INT pin, INT dir);
INT GPIOWrite(INT pin, INT value);
#endif 
