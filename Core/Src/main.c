#include "stm32f407xx.h"
#include "uart.h"
#include "timer.h"
#include "my_bme680_i2c.h"

int main(void)
{
    UART1_Init();
    Timer2_Init();

    Send_String("\r\n========================================\r\n");
    Send_String("Dang khoi tao BME680 (I2C Mode)...\r\n");

    if (!My_BME680_I2C_Init())
    {
        Send_String("-> Init BME680 that bai!\r\n");
        while (1) 
        {
        }
    }
    Send_String("-> Init BME680 thanh cong!\r\n");
    Send_String("========================================\r\n\r\n");

    bme680_data_t Bme_Doc;

    while (1)
    {
        My_BME680_I2C_Read_All(&Bme_Doc);
        Print_Temperature_I2C(Bme_Doc.temperature);
        Print_Pressure_I2C(Bme_Doc.pressure);
        Print_Humidity_I2C(Bme_Doc.humidity);
        Send_String("-----------------------------\r\n");
        Delay_ms_TIM2(1000);
    }
}