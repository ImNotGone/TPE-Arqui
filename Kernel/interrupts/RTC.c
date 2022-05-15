#include <RTC.h>

extern uint8_t sys_RTC(uint8_t mode);

static uint32_t binaryCodedDecimalToDecimal(uint8_t BCD) {
    return (BCD >> 4) * 10 + (BCD & 0x0F);
}

static uint32_t getRTC(uint8_t mode) {
    return binaryCodedDecimalToDecimal(sys_RTC(mode));
}

uint32_t getRTCSeconds() {
    return getRTC(RTC_SECONDS);
}

uint32_t getRTCMinutes() {
    return getRTC(RTC_MINUTES);
}

uint32_t getRTCHours() {
    return getRTC(RTC_HOURS);
}

uint32_t getRTCDayOfWeek() {
    return getRTC(RTC_DAY_OF_WEEK);
}

uint32_t getRTCDayOfMonth() {
    return getRTC(RTC_DAY_OF_MONTH);
}

uint32_t getRTCMonth() {
    return getRTC(RTC_MONTH);
}

uint32_t getRTCYear() {
    return getRTC(RTC_YEAR);
}


