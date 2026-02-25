#pragma once
// ============================================================
// alphasense_b43f.h  –  Alphasense B4 Series (B43F) helpers
// ============================================================
// This header provides:
//   • ChannelData struct      : per-channel measurement result
//   • ADC initialisation
//   • Oversampled ADC read
//   • Moving-average filter
//   • Voltage conversion with cal offset/gain
//   • convertToConcentration() hook (TODO – requires cal data)
//
// All configuration is pulled from config.h.
// ============================================================

#include <Arduino.h>
#include "config.h"

// ------------------------------------------------------------
// Per-channel measurement result
// ------------------------------------------------------------
struct ChannelData {
    uint8_t  channel;       // channel index (0-based)
    int      adcRaw;        // latest oversampled raw ADC count
    float    voltageMV;     // calibrated voltage in mV
    float    movingAvgMV;   // moving-average filtered voltage (mV)
};

// ------------------------------------------------------------
// Moving-average state (one ring buffer per channel)
// ------------------------------------------------------------
static float    _mavgBuf[NUM_ADC_CHANNELS][MOVING_AVG_SIZE];
static uint8_t  _mavgIdx[NUM_ADC_CHANNELS];
static bool     _mavgFull[NUM_ADC_CHANNELS];

// ------------------------------------------------------------
// alphasenseInit()
// Call once in setup(). Configures ADC resolution and
// attenuation for every channel defined in ADC_PINS[].
// ------------------------------------------------------------
inline void alphasenseInit() {
    analogReadResolution(ADC_RESOLUTION_BITS);

    for (uint8_t ch = 0; ch < NUM_ADC_CHANNELS; ch++) {
        analogSetPinAttenuation(ADC_PINS[ch], ADC_ATTEN);
        pinMode(ADC_PINS[ch], INPUT);

        // Initialise moving-average ring buffer
        _mavgIdx[ch]  = 0;
        _mavgFull[ch] = false;
        for (uint8_t i = 0; i < MOVING_AVG_SIZE; i++) {
            _mavgBuf[ch][i] = 0.0f;
        }
    }
}

// ------------------------------------------------------------
// readOversampledRaw()
// Returns the arithmetic mean of OVERSAMPLING_COUNT reads on
// the requested channel pin (reduces random ADC noise).
// ------------------------------------------------------------
inline int readOversampledRaw(uint8_t ch) {
    long sum = 0;
    for (uint8_t i = 0; i < OVERSAMPLING_COUNT; i++) {
        sum += analogRead(ADC_PINS[ch]);
    }
    return (int)(sum / OVERSAMPLING_COUNT);
}

// ------------------------------------------------------------
// rawToVoltageMV()
// Converts a raw ADC count to millivolts, then applies the
// per-channel calibration offset and gain from config.h.
// ------------------------------------------------------------
inline float rawToVoltageMV(uint8_t ch, int raw) {
    float vRaw = (float)raw * ADC_MV_PER_COUNT;
    return (vRaw - CAL_OFFSET_MV[ch]) * CAL_GAIN[ch];
}

// ------------------------------------------------------------
// updateMovingAverage()
// Pushes a new voltage sample into the ring buffer and returns
// the current windowed mean.
// ------------------------------------------------------------
inline float updateMovingAverage(uint8_t ch, float newValMV) {
    _mavgBuf[ch][_mavgIdx[ch]] = newValMV;
    _mavgIdx[ch] = (_mavgIdx[ch] + 1) % MOVING_AVG_SIZE;
    if (_mavgIdx[ch] == 0) _mavgFull[ch] = true;

    uint8_t n = _mavgFull[ch] ? MOVING_AVG_SIZE : _mavgIdx[ch];
    if (n == 0) return newValMV;

    float sum = 0.0f;
    for (uint8_t i = 0; i < n; i++) sum += _mavgBuf[ch][i];
    return sum / (float)n;
}

// ------------------------------------------------------------
// readChannel()
// High-level function: oversample → convert → moving average.
// Fills a ChannelData struct and returns it.
// ------------------------------------------------------------
inline ChannelData readChannel(uint8_t ch) {
    ChannelData d;
    d.channel      = ch;
    d.adcRaw       = readOversampledRaw(ch);
    d.voltageMV    = rawToVoltageMV(ch, d.adcRaw);
    d.movingAvgMV  = updateMovingAverage(ch, d.voltageMV);
    return d;
}

// ------------------------------------------------------------
// convertToConcentration()
// TODO: implement once Alphasense sensitivity (nA/ppb) and the
//       full analogue signal chain (TIA gain, Vref, offset)
//       have been characterised / calibrated for your specific
//       B43F unit.
//
// Inputs:
//   voltageMV  – calibrated output voltage in mV
//   channel    – channel index (in case sensitivities differ)
//
// Returns: concentration in ppb (currently always 0.0)
// ------------------------------------------------------------
inline float convertToConcentration(float voltageMV, uint8_t channel) {
    (void)voltageMV;
    (void)channel;
    // TODO:
    //   1. Subtract baseline / zero-current voltage
    //   2. Divide by TIA gain (Ω) to recover current (A)
    //   3. Divide by sensitivity (A/ppb from datasheet)
    //   4. Apply temperature compensation if required
    return 0.0f;
}
