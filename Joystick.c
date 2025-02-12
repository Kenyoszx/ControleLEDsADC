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
#define I2C_PORT i2c1    // porta i2c
#define endereco 0x3C    // endereço da porta idc
#define WRAP_PERIOD 4096 // Periodo de wrap

// Variáveis Globais
static volatile uint32_t last_time = 0;                                    // Armazena o tempo do último evento (em microssegundos)
static volatile uint slice_red, slice_blue, lr_limit = 125, ud_limit = 60; // Armazena o Numero dos Slices dos LEDs PWM
static volatile bool pwm_state = true, cor = true,board = true;                                     // Informa se os leds pwm estão ativados

// Protótipos das Funções
void init();                                              // inicializa o led verde e os botões
uint pwm_init_gpio(uint gpio, uint wrap);                 // inicializa os LEDs PWM
void refresh_led_state_X(uint16_t x);                     // Muda o estado do LED vermelho de Acordo com X do Joystick
void refresh_led_state_Y(uint16_t y);                     // Muda o estado do LED azul de Acordo com Y do Joystick
static void gpio_irq_handler(uint gpio, uint32_t events); // Trata a interrupção Ao apertar um botão

int main()
{
    stdio_init_all(); // inicialização entrada e saída

    // Variáveis
    uint16_t adc_value_x;                                  // Armazena o valor digital de x
    uint16_t adc_value_y;                                  // Armazena o valor digital de y
    uint16_t pos_x;                                        // Armazena a posição de X no display
    uint16_t pos_y;                                        // Armazena a posição de Y no display
    slice_red = pwm_init_gpio(LED_PIN_RED, WRAP_PERIOD);   // Inicializa o LED PWM Vermelho
    slice_blue = pwm_init_gpio(LED_PIN_BLUE, WRAP_PERIOD); // Inicializa o LED Azul


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
        adc_select_input(0);      // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read(); // lê o valor de X
        adc_select_input(1);      // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read(); // lê o valor de Y

        refresh_led_state_X(adc_value_x); // Muda o estado do led de acordo com x
        refresh_led_state_Y(adc_value_y); // Muda o estado do led de acordo com y

        pos_x = (((adc_value_x * -1) + 4095) / 64); // configura a posição X do quadrado no display
        pos_y = (adc_value_y / 32);                 // configura a posição Y do quadrado no display

        // Ajustes para que o Quadrado não passe da borda
        if (pos_x > 50)
            pos_x = 50;
        else if (pos_x < 8)
            pos_x = 8;
        if (pos_y > 115)
            pos_y = 115;
        else if (pos_y < 8)
            pos_y = 8;

        ssd1306_fill(&ssd, !cor);                                // limpa o buffer do display
        ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, cor, true);        // gera o quadrado 8x8 no buffer
        ssd1306_rect(&ssd, 3, 3, lr_limit, ud_limit, cor, false); // Desenha um retângulo de borda no Buffer
        ssd1306_send_data(&ssd);                                  // envia os dados do buffer ao display

        sleep_ms(100); // aguarda 100ms
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

    pwm_set_gpio_level(LED_PIN_BLUE, Position * 2); // define o ciclo ativo (Ton)
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

    pwm_set_gpio_level(LED_PIN_RED, Position * 2); // define o ciclo ativo (Ton)
}
void init()
{
    // inicializa o botão do joystick e a interrupção
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    // inicializa o botão A e a interrupção
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    // inicializa o LED verde
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
            // Altera o estado de Funcionamento dos LEDs PWM
            pwm_state = !pwm_state;
            pwm_set_enabled(slice_blue, pwm_state);
            pwm_set_enabled(slice_red, pwm_state);
        }
        else if (gpio == JOYSTICK_PB)
        {
            // Alterna o Estado do LED Verde
            bool state = gpio_get(LED_PIN_GREEN);
            gpio_put(LED_PIN_GREEN, !state); 

            // Muda o tamanho da borda e a cor
            board = !board;
            if (board){
                cor = true;
                lr_limit = 125;
                ud_limit = 60;
            }else{
                cor = false;
                ud_limit = 58;
                lr_limit = 122;
            }
        }
    }
}
uint pwm_init_gpio(uint gpio, uint wrap)
{
    // Inicializa um LED como PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true);
    return slice_num;
}