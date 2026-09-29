#include "my_bme_680_spi.h"

bme680_calib_t bme_calib;
int32_t t_fine; /* Chuan Bosch: int32_t co dau */

void My_BME680_SPI_Transmiter(uint8_t reg, uint8_t value)
{
    uint8_t tx_buffer[2];
    tx_buffer[0] = reg & BME680_SPI_WRITE_MASK;
    tx_buffer[1] = value;

    cs_enable();
    SPI_Transmiter(tx_buffer, 2);
    cs_disable();
}

void My_BME680_SPI_Receiver(uint8_t reg, uint8_t *rx_data, uint32_t len)
{
    reg = reg | BME680_SPI_READ_MASK;
    cs_enable();
    SPI_Transmiter(&reg, 1);
    SPI_Receiver(rx_data, len);
    cs_disable();
}

void My_BME680_SPI_All_Calib(void)
{
    uint8_t rx_buffer_calib[3];

    /* ==================== NHIET DO ==================== */
    My_BME680_SPI_Receiver(PAR_T1, rx_buffer_calib, 2);
    bme_calib.par_t1 = (uint16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_T2, rx_buffer_calib, 2);
    bme_calib.par_t2 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_T3, rx_buffer_calib, 1);
    bme_calib.par_t3 = (int8_t)rx_buffer_calib[0];

    /* ==================== AP SUAT ==================== */
    My_BME680_SPI_Receiver(PAR_P1, rx_buffer_calib, 2);
    bme_calib.par_p1 = (uint16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P2, rx_buffer_calib, 2);
    bme_calib.par_p2 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P3, rx_buffer_calib, 1);
    bme_calib.par_p3 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_P4, rx_buffer_calib, 2);
    bme_calib.par_p4 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P5, rx_buffer_calib, 2);
    bme_calib.par_p5 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P6, rx_buffer_calib, 1);
    bme_calib.par_p6 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_P7, rx_buffer_calib, 1);
    bme_calib.par_p7 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_P8, rx_buffer_calib, 2);
    bme_calib.par_p8 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P9, rx_buffer_calib, 2);
    bme_calib.par_p9 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    My_BME680_SPI_Receiver(PAR_P10, rx_buffer_calib, 1);
    bme_calib.par_p10 = (uint8_t)rx_buffer_calib[0];

    /* ==================== DO AM ==================== */
    /* Doc 3 byte tu PAR_H1 (0xE1: par_h2 MSB, 0xE2: LSB chung, 0xE3: par_h1 MSB) */
    My_BME680_SPI_Receiver(PAR_H1, rx_buffer_calib, 3);
    bme_calib.par_h1 = (uint16_t)(((uint16_t)rx_buffer_calib[2] << 4) | (rx_buffer_calib[1] & 0x0F));
    bme_calib.par_h2 = (uint16_t)(((uint16_t)rx_buffer_calib[0] << 4) | ((rx_buffer_calib[1] & 0xF0) >> 4));

    My_BME680_SPI_Receiver(PAR_H3, rx_buffer_calib, 1);
    bme_calib.par_h3 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_H4, rx_buffer_calib, 1);
    bme_calib.par_h4 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_H5, rx_buffer_calib, 1);
    bme_calib.par_h5 = (int8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_H6, rx_buffer_calib, 1);
    bme_calib.par_h6 = (uint8_t)rx_buffer_calib[0];

    My_BME680_SPI_Receiver(PAR_H7, rx_buffer_calib, 1);
    bme_calib.par_h7 = (int8_t)rx_buffer_calib[0];
}

uint8_t My_BME68_SPI_Init(void)
{
    uint8_t ID_Chip = 0;
    Gpio_Spi_Init();
    Spi_Config();
    cs_disable(); 
    Delay_ms_TIM2(100);

    /* Chuyen ve Page 0 truoc khi soft-reset de tranh ket trang thai */
    My_BME680_SPI_Transmiter(STATUS_REG, 0x00);
    Delay_ms_TIM2(10);

    My_BME680_SPI_Transmiter(CTRL_RESET, BME680_SOFT_RESET_MASK);
    Delay_ms_TIM2(50);

    My_BME680_SPI_Receiver(CTRL_ID, &ID_Chip, 1U);
    if (ID_Chip != RST_STATE)
    { 
        return 0; 
    }

    /* Doc toan bo he so calib tren Page 0 */
    My_BME680_SPI_All_Calib();

    /* Chuyen sang Page 1 de cau hinh va doc du lieu */
    My_BME680_SPI_Transmiter(STATUS_REG, PAGE_SELECT_1);
    
    My_BME680_SPI_Transmiter(CTRL_HUM_REG, OSRS_H);
    My_BME680_SPI_Transmiter(CONFIG_REG, IIR);
    
    return 1;
}

int32_t bme680_compensate_temp(uint32_t temp_adc) 
{
    int32_t var1, var2, var3;
    int32_t temp_comp;

    var1 = ((int32_t)temp_adc >> 3) - ((int32_t)bme_calib.par_t1 << 1);
    var2 = (var1 * (int32_t)bme_calib.par_t2) >> 11;
    var3 = (((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)bme_calib.par_t3 << 4)) >> 14);
    
    t_fine = var2 + var3;     
    temp_comp = ((t_fine * 5) + 128) >> 8;

    return temp_comp; 
}

