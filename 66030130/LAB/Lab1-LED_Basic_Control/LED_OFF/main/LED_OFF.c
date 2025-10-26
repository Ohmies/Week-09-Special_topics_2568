#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO GPIO_NUM_2

void app_main(void)
{
    // กำหนด GPIO 2 เป็น output
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // เปิด LED ก่อน (HIGH = 3.3V)
    gpio_set_level(LED_GPIO, 1);
    ESP_LOGI("LED", "LED is ON for 3 seconds...");

    // รอ 3 วินาที
    vTaskDelay(pdMS_TO_TICKS(3000));

    // ปิด LED (LOW = 0V)
    gpio_set_level(LED_GPIO, 0);
    ESP_LOGI("LED", "LED is OFF!");

    // ป้องกันโปรแกรมจบทันที (รอ forever)
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}