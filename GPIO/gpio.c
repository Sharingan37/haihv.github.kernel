#include <linux/module.h>   
#include <linux/string.h>    
#include <linux/fs.h>      
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <linux/list.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/delay.h>

//#include "RPI.h"

#define LED 17 


#define GPIO_ANY_GPIO_DEVICE_DESC    "myLED"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ITtraining.com.tw");
MODULE_DESCRIPTION("A Simple GPIO Device Driver module for RaspPi");

/*
 * INIT_MODULE -- MODULE START --
 * */

int init_module(void)
{


        // -- print message 
        printk("<1> Hello World. This is myLED Driver.\n");

        // -- make 
        if(gpio_is_valid(LED) < 0){
                printk("gpio %d is valid error \n", LED);
                return -1;
        }
        if(gpio_request(LED,"LED_GPIO") < 0){
                printk("gpio %d is request error \n", LED);
                return -1;
        }
        gpio_direction_output(LED, 0);
        gpio_set_value(LED,1);
        return 0;
}



/*
 * CLEANUP_MODULE -- MODULE END --
 * */
void cleanup_module(void)
{
        printk("<1> Goodbye\n");

        gpio_set_value(LED,0);
        gpio_free(LED);

}

