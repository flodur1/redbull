#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include "stm32f10x.h"

typedef struct {
	uint16_t year;	/* 1..4095 */
	uint8_t  month;	/* 1..12 */
	uint8_t  mday;	/* 1..31 */
	uint8_t  wday;	/* 0..6, Sunday = 0*/
	uint8_t  hour;	/* 0..23 */
	uint8_t  min;	/* 0..59 */
	uint8_t  sec;	/* 0..59 */
	uint8_t  dst;	/* 0 Winter, !=0 Summer */
} RTC_t;

int RTC_Init(void);
bool RTC_GetTime (RTC_t*);				/* Get time */
bool RTC_SetTime (const RTC_t*);		/* Set time */

#endif