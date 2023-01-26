#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "LoRa_E32.h"

#include <iostream>

#include "hardware/uart.h"

#include "tusb.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

void core1_entry() {
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
}

int main() {
    stdio_init_all();
    multicore_launch_core1(core1_entry);

    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uint32_t t0,t1;

    t0 = time_us_32();
    while (!tud_cdc_connected()) {}
    t1 = time_us_32();
    printf("\nusb host detected! (%dus)\n", t1-t0);

    LoRa_E32 lora(UART_ID, 4, 2, 3, UART_BPS_RATE_9600);

    bool l = lora.begin();
    printf("%d\n\r", l);

    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
    };

    {
        auto conf = lora.getConfiguration();
        printf("%s\n\r", conf.status.getResponseDescription().c_str());
        Configuration *s = (Configuration *)conf.data;

        for(int i = 0 ; i < sizeof(Configuration); i ++){
            printf("0x%02x ", ((uint8_t*)(conf.data))[i]);
        }

        printf("\r\n");
        printf("add L:0x%02x H:0x%02x\r\n", s->ADDL, s->ADDH);

        s->SPED.uartBaudRate = UART_BPS_115200;
        s->SPED.airDataRate = AIR_DATA_RATE_000_03;
        s->CHAN = 38;

        auto res = lora.setConfiguration(*s, WRITE_CFG_PWR_DWN_SAVE);
        printf("%s\n\r", res.getResponseDescription().c_str());
    }


    {
        auto conf = lora.getConfiguration();
        printf("%s\n\r", conf.status.getResponseDescription().c_str());
        Configuration *s = (Configuration *)conf.data;

        for(int i = 0 ; i < sizeof(Configuration); i ++){
            printf("0x%02x ", ((uint8_t*)(conf.data))[i]);
        }
        
        printf("\r\n");
    }



   


	uart_set_baudrate(UART_ID, 115200);


    char data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    while(1){
        printf("Send Data\r\n");
        t0 = time_us_32();
        auto res = lora.sendMessage(data, 6);
        t1 = time_us_32();
        printf("%s\n\r", res.getResponseDescription().c_str());
        printf("\nSend Data Done: (%dms) %s\r\n", (t1-t0) / 1000);
        sleep_ms(250);
    }

    return 0;
}
