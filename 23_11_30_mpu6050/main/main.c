#include <stdio.h>
#include "unity.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "mpu6050.h"
#include "esp_system.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO 22      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */


static mpu6050_handle_t mpu6050 = NULL;

typedef struct {
    uint8_t mpu6050_deviceid;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;
    complimentary_angle_t angle;
}mpu6050_out;

/**
 * @brief i2c master initialization
 */
static void i2c_bus_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C config returned error");

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C install returned error");
}

/**
 * @brief i2c master initialization
 */
static void i2c_sensor_mpu6050_init(void)
{
    esp_err_t ret;

    i2c_bus_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    TEST_ASSERT_NOT_NULL_MESSAGE(mpu6050, "MPU6050 create returned NULL");
    
    ret = mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    ESP_LOGI("assert nie dziala", "przed");
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ESP_LOGI("assert nie dziala", "po");

    ret = mpu6050_wake_up(mpu6050);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
}

void read_mpu(mpu6050_out *odczyt)
{
    

    mpu6050_get_deviceid(mpu6050, &odczyt->mpu6050_deviceid);
    mpu6050_get_acce(mpu6050, &odczyt->acce);
    mpu6050_get_gyro(mpu6050, &odczyt->gyro);
    mpu6050_get_temp(mpu6050, &odczyt->temp);
    
    //increase precision
    odczyt->acce.acce_x -=1.04;
    odczyt->acce.acce_y +=0.05;
    odczyt->acce.acce_z += 0.12;
    odczyt->gyro.gyro_x +=9.8;
    odczyt->gyro.gyro_y += 1.9;
    odczyt->gyro.gyro_z += 0.7;
    odczyt->temp.temp -= 5;


    mpu6050_complimentory_filter(mpu6050, &odczyt->acce, &odczyt->gyro, &odczyt->angle);
    // mpu6050_delete(mpu6050);
    // i2c_driver_delete(I2C_MASTER_NUM);
}
void app_main(void)
{
    i2c_sensor_mpu6050_init();
    mpu6050_out odczyt;
    while (1)
    {
        vTaskDelay(10);
        read_mpu(&odczyt);
        printf("acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", odczyt.acce.acce_x, odczyt.acce.acce_y, odczyt.acce.acce_z);
        printf("gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f\n", odczyt.gyro.gyro_x, odczyt.gyro.gyro_y, odczyt.gyro.gyro_z);
        printf("roll: %.2f, pitch: %.2f\n",odczyt.angle.roll, odczyt.angle.pitch);
        printf("temp: %.2f\n\n",odczyt.temp.temp);
    }
    
}
