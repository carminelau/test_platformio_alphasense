#pragma once
// ============================================================
// config.h  –  Alphasense B43F via ADS1115 (I2C) configuration
// ============================================================
// Edit this file to match your actual hardware wiring.
// All values marked "!! SOSTITUIRE !!" MUST be verified
// against your custom board schematic before flashing.
//
// I2C pin defaults match the FirmwareSensy sensy_2024 V3/V4
// ESP32-S3 build_flags (-DSDA_PIN=8 -DSCL_PIN=9).
// Override via build_flags in platformio.ini if using a
// different board variant (e.g. V1: SDA=1, SCL=2).
// ============================================================

// ------------------------------------------------------------
// I2C bus pins
// From FirmwareSensy platformio.ini:
//   sensy_2024_V1_green : SDA=1,  SCL=2
//   sensy_2024_V2_ENEA  : SDA=8,  SCL=9
//   sensy_2024_V3_red   : SDA=8,  SCL=9  ← default
//   sensy_2024_V4_green : SDA=8,  SCL=9
// !! SOSTITUIRE se la tua board usa pin diversi !!
// ------------------------------------------------------------
#ifndef SDA_PIN
#define SDA_PIN  8
#endif
#ifndef SCL_PIN
#define SCL_PIN  9
#endif

// ------------------------------------------------------------
// ADS1115 I2C address
// ADDR pin → GND  : 0x48  (default)
// ADDR pin → VDD  : 0x49
// ADDR pin → SDA  : 0x4A
// ADDR pin → SCL  : 0x4B
// ------------------------------------------------------------
#define ADS1115_I2C_ADDRESS   0x48

// ------------------------------------------------------------
// ADS1115 channel mapping (validate against custom board PCB)
// WE (Working Electrode output)    → AIN0
// AE (Auxiliary Electrode output)  → AIN1
// !! SOSTITUIRE se la board mappa diversamente !!
// ------------------------------------------------------------
#define ADS_CH_WE   0   // AIN0 = WE
#define ADS_CH_AE   1   // AIN1 = AE

// ------------------------------------------------------------
// ADS1115 PGA gain
// GAIN_TWOTHIRDS → ±6.144 V  (0.1875 mV/bit)
// GAIN_ONE       → ±4.096 V  (0.125  mV/bit)
// GAIN_TWO       → ±2.048 V  (0.0625 mV/bit)  ← recommended
// GAIN_FOUR      → ±1.024 V  (0.03125 mV/bit)
// GAIN_EIGHT     → ±0.512 V
// GAIN_SIXTEEN   → ±0.256 V
//
// Alphasense ISB Rev5 outputs are nominally 0–~2 V @ 3.3 V.
// TODO: verify actual output range with oscilloscope before
//       changing to a tighter gain setting.
// ------------------------------------------------------------
#define ADS1115_GAIN   GAIN_TWO   // ±2.048 V

// ------------------------------------------------------------
// ADS1115 data rate (samples per second)
// Options: RATE_ADS1115_8SPS  16  32  64  128  250  475  860
// ------------------------------------------------------------
#define ADS1115_SPS    RATE_ADS1115_128SPS

// ------------------------------------------------------------
// Acquisition mode
// 0 = single-ended A0 and A1, then compute WE-AE in software
// 1 = hardware differential AIN0-AIN1 (single read)
// Start with 0 to diagnose each channel individually.
// ------------------------------------------------------------
#define ADS1115_DIFFERENTIAL_MODE   0

// ------------------------------------------------------------
// Calibration offset and gain (per channel, in Volts)
// voltage_cal = (voltage_raw_V - OFFSET_V) * GAIN_CAL
// Measure with a precision reference and update these values.
// !! SOSTITUIRE CON VALORI CALIBRATI !!
// ------------------------------------------------------------
#define WE_OFFSET_V    0.0f
#define WE_GAIN_CAL    1.0f
#define AE_OFFSET_V    0.0f
#define AE_GAIN_CAL    1.0f

// ------------------------------------------------------------
// EMA (Exponential Moving Average) filter
// alpha = 1.0 → no filtering (raw values)
// alpha = 0.1 → strong smoothing (10-sample effective window)
// Can be overridden at compile time via build_flags.
// ------------------------------------------------------------
#ifndef EMA_ALPHA
#define EMA_ALPHA      0.1f
#endif

// ------------------------------------------------------------
// Sampling interval (ms between acquisitions)
// Can be overridden at compile time via build_flags.
// ------------------------------------------------------------
#ifndef SAMPLE_INTERVAL_MS
#define SAMPLE_INTERVAL_MS   1000U
#endif

// ------------------------------------------------------------
// I2C error recovery
// ------------------------------------------------------------
#define I2C_RETRY_DELAY_MS   500U
#define I2C_MAX_RETRIES      5U

// ------------------------------------------------------------
// Serial baud rate
// ------------------------------------------------------------
#define SERIAL_BAUD   115200
