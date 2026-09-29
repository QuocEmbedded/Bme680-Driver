#include "my_bme680_i2c.h"

my_bme680_calib_t my_bme680_i2c_calib;
int32_t t_fine_i2c;

void BME680_Write_Reg(uint8_t reg_addr, uint8_t data)
{
    I2C1_Write(BME680_I2C_ADDR, reg_addr, data);
}

void BME680_Read_Regs(uint8_t start_reg, int length, uint8_t* data_buffer)
{
    I2C1_Burst_Read(BME680_I2C_ADDR, start_reg, length, data_buffer);
}

static void My_BME680_I2C_Calib(void)
{
    uint8_t rx_buffer_calib[3];

    /* ==================== NHIỆT ĐỘ ==================== */
    BME680_Read_Regs(PAR_T1, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_t1 = (uint16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_T2, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_t2 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_T3, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_t3 = (int8_t)rx_buffer_calib[0];

    /* ==================== ÁP SUẤT ==================== */
    BME680_Read_Regs(PAR_P1, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p1 = (uint16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P2, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p2 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P3, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_p3 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_P4, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p4 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P5, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p5 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P6, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_p6 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_P7, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_p7 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_P8, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p8 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P9, 2, rx_buffer_calib);
    my_bme680_i2c_calib.par_p9 = (int16_t)(((uint16_t)rx_buffer_calib[1] << 8) | rx_buffer_calib[0]);

    BME680_Read_Regs(PAR_P10, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_p10 = (uint8_t)rx_buffer_calib[0];

    /* ==================== ĐỘ ẨM ==================== */
    BME680_Read_Regs(PAR_H1, 3, rx_buffer_calib);
    my_bme680_i2c_calib.par_h1 = (uint16_t)(((uint16_t)rx_buffer_calib[2] << 4) | (rx_buffer_calib[1] & 0x0F));
    my_bme680_i2c_calib.par_h2 = (uint16_t)(((uint16_t)rx_buffer_calib[0] << 4) | ((rx_buffer_calib[1] & 0xF0) >> 4));

    BME680_Read_Regs(PAR_H3, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_h3 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_H4, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_h4 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_H5, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_h5 = (int8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_H6, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_h6 = (uint8_t)rx_buffer_calib[0];

    BME680_Read_Regs(PAR_H7, 1, rx_buffer_calib);
    my_bme680_i2c_calib.par_h7 = (int8_t)rx_buffer_calib[0];
}

uint8_t My_BME680_I2C_Init(void)
{
    uint8_t ID_chip = 0;

    I2C1_Init();
    Delay_ms_TIM2(200);

    /* Soft reset chip */
    BME680_Write_Reg(CTRL_RESET, BME680_SOFT_RESET_MASK);
    Delay_ms_TIM2(50);

    /* Doc va kiem tra ID */
    BME680_Read_Regs(CTRL_ID, 1, &ID_chip);
    if (ID_chip != RST_STATE)
    { 
        return 0; 
    }

    /* Doc toan bo bang he so calib */
    My_BME680_I2C_Calib();

    /* Cau hinh bo loc va oversampling san sang */
    BME680_Write_Reg(CTRL_HUM_REG, OSRS_H);
    BME680_Write_Reg(CONFIG_REG, IIR);

    return 1;
}


int32_t bme680_i2c_compensate_temp(uint32_t temp_adc) 
{
    int32_t var1, var2, var3;
    int32_t temp_comp;

    var1 = ((int32_t)temp_adc >> 3) - ((int32_t)my_bme680_i2c_calib.par_t1 << 1);
    var2 = (var1 * (int32_t)my_bme680_i2c_calib.par_t2) >> 11;
    var3 = (((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)my_bme680_i2c_calib.par_t3 << 4)) >> 14);
    
    t_fine_i2c = var2 + var3;
    temp_comp = ((t_fine_i2c * 5) + 128) >> 8;

    return temp_comp; 
}

int32_t bme680_i2c_compensate_pressure(uint32_t press_adc)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t press_comp;

    var1 = ((int32_t)t_fine_i2c >> 1) - 64000;
    var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)my_bme680_i2c_calib.par_p6) >> 2;
    var2 = var2 + ((var1 * (int32_t)my_bme680_i2c_calib.par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t)my_bme680_i2c_calib.par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)my_bme680_i2c_calib.par_p3 << 5)) >> 3) 
           + (((int32_t)my_bme680_i2c_calib.par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)my_bme680_i2c_calib.par_p1) >> 15;

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

    var1 = ((int32_t)my_bme680_i2c_calib.par_p9 * (int32_t)(((press_comp >> 3) * (press_comp >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(press_comp >> 2) * (int32_t)my_bme680_i2c_calib.par_p8) >> 13;
    var3 = ((int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) * (int32_t)(press_comp >> 8) 
            * (int32_t)my_bme680_i2c_calib.par_p10) >> 17;

    press_comp = (int32_t)(press_comp) + ((var1 + var2 + var3 + ((int32_t)my_bme680_i2c_calib.par_p7 << 7)) >> 4);

    return press_comp;
}

uint32_t bme680_i2c_compensate_humidity(uint16_t hum_adc, int32_t temp_comp)
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

    var1 = (int32_t)hum_adc - (int32_t)((int32_t)my_bme680_i2c_calib.par_h1 << 4) -
           (((temp_scaled * (int32_t)my_bme680_i2c_calib.par_h3) / ((int32_t)100)) >> 1);

    var2 = ((int32_t)my_bme680_i2c_calib.par_h2 *
            (((temp_scaled * (int32_t)my_bme680_i2c_calib.par_h4) / ((int32_t)100)) +
             (((temp_scaled * ((temp_scaled * (int32_t)my_bme680_i2c_calib.par_h5) / ((int32_t)100))) >> 6) / ((int32_t)100)) +
             ((int32_t)(1 << 14)))) >> 10;

    var3 = var1 * var2;

    var4 = (((int32_t)my_bme680_i2c_calib.par_h6 << 7) +
            ((temp_scaled * (int32_t)my_bme680_i2c_calib.par_h7) / ((int32_t)100))) >> 4;

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

    return (uint32_t)hum_comp;
}

uint8_t My_BME680_I2C_Read_All(bme680_data_t *data)
{
    uint8_t buff[8];
    uint32_t press_raw;
    uint32_t temp_raw;
    uint16_t hum_raw;

    /* 1. Ghi ctrl_hum */
    BME680_Write_Reg(CTRL_HUM_REG, OSRS_H);

    /* 2. Ghi ctrl_meas de chot cau hinh va kich hoat Forced mode */
    uint8_t ctrl_meas = BME680_OSRS_T_2X | BME680_OSRS_P_4X | BME680_MODE_FORCED;
    BME680_Write_Reg(CTRL_MEAS, ctrl_meas);

    /* 3. Cho lay mau ADC xong */
    Delay_ms_TIM2(20);

    /* 4. Burst Read 8 byte tu 0x1F tren I2C */
    BME680_Read_Regs(BME680_REG_PRESS_MSB, 8, buff);

    /* 5. Giai ma ADC raw */
    press_raw = ((uint32_t)buff[0] << 12) | ((uint32_t)buff[1] << 4) | ((uint32_t)buff[2] >> 4);
    temp_raw  = ((uint32_t)buff[3] << 12) | ((uint32_t)buff[4] << 4) | ((uint32_t)buff[5] >> 4);
    hum_raw   = ((uint16_t)buff[6] << 8)  | ((uint16_t)buff[7]);

    /* 6. Tinh toan bu tru */
    data->temperature = bme680_i2c_compensate_temp(temp_raw);
    data->pressure    = bme680_i2c_compensate_pressure(press_raw);
    data->humidity    = bme680_i2c_compensate_humidity(hum_raw, data->temperature);

    return 1;
}

void Print_Temperature_I2C(int32_t temp_val)
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
    if (fractional_part < 10) Send_String("0");
    Int_To_String(fractional_part, num_buf);
    Send_String(num_buf);
    Send_String(" C\r\n");
}

void Print_Pressure_I2C(int32_t press_val)
{
    char num_buf[16];
    int32_t hpa_int = press_val / 100;
    int32_t hpa_frac = press_val % 100;

    Send_String("Ap suat: ");
    Int_To_String(hpa_int, num_buf);
    Send_String(num_buf);
    Send_String(".");
    if (hpa_frac < 10) Send_String("0");
    Int_To_String(hpa_frac, num_buf);
    Send_String(num_buf);
    Send_String(" hPa\r\n");
}

void Print_Humidity_I2C(uint32_t hum_val)
{
    char num_buf[16];
    int32_t hum_int = hum_val / 1000;
    int32_t hum_frac = (hum_val % 1000) / 10;

    Send_String("Do am: ");
    Int_To_String(hum_int, num_buf);
    Send_String(num_buf);
    Send_String(".");
    if (hum_frac < 10) Send_String("0");
    Int_To_String(hum_frac, num_buf);
    Send_String(num_buf);
    Send_String(" %RH\r\n");
}