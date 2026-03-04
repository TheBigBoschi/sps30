/* sps30 I2C library

   The example shows how to set up an sds30 sensor to read the particulate values.
*/
#include "sps30.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "SPS30";

// I2C settings - Change these to match your setup
#define I2C_PORT        I2C_NUM_0
#define SDA_GPIO        GPIO_NUM_23
#define SCL_GPIO        GPIO_NUM_22
#define I2C_FREQ_HZ     100000


void app_main(void)
{
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    esp_err_t ret;

    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
    ESP_LOGI(TAG, "I2C bus initialized on port %d", I2C_PORT);

    ESP_ERROR_CHECK(sps30_init(bus_handle, &dev_handle, I2C_FREQ_HZ));
    ESP_LOGI(TAG, "SPS30 device initialized.");

    ret = sps30_stop_measurement(dev_handle);
    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Failed to stop measurement: %s", esp_err_to_name(ret));

    char serialNumber[32];
    ret = sps30_read_serial_number(dev_handle,serialNumber);
    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Failed to read SPS30 SN: %s", esp_err_to_name(ret));
    printf("Serial Number: %s \n",serialNumber);
    
    uint8_t FW_version_minor, FW_version_major;
    ret = sps30_read_sw_version(dev_handle, &FW_version_major ,&FW_version_minor);
    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Failed to read version: %s", esp_err_to_name(ret));
    printf("Software version: %d.%d \n",FW_version_major,FW_version_minor);
    
    char product_type[9];
    product_type[8] = '\0';
    ret = sps30_read_prod_type(dev_handle, product_type);
    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Failed to read product type: %s", esp_err_to_name(ret));
    printf("Product type: %s \n",product_type);

    uint32_t interval;
    sps30_read_auto_cleaning_interval(dev_handle,&interval);
    printf("Automatic cleaning interval: %ld seconds\n\n",interval);

    ret = sps30_start_measurement_float(dev_handle);
    if (ret != ESP_OK)
        ESP_LOGE(TAG, "Failed to start measurement: %s", esp_err_to_name(ret));
        
    bool stat;
    sps30_measurement_float_t measurement;
    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ret = sps30_read_data_ready_flag(dev_handle, &stat);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to read data ready flag: %s", esp_err_to_name(ret));
            continue;
        }

        if(stat)
        {
            uint32_t status;
            sps30_read_status_register(dev_handle,&status);
            sps30_clear_status_register(dev_handle);
            status &= 0b00000000001000000000000000110000;
            if(status != 0)
                printf("Warning: Device error.\n");
            else
                printf("The device is working properly.\n");

            ret = sps30_read_measured_values_float(dev_handle, &measurement);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to read measured values: %s", esp_err_to_name(ret));
                continue;
            }
            printf("Mass concentration  PM1: %f, PM2.5: %f, PM4: %f, PM10: %f\n",
                measurement.MC1p0,
                measurement.MC2p5,
                measurement.MC4p0,
                measurement.MC10p0);
            printf("Particle count      PM1: %f, PM2.5: %f, PM4: %f, PM10: %f\n",
                measurement.NC1p0, 
                measurement.NC2p5,
                measurement.NC4p0,
                measurement.NC10p0);
            printf("Typical particle size: %f\n\n",measurement.TypicalParticleSize);
        }
    }
}
