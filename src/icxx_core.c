#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lcd_1602_i2c.h"
#include "ssd1306_i2c.h"

// LCD1602 & SSD1306 I2C Adressen
#define LCD1602_I2C_ADDR 0x27
#define SSD1306_OLED_ADDR 0x3C

// Pin-Defines
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// FreeRTOS Prioritäten
#define LCD1602_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define SSD1306_TASK_PRIORITY   (tskIDLE_PRIORITY + 3)
#define MAIN_TASK_PRIORITY      (tskIDLE_PRIORITY + 1)

// LCD & OLED Variablen
lcd_1602_i2c_t my_lcd1602;
uint8_t ssd1306_frame_buffer[SSD1306_BUF_LEN];
struct render_area ssd1306_full_frame_area = {
    .start_col = 0,
    .end_col = SSD1306_WIDTH - 1,
    .start_page = 0,
    .end_page = SSD1306_NUM_PAGES - 1
};

// FreeRTOS Hooks (Stack Overflow, Malloc Fail)
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask; (void)pcTaskName;
    printf("Stack Overflow in Task %s!\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

void vApplicationMallocFailedHook(void) {
    printf("Malloc Failed!\n");
    taskDISABLE_INTERRUPTS();
    for(;;);
}

// LCD1602 Task
void vLcd1602Task(void *pvParameters) {
    (void)pvParameters;
    lcd_init(&my_lcd1602, I2C_PORT, LCD1602_I2C_ADDR);
    lcd_clear(&my_lcd1602);
    lcd_set_cursor(&my_lcd1602, 0, 0);
    lcd_write_string(&my_lcd1602, "Console > ");
    lcd_set_cursor(&my_lcd1602, 1, 0);
    lcd_write_string(&my_lcd1602, "Started");

    for (;;) {
        static int dot_state = 0;
        lcd_set_cursor(&my_lcd1602, 1, 15);
        lcd_write_char(&my_lcd1602, dot_state ? '.' : ' ');
        dot_state = !dot_state;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// SSD1306 Task
void vSsd1306Task(void *pvParameters) {
    (void)pvParameters;
    ssd1306_init(I2C_PORT, I2C_SDA, I2C_SCL);
    memset(ssd1306_frame_buffer, 0, SSD1306_BUF_LEN);
    ssd1306_render(ssd1306_frame_buffer, &ssd1306_full_frame_area);

    if (SSD1306_HEIGHT == 64) {
        ssd1306_write_string(ssd1306_frame_buffer, 0, 32, "BlackzCoreOS");
        ssd1306_write_string(ssd1306_frame_buffer, 0, 40, "Loading...");
    } else {
        ssd1306_write_string(ssd1306_frame_buffer, 0, 0, "BlackzCoreOS");
        ssd1306_write_string(ssd1306_frame_buffer, 0, 8, "Loading...");
    }
    ssd1306_render(ssd1306_frame_buffer, &ssd1306_full_frame_area);

    int counter = 0;
    for (;;) {
        int y_offset = (SSD1306_HEIGHT == 64) ? 40 : 8;
        memset(ssd1306_frame_buffer + (y_offset / SSD1306_PAGE_HEIGHT) * SSD1306_WIDTH, 0, SSD1306_WIDTH);
        ssd1306_write_string(ssd1306_frame_buffer, 0, y_offset, "Loading...");
        for (int i = 0; i < (counter % 4); i++) {
            ssd1306_write_char(ssd1306_frame_buffer, 8 * 9 + i * 8, y_offset, '.');
        }
        ssd1306_render(ssd1306_frame_buffer, &ssd1306_full_frame_area);
        counter++;
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// Main Task: Initialisierung SPI, I2C, UART + Ausgabe
void vMainTask(void *pvParameters) {
    (void)pvParameters;
    stdio_init_all();

    // SPI initialisieren
    spi_init(SPI_PORT, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // I2C initialisieren
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // UART initialisieren
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_puts(UART_ID, "Hello, UART from FreeRTOS MainTask!\n");

    for (;;) {
        printf("Hello from FreeRTOS MainTask\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    // Tasks erstellen
    if (xTaskCreate(vLcd1602Task, "LCD1602", configMINIMAL_STACK_SIZE * 6, NULL, LCD1602_TASK_PRIORITY, NULL) != pdPASS) {
        printf("Error: LCD1602 Task\n");
        while (1) tight_loop_contents();
    }

    if (xTaskCreate(vSsd1306Task, "SSD1306", configMINIMAL_STACK_SIZE * 16, NULL, SSD1306_TASK_PRIORITY, NULL) != pdPASS) {
        printf("Error: SSD1306 Task\n");
        while (1) tight_loop_contents();
    }

    if (xTaskCreate(vMainTask, "MainTask", 1024, NULL, MAIN_TASK_PRIORITY, NULL) != pdPASS) {
        printf("Error: MainTask\n");
        while (1) tight_loop_contents();
    }

    vTaskStartScheduler();

    // Sollte nie hier ankommen
    printf("Scheduler exited unexpectedly!\n");
    while (1) tight_loop_contents();
}
