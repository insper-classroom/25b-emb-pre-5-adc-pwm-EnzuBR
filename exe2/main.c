#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"

const int LED_R_PIN = 4;
const int LED_G_PIN = 6;

void led_task(void *p) {
    gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);

    uint slice_num_r = pwm_gpio_to_slice_num(LED_R_PIN);
    pwm_set_clkdiv(slice_num_r, 125);
    pwm_set_wrap(slice_num_r, 100);
    pwm_set_chan_level(slice_num_r, PWM_CHAN_A, 80);
    pwm_set_enabled(slice_num_r, true);

    uint slice_num_g = pwm_gpio_to_slice_num(LED_G_PIN);
    pwm_set_clkdiv(slice_num_g, 125);
    pwm_set_wrap(slice_num_g, 100);
    pwm_set_chan_level(slice_num_g, PWM_CHAN_A, 20);
    pwm_set_enabled(slice_num_g, true);

    while (true) {
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    xTaskCreate(led_task, "LED_Task 1", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}