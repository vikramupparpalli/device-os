/*
 * Copyright (c) 2018 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "pinmap_hal.h"

typedef struct Hal_Pin_Info {
    uint8_t     gpio_port; // port0: 0; port: 1;
    uint8_t     gpio_pin;  // range: 0~31;
    PinMode     pin_mode;  // GPIO pin mode
    PinFunction pin_func;
    uint8_t     adc_channel;
    uint8_t     pwm_instance;   // 4 instances on nRF52, range: 0~3
    uint8_t     pwm_channel;    // 4 channels in each instance, range: 0~3
    uint8_t     pwm_resolution; // default 8bit, max 15bit
    uint8_t     exti_channel;   // 16 channels
    uint32_t    user_data;
} Hal_Pin_Info;

// For compatibility
typedef Hal_Pin_Info NRF5x_Pin_Info;

extern const uint8_t NRF_PIN_LOOKUP_TABLE[48];

#define NRF_PORT_NONE ((uint8_t)(0xFF))
#define NRF_PORT_0 ((uint8_t)(0))
#define NRF_PORT_1 ((uint8_t)(1))

#define CHANNEL_NONE ((uint8_t)(0xFF))
#define ADC_CHANNEL_NONE CHANNEL_NONE
#define DAC_CHANNEL_NONE CHANNEL_NONE
#define PWM_INSTANCE_NONE ((uint8_t)(0xFF))
#define PWM_CHANNEL_NONE CHANNEL_NONE
#define EXTI_CHANNEL_NONE CHANNEL_NONE

#define DEFAULT_PWM_FREQ 500 // 500Hz
#define TIM_PWM_FREQ DEFAULT_PWM_FREQ

#include "pinmap_defines.h"

#if PLATFORM_ID == PLATFORM_BG96

#define TOTAL_PINS 42
#define TOTAL_ANALOG_PINS 8
#define FIRST_ANALOG_PIN 8

// Digital pins
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

// Analog pins
#define A0 8
#define A1 9
#define A2 10
#define A3 11
#define A4 12
#define A5 13
#define A6 14
#define A7 15

#define B0 16
#define B1 17
#define B2 18
#define B3 19
#define B4 20
#define B5 21

#define C0 22
#define C1 23
#define C2 24
#define C3 25
#define C4 26
#define C5 27

#define BTN 28
#define RGBR 29
#define RGBG 30
#define RGBB 31

#define TX 32
#define RX 33
#define CTS D3
#define RTS D2

#define SS A2
#define SCK A3
#define MISO A4
#define MOSI A5

#define SDA D0
#define SCL D1

#define TX1 34
#define RX1 35
#define CTS1 36
#define RTS1 37

#define NFC_PIN1 B0
#define NFC_PIN2 B1

#define WKP A7

#define BGPWR 38
#define BGRST 39
#define BGVINT 40
#define BGDTR 41

#endif // PLATFORM_ID == PLATFORM_BG96

#ifdef __cplusplus
}
#endif