int32_t bme680_compensate_pressure(uint32_t press_adc)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t press_comp;

    var1 = ((int32_t)t_fine >> 1) - 64000;
    var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)bme_calib.par_p6) >> 2;
    var2 = var2 + ((var1 * (int32_t)bme_calib.par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t)bme_calib.par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)bme_calib.par_p3 << 5)) >> 3) 
           + (((int32_t)bme_calib.par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)bme_calib.par_p1) >> 15;

    if (var1 == 0)
    {
        return 0;
    }

    press_comp = 1048576 - press_adc;
    press_comp = (uint32_t)((press_comp - (var2 >> 12)) * ((uint32_t)3125));

    if (press_comp >= (1 << 30))
    {
        press_comp = ((press_comp / (uint32_t)var1) << 1);
    }
    else
    {
        press_comp = ((press_comp << 1) / (uint32_t)var1);
    }

    var1 = ((int32_t)bme_calib.par_p9 * (int32_t)(((press_comp >> 3) * (press_comp >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(press_comp >> 2) * (int32_t)bme_calib.par_p8) >> 13;
    var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) 
            * (int32_t)bme_calib.par_p10) >> 17;

    press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)bme_calib.par_p7 << 7)) >> 4);

    return press_comp; /* Don vi: Pascal (Pa) */
}

uint32_t bme680_compensate_humidity(uint16_t hum_adc, int32_t temp_comp)
{
    int32_t temp_scaled;
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t hum_comp;

    temp_scaled = (int32_t)temp_comp;

    var1 = (int32_t)hum_adc - (int32_t)((int32_t)bme_calib.par_h1 << 4) -
           (((temp_scaled * (int32_t)bme_calib.par_h3) / ((int32_t)100)) >> 1);

    var2 = ((int32_t)bme_calib.par_h2 *
            (((temp_scaled * (int32_t)bme_calib.par_h4) / ((int32_t)100)) +
             (((temp_scaled * ((temp_scaled * (int32_t)bme_calib.par_h5) / ((int32_t)100))) >> 6) / ((int32_t)100)) +
             ((int32_t)(1 << 14)))) >> 10;

    var3 = var1 * var2;

    var4 = (((int32_t)bme_calib.par_h6 << 7) +
            ((temp_scaled * (int32_t)bme_calib.par_h7) / ((int32_t)100))) >> 4;

    var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;

    var6 = (var4 * var5) >> 1;

    hum_comp = (((var3 + var6) >> 10) * ((int32_t)1000)) >> 12;

    if (hum_comp > 100000)
    {
        hum_comp = 100000;
    }
    else if (hum_comp < 0)
    {
        hum_comp = 0;
    }

    return (uint32_t)hum_comp; /* Don vi: 0.001 %RH */
}

uint8_t My_Bme680_Read_All(bme680_data_t *data)
{
    uint8_t buff[8];
    uint32_t press_raw;
    uint32_t temp_raw;
    uint16_t hum_raw;

    /* 1. Ghi ctrl_hum truoc */
    My_BME680_SPI_Transmiter(CTRL_HUM_REG, BME680_OSRS_H_1X);

    /* 2. Ghi ctrl_meas de chot cau hinh va kich hoat Forced mode */
    uint8_t ctrl_meas = BME680_OSRS_T_2X | BME680_OSRS_P_4X | BME680_MODE_FORCED;
    My_BME680_SPI_Transmiter(CTRL_MEAS, ctrl_meas);

    /* 3. Cho chu ky do hoan tat */
    Delay_ms_TIM2(20);

    /* 4. Doc burst 8 byte bat dau tu thanh ghi 0x1F */
    My_BME680_SPI_Receiver(BME680_REG_PRESS_MSB, buff, 8);

    /* 5. Tach gia tri ADC raw */
    press_raw = ((uint32_t)buff[0] << 12) | ((uint32_t)buff[1] << 4) | ((uint32_t)buff[2] >> 4);
    temp_raw  = ((uint32_t)buff[3] << 12) | ((uint32_t)buff[4] << 4) | ((uint32_t)buff[5] >> 4);
    hum_raw   = ((uint16_t)buff[6] << 8)  | ((uint16_t)buff[7]);

    /* 6. Tinh bu tru theo dung thu tu: Temp -> Press -> Hum */
    data->temperature = bme680_compensate_temp(temp_raw);
    data->pressure    = bme680_compensate_pressure(press_raw);
    data->humidity    = bme680_compensate_humidity(hum_raw, data->temperature);

    return 1;
}

void Print_Temperature(int32_t temp_val)
{
    char num_buf[16];

    if (temp_val < 0)
    {
        Send_String("-");
        temp_val = -temp_val;
    }

    int32_t integer_part = temp_val / 100;
    int32_t fractional_part = temp_val % 100;

    Send_String("Nhiet do: ");
    Int_To_String(integer_part, num_buf);
    Send_String(num_buf);
    Send_String(".");
    if (fractional_part < 10)
    {
        Send_String("0");
    }
    Int_To_String(fractional_part, num_buf);
    Send_String(num_buf);
    Send_String(" C\r\n");
}

void Print_Pressure(int32_t press_val)
{
    char num_buf[16];

    int32_t hpa_int = press_val / 100;
    int32_t hpa_frac = press_val % 100;

    Send_String("Ap suat: ");
    Int_To_String(hpa_int, num_buf);
    Send_String(num_buf);
    Send_String(".");
    if (hpa_frac < 10)
    {
        Send_String("0");
    }
    Int_To_String(hpa_frac, num_buf);
    Send_String(num_buf);
    Send_String(" hPa\r\n");
}

void Print_Humidity(uint32_t hum_val)
{
    char num_buf[16];

    int32_t hum_int = hum_val / 1000;
    int32_t hum_frac = (hum_val % 1000) / 10;

    Send_String("Do am: ");
    Int_To_String(hum_int, num_buf);
    Send_String(num_buf);
    Send_String(".");
    if (hum_frac < 10)
    {
        Send_String("0");
    }
    Int_To_String(hum_frac, num_buf);
    Send_String(num_buf);
    Send_String(" %RH\r\n");
}