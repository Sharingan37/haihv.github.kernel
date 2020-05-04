
/*
*Name:gpio.c
*Purpose: 
*Author: ThangLMb
*Date: 06/08/2019
*Purpose: Control Gpio qcs
*/
#include "gpio.h"
/*-------------------------------------------------------------------------
* Function:  INT GPIOExport(INT pin)
* Purpose: SEt up gpio for export
* Parameter: None	 
* Return: None
* Comment: 
---------------------------------------------------------------------------*/
 INT GPIOExport(INT pin, const char *label)
{
	 if(gpio_is_valid(pin) < 0){
	 		printk("gpio %d is valid error \n", pin);
	 		return -1;
	 	}
	 if(gpio_request(pin,label) < 0){
	 		printk("gpio %d is request error \n", pin);
	 		return -1;
	 	}
        return 0;
}
/*-------------------------------------------------------------------------
* Function:  int GPIOUnexport(int pin)
* Purpose: SEt up gpio for export
* Parameter: None	 
* Return: None
* Comment: 
---------------------------------------------------------------------------*/
INT GPIOUnexport(INT pin)
{
		gpio_free( pin );
        return 0;
}
/*-------------------------------------------------------------------------
* Function: INT  GPIODirection(INT pin, INT dir)
* Purpose: Set direction of gpio ( out or in)
* Parameter:INT pin: pin of gpio 	 
* Return: None
* Comment: 
---------------------------------------------------------------------------*/
 INT  GPIODirection(INT pin, INT dir)
{
	 if(dir == GPIO_OUT){
		 gpio_direction_output(pin, 0);
	 }else{
		 gpio_direction_input(pin);
	 }
     return(0);
}
/*-------------------------------------------------------------------------
* Function:  INT GPIOWrite(INT pin, INT value)
* Purpose: set value at gpio 
* Parameter: INT pin: pin of gpio 
                INT value: GPIO_HIGH or GPIO_LOW	 
* Return: None
* Comment: 
---------------------------------------------------------------------------*/
INT GPIOWrite(INT pin, INT value)
{
    gpio_set_value( pin, value );
        return(0);
}
