/**
 * @file rtc.c
 * @brief RTCC (Real-Time Clock Calendar) driver for PIC32MZ2048EFG100
 * Provides initialization, time management, and peak hour detection functionality
 * using the microcontroller's internal RTCC peripheral.
 * 
 * Author: Engr. Fahad
 *         Alias: penguin
 * Last Update: August 08, 2025
 */

#include <xc.h>
#include <stdint.h>

// Global buffer for formatted datetime string (YYYY-MM-DD HH:MM:SS)
char current_datetime[20];

// External flag for tariff status (defined in main application)
extern int multiTariff;

// Peak hour configuration (in minutes since midnight)
static uint16_t peak_start_minutes = 0;   // Default: 00:00
static uint16_t peak_end_minutes = 0;     // Default: 00:00

/* RTCC Helper Functions */

/**
 * @brief Converts integer value to BCD format
 * @param value Integer to convert (0-99)
 * @return BCD encoded value
 */
static uint8_t int_to_bcd(uint8_t value) {
    return ((value / 10) << 4) | (value % 10);
}

/**
 * @brief Packs time components into RTCC TIME register format
 * @param hour 24-hour format hour (0-23)
 * @param min Minutes (0-59)
 * @param sec Seconds (0-59)
 * @return 32-bit value for RTCTIME register
 */
static uint32_t pack_time(uint8_t hour, uint8_t min, uint8_t sec) {
    return ((uint32_t)int_to_bcd(hour) << 24) |
           ((uint32_t)int_to_bcd(min) << 16) |
           ((uint32_t)int_to_bcd(sec) << 8);
}

/**
 * @brief Packs date components into RTCC DATE register format
 * @param year Full year (2000-2099)
 * @param month Month (1-12)
 * @param day Day (1-31)
 * @return 32-bit value for RTCDATE register
 */
static uint32_t pack_date(uint16_t year, uint8_t month, uint8_t day) {
    return ((uint32_t)int_to_bcd(year % 100) << 24) |  // Last 2 digits of year
           ((uint32_t)int_to_bcd(month) << 16) |
           ((uint32_t)int_to_bcd(day) << 8);
}

/* Core RTCC Functions */

/**
 * @brief Low-level RTCC hardware initialization
 * @param time Pre-formatted TIME register value
 * @param date Pre-formatted DATE register value
 * 
 * Note: Follows Microchip's recommended initialization sequence
 */
void __initRTC(uint32_t time, uint32_t date) {
    // Disable RTCC before configuration
    RTCCONbits.ON = 0;
    
    // Unlock system for clock configuration
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RTCCONSET = 0x00000008;  // Enable write access
    SYSKEY = 0x00;

    // Clear RTCCON register (recommended sequence)
    RTCCONCLR = 0x8000;
    RTCCONCLR = 0x8000;
    RTCCONCLR = 0x8000;
    RTCCONCLR = 0x8000;
    
    // Recommended delay after configuration changes
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    
    // Set initial time and date
    RTCTIME = time;
    RTCDATE = date;

    // Enable RTCC and wait for synchronization
    RTCCONSET = 0x8000;
    while (!(RTCCON & 0x000040));

    // Lock system after configuration
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RTCCONCLR = 0x00000008;  // Disable write access
    SYSKEY = 0x00;
    
    // Enable RTCC operation
    RTCCONbits.ON = 1;
}

/**
 * @brief User-friendly RTCC initialization
 * @param year Full year (2000-2099)
 * @param month Month (1-12)
 * @param day Day (1-31)
 * @param hour 24-hour format hour (0-23)
 * @param minute Minutes (0-59)
 * @param second Seconds (0-59)
 */
void RTC_Initialize(uint16_t year, uint8_t month, uint8_t day, 
                    uint8_t hour, uint8_t minute, uint8_t second) {
    uint32_t rtc_time = pack_time(hour, minute, second);
    uint32_t rtc_date = pack_date(year, month, day);
    __initRTC(rtc_time, rtc_date);
}

/* Time Management Functions */

/**
 * @brief Updates the global current_datetime string with current RTCC values
 * 
 * Performs consistent reads of both TIME and DATE registers to ensure
 * values are read within the same second.
 */
