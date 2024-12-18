// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <syscalls.h>
#include <time.h>

static uint8_t bcdToDec(uint8_t bcd);

uint8_t getSeconds() {
	return bcdToDec((uint8_t) _sys_getTime(SECONDS_ARG));
}

uint8_t getMinutes() {
	return bcdToDec((uint8_t) _sys_getTime(MINUTES_ARG));
}

uint8_t getHours() {
	return bcdToDec((uint8_t) _sys_getTime(HOURS_ARG));
}

uint8_t getDay() {
	return bcdToDec((uint8_t) _sys_getTime(DAY_ARG));
}

uint8_t getMonth() {
	return bcdToDec((uint8_t) _sys_getTime(MONTH_ARG));
}

uint32_t getYear() {
	return bcdToDec((uint8_t) _sys_getTime(YEAR_ARG)) + YEAR_REFERENCE;
}

static uint8_t bcdToDec(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}