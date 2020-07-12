/*
 * drv_main.c
 *
 * Created: 7/11/2020 6:12:01 PM
 *  Author: connor
 */ 

#include "drv_main.h"
#include "drv_gpio.h"
#include "drv_can.h"
#include "sam.h"

/************************************************************************/
/* Initialize all drivers that will be used by this project.            */
/************************************************************************/
void drv_init(void)
{
	// increase flash wait states from 0 to 1. if this is not done, the
	// CPU has a seizure trying to run at 48MHz
	NVMCTRL->CTRLB.bit.RWS = 1;
	// up speed to 48MHz
	OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(0b0000);
	// wait for synchronization
	while(OSCCTRL->OSC48MSYNCBUSY.reg) ;
	
	drv_gpio_init();
	drv_can_init();
}

/************************************************************************/
/* Periodic run function for all drivers in this project.               */
/************************************************************************/
void drv_periodic(void)
{
}