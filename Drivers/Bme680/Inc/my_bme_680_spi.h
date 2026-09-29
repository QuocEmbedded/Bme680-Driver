#ifndef MY_BME680_H
#define MY_BME680_H

#include "spi.h"
#include "timer.h"
#include "uart.h"
#include <stdint.h>

/* ==================== ĐỊA CHỈ THANH GHI BME680 ==================== */
#define CTRL_MEAS               0x74 // Thanh ghi dieu khien che do va oversampling T, P
#define CTRL_RESET              0x60 // Thanh ghi reset
#define CTRL_ID                 0x50 // Thanh ghi ID chip (0x61 cho BME680)
#define CTRL_HUM_REG            0x72 // Thanh ghi oversampling do am
#define CONFIG_REG              0x75 // Thanh ghi IIR filter
#define STATUS_REG              0x73 // Thanh ghi trang thai va chon page

#define RST_STATE               0x61 // Chip ID mac dinh
#define PAGE_SELECT_1           0x10 // Chuyen sang SPI Memory Page 1

/* SPI Mask */
#define BME680_SPI_READ_MASK    0x80 // Bit 7 = 1 cho lenh Read
#define BME680_SPI_WRITE_MASK   0x7F // Bit 7 = 0 cho lenh Write
#define BME680_SOFT_RESET_MASK  0xB6 // Gia tri ghi vao CTRL_RESET

/* ==================== HỆ SỐ CALIBRATION ==================== */
/* Nhiet do */
#define PAR_T1                  0xE9 
#define PAR_T2                  0x8A
#define PAR_T3                  0x8C

/* Ap suat */
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

/* Do am */
#define PAR_H1                  0xE1 // Diem bat dau doc cum 3 byte (0xE1, 0xE2, 0xE3)
#define PAR_H3                  0xE4
#define PAR_H4                  0xE5
#define PAR_H5                  0xE6
#define PAR_H6                  0xE7
#define PAR_H7                  0xE8

/* ==================== CẤU HÌNH & CHẾ ĐỘ HOẠT ĐỘNG ==================== */
#define BME680_MODE_FORCED      0x01
#define IIR                     0x04 // IIR filter coefficient = 1
#define OSRS_H                  0x05

/* Thanh ghi du lieu ADC (Page 1) */
#define BME680_REG_PRESS_MSB    0x1F
#define BME680_REG_TEMP_MSB     0x22
#define BME680_REG_HUM_MSB      0x25

/* Oversampling */
#define BME680_OSRS_T_2X        (0x02 << 5)
#define BME680_OSRS_P_4X        (0x03 << 2)
#define BME680_OSRS_H_1X         0x01

/* ==================== CẤU TRÚC DỮ LIỆU ==================== */
typedef struct
{
    /* Calibration nhiet do */
    uint16_t par_t1;
    int16_t  par_t2;
    int8_t   par_t3;

    /* Calibration ap suat */
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

    /* Calibration do am */
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t   par_h3;
    int8_t   par_h4;
    int8_t   par_h5;
    uint8_t  par_h6;
    int8_t   par_h7;
} bme680_calib_t;

typedef struct {
    int32_t  temperature; /* Don vi: 0.01 degC (vd: 3025 = 30.25 C) */
    int32_t  pressure;    /* Don vi: Pascal (Pa) (vd: 101325 Pa) */
    uint32_t humidity;    /* Don vi: 0.001 %RH (vd: 55420 = 55.42 %RH) */
} bme680_data_t;

/* ==================== BIẾN NGOÀI (EXTERN) ==================== */
extern bme680_calib_t bme_calib;
extern int32_t t_fine;

/* ==================== NGUYÊN MẪU HÀM (FUNCTION PROTOTYPES) ==================== */
/* Ham giao tiep SPI co ban */
void My_BME680_SPI_Transmiter(uint8_t reg, uint8_t value);
void My_BME680_SPI_Receiver(uint8_t reg, uint8_t *rx_data, uint32_t len);

/* Khoi tao & doc thong so calib */
uint8_t My_BME68_SPI_Init(void);
void My_BME680_SPI_All_Calib(void);

/* Doc gop tat ca du lieu moi chu ky */
uint8_t My_Bme680_Read_All(bme680_data_t *data);

/* Thuat toan bu tru (Compensation formulas) */
int32_t  bme680_compensate_temp(uint32_t temp_adc);
int32_t  bme680_compensate_pressure(uint32_t press_adc);
uint32_t bme680_compensate_humidity(uint16_t hum_adc, int32_t temp_comp);

/* Ham in gia tri ra UART */
void Print_Temperature(int32_t temp_val);
void Print_Pressure(int32_t press_val);
void Print_Humidity(uint32_t hum_val);

#endif /* MY_BME680_H */