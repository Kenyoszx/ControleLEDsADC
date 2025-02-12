// Inclusão das Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Definição dos Pinos

#define I2C_SDA 14        // SDA display
#define I2C_SCL 15        // SCL display
#define JOYSTICK_X_PIN 26 // eixo X
#define JOYSTICK_Y_PIN 27 // eixo Y
#define JOYSTICK_PB 22    // botão do Joystick
#define BUTTON_A 5        // botão A
#define LED_PIN_GREEN 11  // led verde
#define LED_PIN_BLUE 12   // led azul
#define LED_PIN_RED 13    // led vermelho

// Constantes
#define I2C_PORT i2c1
#define endereco 0x3C
#define WRAP_PERIOD 4096
#define STEP 5

// Variáveis Globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile uint slice_red, slice_blue;
static volatile bool pwm_state = true;

// Protótipos das Funções
void init();
void refresh_led_state_X(uint16_t x);
void refresh_led_state_Y(uint16_t y);
static void gpio_irq_handler(uint gpio, uint32_t events);
uint pwm_init_gpio(uint gpio, uint wrap);

int main()
{
    stdio_init_all(); // inicialização entrada e saída

    // Variáveis
    uint16_t adc_value_x;
    uint16_t adc_value_y;
    uint16_t pos_x;
    slice_red = pwm_init_gpio(LED_PIN_RED, WRAP_PERIOD);
    slice_blue = pwm_init_gpio(LED_PIN_BLUE, WRAP_PERIOD);
    bool cor = true;

    init(); // Inicialização dos Pinos

    // Inicialização do I2C

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    ssd1306_fill(&ssd, false);                                    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(&ssd);

    // Inicialização ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    while (true)
    {
        adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read();
        adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read();
        refresh_led_state_X(adc_value_x);
        refresh_led_state_Y(adc_value_y);
        pos_x = ((adc_value_x * -1) + 4095);
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd,pos_x/64,adc_value_y/32,8,8,true,true);
        ssd1306_send_data(&ssd);

        sleep_ms(100);
    }
}

void refresh_led_state_X(uint16_t x)
{

    uint16_t Position = x;
    static bool rise;
    if (Position >= 2048)
        rise = true;
    else
        rise = false;

    if (rise)
        Position = (Position % 2048);
    else
        Position = ((Position * -1) + 2048);

    pwm_set_gpio_level(LED_PIN_RED, Position * 2); // define o ciclo ativo (Ton)
}
void refresh_led_state_Y(uint16_t y)
{
    uint16_t Position = y;
    static bool rise;
    if (Position >= 2048)
        rise = true;
    else
        rise = false;

    if (rise)
        Position = (Position % 2048);
    else
        Position = ((Position * -1) + 2048);

    pwm_set_gpio_level(LED_PIN_BLUE, Position * 2); // define o ciclo ativo (Ton)
}
void init()
{
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_put(LED_PIN_GREEN, 0);
}
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Configura a ação ao apertar o botão e implementa o Debouce

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        // Código Função:
        if (gpio == BUTTON_A)
        {
            pwm_state = !pwm_state;
            pwm_set_enabled(slice_blue,pwm_state);
            pwm_set_enabled(slice_red,pwm_state);
            
        }
        else if (gpio == JOYSTICK_PB)
        {
            uint count = 0;
            bool state = gpio_get(LED_PIN_GREEN);

            gpio_put(LED_PIN_GREEN, !state); // Alterna o Estado do LED Verde

            if (count >=3)
                count = 0;
            switch (count)
            {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            
            default:
                printf("[ERRO]");
                break;
            }
            count ++;
            
        }
    }
}
uint pwm_init_gpio(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true);
    return slice_num;
}