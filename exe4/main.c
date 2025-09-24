/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

const int PIN_LED_B = 4;
const int ADC_PIN = 28;
const int ADC_INPUT = 2;

const float conversion_factor = 3.3f / (1 << 12);

const int SYSTEM_TICK_MS = 10;

typedef struct {
    volatile int toggle_interval_ms;
    volatile int tick_counter;
} TimerData;

bool timer_callback(struct repeating_timer *t) {
    TimerData *data = (TimerData *)t->user_data;

    if (data->toggle_interval_ms == 0) {
        return true;
    }

    data->tick_counter += SYSTEM_TICK_MS;

    if (data->tick_counter >= data->toggle_interval_ms) {
        gpio_put(PIN_LED_B, !gpio_get(PIN_LED_B));
        data->tick_counter = 0;
    }
    
    return true;
}

/**
 * 0..1.0V: Desligado
 * 1..2.0V: 150 ms
 * 2..3.3V: 400 ms
 */
int main() {
    // stdio_init_all();

    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    gpio_put(PIN_LED_B, 0);

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    TimerData timer_data = {0};

    struct repeating_timer timer;
    add_repeating_timer_ms(SYSTEM_TICK_MS, timer_callback, &timer_data, &timer);

    while (1) {
        uint16_t adc_raw = adc_read();
        float voltage = adc_raw * conversion_factor;

        int new_interval = 0;

        if (voltage > 2.0f) {
            new_interval = 500 / 2;
        } else if (voltage > 1.0f) {
            new_interval = 300 / 2;
        }

        timer_data.toggle_interval_ms = new_interval;

        if (timer_data.toggle_interval_ms == 0) {
            gpio_put(PIN_LED_B, 0);
            timer_data.tick_counter = 0;
        }
    }
}