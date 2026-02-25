#pragma once
// ============================================================
// config.h  –  Alphasense B43F ADC reader configuration
// ============================================================
// Edit this file to match your actual hardware wiring and
// measurement requirements.
// All values that say "SOSTITUIRE CON PIN REALI" MUST be
// replaced with the real GPIO numbers before flashing.
// ============================================================

// ------------------------------------------------------------
// ADC channel count
// Add more entries in ADC_PINS[] to extend to N channels.
// ------------------------------------------------------------
#define NUM_ADC_CHANNELS  2

// ------------------------------------------------------------
// ADC pin assignments
// !! SOSTITUIRE CON PIN REALI !!
// On ESP32-S3 only GPIOs on ADC1 (GPIO1–GPIO10) or
// ADC2 (GPIO11–GPIO20) can be used. ADC2 cannot be used
// while Wi-Fi is active. Prefer ADC1.
// ------------------------------------------------------------
static const int ADC_PINS[NUM_ADC_CHANNELS] = {
    4,   // CH0 – Working electrode output  !! SOSTITUIRE !!
    5    // CH1 – Auxiliary electrode output !! SOSTITUIRE !!
};

// ------------------------------------------------------------
// ADC resolution & attenuation
// Resolution: 9–12 bits (default 12 on ESP32-S3 Arduino)
// Attenuation options (arduino-esp32):
//   ADC_0db   → 0–800 mV  (best accuracy)
//   ADC_2_5db → 0–1100 mV
//   ADC_6db   → 0–1350 mV
//   ADC_11db  → 0–2600 mV  (use if Alphasense output ≤ 2.6 V)
// The Alphasense ISB outputs are typically 0–2 V @ 3.3 V supply.
// Adjust ADC_ATTEN if your Alphasense interface board scales
// differently.
// ------------------------------------------------------------
#define ADC_RESOLUTION_BITS   12
#define ADC_ATTEN             ADC_11db   // covers 0–2600 mV

// ------------------------------------------------------------
// Reference voltage (mV)
// ESP32-S3 has an internal ~1100 mV Vref, but when using
// ADC_11db the effective full-scale is ~2600 mV.
// If you have measured your specific chip's Vref, put it here.
// See esp_adc_cal for runtime calibration.
// ------------------------------------------------------------
#define VREF_MV               2600.0f    // mV at ADC full-scale

// ------------------------------------------------------------
// Conversion to volts
// Voltage (V) = (raw / ADC_FULL_SCALE) * (VREF_MV / 1000)
// ADC_FULL_SCALE = 2^ADC_RESOLUTION_BITS - 1
// ------------------------------------------------------------
#define ADC_FULL_SCALE        ((float)((1 << ADC_RESOLUTION_BITS) - 1))
#define ADC_MV_PER_COUNT      (VREF_MV / ADC_FULL_SCALE)

// ------------------------------------------------------------
// Calibration offset and gain (per channel)
// voltage_cal = (voltage_raw - OFFSET_MV[ch]) * GAIN[ch]
// Start with offset=0, gain=1 and refine with a calibrated
// voltage reference after full system characterisation.
// !! SOSTITUIRE CON VALORI CALIBRATI !!
// ------------------------------------------------------------
static const float CAL_OFFSET_MV[NUM_ADC_CHANNELS] = {0.0f, 0.0f};
static const float CAL_GAIN[NUM_ADC_CHANNELS]       = {1.0f, 1.0f};

// ------------------------------------------------------------
// Sampling timing
// SAMPLE_INTERVAL_MS : period between acquisition bursts (ms)
// Can also be overridden at compile time via build_flags.
// ------------------------------------------------------------
#ifndef SAMPLE_INTERVAL_MS
#define SAMPLE_INTERVAL_MS    500U   // ms
#endif

// ------------------------------------------------------------
// Oversampling
// Each reported sample is the mean of OVERSAMPLING_COUNT
// consecutive ADC reads (hardware noise averaging).
// Effective ENOB ≈ ADC_RESOLUTION_BITS + log2(N)/2
// ------------------------------------------------------------
#ifndef OVERSAMPLING_COUNT
#define OVERSAMPLING_COUNT    16U
#endif

// ------------------------------------------------------------
// Moving average window size (software low-pass filter)
// Larger = smoother output, more latency.
// ------------------------------------------------------------
#ifndef MOVING_AVG_SIZE
#define MOVING_AVG_SIZE       8U
#endif

// ------------------------------------------------------------
// Serial baud rate
// ------------------------------------------------------------
#define SERIAL_BAUD           115200
