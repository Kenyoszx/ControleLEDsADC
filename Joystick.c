// Inclusão das Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definição dos Pinos

#define I2C_SDA 14
#define I2C_SCL 15
#define JOYSTICK_X_PIN 26 // GPIO para eixo X
#define JOYSTICK_Y_PIN 27 // GPIO para eixo Y
#define JOYSTICK_PB 22    // GPIO para botão do Joystick
#define BUTTON_A 5         // GPIO para botão A
#define LED_PIN_GREEN 11
#define LED_PIN_BLUE 12
#define LED_PIN_RED 13

//Constantes
#define I2C_PORT i2c1
#define endereco 0x3C
#define WRAP_PERIOD 20000
#define PWM_DIVISOR 125.0
#define MOTOR_STEP 5

//Variáveis Globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

//Protótipos das Funções
refresh_led_state(uint16_t x,uint16_t y);
uint init_PWM_Red_LED();
uint init_PWM_Blue_LED();
void wrapHandlerRED();
void wrapHandlerBLUE();
int main()
{
    stdio_init_all();
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

    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    uint16_t adc_value_x;
    uint16_t adc_value_y;

    bool cor = true;
    while (true)
    {
        adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read();
        adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read();
        refresh_led_state(adc_value_x,adc_value_y);
        sleep_ms(100);
    }
}

refresh_led_state(uint16_t x,uint16_t y){

}
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Configura a ação ao apertar o botão e implementa o Debouce

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 50000) // 49 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        // Código Função:
        if (gpio == BUTTON_A)
        {
            
        }
        else if (gpio == JOYSTICK_PB)
        {
            
        }
    }
}
uint init_PWM_Red_LED(){
    // inicializa o pwm do ServoMotor

    gpio_set_function(LED_PIN_RED, GPIO_FUNC_PWM);        // habilitar o pino GPIO como PWM
    uint slice = pwm_gpio_to_slice_num(LED_PIN_RED); // obter o canal PWM da GPIO

    pwm_clear_irq(slice);                            // Reseta a flag de interrupção para o slice do motor
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrapHandlerRED); // interrupção quando o contador do slice atinge o wrap
    irq_set_enabled(PWM_IRQ_WRAP, true);                  // Habilitar ou desabilitar uma interrupção específica

    pwm_set_clkdiv(slice, PWM_DIVISOR); // define o divisor de clock do PWM
    pwm_set_wrap(slice, WRAP_PERIOD);   // definir o valor de wrap
    pwm_set_gpio_level(LED_PIN_RED, 0);     // definir o ciclo de trabalho (duty cycle) do pwm
    pwm_set_enabled(slice, true);       // habilita o pwm no slice correspondente

    return (slice);
}
uint init_PWM_Blue_LED(){
    // inicializa o pwm do ServoMotor

    gpio_set_function(LED_PIN_BLUE, GPIO_FUNC_PWM);        // habilitar o pino GPIO como PWM
    uint slice = pwm_gpio_to_slice_num(LED_PIN_BLUE); // obter o canal PWM da GPIO

    pwm_clear_irq(slice);                            // Reseta a flag de interrupção para o slice do motor
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrapHandlerBLUE); // interrupção quando o contador do slice atinge o wrap
    irq_set_enabled(PWM_IRQ_WRAP, true);                  // Habilitar ou desabilitar uma interrupção específica

    pwm_set_clkdiv(slice, PWM_DIVISOR); // define o divisor de clock do PWM
    pwm_set_wrap(slice, WRAP_PERIOD);   // definir o valor de wrap
    pwm_set_gpio_level(LED_PIN_BLUE, 0);     // definir o ciclo de trabalho (duty cycle) do pwm
    pwm_set_enabled(slice, true);       // habilita o pwm no slice correspondente

    return (slice);
}
