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

#include "pinmap_hal.h"

#if PLATFORM_ID == PLATFORM_BG96

static Hal_Pin_Info s_pin_map[TOTAL_PINS] = {
/* D0            - 00 */ { NRF_PORT_1,    1,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 0,                8, EXTI_CHANNEL_NONE},
/* D1            - 01 */ { NRF_PORT_1,    2,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 1,                8, EXTI_CHANNEL_NONE},
/* D2            - 02 */ { NRF_PORT_1,    10, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D3 - RESET    - 03 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D4 - SWO      - 04 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D5 -          - 05 */ { NRF_PORT_1,    11, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D6 - SWDCLK   - 06 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D7 - SWDIO    - 07 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 0,                8, EXTI_CHANNEL_NONE},
/* A0            - 08 */ { NRF_PORT_0,    3,  PIN_MODE_NONE, PF_NONE, 1,                2,                 0,                8, EXTI_CHANNEL_NONE},
/* A1            - 09 */ { NRF_PORT_0,    2,  PIN_MODE_NONE, PF_NONE, 2,                2,                 1,                8, EXTI_CHANNEL_NONE},
/* A2            - 10 */ { NRF_PORT_0,    28, PIN_MODE_NONE, PF_NONE, 4,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A3            - 11 */ { NRF_PORT_0,    29, PIN_MODE_NONE, PF_NONE, 5,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A4            - 12 */ { NRF_PORT_0,    30, PIN_MODE_NONE, PF_NONE, 6,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A5            - 13 */ { NRF_PORT_0,    31, PIN_MODE_NONE, PF_NONE, 7,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A6            - 14 */ { NRF_PORT_0,    4,  PIN_MODE_NONE, PF_NONE, 3,                2,                 2,                8, EXTI_CHANNEL_NONE},
/* A7            - 15 */ { NRF_PORT_0,    5,  PIN_MODE_NONE, PF_NONE, 0,                2,                 3,                8, EXTI_CHANNEL_NONE},
/* B0 - NFC_PIN1 - 16 */ { NRF_PORT_0,    9,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B1 - NFC_PIN2 - 17 */ { NRF_PORT_0,    10, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B2            - 18 */ { NRF_PORT_1,    12, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B3            - 19 */ { NRF_PORT_1,    13, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B4            - 20 */ { NRF_PORT_1,    14, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B5            - 21 */ { NRF_PORT_1,    15, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C0            - 22 */ { NRF_PORT_1,    8,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C1            - 23 */ { NRF_PORT_1,    9,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C2            - 24 */ { NRF_PORT_0,    11, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C3            - 25 */ { NRF_PORT_0,    12, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C4            - 26 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C5            - 27 */ { NRF_PORT_1,    3,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 3,                8, EXTI_CHANNEL_NONE},
/* MODE BUTTON   - 28 */ { NRF_PORT_0,    25, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RGBR          - 29 */ { NRF_PORT_0,    26, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 1,                8, EXTI_CHANNEL_NONE},
/* RGBG          - 30 */ { NRF_PORT_0,    27, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 2,                8, EXTI_CHANNEL_NONE},
/* RGBB          - 31 */ { NRF_PORT_0,    7,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 3,                8, EXTI_CHANNEL_NONE},
/* TX            - 32 */ { NRF_PORT_0,    6,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RX            - 33 */ { NRF_PORT_0,    8,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* TX1           - 34 */ { NRF_PORT_0,    16, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RX1           - 35 */ { NRF_PORT_0,    15, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* CTS1          - 36 */ { NRF_PORT_0,    13, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RTS1          - 37 */ { NRF_PORT_0,    14, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGPWR         - 38 */ { NRF_PORT_1,    6,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGRST         - 39 */ { NRF_PORT_1,    7,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGVINT        - 40 */ { NRF_PORT_1,    5,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGDTR         - 41 */ { NRF_PORT_1,    4,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* LOW_BAT_UC    - 42 */ { NRF_PORT_0,    24, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 2,                8, EXTI_CHANNEL_NONE},
};

const uint8_t NRF_PIN_LOOKUP_TABLE[48] = {
    PIN_INVALID, PIN_INVALID, 9,           8,           14,          15,          32,          31,          /* P0.00 ~ P0.07 */
    33,          16,          17,          24,          25,          36,          37,          35,          /* P0.08 ~ P0.15 */
    34,          PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, /* P0.16 ~ P0.23 */
    42,          28,          29,          30,          10,          11,          12,          13,          /* P0.24 ~ P0.31 */
    PIN_INVALID, 0,           1,           27,          41,          40,          38,          39,          /* P1.00 ~ P1.07 */
    22,          23,          2,           5,           18,          19,          20,          21,          /* P1.08 ~ P1.15 */
};

#endif // PLATFORM_ID == PLATFORM_BG96

#if PLATFORM_ID == PLATFORM_EG91

static Hal_Pin_Info s_pin_map[TOTAL_PINS] = {
/* D0            - 00 */ { NRF_PORT_1,    1,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 0,                8, EXTI_CHANNEL_NONE},
/* D1            - 01 */ { NRF_PORT_1,    2,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 1,                8, EXTI_CHANNEL_NONE},
/* D2            - 02 */ { NRF_PORT_1,    10, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D3 - RESET    - 03 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D4 - SWO      - 04 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D5 -          - 05 */ { NRF_PORT_1,    11, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D6 - SWDCLK   - 06 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* D7 - SWDIO    - 07 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 0,                8, EXTI_CHANNEL_NONE},
/* A0            - 08 */ { NRF_PORT_0,    3,  PIN_MODE_NONE, PF_NONE, 1,                2,                 0,                8, EXTI_CHANNEL_NONE},
/* A1            - 09 */ { NRF_PORT_0,    2,  PIN_MODE_NONE, PF_NONE, 2,                2,                 1,                8, EXTI_CHANNEL_NONE},
/* A2            - 10 */ { NRF_PORT_0,    28, PIN_MODE_NONE, PF_NONE, 4,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A3            - 11 */ { NRF_PORT_0,    29, PIN_MODE_NONE, PF_NONE, 5,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A4            - 12 */ { NRF_PORT_0,    30, PIN_MODE_NONE, PF_NONE, 6,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A5            - 13 */ { NRF_PORT_0,    31, PIN_MODE_NONE, PF_NONE, 7,                PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* A6            - 14 */ { NRF_PORT_0,    4,  PIN_MODE_NONE, PF_NONE, 3,                2,                 2,                8, EXTI_CHANNEL_NONE},
/* A7            - 15 */ { NRF_PORT_0,    5,  PIN_MODE_NONE, PF_NONE, 0,                2,                 3,                8, EXTI_CHANNEL_NONE},
/* B0 - NFC_PIN1 - 16 */ { NRF_PORT_0,    9,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B1 - NFC_PIN2 - 17 */ { NRF_PORT_0,    10, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B2            - 18 */ { NRF_PORT_1,    12, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B3            - 19 */ { NRF_PORT_1,    13, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B4            - 20 */ { NRF_PORT_1,    14, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* B5            - 21 */ { NRF_PORT_1,    15, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C0            - 22 */ { NRF_PORT_1,    8,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C1            - 23 */ { NRF_PORT_1,    9,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C2            - 24 */ { NRF_PORT_0,    11, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C3            - 25 */ { NRF_PORT_0,    12, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C4            - 26 */ { NRF_PORT_NONE, 0,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* C5            - 27 */ { NRF_PORT_1,    3,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 3,                8, EXTI_CHANNEL_NONE},
/* MODE BUTTON   - 28 */ { NRF_PORT_0,    25, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RGBR          - 29 */ { NRF_PORT_0,    26, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 1,                8, EXTI_CHANNEL_NONE},
/* RGBG          - 30 */ { NRF_PORT_0,    27, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 2,                8, EXTI_CHANNEL_NONE},
/* RGBB          - 31 */ { NRF_PORT_0,    7,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 0,                 3,                8, EXTI_CHANNEL_NONE},
/* TX            - 32 */ { NRF_PORT_0,    6,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RX            - 33 */ { NRF_PORT_0,    8,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* TX1           - 34 */ { NRF_PORT_0,    16, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RX1           - 35 */ { NRF_PORT_0,    15, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* CTS1          - 36 */ { NRF_PORT_0,    13, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* RTS1          - 37 */ { NRF_PORT_0,    14, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGPWR         - 38 */ { NRF_PORT_1,    6,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGRST         - 39 */ { NRF_PORT_1,    7,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGVINT        - 40 */ { NRF_PORT_1,    5,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* BGDTR         - 41 */ { NRF_PORT_1,    4,  PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, PWM_INSTANCE_NONE, PWM_CHANNEL_NONE, 8, EXTI_CHANNEL_NONE},
/* LOW_BAT_UC    - 42 */ { NRF_PORT_0,    24, PIN_MODE_NONE, PF_NONE, ADC_CHANNEL_NONE, 1,                 2,                8, EXTI_CHANNEL_NONE},
};

const uint8_t NRF_PIN_LOOKUP_TABLE[48] = {
    PIN_INVALID, PIN_INVALID, 9,           8,           14,          15,          32,          31,          /* P0.00 ~ P0.07 */
    33,          16,          17,          24,          25,          36,          37,          35,          /* P0.08 ~ P0.15 */
    34,          PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, PIN_INVALID, /* P0.16 ~ P0.23 */
    42,          28,          29,          30,          10,          11,          12,          13,          /* P0.24 ~ P0.31 */
    PIN_INVALID, 0,           1,           27,          41,          40,          38,          39,          /* P1.00 ~ P1.07 */
    22,          23,          2,           5,           18,          19,          20,          21,          /* P1.08 ~ P1.15 */
};

#endif // PLATFORM_ID == PLATFORM_EG91

Hal_Pin_Info* HAL_Pin_Map(void) {
    return s_pin_map;
}
