#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#define BUTTON_GPIO_PIN 5

#define STATUS_TIME_MS 3000

#define DEBOUNCING_TIME_MS 200

uint8_t leds_gpio_pins[] = {12, 13, 11}; // ordenados pela sequência de desligamento

uint8_t active_leds = 0;
uint64_t last_time = 0;

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    
    if (active_leds)
    {
        gpio_put(leds_gpio_pins[3-active_leds], false);
        active_leds--;
    }

    if (active_leds) add_alarm_in_ms(STATUS_TIME_MS, turn_off_callback, NULL, false);

    return 0;
}

int main() {
    stdio_init_all();

    // inicializa pinos dos LEDs
    for (uint8_t i = 0; i < 3; i++)
    {
        gpio_init(leds_gpio_pins[i]);
        gpio_set_dir(leds_gpio_pins[i], GPIO_OUT);
        gpio_put(leds_gpio_pins[i], false);
    }

    // inicializa pino do botão
    gpio_init(BUTTON_GPIO_PIN);
    gpio_set_dir(BUTTON_GPIO_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO_PIN);

    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        // trata o efeito bouncing com um atraso de DEBOUNCING_TIME_MS milissegundos
        if(current_time - last_time > DEBOUNCING_TIME_MS)
        {
            bool is_button_pressed = !gpio_get(BUTTON_GPIO_PIN);

            // se o botão estiver pressionado e todos os LEDs estiverem apagados
            if (is_button_pressed && !active_leds)
            {
                last_time = current_time;

                // liga os LEDs
                for(uint8_t i = 0; i < 3; i++) gpio_put(leds_gpio_pins[i], true);
                active_leds = 3;

                // programa alarme para desligar o primeiro LED
                add_alarm_in_ms(STATUS_TIME_MS, turn_off_callback, NULL, false);
            }
        }
    }

    return 0;
}