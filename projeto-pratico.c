#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "include/ssd1306.h"

// Definições de pinos
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13
#define BOTAO_A 5
#define BUZZER_PIN 21

// Configuração do I2C para o OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDRESS 0x3C

// Constantes de tempo (em microsegundos)
#define WAIT_TIME 30000000UL
#define ALARM_TIME 10000000UL
#define ALERT_TIME 5000000UL

// Frequências para o buzzer
#define ALARM_FREQUENCY 2000
#define ALERT_FREQUENCY 1500

// Estados do sistema
typedef enum
{
    MODE_WAIT,
    MODE_ALARM_ACTIVE,
    MODE_ALERT
} SystemState;

SystemState current_state = MODE_WAIT;
uint32_t state_start_time = 0;

// Configuração dos GPIOs
void setup_gpio()
{
    // Botão como entrada com pull-up (pressionado = 0)
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    // Configura o buzzer para uso com PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice_num, false);

    // Configura o LED vermelho (alerta) como saída
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0);

    // Configura o LED verde como saída
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);
}

// Configura o I2C e o display OLED
void setup_oled(ssd1306_t *display)
{
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(display, 128, 64, false, OLED_ADDRESS, I2C_PORT);
    ssd1306_config(display);
    ssd1306_fill(display, false);
    ssd1306_send_data(display);
}

// Controla o buzzer via PWM para gerar um tom com frequência desejada
void start_buzzer_tone(uint frequency)
{
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel = pwm_gpio_to_channel(BUZZER_PIN);

    float divider = 125.0f;
    uint32_t wrap = (uint32_t)((125000000.0f / (divider * frequency)) - 1);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_clkdiv(slice_num, divider);

    uint32_t level = (wrap + 1) / 2;
    pwm_set_chan_level(slice_num, channel, level);
    pwm_set_enabled(slice_num, true);
}

void stop_buzzer_tone()
{
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice_num, false);
}

bool is_time_elapsed(uint32_t start_time, uint32_t duration_us)
{
    return time_us_32() - start_time >= duration_us;
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    printf("Sistema iniciado.\n");

    setup_gpio();
    ssd1306_t display;
    setup_oled(&display);

    state_start_time = time_us_32();

    while (1)
    {
        uint32_t current_time = time_us_32();

        if (!gpio_get(BOTAO_A))
        {
            sleep_ms(20);

            if (!gpio_get(BOTAO_A))
            {
                gpio_put(LED_VERDE, 1);
                stop_buzzer_tone();

                absolute_time_t start = get_absolute_time();
                while (!gpio_get(BOTAO_A) && absolute_time_diff_us(start, get_absolute_time()) < 1000000)
                {
                    sleep_ms(10);
                }
                gpio_put(LED_VERDE, 0);
                current_state = MODE_WAIT;
                state_start_time = time_us_32();
                stop_buzzer_tone();
                gpio_put(LED_VERMELHO, 0);
                printf("Reiniciado pelo botão.\n");

                ssd1306_fill(&display, false);
                ssd1306_draw_string(&display, "Reiniciado!", 10, 10);
                ssd1306_send_data(&display);
            }
        }

        switch (current_state)
        {
        case MODE_WAIT:
            if (is_time_elapsed(state_start_time, WAIT_TIME))
            {
                current_state = MODE_ALARM_ACTIVE;
                state_start_time = time_us_32();
                printf("Ativando alarme.\n");
            }
            break;

        case MODE_ALARM_ACTIVE:
            start_buzzer_tone(ALARM_FREQUENCY);
            gpio_put(LED_VERMELHO, 1);
            if (is_time_elapsed(state_start_time, ALARM_TIME))
            {
                current_state = MODE_ALERT;
                state_start_time = time_us_32();
                printf("Alerta!\n");
            }
            break;

        case MODE_ALERT:

            uint32_t elapsed = current_time - state_start_time;
            if ((elapsed % 1000000) < 500000)
            {
                gpio_put(LED_VERMELHO, 1);
                start_buzzer_tone(ALERT_FREQUENCY);
            }
            else
            {
                gpio_put(LED_VERMELHO, 0);
                stop_buzzer_tone();
            }
            if (is_time_elapsed(state_start_time, ALERT_TIME))
            {
                current_state = MODE_WAIT;
                state_start_time = time_us_32();
                printf("Retornando à espera.\n");
            }
            break;
        }

        ssd1306_fill(&display, false);
        char msg[30];
        switch (current_state)
        {
        case MODE_WAIT:
            ssd1306_draw_string(&display, "Sistema Ativo", 10, 10);
            ssd1306_draw_string(&display, "Aguardando...", 10, 30);
            break;
        case MODE_ALARM_ACTIVE:
        {
            uint32_t elapsed = (time_us_32() - state_start_time) / 1000000;
            int remaining = 10 - elapsed;
            if (remaining < 0)
                remaining = 0;
            snprintf(msg, sizeof(msg), "Pressione! %ds", remaining);
            ssd1306_draw_string(&display, msg, 10, 10);
            break;
        }
        case MODE_ALERT:
            ssd1306_draw_string(&display, "ALERTA!", 30, 10);
            ssd1306_draw_string(&display, "Sem resposta!", 10, 30);
            break;
        }
        ssd1306_send_data(&display);

        sleep_ms(50);
    }

    return 0;
}
