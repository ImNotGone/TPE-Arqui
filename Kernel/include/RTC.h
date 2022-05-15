#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define RTC_SECONDS         0x00
#define RTC_SECONDS_ALARM   0x01
#define RTC_MINUTES         0x02
#define RTC_MINUTES_ALARM   0x03
#define RTC_HOURS           0x04
#define RTC_HOURS_ALARM     0x05
#define RTC_DAY_OF_WEEK     0x06
#define RTC_DAY_OF_MONTH    0x07
#define RTC_MONTH           0x08
#define RTC_YEAR            0x09

uint32_t getRTCSeconds();
uint32_t getRTCMinutes();
uint32_t getRTCHours();
uint32_t getRTCDayOfWeek();
uint32_t getRTCDayOfMonth();
uint32_t getRTCMonth();
uint32_t getRTCYear();

#endif//RTC_H