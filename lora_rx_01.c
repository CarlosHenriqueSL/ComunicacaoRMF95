#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Definições dos pinos
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20 // Pino de RESET do RFM95

// Endereços dos Registradores do RFM95
#define REG_OP_MODE 0x01
#define REG_VERSION 0x42

// Função de Reset do Módulo
void rmf95_reset() {
    // Garante que o pino de reset esteja em estado alto (não resetado)
    gpio_put(PIN_RST, 1);
    sleep_ms(1);
    // Pulso de reset: baixo por um curto período e depois alto
    gpio_put(PIN_RST, 0);
    sleep_ms(1);
    gpio_put(PIN_RST, 1);
    sleep_ms(5); // Aguarda o módulo estabilizar após o reset
}

// Função para escrever em um registrador
void rmf95_write_reg(uint8_t reg, uint8_t value) {
    uint8_t tx_data[] = { reg | 0x80, value }; // MSB=1 para escrita

    gpio_put(PIN_CS, 0); // Ativa o chip select
    spi_write_blocking(SPI_PORT, tx_data, 2);
    gpio_put(PIN_CS, 1); // Desativa o chip select
}

// Função para ler um registrador
uint8_t rmf95_read_reg(uint8_t reg) {
    uint8_t tx_data[] = { reg & 0x7F, 0x00 }; // MSB=0 para leitura, 0x00 é um byte dummy
    uint8_t rx_data[2];

    gpio_put(PIN_CS, 0); // Ativa o chip select
    spi_write_read_blocking(SPI_PORT, tx_data, rx_data, 2);
    gpio_put(PIN_CS, 1); // Desativa o chip select

    return rx_data[1]; // O segundo byte recebido contém o valor do registrador
}

int main() {
    stdio_init_all();
    sleep_ms(2000); // Um tempo para o terminal serial conectar
    printf("Iniciando comunicacao com RFM95...\n");

    // 1. Inicializa o SPI
    spi_init(SPI_PORT, 500 * 1000); // 500 kHz é uma velocidade segura para começar
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Configura o modo SPI (Modo 0: CPOL=0, CPHA=0)
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // 2. Inicializa o pino Chip Select (CS)
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // CS em estado alto (desativado)

    // 3. Inicializa e controla o pino de RESET
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    sleep_ms(10000);
    
    // Reseta o módulo
    rmf95_reset();
    
    // Testa a comunicação lendo o registrador de versão
    // O valor esperado para um RFM95/SX1276 é 0x12
    uint8_t version = rmf95_read_reg(REG_VERSION);
    printf("Versao do RFM95: 0x%02X\n", version);

    if (version == 0x12) {
        printf("Comunicacao SPI com RFM95 OK! ✅\n");
    } else {
        printf("Falha na comunicacao SPI. ❌\n");
        printf("Verifique a fiacao, alimentacao e o pino de reset.\n");
        // Trava aqui se a comunicação falhar
        while(1);
    }
    
    // Coloca o rádio em modo LoRa e Sleep para começar
    rmf95_write_reg(REG_OP_MODE, 0x80);

    while (1) {
        uint8_t opmode = rmf95_read_reg(REG_OP_MODE);
        printf("RegOpMode: 0x%02X\n", opmode);
        sleep_ms(1000);
    }

    return 0;
}