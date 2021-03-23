#include <app_statuslight.h>
#include <drv_ws2812b.h>
#include <FreeRTOS.h>
#include <task.h>
#include <drv_can.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdbool.h>
#include <assert.h>
 
jmp_buf env_app, env_test;

TickType_t currentTime;
int canLec;

bool in_delayFlag;
TickType_t in_timeIncrement;
bool in_transmitFlag;
int in_ledLength;
const uint8_t *in_ledData;

#define TEST_START(fn) \
	val = setjmp(env_test); \
	if (val == 0) \
	{ \
		fn(); \
	}

#define TEST_RESUME \
	val = setjmp(env_test); \
	if (val == 0) \
	{ \
		longjmp(env_app, 1); \
	}
	
int main()
{
	int val;
	currentTime = 0;
	canLec = 0;
	in_delayFlag = in_transmitFlag = false;
	TEST_START(app_statuslight_init)
	
	// Should have paused for ~1ms for ws init
	assert(in_timeIncrement == 1);
	
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	
	// Should have tried to transmit a green pulse for 500ms
	assert(in_timeIncrement == 500);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 255);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);

	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	
	// Should have tried to transmit a off pulse for 1500ms
	assert(in_timeIncrement == 1500);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);
	
	canLec = 7;
	
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	
	// Should have tried to transmit a green pulse for 500ms
	assert(in_timeIncrement == 500);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 255);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);

	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	
	// Should have tried to transmit a off pulse for 1500ms
	assert(in_timeIncrement == 1500);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);
	
	canLec = 2; // should trigger two pulses
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	assert(in_timeIncrement == 300);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 255);
	assert(in_ledData[2] == 0);
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	assert(in_timeIncrement == 300);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	assert(in_timeIncrement == 300);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 255);
	assert(in_ledData[2] == 0);
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	assert(in_timeIncrement == 300);
	assert(in_transmitFlag);
	assert(in_ledData);
	assert(in_ledLength == 3);
	assert(in_ledData[0] == 0);
	assert(in_ledData[1] == 0);
	assert(in_ledData[2] == 0);
	// then should turn off for 1500ms
	in_transmitFlag = false;
	in_ledData = NULL;
	in_ledLength = 0;
	TEST_RESUME
	assert(in_timeIncrement == 1500);
	assert(in_transmitFlag == false);

	
	puts("Success");
	
	exit(0);
}

BaseType_t xTaskCreate(
	TaskFunction_t pxTaskCode,
	const char * const pcName,
	const configSTACK_DEPTH_TYPE usStackDepth,
	void * const pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t * const pxCreatedTask)
{
	pxTaskCode(pvParameters);
}

TickType_t xTaskGetTickCount(void)
{
	return currentTime;
}

void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement)
{
	int val;
	in_delayFlag = true;
	in_timeIncrement = xTimeIncrement;
	val = setjmp(env_app);
	if (val == 0)
	{
		// pause app execution for test validation
		longjmp(env_test, 1);
	}
}

void drv_ws2812b_transmit(enum drv_ws2812b_channel led, const uint8_t *data, int length)
{
	in_transmitFlag = true;
	in_ledData = data;
	in_ledLength = length;
}

int drv_can_read_lec(can_registers_t * bus)
{
	return canLec;
}
