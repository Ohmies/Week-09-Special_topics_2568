#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

// กำหนดค่าคงที่
#define LED1_GPIO GPIO_NUM_2
#define LED2_GPIO GPIO_NUM_4
#define LED3_GPIO GPIO_NUM_5
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CH0 LEDC_CHANNEL_0
#define LEDC_CH1 LEDC_CHANNEL_1
#define LEDC_CH2 LEDC_CHANNEL_2
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000
#define FADE_TIME 200     // ms for fade
#define BLINK_PERIOD 1000 // 1 วินาที (1000ms)

static const char *TAG = "PWM_LED_CONTROL";

/**
 * @brief กำหนดค่าเริ่มต้นสำหรับ PWM
 */
void pwm_init(void)
{
    ESP_LOGI(TAG, "Initializing PWM for LEDs on GPIO %d, %d, %d", LED1_GPIO, LED2_GPIO, LED3_GPIO);

    // Configure timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure channel 0
    ledc_channel_config_t ledc_channel0 = {
        .channel = LEDC_CH0,
        .duty = 0,
        .gpio_num = LED1_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel0));

    // Configure channel 1
    ledc_channel_config_t ledc_channel1 = {
        .channel = LEDC_CH1,
        .duty = 0,
        .gpio_num = LED2_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    // Configure channel 2
    ledc_channel_config_t ledc_channel2 = {
        .channel = LEDC_CH2,
        .duty = 0,
        .gpio_num = LED3_GPIO,
        .speed_mode = LEDC_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));

    // Initialize fade service
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    ESP_LOGI(TAG, "PWM initialization completed");
}

/**
 * @brief Knight Rider pattern with breathing effect
 */
void knight_rider(void)
{
    // LED1 fade in
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, 8191, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_WAIT_DONE));
    vTaskDelay(pdMS_TO_TICKS(200));

    // fade to LED2
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, 0, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, 8191, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
    vTaskDelay(pdMS_TO_TICKS(FADE_TIME));

    // fade to LED3
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, 0, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH2, 8191, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH2, LEDC_FADE_NO_WAIT));
    vTaskDelay(pdMS_TO_TICKS(FADE_TIME));

    // back to LED2
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH2, 0, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, 8191, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH2, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
    vTaskDelay(pdMS_TO_TICKS(FADE_TIME));

    // back to LED1
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, 0, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, 8191, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_NO_WAIT));
    vTaskDelay(pdMS_TO_TICKS(FADE_TIME));

    // fade out LED1
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, 0, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_WAIT_DONE));
    vTaskDelay(pdMS_TO_TICKS(200));
}

/**
 * @brief Binary counter pattern with breathing effect
 */
void binary_counter(void)
{
    for (int i = 0; i < 8; i++)
    {
        int duty1 = (i & 1) ? 8191 : 0;
        int duty2 = (i & 2) ? 8191 : 0;
        int duty3 = (i & 4) ? 8191 : 0;
        ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, duty1, FADE_TIME));
        ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, duty2, FADE_TIME));
        ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH2, duty3, FADE_TIME));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_NO_WAIT));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH2, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Random blinking pattern with breathing effect
 */
void random_blink(void)
{
    int duty1 = (rand() % 2) * 8191;
    int duty2 = (rand() % 2) * 8191;
    int duty3 = (rand() % 2) * 8191;
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH0, duty1, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH1, duty2, FADE_TIME));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CH2, duty3, FADE_TIME));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH0, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH1, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CH2, LEDC_FADE_NO_WAIT));
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
    ESP_LOGI(TAG, "ESP32 PWM LEDs Control Demo Started");
    srand(time(NULL));

    // Initialize PWM
    pwm_init();

    // Create LEDs blink task
    xTaskCreate(
        led_blink_task,   // Function pointer
        "led_blink_task", // Task name
        2048,             // Stack size (bytes)
        NULL,             // Parameters
        5,                // Priority
        NULL              // Task handle
    );

    ESP_LOGI(TAG, "PWM LEDs blink task created");
}