void RTC_UpdateDateTime(void) {
    uint32_t date_reg, time_reg;
    uint32_t date_check, time_check;
    
    // Read registers twice to ensure consistent values
    do {
        date_reg = RTCDATE;
        time_reg = RTCTIME;
        date_check = RTCDATE;
        time_check = RTCTIME;
    } while ((date_reg != date_check) || (time_reg != time_check));

    // Extract BCD components from registers
    uint8_t year_bcd  = (date_reg >> 24) & 0xFF;
    uint8_t month_bcd = (date_reg >> 16) & 0xFF;
    uint8_t day_bcd   = (date_reg >> 8)  & 0xFF;
    uint8_t hour_bcd = (time_reg >> 24) & 0xFF;
    uint8_t min_bcd  = (time_reg >> 16) & 0xFF;
    uint8_t sec_bcd  = (time_reg >> 8)  & 0xFF;

    // Format as YYYY-MM-DD HH:MM:SS string
    current_datetime[0] = '2';  // Assume 21st century (2000-2099)
    current_datetime[1] = '0';
    current_datetime[2] = (year_bcd >> 4) + '0';
    current_datetime[3] = (year_bcd & 0x0F) + '0';
    current_datetime[4] = '-';
    current_datetime[5] = (month_bcd >> 4) + '0';
    current_datetime[6] = (month_bcd & 0x0F) + '0';
    current_datetime[7] = '-';
    current_datetime[8] = (day_bcd >> 4) + '0';
    current_datetime[9] = (day_bcd & 0x0F) + '0';
    current_datetime[10] = ' ';
    current_datetime[11] = (hour_bcd >> 4) + '0';
    current_datetime[12] = (hour_bcd & 0x0F) + '0';
    current_datetime[13] = ':';
    current_datetime[14] = (min_bcd >> 4) + '0';
    current_datetime[15] = (min_bcd & 0x0F) + '0';
    current_datetime[16] = ':';
    current_datetime[17] = (sec_bcd >> 4) + '0';
    current_datetime[18] = (sec_bcd & 0x0F) + '0';
    current_datetime[19] = '\0';
}

/* Peak Hour Functions */

/**
 * @brief Configures peak hour time window
 * @param start_hour Starting hour (0-23)
 * @param start_min Starting minute (0-59)
 * @param end_hour Ending hour (0-23)
 * @param end_min Ending minute (0-59)
 */
void setpeakhours(uint8_t start_hour, uint8_t start_min, 
                  uint8_t end_hour, uint8_t end_min) {
    peak_start_minutes = (start_hour * 60) + start_min;
    peak_end_minutes = (end_hour * 60) + end_min;
}

/**
 * @brief Checks if current time falls within peak hours
 * 
 * Updates the multiTariff flag and outputs status via UART
 */
void checkPeakHours(void) {
    uint32_t time_reg, time_check;
    uint8_t hour_bcd, min_bcd;
    
    // Ensure consistent time read
    do {
        time_reg = RTCTIME;
        time_check = RTCTIME;
    } while (time_reg != time_check);

    // Extract time components
    hour_bcd = (time_reg >> 24) & 0xFF;
    min_bcd  = (time_reg >> 16) & 0xFF;
    
    // Convert BCD to integers
    uint8_t current_hour = ((hour_bcd >> 4) * 10) + (hour_bcd & 0x0F);
    uint8_t current_min  = ((min_bcd >> 4) * 10) + (min_bcd & 0x0F);
    
    // Calculate minutes since midnight
    uint16_t current_minutes = (current_hour * 60) + current_min;

    // Determine tariff status
    if (current_minutes >= peak_start_minutes && 
        current_minutes < peak_end_minutes) {
        __writeStringUart1("(PEAK-HOUR)\n");
        multiTariff = 1;
    } else {
        __writeStringUart1("(OFFPEAK-HOUR)\n");
        multiTariff = 0;
    }
}

/* Utility Functions */

/**
 * @brief Displays current datetime via UART
 */
void display_current_time() {
    __writeStringUart1("\n\rCurrent Time: ");
    __writeStringUart1(current_datetime);
    __writeStringUart1("\n");
}
