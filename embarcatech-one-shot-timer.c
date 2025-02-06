#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define RED_GPIO_PIN   13
#define BLUE_GPIO_PIN  12
#define GREEN_GPIO_PIN 11

#define BUTTON_GPIO_PIN 5

#define STATUS_TIME_MS 3000

#define DEBOUNCING_TIME_MS 500

bool is_red_active = false;
bool is_green_active = false;
bool is_blue_active = false;

int64_t turn_off_red_callback(alarm_id_t id, void *user_data) {
    gpio_put(RED_GPIO_PIN, false);
    is_red_active = false;
    return 0;
}

int64_t turn_off_green_callback(alarm_id_t id, void *user_data) {
    gpio_put(GREEN_GPIO_PIN, false);
    is_green_active = false;
    return 0;
}

int64_t turn_off_blue_callback(alarm_id_t id, void *user_data) {
    gpio_put(BLUE_GPIO_PIN, false);
    is_blue_active = false;
    return 0;
}

uint64_t last_time = 0;

int main() {
    stdio_init_all();

    // inicializa pinos
    gpio_init(RED_GPIO_PIN);
    gpio_init(BLUE_GPIO_PIN);
    gpio_init(GREEN_GPIO_PIN);
    gpio_set_dir(RED_GPIO_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_GPIO_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_GPIO_PIN, GPIO_OUT);

    gpio_init(BUTTON_GPIO_PIN);
    gpio_set_dir(BUTTON_GPIO_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO_PIN);

    while (true) {
        bool is_button_pressed = !gpio_get(BUTTON_GPIO_PIN);

        // se o botão estiver pressionado E cada um dos LEDs estiver apagado
        if (is_button_pressed && !is_red_active && !is_green_active && !is_blue_active) {
            uint64_t current_time = to_ms_since_boot(get_absolute_time());

            // trata o efeito bouncing com um atraso de DEBOUNCING_TIME_MS milissegundos
            if (current_time - last_time > DEBOUNCING_TIME_MS)
            {
                last_time = current_time;

                // liga os LEDs
                gpio_put(RED_GPIO_PIN, true);
                gpio_put(GREEN_GPIO_PIN, true);
                gpio_put(BLUE_GPIO_PIN, true);

                // atualiza status de cada um dos LEDs
                is_red_active = true;
                is_green_active = true;
                is_blue_active = true;

                // programa alarmes para desligar cada LED de forma independente
                add_alarm_in_ms(STATUS_TIME_MS, turn_off_blue_callback, NULL, false);
                add_alarm_in_ms(STATUS_TIME_MS*2, turn_off_red_callback, NULL, false);
                add_alarm_in_ms(STATUS_TIME_MS*3, turn_off_green_callback, NULL, false);
            }
        }
    }

    return 0;
}