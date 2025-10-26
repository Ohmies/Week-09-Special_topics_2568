#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// กำหนดค่าคงที่
#define LED1_GPIO GPIO_NUM_2
#define LED2_GPIO GPIO_NUM_4
#define LED3_GPIO GPIO_NUM_5
#define BLINK_PERIOD 1000 // 1 วินาที (1000ms)

static const char *TAG = "LED_CONTROL";

/**
 * @brief กำหนดค่าเริ่มต้นสำหรับ GPIO
 */
void led_init(void)
{
    ESP_LOGI(TAG, "Initializing LEDs on GPIO %d, %d, %d", LED1_GPIO, LED2_GPIO, LED3_GPIO);

    // 1️⃣ สร้าง GPIO configuration structure
    gpio_config_t io_conf = {0};

    // 2️⃣ กำหนด pin ที่ต้องการใช้ (bit mask)
    io_conf.pin_bit_mask = (1ULL << LED1_GPIO) | (1ULL << LED2_GPIO) | (1ULL << LED3_GPIO);

    // 3️⃣ กำหนดเป็น output mode
    io_conf.mode = GPIO_MODE_OUTPUT;

    // 4️⃣ ปิด pull-up/pull-down (ไม่จำเป็นสำหรับ output)
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    // 5️⃣ ปิด interrupt (ไม่ใช้ในการควบคุม LED)
    io_conf.intr_type = GPIO_INTR_DISABLE;

    // 6️⃣ Apply configuration
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // 7️⃣ Set initial state (LEDs OFF)
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 0);

    ESP_LOGI(TAG, "LEDs initialization completed");
}

/**
 * @brief Knight Rider pattern
 */
void knight_rider(void)
{
    // ไปขวา
    gpio_set_level(LED1_GPIO, 1);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 1);
    gpio_set_level(LED3_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    // กลับซ้าย
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 1);
    gpio_set_level(LED3_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LED1_GPIO, 1);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
}

/**
 * @brief Binary counter pattern
 */
void binary_counter(void)
{
    for (int i = 0; i < 8; i++)
    {
        gpio_set_level(LED1_GPIO, (i & 1) ? 1 : 0);
        gpio_set_level(LED2_GPIO, (i & 2) ? 1 : 0);
        gpio_set_level(LED3_GPIO, (i & 4) ? 1 : 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Random blinking pattern
 */
void random_blink(void)
{
    gpio_set_level(LED1_GPIO, rand() % 2);
    gpio_set_level(LED2_GPIO, rand() % 2);
    gpio_set_level(LED3_GPIO, rand() % 2);
    vTaskDelay(pdMS_TO_TICKS(300));
}

/**
 * @brief Task สำหรับกระพริบ LEDs
 */
void led_blink_task(void *pvParameters)
{
    while (1)
    {
        knight_rider();
        binary_counter();
        for (int i = 0; i < 10; i++)
            random_blink();
    }
}

/**
 * @brief Main application entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 LEDs Control Demo Started");
    srand(time(NULL));

    // Initialize LEDs
    led_init();

    // Create LEDs blink task
    xTaskCreate(
        led_blink_task,   // Function pointer
        "led_blink_task", // Task name
        2048,             // Stack size (bytes)
        NULL,             // Parameters
        5,                // Priority
        NULL              // Task handle
    );

    ESP_LOGI(TAG, "LEDs blink task created");
}