#ifndef TPE_ARQ_TIME_H
#define TPE_ARQ_TIME_H

#include <stdint.h>

#define YEAR_REFERENCE 2000

/**
 * @brief returns the seconds of the current time read from the RTC.
 */
uint8_t getSeconds();

/**
 * @brief returns the minutes of the current time read from the RTC.
 */
uint8_t getMinutes();

/**
 * @brief returns the hours of the current time read from the RTC.
 */
uint8_t getHours();

/**
 * @brief returns the day of the month of the current time read from the RTC.
 */
uint8_t getDay();

/**
 * @brief returns the month of the current time read from the RTC.
 */
uint8_t getMonth();

/**
 * @brief returns the year of the current time read from the RTC.
 */
uint32_t getYear();


#endif //TPE_ARQ_TIME_H
