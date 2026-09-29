#ifndef MY_BME680_I2C_H
#define MY_BME680_I2C_H

#include "i2c.h"
#include "timer.h"
#include "uart.h"
#include <stdint.h>

/* Dia chi I2C 7-bit (0x77 neu SDO noi VDD, 0x76 neu SDO noi GND) */
#define BME680_I2C_ADDR         0x77

/* ==================== ĐỊA CHỈ THANH GHI ==================== */
#define CTRL_MEAS               0x74
#define CTRL_RESET              0xE0
#define CTRL_ID                 0xD0
#define CTRL_HUM_REG            0x72
#define CONFIG_REG              0x75
#define RST_STATE               0x61
#define BME680_SOFT_RESET_MASK  0xB6

/* Calibration: Nhiet do */
#define PAR_T1                  0xE9 
#define PAR_T2                  0x8A
#define PAR_T3                  0x8C

/* Calibration: Ap suat */
#define PAR_P1                  0x8E
#define PAR_P2                  0x90
#define PAR_P3                  0x92
#define PAR_P4                  0x94
#define PAR_P5                  0x96
#define PAR_P6                  0x99
#define PAR_P7                  0x98
#define PAR_P8                  0x9C
#define PAR_P9                  0x9E
#define PAR_P10                 0xA0

/* Calibration: Do am */
#define PAR_H1                  0xE1 // Diem bat dau burst 3 byte (0xE1, 0xE2, 0xE3)
#define PAR_H3                  0xE4
#define PAR_H4                  0xE5
#define PAR_H5                  0xE6
#define PAR_H6                  0xE7
#define PAR_H7                  0xE8

/* Che do va Oversampling */
#define BME680_MODE_FORCED      0x01
#define IIR                     0x04 

#define BME680_REG_PRESS_MSB    0x1F
#define BME680_REG_TEMP_MSB     0x22
#define BME680_REG_HUM_MSB      0x25

#define BME680_OSRS_T_2X        (0x02 << 5)
#define BME680_OSRS_P_4X        (0x03 << 2)
#define BME680_OSRS_H_16X       0x05
#define OSRS_H                  BME680_OSRS_H_16X

/* ==================== CẤU TRÚC DỮ LIỆU ==================== */
typedef struct
{
    /* Nhiet do */
    uint16_t par_t1;
    int16_t  par_t2;
    int8_t   par_t3;

    /* Ap suat */
    uint16_t par_p1;   
    int16_t  par_p2;   
    int8_t   par_p3;   
    int16_t  par_p4;
    int16_t  par_p5;
    int8_t   par_p7;
    int8_t   par_p6;
    int16_t  par_p8;
    int16_t  par_p9;
    uint8_t  par_p10;

    /* Do am */
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t   par_h3;
    int8_t   par_h4;
    int8_t   par_h5;
    uint8_t  par_h6;
    int8_t   par_h7;
} my_bme680_calib_t;

typedef struct {
    int32_t  temperature; /* 0.01 degC */
    int32_t  pressure;    /* Pa */
    uint32_t humidity;    /* 0.001 %RH */
} bme680_data_t;

extern my_bme680_calib_t my_bme680_i2c_calib;
extern int32_t t_fine_i2c;

/* ==================== FUNCTION PROTOTYPES ==================== */
void BME680_Write_Reg(uint8_t reg_addr, uint8_t data);
void BME680_Read_Regs(uint8_t start_reg, int length, uint8_t* data_buffer);

uint8_t My_BME680_I2C_Init(void);
uint8_t My_BME680_I2C_Read_All(bme680_data_t *data);

int32_t  bme680_i2c_compensate_temp(uint32_t temp_adc);
int32_t  bme680_i2c_compensate_pressure(uint32_t press_adc);
uint32_t bme680_i2c_compensate_humidity(uint16_t hum_adc, int32_t temp_comp);

void Print_Temperature_I2C(int32_t temp_val);
void Print_Pressure_I2C(int32_t press_val);
void Print_Humidity_I2C(uint32_t hum_val);

#endif /* MY_BME680_I2C_H */