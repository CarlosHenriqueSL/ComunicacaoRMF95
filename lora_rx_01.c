/* EMBARCATECH - INTRODUÇÃO AO PROTOCOLO LORA - CAPÍTULO 3 / PARTE 4
 * Receptor LoRa (TX) - BitDogLAB
 * Comunicação com módulo LoRa (Heltec WiFi LoRa 32 V3.2) via protocolo UART para recepção de informações.
 * Prof: Ricardo Prates
 */

#include <stdio.h>
#include "pico/stdlib.h"

#define UART_ID uart1 // Seleciona a UART0
#define BAUD_RATE 115200 // Define a taxa de transmissão
#define UART_TX_PIN 8 // Pino GPIO usado para TX
#define UART_RX_PIN 9 // Pino GPIO usado para RX
#define LED_PIN 11 // Pino GPIO usado para o LED

// Função Principal
int main() {

    // Inicializa a biblioteca padrão
    stdio_init_all();

    // Inicializa a UART
    uart_init(UART_ID, BAUD_RATE);

    // Configura os pinos GPIO para a UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Configura o pino do LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    char buffer[100];  // Buffer para armazenar a mensagem
    int index = 0;     // Índice para armazenar caracteres no buffer

    // Loop principal
    while (1) {
        
        // Verifica se há dados disponíveis para leitura
        if (uart_is_readable(UART_ID)) {
            // Lê o caractere recebido
            char c = uart_getc(UART_ID);

            // Adiciona o caractere ao buffer se não for '\n'
            if (c != '\n') {
                buffer[index++] = c;

                // Garante que o índice não ultrapasse o tamanho do buffer
                if (index >= sizeof(buffer) - 1) {
                    index = 0; // Reinicia o índice se o buffer estiver cheio
                }
            } else {
                // Termina a string e processa a mensagem
                buffer[index] = '\0';
                index = 0; // Reinicia o índice para a próxima mensagem

                // Exibe a mensagem recebida
                printf("Mensagem recebida: %s\n", buffer);

                // Pisca o LED para indicar recebimento
                gpio_put(LED_PIN, 1);
                sleep_ms(50);
                gpio_put(LED_PIN, 0);
                sleep_ms(50);
            }
        }

        // Pequeno atraso para evitar sobrecarga no loop principal
        sleep_ms(10);
    }

    return 0;
}
