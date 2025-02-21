# ControleLEDsADC
O algoritmo implementa o controle de alguns LEDs utilizando um joystick e Botões além de animação para visualização em um display ssd1306.

## Funcionalidades

- De acordo com a posição do joystick os leds azul e vermelho aumentam a intensidade, chegando ao ápice ao nas extremidades.
- De acordo com a posição do joystick um quadrado se move no display.
- Ao apertar o Botão A os leds azul e vemelho são ativados/desativados.
- Ao apertar o Joystick o led verde é ativado/desativado.
- Ao apertar o Joystick o tamanho e cor da borda do display muda.

## Hardware Necessário

- 1 Raspberry Pi Pico W (rp2040).
- 3 LEDs (Vermelho, Azul, Verde).
- Display ssd1306 128 x 64.
- Joysctick
- Resistores apropriados para os LEDs.
- Fios de conexão.

## Pinagem

- Display SDA 14
- Display SCL 15
- Botão A 5
- Botão do Joystick 22
- Joystick X 26
- Joystick Y 27
- LED Vermelho: Pino GPIO 13
- LED Azul: Pino GPIO 12
- LED Verde: Pino GPIO 11

## Configuração do Ambiente

Antes de começar, certifique-se de que você tenha o ambiente de desenvolvimento do **Raspberry Pi Pico** configurado corretamente. Siga as instruções do [Raspberry Pi Pico SDK](https://www.raspberrypi.org/documentation/rp2040/getting-started/) para configurar o SDK e as ferramentas de compilação.
## Compilação e Upload

1. Compile o código usando o ambiente de desenvolvimento configurado.
2. Após a compilação, faça o upload do código para o seu **Raspberry Pi Pico**.

## Tecnologias Utilizadas

- **Conversor Analógico Digital**
- **Comunicação I2C**
- **C/C++**
- **Raspberry Pi Pico SDK**

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir um problema ou enviar um pull request.
